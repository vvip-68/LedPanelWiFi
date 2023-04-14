import {Component, Input, OnDestroy, OnInit} from '@angular/core';
import {BehaviorSubject, debounceTime, Subject, takeUntil} from 'rxjs';
import {WebsocketService} from '../../services/websocket/websocket.service';

@Component({
  selector: 'app-brightness-slider',
  templateUrl: './brightness-slider.component.html',
  styleUrls: ['./brightness-slider.component.scss']
})
export class BrightnessSliderComponent implements OnInit, OnDestroy {

  @Input() disabled: boolean = false;
  @Input() value: number = -1;

  private destroy$ = new Subject();
  private valueChanged$ = new BehaviorSubject(this.value);

  public get percent(): number {
    return Math.round(this.value * 100 / 255);
  }

  constructor(public socketService: WebsocketService) {
  }

  ngOnInit(): void {
    this.valueChanged$
      .pipe(takeUntil(this.destroy$), debounceTime(100))
      .subscribe((value) => {
        if (value >= 0) {
          this.value = value;
          // $4 0 value   установить текущий уровень общей яркости
          const command = `$4 0 ${value};`;
          this.socketService.sendText(command);
        }
      });
  }

  formatLabel(percent: number) {
    return percent + (percent === 100 ? '' : '%');
  }

  valueChanged($event: number | null) {
    const value = Math.round(Number($event) * 255 / 100);
    this.valueChanged$.next(value);
  }

  ngOnDestroy() {
    this.destroy$.next(true);
    this.destroy$.complete();
  }

}
