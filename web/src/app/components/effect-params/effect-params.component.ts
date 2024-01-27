import {Component, Inject, OnInit} from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import { MatSlideToggleChange, MatSlideToggleModule } from '@angular/material/slide-toggle';
import {BehaviorSubject, debounceTime, Subject, takeUntil} from 'rxjs';
import {ComboBoxItem} from '../../models/combo-box.model';
import {EffectModel} from '../../models/effect.model';
import {isNullOrUndefined, isNullOrUndefinedOrEmpty} from '../../services/helper';
import {LanguagesService} from '../../services/languages/languages.service';
import {ManagementService} from '../../services/management/management.service';
import {WebsocketService} from '../../services/websocket/websocket.service';
import { MatButtonModule } from '@angular/material/button';
import { MatOptionModule } from '@angular/material/core';
import { MatSelectModule } from '@angular/material/select';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSliderModule } from '@angular/material/slider';
import { MatTooltipModule } from '@angular/material/tooltip';
import { FormsModule } from '@angular/forms';
import { CdkDrag, CdkDragHandle } from '@angular/cdk/drag-drop';
import {Base} from "../base.class";

export enum ControlType {
  NONE,
  SLIDER,
  CHECKBOX,
  COMBOBOX
}

interface EffectParams {
  id: number,         // ID эффекта
  name: string,       // Название эффекта (справочно)
  label1: string      // Имя параметра 1
  label2: string      // Имя параметра 2
  tooltip1: string;   // Подсказка к параметру эффекта 1
  tooltip2: string;   // Подсказка к параметру эффекта 1
}

@Component({
    selector: 'app-effect-params',
    templateUrl: './effect-params.component.html',
    styleUrls: ['./effect-params.component.scss'],
    standalone: true,
    imports: [MatDialogModule, CdkDrag, CdkDragHandle, MatSlideToggleModule, FormsModule, MatTooltipModule, MatSliderModule, MatFormFieldModule, MatSelectModule, MatOptionModule, MatButtonModule]
})
export class EffectParamsComponent extends Base implements OnInit {

  controlType = ControlType;

  public tooltipContrast = '';
  public tooltipSpeed = '';
  public tooltipParam1 = '';
  public tooltipParam2 = '';
  public labelParam1 = '';
  public labelParam2 = '';
  public speedTitle = '';

  public model: EffectModel = new EffectModel();
  public isLoaded = false;

  public allowClockVisible = false;
  public allowTextVisible = false;
  public contrastVisible = false;
  public speedVisible = false;
  public param1Visible = false;
  public param2Visible = false;

  public param2Control: ControlType = ControlType.NONE;
  public param2Flag = false;
  public param2List: Array<ComboBoxItem> = [];

  private contrastChanged$ = new BehaviorSubject(this.model.contrast);
  private speedChanged$ = new BehaviorSubject(this.model.speed);
  private param1Changed$ = new BehaviorSubject(this.model.param1);
  private param2Changed$ = new BehaviorSubject(this.model.param1);

