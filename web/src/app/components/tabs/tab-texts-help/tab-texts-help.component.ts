import {Component, OnDestroy, OnInit} from '@angular/core';
import {LanguagesService} from '../../../services/languages/languages.service';

@Component({
  selector: 'app-tab-texts-help',
  templateUrl: './tab-texts-help.component.html',
  styleUrls: ['./tab-texts-help.component.scss'],
})
export class TabTextsHelpComponent implements OnInit, OnDestroy {
  constructor(public L: LanguagesService) {
  }

  ngOnInit() {
  }

  ngOnDestroy() {
  }

}
