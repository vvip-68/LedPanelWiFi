import {Injectable, OnDestroy} from '@angular/core';
import {BehaviorSubject, debounceTime, Subject, takeUntil, timer} from 'rxjs';
import {EffectModel, IEffectModel} from '../../models/effect.model';
import {StateModel} from '../../models/state.model';
import {CommonService, MessageType} from '../common/common.service';
import {clearBraces, isNullOrUndefined, isNullOrUndefinedOrEmpty} from '../helper';
import {LanguagesService} from '../languages/languages.service';
import {WebsocketService, WS} from '../websocket/websocket.service';
import {ComboBoxItem} from "../../models/combo-box.model";
import {distinctUntilChanged, map} from "rxjs/operators";
import {HttpClient, HttpHeaders} from "@angular/common/http";
import {stripComments} from "jsonc-parser";
import {Base} from "../../components/base.class";

@Injectable({
  providedIn: 'root'
})
export class ManagementService extends Base implements OnDestroy {

  private checkInterval = 2000;

  // Строка перевода ID эффекта в символьное представления 0..9,A..Z,a..z - должна соответствовать строке в прошивке устройства
  // Здесь 62 символа. Эффектов сейчас 49. При увеличении количества эффектов строку нужно будет добавить другими символами,
  // синхронизировав с такой же кодировочной строкой в прошивке
  public effects_code = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';

  public effects: Array<IEffectModel> = [];        // Полный список эффектов, полученный из устройства

  public effects_in_use: Array<number> = [];       // Список индексов эффектов из массива effects, выбранных для использования
  public effects_not_use: Array<number> = [];      // Список индексов эффектов из массива effects, НЕ выбранных для использования

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

  public matrixColors: Array<string[]> = [];                 // Цвета в массиве рисования
  public tz_map: Map<string, ComboBoxItem[]> = new Map();    // Зоны часового пояса

  private stateValues: Map<string, boolean> = new Map();
  private parts: Map<string, string> = new Map();

  private favorites: string = "";
  private last_message_time: number = Date.now();

  private loadIndex: number = -1;     // индекс загружаемой строки
  private intervalId: any = undefined;


