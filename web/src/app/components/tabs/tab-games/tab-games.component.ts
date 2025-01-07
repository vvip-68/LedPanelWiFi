import {Component, HostListener, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, takeUntil, timer} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {isNullOrUndefinedOrEmpty} from "../../../services/helper";
import { MatIconModule } from '@angular/material/icon';
import { NgClass } from '@angular/common';
import { MatSliderModule } from '@angular/material/slider';
import { MatFormFieldModule } from '@angular/material/form-field';
import {Base} from "../../base.class";
import {MatCheckbox} from "@angular/material/checkbox";
import {FormsModule} from "@angular/forms";

@Component({
    selector: 'app-tab-games',
    templateUrl: './tab-games.component.html',
    styleUrls: ['./tab-games.component.scss'],
    standalone: true,
  imports: [
    MatFormFieldModule,
    MatSliderModule,
    NgClass,
    MatIconModule,
    MatCheckbox,
    FormsModule,
  ],
})
export class TabGamesComponent extends Base implements OnInit, OnDestroy {

  public bright: number = -1;
  public speed: number = -1;
  public repeat: number = 0;
  public invert_left_right: boolean = false;

  private brightChanged$ = new BehaviorSubject(this.bright);
  private speedChanged$ = new BehaviorSubject(this.speed);
  private repeatChanged$ = new BehaviorSubject(this.repeat);

  private key_code: number = 0;   // 0 - не нажата; 1 - вверх; 2 - вправо; 3 - вниз; 4- влево; 5 - центр
  private last_time: number = Date.now();

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
          let request = 'BR|SE|BS';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'BR':
              this.bright = this.managementService.state.brightness;
              break;
            case 'SE':
              this.speed = this.managementService.state.game_speed;
              break;
            case 'BS':
              this.repeat = this.managementService.state.game_button_speed * 10;
              break;
          }
        }
      });

    this.brightChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (value >= 0) {
          this.bright = value;
          // $4 0 D; установить текущий уровень общей яркости; D - яркость в диапазоне 1..255
          this.socketService.sendText(`$4 0 ${value};`);
        }
      });

    this.speedChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (value >= 0) {
          this.speed = value;
          // - $15 D N; D - скорость 0..255; N - таймер, где N 0 - таймер эффектов
          this.socketService.sendText(`$15 ${value} 0;`);
        }
      });

    this.repeatChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (value >= 100) {
          this.repeat = value;
          // - $3 15 D; D - скорость 10..100;
          this.socketService.sendText(`$3 15 ${(value / 10).toFixed(0)};`);
        }
      });

    timer(100, 100)
      .pipe(takeUntil(this.destroy$))
      .subscribe(() => {
        const time = Date.now();
        if (time - this.last_time > this.repeat) {
          this.repeatAction();
        }
      });
  }

  formatBrightness(value: any) {
    const percent = Math.round(Number(value) * 100 / 255);
    return percent + (percent === 100 ? '' : '%');
  }

  formatLabel(value: number) {
    return value.toString();
  }

  isDisabled(): boolean {
    // @formatter:off
    return !this.managementService.state.power ||
           !this.socketService.isConnected ||
            this.managementService.state.isNightClockRunnung() ||
            (!(this.managementService.state.isGameSnake() ||
               this.managementService.state.isGameMaze()  ||
               this.managementService.state.isGameTetris() ||
               this.managementService.state.isGameArkanoid()));
    // @formatter:on
  }

  brightChanged(value: number) {
    this.brightChanged$.next(value);
  }

  speedChanged(value: number) {
    this.speedChanged$.next(value);
  }

  repeatChanged(value: number) {
    this.repeatChanged$.next(value);
  }

  switchToMaze() {
    // - $3 1; - включить игру "Лабиринт" в режиме ожидания начала игры
    this.socketService.sendText('$3 1;');
  }

  switchToSnake() {
    // - $3 2; - включить игру "Змейка" в режиме ожидания начала игры
    this.socketService.sendText('$3 2;');
  }

  switchToTetris() {
    // - $3 3; - включить игру "Тетрис" в режиме ожидания начала игры
    this.socketService.sendText('$3 3;');
  }

  switchToArkanoid() {
    // - $3 4; - включить игру "Арканоид" в режиме ожидания начала игры
    this.socketService.sendText('$3 4;');
  }

  repeatAction() {
    if (this.isDisabled()) {
      return;
    }
    switch (this.key_code) {
      case 1:
        this.btnUp();
        break;
      case 2:
        this.btnRight();
        break;
      case 3:
        this.btnDown();
        break;
      case 4:
        this.btnLeft();
        break;
      case 5:
        this.btnCenter();
        break;
      default:
        return;
    }
    this.last_time = Date.now();
  }

  btnUp() {
    // $3 10 - кнопка вверх
    this.socketService.sendText('$3 10;');
  }

  btnRight() {
    // $3 11 - кнопка вправо
    if (this.invert_left_right)
      this.socketService.sendText('$3 13;');
    else
      this.socketService.sendText('$3 11;');
  }

  btnDown() {
    // $3 12 - кнопка вниз
    this.socketService.sendText('$3 12;');
  }

  btnLeft() {
    // $3 13 - кнопка влево
    if (this.invert_left_right)
      this.socketService.sendText('$3 11;');
    else
      this.socketService.sendText('$3 13;');
  }

  btnCenter() {
    // $3 14 - кнопка вверх
    this.socketService.sendText('$3 14;');
  }

  btnPress(value: number) {
    this.key_code = value;
    this.last_time = Date.now() - 2000;
  }

  btnRelease() {
    this.key_code = 0;
  }

  @HostListener('window:keydown', ['$event'])
  onKeyDown(event: KeyboardEvent): void {
    switch (event.key) {
      case 'ArrowLeft':
        this.btnPress(4);
        break;
      case 'ArrowRight':
        this.btnPress(2);
        break;
      case 'ArrowUp':
        this.btnPress(1);
        break;
      case 'ArrowDown':
        this.btnPress(3);
        break;
      case 'Enter':
        this.btnPress(5);
        break;
      case '+':
        if (this.speed <= 250) {
          this.speed += 5;
          this.speedChanged$.next(this.speed);
        }
        break;
      case '-':
        if (this.speed >= 10) {
          this.speed -= 5;
          this.speedChanged$.next(this.speed);
        }
        break;
    }
  }

  @HostListener('window:keyup', ['$event'])
  onKeyUp(event: KeyboardEvent): void {
    switch (event.key) {
      case 'ArrowLeft':
      case 'ArrowRight':
      case 'ArrowUp':
      case 'ArrowDown':
      case 'Enter':
        this.btnRelease();
        break;
    }
  }

}
