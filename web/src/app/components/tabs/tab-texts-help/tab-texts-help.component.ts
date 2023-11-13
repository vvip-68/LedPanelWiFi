import {Component, OnDestroy, OnInit} from '@angular/core';
import {LanguagesService} from '../../../services/languages/languages.service';
import { MatIconModule } from '@angular/material/icon';

@Component({
    selector: 'app-tab-texts-help',
    templateUrl: './tab-texts-help.component.html',
    styleUrls: ['./tab-texts-help.component.scss'],
    standalone: true,
    imports: [MatIconModule],
})
export class TabTextsHelpComponent implements OnInit, OnDestroy {
  constructor(public L: LanguagesService) {
  }

  ngOnInit() {
  }

  ngOnDestroy() {
  }

}
