import {Directive, HostListener, Input} from '@angular/core';
import {isNullOrUndefined, isNullOrUndefinedOrEmpty} from "../services/helper";

@Directive({
  selector: '[restrict]'
})
export class InputRestrictionDirective {
  public className = 'InputRestrictionDirective';

  @Input() inputType: string = "";
  @Input() regExp: string = "";

  private regexMap: Map<string, RegExp> = new Map([
    // @formatter:off
    ['integer',  /^[0-9]$/g],
    ['float',    /^[+\-.0-9]$/g],
    ['ipv4',     /^[.0-9]$/g],
    ['ipv6',     /^[.:0-9a-fA-F]$/g],
    ['ipv4pfx',  /^[.0-9/]$/g],
    ['ipv6pfx',  /^[.:0-9a-fA-F/]$/g],
    // add your own below
    ['range',    /^[0-9\-,]$/g],
    ['number',   /^[0-9]$/g],
    ['timezone', /^[0-9:+-]+$/g],
    ['website',  /^[a-zA-Z0-9._-]+$/g],
    ['password', /^[a-zA-Z0-9.+!@#$%^&*()_-]+$/g]
    // @formatter:on
  ]);

  @HostListener('keypress', ['$event']) onInput(e: KeyboardEvent) {

    const inputChar = e.key;
    let isValidChar = true;

    if (!isNullOrUndefinedOrEmpty(this.inputType)) {
      const pattern = this.regexMap.get(this.inputType);
      if (!isNullOrUndefined(pattern)) {
        pattern!.lastIndex = 0;
        isValidChar = pattern!.test(inputChar);
      }
    }

    if (isValidChar && !isNullOrUndefinedOrEmpty(this.regExp)) {
      const pattern = new RegExp(this.regExp);
      pattern.lastIndex = 0;
      isValidChar = pattern.test(inputChar);
    }

    if (!isValidChar) {
      e.preventDefault();
    }
  }
}
