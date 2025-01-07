import {Component, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
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
import {HttpClient} from "@angular/common/http";
import {Base} from "../../base.class";
import {MatTooltip} from "@angular/material/tooltip";
import {ComboBoxItem} from "../../../models/combo-box.model";

@Component({
  selector: 'app-tab-layout',
  templateUrl: './tab-layout.component.html',
  styleUrls: ['./tab-layout.component.scss'],
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
    MatTooltip
  ],
})
export class TabLayoutComponent extends Base implements OnInit, OnDestroy {

  public supportWeather: boolean = false;
  public clock_orientation: number = -1;
  public clock_size: number = 0;

  public offsetClockX = 0;
  public offsetClockY = 0;
  public offsetTempX = 0;
  public offsetTempY = 0;
  public offsetCalendarX = 0;
  public offsetCalendarY = 0;
  public offsetTextY = 0;
  public dotWidth = 2;
  public dotSpace = 1;
  public dotWidthToggler = true;
  public dotSpaceToggler = true;

  // Варианты размещения области отображения температуры и календаря
  public temp_show_variant: number = 0;         // 0 - под часами (снизу), 1 - за часами (справа в той же строке), 2 - произвольно
  public calendar_show_variant: number = 0;     // 0 - в позиции часов, 2 - произвольно

  // Выравнивание внутри области отображения температуры и календаря
  public clock_show_alignment: number = 0;       // 0 - по центру, 1- по левому краю, 2 - по правому краю
  public temp_show_alignment: number = 0;        // 0 - по центру, 1- по левому краю, 2 - по правому краю
  public calendar_show_alignment: number = 0;    // 0 - по центру, 1- по левому краю, 2 - по правому краю

  public offsetClockXinitialized = false;
  public offsetClockYinitialized = false;
  public offsetTempXinitialized = false;
  public offsetTempYinitialized = false;
  public offsetCalendarXinitialized = false;
  public offsetCalendarYinitialized = false;
  public offsetTextYinitialized = false;

  public allowDotWidthAndSpace = false;

  public calendarWidthList: ComboBoxItem[] = [];
  public calendarLinesList: ComboBoxItem[] = [];
  public calendarWidth: number = 2;         // - $19 13 X;   - Показывать 0/2/4 цифры года  X: 0/2/4, 1/2 строки
  public calendarLines: number = 2;         // - $19 13 X;   - Показывать 0/2/4 цифры года  X: 0/2/4, 1/2 строки

  public clock_t1: boolean = false;
  public clock_t2: boolean = false;
  public clock_t3: boolean = false;
  public clock_t4: boolean = false;

  public calendar_t1: boolean = false;
  public calendar_t2: boolean = false;
  public calendar_t3: boolean = false;
  public calendar_t4: boolean = false;

  public temp_t1: boolean = false;
  public temp_t2: boolean = false;
  public temp_t3: boolean = false;
  public temp_t4: boolean = false;

  public hide_clock_on_text: boolean = false;
  public hide_temp_on_text: boolean = false;
  public hide_calendar_on_text: boolean = false;

