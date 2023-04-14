import {Injectable, OnDestroy} from '@angular/core';
import {BehaviorSubject, debounceTime, Subject, takeUntil, timer} from 'rxjs';
import {EffectModel, IEffectModel} from '../../models/effect.model';
import {StateModel} from '../../models/state.model';
import {CommonService, MessageType} from '../common/common.service';
import {clearBraces, isNullOrUndefined, isNullOrUndefinedOrEmpty} from '../helper';
import {LanguagesService} from '../languages/languages.service';
import {WebsocketService, WS} from '../websocket/websocket.service';
import {ComboBoxItem} from "../../models/combo-box.model";
import {distinctUntilChanged} from "rxjs/operators";

@Injectable({
  providedIn: 'root'
})
export class ManagementService implements OnDestroy {

  private checkInterval = 2000;

  // Строка перевода ID эффекта в символьное представления 0..9,A..Z,a..z - должна соответствовать строке в прошивке устройства
  // Здесь 62 символа. Эффектов сейчас 49. При увеличении количества эффектов строку нужно будет добавить другими символами,
  // синхронизировав с такой же кодировочной строкой в прошивке
  public effects_code = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';

  public effects: Array<IEffectModel> = [];        // Полный список эффeктов, полученный из устройства

  public effects_in_use: Array<number> = [];       // Список индексов эффeктов из массива effects, выбранных для использования
  public effects_not_use: Array<number> = [];      // Список индексов эффeктов из массива effects, НЕ выбранных для использования

  public alarm_sounds: Array<ComboBoxItem> = [];

  public dawn_sounds: Array<ComboBoxItem> = [];

  public notify_sounds: Array<ComboBoxItem> = [];

  public text_lines: Array<string> = [];

  public picture_list: Array<ComboBoxItem> = [];

  public state: StateModel;

  public stateKey$ = new BehaviorSubject<string>('');
  public effects$ = new BehaviorSubject<Array<IEffectModel>>([]);
  public effects_in_use$ = new BehaviorSubject<Array<number>>([]);
  public effects_not_use$ = new BehaviorSubject<Array<number>>([]);
  public alarm_sounds$ = new BehaviorSubject<Array<ComboBoxItem>>([]);
  public dawn_sounds$ = new BehaviorSubject<Array<ComboBoxItem>>([]);
  public notify_sounds$ = new BehaviorSubject<Array<ComboBoxItem>>([]);
  public picture_list$ = new BehaviorSubject<Array<ComboBoxItem>>([]);
  public edit$ = new BehaviorSubject<any>(null);

  public matrixColors: Array<string[]> = [];    // Цвета в массиве рисования

  private destroy$ = new Subject();
  private stateValues: Map<string, boolean> = new Map();
  private favorites: string = "";
  private last_message_time: number = Date.now();

  private loadIndex: number = -1;     // индекс загружаемой строки
  private intervalId: any = undefined;

