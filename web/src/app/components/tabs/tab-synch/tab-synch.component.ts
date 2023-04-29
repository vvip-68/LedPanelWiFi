import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {AppErrorStateMatcher, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import {ComboBoxItem} from "../../../models/combo-box.model";
import {FormControl, Validators} from "@angular/forms";

@Component({
  selector: 'app-tab-synch',
  templateUrl: './tab-synch.component.html',
  styleUrls: ['./tab-synch.component.scss'],
})
export class TabSynchComponent implements OnInit, OnDestroy {
  private destroy$ = new Subject();

  e131_mode: number = -1;
  e131_type: number = -1;
  e131_group: number = -1;

  e131_modes: ComboBoxItem[] = [
    {value: 0, displayText: this.L.$('Автономный')},
    {value: 1, displayText: this.L.$('Источник (мастер)')},
    {value: 2, displayText: this.L.$('Приемник (ведомый)')},
  ];

  e131_types: ComboBoxItem[] = [
    {value: 0, displayText: this.L.$('Физический порядок')},
    {value: 1, displayText: this.L.$('Логический порядок')},
    {value: 2, displayText: this.L.$('Команды')},
  ];

  e131_groups: ComboBoxItem[] = [
    {value: 0, displayText: this.L.$('Группа 0')},
    {value: 1, displayText: this.L.$('Группа 1')},
    {value: 2, displayText: this.L.$('Группа 2')},
    {value: 3, displayText: this.L.$('Группа 3')},
    {value: 4, displayText: this.L.$('Группа 4')},
    {value: 5, displayText: this.L.$('Группа 5')},
    {value: 6, displayText: this.L.$('Группа 6')},
    {value: 7, displayText: this.L.$('Группа 7')},
    {value: 8, displayText: this.L.$('Группа 8')},
    {value: 9, displayText: this.L.$('Группа 9')},
  ];

  sync_master_x = 0;
  sync_master_y = 0;
  sync_local_x = 0;
  sync_local_y = 0;
  sync_local_w = 16;
  sync_local_h = 16;

  masterXFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 127)]);
  masterYFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 127)]);
  localXFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 127)]);
  localYFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 127)]);
  localWFormControl = new FormControl(16, [Validators.required, rangeValidator(4, 128)]);
  localHFormControl = new FormControl(16, [Validators.required, rangeValidator(4, 128)]);

  matcher = new AppErrorStateMatcher();

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService
  ) {
  }

  ngOnInit() {

    this.sync_local_w = this.managementService.state.width;
    this.sync_local_h = this.managementService.state.height;

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          const request = 'E1|E2|E3|EMX|EMY|ELX|ELY|ELW|ELH';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'E1':
              this.e131_mode = this.managementService.state.e131_mode;
              break;
            case 'E2':
              this.e131_type = this.managementService.state.e131_type;
              break;
            case 'E3':
              this.e131_group = this.managementService.state.e131_group;
              break;
            case 'EMX':
              this.sync_master_x = this.managementService.state.sync_master_x;
              this.masterXFormControl.setValue(this.sync_master_x);
              break;
            case 'EMY':
              this.sync_master_y = this.managementService.state.sync_master_y;
              this.masterYFormControl.setValue(this.sync_master_y);
              break;
            case 'ELX':
              this.sync_local_x = this.managementService.state.sync_local_x;
              this.localXFormControl.setValue(this.sync_local_x);
              break;
            case 'ELY':
              this.sync_local_y = this.managementService.state.sync_local_y;
              this.localYFormControl.setValue(this.sync_local_y);
              break;
            case 'ELW':
              this.sync_local_w = this.managementService.state.sync_local_w;
              this.localWFormControl.setValue(this.sync_local_w);
              break;
            case 'ELH':
              this.sync_local_h = this.managementService.state.sync_local_h;
              this.localHFormControl.setValue(this.sync_local_h);
              break;
          }
        }
      });
  }

  applySyncSettings($event: MouseEvent) {
    /*
       - $23 3 E1 E2 E3;   - Установка режима работы панели и способа трактовки полученных данных синхронизации
             E1 - режим работы 0 - STANDALONE; 1 - MASTER; 2 - SLAVE
             E2 - данные в кадре: 0 - физическое расположение цепочки диодов
                                  1 - логическое расположение - X,Y - 0,0 - левый верхний угол и далее вправо по X, затем вниз по Y
                                  2 - строка команды в payload пакета
             E3 - группа синхронизации 0..9
     */
    const E1 = this.managementService.state.e131_mode = this.e131_mode;
    const E2 = this.managementService.state.e131_type = this.e131_type;
    const E3 = this.managementService.state.e131_group = this.e131_group;

    this.socketService.sendText(`$23 3 ${E1} ${E2} ${E3};`);

    /*
       - $23 5 MX, MY, LX, LY, LW, LH; - настройка окна отображения трансляции с MASTER на SLAVE
             MX - логическая координата X мастера, с которого начинается вывод на локальную матрицу.
             MY - логическая координата Y мастера, с которого начинается вывод на локальную матрицу.
                  MX,MY - левый верхний угол начала трансляции мастера;
             LX - логическая координата X приемника, на которую начинается вывод на локальную матрицу.
             LY - логическая координата Y приемника, на которую начинается вывод на локальную матрицу.
                  LX,LY - левый верхний угол начала отображения на приемнике
             LW - ширина окна отображения на локальной матрице
             LH - высота окна отображения на локальной матрице
     */
    if (E2 === 1) {
      const EMX = this.managementService.state.sync_master_x = this.sync_master_x = this.masterXFormControl.value as number;
      const EMY = this.managementService.state.sync_master_y = this.sync_master_y = this.masterYFormControl.value as number;
      const ELX = this.managementService.state.sync_local_x  = this.sync_local_x  = this.localXFormControl.value as number;
      const ELY = this.managementService.state.sync_local_y  = this.sync_local_y  = this.localYFormControl.value as number;
      const ELW = this.managementService.state.sync_local_w  = this.sync_local_w  = this.localWFormControl.value as number;
      const ELH = this.managementService.state.sync_local_h  = this.sync_local_h  = this.localHFormControl.value as number;

      this.socketService.sendText(`$23 5 ${EMX} ${EMY} ${ELX} ${ELY} ${ELW} ${ELH};`);
    }
  }

  isDisabled(): boolean {
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
