import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {AppErrorStateMatcher, isNullOrUndefined, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import {distinctUntilChanged} from "rxjs/operators";
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {ComboBoxItem} from "../../../models/combo-box.model";
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatOptionModule } from '@angular/material/core';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatSelectModule } from '@angular/material/select';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { MatIconModule } from '@angular/material/icon';
import { MatTooltipModule } from '@angular/material/tooltip';
import { NgClass } from '@angular/common';

interface LineParameters {
  idx: number,    // Номер линии 1..4
  pin: Pin,       // Описатель пина подключения
  start: number,  // Начальный индекс сегмента в массиве светодиодов
  length: number, // Длина сегмента
  startControl: FormControl,
  lengthControl: FormControl
}

interface Pin {
  idx: number,     // Для упорядочивания
  use: boolean,    // toggler state - используется / не используется
  gpio: number,    // GPIO
  name: string,    // Общепринятое имя пина
  alias: string,   // Мнемоническое обозначение в коде
  assign: number   // Код назначения пина
}

interface Assignment {
  id: number,      // Код назначения
  name: string     // Имя назначения
}

@Component({
    selector: 'app-tab-wiring',
    templateUrl: './tab-wiring.component.html',
    styleUrls: ['./tab-wiring.component.scss'],
    standalone: true,
    imports: [
        MatTooltipModule,
        MatIconModule,
        NgClass,
        MatSlideToggleModule,
        FormsModule,
        MatSelectModule,
        DisableControlDirective,
        MatOptionModule,
        MatFormFieldModule,
        MatInputModule,
        ReactiveFormsModule,
        MatButtonModule,
    ],
})

