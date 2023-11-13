import {Component, Inject, OnDestroy, OnInit, ViewEncapsulation} from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogModule } from '@angular/material/dialog';
import {Subject} from 'rxjs';
import {LanguagesService} from "../../services/languages/languages.service";
import { MatButtonModule } from '@angular/material/button';
import { CdkDrag, CdkDragHandle } from '@angular/cdk/drag-drop';

@Component({
    selector: 'app-confirmation-dialog',
    templateUrl: './confirmation-dialog.component.html',
    styleUrls: ['./confirmation-dialog.component.scss'],
    encapsulation: ViewEncapsulation.None,
    standalone: true,
    imports: [CdkDrag, CdkDragHandle, MatDialogModule, MatButtonModule]
})
export class ConfirmationDialogComponent implements OnInit, OnDestroy {
  title: string = '';
  message: string = '';
  cancelVisible = true;
  private readonly destroy$ = new Subject<void>();

  constructor(@Inject(MAT_DIALOG_DATA) private data: any,
              public L: LanguagesService) {
  }

  ngOnInit() {
    this.title = this.data.title;
    this.message = this.data.message.replace(/\n/gi, '<br>');
    this.cancelVisible = typeof this.data.useCancel === 'undefined' || this.data.useCancel !== false;
  }

  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
