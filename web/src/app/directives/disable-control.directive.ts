import {NgControl} from '@angular/forms';
import {Directive, Input, OnChanges} from '@angular/core';

//https://stackoverflow.com/questions/60075561/directives-not-working-with-formcontrol-after-upgrading-to-angular-9
//https://twitter.com/yurzui/status/1225050458097704960
@Directive({
    selector: '[disableControl]',
    standalone: true
})
export class DisableControlDirective implements OnChanges {
  @Input('disableControl') disableControl: boolean = false;

  constructor(private ngControl: NgControl) {
  }

  ngOnChanges(changes: any) {
    if (changes['disableControl']) {
      const action = this.disableControl ? 'disable' : 'enable';
      // @ts-ignore
      this.ngControl.control[action]()
    }
  }
}
