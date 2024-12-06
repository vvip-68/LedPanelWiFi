import {Component, EventEmitter, Input, OnDestroy, OnInit, Output} from '@angular/core';
import {LanguagesService} from '../../services/languages/languages.service';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatIconModule } from '@angular/material/icon';
import { NgClass } from '@angular/common';
import {Base} from "../base.class";

@Component({
    selector: 'app-plugger',
    templateUrl: './plugger.component.html',
    styleUrls: ['./plugger.component.scss'],
    standalone: true,
    imports: [
        NgClass,
        MatIconModule,
        MatTooltipModule,
    ],
})
export class PluggerComponent extends Base implements OnInit, OnDestroy {

  @Input()
  public set type(value: number) {
    this._type = value === 2 ? 0 : value;
  }
  private _type: number = -1;

  public get type(): number {
    return this._type;
  }

  // @formatter:off
  @Output() typeChange = new EventEmitter<number>();
  @Input()  angle: number = -1;
  @Output() angleChange = new EventEmitter<number>();
  @Input()  direction: number = -1;
  @Output() directionChange = new EventEmitter<number>();
  @Input()  disabled: boolean = false;
  // @formatter:oт

  constructor(public L: LanguagesService) {
    super();
  }

  ngOnInit() {
  }

  // type      - тип соединения диодов в сегменте матрицы: 0 - зигзаг, 1 - параллельная, 2 - использовать карту индексов
  // angle     - угол подключения диодов в сегменте: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний | для MATRIX_TYPE == 2 - не используется
  // direction - направление ленты из угла сегмента: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз                                 | для MATRIX_TYPE == 2 - не используется

  setAngle(angle: number) {
    this.angle = angle;
    this.angleChange.emit(angle);
    switch (angle) {
      case 0:    // левый нижний
        if (!(this.direction === 0 || this.direction === 1)) {  // вправо или вверх
          this.direction = 0;
          this.directionChange.emit(this.direction);
        }
        break;
      case 1:    // левый верхний
        if (!(this.direction === 0 || this.direction === 3)) {  // вправо или вниз
          this.direction = 0;
          this.directionChange.emit(this.direction);
        }
        break;
      case 2:    // правый верхний
        if (!(this.direction === 2 || this.direction === 3)) {  // влево или вниз
          this.direction = 2;
          this.directionChange.emit(this.direction);
        }
        break;
      case 3:    // правый нижний
        if (!(this.direction === 2 || this.direction === 1)) {  // влево или вверх
          this.direction = 2;
          this.directionChange.emit(this.direction);
        }
        break;
    }
  }

  setDirectionAndAngle(dir: number, angle: number) {
    this.direction = dir;
    this.directionChange.emit(dir);
    this.setAngle(angle);
  }

  setMatrixType() {
    this.type = this.type === 0 ? 1 : 0;
  }

  getMatrixTypeClass() {
    let cls;
    if (this.direction === 1 || this.direction === 3) {
      cls = 'vert';
    } else {
      cls = 'horz';
    }
    if (this.type !== 0) {
      if (this.direction === 0 || this.direction === 2) {
        if (this.angle === 0 || this.angle === 1)
          cls += ' right';
        else
          cls += ' left';
      } else {
        if (this.angle === 1 || this.angle === 2)
          cls += ' down';
        else
          cls += ' up';
      }
    }
    return cls;
  }

}
