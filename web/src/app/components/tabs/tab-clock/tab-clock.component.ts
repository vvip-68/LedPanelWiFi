import {Component, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {ComboBoxItem} from "../../../models/combo-box.model";
import {distinctUntilChanged, map} from "rxjs/operators";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import {FormControl, FormsModule, ReactiveFormsModule, Validators} from "@angular/forms";
import {MatButtonModule} from '@angular/material/button';
import {MatInputModule} from '@angular/material/input';
import {MatCheckboxModule} from '@angular/material/checkbox';
import {MatRadioModule} from '@angular/material/radio';
import {MatIconModule} from '@angular/material/icon';
import {MatSliderModule} from '@angular/material/slider';
import {MatOptionModule} from '@angular/material/core';
import {DisableControlDirective} from '../../../directives/disable-control.directive';
import {MatSelectModule} from '@angular/material/select';
import {MatFormFieldModule} from '@angular/material/form-field';
import {MatSlideToggleModule} from '@angular/material/slide-toggle';
import {HttpClient, HttpHeaders} from "@angular/common/http";
import {stripComments} from "jsonc-parser";
import {Base} from "../../base.class";

@Component({
  selector: 'app-tab-clock',
  templateUrl: './tab-clock.component.html',
  styleUrls: ['./tab-clock.component.scss'],
  standalone: true,
  imports: [
    MatSlideToggleModule,
    FormsModule,
    MatFormFieldModule,
    MatSelectModule,
    DisableControlDirective,
    MatOptionModule,
    MatSliderModule,
    MatIconModule,
    MatRadioModule,
    MatCheckboxModule,
    MatInputModule,
    ReactiveFormsModule,
    MatButtonModule
  ],
})
export class TabClockComponent extends Base implements OnInit, OnDestroy {

  public supportWeather: boolean = false;
  public time12h = -1;
  public small_font_type = -1;
  public clock_use_overlay = false;
  public clock_orientation: number = -1;
  public orient_list: ComboBoxItem[] = [];
  public clock_color_mode: number = -1;
  public color_list: ComboBoxItem[] = [];
  public clock_size: number = 0;
  public clock_show_date: boolean = false;
  public clock_show_temp = false;
  public clock_temp_color_day = false;
  public clock_temp_color_night = false;
  public clock_night_color = -1;
  public clock_use_ntp = false;
  public clock_tm1627_off = false;
  public clock_scroll_speed = -1;
  public show_degree = false;
  public show_letter = false;
  public offsetX = 0;
  public offsetY = 0;
  public dotWidth = 2;
  public dotSpace = 1;
  public dotWidthToggler = true;
  public dotSpaceToggler = true;

  public offsetXinitialized = false;
  public offsetYinitialized = false;


  public allowX = 0;
  public allowY = 0;
  public allowAdvanced = false;
  public allowDotWidth = false;
  public allowDotSpace = false;

  public time_area_list: ComboBoxItem[] = [];
  public time_area: string = '';
  public time_zone: string = '';

  showDateDurationFormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  showDateIntervalFormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  ntpServerNameFormControl = new FormControl('', [Validators.required]);
  ntpTimeZoneFormControl = new FormControl('', [Validators.required]);
  matcher = new AppErrorStateMatcher();

  private calcDelayedTimer: any;

  private speedChanged$ = new BehaviorSubject(this.clock_scroll_speed);
  private offsetXChanged$ = new BehaviorSubject(this.offsetX);
  private offsetYChanged$ = new BehaviorSubject(this.offsetY);

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService,
    private httpClient: HttpClient) {
    super();
    this.color_list.push(new ComboBoxItem(this.L.$('Одноцветные'), 0));
    this.color_list.push(new ComboBoxItem(this.L.$('Каждая цифра свой цвет'), 1));
    this.color_list.push(new ComboBoxItem(this.L.$('Часы, точки, минуты'), 2));
  }

  ngOnInit() {
    if (this.managementService.state.clock_allow_horizontal) {
      this.orient_list.push(new ComboBoxItem(this.L.$('Горизонтально'), 0));
    }
    if (this.managementService.state.clock_allow_vertical) {
      this.orient_list.push(new ComboBoxItem(this.L.$('Вертикально'), 1));
    }

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в экране
          let request = 'W|H|C12|C35|CE|CO|CC|CK|CX|CY|CD|CS|DC|DD|DI|DW|NC|NP|NZ|NS|SC|WC|WN|WV|WZ';
          if (this.managementService.state.supportTM1637) {
            request += '|OF';
          }
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'C12':
              this.time12h = this.managementService.state.time12h ? 1 : 0;
              break;
            case 'C35':
              this.small_font_type = this.managementService.state.small_font_type;
              break;
            case 'CE':
              this.clock_use_overlay = this.managementService.state.clock_use_overlay;
              break;
            case 'CO':
              this.clock_orientation = this.managementService.state.clock_orientation;
              this.calculateOffsetAllowed();
              break;
            case 'CC':
              this.clock_color_mode = this.managementService.state.clock_color_mode;
              break;
            case 'CD':
              this.dotWidth = this.managementService.state.clock_dot_width;
              this.calculateOffsetAllowed();
              break;
            case 'CK':
              this.clock_size = this.managementService.state.clock_size;
              this.calculateOffsetAllowed();
              break;
            case 'CS':
              this.dotSpace = this.managementService.state.clock_dot_space;
              this.calculateOffsetAllowed();
              break;
            case 'CV':
              this.calculateOffsetAllowed();
              break;
            case 'CH':
              this.calculateOffsetAllowed();
              break;
            case 'CX':
              this.offsetX = this.managementService.state.clock_offset_x;
              this.offsetXinitialized = true;
              break;
            case 'CY':
              this.offsetY = this.managementService.state.clock_offset_y;
              this.offsetYinitialized = true;
              break;
            case 'DC':
              this.clock_show_date = this.managementService.state.clock_show_date;
              this.calculateOffsetAllowed();
              break;
            case 'DD':
              this.showDateDurationFormControl.setValue(this.managementService.state.clock_show_date_time);
              break;
            case 'DI':
              this.showDateIntervalFormControl.setValue(this.managementService.state.clock_show_date_intvl);
              break;
            case 'DW':
              this.clock_show_temp = this.managementService.state.clock_show_temp;
              this.calculateOffsetAllowed();
              break;
            case 'WC':
              this.clock_temp_color_day = this.managementService.state.clock_temp_color_day;
              break;
            case 'WN':
              this.clock_temp_color_night = this.managementService.state.clock_temp_color_night;
              break;
            case 'NC':
              this.clock_night_color = this.managementService.state.clock_night_color;
              break;
            case 'NP':
              this.clock_use_ntp = this.managementService.state.clock_use_ntp;
              break;
            case 'NS':
              this.ntpServerNameFormControl.setValue(this.managementService.state.clock_ntp_server);
              break;
            case 'NZ':
              this.ntpTimeZoneFormControl.setValue(this.managementService.state.clock_time_zone);
              // Если карта ключей уже загружена - составить список зон времени по ключам карты временнЫх зон
              const list = this.managementService.tz_map.keys();
              let value = list.next().value;
              this.time_area_list = [];
              while (!isNullOrUndefinedOrEmpty(value)) {
                this.time_area_list.push(new ComboBoxItem(value, value));
                value = list.next().value;
              }
              // Если временные зоны еще не были загружены - загрузить
              // Если уже были загружены - найти в списках текущую временнУю зону
              if (this.time_area_list.length === 0) {
                this.loadTimeZones();
              } else {
                this.findCurrentTimeZone();
              }
              break;
            case 'OF':
              this.clock_tm1627_off = this.managementService.state.clock_tm1627_off;
              break;
            case 'SC':
              this.clock_scroll_speed = this.managementService.state.clock_scroll_speed;
              break;
            case 'WV':
              const props = this.managementService.state.show_temp_props;
              this.show_degree = (props & 0x02) > 0;
              this.show_letter = (props & 0x01) > 0;
              break;
            case 'WZ':
              this.supportWeather = this.managementService.state.supportWeather;
              break;
          }
        }
      });

    this.speedChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (value >= 0) {
          this.clock_scroll_speed = value;
          // $19 12 X;   - скорость прокрутки часов оверлея или 0, если часы остановлены по центру матрицы
          this.socketService.sendText(`$19 12 ${value};`);
        }
      });

    this.offsetXChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetXinitialized) {
          this.offsetX = value;
          this.managementService.state.clock_offset_x = this.offsetX;
          // $19 21 X Y; - Смещение часов по X,Y
          this.socketService.sendText(`$19 21 ${this.offsetX} ${this.offsetY};`);
        }
      });

    this.offsetYChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetYinitialized) {
          this.offsetY = value;
          this.managementService.state.clock_offset_y = this.offsetY;
          // $19 21 X Y; - Смещение часов по X,Y
          this.socketService.sendText(`$19 21 ${this.offsetX} ${this.offsetY};`);
        }
      });

  }

  public loadTimeZones() {
    const filePath = `a/tz-${this.L.lang}.json`;
    const headers = new HttpHeaders();
    this.httpClient.get(filePath, {headers, responseType: 'text'})
      .pipe(map(response => JSON.parse(stripComments(<string>response))))
      .subscribe({
        next: (data) => {
          //  {
          //     "label": "Africa/Abidjan",
          //     "value": "000_GMT0"
          //   },
          const keys = Object.keys(data);
          let first = true;
          let area = '';
          let list: ComboBoxItem[] = [];
          let item_idx = 0;

          for (const key of keys) {
            const label = data[key].label;
            const zone = data[key].value;
            const idx = label.indexOf('/');
            const c_area = label.substring(0, idx);
            const c_zone = label.substring(idx + 1);
            item_idx++;

            if (first) {
              area = c_area;
              first = false;
            }

            const zx = zone.indexOf('_');
            if (zone === this.managementService.state.clock_time_zone || (zx !== -1 && zone.substring(zx + 1) === this.managementService.state.clock_time_zone)) {
              this.time_area = area;
              this.time_zone = zone;
            }

            if (c_area !== area || item_idx === keys.length) {
              this.time_area_list.push(new ComboBoxItem(area, area));
              if (item_idx === keys.length) {
                list.push(new ComboBoxItem(c_zone, zone));
              }
              list.sort((a, b) => a.displayText.localeCompare(b.displayText));
              this.managementService.tz_map.set(area, list);
              area = c_area;
              list = [];
            }

            list.push(new ComboBoxItem(c_zone, zone));
          }
          this.time_area_list.sort((a, b) => a.displayText.localeCompare(b.displayText));
        },
        error: (error) => {
          console.warn('Не удалось загрузить список временных зон');
          console.warn(error.message);
        }
      });
  }

  findCurrentTimeZone() {
    const tz_rule = this.managementService.state.clock_time_zone;
    this.managementService.tz_map.forEach((list, area) => {
      const item = list.find(item => item.value === tz_rule);
      if (item) {
        this.time_area = area;
        this.time_zone = item.value;
      }
    })
  }

  formatLabel(value: number) {
    return value.toString();
  }

  speedChanged(value: number) {
    this.speedChanged$.next(value);
  }

  offsetXChanged(value: number) {
    this.offsetXChanged$.next(value);
  }

  offsetYChanged(value: number) {
    this.offsetYChanged$.next(value);
  }

  toggleClockOverEffects() {
    this.clock_use_overlay = !this.clock_use_overlay;
    // $19 1 X;    - сохранить настройку X "Часы поверх эффектов"; X=0 - нет, X=1 - да
    this.socketService.sendText(`$19 1 ${this.clock_use_overlay ? 1 : 0};`);
  }

  toggleDotWidth() {
    this.dotWidthToggler = !this.dotWidthToggler;
    this.dotWidth = this.managementService.state.clock_dot_width = this.dotWidthToggler ? 2 : 1;
    // $19 22 X Y; - Ширина разделительных точек в больших часах (X = 1|2), Y - есть ли пробелы между цифрами часов и точками 0|1
    this.socketService.sendText(`$19 22 ${this.dotWidth} ${this.dotSpace};`);
  }

  toggleDotSpace() {
    this.dotSpaceToggler = !this.dotSpaceToggler;
    this.dotSpace = this.managementService.state.clock_dot_space = this.dotSpaceToggler ? 1 : 0;
    // $19 22 X Y; - Ширина разделительных точек в больших часах (X = 1|2), Y - есть ли пробелы между цифрами часов и точками 0|1
    this.socketService.sendText(`$19 22 ${this.dotWidth} ${this.dotSpace};`);
  }

  toggleTM1637() {
    this.clock_tm1627_off = !this.clock_tm1627_off;
    // $19 4 X;    - Выключать индикатор TM1637 при выключении экрана X=0 - нет, X=1 - да
    this.socketService.sendText(`$19 4 ${this.clock_tm1627_off ? 1 : 0};`);
  }

  clockOrientationChanged(value: number) {
    // $19 6 X; - Ориентация часов  X: 0 - горизонтально, 1 - вертикально
    if (value >= 0) {
      this.clock_orientation = value;
      this.socketService.sendText(`$19 6 ${this.clock_orientation};`);
    }
  }

  clockColorModeChanged(value: number) {
    // $19 5 X;    - Режим цвета часов оверлея X: 0 - монохром, 1 - цифра-цвет, 2 - часы-точки-минуты;
    if (value >= 0) {
      this.clock_color_mode = value;
      this.socketService.sendText(`$19 5 ${this.clock_color_mode};`);
    }
  }

  timeAreaChanged(value: string) {
    this.time_area = value;
  }

  timeZoneChanged(value: string) {
    this.time_zone = value;
  }

  getTimeZones(area: string): ComboBoxItem[] {
    return this.managementService.tz_map.get(area) || [];
  }


  toggleTemperatureInClock() {
    this.clock_show_temp = !this.clock_show_temp;
    // $19 9 X;    - Показывать температуру вместе с часами X=1 - да; X=0 - нет
    this.socketService.sendText(`$19 9 ${this.clock_show_temp ? 1 : 0};`);
  }

  toggleDateInClock() {
    this.clock_show_date = !this.clock_show_date;
    // $19 16 X;   - Показывать дату в режиме часов:  X=0 - нет, X=1 - да
    this.socketService.sendText(`$19 16 ${this.clock_show_date ? 1 : 0};`);
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  toNumber(value: string): number {
    return Number(value);
  }

  setNightColor(color: number) {
    if (color >= 0) {
      this.clock_night_color = color;
      // $19 10 X;   - Цвет ночных часов:  0 - R; 1 - G; 2 - B; 3 - C; 3 - M; 5 - Y; 6 - W;
      this.socketService.sendText(`$19 10 ${color};`);
    }
  }

  changeClockSize() {
    // $19 7 X;    - Размер часов X: 0 - автовыбор в звыисимости от размера матрицы (3x5 или 5x7), 1 - малые 3х5, 2 - большие 5x7
    this.managementService.state.clock_size = this.clock_size;
    this.socketService.sendText(`$19 7 ${this.clock_size};`);
    this.calculateOffsetAllowed();
  }

  changeFontType() {
    // $19 20 X; - Показывать малые часы шрифтом 3х5 - 0 квадратный шрифт, 1 - круглый шрифт
    this.socketService.sendText(`$19 20 ${this.small_font_type};`);
  }

  changeTimeFormat() {
    // $19 19 X;    - X=0 - 24-часовой формат, X=1 - 12-часовой формат
    this.socketService.sendText(`$19 19 ${this.time12h};`);
  }

  isLargeClockAvailable(): boolean {
    // Большие часы для шрифта 5x7 требуют 4*5 /цифры/ + 4 /двоеточие/ + 2 /пробел между цифрами часов и минут / = 23, 25 или 26 колонки (одинарные / двойные точки в часах) если 23 - нет пробела вокруг точек
    const enableBigH = this.clock_orientation === 0 && this.managementService.state.width >= 23;
    const enableBigV = this.clock_orientation === 1 && this.managementService.state.height >= 15;
    return (enableBigH || enableBigV) && (this.managementService.state.clock_allow_horizontal || this.managementService.state.clock_allow_vertical);
  }

  setTempUseDayColor(checked: boolean) {
    this.clock_temp_color_day = checked;
    this.socketService.sendText(`$12 3 ${checked ? 1 : 0};`);
  }

  setTempUseNightColor(checked: boolean) {
    this.clock_temp_color_night = checked;
    this.socketService.sendText(`$12 6 ${checked ? 1 : 0};`);
  }

  setTempShowDegree(checked: boolean) {
    this.show_degree = checked;
    const value = (this.show_degree ? 0x02 : 0x00) | (this.show_letter ? 0x01 : 0x00);
    this.socketService.sendText(`$12 7 ${value};`);
    this.managementService.state.show_temp_props = value;
  }

  setTempShowLetter(checked: boolean) {
    this.show_letter = checked;
    const value = (this.show_degree ? 0x02 : 0x00) | (this.show_letter ? 0x01 : 0x00);
    this.socketService.sendText(`$12 7 ${value};`);
    this.managementService.state.show_temp_props = value;
  }

  isValid(): boolean {
    return this.showDateDurationFormControl.valid && this.showDateIntervalFormControl.valid;
  }

  applyDateShow($event: MouseEvent) {
    // $19 17 DD II; - Продолжительность / интервал отображения даты / температуры (в секундах): D - продолжительность; I - интервал;
    const DD = this.managementService.state.dawn_duration = this.showDateDurationFormControl.value as number;
    const II = this.managementService.state.alarm_duration = this.showDateIntervalFormControl.value as number;

    this.socketService.sendText(`$19 17 ${DD} ${II};`);
  }

  toggleUseNtpClock() {
    this.clock_use_ntp = !this.clock_use_ntp;
    // $19 2 X;    - Использовать синхронизацию часов NTP;  X=0 - нет, X=1 - да
    this.socketService.sendText(`$19 2 ${this.clock_use_ntp ? 1 : 0};`);
  }

  applySetClockTime($event: MouseEvent) {
    // $19 8 YYYY MM DD HH MM; - Установить текущее время YYYY.MM.DD HH:MM
    const time = new Date(Date.now());
    const YYYY = time.getFullYear();
    const MNTH = time.getMonth() + 1;  // В Javascript счет месяцев с нуля: 0 - январь
    const DAY = time.getDate();
    const HH = time.getHours();
    const MM = time.getMinutes();
    this.socketService.sendText(`$19 8 ${YYYY} ${MNTH} ${DAY} ${HH} ${MM};`);
  }

  isValidSync(): boolean {
    return this.ntpServerNameFormControl.valid;
  }

  applySyncSettings($event: MouseEvent) {
    // $6 1|текст   - имя сервера NTP
    // $6 10|текст  - Выбранное правила временнОй зоны часового пояса

    const server_name = this.managementService.state.clock_ntp_server = this.ntpServerNameFormControl.value as string;
    const time_zone = this.managementService.state.clock_time_zone = this.time_zone;

    this.socketService.sendText(`$6 1|${server_name}`);
    this.socketService.sendText(`$6 10|${time_zone}`);
  }

  calculateOffsetAllowed() {
    // Пересчет параметров зависит от нескольких значений, которые поступают пачкой последовательно
    // Не нужно выполнять пересчет на каждый поступивший параметр.
    // Ждем, пока поступит последний и через xxx мс выполняем расчет
    if (this.calcDelayedTimer) clearTimeout(this.calcDelayedTimer);
    this.calcDelayedTimer = setTimeout(() => {
      this.doCalculateOffsetAllowed();
      this.calcDelayedTimer = undefined;
    }, 500);
  }

  getClockSizeType(): number {
    const clock_orient = this.managementService.state.clock_orientation;
    const pWIDTH = this.managementService.state.width;
    const pHEIGHT = this.managementService.state.height;
    let clock_size = this.managementService.state.clock_size;

    // Если часы авто или большие - определить - а поместятся ли они на
    // матрицу по ширине при горизонтальном режиме / по высоте при вертикальном
    // Большие часы для шрифта 5x7 требуют 4*5 /цифры/ + 4 /двоеточие/ + 2 /пробел между цифрами часов и минут / = 23, 25 или 26 колонки (одинарные / двойные точки в часах) если 23 - нет пробела вокруг точек
    if ((clock_size == 0 || clock_size == 2) && ((clock_orient == 0 && pWIDTH < 23) || (clock_orient == 1 && pHEIGHT < 15))) clock_size = 1;
    if (clock_size == 0) clock_size = 2;
    return clock_size;
  }

  doCalculateOffsetAllowed() {

    // Вычислить ширину и высоту часов в зависимости от того - большие или малые часы, горизонтально или вертикально,
    // показывается ли дата, температура, ширины точки и пробела между точками и цифрами
    let clockWidth = 0;
    let clockHeight = 0;
    let allowDotWidth = false;
    let allowDotSpace = false;

    const smallClock = this.getClockSizeType() == 1;

    if (this.managementService.state.clock_orientation == 0 ) {
      // Горизонтальные часы
      if (smallClock) {
        // Малые часы - Шрифт 3x5
        // Ширина - 4 цифры, nри пробела между цифрами - ширина 15
        clockWidth = 15;
        // Высота - если есть температура или отображении даты - два ряда + 1 строка пробела == 11, если в один ряд - 5
        clockHeight = this.managementService.state.clock_show_temp || this.managementService.state.clock_show_date ? 11 : 5;
        // Насьройка ширины точки и прбелов  около точки - только для больших часов
      } else {
        allowDotWidth = true;
        allowDotSpace = true;
        // Ширина - 4 цифры, два пробела между цифрами часов и минут, плюс ширина точек и пробелы между точками и цифрами- ширина 22 + 1/2 - гирина точки + 2/0 - наличие пробела между цифрами
        clockWidth = 22 + this.managementService.state.clock_dot_width + this.managementService.state.clock_dot_space * 2;
        // Высота - если есть температура или отображении даты - два ряда + 1 строка пробела == 15, если в один ряд - 7
        clockHeight = this.managementService.state.clock_show_temp || this.managementService.state.clock_show_date ? 15 : 7;
        // Полная ширина больших часов - 26.
        // Если ширина матрицы меньше 26 - ширина точек - 1
        if (this.managementService.state.width < 26) {
          clockWidth = 25;
          this.dotWidth = this.managementService.state.clock_dot_width = 1;
          allowDotWidth = false;
        }
        // Если ширина матрицы меньше 25 - нет пробела между точками и окружающими цифрами
        if (this.managementService.state.width < 25) {
          clockWidth = 23;
          this.dotSpace = this.managementService.state.clock_dot_space = 0;
          allowDotSpace = false;
        }
      }
    } else {
      // Вертикальные часы
      if (smallClock) {
        // Малые часы - шрифт 3x5 - в два ряда - нет зависимости от даты и температуры - дата - в два ряда, температура не отображается
        clockWidth = 7;
        clockHeight = 11;
      } else {
        // Большие часы - шрифт 5x7
        allowDotWidth = true;
        allowDotSpace = true;
        // Ширина - 2 цифры + 1 пробел между цифрами = 11
        clockWidth = 11;
        // Высота - в два ряда плюс пробельная строка = 15
        clockHeight = 15;
      }
    }

    this.allowDotWidth = allowDotWidth;
    this.allowDotSpace = allowDotSpace;

    this.dotWidthToggler = this.dotWidth == 2;
    this.dotSpaceToggler = this.dotSpace == 1;

    // Если ширина матрицы меньше ширины часов
    this.allowX = this.managementService.state.width < clockWidth ? 0 : Math.round((this.managementService.state.width - clockWidth) / 2) + 1;
    // Если высота матрицы меньше высоты часов
    this.allowY = this.managementService.state.height < clockHeight ? 0 : Math.round((this.managementService.state.height - clockHeight) / 2);

    // Рассчитать доступность блока "Расширенные настройки и допустимые смещения часов по осям X,Y
    this.allowAdvanced = this.allowX > 0 || this.allowY > 0 || allowDotWidth || allowDotSpace;
  }
}
