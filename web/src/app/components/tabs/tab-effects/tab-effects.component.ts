import { DOCUMENT, NgClass } from '@angular/common';
import {Component, Inject, OnDestroy, OnInit, ViewChild} from '@angular/core';
import { MatSlideToggleChange, MatSlideToggleModule } from '@angular/material/slide-toggle';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {ActionModel, ActionType} from '../../../models/action.model';
import {CommonService, MessageType} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {AppErrorStateMatcher, isNullOrUndefined, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import { CdkDragDrop, moveItemInArray, transferArrayItem, CdkDropListGroup, CdkDropList, CdkDrag } from "@angular/cdk/drag-drop";
import {IEffectModel} from "../../../models/effect.model";
import { EffectComponent } from '../../effect/effect.component';
import { MatTabsModule } from '@angular/material/tabs';
import { MatIconModule } from '@angular/material/icon';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatButtonModule } from '@angular/material/button';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { BrightnessSliderComponent } from '../../brightness-slider/brightness-slider.component';
import { ActionComponent } from '../../action/action.component';

@Component({
    selector: 'app-tab-effects',
    templateUrl: './tab-effects.component.html',
    styleUrls: ['./tab-effects.component.scss'],
    standalone: true,
    imports: [
        ActionComponent,
        BrightnessSliderComponent,
        MatFormFieldModule,
        MatInputModule,
        FormsModule,
        ReactiveFormsModule,
        DisableControlDirective,
        MatButtonModule,
        MatSlideToggleModule,
        MatTooltipModule,
        NgClass,
        MatIconModule,
        MatTabsModule,
        EffectComponent,
        CdkDropListGroup,
        CdkDropList,
        CdkDrag,
    ],
})
export class TabEffectsComponent implements OnInit, OnDestroy {

  // @ts-ignore
  @ViewChild('input2') input2: ElementRef;

  // @formatter:off
  public power    : ActionModel;
  public bright_5 : ActionModel;
  public bright_25: ActionModel;
  public bright_50: ActionModel;
  public bright_75: ActionModel;
  public bright_100: ActionModel;
  // @formatter:on

