import { DOCUMENT } from '@angular/common';
import { Component, Inject, OnDestroy, OnInit } from '@angular/core';
import { MatDialogRef} from '@angular/material/dialog';
import { MatSlideToggleChange, MatSlideToggleModule } from '@angular/material/slide-toggle';
import { debounceTime, Subject, takeUntil } from 'rxjs';
import { distinctUntilChanged } from 'rxjs/operators';
import { ColorPickerComponent } from './components/color-picker/color-picker.component';
import { CommonService, MessageType } from './services/common/common.service';
import {getUptime, isNullOrUndefined, isNullOrUndefinedOrEmpty} from './services/helper';
import { LanguagesService } from './services/languages/languages.service';
import { ManagementService} from './services/management/management.service';
import { WebsocketService } from './services/websocket/websocket.service';
import { TabSetupComponent } from './components/tabs/tab-setup/tab-setup.component';
import { TabGamesComponent } from './components/tabs/tab-games/tab-games.component';
import { TabDrawComponent } from './components/tabs/tab-draw/tab-draw.component';
import { TabModesComponent } from './components/tabs/tab-modes/tab-modes.component';
import { TabAlarmComponent } from './components/tabs/tab-alarm/tab-alarm.component';
import { TabClockComponent } from './components/tabs/tab-clock/tab-clock.component';
import { TabTextsPanelComponent } from './components/tabs/tab-texts-panel/tab-texts-panel.component';
import { TabEffectsComponent } from './components/tabs/tab-effects/tab-effects.component';
import { MatTabsModule } from '@angular/material/tabs';
import { MatIconModule } from '@angular/material/icon';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MatTooltipDefaultOptions, MatTooltipModule} from '@angular/material/tooltip';

export const customTooltipDefaults: MatTooltipDefaultOptions = {
  showDelay: 1000,
  hideDelay: 100,
  touchendHideDelay: 100
};

@Component({
    selector: 'app-root',
    templateUrl: './app.component.html',
    styleUrls: ['./app.component.scss'],
    standalone: true,
    imports: [
      MatToolbarModule, MatIconModule, MatTooltipModule, MatSlideToggleModule, MatTabsModule,
      TabEffectsComponent, TabTextsPanelComponent, TabClockComponent, TabAlarmComponent, TabModesComponent, TabDrawComponent, TabGamesComponent, TabSetupComponent
    ]
})
export class AppComponent implements OnInit, OnDestroy {
  private static readonly DARK_THEME_CLASS = 'dark-theme';

  get isDarkTheme() {
    return this._darkTheme;
  }

  set isDarkTheme(value: boolean) {
    this._darkTheme = value;
    if (value) {
      this.document.documentElement.classList.add(AppComponent.DARK_THEME_CLASS);
    } else {
      this.document.documentElement.classList.remove(AppComponent.DARK_THEME_CLASS);
    }
    try {
      window.localStorage[AppComponent.DARK_THEME_CLASS] = value;
    } catch {
    }
  }

  public selectedTab: number = 0;

  private _darkTheme: boolean = false;

  private uptime: number = 0;

  private freeMemory: number = 0;

  private destroy$ = new Subject();

  private colorDialogRef: MatDialogRef<ColorPickerComponent> | null = null;

  constructor(@Inject(DOCUMENT) private document: Document,
              public socketService: WebsocketService,
              public managementService: ManagementService,
              public commonService: CommonService,
              public L: LanguagesService) {
    this.isDarkTheme = window.localStorage[AppComponent.DARK_THEME_CLASS] === 'true' || false;

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в главном экране
          const request = 'W|H|PS|LG|VR|HN|FS|SX|CH|CV|WZ|AL|SM|CRLE|UP|FM|PN|MC|MX|MZ|TM';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'ER':
              this.commonService.timerMessage(MessageType.ERROR, this.managementService.state.last_error, 5000);
              break;
            case 'NF':
              this.commonService.timerMessage(MessageType.INFO, this.managementService.state.last_info, 5000);
              break;
            case 'UP':
              this.uptime = this.managementService.state.uptime;
              break;
            case 'FM':
              this.freeMemory = this.managementService.state.freeMemory;
              break;
          }
        }
      });

  }

  ngOnInit() {
  }

  changeTheme($event: MatSlideToggleChange) {
    this.isDarkTheme = $event.checked;
  }

  setDarkTheme(isDark: boolean) {
    this.isDarkTheme = isDark;
  }

  isDisabled(): boolean {
    return !this.managementService.state.power || !this.socketService.isConnected;
  }

  isStreaming(): boolean {
    return this.managementService.state.e131_mode === 2 && this.managementService.state.e131_streaming === true;
  }


  isInitialized(): boolean {
    return !isNullOrUndefined(this.managementService.state.e131_streaming);
  }

  getUptimeAndMemoryUsage() {
    if (this.freeMemory > 0 && this.uptime > 0) {
      return `${this.L.$('Время работы:')} ${getUptime(this.uptime, this.L)} --- ${this.L.$('Память:')} ${this.freeMemory} ${this.L.$('байт')}`;
    }
    return this.managementService.state.version;
  }

  showEffectInformation(duration?: number) {
    this.commonService.timerMessage(MessageType.INFO, `${this.L.$('Текущий эффект:')} '${this.managementService.state.effectName}'`, duration);
  }

  stopAlarm() {
    // - $20 0;  - отключение будильника после его срабатывания (сброс состояния isAlarming)
    this.managementService.state.alarming = !this.managementService.state.alarming;
    this.socketService.sendText('$20 0;');
  }

  onRightMouseClick(e: MouseEvent) {
    e.preventDefault();
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }

}
