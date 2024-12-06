import {Component, ElementRef, Input, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {takeUntil} from 'rxjs';
import {ActionModel, ActionType} from '../../models/action.model';
import {isNullOrUndefinedOrEmpty} from '../../services/helper';
import {ManagementService} from '../../services/management/management.service';
import {WebsocketService} from '../../services/websocket/websocket.service';
import { MatIconModule } from '@angular/material/icon';
import { MatTooltipModule } from '@angular/material/tooltip';
import { NgClass } from '@angular/common';
import {Base} from "../base.class";

@Component({
    selector: 'app-action',
    templateUrl: './action.component.html',
    styleUrls: ['./action.component.scss'],
    standalone: true,
    imports: [NgClass, MatTooltipModule, MatIconModule]
})
export class ActionComponent extends Base implements OnInit, OnDestroy {

  actionType = ActionType;

  @ViewChild('card', {static: true}) card!: ElementRef;
  @ViewChild('indicator', {static: true}) indicator!: ElementRef;

  @Input() disabled: boolean = false;
  @Input() tooltip: string = '';

  @Input()
  get config(): ActionModel {
    return this._config;
  }

  set config(cfg: ActionModel) {
    this._config = cfg;
    this.updateValues();
  }

  public isActive = false;

  private _config!: ActionModel;

  constructor(private socketService: WebsocketService,
              private managementService: ManagementService) {
    super();
  }

  ngOnInit() {
    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) this.processKey(key);
      });
  }

  private processKey(key: string) {
    if (this._config && this._config.key == key) {
      this._config.value = this.managementService.state.getValueByKey(key);
      this.updateValues();
    }
  }

  private updateValues() {
    if (this.config && this.config.mode !== ActionType.NONE) {

      this.card.nativeElement.classList.add(`card_${this.config.key.toLowerCase()}`);

      if (this.config.mode === ActionType.POWER || this.config.mode === ActionType.BRIGHT) {
        if (typeof this.config.active === 'boolean') {
          this.isActive = this.managementService.state.power && this.config.active;
        } else if (typeof this.config.active === 'function') {
          // Если питание отключено - все другие "кнопки" также должны быть отключены
          this.isActive = this.config.active(this.config.value);
        }
      }
    }
  }

  doSetCommand() {
    if (this.config.set) {
      let command = '';
      if (this.config.key === 'PS' || this.managementService.state.power) {
        if (typeof this.config.set === 'string' && this.config.set.length > 0) {
          command = this.config.set as string;
        } else if (typeof this.config.set === 'function') {
          command = this.config.set(this.config.value) ?? '';
        }
        if (command.length > 0) {
          this.socketService.sendText(command);
        }
      }
    }
  }

  getIcon(): string {
    switch (this.config.mode) {
      case ActionType.POWER:
        return "power_settings_new";
      case ActionType.BRIGHT:
        return "wb_sunny";
    }
    return "";
  }

  getIconClass(): string {
    let cls = this.config.mode.toString();
    if (this.config.icon && this.config.icon.length > 0) {
      cls += ' ' + this.config.icon;
    }
    cls += ` state_${this.isActive ? 'ON' : 'OFF'}`
    return cls;
  }

  getIndicatorClass(): string {
    return `indicator state_${this.isActive ? 'ON' : 'OFF'}` + (this.managementService.state.e131_mode == 2 && this.managementService.state.e131_streaming === true ? ' streaming' : '');
  }
}
