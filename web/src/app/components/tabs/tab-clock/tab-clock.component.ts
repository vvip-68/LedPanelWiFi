import {Component, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {ComboBoxItem} from "../../../models/combo-box.model";
import {distinctUntilChanged, map} from "rxjs/operators";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import { InputRestrictionDirective } from '../../../directives/input-restrict.directive';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatRadioModule } from '@angular/material/radio';
import { MatIconModule } from '@angular/material/icon';
import { MatSliderModule } from '@angular/material/slider';
import { MatOptionModule } from '@angular/material/core';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatSelectModule } from '@angular/material/select';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
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
        MatButtonModule,
        InputRestrictionDirective,
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

  public time_area_list: ComboBoxItem[] = [];
  public time_area: string = '';
  public time_zone: string = '';

  private speedChanged$ = new BehaviorSubject(this.clock_scroll_speed);

  showDateDurationFormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  showDateIntervalFormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  ntpServerNameFormControl = new FormControl('', [Validators.required]);
  ntpTimeZoneFormControl = new FormControl('', [Validators.required]);
  matcher = new AppErrorStateMatcher();

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService,
    private httpClient: HttpClient)
  {
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
          let request = 'C12|C35|CE|CO|CC|CK|DC|DD|DI|DW|NC|NP|NZ|NS|SC|WC|WN|WV|WZ';
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
              break;
            case 'CC':
              this.clock_color_mode = this.managementService.state.clock_color_mode;
              break;
            case 'CK':
              this.clock_size = this.managementService.state.clock_size;
              break;
            case 'DC':
              this.clock_show_date = this.managementService.state.clock_show_date;
              break;
            case 'DD':
              this.showDateDurationFormControl.setValue(this.managementService.state.clock_show_date_time);
              break;
            case 'DI':
              this.showDateIntervalFormControl.setValue(this.managementService.state.clock_show_date_intvl);
              break;
            case 'DW':
              this.clock_show_temp = this.managementService.state.clock_show_temp;
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
  }

  public loadTimeZones() {
    const filePath = `assets/tz-${this.L.lang}.json`;
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

            if (first) {area = c_area; first = false;}

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
              list.sort((a,b) => a.displayText.localeCompare(b.displayText));
              this.managementService.tz_map.set(area, list);
              area = c_area;
              list = [];
            }

            list.push(new ComboBoxItem(c_zone, zone));
          }
          this.time_area_list.sort((a,b) => a.displayText.localeCompare(b.displayText));
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

  toggleClockOverEffects() {
    this.clock_use_overlay = !this.clock_use_overlay;
    // $19 1 X;    - сохранить настройку X "Часы поверх эффектов"; X=0 - нет, X=1 - да
    this.socketService.sendText(`$19 1 ${this.clock_use_overlay ? 1 : 0};`);
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
    this.socketService.sendText(`$19 7 ${this.clock_size};`);
  }

  changeFontType() {
    // $19 20 X;    - Размер часов X: 0 - автовыбор в звыисимости от размера матрицы (3x5 или 5x7), 1 - малые 3х5, 2 - большие 5x7
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
  }

  setTempShowLetter(checked: boolean) {
    this.show_letter = checked;
    const value = (this.show_degree ? 0x02 : 0x00) | (this.show_letter ? 0x01 : 0x00);
    this.socketService.sendText(`$12 7 ${value};`);
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

}
