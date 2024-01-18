import {Component, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import {ComboBoxItem} from "../../../models/combo-box.model";
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {PluggerComponent} from "../../plugger/plugger.component";
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatOptionModule } from '@angular/material/core';
import { MatSelectModule } from '@angular/material/select';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatRadioModule } from '@angular/material/radio';
import { NgStyle } from '@angular/common';
import {Base} from "../../base.class";

@Component({
    selector: 'app-tab-matrix',
    templateUrl: './tab-matrix.component.html',
    styleUrls: ['./tab-matrix.component.scss'],
    standalone: true,
    imports: [
        NgStyle,
        MatRadioModule,
        FormsModule,
        DisableControlDirective,
        MatSelectModule,
        MatOptionModule,
        MatFormFieldModule,
        MatInputModule,
        ReactiveFormsModule,
        PluggerComponent,
        MatButtonModule,
    ],
})
export class TabMatrixComponent extends Base implements OnInit, OnDestroy {

  @ViewChild('matrixPlugger') private matrixPlugger!: PluggerComponent;
  @ViewChild('totalPlugger') private totalPlugger!: PluggerComponent;

  index_files: ComboBoxItem[] = [];
  index_file: number = -1;
  radio_idx: number = 0;

  mtx_seg_type: number = 0;
  mtx_seg_angle: number = 0;
  mtx_seg_dir: number = 0;
  tol_seg_type: number = 0;
  tol_seg_angle: number = 0;
  tol_seg_dir: number = 0;

  segWidthFormControl = new FormControl(0, [Validators.required, rangeValidator(4, 128)]);
  segHeightFormControl = new FormControl(0, [Validators.required, rangeValidator(4, 128)]);
  tolWidthFormControl = new FormControl(0, [Validators.required, rangeValidator(1, 15)]);
  tolHeightFormControl = new FormControl(0, [Validators.required, rangeValidator(1, 15)]);

  matcher = new AppErrorStateMatcher();

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
          const request = 'M10|M11|M0|M1|M2|M3|M4|M5|M6|M7|M8|M9';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'M0':
              this.segWidthFormControl.setValue(this.managementService.state.mtx_seg_width);
              break;
            case 'M1':
              this.segHeightFormControl.setValue(this.managementService.state.mtx_seg_height);
              break;
            case 'M2':
              this.radio_idx = this.managementService.state.mtx_seg_type == 2 ? 0 : 1;
              this.mtx_seg_type = this.managementService.state.mtx_seg_type;
              this.matrixPlugger.type = this.mtx_seg_type;
              break;
            case 'M3':
              this.mtx_seg_angle = this.managementService.state.mtx_seg_angle;
              this.matrixPlugger.angle = this.mtx_seg_angle;
              break;
            case 'M4':
              this.mtx_seg_dir = this.managementService.state.mtx_seg_dir;
              this.matrixPlugger.direction = this.mtx_seg_dir;
              break;
            case 'M5':
              this.tolWidthFormControl.setValue(this.managementService.state.tol_seg_width);
              break;
            case 'M6':
              this.tolHeightFormControl.setValue(this.managementService.state.tol_seg_height);
              break;
            case 'M7':
              this.tol_seg_type = this.managementService.state.tol_seg_type;
              this.totalPlugger.type = this.tol_seg_type;
              break;
            case 'M8':
              this.tol_seg_angle = this.managementService.state.tol_seg_angle;
              this.totalPlugger.angle = this.tol_seg_angle;
              break;
            case 'M9':
              this.tol_seg_dir = this.managementService.state.tol_seg_dir;
              this.totalPlugger.direction = this.tol_seg_dir;
              break;
            case 'M10':
              this.index_files = this.managementService.state.map_idx_list;
              break;
            case 'M11':
              this.index_file = this.managementService.state.map_idx;
              break;
          }
        }
      });
  }

  applyMatrixSettings($event: MouseEvent) {
    /*
      - $2 M0 M1 M2 M3 M4 M5 M6 M7 M8 M9 M11
         M0  - ширина сегмента матрицы 1..127
         M1  - высота сегмента матрицы 1..127
         M2  - тип сегмента матрицы - 0 - зигзаг; 1 - параллельная
         M3  - угол подключения диодов в сегменте: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3
         M4  - направление ленты из угла сегмента: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
         M5  - количество сегментов в ширину составной матрицы
         M6  - количество сегментов в высоту составной матрицы
         M7  - соединение сегментов составной матрицы: 0 - зигзаг, 1 - параллельная
         M8  - угол 1-го сегмента мета-матрицы: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
         M9  - направление следующих сегментов мета-матрицы из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
         M11 - Индекс текущей используемой карты индексов
    */

    const M0 = this.managementService.state.mtx_seg_width = this.segWidthFormControl.value as number;
    const M1 = this.managementService.state.mtx_seg_height = this.segHeightFormControl.value as number;
    const M2 = this.managementService.state.mtx_seg_type = this.radio_idx == 0 ? 2 : this.matrixPlugger.type;
    const M3 = this.managementService.state.mtx_seg_angle = this.matrixPlugger.angle;
    const M4 = this.managementService.state.mtx_seg_dir = this.matrixPlugger.direction;

    const M5 = this.managementService.state.tol_seg_width = this.tolWidthFormControl.value as number;
    const M6 = this.managementService.state.tol_seg_height = this.tolHeightFormControl.value as number;
    const M7 = this.managementService.state.tol_seg_type = this.totalPlugger.type;
    const M8 = this.managementService.state.tol_seg_angle = this.totalPlugger.angle;
    const M9 = this.managementService.state.tol_seg_dir = this.totalPlugger.direction;

    const M11 = this.managementService.state.map_idx = this.index_file;

    this.socketService.sendText(`$2 ${M0} ${M1} ${M2} ${M3} ${M4} ${M5} ${M6} ${M7} ${M8} ${M9} ${M11};`);
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

}
