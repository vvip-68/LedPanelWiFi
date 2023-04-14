export type OnOffFunction = (a: any) => string;
export type ActiveFunction = (a: any) => boolean;
export type CommandFunction = (a: any) => string;

export enum ActionType {
  NONE = 'none',
  POWER = 'power',
  BRIGHT = 'bright'
}

export interface IActionModel {
  id: number;                       // ID режима
  readonly key: string;             // список команд при получении которых требуется обновление отображения представления плитки
  readonly initkeys: string;        // список параметров, которые требуется запросить с сервера в момент инициализации плитки
  mode: ActionType;                 // Тип действия
  icon: string;                     // класс, определяющий отображаемую иконку плитки action
  value: any;                       // Значение параметра
  get: string | CommandFunction;    // Команда запроса параметров
  set: string | CommandFunction;    // Команда установки параметра
  active: boolean | ActiveFunction; // Флаг активного режима или функция, возвращающая true - активно, false - не активно
}


export class ActionModel implements IActionModel {

  public id = 0;
  public key = '';
  public initkeys = '';
  public mode = ActionType.NONE;
  public icon = '';
  public get: string | CommandFunction = (val: any) => '';
  public set: string | CommandFunction = (val: any) => '';
  public value: any = null;
  public active: boolean | ActiveFunction = (val: any) => false;

  constructor(data?: object) {
    if (data) {
      Object.assign(this, data);
    }
    switch (this.mode) {
      case ActionType.POWER:
        this.key = 'PS';
        break;
      case ActionType.BRIGHT:
        this.key = 'BR';
        break;
    }
  }
}
