import {Component, OnDestroy, OnInit} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {distinctUntilChanged} from "rxjs/operators";
import {isNullOrUndefinedOrEmpty} from "../../../services/helper";
import {ComboBoxItem} from "../../../models/combo-box.model";

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
          const request = 'E1|E2|E3';
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
          }
        }
      });
  }

  isDisabled(): boolean {
    return (
      !this.managementService.state.power || !this.socketService.isConnected
    );
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
    this.managementService.state.e131_mode = this.e131_mode;
    this.managementService.state.e131_type = this.e131_type;
    this.managementService.state.e131_group = this.e131_group;

    this.socketService.sendText(`$23 3 ${this.e131_mode} ${this.e131_type} ${this.e131_group};`);
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }
}
