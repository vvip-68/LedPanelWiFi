import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty} from "../../../services/helper";
import {distinctUntilChanged} from "rxjs/operators";
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';

@Component({
    selector: 'app-tab-network-ap',
    templateUrl: './tab-network-ap.component.html',
    styleUrls: ['./tab-network-ap.component.scss'],
    standalone: true,
    imports: [
        MatSlideToggleModule,
        FormsModule,
        MatFormFieldModule,
        MatInputModule,
        ReactiveFormsModule,
        DisableControlDirective,
        MatButtonModule,
        MatIconModule,
    ],
})
export class TabNetworkApComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  hideAPPassword = true;
  createAccessPoint = false;

  apFormControl = new FormControl('', [Validators.required]);
  passAPFormControl = new FormControl('', [Validators.required]);

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
          const request = 'AN|AB|AU';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'AN':
              this.apFormControl.setValue(this.managementService.state.apName);
              break;
            case 'AB':
              this.passAPFormControl.setValue(this.managementService.state.apPass);
              break;
            case 'AU':
              this.createAccessPoint = this.managementService.state.useAP;
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

  isAccessPointValid(): boolean {
    return this.apFormControl.valid && this.passAPFormControl.valid;
  }

  applyAccessPoint($event: MouseEvent) {
    this.managementService.state.apName = this.apFormControl.value as string;
    this.managementService.state.apPass = this.passAPFormControl.value as string;
    this.managementService.state.useAP = this.createAccessPoint;

    // $6 4|ap_name - имя точки доступа
    // $6 5|ap_pwd  - пароль точки доступа
    // $21 0 X; - использовать точку доступа: X=0 - не использовать X=1 - использовать
    this.socketService.sendText(`$6 4|${this.managementService.state.apName}`);
    this.socketService.sendText(`$6 5|${this.managementService.state.apPass}`);
    this.socketService.sendText(`$21 0 ${this.managementService.state.useAP ? '1' : '0'};`);
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
