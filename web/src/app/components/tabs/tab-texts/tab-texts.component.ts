import {Component, ElementRef, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {BehaviorSubject, debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty, rangeValidator, replaceAll} from "../../../services/helper";
import {distinctUntilChanged} from "rxjs/operators";
import {RGBA} from "ngx-color";
import {ColorPickerComponent} from "../../color-picker/color-picker.component";
import {MatDialog, MatDialogRef} from "@angular/material/dialog";
import {ComboBoxItem} from "../../../models/combo-box.model";
import {isValidDate} from "rxjs/internal/util/isDate";
import { MatDatepickerModule } from '@angular/material/datepicker';
import { MatMenuModule } from '@angular/material/menu';
import { MatIconModule } from '@angular/material/icon';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatSliderModule } from '@angular/material/slider';
import { MatRadioModule } from '@angular/material/radio';
import { MatButtonModule } from '@angular/material/button';
import { NgClass } from '@angular/common';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';

export class TextButtonItem {
  constructor(public label: string, public index: number, public type: number, public text: string) {
  }
}

@Component({
    selector: 'app-tab-texts',
    templateUrl: './tab-texts.component.html',
    styleUrls: ['./tab-texts.component.scss'],
    standalone: true,
    imports: [
        MatSlideToggleModule,
        FormsModule,
        MatFormFieldModule,
        MatInputModule,
        ReactiveFormsModule,
        DisableControlDirective,
        MatButtonModule,
        MatRadioModule,
        MatSliderModule,
        MatTooltipModule,
        NgClass,
        MatIconModule,
        MatMenuModule,
        MatDatepickerModule,
    ],
})
export class TabTextsComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  @ViewChild('text', {static: true}) text!: ElementRef;
  @ViewChild('dateR', {static: true}) dateR!: ElementRef;
  @ViewChild('dateP', {static: true}) dateP!: ElementRef;
  @ViewChild('dateS1', {static: true}) dateS1!: ElementRef;
  @ViewChild('dateS2', {static: true}) dateS2!: ElementRef;

  // @formatter:off
  public text_color_mode = -1;           // CT  - CT:X	режим цвета текстовой строки: 0 - монохром, 1 - радуга, 2 - каждая буква своим цветом
  public text_use_overlay = false;      // TE  - TE:X	оверлей текста бегущей строки вкл/выкл, где Х = 0 - выкл; 1 - вкл (использовать бегущую строку в эффектах)
  public text_scroll_speed = -1;        // ST  - ST:число	скорость смещения бегущей строки
  public text_cells_type = '';           // TS  - TS:строка - строка состояния кнопок выбора текста из массива строк: 36 символов 0..5, где
                                        //       - 0 - серый - пустая
                                        //       - 1 - черный - отключена
                                        //       - 2 - зеленый - активна - просто текст, без макросов
                                        //       - 3 - голубой - активна, содержит макросы кроме даты
                                        //       - 4 - синий - активная, содержит макрос даты
                                        //       - 5 - красный - для строки 0 - это управляющая строка
  public text_edit: string = '';        // TY  - TY:[I:Z > текст] текст для строки, с указанным индексом I 0..35, Z 0..9,A..Z. Ограничители [] обязательны; текст ответа в формате: 'I:Z > текст';
  public text_index: number = -1;
  public text_color: RGBA = {r: 255, g: 255, b: 255, a: 1};
  public sample_color_visible = false;
  // @formatter:on

  public supportWeather: boolean = false;

  public buttons: Array<TextButtonItem> = [];
  public date_fmt: ComboBoxItem[] = [];

  public textIntervalFormControl = new FormControl(0, [Validators.required, rangeValidator(1, 64000)]);
  public matcher = new AppErrorStateMatcher();

  public repeatNum: number = 1;
  public repeatTime: number = 30;

  private colorDialogRef: MatDialogRef<ColorPickerComponent> | null = null;
  private speedChanged$ = new BehaviorSubject(this.text_scroll_speed);
  private macros_text_color: RGBA = {r: 255, g: 255, b: 255, a: 1};
  private macros_back_color: RGBA = {r: 0, g: 0, b: 0, a: 1};
  private dateApplied: boolean = false;

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService,
    private dialog: MatDialog) {

    const str = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    for (let i = 0; i < str.length; i++) {
      this.buttons.push(new TextButtonItem(str[i], i, 0, ''));

      this.buttons[i].text = this.managementService.text_lines[i];
    }

    // @formatter:off
    this.date_fmt.push(new ComboBoxItem('dd.MM.yyyy',          '{D:dd.MM.yyyy}'))
    this.date_fmt.push(new ComboBoxItem('dd MMMM yyyy',        '{D:dd MMMM yyyy}'))
    this.date_fmt.push(new ComboBoxItem('dd.MM.yyyy dddd',     '{D:dd.MM.yyyy dddd}'))
    this.date_fmt.push(new ComboBoxItem('dd.MM.yyyy HH:mm',    '{D:dd.MM.yyyy HH:mm}'))
    this.date_fmt.push(new ComboBoxItem('dd.MM.yyyy HH:mm:ss', '{D:dd.MM.yyyy HH:mm:ss}'))
    this.date_fmt.push(new ComboBoxItem('HH:mm',               '{D:HH:mm}'))
    this.date_fmt.push(new ComboBoxItem('HH:mm:ss',            '{D:HH:mm:ss}'))
    this.date_fmt.push(new ComboBoxItem('hh:mm T',             '{D:hh:mm T}'))
    this.date_fmt.push(new ComboBoxItem('hh:mm TT',            '{D:hh:mm TT}'))
    this.date_fmt.push(new ComboBoxItem('hh:mm t',             '{D:hh:mm t}'))
    this.date_fmt.push(new ComboBoxItem('hh:mm tt',            '{D:hh:mm tt}'))
    // @formatter:on
  }

  ngOnInit() {
    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в экране
          const request = 'CT|TE|TI|ST|TS|C2|MX|WZ';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'CT':
              this.text_color_mode = this.managementService.state.text_color_mode;
              break;
            case 'TE':
              this.text_use_overlay = this.managementService.state.text_use_overlay;
              break;
            case 'TI':
              this.textIntervalFormControl.setValue(this.managementService.state.text_interval);
              break;
            case 'ST':
              this.text_scroll_speed = this.managementService.state.text_scroll_speed;
              break;
            case 'TS':
              this.text_cells_type = this.managementService.state.text_cells_type;
              for (let i = 0; i < this.buttons.length; i++) {
                this.buttons[i].type = (i >= this.text_cells_type.length ? 0 : Number(this.text_cells_type[i]));
              }
              break;
            case 'TY':
              // 12:C > text
              const str = this.managementService.state.text_edit;
              let line_idx = -1;
              let line_text = '';
              if (!isNullOrUndefinedOrEmpty(str)) {
                let idx = str.indexOf(':');
                if (idx > 0) {
                  line_idx = Number(str.substring(0, idx));
                  idx = str.indexOf('>');
                  if (idx > 0) {
                    line_text = str.substring(idx + 1).trim();
                    this.buttons[line_idx].text = line_text;
                  }
                }
                if (this.text_index === line_idx) {
                  this.text_edit = line_text;
                }
              }
              break;
            case 'C2':
              const parts = this.managementService.state.text_color.split(',');
              if (parts.length === 3) {
                this.text_color = {r: Number(parts[0]), g: Number(parts[1]), b: Number(parts[2]), a: 1};
                this.sample_color_visible = true;
              }
              break;
            case 'MX':
              if (this.managementService.state.supportMP3) {
                this.managementService.getKeys('S3');
              }
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
          this.text_scroll_speed = value;
          // $13 13 X;   - скорость прокрутки бегущей строки
          this.socketService.sendText(`$13 13 ${value};`);
        }
      });

  }

  isDisabled(): boolean {
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
  }

  isDisabled2(): boolean {
    return this.isDisabled() || this.text_index < 0;
  }

  formatLabel(value: number) {
    return value.toString();
  }

  toggleTextOverEffects() {
    this.text_use_overlay = !this.text_use_overlay;
    // $13 18 X;    - сохранить настройку X "Текст поверх эффектов"; X=0 - нет, X=1 - да
    this.socketService.sendText(`$13 18 ${this.text_use_overlay ? 1 : 0};`);
  }

  applyInterval($event: MouseEvent) {
    // $13 9 D;    - сохранить настройку D - интервал в секундах отображения бегущей строки
    const TI = this.managementService.state.autochange_time = this.textIntervalFormControl.value as number;
    this.socketService.sendText(`$13 9 ${TI};`);
  }

  setColorMode() {
    // $13 11 X;   - режим цвета бегущей строки X: 0,1,2; 0 - цвет из команды $13 15; 1 - "Радуга по ширине матрицы"; 2 - каждая буква своим цветом.
    const CL = this.managementService.state.text_color_mode = this.text_color_mode;
    this.socketService.sendText(`$13 11 ${CL};`);
  }

  showColorSelector() {
    this.colorDialogRef = this.dialog.open(ColorPickerComponent, {
      panelClass: 'color-dialog-panel',
      data: {color: this.text_color},
    });

    this.colorDialogRef.afterClosed().subscribe((result) => {
      if (result) {
        // $13 15 00FFAA; - цвет текстовой строки для режима "монохромный", сохраняемый в globalTextColor
        this.managementService.state.text_color = result;
        const command = `$13 15 ${result.r.toString(16).padStart(2, '0').toUpperCase()}${result.g.toString(16).padStart(2, '0').toUpperCase()}${result.b.toString(16).padStart(2, '0').toUpperCase()};`;
        this.socketService.sendText(command);
      }
    });
  }

  getTextSampleColor(): string {
    const cl = this.text_color;
    return `#${cl.r.toString(16).padStart(2, '0').toUpperCase()}${cl.g.toString(16).padStart(2, '0').toUpperCase()}${cl.b.toString(16).padStart(2, '0').toUpperCase()}`;
  }

  speedChanged(value: number) {
    if (value < 0) return;
    this.speedChanged$.next(value);
  }

  textAction(i: number) {
    // $13 2 N;    - запросить текст бегущей строки с индексом N 0..35 как есть, без обработки макросов - ответ - параметр "TY"
    this.text_index = i;
    if (this.buttons[i].text.length == 0) {
      this.text_edit = '';
      this.socketService.sendText(`$13 2 ${i};`);
      this.managementService.getKeys("TS");
    } else {
      this.text_edit = this.buttons[i].text;
    }
  }

  getTextButtonTooltip(i: number): string {
    if (this.text_index === i) {
      return this.L.$('Редактируемая строка');
    }
    let text = '';
    switch (this.buttons[i].type) {
      case 0:
        text = this.L.$('Текст не задан');
        break;
      case 1:
        text = this.L.$('Строка отключена');
        break;
      case 2:
        text = this.L.$('Содержит текст без макросов');
        break;
      case 3:
        text = this.L.$('Содержит текст с макросами');
        break;
      case 4:
        text = this.L.$('Содержит текст с макросами даты');
        break;
      case 5:
        text = this.L.$('Содержит управляющую последовательность');
        break;
    }
    if (this.buttons[i].text.length > 0) {
      text += '\n' + this.buttons[i].text;
    }
    return text;
  }

  getTextButtonClass(i: number): string {
    return this.text_index == i ? 'btn btn_6' : `btn btn_${this.buttons[i].type}`;
  }

  saveText() {
    // $6 0|текст  - текст бегущей строки "$6 0|X|text" - X - 0..9,A..Z - индекс строки, text - сохраняемый текст
    const text = replaceAll(this.text.nativeElement.value, '\n', ' ');
    this.socketService.sendText(`$6 0|${this.buttons[this.text_index].label}|${text}`);
    this.buttons[this.text_index].text = text;
    this.managementService.text_lines[this.text_index] = text;
  }

  showText() {
    // $6 14|текст - текст бегущей строки для немедленного отображения без сохранения
    const text = replaceAll(this.text.nativeElement.value, '\n', ' ');
    this.socketService.sendText(`$6 14|${text}`);
  }

  clearText() {
    // $6 14|текст - текст бегущей строки для немедленного отображения без сохранения
    this.text.nativeElement.value = '';
    this.text_edit = '';
  }

  putMacrosInText(macros: string, replace: boolean = false) {
    const pos = this.text.nativeElement.selectionStart;
    const text = this.text.nativeElement.value;
    const text_1 = text.substring(0, pos);
    const text_2 = text.substring(pos + (replace ? macros.length : 0));
    this.text_edit = text_1 + macros + text_2;
    this.text.nativeElement.value = this.text_edit;
    this.text.nativeElement.selectionStart = this.text.nativeElement.selectionEnd = pos + macros.length;
    this.text.nativeElement.focus();
  }

  getColorFromMacros(): RGBA | null {
    const text = this.text.nativeElement.value;
    const pos1 = this.text.nativeElement.selectionStart;
    const pos2 = this.text.nativeElement.selectionEnd;
    const color = text.substring(pos1, pos2)
    if (color.length === 6) {
      const sR = color.substring(0, 2);
      const sG = color.substring(2, 4);
      const sB = color.substring(4, 6);
      const R = parseInt(sR, 16);
      const G = parseInt(sG, 16);
      const B = parseInt(sB, 16);
      if (!(isNaN(R) || isNaN(G) || isNaN(B))) {
        return {r: R, g: G, b: B, a: 1};
      }
    }
    return null;
  }

  macrosTextColor() {
    const clr = this.getColorFromMacros();
    const isFromColor = clr !== null;
    this.colorDialogRef = this.dialog.open(ColorPickerComponent, {
      panelClass: 'color-dialog-panel',
      data: {color: isFromColor ? clr : this.macros_text_color},
    });

    this.colorDialogRef.afterClosed().subscribe((result) => {
      if (result) {
        this.macros_text_color = result;
        const color = `${result.r.toString(16).padStart(2, '0').toUpperCase()}${result.g.toString(16).padStart(2, '0').toUpperCase()}${result.b.toString(16).padStart(2, '0').toUpperCase()}`;
        const macros = `{C#${color}}`;
        this.putMacrosInText(isFromColor ? color : macros, isFromColor);
      }
    });
  }

  macrosBackColor() {
    const clr = this.getColorFromMacros();
    const isFromColor = clr !== null;
    this.colorDialogRef = this.dialog.open(ColorPickerComponent, {
      panelClass: 'color-dialog-panel',
      data: {color: isFromColor ? clr : this.macros_back_color},
    });

    this.colorDialogRef.afterClosed().subscribe((result) => {
      if (result) {
        this.macros_back_color = result;
        const color = `${result.r.toString(16).padStart(2, '0').toUpperCase()}${result.g.toString(16).padStart(2, '0').toUpperCase()}${result.b.toString(16).padStart(2, '0').toUpperCase()}`;
        const macros = `{B#${color}}`;
        this.putMacrosInText(isFromColor ? color : macros, isFromColor);
      }
    });
  }

  putEffect(id: number) {
    this.putMacrosInText(`{E${id}}`);
  }

  putNotifySound(id: number) {
    this.putMacrosInText(`{A${id + 1}+}`);
  }

  macrosWeather() {
    this.putMacrosInText('{WS} {WT}');
  }

  macrosDate() {
    this.putMacrosInText('{D}');
  }

  macrosDisableString() {
    const pos = this.text.nativeElement.selectionStart;
    const text = this.text.nativeElement.value;
    const pos1 = this.text.nativeElement.selectionStart;
    const pos2 = this.text.nativeElement.selectionEnd;
    if (text.length > 0) {
      if (text[0] !== '-' && text.indexOf('{-}') === -1) {
        this.text_edit = '-' + text;
        this.text.nativeElement.value = this.text_edit;
        this.text.nativeElement.selectionStart = pos1 + 1;
        this.text.nativeElement.selectionEnd = pos2 + 1;
      } else {
        if (text[0] === '-') {
          this.text_edit = text.substring(1);
        }
        const pos = text.indexOf('{-}');
        if (pos >= 0) {
          this.text_edit = replaceAll(text, '{-}', '');
        }
      }
    } else {
      this.text_edit = '{-}';
      this.text.nativeElement.value = this.text_edit;
      this.text.nativeElement.selectionStart = this.text.nativeElement.selectionEnd = 3;
    }
    this.text.nativeElement.focus();
  }

  setDateR() {
    if (!this.dateApplied) return;
    const dateR = new Date(this.dateR.nativeElement.value);
    if (isValidDate(dateR)) {
      this.putMacrosInText(`{R${dateR.getDate().toString().padStart(2, '0')}.${(dateR.getMonth() + 1).toString().padStart(2, '0')}.***+ 0:00#N}`);
    }
  }

  setDateP() {
    if (!this.dateApplied) return;
    const dateP = new Date(this.dateP.nativeElement.value);
    if (isValidDate(dateP)) {
      this.putMacrosInText(`{P${dateP.getDate().toString().padStart(2, '0')}.${(dateP.getMonth() + 1).toString().padStart(2, '0')}.${dateP.getFullYear().toString().padStart(4, '0')} 0:00#N#60#60#1234567}`);
    }
  }

  setDateS() {
    if (!this.dateApplied) return;
    const dateS1 = new Date(this.dateS1.nativeElement.value);
    const dateS2 = new Date(this.dateS2.nativeElement.value);
    if (isValidDate(dateS1) && isValidDate(dateS1)) {
      // formatter:off
      this.putMacrosInText(`{S${dateS1.getDate().toString().padStart(2, '0')}.${(dateS1.getMonth() + 1).toString().padStart(2, '0')}.${dateS1.getFullYear().toString().padStart(4, '0')} 0:00#` +
        `${dateS2.getDate().toString().padStart(2, '0')}.${(dateS2.getMonth() + 1).toString().padStart(2, '0')}.${dateS2.getFullYear().toString().padStart(4, '0')} 23:59}`);
      // formatter:oт
    }
  }

  macrosRepeatNum(value: string) {
    this.putMacrosInText(`{N${value}}`);
  }

  macrosRepeatTime(value: string) {
    this.putMacrosInText(`{T${value}}`);
  }

  linkToLine(mark: string) {
    this.putMacrosInText(`{#${mark}}`);
  }

  filterButtons(): Array<TextButtonItem> {
    return this.buttons.filter((s) => s.text.length > 0 && s.index > 0);
  }

  getLineText(index: number) {
    const line = `${this.buttons[index].label}: ${this.buttons[index].text.substring(0, 26)}`;
    return line + (this.buttons[index].text.length > 26 ? '...' : '');
  }

  clearDateApplied() {
    this.dateApplied = false;
  }

  setDateApply() {
    this.dateApplied = true;
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }

}
