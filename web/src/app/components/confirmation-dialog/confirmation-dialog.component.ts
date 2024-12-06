import {Component, Inject, OnDestroy, OnInit, ViewEncapsulation} from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogModule } from '@angular/material/dialog';
import {LanguagesService} from "../../services/languages/languages.service";
import { MatButtonModule } from '@angular/material/button';
import { CdkDrag, CdkDragHandle } from '@angular/cdk/drag-drop';
import {Base} from "../base.class";

@Component({
    selector: 'app-confirmation-dialog',
    templateUrl: './confirmation-dialog.component.html',
    styleUrls: ['./confirmation-dialog.component.scss'],
    encapsulation: ViewEncapsulation.None,
    standalone: true,
    imports: [CdkDrag, CdkDragHandle, MatDialogModule, MatButtonModule]
})
export class ConfirmationDialogComponent extends Base implements OnInit, OnDestroy {
  title: string = '';
  message: string = '';
  okText: string = '';
  cancelText: string = '';
  cancelVisible = true;

  constructor(@Inject(MAT_DIALOG_DATA) private data: any,
              public L: LanguagesService) {
    super();
  }

  ngOnInit() {
    this.title = this.data.title;
    this.message = this.data.message.replace(/\n/gi, '<br>');
    this.cancelVisible = typeof this.data.useCancel === 'undefined' || this.data.useCancel;
    this.okText = this.data.okText ?? this.L.$('Да');
    this.cancelText = this.data.cancelText ?? this.L.$('Отмена');
  }

}
