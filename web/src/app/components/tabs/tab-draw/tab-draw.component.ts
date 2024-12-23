import {AfterViewInit, Component, ElementRef, Inject, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {debounceTime, fromEvent, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {MatDialog} from "@angular/material/dialog";
import { DOCUMENT, NgClass } from "@angular/common";
import {distinctUntilChanged} from "rxjs/operators";
import {HSLA, HSVA, RGBA} from "ngx-color";
import {isNullOrUndefined, isNullOrUndefinedOrEmpty} from "../../../services/helper";
import {ConfirmationDialogComponent} from "../../confirmation-dialog/confirmation-dialog.component";
import {ComboBoxItem} from "../../../models/combo-box.model";
import { MatInputModule } from '@angular/material/input';
import { MatButtonModule } from '@angular/material/button';
import { MatOptionModule } from '@angular/material/core';
import { MatSelectModule } from '@angular/material/select';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { FormsModule } from '@angular/forms';
import { MatRadioModule } from '@angular/material/radio';
import { MatFormFieldModule } from '@angular/material/form-field';
import { ColorCircleModule } from 'ngx-color/circle';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatIconModule } from '@angular/material/icon';
import { ColorPhotoshopModule } from 'ngx-color/photoshop';
import {Base} from "../../base.class";

@Component({
    selector: 'app-tab-draw',
    templateUrl: './tab-draw.component.html',
    styleUrls: ['./tab-draw.component.scss'],
    standalone: true,
    imports: [
        ColorPhotoshopModule,
        MatIconModule,
        MatTooltipModule,
        NgClass,
        ColorCircleModule,
        MatFormFieldModule,
        MatRadioModule,
        FormsModule,
        DisableControlDirective,
        MatSelectModule,
        MatOptionModule,
        MatButtonModule,
        MatInputModule,
    ],
})
export class TabDrawComponent extends Base implements OnInit, OnDestroy, AfterViewInit {

  @ViewChild('canvas_container') canvasContainer!: ElementRef;
  @ViewChild('canvas') canvas!: ElementRef;

  swatch = [
    "#f44336", "#e91e63", "#9c27b0", "#673ab7", "#3f51b5", "#2196f3", "#03a9f4", "#00bcd4",
    "#009688", "#4caf50", "#8bc34a", "#cddc39", "#ffeb3b", "#ffc107", "#ff9800", "#ff5722",
    "#795548", "#607d8b", "#ffffff", "#000000"
  ];

  reflect_horiz: boolean = false;
  reflect_vert:  boolean = false;
  reflect_diag:  boolean = false;

  active_tool: number = 1;    // 0 - пипетка; 1 - кисть; 2 - ластик
  picture_file: number = -1;  // Выбранный из сохраненных файл картинки
  file_name: string = "";

  mouseDown: boolean = false;
  mouseCellX: number = -1;
  mouseCellY: number = -1;

  get brushColor(): HSLA | HSVA | RGBA | string  {
    return this._brushColor;
  }

  set brushColor(value: HSLA | HSVA | RGBA | string) {
    this._brushColor = value;
    // $5 0 RRGGBB; - установить активный цвет рисования в формате RRGGBB
    this.socketService.sendText(`$5 0 ${value.toString().substring(1)};`);
  }
  _brushColor: HSLA | HSVA | RGBA | string = '#ffffff';

  cellSize: number = 0;

  fs_allow: boolean = true;
  sd_allow: boolean = false;
  storage: number = 0;        // 0 - FS; 1 - SD

  private loadType: number = -1;      // 0 - загрузка по строкам; 1 - загрузка по колонкам
  private loadIndex: number = -1;     // индекс загружаемой строки / колонки
  private intervalId: any = undefined;

  constructor(@Inject(DOCUMENT) private document: Document,
              public socketService: WebsocketService,
              public managementService: ManagementService,
              public commonService: CommonService,
              public L: LanguagesService,
              private dialog: MatDialog) {
    super();
  }

  ngOnInit() {
    fromEvent(window, 'resize')
      .pipe(takeUntil(this.destroy$), debounceTime(250))
      .subscribe( evt => { this.drawCanvas(); } );

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          // При первом соединении сокета с устройством запросить параметры, используемые в главном экране
          const request: string = `FS|SX|CL|CRF${this.storage}`;
          this.managementService.getKeys(request);
        }
        // Если соединение пропало во время загрузки картинки - остановить
        this.stopLoadImage();
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'L':
              // Пришло подтверждение приема отправленной строки изображения
              if (this.managementService.state.image_line.length > 0) {
                const row = Number(this.managementService.state.image_line);
                this.loadIndex = row + 1;
                if (this.loadIndex >= this.managementService.state.height) {
                  this.stopLoadImage();
                } else {
                  this.sendImageLine();
                }
              }
              break;
            case 'C':
              // Пришло подтверждение приема отправленной колонки изображения
              if (this.managementService.state.image_line.length > 0) {
                const col = Number(this.managementService.state.image_line);
                this.loadIndex = col + 1;
                if (this.loadIndex >= this.managementService.state.width) {
                  this.stopLoadImage();
                } else {
                  this.sendImageLine();
                }
              }
              break;
            case 'IC':
              // Получена колонка картинки матрицы: N|RRGGBB,RRGGBB,...,RRGGBB
              if (this.managementService.state.image_line.length > 0) {
                const parts = this.managementService.state.image_line.split('|');
                if (parts.length === 2) {
                  const col = Number(parts[0]);
                  const colors = parts[1].split(',');
                  const canvas = this.canvas.nativeElement;
                  const ctx = canvas.getContext("2d");
                  for (let i = 0; i < this.managementService.state.height; i++) {
                    const color = `#${colors[i]}`;
                    this.managementService.matrixColors[col][i] = color;
                    this.paintCell(col, i, color, ctx);
                  }
                  this.loadIndex = col + 1;
                  if (this.loadIndex >= this.managementService.state.width) {
                    this.stopLoadImage();
                  } else {
                    this.sendRequestLine();
                  }
                } else {
                  this.stopLoadImage();
                }
              }
              break;
            case 'IR':
              // Получена строка картинки матрицы: N|RRGGBB,RRGGBB,...,RRGGBB
              if (this.managementService.state.image_line.length > 0) {
                const parts = this.managementService.state.image_line.split('|');
                if (parts.length === 2) {
                  const row = Number(parts[0]);
                  const colors = parts[1].split(',');
                  const canvas = this.canvas.nativeElement;
                  const ctx = canvas.getContext("2d");
                  for (let i = 0; i < this.managementService.state.width; i++) {
                    const color = `#${colors[i]}`;
                    this.managementService.matrixColors[i][row] = color;
                    this.paintCell(i, row, color, ctx);
                  }
                  this.loadIndex = row + 1;
                  if (this.loadIndex >= this.managementService.state.height) {
                    this.stopLoadImage();
                  } else {
                    this.sendRequestLine();
                  }
                } else {
                  this.stopLoadImage();
                }
              }
              break;
            case 'W':
            case 'H':
              this.createMatrix();
              this.drawCanvas();
              break;
            case 'FS': // доступность внутренней файловой системы микроконтроллера для хранения файлов: 0 - нет, 1 - да
              this.fs_allow = this.managementService.state.fs_allow;
              if (this.storage === 0 && !this.fs_allow) this.storage = 1;
              break;
            case 'SX': // наличие и доступность SD карты в системе: Х = 0 - нат SD карты; 1 - SD карта доступна
              this.sd_allow = this.managementService.state.sd_allow;
              if (this.storage === 1 && !this.sd_allow) this.storage = 0;
              break;
            case 'CL':
              this._brushColor = this.managementService.state.color;
              break;
          }
        }
      })

    this.fs_allow = this.managementService.state.fs_allow;
    if (this.storage === 0 && !this.fs_allow) this.storage = 1;
    this.sd_allow = this.managementService.state.sd_allow;
    if (this.storage === 1 && !this.sd_allow) this.storage = 0;
  }

  ngAfterViewInit() {
    this.createMatrix();
    this.drawCanvas();

    // Получить список картинок, найденных в файловой системе или на SD карте в зависимости от
    // текущего выбранного хранилища
    setTimeout(() => {
      this.managementService.getKeys(`CRF${this.storage}`);
    }, 500);
  }

  isDisabled(): boolean {
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
  }

  private createMatrix() {
    this.managementService.createMatrix();
  }

  private drawCanvas() {
    const canvas = this.canvas.nativeElement;
    const isMobile = navigator.userAgent.indexOf('Android') >= 0 || navigator.userAgent.indexOf('iPhone') >= 0;
    const width =  Math.min(960, document.body.clientWidth - (isMobile ? 24 : 40));
    const height = document.body.clientHeight - 175;

    const w = this.managementService.state.width;
    const h = this.managementService.state.height;

    const cellWidth = width / w;
    const cellHeight = height / h;

    let cellSize = Math.min(cellWidth, cellHeight);
    if (cellSize > 35) cellSize = 35;

    const matrixWidth = w * cellSize;
    const matrixHeight = h * cellSize;

    this.cellSize = cellSize;

    const ctx = canvas.getContext("2d");
    if (ctx !== null) {

      canvas.width = matrixWidth;
      canvas.height = matrixHeight;

      ctx.fillStyle = "#000";
      ctx.strokeStyle = "#888888"
      ctx.lineWidth = 1;

      ctx.beginPath();
      ctx.rect(0, 0, matrixWidth, matrixHeight);
      ctx.closePath();
      ctx.fill();
      ctx.stroke();

      for (let i = 1; i <= this.managementService.state.width; i++) {
        ctx.beginPath();
        ctx.moveTo(i * cellSize, 0);
        ctx.lineTo(i * cellSize, matrixHeight - 1);
        ctx.stroke();
      }

      for (let i = 1; i <= this.managementService.state.height; i++) {
        ctx.beginPath();
        ctx.moveTo(0, i * cellSize);
        ctx.lineTo(matrixWidth - 1, i * cellSize);
        ctx.stroke();
      }

      for (let y= 0; y < h; y++) {
        for (let x= 0; x < w; x++) {
          this.paintCell(x, y, this.managementService.matrixColors[x][y], ctx);
        }
      }
    }
  }

  paintCell(x: number, y: number, c: string, ctx?: CanvasRenderingContext2D) {

    if (isNullOrUndefined(ctx)) {
      const canvas = this.canvas.nativeElement;
      ctx = canvas.getContext("2d");
    }

    ctx!.fillStyle = c;
    ctx!.strokeStyle = "#888888"

    const xx = x * this.cellSize;
    const yy = y * this.cellSize;
    ctx!.beginPath();
    ctx!.rect(xx, yy, this.cellSize, this.cellSize);
    ctx!.closePath();
    ctx!.fill();
    ctx!.stroke();
  }

  canvasMouseDown($event: MouseEvent) {
    this.mouseDown = true;
    // @ts-ignore
    const x = $event.layerX - $event.target.offsetLeft;
    // @ts-ignore
    const y = $event.layerY - $event.target.offsetTop;
    const cellX = Math.trunc(x / this.cellSize);
    const cellY = Math.trunc(y / this.cellSize);
    this.mouseCellX = cellX;
    this.mouseCellY = cellY;
    this.canvasAction(cellX, cellY)
  }

  canvasTouchDown($event: TouchEvent) {
    if (!$event.touches) return;
    this.mouseDown = true;
    // @ts-ignore
    const rect = $event.touches[0].target.getBoundingClientRect();
    // @ts-ignore
    const x = $event.touches[0].pageX - rect.left;
    // @ts-ignore
    const y = $event.touches[0].pageY - rect.top;
    const cellX = Math.trunc(x / this.cellSize);
    const cellY = Math.trunc(y / this.cellSize);
    this.mouseCellX = cellX;
    this.mouseCellY = cellY;
    this.canvasAction(cellX, cellY)
  }

  canvasMouseUp($event: MouseEvent) {
    this.mouseDown = false;
    this.mouseCellX = -1;
    this.mouseCellY = -1;
  }

  canvasTouchUp($event: TouchEvent) {
    this.mouseDown = false;
    this.mouseCellX = -1;
    this.mouseCellY = -1;
  }

  canvasMouseMove($event: MouseEvent) {
    if (!this.mouseDown) return;
    // @ts-ignore
    const x = $event.layerX - $event.target.offsetLeft;
    // @ts-ignore
    const y = $event.layerY - $event.target.offsetTop;
    const cellX = Math.trunc(x / this.cellSize);
    const cellY = Math.trunc(y / this.cellSize);
    // Если мышка нажата и находится в области рисования и позиция не совпадает с предыдущей - выполнить действие для новой ячейки - рисовать / стирать
    if (cellX >= 0 && cellY >= 0 &&  cellX < this.managementService.state.width && cellY < this.managementService.state.height && (cellX !== this.mouseCellX || cellY !== this.mouseCellY)) {
      this.mouseCellX = cellX;
      this.mouseCellY = cellY;
      this.canvasAction(cellX, cellY);
    }
    $event.stopPropagation();
    $event.preventDefault();
  }

  canvasTouchMove($event: TouchEvent) {
    if (!this.mouseDown) return;
    if (!$event.touches) return;
    // @ts-ignore
    const rect = $event.touches[0].target.getBoundingClientRect();
    // @ts-ignore
    const x = $event.touches[0].pageX - rect.left;
    // @ts-ignore
    const y = $event.touches[0].pageY - rect.top;
    const cellX = Math.trunc(x / this.cellSize);
    const cellY = Math.trunc(y / this.cellSize);
    // Если мышка нажата и находится в области рисования и позиция не совпадает с предыдущей - выполнить действие для новой ячейки - рисовать / стирать
    if (cellX >= 0 && cellY >= 0 &&  cellX < this.managementService.state.width && cellY < this.managementService.state.height && (cellX !== this.mouseCellX || cellY !== this.mouseCellY)) {
      this.mouseCellX = cellX;
      this.mouseCellY = cellY;
      this.canvasAction(cellX, cellY);
    }
    $event.stopPropagation();
    $event.preventDefault();
  }

  canvasOutMouseMove($event: MouseEvent) {
    this.mouseDown = false;
    this.mouseCellX = -1;
    this.mouseCellY = -1;
  }

  canvasAction(cellX: number, cellY: number) {
    if (cellX < 0 || cellX >= this.managementService.state.width || cellY < 0 || cellY >= this.managementService.state.height) return;

    if (this.active_tool === 0) {
      // Пипетка
      this.brushColor = this.managementService.matrixColors[cellX][cellY];
      this.active_tool = 1; // Вернуться к инструменту 'кисть'
    } else

    if (this.active_tool === 1 || this.active_tool === 2) {
      // 1 - кисть
      // 2 - ластик
      const color = this.active_tool === 1 ? this.brushColor.toString() : '#000000';
      this.managementService.matrixColors[cellX][cellY] = color;
      this.paintCell(cellX, cellY, color);

      const w = this.managementService.state.width;
      const h = this.managementService.state.height;

      // На матрице устройства точка 0,0 - левый нижний угол.
      // На матрице Web-страницы - левый верхний угол
      // При передаче команды на матрицу корректировать координату по Y


      // $5 3 X Y;    - рисовать точку активным цветом рисования в позицию X Y
      // $5 6 X Y;    - рисовать точку черным в позицию X Y - стереть точку - ластик
      this.socketService.sendText(`$5 ${this.active_tool === 1 ? '3' : '6'} ${cellX} ${h - cellY - 1};`);

      // Если включен режим симметричного рисования - отражения - рисовать отраженные точки
      if (this.reflect_horiz) {
        this.managementService.matrixColors[w - cellX - 1][cellY] = color;
        this.paintCell(w - cellX - 1, cellY, color);
        this.socketService.sendText(`$5 ${this.active_tool === 1 ? '3' : '6'} ${w - cellX - 1} ${h - cellY - 1};`);
      }
      if (this.reflect_vert) {
        this.managementService.matrixColors[cellX][h - cellY - 1] = color;
        this.paintCell(cellX, h - cellY - 1, color);
        this.socketService.sendText(`$5 ${this.active_tool === 1 ? '3' : '6'} ${cellX} ${cellY};`);
      }
      if (this.reflect_diag) {
        this.managementService.matrixColors[w - cellX - 1][h - cellY - 1] = color;
        this.paintCell(w - cellX - 1, h - cellY - 1, color);
        this.socketService.sendText(`$5 ${this.active_tool === 1 ? '3' : '6'} ${w - cellX - 1} ${cellY};`);
      }
    }
  }

  loadFromMatrix() {
    // Если ширина матрицы меньше высоты - загрузка по строкам
    // Если ширина матрицы больше высоты - загрузка по колонкам
    const w = this.managementService.state.width;
    const h = this.managementService.state.height;
    this.loadType = w <= h ? 0 : 1;
    this.loadIndex = 0;
    this.sendRequestLine();
  }

  sendToMatrix() {
    // Если ширина матрицы меньше высоты - загрузка по строкам
    // Если ширина матрицы больше высоты - загрузка по колонкам
    const w = this.managementService.state.width;
    const h = this.managementService.state.height;
    this.loadType = w <= h ? 0 : 1;
    this.loadIndex = 0;
    this.sendImageLine();
  }

  sendRequestLine() {
    // Параметры запроса указывают на то, что он активен?
    if (this.loadType < 0 || this.loadIndex < 0) {
      this.stopLoadImage();
      return;
    }
    // Завершить ожидание предыдущей запрошенной строки / колонки
    if (!isNullOrUndefined(this.intervalId)) {
      clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
    // Отправить запрос на получение строки изображения
    this.socketService.sendText(`$5 4 ${this.loadType} ${this.loadIndex};`);
    // Отложенный запрос, на случай, если запрос, отправленной строкой выше не будет выполнен - отправить повторно
    this.intervalId = setInterval(() => {
      if (this.loadType < 0 || this.loadIndex < 0) {
        this.stopLoadImage();
      } else {
        this.socketService.sendText(`$5 4 ${this.loadType} ${this.loadIndex};`);
      }
    }, 500);
  }

  sendImageLine() {
    // Параметры запроса указывают на то, что он активен?
    if (this.loadType < 0 || this.loadIndex < 0) {
      this.stopLoadImage();
      return;
    }
    // Завершить ожидание предыдущей запрошенной строки / колонки
    if (!isNullOrUndefined(this.intervalId)) {
      clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
    // 11 - картинка построчно $6 11|Y colorHEX,colorHEX,...,colorHEX
    // 12 - картинка по колонкам $6 12|X colorHEX,colorHEX,...,colorHEX
    let line = '';
    if (this.loadType === 0) {
      // Отправка строки изображения
      for (let i = 0; i < this.managementService.state.width; i++) {
        line += this.managementService.matrixColors[i][this.loadIndex].substring(1) + ',';
      }
    } else {
      // Отправка колонки изображения
      for (let i = 0; i < this.managementService.state.height; i++) {
        line += this.managementService.matrixColors[this.loadIndex][i].substring(1) + ',';
      }
    }
    line = line.substring(0,line.length - 1);
    this.socketService.sendText(`$6 ${this.loadType === 0 ? 11 : 12}|${this.loadIndex} ${line}`);
    // Отложенный запрос, на случай, если запрос, отправленной строкой выше не будет выполнен - отправить повторно
    this.intervalId = setInterval(() => {
      if (this.loadType < 0 || this.loadIndex < 0) {
        this.stopLoadImage();
      } else {
        this.socketService.sendText(`$6 ${this.loadType === 0 ? 11 : 12}|${this.loadIndex} ${line}`);
      }
    }, 500);
  }

  stopLoadImage() {
    if (!isNullOrUndefined(this.intervalId)) {
      clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
    this.loadType = -1;
    this.loadIndex = -1;
  }

  clearMatrix() {
    const w = this.managementService.state.width;
    const h = this.managementService.state.height;
    const color = '#000000';

    // $5 1;        - очистить матрицу (заливка черным)
    this.socketService.sendText(`$5 1;`);

    for (let y= 0; y < h; y++) {
      for (let x= 0; x < w; x++) {
        this.managementService.matrixColors[x][y] = color;
        this.paintCell(x, y, this.managementService.matrixColors[x][y]);
      }
    }
  }

  fillMatrix() {
    const w = this.managementService.state.width;
    const h = this.managementService.state.height;
    const color = this.brushColor.toString();

    // $5 2;        - заливка матрицы активным цветом рисования
    this.socketService.sendText(`$5 2;`);

    for (let y= 0; y < h; y++) {
      for (let x= 0; x < w; x++) {
        this.managementService.matrixColors[x][y] = color;
        this.paintCell(x, y, this.managementService.matrixColors[x][y]);
      }
    }
  }

  changeStorage() {
    this.managementService.getKeys(`CRF${this.storage}`);
    this.picture_file = -1;
  }

  loadPicture() {
    // $6 15|ST|файл - Загрузить пользовательскую картинку из файла на матрицу; $6 15|ST|filename; ST - "FS" - файловая система; "SD" - карточка
    const fileItem = this.managementService.picture_list.find((item) => item.value === this.picture_file);
    if (!isNullOrUndefined(fileItem)) {
      this.socketService.sendText(`$6 15|${this.storage === 0 ? 'FS' : 'SD'}|${fileItem!.displayText}`);
    }
    this.loadFromMatrix();
  }

  deletePicture() {
    const fileItem = this.managementService.picture_list.find((item) => item.value === this.picture_file);
    if (isNullOrUndefined(fileItem)) return;

    const dialog = this.dialog.open(ConfirmationDialogComponent, {
      width: '400px',
      panelClass: 'centralized-dialog-content',
      data: {title: this.L.$('Удалить картинку'), message: this.L.$('Удалить картинку') + ' \'' + fileItem!.displayText + '\' ' + this.L.$('из хранилища?')}
    });

    dialog.afterClosed()
      .subscribe(result => {
        if (result === true) {
          // $6 17|ST|файл - Удалить файл изображения $6 17|ST|filename; ST - "FS" - файловая система; "SD" - карточка
          this.socketService.sendText(`$6 17|${this.storage === 0 ? 'FS' : 'SD'}|${fileItem!.displayText}`);
          const idx = this.managementService.picture_list.indexOf(fileItem!);
          if (idx >= 0) {
            this.managementService.picture_list.splice(idx, 1);
            this.picture_file = -1;
          }
        }
      });
  }

  savePicture(value: string) {
    // $6 16|ST|файл - Сохранить текущее изображение с матрицы в файл $6 16|ST|filename; ST - "FS" - файловая система; "SD" - карточка
    this.socketService.sendText(`$6 16|${this.storage === 0 ? 'FS' : 'SD'}|${value}`);
    const file = this.managementService.picture_list.find((item) => item.displayText === value);
    if (isNullOrUndefined(file)) {
      this.managementService.picture_list.push(new ComboBoxItem(value, this.managementService.picture_list.length));
      this.managementService.sortPictureList();
    }
  }

  isValidFileName(value: string): boolean {
    return value.length > 0 && value.length < 16 && !(/[^a-zA-Z0-9_!\-]/.test(value));
  }

  click($event: MouseEvent) {
    const target = $event.currentTarget as HTMLElement;
    if (target) {
      target.classList.add('clicked');
      setTimeout(() => { target.classList.remove('clicked')}, 250);
    }

  }

  override ngOnDestroy() {
    this.stopLoadImage();
    super.ngOnDestroy();
  }
}
