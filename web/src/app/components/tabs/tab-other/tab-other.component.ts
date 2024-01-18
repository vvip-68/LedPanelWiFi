import {Component, ElementRef, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {debounceTime, Subject, takeUntil} from 'rxjs';
import {CommonService} from '../../../services/common/common.service';
import {LanguagesService} from '../../../services/languages/languages.service';
import {ManagementService} from '../../../services/management/management.service';
import {WebsocketService} from '../../../services/websocket/websocket.service';
import {AppErrorStateMatcher, isNullOrUndefined, isNullOrUndefinedOrEmpty, rangeValidator} from "../../../services/helper";
import { FormControl, Validators, FormsModule, ReactiveFormsModule } from "@angular/forms";
import {distinctUntilChanged} from "rxjs/operators";
import {ConfirmationDialogComponent} from "../../confirmation-dialog/confirmation-dialog.component";
import {MatDialog} from "@angular/material/dialog";
import { MatIconModule } from '@angular/material/icon';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatButtonModule } from '@angular/material/button';
import { DisableControlDirective } from '../../../directives/disable-control.directive';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import {Base} from "../../base.class";
import {FileUploaderWrapper} from "../../../services/file.uploader.wrapper";
import {FileUploadModule} from "ng2-file-upload";

@Component({
    selector: 'app-tab-other',
    templateUrl: './tab-other.component.html',
    styleUrls: ['./tab-other.component.scss'],
    standalone: true,
  imports: [
    MatFormFieldModule,
    MatInputModule,
    FormsModule,
    ReactiveFormsModule,
    DisableControlDirective,
    MatButtonModule,
    MatTooltipModule,
    MatIconModule,
    FileUploadModule,
  ],
})
export class TabOtherComponent extends Base implements OnInit, OnDestroy {

  // @ts-ignore
  @ViewChild('input2') input2: ElementRef;
  @ViewChild('uploadFileForm') uploadFileForm!: ElementRef;

  autoLimitFormControl = new FormControl(0, [Validators.required, rangeValidator(0, 50000)]);
  systemNameFormControl = new FormControl('', [Validators.required]);
  matcher = new AppErrorStateMatcher();

  fs_allow: boolean = true;
  sd_allow: boolean = false;
  backup_place: number = 0;
  backup_file: string = '';

  constructor(
    public socketService: WebsocketService,
    public managementService: ManagementService,
    public commonService: CommonService,
    public L: LanguagesService,
    public fileUploaderWrapper: FileUploaderWrapper,
    private dialog: MatDialog)
  {
    super();
    this.fileUploaderWrapper.complete$.subscribe((result) => {
      if (result.result === 'success') {
        // Отправить команду восстановления конфигурации из загруженного файла
        // $23 2 ST;   - Загрузить EEPROM из файла  ST = 0 - внутр. файл. системы; 1 - на SD-карты
        this.socketService.sendText('$23 2 0;');
        this.managementService.text_lines = [];
      } else if (result.result === 'error') {
        // Вывести сообщение об ошибке
        const dialog = this.dialog.open(ConfirmationDialogComponent, {
          width: '400px',
          panelClass: 'centralized-dialog-content',
          data: {title: this.L.$('Ошибка'), message: this.L.$('Upload error'), useCancel: false, okText: this.L.$('OK') }
        });
      }
    })
  }

  ngOnInit() {

    this.socketService.isConnected$
      .pipe(takeUntil(this.destroy$), distinctUntilChanged(), debounceTime(1000))
      .subscribe((isConnected: boolean) => {
        if (isConnected) {
          const request = 'PW|HN|FS|SX|EE|BF';
          this.managementService.getKeys(request);
        }
      });

    this.managementService.stateKey$
      .pipe(takeUntil(this.destroy$))
      .subscribe((key: string) => {
        if (!isNullOrUndefinedOrEmpty(key)) {
          switch (key) {
            case 'HN':
              this.systemNameFormControl.setValue(this.managementService.state.hostName);
              break;
            case 'PW':
              this.autoLimitFormControl.setValue(this.managementService.state.curr_limit);
              break;
            case 'FS': // доступность внутренней файловой системы микроконтроллера для хранения файлов: 0 - нет, 1 - да
              this.fs_allow = this.managementService.state.fs_allow;
              break;
            case 'SX': // наличие и доступность SD карты в системе: Х = 0 - нат SD карты; 1 - SD карта доступна
              this.sd_allow = this.managementService.state.sd_allow;
              break;
            case 'EE': // Наличие сохраненных настроек EEPROM на SD-карте или в файловой системе МК: 0 - нет 1 - есть в FS; 2 - есть на SD; 3 - есть в FS и на SD
              this.backup_place = this.managementService.state.backup_place;
              break;
            case 'BF': // Имя файла сохраненных настроек EEPROM
              this.backup_file = this.managementService.state.backup_file;
              break;
          }
        }
      });
  }

