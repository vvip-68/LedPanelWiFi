export interface IEffectModel {
  id: number;            // id эффекта
  name: string;          // имя эффекта
  allowClock: boolean;   // флаг: разрешить часы поверх эффекта
  allowText: boolean;    // флаг: разрешить бегущую строку поверх эффекта
  contrast: number;      // контраснонсть
  speed: number;         // скорость
  param1: number;        // Параметр эффекта #1
  param2: number;        // Параметр эффекта #2
  active: boolean;       // Флаг активного режима
  paramName1: string;    // Название параметра 1 для отображения в Web-интерфейсе
  paramName2: string;    // Название параметра 2 для отображения в Web-интерфейсе
  order: number;         // Индекс в порядке воспроизведения
}

export class EffectModel implements IEffectModel {

  public id: number = 0;
  public name: string = '';
  public allowClock: boolean = false;
  public allowText: boolean = false;
  public contrast: number = 100;
  public speed: number = 100;
  public param1: number = 50;
  public param2: number = 50;
  public active = false;
  public paramName1: string = '';
  public paramName2: string = '';
  public order: number = -1;

  constructor(id?: number, name?: string) {
    this.id = id ?? 0;
    this.name = name ?? '';
  }
}
