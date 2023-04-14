import {Component, Input, OnInit} from '@angular/core';
import {MatDialog, MatDialogRef} from '@angular/material/dialog';
import {EffectModel} from '../../models/effect.model';
import {LanguagesService} from '../../services/languages/languages.service';
import {ManagementService} from '../../services/management/management.service';
import {WebsocketService} from '../../services/websocket/websocket.service';
import {EffectParamsComponent} from '../effect-params/effect-params.component';

@Component({
  selector: 'app-effect',
  templateUrl: './effect.component.html',
  styleUrls: ['./effect.component.scss']
})
export class EffectComponent implements OnInit {

  @Input() model!: EffectModel;
  @Input() disabled: boolean = false;
  @Input() choosing: boolean = false;

  private dialogRef: MatDialogRef<EffectParamsComponent> | null = null;

  constructor(public socketService: WebsocketService,
              public managementService: ManagementService,
              public L: LanguagesService,
              private dialog: MatDialog) {
  }

  ngOnInit(): void {
  }

  activate() {
    // $8 0 N; включить эффект N
    this.socketService.sendText(`$8 0 ${this.model.id};`);
  }

  settings() {
    this.dialogRef = this.dialog.open(EffectParamsComponent, {
      panelClass: 'settings-dialog-panel',
      data: {model: this.model}
    });
  }
}
