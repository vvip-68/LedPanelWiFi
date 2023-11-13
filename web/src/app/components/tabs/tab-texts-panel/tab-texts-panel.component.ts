import {Component, OnDestroy, OnInit} from '@angular/core';
import {LanguagesService} from "../../../services/languages/languages.service";
import { TabTextsHelpComponent } from '../tab-texts-help/tab-texts-help.component';
import { TabTextsComponent } from '../tab-texts/tab-texts.component';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatIconModule } from '@angular/material/icon';
import { MatTabsModule } from '@angular/material/tabs';

@Component({
    selector: 'app-tab-texts-panel',
    templateUrl: './tab-texts-panel.component.html',
    styleUrls: ['./tab-texts-panel.component.scss'],
    standalone: true,
    imports: [
        MatTabsModule,
        MatIconModule,
        MatTooltipModule,
        TabTextsComponent,
        TabTextsHelpComponent,
    ],
})
export class TabTextsPanelComponent implements OnInit, OnDestroy {
  constructor(public L: LanguagesService) {
  }

  ngOnInit() {
  }

  ngOnDestroy() {
  }

}