  isDisabled(): boolean {
    return !this.socketService.isConnected;
  }

  getHintForLimit() {
    if (isNullOrUndefined(this.input2) || Number(this.input2.nativeElement.value) === 0) {
      return this.L.$('ограничение по току выключено');
    }
    return `${this.L.$('лимит по току')} ${this.input2.nativeElement.value} ${this.L.$('миллиампер')} ${this.L.$('[0 - выключено]')}`;
  }

  isSettingsValid(): boolean {
    return this.autoLimitFormControl.valid;
  }

  isSystemNameValid(): boolean {
    return this.systemNameFormControl.valid;
  }

  applySettings($event: MouseEvent) {
    const PW = this.managementService.state.curr_limit = Number(this.autoLimitFormControl.value);

    // $23 0 VAL;  - лимит по потребляемому току
    this.socketService.sendText(`$23 0 ${PW};`);
  }

  applySystemName($event: MouseEvent) {
    const SN = this.managementService.state.hostName = this.systemNameFormControl.value ?? "";

    // $6 19|text;  - имя системы
    this.socketService.sendText(`$6 19|${SN}`);
  }

  loadFrom(from: number) {
    const dialog = this.dialog.open(ConfirmationDialogComponent, {
      width: '400px',
      panelClass: 'centralized-dialog-content',
      data: {title: this.L.$('Восстановление настроек'), message: this.L.$('Load SDFS')}
    });

    dialog.afterClosed()
      .subscribe(result => {
        if (result === true) {
          // $23 2 ST;   - Загрузить EEPROM из файла  ST = 0 - внутр. файл. системы; 1 - на SD-карты
          this.socketService.sendText(`$23 2 ${from};`);
          this.managementService.text_lines = [];
        }
      });
  }

  saveTo(from: number) {
    // backup_place - наличие сохраненных настроек EEPROM на SD-карте или в файловой системе МК: 0 - нет 1 - есть в FS; 2 - есть на SD; 3 - есть в FS и на SD
    // - $23 1 ST;  - Сохранить EEPROM в файл    ST = 0 - внутр. файл. систему; 1 - на SD-карту
    const need_ask = (from === 0 && (this.backup_place == 1 || this.backup_place == 3)) ||
      (from === 1 && (this.backup_place == 2 || this.backup_place == 3));
    // Если файл бэкапа уже есть на носителе - предварительно спросить перезапись настроек
    if (need_ask) {
      const dialog = this.dialog.open(ConfirmationDialogComponent, {
        width: '400px',
        panelClass: 'centralized-dialog-content',
        data: {title: this.L.$('Сохранение настроек'), message: this.L.$('Save SDFS')}
      });

      dialog.afterClosed()
        .subscribe(result => {
          if (result === true) {
            // $23 1 ST;   - Сохранить EEPROM в файл    ST = 0 - внутр. файл. систему; 1 - на SD-карту
            this.socketService.sendText(`$23 1 ${from};`);
          }
        });
    } else {
      // $23 1 ST;   - Сохранить EEPROM в файл    ST = 0 - внутр. файл. систему; 1 - на SD-карту
      this.socketService.sendText(`$23 1 ${from};`);
    }
  }

  restart() {
    // $23 4; - перезапуск устройства (reset/restart/reboot)
    this.socketService.sendText('$23 4;');
  }

  onUpload() {
    this.fileUploaderWrapper.onFileSelect(
      this.L.$("Загрузить"),
      this.L.$("Загрузить"),
      this.uploadFileForm,
      '/assets/',
      { confirmationMessage: null, fileNewName: this.backup_file });
  }

}
