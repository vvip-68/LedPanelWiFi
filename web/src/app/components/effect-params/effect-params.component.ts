import {Component, Inject, OnInit} from '@angular/core';
import {MAT_DIALOG_DATA, MatDialogRef} from '@angular/material/dialog';
import {MatSlideToggleChange} from '@angular/material/slide-toggle';
import {Subject, takeUntil} from 'rxjs';
import {ComboBoxItem} from '../../models/combo-box.model';
import {EffectModel} from '../../models/effect.model';
import {isNullOrUndefined} from '../../services/helper';
import {LanguagesService} from '../../services/languages/languages.service';
import {ManagementService} from '../../services/management/management.service';
import {WebsocketService} from '../../services/websocket/websocket.service';

export enum ControlType {
  NONE,
  SLIDER,
  CHECKBOX,
  COMBOBOX
}

@Component({
  selector: 'app-effect-params',
  templateUrl: './effect-params.component.html',
  styleUrls: ['./effect-params.component.scss']
})
export class EffectParamsComponent implements OnInit {

  controlType = ControlType;

  public tooltip1 = '';
  public tooltip2 = '';
  public tooltip3 = '';

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

  private destroy$ = new Subject();

  constructor(public dialogRef: MatDialogRef<EffectParamsComponent>,
              public socketService: WebsocketService,
              public managementService: ManagementService,
              public L: LanguagesService,
              @Inject(MAT_DIALOG_DATA) private data: any) {
    this.model = data.model;

    this.tooltip1 = L.$('Контрастность эффекта.');
    this.tooltip2 = L.$('Скорость воспроизведения эффекта.');
    this.tooltip3 = L.$('Вариант отображения эффекта.');
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

          this.allowClockVisible = this.model.id !== 0;      // 0 - эффект "Часы" - галка "Часы поверх эффекта" недоступны
          this.allowTextVisible = data['allowText'] !== 'X';
          this.contrastVisible = data['contrast'] !== 'X';
          this.speedVisible = data['speed'] !== 'X';
          this.param1Visible = data['param1'] !== 'X';
          this.param2Visible = data['param2'] !== 'X';

          this.model.name = this.L.$(data['name'] as string);
          this.model.allowClock = data['allowClock'] === '1';
          this.model.allowText = data['allowText'] === '1';
          this.model.contrast = this.contrastVisible ? Number(data['contrast']) : -1;
          this.model.speed = this.speedVisible ? Number(data['speed']) : -1;
          this.model.param1 = this.param1Visible ? Number(data['param1']) : -1;
          this.model.param2 = this.param2Visible ? Number(data['param2']) : -1;
          this.model.paramName1 = data['paramName1'] as string;
          this.model.paramName2 = data['paramName2'] as string;
          this.model.order = Number(data['order']);

          if (isNaN(this.model.order)) this.model.order = -1;

          // @formatter:off
          const aType = data['param2'] as string;
          if (aType === 'X')          this.param2Control = ControlType.NONE;     else
          if (aType.startsWith('L>')) this.param2Control = ControlType.COMBOBOX; else
          if (aType.startsWith('C>')) this.param2Control = ControlType.CHECKBOX; else
                                      this.param2Control = ControlType.SLIDER;
          // @formatter:on

          switch (this.param2Control) {
            case ControlType.SLIDER: {
              this.model.param2 = this.param2Visible ? Number(data['param2']) : -1;
              break;
            }

            case ControlType.CHECKBOX: {
              const parts = <string>(data['param2']).split('>');
              this.param2Flag = Number(parts[1]) === 1;                  // 0 -false; 1 - true
              this.model.paramName2 = parts[2] as string;
              break;
            }

            case ControlType.COMBOBOX: {
              const parts = <string>(data['param2']).split('>');
              this.model.param2 = this.param2Visible ? Number(parts[1]) : -1;                   // 0 -false; 1 - true
              const parts2 = (<string>parts[2]).split(',');
              this.param2List = new Array<ComboBoxItem>();
              for (let i = 0; i < parts2.length; i++) {
                this.param2List.push(new ComboBoxItem(parts2[i], i));
              }
              break;
            }
          }

          this.isLoaded = true;
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

  getSpeedTitle(): string {
    // Для режима "Лампа" (id == 1) ползунок "Скорость" регулирует цвет
    return this.model.id === 1
      ? this.L.$('Цвет')
      : this.L.$('Скорость');
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
    this.model.contrast = Number(item);
    // $8 6 N D; D -> контрастность эффекта N;
    this.socketService.sendText(`$8 6 ${this.model.id} ${this.model.contrast};`);
  }

  changeSpeedValue(item: number | null) {
    this.model.speed = Number(item);
    // $8 8 N D; D -> скорость эффекта N;
    this.socketService.sendText(`$8 8 ${this.model.id} ${this.model.speed};`);
  }

  changeParam1Value(item: number | null) {
    this.model.param1 = Number(item);
    // $8 1 N D; D -> параметр #1 для эффекта N;
    this.socketService.sendText(`$8 1 ${this.model.id} ${this.model.param1};`);
  }

  changeParam2SliderValue(item: number | null) {
    this.model.param2 = Number(item);
    // $8 3 N D; D -> параметр #2 для эффекта N;
    this.socketService.sendText(`$8 3 ${this.model.id} ${this.model.param2};`);
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

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