  clockCycleT1FormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);
  clockCycleT2FormControl = new FormControl(0, [Validators.required, rangeValidator(2, 240)]);

  matcher = new AppErrorStateMatcher();

  private offsetClockXChanged$ = new BehaviorSubject(this.offsetClockX);
  private offsetClockYChanged$ = new BehaviorSubject(this.offsetClockY);
  private offsetTempXChanged$ = new BehaviorSubject(this.offsetTempX);
  private offsetTempYChanged$ = new BehaviorSubject(this.offsetTempY);
  private offsetCalendarXChanged$ = new BehaviorSubject(this.offsetCalendarX);
  private offsetCalendarYChanged$ = new BehaviorSubject(this.offsetCalendarY);
  private offsetTextYChanged$ = new BehaviorSubject(this.offsetTextY);

  private calcDelayedTimer: any;

  hoursFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 23)]);
  minutesFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 59)]);
  temperatureFormControl = new FormControl(0, [Validators.required, rangeValidator(-40, 40)]);

  daysFormControl = new FormControl(1, [Validators.required, rangeValidator(1, 31)]);
  monthFormControl = new FormControl(1, [Validators.required, rangeValidator(1, 12)]);
  yearFormControl = new FormControl(1900, [Validators.required, rangeValidator(1900, 2100)]);

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService,
    private httpClient: HttpClient) {
    super();
    this.calendarWidthList.push(new ComboBoxItem(this.L.$('Не показывать'), 0));
    this.calendarWidthList.push(new ComboBoxItem(this.L.$('2 цифры'), 2));
    this.calendarWidthList.push(new ComboBoxItem(this.L.$('4 цифры'), 4));
    this.calendarLinesList.push(new ComboBoxItem(this.L.$('1 строка'), 1));
    this.calendarLinesList.push(new ComboBoxItem(this.L.$('2 строки'), 2));
  }

  ngOnInit() {
    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в экране
          let request = 'W|H|WZ|CO|CK|CF|CX|CY|CXT|CYT|CXC|CYC|CYL|CSV|CSA|HTR|CD|CS|CT1|CT2|CF1|CF2|DBG';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'CD':
              this.dotWidth = this.managementService.state.clock_dot_width;
              break;
            case 'CS':
              this.dotSpace = this.managementService.state.clock_dot_space;
              break;
            case 'CO':
              this.clock_orientation = this.managementService.state.clock_orientation;
              this.checkAllowDotWidthAndDotSpace();
              break;
            case 'CF':
              this.calendarWidth = this.managementService.state.calendar_width & 0x0F;
              this.calendarLines = (this.managementService.state.calendar_width & 0xF0) >> 4;
              break;
            case 'CK':
              this.clock_size = this.managementService.state.clock_size;
              this.checkAllowDotWidthAndDotSpace();
              break;
            case 'CX':
              this.offsetClockX = this.managementService.state.clock_offset_x;
              this.offsetClockXinitialized = true;
              break;
            case 'CY':
              this.offsetClockY = this.managementService.state.clock_offset_y;
              this.offsetClockYinitialized = true;
              break;
            case 'CXT':
              this.offsetTempX = this.managementService.state.temp_offset_x;
              this.offsetTempXinitialized = true;
              break;
            case 'CYT':
              this.offsetTempY = this.managementService.state.temp_offset_y;
              this.offsetTempYinitialized = true;
              break;
            case 'CXC':
              this.offsetCalendarX = this.managementService.state.calendar_offset_x;
              this.offsetCalendarXinitialized = true;
              break;
            case 'CYC':
              this.offsetCalendarY = this.managementService.state.calendar_offset_y;
              this.offsetCalendarYinitialized = true;
              break;
            case 'CYL':
              this.offsetTextY = this.managementService.state.text_offset_y;
              this.offsetTextYinitialized = true;
              break;
            case 'CSV':
              this.calendar_show_variant = this.managementService.state.clock_show_variant & 0x03;
              this.temp_show_variant = (this.managementService.state.clock_show_variant >> 2) & 0x03;
              break;
            case 'CSA':
              this.clock_show_alignment = this.managementService.state.clock_show_alignment & 0x03;
              this.calendar_show_alignment = (this.managementService.state.clock_show_alignment >> 2) & 0x03;
              this.temp_show_alignment = (this.managementService.state.clock_show_alignment >> 4) & 0x03;
              break;
            case 'HTR':
              this.hide_clock_on_text = (this.managementService.state.hide_on_text_running & 0x01) != 0;
              this.hide_calendar_on_text = (this.managementService.state.hide_on_text_running & 0x02) != 0;
              this.hide_temp_on_text = (this.managementService.state.hide_on_text_running & 0x04) != 0;
              break;
            case 'CT1':
              this.clockCycleT2FormControl.setValue(this.managementService.state.clock_cycle_T1);
              break;
            case 'CT2':
              this.clockCycleT1FormControl.setValue(this.managementService.state.clock_cycle_T2);
              break;
            case 'CF1':
              this.clock_t1    = (this.managementService.state.clock_cycle_F1 & 0x01) > 0;
              this.clock_t2    = (this.managementService.state.clock_cycle_F1 & 0x02) > 0;
              this.clock_t3    = (this.managementService.state.clock_cycle_F1 & 0x04) > 0;
              this.clock_t4    = (this.managementService.state.clock_cycle_F1 & 0x08) > 0;
              this.calendar_t1 = (this.managementService.state.clock_cycle_F1 & 0x10) > 0;
              this.calendar_t2 = (this.managementService.state.clock_cycle_F1 & 0x20) > 0;
              this.calendar_t3 = (this.managementService.state.clock_cycle_F1 & 0x40) > 0;
              this.calendar_t4 = (this.managementService.state.clock_cycle_F1 & 0x80) > 0;
              break;
            case 'CF2':
              this.temp_t1 = (this.managementService.state.clock_cycle_F2 & 0x01) > 0;
              this.temp_t2 = (this.managementService.state.clock_cycle_F2 & 0x02) > 0;
              this.temp_t3 = (this.managementService.state.clock_cycle_F2 & 0x04) > 0;
              this.temp_t4 = (this.managementService.state.clock_cycle_F2 & 0x08) > 0;
              break;
            case 'WZ':
              this.supportWeather = this.managementService.state.supportWeather;
              break;
            case 'DBG':
              if (!this.managementService.state.debug) {
                this.managementService.state.debug_frame = false;
                this.managementService.state.debug_cross = false;
              }
              break;
          }
        }
      });

    this.hoursFormControl.setValue(this.managementService.state.debug_hour);
    this.minutesFormControl.setValue(this.managementService.state.debug_minutes);
    this.temperatureFormControl.setValue(this.managementService.state.debug_temperature);

    this.daysFormControl.setValue(this.managementService.state.debug_day);
    this.monthFormControl.setValue(this.managementService.state.debug_month);
    this.yearFormControl.setValue(this.managementService.state.debug_year);

    this.offsetClockXChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetClockXinitialized) {
          this.offsetClockX = value;
          this.managementService.state.clock_offset_x = this.offsetClockX;
          // $19 21 X Y; - Смещение часов по X,Y
          this.socketService.sendText(`$19 21 ${this.offsetClockX} ${this.offsetClockY};`);
        }
      });

    this.offsetClockYChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetClockYinitialized) {
          this.offsetClockY = value;
          this.managementService.state.clock_offset_y = this.offsetClockY;
          // $19 21 X Y; - Смещение часов по X,Y
          this.socketService.sendText(`$19 21 ${this.offsetClockX} ${this.offsetClockY};`);
        }
      });

    this.offsetTempXChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetTempXinitialized) {
          this.offsetTempX = value;
          this.managementService.state.temp_offset_x = this.offsetTempX;
          // $19 23 X Y; - Смещение температуры по X,Y
          this.socketService.sendText(`$19 23 ${this.offsetTempX} ${this.offsetTempY};`);
        }
      });

    this.offsetTempYChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetTempYinitialized) {
          this.offsetTempY = value;
          this.managementService.state.temp_offset_y = this.offsetTempY;
          // $19 23 X Y; - Смещение температуры по X,Y
          this.socketService.sendText(`$19 23 ${this.offsetTempX} ${this.offsetTempY};`);
        }
      });

    this.offsetCalendarXChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetCalendarXinitialized) {
          this.offsetCalendarX = value;
          this.managementService.state.calendar_offset_x = this.offsetCalendarX;
          // $19 24 X Y; - Смещение календаря по X,Y
          this.socketService.sendText(`$19 24 ${this.offsetCalendarX} ${this.offsetCalendarY};`);
        }
      });

    this.offsetCalendarYChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetCalendarYinitialized) {
          this.offsetCalendarY = value;
          this.managementService.state.calendar_offset_y = this.offsetCalendarY;
          // $19 24 X Y; - Смещение календаря по X,Y
          this.socketService.sendText(`$19 24 ${this.offsetCalendarX} ${this.offsetCalendarY};`);
        }
      });

    this.offsetTextYChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.offsetTextYinitialized) {
          this.offsetTextY = value;
          this.managementService.state.text_offset_y = this.offsetTextY;
          // $19 25 Y; - Смещение бегущей строки по Y
          this.socketService.sendText(`$19 25 ${this.offsetTextY};`);
        }
      });
  }

  checkAllowDotWidthAndDotSpace() {
    // Пересчет параметров зависит от нескольких значений, которые поступают пачкой последовательно
    // Не нужно выполнять пересчет на каждый поступивший параметр.
    // Ждем, пока поступит последний и через xxx мс выполняем расчет
    if (this.calcDelayedTimer) clearTimeout(this.calcDelayedTimer);
    this.calcDelayedTimer = setTimeout(() => {
      this.doAllowDotWidthAndDotSpace();
      this.calcDelayedTimer = undefined;
    }, 500);
  }

  doAllowDotWidthAndDotSpace() {
    const smallClock = this.getClockSizeType() == 1;
    this.allowDotWidthAndSpace = !smallClock;
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

  formatLabel(value: number) {
    return value.toString();
  }

  changeYearWidth(value: number) {
    this.calendarWidth = value;
    value = this.calendarWidth | this.calendarLines << 4;
    this.socketService.sendText(`$19 13 ${value};`);
  }

  changeYearLines(value: number) {
    this.calendarLines = value;
    if (this.calendarLines == 2) this.calendarWidth = 4;
    value = this.calendarWidth | this.calendarLines << 4;
    this.socketService.sendText(`$19 13 ${value};`);
  }

  offsetClockXChanged(value: number) {
    this.offsetClockXChanged$.next(value);
  }

  offsetClockYChanged(value: number) {
    this.offsetClockYChanged$.next(value);
  }

  offsetTempXChanged(value: number) {
    this.offsetTempXChanged$.next(value);
  }

  offsetTempYChanged(value: number) {
    this.offsetTempYChanged$.next(value);
  }

  offsetCalendarXChanged(value: number) {
    this.offsetCalendarXChanged$.next(value);
  }

  offsetCalendarYChanged(value: number) {
    this.offsetCalendarYChanged$.next(value);
  }

  offsetTextYChanged(value: number) {
    this.offsetTextYChanged$.next(value);
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

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  isValid(): boolean {
    return this.clockCycleT1FormControl.valid && this.clockCycleT2FormControl.valid;
  }

  applyOptions($event: MouseEvent) {
    // $19 9 X Y;    - Флаги отображения часов/календаря/температуры в циклах T1-T4
    // $19 17 T1 T2; - Продолжительность отображения даты / часов в секундах - время интервалов фазы T1/T3 и T2/T4
    // $19 18 X;     - флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру b3b4b5b6b7 - зарезервировано

    // Флаги отображения часов / календаря / температуры в циклах
    const F1 = (this.clock_t1 ? 1 : 0) + (this.clock_t2 ? 2 : 0) + (this.clock_t3 ? 4 : 0) + (this.clock_t4 ? 8 : 0) +
      (this.calendar_t1 ? 16 : 0) + (this.calendar_t2 ? 32 : 0)  + (this.calendar_t3 ? 64 : 0) + (this.calendar_t4 ? 128 : 0);
    const F2 = (this.temp_t1 ? 1 : 0) + (this.temp_t2 ? 2 : 0) + (this.temp_t3 ? 4 : 0) + (this.temp_t4 ? 8 : 0);
    this.managementService.state.clock_cycle_F1 = F1;
    this.managementService.state.clock_cycle_F2 = F2;
    this.socketService.sendText(`$19 9 ${F1} ${F2};`);

    // Время фаз цикла T1/T2 и T2/T4
    const T1 = this.managementService.state.clock_cycle_T1 = this.clockCycleT2FormControl.value as number;
    const T2 = this.managementService.state.clock_cycle_T2 = this.clockCycleT1FormControl.value as number;
    this.clockCycleT2FormControl.setValue(this.managementService.state.clock_cycle_T1);
    this.clockCycleT1FormControl.setValue(this.managementService.state.clock_cycle_T2);
    this.socketService.sendText(`$19 17 ${T1} ${T2};`);

    // Флаги сокрытия часов/календаря/температуры во время показа бегущей строки
    const HTR = (this.hide_clock_on_text ? 1 : 0) + (this.hide_calendar_on_text ? 2 : 0) + (this.hide_temp_on_text ? 4 : 0);
    this.managementService.state.hide_on_text_running = HTR;
    this.socketService.sendText(`$19 18 ${HTR};`);
  }

  changeShowVariant() {
    // $19 16 X;     - флаги вариантов отображения календаря и температуры - относительно часов или произвольно по указанному смещению
    //  - флаги b7b6b5b4b3b2b1b0 - b1b0 - отображения календаря 00 - в позиции часов, 11 - произвольно; 01 и 10 - зарезервировано
    //                             b3b2 - отображение температуры 00 - под часами, 01 - справа от часов; 10 - зарезервировано; 11 - произвольно
    const value = this.calendar_show_variant + this.temp_show_variant * 4;
    this.managementService.state.clock_show_variant = value;
    this.socketService.sendText(`$19 16 ${value};`);
  }

  changeAlignment() {
    // $19 15 X;     - флаги выравнивани часов/календаря/температуры в области их отображения;
    //  - флаги b7b6b5b4b3b2b1b0 - b1b0 - выравнивание часов       00 - центр 01 - лево 10 - право 11 - зарезервировано
    //                           - b3b2 - выравнивание календаря   00 - центр 01 - лево 10 - право 11 - зарезервировано
    //                           - b5b4 - выравнивание температуры 00 - центр 01 - лево 10 - право 11 - зарезервировано
    //                           - b7b6 - зарезервировано
    const value = this.clock_show_alignment + this.calendar_show_alignment * 4 + this.temp_show_alignment * 16;
    this.managementService.state.clock_show_alignment = value;
    this.socketService.sendText(`$19 15 ${value};`);
  }

  send() {
    // $18 1 HH MM TT DD MN YYYY; - Установить указанное время HH:MM и температуру TT, DD.MN.YYYY - День, месяц, год - для отладки позиционирования часов с температурой
    const HH = this.managementService.state.debug_hour = this.hoursFormControl.value ?? 0;
    const MM = this.managementService.state.debug_minutes = this.minutesFormControl.value ?? 0;
    const TT = this.managementService.state.debug_temperature = this.temperatureFormControl.value ?? 0;
    const DD = this.managementService.state.debug_day = this.daysFormControl.value ?? 0;
    const MN = this.managementService.state.debug_month = this.monthFormControl.value ?? 0;
    const YY = this.managementService.state.debug_year = this.yearFormControl.value ?? 0;
    this.socketService.sendText(`$18 1 ${HH} ${MM} ${TT} ${DD} ${MN} ${YY};`);
  }

  left() {
    // $18 3 X; - Сдвиг позиции вывода часов при скроллинге на X колонок
    this.socketService.sendText(`$18 3 -1;`);
  }

  right() {
    // $18 3 X; - Сдвиг позиции вывода часов при скроллинге на X колонок
    this.socketService.sendText(`$18 3 1;`);
  }

  reset() {
    // $18 4; - Сброс позиции вывода часов при скроллинге на X колонок
    this.socketService.sendText(`$18 4;`);
  }

  debugOnOf(checked: boolean) {
    this.managementService.state.debug = checked;
    if (!checked) {
      // $18 1 HH MM TT DD MN YYYY; - Установить указанное время HH:MM и температуру TT, DD.MN.YYYY - День, месяц, год - для отладки позиционирования часов с температурой
      // Для отключения - отправить значения -1
      this.managementService.state.debug_frame = false;
      this.managementService.state.debug_cross = false;
      this.socketService.sendText('$18 5 0');
      this.socketService.sendText('$18 2 0');
      this.socketService.sendText('$18 1 -1 -1 -1 -1 -1 -1;');
    } else {
      // Отправить текущие значения параметров отладки
      this.send();
    }
  }

  frameOnOf(checked: boolean) {
    this.managementService.state.debug_frame = checked;
    // $18 5 X; - Вкл/выкл отображение рамки области
    this.socketService.sendText(`$18 5 ${checked ? 1 : 0};`);
  }

  crossOnOf(checked: boolean) {
    this.managementService.state.debug_cross = checked;
    // $18 2 X; - $18 2 X; - Вкл/выкл отображение креста
    this.socketService.sendText(`$18 2 ${checked ? 1 : 0};`);
  }

  isDebugValid() {
    return this.hoursFormControl.valid && this.minutesFormControl.valid && this.temperatureFormControl.valid &&
      this.daysFormControl.valid && this.monthFormControl.valid && this.yearFormControl.valid;
  }

  isDebugOn()  {
    return this.managementService.state.debug;
  }

}
