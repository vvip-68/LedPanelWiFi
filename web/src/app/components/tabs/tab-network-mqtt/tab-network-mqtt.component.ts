import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {FormControl, Validators} from "@angular/forms";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty} from "../../../services/helper";
import {distinctUntilChanged} from "rxjs/operators";

@Component({
  selector: 'app-tab-network-mqtt',
  templateUrl: './tab-network-mqtt.component.html',
  styleUrls: ['./tab-network-mqtt.component.scss'],
})
export class TabNetworkMqttComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  supportMQTT = false;
  useMQTT = false;
  hideMqttPassword = true;
  mqttServerFormControl = new FormControl('', [Validators.required]);
  mqttUserFormControl = new FormControl('', [Validators.required]);
  mqttPassFormControl = new FormControl('', [Validators.required]);
  mqttPortFormControl = new FormControl(0, [Validators.required]);
  mqttPrefixFormControl = new FormControl('');

  matcher = new AppErrorStateMatcher();

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService
  ) {
  }

  ngOnInit() {

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          const request = 'QA|QS|QU|QW|QR|QP|QZ';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'QZ':
              this.supportMQTT = this.managementService.state.supportMQTT;
              break;
            case 'QA':
              this.useMQTT = this.managementService.state.useMQTT;
              break;
            case 'QS':
              this.mqttServerFormControl.setValue(this.managementService.state.mqttServerName);
              break;
            case 'QU':
              this.mqttUserFormControl.setValue(this.managementService.state.mqttUserName);
              break;
            case 'QW':
              this.mqttPassFormControl.setValue(this.managementService.state.mqttPassword);
              break;
            case 'QR':
              this.mqttPrefixFormControl.setValue(this.managementService.state.mqttPrefix);
              break;
            case 'QP':
              this.mqttPortFormControl.setValue(this.managementService.state.mqttPort);
              break;

          }
        }
      });
  }

  isDisabled(): boolean {
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
  }

  isMqttSettingsValid(): boolean {
    return this.mqttServerFormControl.valid &&
      this.mqttUserFormControl.valid &&
      this.mqttPassFormControl.valid &&
      this.mqttPortFormControl.valid &&
      this.mqttPrefixFormControl.valid;
  }

  applyMqttSettings($event: MouseEvent) {
    this.managementService.state.useMQTT = this.useMQTT;
    this.managementService.state.mqttServerName = this.mqttServerFormControl.value as string;
    this.managementService.state.mqttUserName = this.mqttUserFormControl.value as string;
    this.managementService.state.mqttPassword = this.mqttPassFormControl.value as string;
    this.managementService.state.mqttPrefix = this.mqttPrefixFormControl.value as string;
    this.managementService.state.mqttPort = this.mqttPortFormControl.value as number;

    // $6 8|server  - MQTT сервер
    // $6 9|user    - MQTT пользователь
    // $6 10|pwd    - MQTT пароль
    // $6 13|prefix - MQTT префикс топика
    // $11 1 X;     - использовать управление через MQTT сервер X; 0 - не использовать; 1 - использовать
    // $11 2 D;     - порт MQTT
    // $11 5;       - Разорвать подключение к MQTT серверу, чтобы он мог переподключиться с новыми параметрами

    this.socketService.sendText(`$6 8|${this.managementService.state.mqttServerName}`);
    this.socketService.sendText(`$6 9|${this.managementService.state.mqttUserName}`);
    this.socketService.sendText(`$6 10|${this.managementService.state.mqttPassword}`);
    this.socketService.sendText(`$6 13|${this.managementService.state.mqttPrefix}`);
    this.socketService.sendText(`$11 2 ${this.managementService.state.mqttPort};`);
    this.socketService.sendText(`$11 1 ${this.managementService.state.useMQTT ? '1' : '0'};`);
    this.socketService.sendText('$11 5;');
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