export class TabWiringComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  // MC	MC:[текст]	тип микроконтроллера "ESP32", "NodeMCU", "Wemos d1 mini"
  // MZ	MZ:X	        прошивка поддерживает MP3 плеер 0-нет, 1-да
  // SZ SZ:X          поддержка прошивкой функционала SD карты в системе - USE_SD: Х = 0 - USE_SD = 0; USE_SD = 1
  // TM	TM:X	        в системе присутствует индикатор TM1637, где Х = 0 - нет; 1 - есть
  // UB	UВ:X	        прошивка поддерживает кнопку USE_BUTTON == 1 - 0 - выключено; 1 - включено

  // 2306:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2307:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2308:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2309:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2310:X           тип матрицы vDEVICE_TYPE: 0 - труба; 1 - панель
  // 2311:X Y         X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
  // 2312:X Y         X - GPIO пин управления питанием; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
  // 2313:X Y         X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
  // 2314:X           vDEBUG_SERIAL
  // 2315:X Y         X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
  // 2316:X Y         X - GPIO пин DIO на TM1637; Y - GPIO пин CLK на TM1637

  assignment: Assignment[] = [
    {id:  0, name: 'N/A'},
    {id:  1, name: 'LED LINE 1'},
    {id:  2, name: 'LED LINE 2'},
    {id:  3, name: 'LED LINE 3'},
    {id:  4, name: 'LED LINE 4'},
    {id:  5, name: 'BUTTON'},
    {id:  6, name: 'SD CS'},
    {id:  7, name: 'SD CLK'},
    {id:  8, name: 'SD MISO'},
    {id:  9, name: 'SD MOSI'},
    {id: 10, name: 'MP3 STX'},
    {id: 11, name: 'MP3 SRX'},
    {id: 12, name: 'TM1637 CLK'},
    {id: 13, name: 'TM1637 DIO'},
    {id: 14, name: 'POWER'}
 ];

  controller = "";
  controller_type = 0;  // 0 - UNKNOWN; 1 - NodeMCU; 2 - Wemos d1 mini; 3 - ESP32

  supportMP3 = false;
  supportSD = false;
  supportTM1637 = false;
  supportButton = false;
  supportPower = false;

  device_type: number = 1;
  button_type: number = 1;
  power_level: number = 1;
  wait_play_finished: boolean = false;
  repeat_play: boolean = true;
  debug_serial: boolean = true;

  button_pin:     Pin;
  power_pin:      Pin;
  player_tx_pin:  Pin;
  player_rx_pin:  Pin;
  tm1637_dio_pin: Pin;
  tm1637_clk_pin: Pin;

  sd_cs_pin:      Pin;
  sd_miso_pin:    Pin;
  sd_mosi_pin:    Pin;
  sd_clk_pin:     Pin;

  button_pin_list:     Pin[] = [];
  power_pin_list:      Pin[] = [];
  player_tx_pin_list:  Pin[] = [];
  player_rx_pin_list:  Pin[] = [];
  tm1637_dio_pin_list: Pin[] = [];
  tm1637_clk_pin_list: Pin[] = [];
  sd_cs_pin_list:      Pin[] = [];
  sd_miso_pin_list:    Pin[] = [];
  sd_mosi_pin_list:    Pin[] = [];
  sd_clk_pin_list:     Pin[] = [];

  lines: LineParameters[] = [];

  line1_pin_list: Pin[] = [];
  line2_pin_list: Pin[] = [];
  line3_pin_list: Pin[] = [];
  line4_pin_list: Pin[] = [];

  pins_free    : Pin[] = [];
  pins_assigned: Pin[] = [];

  line1StartFormControl = new FormControl(0);
  line1LengthFormControl = new FormControl(1);
  line2StartFormControl = new FormControl(0);
  line2LengthFormControl = new FormControl(1);
  line3StartFormControl = new FormControl(0);
  line3LengthFormControl = new FormControl(1);
  line4StartFormControl = new FormControl(0);
  line4LengthFormControl = new FormControl(1);

  numLeds: number = 0;
  button_type_list = new Array<ComboBoxItem>();
  power_type_list = new Array<ComboBoxItem>();
  device_type_list = new Array<ComboBoxItem>();

  matcher = new AppErrorStateMatcher();

  private timerid: any = undefined;

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService) {
      this.button_pin = this.createEmptyPin();
      this.power_pin = this.createEmptyPin();
      this.player_tx_pin = this.createEmptyPin();
      this.player_rx_pin = this.createEmptyPin();
      this.tm1637_dio_pin = this.createEmptyPin();
      this.tm1637_clk_pin = this.createEmptyPin();
      this.sd_cs_pin = this.createEmptyPin();
      this.sd_miso_pin = this.createEmptyPin();
      this.sd_mosi_pin = this.createEmptyPin();
      this.sd_clk_pin = this.createEmptyPin();

      this.lines.push( { idx: 1, pin: this.createEmptyPin(), start: 0, length: 0, startControl: this.line1StartFormControl, lengthControl: this.line1LengthFormControl } );
      this.lines.push( { idx: 2, pin: this.createEmptyPin(), start: 0, length: 0, startControl: this.line2StartFormControl, lengthControl: this.line2LengthFormControl } );
      this.lines.push( { idx: 3, pin: this.createEmptyPin(), start: 0, length: 0, startControl: this.line3StartFormControl, lengthControl: this.line3LengthFormControl } );
      this.lines.push( { idx: 4, pin: this.createEmptyPin(), start: 0, length: 0, startControl: this.line4StartFormControl, lengthControl: this.line4LengthFormControl } );

      this.button_type_list.push({value: 0, displayText: this.L.$('Сенсорная')});
      this.button_type_list.push({value: 1, displayText: this.L.$('Тактовая')});

      this.power_type_list.push({value: 0, displayText: this.L.$('Низкий')});
      this.power_type_list.push({value: 1, displayText: this.L.$('Высокий')});

      this.device_type_list.push({value: 0, displayText: this.L.$('Труба')});
      this.device_type_list.push({value: 1, displayText: this.L.$('Панель')});
  }

  ngOnInit() {

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          const request = 'MC|MZ|SZ|TM|UB|PZ';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'MC':
              this.setControllerType();
              const request = '2306|2307|2308|2309|2310|2311|2312|2313|2314|2315|2316';
              this.managementService.getKeys(request);
              break;
            case 'MZ':
              this.supportMP3 = this.managementService.state.supportPlayer;
              // В ESP32 пины подключения DFPlayer - на аппаратные RX2/TX2 - G16/G17
              if (this.supportMP3 && this.controller_type === 3) {
                this.managementService.state.player_rx_pin = 16;
                this.managementService.state.player_tx_pin = 17;
                this.player_rx_pin = this.assignPin(16, 'MP3 SRX');
                this.player_tx_pin = this.assignPin(17, 'MP3 STX');
              }
              break;
            case 'SZ':
              this.supportSD = this.managementService.state.supportSD;
              if (this.supportSD) {
                if (this.controller_type === 1 || this.controller_type === 2) {
                  // ESP8266 - назначение пинов SD-карты - D5(14-CLK), D6(12-MISO),D7(13-MOSI), D8(15-CS)
                  this.sd_clk_pin  = this.assignPin(14, 'SD CLK');  // D5
                  this.sd_miso_pin = this.assignPin(12, 'SD MISO'); // D6
                  this.sd_mosi_pin = this.assignPin(13, 'SD MOSI'); // D7
                  this.sd_cs_pin   = this.assignPin(15, 'SD CS');   // D8
                } else {
                  // ESP32 - назначение пинов SD-карты - G18(CLK), G19(MISO), G23(MOSI) G5(CS)
                  this.sd_clk_pin  = this.assignPin(18, 'SD CLK');  // G18
                  this.sd_miso_pin = this.assignPin(19, 'SD MISO'); // G19
                  this.sd_mosi_pin = this.assignPin(23, 'SD MOSI'); // G23
                  this.sd_cs_pin   = this.assignPin(5,  'SD CS');   // G5
                }
              }
              break;
            case 'TM':   this.supportTM1637 = this.managementService.state.supportTM1637; break;
            case 'UB':   this.supportButton = this.managementService.state.supportButton; break;
            case 'PZ':   this.supportPower  = this.managementService.state.supportPower; break;
            case '2306': this.setLineParams(1, this.managementService.state.led_line_1); break;
            case '2307': this.setLineParams(2, this.managementService.state.led_line_2); break;
            case '2308': this.setLineParams(3, this.managementService.state.led_line_3); break;
            case '2309': this.setLineParams(4, this.managementService.state.led_line_4); break;
            case '2310': this.device_type = this.managementService.state.device_type; break;
            case '2311':
              if (this.supportButton) {
                this.button_pin = this.assignPin(this.managementService.state.button_pin, 'BUTTON');
                this.button_type = this.managementService.state.button_type;
              }
              break;
            case '2312':
              if (this.supportPower) {
                this.power_pin = this.assignPin(this.managementService.state.power_pin, 'POWER');
                this.power_level = this.managementService.state.power_level;
              }
              break;
            case '2313':
              if (this.supportSD) {
                this.wait_play_finished = this.managementService.state.wait_play_finished;
                this.repeat_play = this.managementService.state.repeat_play;
              }
              break;
            case '2314':
              this.debug_serial = this.managementService.state.debug_serial;
              setTimeout(()=> {
                // Если один из занятых пинов - RX/TX - вывод в Serial недоступен
                if (this.isRxTxInUse()) this.debug_serial = false;
              }, 500);
              break;
            case '2315':
              if (this.supportMP3) {
                if (this.controller_type === 3) {
                  // В ESP32 пины подключения DFPlayer - на аппаратные RX2/TX2 - G16/G17
                  this.managementService.state.player_rx_pin = 16;
                  this.managementService.state.player_tx_pin = 17;
                  this.player_rx_pin = this.assignPin(16, 'MP3 SRX');
                  this.player_tx_pin = this.assignPin(17, 'MP3 STX');
                } else {
                  // В ESP8266 используется SoftwareSerial с назначением пинов
                  this.player_tx_pin = this.assignPin(this.managementService.state.player_tx_pin, 'MP3 STX');
                  this.player_rx_pin = this.assignPin(this.managementService.state.player_rx_pin, 'MP3 SRX');
                }
              }
              break;
            case '2316':
              if (this.supportTM1637) {
                this.tm1637_dio_pin = this.assignPin(this.managementService.state.tm1637_dio_pin, 'TM1637 DIO');
                this.tm1637_clk_pin = this.assignPin(this.managementService.state.tm1637_clk_pin, 'TM1637 CLK');
              }
              break;
          }
        }
      });

    this.numLeds = this.managementService.state.width * this.managementService.state.height;

    this.line1StartFormControl.setValidators([rangeValidator(0, this.numLeds - 1)]);
    this.line1LengthFormControl.setValidators([rangeValidator(1, this.numLeds)]);
    this.line2StartFormControl.setValidators([rangeValidator(0, this.numLeds - 1)]);
    this.line2LengthFormControl.setValidators([rangeValidator(1, this.numLeds)]);
    this.line3StartFormControl.setValidators([rangeValidator(0, this.numLeds - 1)]);
    this.line3LengthFormControl.setValidators([rangeValidator(1, this.numLeds)]);
    this.line4StartFormControl.setValidators([rangeValidator(0, this.numLeds - 1)]);
    this.line4LengthFormControl.setValidators([rangeValidator(1, this.numLeds)]);

    this.setControllerType();
  }

  private setControllerType() {
    // Тип контроллера - мнемоника и код
    this.controller = this.managementService.state.controller;
    this.controller_type = this.controller === 'ESP32' ? 3 : (this.controller === 'NodeMCU' ? 1 : 2);
    // Создать массив доступных пинов
    this.pins_free = [];
    if (this.controller_type === 1 || this.controller_type === 2) {
      // Набор пинов для ESP8266: D0(16), D1(5), D2(4), D3(0), D4(2), D5(14), D6(12), D7(13), D8(15), D9/RX(3), D10/TX(1)
      this.pins_free.push({idx:  1, use: false, name: 'D0',     alias: 'd0', gpio: 16, assign: 0})
      this.pins_free.push({idx:  2, use: false, name: 'D1',     alias: 'd1', gpio:  5, assign: 0})
      this.pins_free.push({idx:  3, use: false, name: 'D2',     alias: 'd2', gpio:  4, assign: 0})
      this.pins_free.push({idx:  4, use: false, name: 'D3',     alias: 'd3', gpio:  0, assign: 0})
      this.pins_free.push({idx:  5, use: false, name: 'D4',     alias: 'd4', gpio:  2, assign: 0})
      this.pins_free.push({idx:  6, use: false, name: 'D5',     alias: 'd5', gpio: 14, assign: 0})
      this.pins_free.push({idx:  7, use: false, name: 'D6',     alias: 'd6', gpio: 12, assign: 0})
      this.pins_free.push({idx:  8, use: false, name: 'D7',     alias: 'd7', gpio: 13, assign: 0})
      this.pins_free.push({idx:  9, use: false, name: 'D8',     alias: 'd8', gpio: 15, assign: 0})
      this.pins_free.push({idx: 10, use: false, name: 'D9/RX',  alias: 'rx', gpio:  3, assign: 0})
      this.pins_free.push({idx: 11, use: false, name: 'D10/TX', alias: 'tx', gpio:  1, assign: 0})
    }
    if (this.controller_type === 3) {
      // Набор пинов для ESP32 : 0,1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33
      this.pins_free.push({idx:  1, use: false, name: 'G0',     alias: 'g0',  gpio:  0, assign: 0})
      this.pins_free.push({idx:  2, use: false, name: 'G1/TX0', alias: 'tx',  gpio:  1, assign: 0})
      this.pins_free.push({idx:  3, use: false, name: 'G2',     alias: 'g2',  gpio:  2, assign: 0})
      this.pins_free.push({idx:  4, use: false, name: 'G3/RX0', alias: 'rx',  gpio:  3, assign: 0})
      this.pins_free.push({idx:  5, use: false, name: 'G4',     alias: 'g4',  gpio:  4, assign: 0})
      this.pins_free.push({idx:  6, use: false, name: 'G5',     alias: 'g5',  gpio:  5, assign: 0})
      this.pins_free.push({idx:  7, use: false, name: 'G12',    alias: 'g12', gpio: 12, assign: 0})
      this.pins_free.push({idx:  8, use: false, name: 'G13',    alias: 'g13', gpio: 13, assign: 0})
      this.pins_free.push({idx:  9, use: false, name: 'G14',    alias: 'g14', gpio: 14, assign: 0})
      this.pins_free.push({idx: 10, use: false, name: 'G15',    alias: 'g15', gpio: 15, assign: 0})
      this.pins_free.push({idx: 11, use: false, name: 'G16',    alias: 'g16', gpio: 16, assign: 0})
      this.pins_free.push({idx: 12, use: false, name: 'G17',    alias: 'g17', gpio: 17, assign: 0})
      this.pins_free.push({idx: 13, use: false, name: 'G18',    alias: 'g18', gpio: 18, assign: 0})
      this.pins_free.push({idx: 14, use: false, name: 'G19',    alias: 'g19', gpio: 19, assign: 0})
      this.pins_free.push({idx: 15, use: false, name: 'G21',    alias: 'g21', gpio: 21, assign: 0})
      this.pins_free.push({idx: 16, use: false, name: 'G22',    alias: 'g22', gpio: 22, assign: 0})
      this.pins_free.push({idx: 17, use: false, name: 'G23',    alias: 'g23', gpio: 23, assign: 0})
      this.pins_free.push({idx: 18, use: false, name: 'G25',    alias: 'g25', gpio: 25, assign: 0})
      this.pins_free.push({idx: 19, use: false, name: 'G26',    alias: 'g26', gpio: 26, assign: 0})
      this.pins_free.push({idx: 20, use: false, name: 'G27',    alias: 'g27', gpio: 27, assign: 0})
      this.pins_free.push({idx: 21, use: false, name: 'G32',    alias: 'g32', gpio: 32, assign: 0})
      this.pins_free.push({idx: 22, use: false, name: 'G33',    alias: 'g33', gpio: 33, assign: 0})
    }

    // Если есть какие-то назначенные пины - удалить их из массива свободных
    this.pins_assigned.forEach((pin) => {
      const idx = this.pins_free.findIndex(itm => itm.gpio === pin.gpio);
      if (idx !== -1) {
        this.pins_free.splice(idx, 1);
      }
    });

    this.rebuildFreePinsByTimer();
  }

  private getAssignmentName(code: number): string {
    const idx = this.assignment.findIndex(p => p.id === code);
    return idx < 0 ? '' : this.assignment[idx].name;
  }

  private getAssignmentCode(name: string): number {
    const idx = this.assignment.findIndex(p => p.name === name);
    return idx < 0 ? 0 : this.assignment[idx].id;
  }

  private createEmptyPin(): Pin {
    return {idx: 0, use: false, name: 'N/A', alias: 'n/a', gpio: -1, assign: 0};
  }

  private setLineParams(i: number, params: string) {
    const arr = params.split(' ');
    const gpio = Number(arr[1]);

    const line = this.lines.find(p => p.idx === i);
    if (line) {
      line.pin = this.createEmptyPin();
      line.start = Number(arr[2]);
      line.length = Number(arr[3]);
      line.startControl.setValue(line.start);
      line.lengthControl.setValue(line.length);
      if (gpio >= 0) {
        line.pin = this.assignPin(gpio, `LED LINE ${line.idx}`);
      }
    }
  }

  rebuildFreePinsByTimer() {
    if (this.timerid) clearTimeout(this.timerid);
    this.timerid = setTimeout(() => { this.rebuildFreePins(); }, 250);
  }

  private rebuildFreePins() {
    this.button_pin_list = this.updateFreePins(this.button_pin);
    this.power_pin_list = this.updateFreePins(this.power_pin);
    this.player_tx_pin_list = this.updateFreePins(this.player_tx_pin);
    this.player_rx_pin_list = this.updateFreePins(this.player_rx_pin);
    this.tm1637_dio_pin_list = this.updateFreePins(this.tm1637_dio_pin);
    this.tm1637_clk_pin_list = this.updateFreePins(this.tm1637_clk_pin);
    this.line1_pin_list = this.updateFreePins(this.lines[0].pin);
    this.line2_pin_list = this.updateFreePins(this.lines[1].pin);
    this.line3_pin_list = this.updateFreePins(this.lines[2].pin);
    this.line4_pin_list = this.updateFreePins(this.lines[3].pin);
    this.sd_clk_pin_list = this.updateFreePins(this.sd_clk_pin);
    this.sd_miso_pin_list = this.updateFreePins(this.sd_miso_pin);
    this.sd_mosi_pin_list = this.updateFreePins(this.sd_mosi_pin);
    this.sd_cs_pin_list = this.updateFreePins(this.sd_cs_pin);

    // Согласно https://github.com/vvip-68/LedPanelWiFi/wiki/Продвинутый-уровень.-Использование-портов-микроконтроллера.
    // Некоторые пины на ESP8266 не могут использоваться для устройств. Удалить эти пины из списка доступных
    if (this.controller_type !== 3) {
      let idx = -1;
      // Кнопка
      if (this.supportButton) {
        if (this.button_type === 0) {
          // Сенсорная кнопка не может работать на пинах D3(0), D4(2), D10/TX(1)
          idx = this.button_pin_list.findIndex(p => p.gpio === 0);
          if (idx !== -1) this.button_pin_list.splice(idx, 1);
          idx = this.button_pin_list.findIndex(p => p.gpio === 1);
          if (idx !== -1) this.button_pin_list.splice(idx, 1);
          idx = this.button_pin_list.findIndex(p => p.gpio === 2);
          if (idx !== -1) this.button_pin_list.splice(idx, 1);
        } else {
          // Тактовая кнопка не может работать на пинах D0(16), D8(15)
          idx = this.button_pin_list.findIndex(p => p.gpio === 15);
          if (idx !== -1) this.button_pin_list.splice(idx, 1);
          idx = this.button_pin_list.findIndex(p => p.gpio === 16);
          if (idx !== -1) this.button_pin_list.splice(idx, 1);
        }
      }
      // Управление питанием
      if (this.supportPower) {
        // Управление питанием (реле) не может работать на пинах D3(0), D4(2), D10/TX(1)
        idx = this.power_pin_list.findIndex(p => p.gpio === 0);
        if (idx !== -1) this.power_pin_list.splice(idx, 1);
        idx = this.power_pin_list.findIndex(p => p.gpio === 1);
        if (idx !== -1) this.power_pin_list.splice(idx, 1);
        idx = this.power_pin_list.findIndex(p => p.gpio === 2);
        if (idx !== -1) this.power_pin_list.splice(idx, 1);
      }
      // TM1637
      if (this.supportTM1637) {
        // TM1637 CLK и DIO не может работать на пину D8(15)
        idx = this.tm1637_clk_pin_list.findIndex(p => p.gpio === 15);
        if (idx !== -1) this.tm1637_clk_pin_list.splice(idx, 1);
        idx = this.tm1637_dio_pin_list.findIndex(p => p.gpio === 15);
        if (idx !== -1) this.tm1637_dio_pin_list.splice(idx, 1);
      }
    }
  }

  private updateFreePins(pin: Pin): Pin[] {
    const helper: Pin[] = [...this.pins_free];
    // Add 'N/A'
    let idx = helper.findIndex(p => p.gpio === -1);
    if (idx === -1) {
      helper.push(this.createEmptyPin());
    }
    // Add pin itself
    idx = helper.findIndex(p => p.gpio === pin.gpio);
    if (idx === -1) {
      helper.push(...[pin]);
    }
    helper.sort((a,b) => a.idx < b.idx ? -1 : (a.idx > b.idx ? 1 : 0) );
    return helper;
  }

  assignPin(gpio: number, name: string): Pin {
    let idx = this.pins_assigned.findIndex(p => p.gpio == gpio);
    let pin= this.createEmptyPin();
    if (idx === -1) {
      idx = this.pins_free.findIndex(p => p.gpio == gpio);
      if (idx >= 0) {
        pin = this.pins_free[idx];
        this.pins_free.splice(idx, 1);
        this.pins_assigned.push(pin);
        this.pins_assigned.sort((a, b) => a.idx < b.idx ? -1 : (a.idx > b.idx ? 1 : 0));
      }
    } else {
      pin = this.pins_assigned[idx];
    }
    if (pin.gpio >= 0) {
      pin.use = true;
      pin.assign = this.getAssignmentCode(name);
    }
    this.rebuildFreePinsByTimer();
    return pin;
  }

  unassignPin(gpio: number): Pin {
    let idx = this.pins_assigned.findIndex(p => p.gpio == gpio);
    if (idx !== -1) {
      const pin = this.pins_assigned[idx];
      this.pins_assigned.splice(idx,1);
      pin.use = false;
      pin.assign = 0;
      this.pins_free.push(pin);
      this.pins_free.sort((a,b) => a.idx < b.idx ? -1 : (a.idx > b.idx ? 1 : 0) );
    }
    return this.createEmptyPin();
  }

  getUsageName(alias: string) {
    const item = this.pins_assigned.find(itm => itm.alias === alias);
    return isNullOrUndefined(item) ? '' : this.getAssignmentName(item!.assign);
  }

  getUsage(pin: string) {
    return this.pins_assigned.findIndex(itm => itm.alias === pin) !== -1;
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  isDisabled2(): boolean {
    // Disabled когда выключено или нет соединения, а также
    // если один из пинов RX/TX назначен к использованию
    return this.isDisabled() || this.isRxTxInUse();
  }

  private isRxTxInUse(): boolean {
    // если один из пинов RX/TX назначен к использованию
    return this.pins_assigned.findIndex(p => p.alias === 'rx') !== -1 || this.pins_assigned.findIndex(p => p.alias === 'tx') !== -1;
  }

  updateRxTxUsage() {
    // Если новый пин - RX/TX - вывод в Serial-порт должен быть отключен
    if (this.isRxTxInUse()) {
      this.debug_serial = false;
    }
  }

  isSettingsValid(): boolean {
    const line1Valid = !this.lines[0].pin.use || (this.lines[0].pin.assign > 0 && this.line1StartFormControl.valid &&  this.line1LengthFormControl.valid);
    const line2Valid = !this.lines[1].pin.use || (this.lines[1].pin.assign > 0 && this.line2StartFormControl.valid &&  this.line2LengthFormControl.valid);
    const line3Valid = !this.lines[2].pin.use || (this.lines[2].pin.assign > 0 && this.line3StartFormControl.valid &&  this.line3LengthFormControl.valid);
    const line4Valid = !this.lines[3].pin.use || (this.lines[3].pin.assign > 0 && this.line4StartFormControl.valid &&  this.line4LengthFormControl.valid);
    return line1Valid && line2Valid && line3Valid && line4Valid;
  }

  applySettings($event: MouseEvent) {

    // Матрица
    // - $23 6 U1,P1,S1,L1; - подключение матрицы светодиодов линия 1
    // - $23 7 U2,P2,S2,L2; - подключение матрицы светодиодов линия 2
    // - $23 8 U3,P3,S3,L3; - подключение матрицы светодиодов линия 3
    // - $23 9 U4,P4,S4,L4; - подключение матрицы светодиодов линия 4
    //         Ux - 1 - использовать линию, 0 - линия не используется
    //         Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
    //         Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
    //         Lx - длина цепочки светодиодов, подключенной к линии x
    let pin = this.lines[0].pin.gpio;
    let use = this.lines[0].pin.use && pin >= 0;
    let sta = this.lines[0].start = Number(this.lines[0].startControl.value);
    let len = this.lines[0].length = Number(this.lines[0].lengthControl.value);
    this.socketService.sendText(`$23 6 ${use ? 1 : 0} ${use ? pin : -1} ${use ? sta : -1} ${use ? len : -1};`);

    pin = this.lines[1].pin.gpio;
    use = this.lines[1].pin.use && pin >= 0;
    sta = this.lines[1].start = Number(this.lines[1].startControl.value);
    len = this.lines[1].length = Number(this.lines[1].lengthControl.value);
    this.socketService.sendText(`$23 7 ${use ? 1 : 0} ${use ? pin : -1} ${use ? sta : -1} ${use ? len : -1};`);

    pin = this.lines[2].pin.gpio;
    use = this.lines[2].pin.use && pin >= 0;
    sta = this.lines[2].start = Number(this.lines[2].startControl.value);
    len = this.lines[2].length = Number(this.lines[2].lengthControl.value);
    this.socketService.sendText(`$23 8 ${use ? 1 : 0} ${use ? pin : -1} ${use ? sta : -1} ${use ? len : -1};`);

    pin = this.lines[3].pin.gpio;
    use = this.lines[3].pin.use && pin >= 0;
    sta = this.lines[3].start = Number(this.lines[3].startControl.value);
    len = this.lines[3].length = Number(this.lines[3].lengthControl.value);
    this.socketService.sendText(`$23 9 ${use ? 1 : 0} ${use ? pin : -1} ${use ? sta : -1} ${use ? len : -1};`);

    // Кнопка
    // - $23 11 X Y; - кнопка
    //         X - GPIO пин к которому подключена кнопка
    //         Y - BUTTON_TYPE - 0 - сенсорная кнопка, 1 - тактовая кнопка
    if (this.supportButton) {
      // button_pin_list содержит список свободных пинов, сам пин, плюс N/A
      // Тактовая и скнсорная кнопка имеют разный спписок запрещенных пинов и может случиться так, что при смене типа кнопки
      // назначение пина сохранится в button_pin, но в списке button_pin_list его не будет. В этом случае - принять значение как N/A
      if (this.button_pin.gpio !== -1) {
        const idx = this.button_pin_list.findIndex(p => p.gpio === this.button_pin.gpio);
        if (idx === -1) {
          this.button_pin = this.unassignPin(this.button_pin.gpio);
        }
      }
      this.socketService.sendText(`$23 11 ${this.button_pin.gpio} ${this.button_type};`);
    }

    // Управление питанием
    // - $23 12 X Y; - управление питанием
    //         X - GPIO пин к которому подключено реле управления питанием
    //         Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
    if (this.supportPower) {
      this.socketService.sendText(`$23 12 ${this.power_pin.gpio} ${this.power_level};`);
    }

    // SD-карта
    // - $23 13 X Y; - для SD-карты - алгоритм воспроизведения ролика
    //         X - WAIT_PLAY_FINISHED - алгоритм проигрывания эффекта SD-карта
    //         Y - REPEAT_PLAY - алгоритм проигрывания эффекта SD-карта
    if (this.supportSD) {
      this.socketService.sendText(`$23 13 ${this.wait_play_finished ? 1 : 0} ${this.repeat_play ? 1 : 0};`);
    }

    // MP3 DFPlayer
    // - $23 15 X Y;  - Подключение пинов MP3 DFPlayer
    //         X - STX - GPIO пин контроллера TX -> к RX плеера
    //         Y - SRX - GPIO пин контроллера RX -> к TX плеера
    if (this.supportMP3) {
      this.socketService.sendText(`$23 15 ${this.player_tx_pin.gpio} ${this.player_rx_pin.gpio};`);
    }

    // TM1637
    // - $23 16 X Y;  - Подключение пинов TM1637
    //         X - DIO - GPIO пин контроллера к DIO индикатора
    //         Y - CLK - GPIO пин контроллера к CLK индикатора
    if (this.supportTM1637) {
      this.socketService.sendText(`$23 16 ${this.tm1637_dio_pin.gpio} ${this.tm1637_clk_pin.gpio};`);
    }

    // Тип матрицы - труба/панель - DEVICE_TYPE
    // - $23 10 X; - X - DEVICE_TYPE - 0 - труба, 1 - плоская панель
    this.socketService.sendText(`$23 10 ${this.device_type};`);


    // Вывод в монитор порта - DEBUG_SERIAL
    // - $23 14 X;  - Вкл/выкл отладочного вывода в монитор порта DEBUG_SERIAL - 1-вкл, 0-выкл
    this.socketService.sendText(`$23 14 ${this.debug_serial ? 1 : 0};`);


    // Перезагрузить контроллер
    // - $23 4; - перезапуск устройства (reset/restart/reboot)
    this.socketService.sendText('$23 4;');
  }

  goToLink() {
    // @ts-ignore
    window.open('https://github.com/vvip-68/LedPanelWiFi/wiki/Продвинутый-уровень.-Использование-портов-микроконтроллера.', '_blank').focus();
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