  // Имя эффекта тут справочно - чтобы понимать к какому эффекту относится наименование параметров
  // На самом деле имя эффекта передается с контроллера
  // А вот наименования параметров 1 и 2, а также tooltip Для них  - задаются ниже в массиве
  //@formatter:off
  effectParams: EffectParams[] = [
    {id:  0, name: 'Часы',            label1: 'Цвет часов',    label2: '',         tooltip1: 'ef_00_tt_1', tooltip2: ''          },
    {id:  1, name: 'Лампа',           label1: 'Насыщенность',  label2: '',         tooltip1: 'ef_01_tt_1', tooltip2: ''          },
    {id:  2, name: 'Снегопад',        label1: 'Плотность',     label2: '',         tooltip1: 'ef_02_tt_1', tooltip2: ''          },
    {id:  3, name: 'Кубик',           label1: 'Размер',        label2: '',         tooltip1: 'ef_03_tt_1', tooltip2: ''          },
    {id:  4, name: 'Радуга',          label1: 'Ширина',        label2: 'Вариант',  tooltip1: 'ef_04_tt_1', tooltip2: 'ef_04_tt_2'},
    {id:  5, name: 'Пейнтбол',        label1: 'Плотность',     label2: 'Сегменты', tooltip1: 'ef_05_tt_1', tooltip2: 'ef_SG_tt_2'},
    {id:  6, name: 'Огонь',           label1: 'Цвет',          label2: '',         tooltip1: 'ef_06_tt_1', tooltip2: ''          },
    {id:  7, name: 'The Matrix',      label1: 'Плотность',     label2: '',         tooltip1: 'ef_07_tt_1', tooltip2: ''          },
    {id:  8, name: 'Шарики',          label1: 'Количество',    label2: 'Хвост',    tooltip1: 'ef_08_tt_1', tooltip2: 'ef_08_tt_2'},
    {id:  9, name: 'Звездопад',       label1: 'Плотность',     label2: '',         tooltip1: 'ef_09_tt_1', tooltip2: ''          },
    {id: 10, name: 'Конфетти',        label1: 'Плотность',     label2: '',         tooltip1: 'ef_10_tt_1', tooltip2: ''          },
    {id: 11, name: 'Цветной шум',     label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 12, name: 'Облака',          label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 13, name: 'Лава',            label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 14, name: 'Плазма',          label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 15, name: 'Бензин на воде',  label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 16, name: 'Павлин',          label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 17, name: 'Зебра',           label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 18, name: 'Шумящий лес',     label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 19, name: 'Морской прибой',  label1: 'Размер пятна',  label2: '',         tooltip1: 'ef_NZ_tt_1', tooltip2: ''          },
    {id: 20, name: 'Смена цвета',     label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 21, name: 'Светлячки',       label1: 'Количество',    label2: '',         tooltip1: 'ef_20_tt_1', tooltip2: ''          },
    {id: 22, name: 'Водоворот',       label1: '',              label2: 'Сегменты', tooltip1: '',           tooltip2: 'ef_SG_tt_2'},
    {id: 23, name: 'Циклон',          label1: 'Затухание',     label2: 'Сегменты', tooltip1: 'ef_23_tt_1', tooltip2: 'ef_SG_tt_2'},
    {id: 24, name: 'Мерцание',        label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 25, name: 'Северное сияние', label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 26, name: 'Тени',            label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 27, name: 'Лабиринт',        label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 28, name: 'Змейка',          label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 29, name: 'Тетрис',          label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 30, name: 'Арканоид',        label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 31, name: 'Палитра',         label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 32, name: 'Спектрум',        label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 33, name: 'Синусы',          label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 34, name: 'Вышиванка',       label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 35, name: 'Дождь',           label1: 'Динамика',      label2: '',         tooltip1: 'ef_35_tt_1', tooltip2: ''          },
    {id: 36, name: 'Камин',           label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 37, name: 'Стрелки',         label1: '',              label2: 'Вариант',  tooltip1: '',           tooltip2: 'ef_37_tt_2'},
    {id: 38, name: 'Узоры',           label1: 'Смещение',      label2: 'Узор',     tooltip1: 'ef_38_tt_1', tooltip2: 'ef_38_tt_2'},
    {id: 39, name: 'Рубик',           label1: 'Размер',        label2: 'Вариант',  tooltip1: 'ef_39_tt_1', tooltip2: 'ef_39_tt_2'},
    {id: 40, name: 'Звёзды',          label1: 'Цвет',          label2: 'Вариант',  tooltip1: 'ef_40_tt_1', tooltip2: 'ef_40_tt_2'},
    {id: 41, name: 'Штора',           label1: 'Цвет',          label2: 'Вариант',  tooltip1: 'ef_41_tt_1', tooltip2: 'ef_41_tt_2'},
    {id: 42, name: 'Трафик',          label1: 'Плотность',     label2: 'Цвета',    tooltip1: 'ef_42_tt_1', tooltip2: 'ef_42_tt_2'},
    {id: 43, name: 'Рассвет',         label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 44, name: 'Анимация',        label1: '',              label2: 'Анимация', tooltip1: '',           tooltip2: 'ef_44_tt_2'},
    {id: 45, name: 'Погода',          label1: '',              label2: '',         tooltip1: '',           tooltip2: ''          },
    {id: 46, name: 'Слайды',          label1: 'Слайд',         label2: '',         tooltip1: 'ef_46_tt_1', tooltip2: ''          },
    {id: 47, name: 'SD-Карта',        label1: '',              label2: 'Ролик',    tooltip1: '',           tooltip2: 'ef_47_tt_2'},
  ];
  //@formatter:on

