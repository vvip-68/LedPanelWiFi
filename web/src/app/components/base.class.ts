import {Injectable, OnDestroy} from '@angular/core';
import {Subject} from 'rxjs';


@Injectable()
export abstract class Base implements OnDestroy{
  protected destroy$ = new Subject<void>();

  ngOnDestroy() {
    this.destroy$.next();
    this.destroy$.complete();
  }
}