  constructor(private commonService: CommonService,
              private wsService: WebsocketService,
              private httpClient: HttpClient,
              private L: LanguagesService) {
    super();
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

    const obj = JSON.parse(data);
    const keys = Object.keys(obj);

    for (const key of keys) {

      if (isNullOrUndefinedOrEmpty(key)) continue;

      const cmd = key;
      let args = obj[cmd]?.trim();

      if (cmd.length === 0 || isNullOrUndefined(args)) continue;
      // console.log('cmd=\'%s\'; args=\'%s\'', cmd, args);

      // Длинные сообщения, не вмещающиеся в буфер отправляются клиентом по частям
      // Первая часть в начале имеет фразу "`!`", следующие части кроме последней начинаются с "`>`", последняя часть - "`#`"
      // Части собираются в объект Map -> parts. Когда приходит завершающая часть - собранная строка передается на обработку

      if (args.startsWith('`!`')) {
        this.parts.set(key, args.substring(3));
        continue;
      } else
      if (args.startsWith('`>`')) {
        let prev = this.parts.get(key) + args.substring(3);
        this.parts.set(key, prev);
        continue;
      } else
      if (args.startsWith('`#`')) {
        args = this.parts.get(key) + args.substring(3);
        this.parts.delete(key);
        console.log(`json="{${key}":"${args}"}`);
      } else {
        console.log('json=\'%s\'', data);
      }

      this.stateValues.set(key, true);

      switch (cmd.toUpperCase()) {

        // Язык из прошивки - 'RUS', 'ENG', 'ESP', 'LAT' или что в прошивке еще определено.
        // Встроенные языки - 'RUS', 'ENG', 'ESP', 'LAT' изначально присутствуют в прошивке в сервисе languageService,
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

        // --- Контрольная сумма списка эффектов
        // --- Список эффектов в формате строки имен, разделенных запятыми
        case 'LE':
        case 'CRLE': {

          let key2 = key;
          // Если пришла контрольная сумма списка эффектов...
          if (key2 === 'CRLE') {
            // ... проверить что она не пустая и совпадает с сохраненной в localStorage
            const crc = window.localStorage['CRLE'];
            const list = window.localStorage['LE'];
            if (isNullOrUndefinedOrEmpty(list) || isNullOrUndefinedOrEmpty(crc) || crc !== args) {
              // ... если нет или не совпадает - запросить список эффектов у устройства
              window.localStorage['CRLE'] = args;
              this.getKeys('LE');
              return;
            } else {
              args = list;
              key2 = 'LE';
            }
          }

          // Если пришел список эффектов или он был извлечен из хранилища localStorage...
          if (key2 === 'LE') {
            // Сохранить его в локальном хранилище
            window.localStorage['LE'] = args;
            // Разобрать список в массив эффектов
            // Список эффектов передается строго в порядке соответствия названия эффекта их индексу, используемому для управления эффектом
            // Чтобы при отключении в прошивке некоторых эффектов индексы не смещались - имена пропущенных эффектов в списке - пустые.
            // Это позволяет сохранить позицию (номер эффекта), передаваемой командам "Включить эффект #N" и "Редактировать эффект #N"
            let idx = 0;
            const parts = clearBraces(args).split(',');
            this.effects = [];
            for (const part of parts) {
              // Добавляем только не пустые эффекты. Пустые позиции - эффекты, выключенные в прошивке
              if (!isNullOrUndefinedOrEmpty(part)) {
                this.effects.push(new EffectModel(idx, part));
              }
              // Индекс увеличиваем в любом случае
              idx++;
            }
            this.effects$.next(this.effects);
            // @ts-ignore
            const fav = this.favorites;
            if (!isNullOrUndefinedOrEmpty(fav)) {
              this.updateUsage(fav);
            } else {
              this.getKeys('FV');
            }
          }

          break;
        }

        // --- Контрольная сумма списка звуков будильника
        // --- Список звуков будильника в формате строки имен, разделенных запятыми
        case 'S1':
        case 'CRS1': {
          let key2 = key;
          // Если пришла контрольная сумма списка звуков будильника...
          if (key2 === 'CRS1') {
            // ... проверить что она не пустая и совпадает с сохраненной в localStorage
            const crc = window.localStorage['CRS1'];
            const list = window.localStorage['S1'];
            if (isNullOrUndefinedOrEmpty(list) || isNullOrUndefinedOrEmpty(crc) || crc !== args) {
              window.localStorage['CRS1'] = args;
              this.getKeys('S1');
              return;
            } else {
              args = list;
              key2 = 'S1';
            }
          }

          // Если пришел список звуков будильника или он был извлечен из хранилища localStorage...
          if (key2 === 'S1') {
            // Сохранить его в локальном хранилище
            window.localStorage['S1'] = args;
            // Разобрать список в массив
            let idx = 0;
            const parts = clearBraces(args).split(',');
            this.alarm_sounds = [];
            for (const part of parts) {
              this.alarm_sounds.push(new ComboBoxItem(part, idx++));
            }
            this.alarm_sounds$.next(this.alarm_sounds);
            this.stateKey$.next(cmd);
          }

          break;
        }

        // --- Контрольная сумма списка звуков рассвета
        // --- Список звуков рассвета в формате строки имен, разделенных запятыми
        case 'S2':
        case 'CRS2': {
          // Если пришла контрольная сумма списка звуков рассвета...
          let key2 = key;
          if (key2 === 'CRS2') {
            // ... проверить что она не пустая и совпадает с сохраненной в localStorage
            const crc = window.localStorage['CRS2'];
            const list = window.localStorage['S2'];
            if (isNullOrUndefinedOrEmpty(list) || isNullOrUndefinedOrEmpty(crc) || crc !== args) {
              window.localStorage['CRS2'] = args;
              this.getKeys('S2');
              return;
            } else {
              args = list;
              key2 = 'S2';
            }
          }

          // Если пришел список звуков рассвета или он был извлечен из хранилища localStorage...
          if (key2 === 'S2') {
            // Сохранить его в локальном хранилище
            window.localStorage['S2'] = args;
            // Разобрать список в массив
            let idx = 0;
            const parts = clearBraces(args).split(',');
            this.dawn_sounds = [];
            for (const part of parts) {
              this.dawn_sounds.push(new ComboBoxItem(part, idx++));
            }
            this.dawn_sounds$.next(this.dawn_sounds);
            this.stateKey$.next(cmd);
          }

          break;
        }

        // --- Контрольная сумма списка звуков нотификации
        // --- Список звуков нотификации в формате строки имен, разделенных запятыми
        case 'S3':
        case 'CRS3': {
          // Если пришла контрольная сумма списка звуков рассвета...
          let key2 = key;
          if (key2 === 'CRS3') {
            // ... проверить что она не пустая и совпадает с сохраненной в localStorage
            const crc = window.localStorage['CRS3'];
            const list = window.localStorage['S3'];
            if (isNullOrUndefinedOrEmpty(list) || isNullOrUndefinedOrEmpty(crc) || crc !== args) {
              window.localStorage['CRS3'] = args;
              this.getKeys('S3');
              return;
            } else {
              args = list;
              key2 = 'S3';
            }
          }

          // Если пришел список звуков рассвета или он был извлечен из хранилища localStorage...
          if (key2 === 'S3') {
            // Сохранить его в локальном хранилище
            window.localStorage['S3'] = args;
            // Разобрать список в массив
            let idx = 0;
            const parts = clearBraces(args).split(',');
            this.notify_sounds = [];
            for (const part of parts) {
              this.notify_sounds.push(new ComboBoxItem(part, idx++));
            }
            this.notify_sounds$.next(this.notify_sounds);
            this.stateKey$.next(cmd);
          }
          break;
        }

        case 'TY': {
          // 12:C > text
          let line_idx = -1;
          let line_text = '';
          let idx = args.indexOf(':');
          if (idx > 0) {
            line_idx = Number(args.substring(0, idx));
            if (line_idx >= 0) {
              idx = args.indexOf('>');
              if (idx > 0 && idx < this.text_lines.length) {
                line_text = args.substring(idx + 1).trim();
                this.text_lines[line_idx] = line_text;
                this.state.text_edit = args;
              }
              this.stateKey$.next(cmd);
            }
          }
          break;
        }

        case 'CRF0':
        case 'CRF1':
          // CRF0 - CRC списка файлов сохраненных картинок в файловой системе микроконтроллера
          // CRF1 - CRC списка файлов сохраненных картинок в папке SD-карты
          const crc = window.localStorage[key];
          const list = window.localStorage[key == 'CRF0' ? 'FL0' : 'FL1'];
          if (isNullOrUndefinedOrEmpty(list) || isNullOrUndefinedOrEmpty(crc) || crc !== args) {
            // ... если нет или не совпадает - запросить список эффектов у устройства
            window.localStorage[key] = args;
            this.getKeys(key == 'CRF0' ? 'FL0' : 'FL1');
            return;
          } else {
            let idx = 0;
            const parts = clearBraces(list).split(',');
            this.picture_list = [];
            for (const part of parts) {
              if (!isNullOrUndefinedOrEmpty(part)) {
                this.picture_list.push(new ComboBoxItem(part, idx++));
              }
            }
            this.picture_list = this.sortComboBoxItems(this.picture_list);
            this.picture_list$.next(this.picture_list);
            this.stateKey$.next(key == 'CRF0' ? 'FL0' : 'FL1');
          }
          break;

        case 'FL':
        case 'FL0':
        case 'FL1':{
          let idx = 0;
          window.localStorage[key] = args;
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
    let args = { };
    try { args = JSON.parse(data); } catch (e) { }

    console.log(topic, data, args);

    switch (topic) {
      // уведомления об ошибке клиенту
      // {
      //   "message": "message",    // "unknown page" / "unknown command"
      //   "text": "text"           // текст сообщения
      // }
      case WS.ON.ERROR: {
        // @ts-ignore
        const str = isNullOrUndefinedOrEmpty(args) || isNullOrUndefinedOrEmpty(args['text']) ? data : args['text'];
        this.commonService.timerMessage(MessageType.ERROR, str);
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
    // индекс эффектов 0..MAX_EFFECT-1 кодируется символами 0..9,A..Z,a..z
    // Позиция в строке - очередность воспроизведения эффектов - с 0 до конца строки.
    // Символ в позиции - кодированный ID эффекта (индекс в массиве this.effects)
    this.effects_in_use = [];
    this.effects_not_use = [];
    if (this.effects.length === 0) return;

    usage = clearBraces(usage);

    // Сформировать массив эффектов, отмеченных к использованию в порядке, определенном в переданной строке-параметре
    for (let idx = 0; idx < usage.length; idx++) {
      const effect_idx = this.effects_code.indexOf(usage[idx]);
      if (effect_idx >= 0 && effect_idx < this.effects.length) {
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

    const indecies = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    for (let i = 0; i < 36; i++) {
      const filePath = `assets/txt/${indecies[i]}?nocache=${Date.now()}`;
      const headers = new HttpHeaders().set('Cache-Control', 'no-cache, no-store, must-revalidate, post-check=0, pre-check=0').set('Pragma', 'no-cache').set('Expires', '0');
      this.httpClient.get(filePath, {headers, responseType: 'text'})
        .subscribe({
          next: (text) => {
            this.text_lines[i] = isNullOrUndefinedOrEmpty(text) ? '' : text.replace(/\u0000/g, ' ').trim();
          },
          error: (error) => {
            // this.wsService.sendText(`$13 2 ${i};`);
          }
        });
    }

  }

}
