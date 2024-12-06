import {Component, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {AppErrorStateMatcher, convertTimeTo24, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import {distinctUntilChanged} from "rxjs/operators";
import {ComboBoxItem} from "../../../models/combo-box.model";
import { MatSliderModule } from '@angular/material/slider';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';
import { NgxMatTimepickerModule } from 'ngx-mat-timepicker';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { MatInputModule } from '@angular/material/input';
import { MatOptionModule } from '@angular/material/core';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatSelectModule } from '@angular/material/select';
import { MatFormFieldModule } from '@angular/material/form-field';
import {Base} from "../../base.class";

@Component({
    selector: 'app-tab-alarm',
    templateUrl: './tab-alarm.component.html',
    styleUrls: ['./tab-alarm.component.scss'],
    standalone: true,
    imports: [
        MatFormFieldModule,
        MatSelectModule,
        FormsModule,
        DisableControlDirective,
        MatOptionModule,
        MatInputModule,
        ReactiveFormsModule,
        MatSlideToggleModule,
        NgxMatTimepickerModule,
        MatButtonModule,
        MatIconModule,
        MatSliderModule,
    ],
})
export class TabAlarmComponent extends Base implements OnInit, OnDestroy {

  public day1_On = false;
  public day2_On = false;
  public day3_On = false;
  public day4_On = false;
  public day5_On = false;
  public day6_On = false;
  public day7_On = false;

  public day1_time: string = '00:00';
  public day2_time: string = '00:00';
  public day3_time: string = '00:00';
  public day4_time: string = '00:00';
  public day5_time: string = '00:00';
  public day6_time: string = '00:00';
  public day7_time: string = '00:00';

  public dawnTimeFormControl = new FormControl(0, [Validators.required, rangeValidator(1, 30)]);
  public alarmTimeFormControl = new FormControl(0, [Validators.required, rangeValidator(1, 5)]);
  public matcher = new AppErrorStateMatcher();

  public supportMP3: boolean = false;
  public alarm_effect: number = -1;
  public alarming: boolean = false;
  public alarm_sound: number = -1;
  public dawn_sound: number = -1;
  public alarm_playing_file: string = '';
  public alarm_use_sound: boolean = false;
  public alarm_volume: number = 20;
  public time12H: boolean = false;           // false: 24H;  true: 12H

  public isAlarmPlaying: boolean = false;
  public isDawnPlaying: boolean = false;
  public alarm_sounds: Array<ComboBoxItem> = [];
  public dawn_sounds: Array<ComboBoxItem> = [];

  private valueChanged$ = new BehaviorSubject(this.alarm_volume);

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
          // При первом соединении сокета с устройством запросить параметры, используемые в экране
          const request = 'MX|C12|AL|AE|AD|AT|AW|MD';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'AL':
              this.alarming = this.managementService.state.alarming;
              break;
            case 'AE':
              this.alarm_effect = this.managementService.state.alarm_effect;
              break;
            case 'AD':
              this.dawnTimeFormControl.setValue(this.managementService.state.dawn_duration);
              break;
            case 'AT': {
              // AT	- "1 0 0|2 0 0|3 0 0|4 0 0|5 0 0|6 0 0|7 0 0"
              const parts = this.managementService.state.alarm_time.split('|');
              let sett = parts[0].split(' ');
              this.day1_time = `${sett[1]}:${sett[2]}`;
              sett = parts[1].split(' ');
              this.day2_time = `${sett[1]}:${sett[2]}`;
              sett = parts[2].split(' ');
              this.day3_time = `${sett[1]}:${sett[2]}`;
              sett = parts[3].split(' ');
              this.day4_time = `${sett[1]}:${sett[2]}`;
              sett = parts[4].split(' ');
              this.day5_time = `${sett[1]}:${sett[2]}`;
              sett = parts[5].split(' ');
              this.day6_time = `${sett[1]}:${sett[2]}`;
              sett = parts[6].split(' ');
              this.day7_time = `${sett[1]}:${sett[2]}`;
              break;
            }
            case 'AW':
              // AW	- AW:число	битовая маска дней недели будильника b6..b0: b0 - пн .. b7 - вс
              const bits = this.managementService.state.alarm_weekday.toString(2).padStart(7, '0');
              this.day1_On = bits[6] === '1';
              this.day2_On = bits[5] === '1';
              this.day3_On = bits[4] === '1';
              this.day4_On = bits[3] === '1';
              this.day5_On = bits[2] === '1';
              this.day6_On = bits[1] === '1';
              this.day7_On = bits[0] === '1';
              break;
            case 'MA':
              this.alarm_sound = this.managementService.state.alarm_sound + 2;
              break;
            case 'MB':
              this.dawn_sound = this.managementService.state.dawn_sound + 2;
              break;
            case 'MD':
              this.alarmTimeFormControl.setValue(this.managementService.state.alarm_duration);
              break;
            case 'MP': {
              // MP:папка~файл	номер папки и файла звука который проигрывается, номер папки и звука разделены '~' -> String(soundFolder) + '~' + String(soundFile+2);
              this.alarm_playing_file = this.managementService.state.alarm_playing_file;
              const parts = this.alarm_playing_file.split('~');
              switch (Number(parts[0])) {
                case 1: // Папка звуков будильника
                  this.alarm_sound = Number(parts[1]);
                  this.isAlarmPlaying = this.alarm_sound > 1;
                  break;
                case 2: // Папка звуков рассвета
                  this.dawn_sound = Number(parts[1]);
                  this.isDawnPlaying = this.dawn_sound > 1;
                  break;
                default:
                  this.isDawnPlaying = false;
                  this.isAlarmPlaying = false;
                  break;
              }
              break;
            }
            case 'MU':
              this.alarm_use_sound = this.managementService.state.alarm_use_sound;
              break;
            case 'MV':
              this.alarm_volume = this.managementService.state.alarm_volume;
              break;
            case 'MX':
              this.supportMP3 = this.managementService.state.supportMP3;
              // Если MP3-плеер поддерживается - запросить параметры, касающиеся настроек звука рассветв / будильника
              if (this.supportMP3) {
                let request = 'MA|MB|MP|MU|MV';
                if (this.managementService.alarm_sounds.length === 0) {
                  request += '|CRS1'
                }
                if (this.managementService.dawn_sounds.length === 0) {
                  request += '|CRS2'
                }
                this.managementService.getKeys(request);
              }
              break;
            case 'C12': {
              this.time12H = this.managementService.state.time12h;
              break;
            }
          }
        }
      });

    this.valueChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        this.alarm_volume = value;
        // $20 5 VV; - установить уровень громкости проигрывания примеров (когда звук уже воспроизводится плеером)
        //       VV - уровень громкости
        if (this.isAlarmPlaying || this.isDawnPlaying) {
          this.socketService.sendText(`$20 5 ${value};`);
        }
      });

    this.managementService.alarm_sounds$
      .pipe(takeUntil(this.destroy$)).subscribe((sounds: ComboBoxItem[]) => {
      this.alarm_sounds = [];
      this.alarm_sounds.push({value: 1, displayText: this.L.$('Нет звука')});
      this.alarm_sounds.push({value: 2, displayText: this.L.$('Случайный звук')});
      let idx = 3;
      for (const sound of sounds) {
        this.alarm_sounds.push({value: idx++, displayText: sound.displayText});
      }
    });

    this.managementService.dawn_sounds$
      .pipe(takeUntil(this.destroy$)).subscribe((sounds: ComboBoxItem[]) => {
      this.dawn_sounds = [];
      this.dawn_sounds.push({value: 1, displayText: this.L.$('Нет звука')});
      this.dawn_sounds.push({value: 2, displayText: this.L.$('Случайный звук')});
      let idx = 3;
      for (const sound of sounds) {
        this.dawn_sounds.push({value: idx++, displayText: sound.displayText});
      }
    });
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  isValid(): boolean {
    return this.dawnTimeFormControl.valid && this.alarmTimeFormControl.valid;
  }

  applyAlarmModes($event: MouseEvent) {
    /*
      $6 6|текст  - Настройки будильника в формате "$6 6|DD EF WD AD HH1 MM1 HH2 MM2 HH3 MM3 HH4 MM4 HH5 MM5 HH6 MM6 HH7 MM7"
        DD    - установка продолжительности рассвета (рассвет начинается за DD минут до установленного времени будильника)
        EF    - установка эффекта, который будет использован в качестве рассвета
        WD    - установка дней пн-вс как битовая маска
        AD    - продолжительность "звонка" сработавшего будильника
        HHx   - часы дня недели x (1-пн..7-вс)
        MMx   - минуты дня недели x (1-пн..7-вс)
    */

    const DD = this.managementService.state.dawn_duration = this.dawnTimeFormControl.value as number;
    const AD = this.managementService.state.alarm_duration = this.alarmTimeFormControl.value as number;
    const EF = this.alarm_effect;
    // @formatter:off
    const WD = (this.day7_On ? 1 : 0) * 64 | (this.day6_On ? 1 : 0) * 32 | (this.day5_On ? 1 : 0) * 16 |
               (this.day4_On ? 1 : 0) * 8  | (this.day3_On ? 1 : 0) * 4  | (this.day2_On ? 1 : 0) * 2  | (this.day1_On ? 1 : 0);
    // @formatter:on
    let time1 = convertTimeTo24(this.day1_time).replace(':',' ');
    let time2 = convertTimeTo24(this.day2_time).replace(':',' ');
    let time3 = convertTimeTo24(this.day3_time).replace(':',' ');
    let time4 = convertTimeTo24(this.day4_time).replace(':',' ');
    let time5 = convertTimeTo24(this.day5_time).replace(':',' ');
    let time6 = convertTimeTo24(this.day6_time).replace(':',' ');
    let time7 = convertTimeTo24(this.day7_time).replace(':',' ');

    this.socketService.sendText(`$6 6|${DD} ${EF} ${WD} ${AD} ${time1} ${time2} ${time3} ${time4} ${time5} ${time6} ${time7}`);
  }

  applySounds($event: MouseEvent) {
    // $20 2 X VV MA MB;
    //    X    - использовать звук будильника X=0 - нет, X=1 - да
    //   VV    - максимальная громкость
    //   MA    - номер файла звука будильника  -1 - нет; 0 - случайно; 1 - 1-й файл и т.д
    //   MB    - номер файла звука рассвета    -1 - нет; 0 - случайно; 1 - 1-й файл и т.д
    const X  = this.alarm_use_sound ? 1 : 0;
    const VV = this.alarm_volume;
    const MA = this.alarm_sound;
    const MB = this.dawn_sound;
    this.isDawnPlaying = false;
    this.isAlarmPlaying = false;
    this.socketService.sendText(`$20 2 ${X} ${VV} ${MA} ${MB};`);
  }

  toggleDawnSound() {
    this.isAlarmPlaying = false;
    this.isDawnPlaying = !this.isDawnPlaying;
    // $20 4 X NN VV; - воспроизведение примера звука рассвета
    //   X - 1 играть 0 - остановить
    //  NN - номер файла звука рассвета из папки SD:/02  // Знач: -1 - нет; 0 - случайно; 1 и далее - файлы;
    //  VV - уровень громкости
    const X  = this.isDawnPlaying ? 1 : 0;
    const VV = this.alarm_volume;
    const NN = this.dawn_sound;
    this.socketService.sendText(`$20 4 ${X} ${NN} ${VV};`);
  }

  toggleAlarmSound() {
    this.isDawnPlaying = false;
    this.isAlarmPlaying = !this.isAlarmPlaying;
    // $20 3 X NN VV; - воспроизведение примера звука будильника
    //   X - 1 играть 0 - остановить
    //  NN - номер файла звука будильника из папки SD:/01  // Знач: -1 - нет; 0 - случайно; 1 и далее - файлы;
    //  VV - уровень громкости
    const X  = this.isAlarmPlaying ? 1 : 0;
    const VV = this.alarm_volume;
    const NN = this.alarm_sound;
    this.socketService.sendText(`$20 3 ${X} ${NN} ${VV};`);
  }

  volumeChanged(value: number) {
    this.valueChanged$.next(value);
  }

  formatLabel(value: number) {
    return value.toString();
  }

  downSoundChanged(sound: number) {
    if (this.isDawnPlaying) {
      const VV = this.alarm_volume;
      const NN = this.dawn_sound = sound;
      const X  = this.dawn_sound > 2 ? 1 : 0;
      if (X == 0) this.isDawnPlaying = false;
      this.socketService.sendText(`$20 4 ${X} ${NN} ${VV};`);
    }
  }

  alarmSoundChanged(sound: number) {
    if (this.isAlarmPlaying) {
      const VV = this.alarm_volume;
      const NN = this.alarm_sound = sound;
      const X  = this.alarm_sound > 2 ? 1 : 0;
      if (X == 0) this.isAlarmPlaying = false;
      this.socketService.sendText(`$20 3 ${X} ${NN} ${VV};`);
    }
  }

}