  constructor(public dialogRef: MatDialogRef<EffectParamsComponent>,
              public socketService: WebsocketService,
              public managementService: ManagementService,
              public L: LanguagesService,
              @Inject(MAT_DIALOG_DATA) private data: any) {
    super();
    this.model = data.model;

    this.tooltipContrast = L.$('Контрастность эффекта.');
    switch (this.model.id) {
      case  1: this.tooltipSpeed = L.$('Цвет лампы.'); break;                            // Лампа
      case 20: this.tooltipSpeed = L.$('Скорость смены цвета.'); break;                  // Смена цвета
      case 24: this.tooltipSpeed = L.$('ef_24_tt_1'); break;                             // Мерцание
      case 31: this.tooltipSpeed = L.$('ef_31_tt_1'); break;                             // Палитра
      case 45: this.tooltipSpeed = L.$('ef_45_tt_1'); break;                             // Погода
      default: this.tooltipSpeed = L.$('Скорость воспроизведения эффекта.'); break;      // Все остальные эффекты
    }

    // Для режима "Лампа" (id == 1) ползунок "Скорость" регулирует цвет
    switch (this.model.id) {
      case  1: this.speedTitle = L.$('Цвет'); break;                                    // Лампа
      case 24: this.speedTitle = L.$('Динамика'); break;                                // Мерцание
      case 31: this.speedTitle = L.$('Динамика'); break;                                // Палитра
      case 45: this.speedTitle = L.$('Дыхание'); break;                                 // Погода
      default: this.speedTitle = L.$('Скорость'); break;                                // Все остальные эффекты
    }

    const description = this.effectParams.find(el => el.id === this.model.id);
    if (!isNullOrUndefined(description)) {
      this.labelParam1   = L.$(description!.label1);
      this.labelParam2   = L.$(description!.label2);
      this.tooltipParam1 = L.$(description!.tooltip1);
      this.tooltipParam2 = L.$(description!.tooltip2);
    } else {
      this.labelParam1   = this.model.paramName1;
      this.labelParam2   = this.model.paramName2;
      this.tooltipParam1 = L.$('Вариант отображения эффекта.');
      this.tooltipParam2 = L.$('Вариант отображения эффекта.');
    }
  }

