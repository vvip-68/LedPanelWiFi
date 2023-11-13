import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {isNullOrUndefinedOrEmpty} from "../../../services/helper";
import {ComboBoxItem} from "../../../models/combo-box.model";
import {EffectModel} from "../../../models/effect.model";
import { MatButtonModule } from '@angular/material/button';
import { MatOptionModule } from '@angular/material/core';
import { NgStyle } from '@angular/common';
import { MatSelectModule } from '@angular/material/select';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { NgxMatTimepickerModule } from 'ngx-mat-timepicker';
import { FormsModule } from '@angular/forms';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';

@Component({
    selector: 'app-tab-modes',
    templateUrl: './tab-modes.component.html',
    styleUrls: ['./tab-modes.component.scss'],
    standalone: true,
    imports: [
        MatFormFieldModule,
        MatInputModule,
        FormsModule,
        NgxMatTimepickerModule,
        DisableControlDirective,
        MatSelectModule,
        MatOptionModule,
        NgStyle,
        MatButtonModule,
    ],
})
export class TabModesComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  public mode1_time: string = '00:00';
  public mode1_effect = -3;
  public mode2_time: string = '00:00';
  public mode2_effect = -3;
  public mode3_time: string = '00:00';
  public mode3_effect = -3;
  public mode4_time: string = '00:00';
  public mode4_effect = -3;
  public mode5_time: string = '00:00';
  public mode5_effect = -3;
  public mode6_time: string = '00:00';
  public mode6_effect = -3;

  public effects: ComboBoxItem[] = [];

  public supportWeather: boolean = false;

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
          const request = 'WZ|AM1T|AM1A|AM2T|AM2A|AM3T|AM3A|AM4T|AM4A|T1|AM5A|T2|AM6A';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'AM1T': {
              this.mode1_time = this.managementService.state.mode1_time.replace(' ', ':');
              break;
            }
            case 'AM1A':
              this.mode1_effect = this.managementService.state.mode1_effect;
              break;
            case 'AM2T': {
              this.mode2_time = this.managementService.state.mode2_time.replace(' ', ':');
              break;
            }
            case 'AM2A':
              this.mode2_effect = this.managementService.state.mode2_effect;
              break;
            case 'AM3T': {
              this.mode3_time = this.managementService.state.mode3_time.replace(' ', ':');
              break;
            }
            case 'AM3A':
              this.mode3_effect = this.managementService.state.mode3_effect;
              break;
            case 'AM4T': {
              this.mode4_time = this.managementService.state.mode4_time.replace(' ', ':');
              break;
            }
            case 'AM4A':
              this.mode4_effect = this.managementService.state.mode4_effect;
              break;
            case 'AM5A':
              this.mode5_effect = this.managementService.state.mode5_effect;
              break;
            case 'AM6A':
              this.mode6_effect = this.managementService.state.mode6_effect;
              break;
            case 'T1': {
              this.mode5_time = this.managementService.state.time_sunrise;
              break;
            }
            case 'T2': {
              this.mode6_time = this.managementService.state.time_sunset
              break;
            }
            case 'WZ': {
              this.supportWeather = this.managementService.state.supportWeather;
              break;
            }
          }
        }
      });

    this.managementService.effects$
      .pipe(takeUntil(this.destroy$)).subscribe((effects: EffectModel[]) => {
      this.effects = [];
      this.effects.push({value: -3, displayText: this.L.$('Нет действия')});
      this.effects.push({value: -2, displayText: this.L.$('Выключить матрицу')});
      this.effects.push({value: -1, displayText: this.L.$('Ночные часы')});
      this.effects.push({value: 0, displayText: this.L.$('Демо режим')});
      let idx = 1;
      for (const effect of effects) {
        this.effects.push({value: idx++, displayText: effect.name});
      }
    });
  }

  isDisabled(): boolean {
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
  }

  applyTimeModes($event: MouseEvent) {

    // ----------------------------------------------------
    // 22 - настройки включения режимов матрицы в указанное время NN5 - Действие на "Рассвет", NN6 - действие на "Закат"
    // - $22 HH1 MM1 NN1 HH2 MM2 NN2 HH3 MM3 NN3 HH4 MM4 NN4 NN5 NN6;
    //     HHn - час срабатывания
    //     MMn - минуты срабатывания
    //     NNn - эффект: -3 - выключено; -2 - выключить матрицу; -1 - ночные часы; 0 - случайный режим и далее по кругу; 1 и далее - список режимов EFFECT_LIST
    // ----------------------------------------------------

    let time = this.mode1_time.split(':');
    this.managementService.state.mode1_time = `${time[0]} ${time[1]}`;
    const HH1 = Number(time[0]);
    const MM1 = Number(time[1]);
    const NN1 = this.mode1_effect;

    time = this.mode2_time.split(':');
    this.managementService.state.mode2_time = `${time[0]} ${time[1]}`;
    const HH2 = Number(time[0]);
    const MM2 = Number(time[1]);
    const NN2 = this.mode2_effect;

    time = this.mode3_time.split(':');
    this.managementService.state.mode3_time = `${time[0]} ${time[1]}`;
    const HH3 = Number(time[0]);
    const MM3 = Number(time[1]);
    const NN3 = this.mode3_effect;

    time = this.mode4_time.split(':');
    this.managementService.state.mode4_time = `${time[0]} ${time[1]}`;
    const HH4 = Number(time[0]);
    const MM4 = Number(time[1]);
    const NN4 = this.mode4_effect;

    const NN5 = this.mode5_effect;
    const NN6 = this.mode6_effect;
    this.socketService.sendText(`$22 ${HH1} ${MM1} ${NN1} ${HH2} ${MM2} ${NN2} ${HH3} ${MM3} ${NN3} ${HH4} ${MM4} ${NN4} ${NN5} ${NN6};`);
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }

}
