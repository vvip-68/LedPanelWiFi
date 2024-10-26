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

export function makeid(length: number) {
  let result = '';
  const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const charactersLength = characters.length;
  let counter = 0;
  while (counter < length) {
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
    counter += 1;
  }
  return result;
}

export function validateCharacters(message: string): ValidatorFn {
  return formControl => {
    const obj = formControl.value;
    const regExp = /^[\s\w?<>.,\\/|+=:;№'"`~!@#$%^&*(){}\[\]_-]+$/;
    return !regExp.test(formControl.value) && obj.length > 0 ? { ['invalidChars']: message } : null;
  };
}

export function convertTimeTo24(time: string): string {
  // time = 'HH:MM AM' или 'HH:MM PM' при 12-часовом формате
  // time = 'HH:MM' при 24-часовом формате
  if (!time.endsWith('M')) return time;

  time = time.replace(' ', ':');
  const [hh, mm, pm] = time.split(':');
  const isPM = pm === 'PM';
  let h = Number(hh) + (isPM ? 12 : 0);
  if (h > 23) h -= 24;

  return `${h}:${mm}`;
}

export function convertTimeTo12(time: string): string {
  // time = 'HH:MM AM' или 'HH:MM PM' при 12-часовом формате
  // time = 'HH:MM' при 24-часовом формате
  if (time.endsWith('M')) return time;

  const [hh, mm] = time.split(':');
  let h = Number(hh);
  const isPM = h >= 12;
  if (isPM) h -= 12;
  if (h === 0) h = 12;

  return `${h}:${mm} ${isPM ? 'PM' : 'AM'}`;
}