  effectTimeFormControl = new FormControl(5, [Validators.required, rangeValidator(5, 255)]);
  autoModeTimeFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 255)]);
  matcher = new AppErrorStateMatcher();

  private e131_mode: number = -1;
  private e131_type: number = -1;
  private e131_group: number = -1;
  private e131_streaming: boolean | undefined = undefined;

  private destroy$ = new Subject();

  constructor(
    @Inject(DOCUMENT) private document: Document,
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService
  ) {
    function checkRange(val: any, min: number, max: number): boolean {
      let res = false;
      if (typeof val === 'number') {
        const v = Number(val);
        return v >= min && v <= max;
      }
      return res;
    }

    this.power = new ActionModel({
      mode: ActionType.POWER,
      active: (value: boolean) => value,
      set: (value: any) => (value ? '$1 0;' : '$1 1;'), // ВЫКЛ / ВКЛ
    });

    this.bright_5 = new ActionModel({
      mode: ActionType.BRIGHT,
      icon: 'bright_5',
      active: (value: number) => checkRange(value, 0, 13),
      set: '$4 0 13;',
    });
    this.bright_25 = new ActionModel({
      mode: ActionType.BRIGHT,
      icon: 'bright_25',
      active: (value: number) => checkRange(value, 14, 64),
      set: '$4 0 64;',
    });
    this.bright_50 = new ActionModel({
      mode: ActionType.BRIGHT,
      icon: 'bright_50',
      active: (value: number) => checkRange(value, 65, 128),
      set: '$4 0 128;',
    });
    this.bright_75 = new ActionModel({
      mode: ActionType.BRIGHT,
      icon: 'bright_75',
      active: (value: number) => checkRange(value, 129, 192),
      set: '$4 0 192;',
    });
    this.bright_100 = new ActionModel({
      mode: ActionType.BRIGHT,
      icon: 'bright_100',
      active: (value: number) => checkRange(value, 193, 255),
      set: '$4 0 255',
    });
  }

  ngOnInit() {
    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в главном экране
          const request = 'E1|E2|E3|E4|PS|BR|DM|RM|FV|PD|IT|EF|EN';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'E1':
              this.e131_mode = this.managementService.state.e131_mode;
              break;
            case 'E2':
              this.e131_type = this.managementService.state.e131_type;
              break;
            case 'E3':
              this.e131_group = this.managementService.state.e131_group;
              break;
            case 'E4':
                this.e131_streaming = this.managementService.state.e131_streaming === true;
                break;
            case 'EF':
              this.managementService.effects.forEach((effect) => {
                effect.active = effect.id == this.managementService.state.effect;
              });
              break;
            case 'EN':
              this.showEffectInformation(5000);
              break;
            case 'PD':
              this.effectTimeFormControl.setValue(this.managementService.state.effect_time);
              break;
            case 'IT':
              this.autoModeTimeFormControl.setValue(this.managementService.state.autochange_time);
              break;
          }
        }
      })
  }

  isDisabled(): boolean {
    // Выключено иди нет подключения
    return !this.managementService.state.power || !this.socketService.isConnected;
  }

  isDisabledByIncomeStream(): boolean {
    // Выключено иди нет подключения или режим - слушатель и идет вещание
    return this.isDisabled() || (this.e131_mode === 2 && this.e131_streaming === true);
  }

  showEffectInformation(duration?: number) {
    this.commonService.timerMessage(MessageType.INFO, `${this.L.$('Текущий эффект:')} '${this.managementService.state.effectName}'`, duration);
  }

  toggleAutoMode($event: MatSlideToggleChange) {
    // $16 0; - ручной режим;
    // $16 1; - авторежим;
    const command = `$16 ${$event.checked ? '1' : '0'};`;
    this.socketService.sendText(command);
  }

  toggleRandomMode($event: MatSlideToggleChange) {
    // $16 5; - вкл/выкл случайный выбор следующего режима
    const command = `$16 5 ${$event.checked ? '1' : '0'};`;
    this.socketService.sendText(command);
  }

  prev($event: MouseEvent) {
    // $16 2; - PrevMode;
    const command = '$16 2;';
    this.socketService.sendText(command);
  }

  next($event: MouseEvent) {
    // $16 3; - NextMode;
    const command = '$16 3;';
    this.socketService.sendText(command);
  }

  applyIntervals($event: MouseEvent) {
    // $17 PD IT; - Время автосмены эффектов (секунд) и бездействия (минут)
    const PD = this.managementService.state.effect_time = this.effectTimeFormControl.value as number;
    const IT = this.managementService.state.autochange_time = this.autoModeTimeFormControl.value as number;

    this.socketService.sendText(`$17 ${PD} ${IT};`);
  }

  getHintForAutoModeTime(): string {
    if (isNullOrUndefined(this.input2) || Number(this.input2.nativeElement.value) === 0) {
      return this.L.$('возврат в авторежим выключен');
    }
    return `${this.L.$('включать авторежим через')} ${this.input2.nativeElement.value} ${this.L.$('минут')} ${this.L.$('[0 - выключено]')}`;
  }

  getNightClockTooltip() {
    return this.e131_mode === 2 && this.e131_streaming === true
    ? this.L.$('В режиме приема вещания в группе недоступно')
    : (this.managementService.state.isNightClockRunnung()  // MC_NIGHT_CLOCK
        ? this.L.$('Выключить ночные часы')
        : this.L.$('Включить ночные часы'));
  }

  getDisabledTooltip() {
    return this.e131_mode === 2 && this.e131_streaming === true
      ? this.L.$('В режиме приема вещания в группе недоступно')
      : "";
  }

  toggleNightClock() {
    if (this.managementService.state.isNightClockRunnung())    // MC_NIGHT_CLOC
      // $3 0; - включить на устройстве демо-режим
      this.socketService.sendText('$3 0;');
    else
      // $14 8; - Включить ночные часы;
      this.socketService.sendText('$14 8;');
  }

  drop(event: CdkDragDrop<number[]>) {
    if (event.previousContainer === event.container) {
      moveItemInArray(event.container.data, event.previousIndex, event.currentIndex);
    } else {
      transferArrayItem(
        event.previousContainer.data,
        event.container.data,
        event.previousIndex,
        event.currentIndex,
      );
    }
    this.managementService.saveCardsUsage();
  }

  selectAll(select: boolean) {
    if (select) {
      // Добавить все карточки в список выбранных
      this.managementService.usageSelectAll();
    } else {
      // Удалить все карточки в список выбранных
      this.managementService.usageUnselectAll();
    }
    this.managementService.saveCardsUsage();
  }

  getSyncType(): string {
    return this.managementService.state.e131_mode == 1
      ? 'MASTER'
      : this.managementService.state.e131_mode == 2
        ? 'SLAVE'
        : '';
  }

  getSyncTypeTooltip(): string {
    return this.managementService.state.e131_mode == 1
      ? this.L.$('MASTER, группа ') + this.managementService.state.e131_group
      : this.managementService.state.e131_mode == 2
        ? this.L.$('SLAVE, группа ') + this.managementService.state.e131_group
        : '';
  }

  getSyncClass(): string {
    switch (this.managementService.state.e131_mode) {
      case 1:
        return 'master ' + (this.managementService.state.power ? 'power-on' : 'power-off');
      case 2:
        return 'slave ' + (this.managementService.state.e131_streaming === true ? 'stream-on' : 'stream-off');
      default:
        return '';
    }
  }

  getPowerButtonClass(): string {
    let str = 'power-btn';
    if (!this.managementService.state.power) {
      str += this.managementService.state.e131_streaming === true &&  this.managementService.state.e131_mode == 2 ? ' power-on slave' : ' power-off';
    } else {
      str += ' power-on';
      switch (this.managementService.state.e131_mode) {
        case 1:
          str += ' master';
          break;
        case 2:
          str += this.managementService.state.e131_streaming === true ? ' slave' : ' standalone';
          break;
        default:
          str += ' standalone';
          break;
      }
    }
    return str;
  }

  getBrightnessLabelClass(): string {
    let str = 'brightness-label';
    if (!this.managementService.state.power) {
      str += ' power-off';
    } else {
      str += ' power-on';
    }
    return str;
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }

}
