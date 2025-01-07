import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {isNullOrUndefinedOrEmpty} from "../../../services/helper";
import { TabOtherComponent } from '../tab-other/tab-other.component';
import { TabWeatherComponent } from '../tab-weather/tab-weather.component';
import { TabNetworkComponent } from '../tab-network/tab-network.component';
import { TabSynchComponent } from '../tab-synch/tab-synch.component';
import { TabWiringComponent } from '../tab-wiring/tab-wiring.component';
import { TabMatrixComponent } from '../tab-matrix/tab-matrix.component';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatIconModule } from '@angular/material/icon';
import { MatTabsModule } from '@angular/material/tabs';
import {Base} from "../../base.class";
import {TabLayoutComponent} from "../tab-layout/tab-layout.component";
import {TabAlarmComponent} from "../tab-alarm/tab-alarm.component";

@Component({
    selector: 'app-tab-setup',
    templateUrl: './tab-setup.component.html',
    styleUrls: ['./tab-setup.component.scss'],
    standalone: true,
  imports: [
    MatTabsModule,
    MatIconModule,
    MatTooltipModule,
    TabMatrixComponent,
    TabWiringComponent,
    TabSynchComponent,
    TabNetworkComponent,
    TabWeatherComponent,
    TabOtherComponent,
    TabLayoutComponent,
  ],
})
export class TabSetupComponent extends Base implements OnInit, OnDestroy {

  supportWeather: boolean = false;
  supportE131: boolean = false;

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService
  ) {
    super();
  }

  ngOnInit() {
    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          const request = 'WZ|E0';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'WZ':
              this.supportWeather = this.managementService.state.supportWeather;
              break;
            case 'E0':
              this.supportE131 = this.managementService.state.supportE131;
              break;
          }
        }
      });
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

}
