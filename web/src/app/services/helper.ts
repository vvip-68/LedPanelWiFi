import {AbstractControl, FormControl, FormGroupDirective, NgForm, ValidatorFn} from "@angular/forms";
import {ErrorStateMatcher} from "@angular/material/core";
import {LanguagesService} from "./languages/languages.service";
import {_isNumberValue} from "@angular/cdk/coercion";

/** Error when invalid control is dirty, touched, or submitted. */
export class AppErrorStateMatcher implements ErrorStateMatcher {
  isErrorState(control: FormControl | null, form: FormGroupDirective | NgForm | null): boolean {
    return !!(control && control.invalid);
  }
}

export function isNullOrUndefined(value: any) {
  return value === null || value === undefined;
}

export function isNullOrUndefinedOrEmpty(value: string | object): boolean {
  return isNullOrUndefined(value) || (value === '' || (Array.isArray(value) && value.length === 0));
}

export function replaceAll(value: string, search: string, replace: string): string {
  return value.split(search).join(replace);
}

export function isEmptyObject(obj: object): boolean {
  for (const key in obj) {
    if (obj.hasOwnProperty(key)) {
      return false;
    }
  }
  return true;
}

export function clearBraces(value: string): string {
  let list = value;
  if (list.startsWith('[')) list = list.substring(1);
  if (list.endsWith(']')) list = list.substring(0, list.length - 1);
  return list;
}

//* Строка времени uptime из количества секунд
export function getUptime(value: number, L: LanguagesService): string {
  let totalSeconds = value;
  const dn = Math.trunc(totalSeconds / (60 * 60 * 24));
  totalSeconds -= dn * 60 * 60 * 24;
  const hr = Math.trunc(totalSeconds / (60 * 60));
  totalSeconds -= hr * 60 * 60;
  const min = Math.trunc(totalSeconds / 60);
  let str = '';
  if (dn > 0) str = `${dn} ${L.$('дн.')} `;
  if (dn > 0 || hr > 0) str += `${hr} ${L.$('час.')} `;
  str += `${min.toString().padStart(2, '0')} ${L.$('мин.')}`;
  return str;
}

// ----------------- Validators --------------------

export function rangeValidator(min: number, max: number): ValidatorFn {
  return (control: AbstractControl): { [key: string]: boolean } | null => {
    if (control.value !== undefined && (isNaN(control.value) || control.value < min || control.value > max)) {
      return {'range': true};
    }
    return null;
  };
}

export function lengthValidator(min: number, max: number): ValidatorFn {
  return (control: AbstractControl): { [key: string]: boolean } | null => {
    const str = control.value.toString();
    if (control.value !== undefined && (str.length < min || str.length > max)) {
      return {'length': true};
    }
    return null;
  };
}

export function ipV4Validator(): ValidatorFn {
  return (control: AbstractControl): { [key: string]: boolean } | null => {
    const obj = control.value;
    if (!isNullOrUndefinedOrEmpty(obj) && !isValidIPv4(obj)) {
      return {'ipv4': true};
    }
    return null;
  };
}

export function isValidIPv4(str: string) {
  // Valid IPv4
  const regExp = /^(([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-3][0-9])(([.]([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])){3}))$/;
  return regExp.test(str);
}

export function timeZoneValidator(): ValidatorFn {
  return (control: AbstractControl): { [key: string]: boolean } | null => {
    if (!isNullOrUndefinedOrEmpty(control.value)) {
      let hours = 0;
      let minutes = 0;
      // Часовой пояс - число от -12 до 12 - если нет минут
      // Если минуты есть - отделяются от часов символом двоеточие ':'
      const tz = control.value.trim().split(':');
      if (tz.length == 1) {
        // нет минут, только часы
        if (!_isNumberValue(tz[0])) return {'timezone': true};
        hours = Number(tz[0])
      } else {
        // есть минуты
        if (!_isNumberValue(tz[0])) return {'timezone': true};
        hours = Number(tz[0])
        if (!_isNumberValue(tz[1])) return {'timezone': true};
        minutes = Number(tz[1])
      }
      if (hours < -12 || hours > 12) return {'timezone': true};
      if (!(minutes === 0 || minutes === 15 || minutes === 30 || minutes === 45)) return {'timezone': true};
    }
    return null;
  };
}