  ngOnInit(): void {
    this.dialogRef.disableClose = true;
    this.managementService.edit$
      .pipe(takeUntil(this.destroy$))
      .subscribe((data: any) => {
        if (!isNullOrUndefined(data) && data['act'].toString() === 'EDIT' && Number(data['id']) === this.model.id) {
          // {
          //    "act":"EDIT",
          //    "id": "1",
          //    "name": "Лампа",
          //    "usage": "true",
          //    "allowClock": "true",
          //    "allowText": "true",
          //    "speed": "122",    // "X" - не используется;
          //    "contrast": "86",  // "X" - не используется;
          //    "param1": "4",     // "X" - не используется;
          //    "param2": ""       // "X" - не используется; "L>val>item1,item2,..itemN" - список, где val - текущее, далее список; "C>x>title" - чекбокс, где x=0 - выкл, x=1 - вкл; title - текст чекбокса
          //    "paramName1": "Вариант",
          //    "paramName2": "Выбор",
          // }

          this.allowClockVisible = this.model.id !== 0;        // 0 - эффект "Часы" - галка "Часы поверх эффекта" недоступны
          this.allowTextVisible  = data['allowText'] !== 'X';
          this.contrastVisible   = data['contrast']  !== 'X';
          this.speedVisible      = data['speed']     !== 'X';
          this.param1Visible     = data['param1']    !== 'X';
          this.param2Visible     = data['param2']    !== 'X';

          this.model.name        = this.L.$(data['name'] as string);
          this.model.allowClock  = data['allowClock'] === '1';
          this.model.allowText   = data['allowText'] === '1';
          this.model.contrast    = this.contrastVisible ? Number(data['contrast']) : -1;
          this.model.speed       = this.speedVisible ? Number(data['speed']) : -1;
          this.model.param1      = this.param1Visible ? Number(data['param1']) : -1;
          this.model.param2      = this.param2Visible ? Number(data['param2']) : -1;
          this.model.paramName1  = data['paramName1'] as string;
          this.model.paramName2  = data['paramName2'] as string;
          this.model.order       = Number(data['order']);

          if (isNaN(this.model.order)) this.model.order = -1;

          const param2 = data['param2'] as string ?? this.managementService.state.param2;

          if (!isNullOrUndefined(param2)) {
            // @formatter:off
            if (param2 === 'X')          this.param2Control = ControlType.NONE;     else
            if (param2.startsWith('L>')) this.param2Control = ControlType.COMBOBOX; else
            if (param2.startsWith('C>')) this.param2Control = ControlType.CHECKBOX; else
                                         this.param2Control = ControlType.SLIDER;
            // @formatter:on

            switch (this.param2Control) {
              case ControlType.SLIDER: {
                this.model.param2 = this.param2Visible ? Number(param2) : -1;
                break;
              }

              case ControlType.CHECKBOX: {
                const parts = param2.split('>');
                this.param2Flag = Number(parts[1]) === 1;                                         // 0 -false; 1 - true
                this.model.paramName2 = parts[2] as string;
                break;
              }

              case ControlType.COMBOBOX: {
                const parts = param2.split('>');
                this.model.param2 = this.param2Visible ? Number(parts[1]) : -1;                   // 0 -false; 1 - true
                let list = parts[2] as string;
                while (list.endsWith(',')) list = list.slice(0, -1);
                const parts2 = list.split(',');
                this.param2List = new Array<ComboBoxItem>();
                for (let i = 0; i < parts2.length; i++) {
                  this.param2List.push(new ComboBoxItem(parts2[i], i));
                }
                break;
              }
            }
          }
          this.managementService.state.param2 = '';

          setTimeout(() => { this.isLoaded = true; }, 500);
        }
      });

    this.contrastChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.isLoaded) {
          this.model.contrast = value;
          // $8 6 N D; D -> контрастность эффекта N;
          this.socketService.sendText(`$8 6 ${this.model.id} ${this.model.contrast};`);
        }
      });

    this.speedChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.isLoaded) {
          this.model.speed = value;
          // $8 8 N D; D -> скорость эффекта N;
          this.socketService.sendText(`$8 8 ${this.model.id} ${this.model.speed};`);
        }
      });

    this.param1Changed$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.isLoaded) {
          this.model.param1 = value;
          // $8 1 N D; D -> параметр #1 для эффекта N;
          this.socketService.sendText(`$8 1 ${this.model.id} ${this.model.param1};`);
        }
      });

    this.param2Changed$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (this.isLoaded) {
          this.model.param2 = value;
          // $8 3 N D; D -> параметр #2 для эффекта N;
          this.socketService.sendText(`$8 3 ${this.model.id} ${this.model.param2};`);
        }
      });

    // Запросить параметры редактируемого эффекта
    const command = `$8 7 ${this.model.id};`;
    this.socketService.sendText(command);
  }

  close($event: MouseEvent) {
    this.dialogRef.close();
  }

  getContrastTitle(): string {
    return this.L.$('Контраст');
  }

  toggleAllowClock(value: MatSlideToggleChange) {
    this.model.allowClock = value.checked;
    // $8 5 N X; вкл/выкл оверлей часов поверх эффекта; N - номер эффекта, X=0 - выкл X=1 - вкл
    this.socketService.sendText(`$8 5 ${this.model.id} ${this.model.allowClock ? '1' : '0'};`);
  }

  toggleAllowText(value: MatSlideToggleChange) {
    this.model.allowText = value.checked;
    // $8 4 N X; вкл/выкл оверлей текста поверх эффекта; N - номер эффекта, X=0 - выкл X=1 - вкл
    this.socketService.sendText(`$8 4 ${this.model.id} ${this.model.allowText ? '1' : '0'};`);
  }

  changeContrastValue(item: number | null) {
    this.contrastChanged$.next(Number(item));
  }

  changeSpeedValue(item: number | null) {
    this.speedChanged$.next(Number(item));
  }

  changeParam1Value(item: number | null) {
    this.param1Changed$.next(Number(item));
  }

  changeParam2SliderValue(item: number | null) {
    this.param2Changed$.next(Number(item));
  }

  changeParam2SelectValue(item: any) {
    this.model.param2 = item.value;
    // $8 3 N D; D -> параметр #2 для эффекта N;
    this.socketService.sendText(`$8 3 ${this.model.id} ${this.model.param2};`);
  }

  changeParam2ToggleValue() {
    this.param2Flag = !this.param2Flag;
    this.model.param2 = this.param2Flag ? 1 : 0;
    // $8 3 N D; D -> параметр #2 для эффекта N;
    this.socketService.sendText(`$8 3 ${this.model.id} ${this.model.param2};`);
  }

}
