import {Component, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {ComboBoxItem} from "../../../models/combo-box.model";
import {distinctUntilChanged} from "rxjs/operators";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty, rangeValidator, timeZoneValidator} from "../../../services/helper";
import {FormControl, Validators} from "@angular/forms";

@Component({
  selector: 'app-tab-clock',
  templateUrl: './tab-clock.component.html',
  styleUrls: ['./tab-clock.component.scss'],
})
export class TabClockComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  supportWeather: boolean = false;

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

  private speedChanged$ = new BehaviorSubject(this.clock_scroll_speed);

  showDateDurationFormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  showDateIntervalFormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  ntpServerNameFormControl = new FormControl('', [Validators.required]);
  ntpSyncIntervalFormControl = new FormControl(0, [Validators.required, rangeValidator(5, 240)]);
  ntpTimeZoneFormControl = new FormControl('', [Validators.required, timeZoneValidator()]);
  matcher = new AppErrorStateMatcher();

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService
  ) {
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
          let request = 'CE|CO|CC|CK|DC|DD|DI|DW|NC|NP|NS|NT|NZ|SC|WC|WN|WZ';
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
            case 'NT':
              this.ntpSyncIntervalFormControl.setValue(this.managementService.state.clock_ntp_sync);
              break;
            case 'NZ':
            case 'NM':
              const time_zone_hour = this.managementService.state.clock_time_zone_hour;
              const time_zone_minutes = this.managementService.state.clock_time_zone_minutes;
              if (time_zone_minutes === 0)
                this.ntpTimeZoneFormControl.setValue(`${time_zone_hour}`);
              else
                this.ntpTimeZoneFormControl.setValue(`${time_zone_hour}:${time_zone_minutes.toString().padStart(2, '0')}`);
              break;
            case 'OF':
              this.clock_tm1627_off = this.managementService.state.clock_tm1627_off;
              break;
            case 'SC':
              this.clock_scroll_speed = this.managementService.state.clock_scroll_speed;
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
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
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
    // $19 7 X;    - Размер часов X: 0 - автовыбор ы звыисимости от размера матрицы (3x5 или 5x7), 1 - малые 3х5, 2 - большие 5x7
    this.socketService.sendText(`$19 7 ${this.clock_size};`);
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
    return this.ntpSyncIntervalFormControl.valid && this.ntpTimeZoneFormControl.valid;
  }

  applySyncSettings($event: MouseEvent) {
    // $6 1|текст  - имя сервера NTP
    // $19 3 N Z;  - $19 3 N ZH ZM; - Период синхронизации часов NTP (N) и Часовой пояс (ZH) -12..12  и минуты 0 / 15 / 30 / 45 (ZM)
    const N = this.managementService.state.clock_ntp_sync = this.ntpSyncIntervalFormControl.value as number;
    const tz = this.ntpTimeZoneFormControl.value as string;

    let ZH = 0;
    let ZM = 0;
    // Часовой пояс - число от -12 до 12 - если нет минут
    // Если минуты есть - отделяются от часов символом двоеточие ':'
    const tzp = tz.trim().split(':');
    if (tzp.length == 1) {
      // нет минут, только часы
      ZH = Number(tzp[0])
    } else {
      // есть минуты
      ZH = Number(tzp[0])
      ZM = Number(tzp[1])
    }

    this.managementService.state.clock_time_zone_hour = ZH;
    this.managementService.state.clock_time_zone_minutes = ZM;

    const server_name = this.managementService.state.clock_ntp_server = this.ntpServerNameFormControl.value as string

    this.socketService.sendText(`$6 1|${server_name}`);
    this.socketService.sendText(`$19 3 ${N} ${ZH} ${ZM};`);
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }

}
