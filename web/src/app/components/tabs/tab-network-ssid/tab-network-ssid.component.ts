import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {AppErrorStateMatcher, ipV4Validator, isNullOrUndefinedOrEmpty, lengthValidator} from "../../../services/helper";
import {distinctUntilChanged} from "rxjs/operators";
import { InputRestrictionDirective } from '../../../directives/input-restrict.directive';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';

@Component({
    selector: 'app-tab-network-ssid',
    templateUrl: './tab-network-ssid.component.html',
    styleUrls: ['./tab-network-ssid.component.scss'],
    standalone: true,
    imports: [
        MatFormFieldModule,
        MatInputModule,
        FormsModule,
        ReactiveFormsModule,
        DisableControlDirective,
        MatButtonModule,
        MatIconModule,
        InputRestrictionDirective,
    ],
})
export class TabNetworkSsidComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  hidePassword = true;

  ssidFormControl = new FormControl('', [Validators.required]);
  passwordFormControl = new FormControl('', [Validators.required, lengthValidator(8, 64)]);
  ipAddressFormControl = new FormControl('', [ipV4Validator()]);

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
          const request = 'NW|NX|IP';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'NW':
              this.ssidFormControl.setValue(this.managementService.state.ssid);
              break;
            case 'NX':
              this.passwordFormControl.setValue(this.managementService.state.password);
              break;
            case 'IP':
              this.ipAddressFormControl.setValue(this.managementService.state.ip);
              break;
          }
        }
      });
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  isNetworkValid(): boolean {
    return this.ssidFormControl.valid && this.passwordFormControl.valid && this.ipAddressFormControl.valid;
  }

  applyNetwork($event: MouseEvent) {
    this.managementService.state.ssid = this.ssidFormControl.value as string;
    this.managementService.state.password = this.passwordFormControl.value as string;
    this.managementService.state.ip = this.ipAddressFormControl.value as string;

    // $6 2|ssid_name - имя сети
    // $6 3|ssid_pwd  - пароль к сети
    this.socketService.sendText(`$6 2|${this.managementService.state.ssid}`);
    this.socketService.sendText(`$6 3|${this.managementService.state.password}`);

    // $21 1 IP1 IP2 IP3 IP4; - установить статический IP адрес подключения к локальной WiFi сети, пример: $21 1 192 168 0 106
    if (isNullOrUndefinedOrEmpty(this.managementService.state.ip)) {
      this.socketService.sendText('$21 1 0 0 0 0;');
    } else {
      const parts = this.managementService.state.ip.split('.');
      this.socketService.sendText(`$21 1 ${parts[0]} ${parts[1]} ${parts[2]} ${parts[3]};`);
    }

    // $21 2; Выполнить переподключение к сети WiFi
    this.socketService.sendText('$21 2;');
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
