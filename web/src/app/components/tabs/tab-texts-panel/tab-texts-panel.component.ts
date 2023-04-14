import {Component, OnDestroy, OnInit} from '@angular/core';
import {LanguagesService} from "../../../services/languages/languages.service";

@Component({
  selector: 'app-tab-texts-panel',
  templateUrl: './tab-texts-panel.component.html',
  styleUrls: ['./tab-texts-panel.component.scss'],
})
export class TabTextsPanelComponent implements OnInit, OnDestroy {
  constructor(public L: LanguagesService) {
  }

  ngOnInit() {
  }

  ngOnDestroy() {
  }

}
