import {Component, Input, OnInit} from '@angular/core';
import {MatDialog, MatDialogRef} from '@angular/material/dialog';
import {EffectModel} from '../../models/effect.model';
import {LanguagesService} from '../../services/languages/languages.service';
import {ManagementService} from '../../services/management/management.service';
import {WebsocketService} from '../../services/websocket/websocket.service';
import {EffectParamsComponent} from '../effect-params/effect-params.component';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatIconModule } from '@angular/material/icon';
import { NgClass } from '@angular/common';
import {CdkDrag} from "@angular/cdk/drag-drop";

@Component({
    selector: 'app-effect',
    templateUrl: './effect.component.html',
    styleUrls: ['./effect.component.scss'],
    standalone: true,
  imports: [NgClass, MatIconModule, MatTooltipModule, CdkDrag]
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
    if (this.isStreaming()) return;
    // $8 0 N; включить эффект N
    this.socketService.sendText(`$8 0 ${this.model.id};`);
  }

  getDisabledTooltip() {
    return this.managementService.state.e131_mode === 2 && this.managementService.state.e131_streaming === true
      ? this.L.$('В режиме приема вещания в группе недоступно')
      : "";
  }

  isStreaming(): boolean {
    return this.managementService.state.e131_mode === 2 && this.managementService.state.e131_streaming === true;
  }

  settings() {
    this.dialogRef = this.dialog.open(EffectParamsComponent, {
      panelClass: 'settings-dialog-panel',
      data: {model: this.model}
    });
  }
}
