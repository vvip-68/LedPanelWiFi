import {CdkDrag, CdkDragHandle} from '@angular/cdk/drag-drop';
import {CommonModule} from '@angular/common';
import {Component, Inject, OnInit} from '@angular/core';
import {FormsModule} from '@angular/forms';
import {MatButtonModule} from '@angular/material/button';
import {MatCheckboxModule} from '@angular/material/checkbox';
import {MAT_DIALOG_DATA, MatDialogModule, MatDialogRef} from '@angular/material/dialog';
import {FileUploader} from 'ng2-file-upload';
import {Base} from "../base.class";
import {isNullOrUndefinedOrEmpty} from "../../services/helper";
import {LanguagesService} from "../../services/languages/languages.service";

@Component({
    selector: 'app-upload',
    templateUrl: './upload.component.html',
    styleUrls: ['./upload.component.scss'],
    standalone: true,
    imports: [CommonModule, CdkDrag, CdkDragHandle, MatDialogModule, MatCheckboxModule, FormsModule, MatButtonModule]
})
export class UploadFileComponent extends Base implements OnInit {

  title!: string;
  actionButton!: string;

  folder!: string;
  fileNewName!: string;
  uploader!: FileUploader;

  constructor (
    public L: LanguagesService,
    @Inject(MAT_DIALOG_DATA) private data: any,
    private dialogRef: MatDialogRef<UploadFileComponent>)
  {
    super();
    // Do not close dialog on outside clicking
    dialogRef.disableClose = true;
  }

  // ------------- Component lifecycle -------------------

  ngOnInit() {

    this.title = this.data.title;
    this.folder = this.data.folder;
    this.uploader = this.data.uploader;
    this.actionButton = this.data.actionButton;
    this.fileNewName = this.data.fileNewName;

    this.uploader.onCompleteAll = () => {
      this.closeDialog();
    };
  }

  uploadFile() {
    this.uploader.onBuildItemForm = (fileItem: any, form: any) => {
      form.append('folder', this.folder);
    };
    if (!isNullOrUndefinedOrEmpty(this.fileNewName) && !isNullOrUndefinedOrEmpty(this.uploader.queue)) {
      this.uploader.queue[0].file.name = this.fileNewName;
    }
    this.uploader.uploadAll();
  }

  closeDialog() {
    const result: any = {};

    // If uploading by HTML5 uploader still is in progress - cancel process
    if (this.uploader.isUploading) {
      this.uploader.cancelAll();
    }

    if (this.uploader.queue.length === 0 || this.uploader.queue[0].isCancel) {
      result.result = 'cancel';
    } else if (this.uploader.queue[0].isSuccess && this.uploader.queue[0].isUploaded) {
      result.result = 'success';
    } else if (this.uploader.queue[0].isError) {
      result.result = 'error';
    } else {
      result.result = 'cancel';
    }

    result.file = this.uploader.queue.length > 0 ? this.uploader.queue[0].file.name : '';
    this.dialogRef.close(result);
  }

}
