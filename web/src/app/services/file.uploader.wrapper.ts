import {ElementRef, Injectable} from '@angular/core';
import {MatDialog} from '@angular/material/dialog';
import {FileUploader} from 'ng2-file-upload';
import {UploadFileComponent} from "../components/upload/upload.component";
import {ConfirmationDialogComponent} from "../components/confirmation-dialog/confirmation-dialog.component";
import {WebsocketService} from "./websocket/websocket.service";
import {isNullOrUndefined} from "./helper";
import {LanguagesService} from "./languages/languages.service";
import {Subject} from "rxjs";

@Injectable({ providedIn: 'root' })
export class FileUploaderWrapper {

  complete$ = new Subject<any>();

  uploader: FileUploader;
  inputFileForm!: ElementRef;

  private maxFileSize = 1024 * 1024 * 1; // Max upload file size is 1MB

  constructor(
    public socketService: WebsocketService,
    public L: LanguagesService,
    private dialog: MatDialog)
  {
    const URL = `http://${this.socketService.host}`;
    this.uploader = new FileUploader({
        url: URL,
        maxFileSize: this.maxFileSize
    });
  }

  getUploader(): FileUploader {
    return this.uploader;
  }

  onFileSelect(title: string,
               actionButton: string,
               inputFileForm: ElementRef,
               uploadFolder: string,
               extraData?: any)
  {
    this.inputFileForm = inputFileForm;

    if (!isNullOrUndefined(extraData)) {
      this.executeWithExtra(title, actionButton, uploadFolder, extraData);
    } else {
      this.execute(title, actionButton, uploadFolder);
    }
  }

  private executeWithExtra(title: string,
                           actionButton: string,
                           uploadFolder: string,
                           extraData: any)
  {
    if (!isNullOrUndefined(extraData.confirmationMessage)) {
      const confirmationDialogRef = this.dialog.open(ConfirmationDialogComponent, {
        width: '400px',
        panelClass: 'centralized-dialog-content',
        data: { title: this.L.$("Подтвердите"), message: extraData.confirmationMessage }
      });

      confirmationDialogRef.afterClosed()
        .subscribe(result => {
          if (result === true) {
            while (this.uploader.queue.length > 1) {
              this.uploader.removeFromQueue(this.uploader.queue[0]);
            }

            this.execute(title, actionButton, uploadFolder, extraData);
          } else {
            this.inputFileForm.nativeElement.reset();
          }
        });
    } else {
      this.execute(title, actionButton, uploadFolder, extraData);
    }
  }

  private execute(title: string, actionButton: string, uploadFolder: string, extraData?: any) {
    while (this.uploader.queue.length > 1) {
      this.uploader.removeFromQueue(this.uploader.queue[0]);
    }

    const uploadDialogRef = this.dialog.open(UploadFileComponent, {
      data: {
        title: title,
        actionButton: actionButton,
        folder: uploadFolder,
        uploader: this.uploader,
        fileNewName: (!isNullOrUndefined(extraData) && !isNullOrUndefined(extraData.fileNewName)) ? extraData.fileNewName : ''
      },
      panelClass: 'dialog-container'
    });

    this.inputFileForm.nativeElement.reset();

    uploadDialogRef.afterClosed()
      .subscribe(result => {
        this.complete$.next(result)
      });
  }
}
