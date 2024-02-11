import {Injectable, OnDestroy} from '@angular/core';
import {Subject, Subscription, take, timer} from 'rxjs';
import {isNullOrUndefined} from '../helper';
import {Base} from "../../components/base.class";

export enum MessageType {
  INFO,
  ERROR
}

@Injectable({
  providedIn: 'root'
})
export class CommonService extends Base implements OnDestroy {

  public info = '';
  public error = '';

  private timer: Subscription | null = null;

  constructor() {
    super();
  }

  //* Вывод информационного сообщения или сообщения об ошибке в footer страницы
  public timerMessage(type: MessageType, message: string, duration?: number) {
    if (this.timer) this.timer.unsubscribe();
    this.info = '';
    this.error = '';

    if (type == MessageType.INFO) {
      this.info = message ?? '';
    } else {
      this.error = message ?? '';
    }
    let delay = Number(isNullOrUndefined(duration) ? 5000 : duration);
    if (delay < 1000) delay = 1000; else if (delay > 30000) delay = 30000;

    this.timer = timer(delay).pipe(take(1)).subscribe(() => {
      this.info = '';
      this.error = '';
      this.timer = null;
    });
  }

// ---------------- Input checkers ----------------

  checkLength($event: KeyboardEvent, number: number) {
    // @ts-ignore
    const value = $event.target?.value;
    const allow = $event.key === "ArrowRight" || $event.key === "ArrowLeft" || $event.key === "ArrowUp" || $event.key === "ArrowDown" || $event.key === "Backspace" || $event.key === "Delete" || $event.key === "Home" || $event.key === "End";
    if (value.length >= number && !allow) $event.preventDefault();
  }

  override ngOnDestroy() {
    if (this.timer) this.timer.unsubscribe();
    super.ngOnDestroy();
  }
}
