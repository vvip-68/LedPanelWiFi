import {HttpClient, HttpHeaders} from '@angular/common/http';
import {Injectable, OnDestroy, OnInit} from '@angular/core';
import {stripComments} from 'jsonc-parser';
import {map} from 'rxjs/operators';
import {CommonService} from '../common/common.service';
import {isNullOrUndefined, isNullOrUndefinedOrEmpty} from '../helper';

@Injectable({
  providedIn: 'root'
})
export class LanguagesService implements OnInit, OnDestroy {

  public lang: string;
  public loaded: boolean = false;

  private default_lang = 'rus';
  private lmap: Map<string, string> = new Map();

  constructor(private httpClient: HttpClient,
              private commonService: CommonService) {
    this.lang = window.localStorage['lang'];
    if (isNullOrUndefinedOrEmpty(this.lang)) {
      this.lang = this.default_lang;
    }
    this.lang = this.lang.toLowerCase();
    this.loadLanguage(this.lang);
  }

  ngOnInit() {

  }

  //* Вывод информационного сообщения или сообщения об ошибке в footer страницы
  public $(message: string): string {
    const str = this.lmap.get(message);
    // @ts-ignore
    return isNullOrUndefinedOrEmpty(str) ? message : str;
  }

  public loadLanguage(lang: string) {
    this.lang = lang.toLowerCase();
    window.localStorage['lang'] = lang;
    const filePath = `assets/langs/${this.lang}.json`;
    const headers = new HttpHeaders();
    this.httpClient.get(filePath, {headers, responseType: 'text'})
      .pipe(map(response => JSON.parse(stripComments(<string>response))))
      .subscribe({
        next: (data) => {
          const keys = Object.keys(data);
          for (const key of keys) {
            this.lmap.set(key, data[key]);
          }
          this.loaded = true;
        },
        error: (error) => {
          this.loaded = true;
          console.warn(`Не удалось загрузить язык интерфейса: '${this.lang}'`);
          console.warn(error.message);
        }
      });
  }

  ngOnDestroy() {
  }
}