  constructor(private commonService: CommonService,
              private wsService: WebsocketService,
              private L: LanguagesService) {

    this.state = new StateModel();

    // сообщения о текущем статусе параметров устройства
    this.wsService.on<string>(WS.ON.STATE)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processStateMessage(data);
      });
    // отправка запрошенных данных клиенту, в ответ на полученную команду, например '$6 7
    this.wsService.on<string>(WS.ON.DATA)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.DATA, data);
      });
    // уведомления об ошибке клиенту
    this.wsService.on<string>(WS.ON.ERROR)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.ERROR, data);
      });
    // сообщения о событиях будильника
    this.wsService.on<string>(WS.ON.ALARM)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.ALARM, data);
      });
    // сообщения о наступлении событий авторежимов по времени
    this.wsService.on<string>(WS.ON.AMODE)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.AMODE, data);
      });
    // сообщения о получении актуальной погоды
    this.wsService.on<string>(WS.ON.WEATHER)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.WEATHER, data);
      });
    // сообщения о синхронизации времени
    this.wsService.on<string>(WS.ON.TIME)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.TIME, data);
      });
    // сообщения о событиях SD-карты (загрузка файла эффекта)
    this.wsService.on<string>(WS.ON.SDCARD)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.SDCARD, data);
      });
    // сообщения о событиях бегущей строки - запуск, окончание
    this.wsService.on<string>(WS.ON.TEXT)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.TEXT, data);
      });
    // сообщения о событиях синхронизации E131
    this.wsService.on<string>(WS.ON.E131)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        this.processMessage(WS.ON.E131, data);
      });
    // параметры редактируемого эффекта
    this.wsService.on<string>(WS.ON.EDT)
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: string) => {
        const args = JSON.parse(data);
        this.edit$.next(args);
      });

    this.wsService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в главном экране
          if (this.text_lines.length == 0) {
            this.loadTextLines();
          }
        } else {
          // Если соединение пропало во время загрузки картинки - остановить
          this.stopLoadText();
        }
      });

    timer(this.checkInterval, this.checkInterval).pipe(takeUntil(this.destroy$)).subscribe(() => {
      if (!isNullOrUndefinedOrEmpty(this.stateValues) && Date.now() - this.last_message_time > (this.checkInterval / 1000)) {
        let req = '';
        for (const key of this.stateValues.keys()) {
          if (!this.stateValues.get(key)) {
            req += `|${key}`;
          }
        }
        if (req.length > 0) {
          this.wsService.send(WS.SEND.COMMAND, `$6 7${req}`);
        }
      }
    });
  }

  //* Обработка поступивших от прошивки сообщений в ответ на запрос, на выполнение команды или
  //* поступление инициативных уведомлений от прошивки
  private processStateMessage(data: string) {
    if (!data) return;

    this.last_message_time = Date.now();

    this.wsService.pong();

    console.log('json=\'%s\'', data);

    const obj = JSON.parse(data);
    const keys = Object.keys(obj);

    for (const key of keys) {

      if (isNullOrUndefinedOrEmpty(key)) continue;

      this.stateValues.set(key, true);

      const cmd = key.toUpperCase();
      const args = obj[cmd].trim();

      if (cmd.length === 0) continue;

      // console.log('cmd=\'%s\'; args=\'%s\'', cmd, args);

      switch (cmd) {

        // Язык из прошивки - 'RUS', 'ENG' или что в прошивке еще определено.
        // Встроенные языки - 'RUS', 'ENG' изначально присутствуют в прошивке в сервисе languageService,
        // Дополнительные языки могут быть загружены из assets/langs в json файлах с именем файла, соответствующим запрошенному языку,
        // например 'ger.json', 'fra.json' и т.д.
        case 'LG':
          this.L.loadLanguage(args);
          break;

        // --- Список эффектов, отмеченных как "любимые" --------
        //     где позиция в строке - порядок в списке воспроизведения, символ строки - ID эффекта, приведенный к коду 0..9,A..Z,a..z
        case 'FV': {
          this.favorites = args;
          this.updateUsage(args);
          break;
        }

        // --- Список эффектов в формате строки имен, разделенных запятыми
        case 'LE': {
          let idx = 0;
          let list = args;
          const parts = clearBraces(args).split(',');
          this.effects = [];
          for (const part of parts) {
            this.effects.push(new EffectModel(idx++, part));
          }
          this.effects$.next(this.effects);
          // @ts-ignore
          const fav = this.favorites;
          if (!isNullOrUndefinedOrEmpty(fav)) {
            this.updateUsage(fav);
          } else {
            this.getKeys('FV');
          }
          break;
        }

        case 'S1': {
          let idx = 0;
          let list = args;
          const parts = clearBraces(args).split(',');
          this.alarm_sounds = [];
          for (const part of parts) {
            this.alarm_sounds.push(new ComboBoxItem(part, idx++));
          }
          this.alarm_sounds$.next(this.alarm_sounds);
          this.stateKey$.next(cmd);
          break;
        }

        case 'S2': {
          let idx = 0;
          const parts = clearBraces(args).split(',');
          this.dawn_sounds = [];
          for (const part of parts) {
            this.dawn_sounds.push(new ComboBoxItem(part, idx++));
          }
          this.dawn_sounds$.next(this.dawn_sounds);
          this.stateKey$.next(cmd);
          break;
        }

        case 'S3': {
          let idx = 0;
          const parts = clearBraces(args).split(',');
          this.notify_sounds = [];
          for (const part of parts) {
            this.notify_sounds.push(new ComboBoxItem(part, idx++));
          }
          this.notify_sounds$.next(this.notify_sounds);
          this.stateKey$.next(cmd);
          break;
        }

        case 'TY': {
          // 12:C > text
          let line_idx = -1;
          let line_text = '';
          let idx = args.indexOf(':');
          if (idx > 0) {
            line_idx = Number(args.substring(0, idx));
            idx = args.indexOf('>');
            if (idx > 0 && idx < this.text_lines.length) {
              line_text = args.substring(idx + 1).trim();
              this.text_lines[line_idx] = line_text;
              this.state.text_edit = args;
            }
            this.stateKey$.next(cmd);
            // Если текст получен в ответ на выполняющуюся загрузку строк - запросить следующую строку
            if (this.loadIndex >= 0) {
              this.loadIndex = line_idx + 1;
              if (this.loadIndex >= this.text_lines.length) {
                this.stopLoadText();
              } else {
                this.sendRequestText();
              }
            }
          } else {
            if (this.loadIndex >= 0) {
              this.stopLoadText();
            }
          }
          break;
        }

        case 'FL':
        case 'FL0':
        case 'FL1':{
          let idx = 0;
          const parts = clearBraces(args).split(',');
          this.picture_list = [];
          for (const part of parts) {
            if (!isNullOrUndefinedOrEmpty(part)) {
              this.picture_list.push(new ComboBoxItem(part, idx++));
            }
          }
          this.picture_list = this.sortComboBoxItems(this.picture_list);
          this.picture_list$.next(this.picture_list);
          this.stateKey$.next(cmd);
          break;
        }

        default: {
          this.state.setValueByKey(cmd, args);
          this.stateKey$.next(cmd);
          break;
        }
      }
    }
  }

  private sortComboBoxItems(array: Array<ComboBoxItem>): Array<ComboBoxItem> {
    return  array.sort((a,b) => { return a.displayText > b.displayText ? 1 : (a.displayText === b.displayText ? 0 : -1)});
  }

  private processMessage(topic: string, data: string) {

    this.wsService.pong();

    const args = JSON.parse(data);

    console.log(topic, data, args);

    switch (topic) {
      // уведомления об ошибке клиенту
      // {
      //   "message": "message",    // "unknown page" / "unknown command"
      //   "text": "text"           // текст сообщения
      // }
      case WS.ON.ERROR: {
        this.commonService.timerMessage(MessageType.ERROR, args['text']);
        break;
      }

      // отправка запрошенных данных клиенту, в ответ на полученную команду, например '$6 7
      case WS.ON.DATA: {
        console.log(data);
        break;
      }

      // сообщения о событиях будильника
      // {
      //   "act": "ALARM",         // тип сообщения
      //   "state": "state",       // on/off
      //   "type": "type"          // dawn/alarm; для off - auto/stop
      // }
      case WS.ON.ALARM: {
        break;
      }

      // сообщения о наступлении событий авторежимов по времени
      // {
      //   "act": "AUTO",          // тип сообщения
      //   "mode": N,              // ID эффекта
      //   "mode_name": "name"     // название эффекта
      //   "text": "text"          // Сообщение о действии
      // }
      case WS.ON.AMODE: {
        break;
      }

      // получение статуса запроса данных о погоде
      // {
      //   "act": "WEATHER",       // тип сообщения
      //   "region": N,            // ID региона погоды
      //   "result": "text",       // Результат выполнения операции TIMEOUT/ERROR/OK
      //   "status": "text",       // для ERROR - причина ошибки; для OK - строка текущей погоды
      //   "temp": N,              // для OK - температура (градусы)
      //   "night": "val",         // для OK - true/false - день/ночь
      //   "icon": "code",         // для OK - код иконки погоды
      //   "town": "town",         // для OK - город
      //   "server": "name",       // для OK - погодный сервер - Yandex/OpenWeatherMap
      //   "sky": "color",         // для OK - для Yandex - цвет неба
      //   "code": N,              // для OK - для OpenWeatherMap - Уточненный код погодных условий
      //   "sunrise": "time",      // для OK - время рассвета
      //   "sunset": "time"        // для OK - время заката
      // }
      case WS.ON.WEATHER: {
        break;
      }

      // сообщения о синхронизации времени
      // {
      //   "act": "TIME",          // тип сообщения
      //   "server_name": N,       // сервер впемени - имя
      //   "server_ip": "name"     // сервер времени - IP
      //   "result": "text",       // Сообщение о действии TIMEOUT/OK/REQUEST
      //   "time": N,              // для OK - количество секунд с 1900 года
      //   "s_time2": "time",      // Текущее время (строка)
      // }
      case WS.ON.TIME: {
        break;
      }

      // сообщения о событиях SD-карты (загрузка файла эффекта)
      // {
      //   "act": "SDCARD",        // тип сообщения
      //   "result": "state"       // ERROR/OK
      //   "file": "file",         // Имя файла эффекта
      // }
      case WS.ON.SDCARD: {
        break;
      }

      // сообщения о событиях бегущей строки - запуск, окончание
      // {
      //   "act": "TEXT",          // тип сообщения
      //   "run": "state"          // true/false - отображается/нет строка на матрице
      //   "text": "text",         // для true - текст бегущей строки
      // }
      case WS.ON.TEXT: {
        break;
      }

      // сообщения о событиях синхронизации E131
      // {
      //   "act": "E131",          // тип сообщения
      //   "mode": "mode"          // MASTER/SLAVE
      //   "type": "type",         // PHYSIC/LOGIC/CPMMAND
      //   "group": N              // номер группы
      //   "run": "state"          // true/false - состояние активности
      // }
      case WS.ON.E131: {
        // MASTER: "Запуск вещания E1.31 потока..."  / SLAVE + COMMAND: "Ожидание поступления потока команд E1.31...", SLAVE + LOGIC/PHYSIC: Ожидание поступления потока данных E1.31...
        break;
      }

      default:
        // console.log(topic, data, args);
        break;
    }

  }

  private updateUsage(usage: string) {
    // usage - строка, содержащая список индексов эффектов, выбранных для воспроизведения в порядке их следования
    // индекс эффектов 0..MAX_EFFECT кодируется символами 0..9,A..Z,a..z
    // Позиция в строке - очередность воспроизведения эффектов - с 0 до конца строки.
    // Символ в позиции - кодированный ID эффекта (индекс в массиве this.effects)
    this.effects_in_use = [];
    this.effects_not_use = [];
    if (this.effects.length === 0) return;

    usage = clearBraces(usage);

    // Сформировать массив эффектов, отмеченных к использованию в порядке, определенном в переданной строке-параметре
    for (let idx = 0; idx < usage.length; idx++) {
      const effect_idx = this.effects_code.indexOf(usage[idx]);
      if (effect_idx >= 0) {
        this.effects[effect_idx].order = idx;
        this.effects_in_use.push(effect_idx);
      }
    }

    // Все остальные эффекты, не вошедшие в список используемых добавить в список НЕиспользуемых
    for (let idx = 0; idx < this.effects.length; idx++) {
      if (this.effects_in_use.indexOf(idx) < 0) {
        this.effects[idx].order = -1;
        this.effects_not_use.push(idx);
      }
    }

    // Сообщить об изменении списка эффектов
    this.effects_in_use$.next(this.effects_in_use);
    this.effects_not_use$.next(this.effects_not_use);
  }

  usageSelectAll() {
    // Добавить все карточки в список выбранных
    this.effects_not_use.forEach(idx => {
      this.effects_in_use.push(idx);
    });
    this.effects_not_use = [];
    this.effects_in_use$.next(this.effects_in_use);
    this.effects_not_use$.next(this.effects_not_use);
  }

  usageUnselectAll() {
    // Удалить все карточки в список выбранных
    this.effects_in_use = [];
    this.effects_not_use = [];
    this.effects.forEach(item => {
      this.effects_not_use.push(item.id);
    });
    this.effects_in_use$.next(this.effects_in_use);
    this.effects_not_use$.next(this.effects_not_use);
  }

  saveCardsUsage() {
    let keys = "";
    this.effects_in_use.forEach(idx => {
      keys += this.effects_code[this.effects[idx].id];
    });
    this.wsService.send(WS.SEND.COMMAND, `$6 18|${keys}`);
  }

  public getKeys(keys: string): void {
    const parts = keys.split('|');
    parts.forEach((key) => {
      // @ts-ignore
      if (!isNullOrUndefinedOrEmpty(key)) {
        this.stateValues.set(key, false);
      }
    });

    if (!keys.startsWith('|')) keys = '|' + keys;
    this.wsService.send(WS.SEND.COMMAND, `$6 7${keys}`);
  }

  public createMatrix() {
    let cols = this.matrixColors.length;
    let rows = cols === 0 ? 0 : this.matrixColors[0].length;
    // Если матрица создана и текущий размер совпадает с заапрошенным - пересоздавать не нужно
    if (rows !== this.state.height || cols !== this.state.width) {
      rows = this.state.height;
      cols = this.state.width;
      this.matrixColors = new Array<string[]>();

      for (let y = 0; y < cols; y++) {
        const helperArray: Array<string> = new Array<string>();
        for (let x = 0; x < rows; x++) {
          helperArray.push('#000000')
        }
        this.matrixColors.push(helperArray);
      }
    }
  }

  public sortPictureList() {
    this.picture_list = this.sortComboBoxItems(this.picture_list);
  }

  private loadTextLines() {
    this.text_lines = [];
    for (let i = 0; i < 36; i++) {
      this.text_lines.push('');
    }
    this.loadIndex = 0;
    this.sendRequestText();
  }

  private sendRequestText() {
    // Параметры запроса указывают на то, что он активен?
    if (this.loadIndex < 0 || this.loadIndex >= this.text_lines.length) {
      this.stopLoadText();
      return;
    }
    // Завершить ожидание предыдущей запрошенной строки / колонки
    if (!isNullOrUndefined(this.intervalId)) {
      clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
    // Отправить запрос на получение строки текста
    this.wsService.sendText(`$13 2 ${this.loadIndex};`);
    // Отложенный запрос, на случай, если запрос, отправленной строкой выше не будет выполнен - отправить повторно
    this.intervalId = setInterval(() => {
      if (this.loadIndex < 0 || this.loadIndex >= this.text_lines.length) {
        this.stopLoadText();
      } else {
        this.wsService.sendText(`$13 2 ${this.loadIndex};`);
      }
    }, 1000);
  }

  private stopLoadText() {
    if (!isNullOrUndefined(this.intervalId)) {
      clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
    this.loadIndex = -1;
  }

  ngOnDestroy() {
    this.stopLoadText();
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
