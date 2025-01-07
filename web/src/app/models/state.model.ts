import {ComboBoxItem} from "./combo-box.model";

export interface IStateModel {
  version: string;                 // VR - Версия прошивки
  hostName: string;                // HN - Имя устройства
  last_error: string;              // ER - Последнее сообщение об ошибке
  width: number;                   // W - Ширина матрицы (полная, в точках)
  height: number;                  // H - высота матрицы (полная, в точках)
  supportTM1637: boolean;          // TM - устройство поддерживает TM1637 - USE_TM1637
  supportWeather: boolean;         // WZ - устройство поддерживает получение погоды - USE_WEATHER
  supportMP3: boolean;             // MX - MX:X	MP3 плеер доступен для использования 0-нет, 1-да - включено в прошивке и звуки обнаружены
  supportPlayer: boolean;          // MZ - MZ:X	MP3 плеер доступен для использования 0-нет, 1-да - USE_MP3 == 1
  supportE131: boolean;            // E0 - устройство поддерживает групповую синхронизацию по протоколу E1.31 - USE_E131
  supportSD: boolean;              // SZ - устройство поддерживает SD-карту - USE_SD
  supportButton: boolean;          // UB - устройство поддерживает кнопку USE_BUTTON
  supportPower: boolean;           // PZ0 - устройство поддерживает управление питанием мaтрицы USE_POWER
  supportAlarmPower: boolean;      // PZ1 - устройство поддерживает линию управления питанием по включению будильника USE_ALARM
  supportAuxPower: boolean;        // PZ2 - устройство поддерживает дополнительную линию управления питанием USE_AUX
  brightness: number;              // BR - Текущая яркость
  game_speed: number;              // SE - Текущая скорость эффекта (игры)
  game_button_speed: number;       // BS - Текущая скорость повтора нажатия кнопки (игры)
  power: boolean;                  // PS - состояние питания: вкл/выкл
  isAutomatic: boolean;            // DM - состояние "автоматическая смена эффектов"
  isRandom: boolean;               // RM - состояние "выбор случайного эффекта"
  isAuxActive: boolean;            // FK - состояние канала управления по AUX
  auxPowerModes: number;           // FG - состояние канала управления по AUX (по режимам)
  effect: number;                  // EF - ID текущего режима (эффекта)
  special_effect: number;          // SM - ID текущего специального режима (эффекта) или -1 если такого нет
  effectName: string;              // EN - имя эффекта
  color: string;                   // CL - Текущий установленный пользовательский цвет
  uptime: number;                  // UP - время работы в секундах (uptime)
  freeMemory: number;              // FM - остаток свободной памяти в байтах
  ssid: string;                    // NW - ssid - имя сети
  password: string;                // NX - пароль подключения к сети
  ip: string;                      // IP - IP адрес устройства, заданный в настройках для статического IP
  ipc: string;                     // IPC- Текущий IP адрес устройства
  useDHCP: boolean;                // DH - использовать DHCP
  apName: string;                  // AN - имя точки доступа
  apPass: string;                  // AB - пароль подключения к точке доступа
  useAP: boolean;                  // AU - создавать точку доступа даже при подключении к сети
  e131_mode: number;               // E1 - режим - 0 - автономный, 1 - вещатель, 2 - слушатель
  e131_type: number;               // E2 - тип потока 0 - физический; 1 - логический, 2 - команды
  e131_group: number;              // E3 - номер группы синхронизации
  e131_streaming: boolean | undefined;  // E4 - активно ли вещание в настоящий момент
  mtx_seg_width: number;           // M0 - ширина сегмента матрицы 1..128
  mtx_seg_height: number;          // M1 - высота сегмента матрицы 1..128
  mtx_seg_type: number;            // M2 - тип сегмента матрицы - 0 - зигзаг; 1 - параллельная; 2 - карта индексов
  mtx_seg_angle: number;           // M3 - угол подключения диодов в сегменте: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
  mtx_seg_dir: number;             // M4 - направление ленты из угла сегмента: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
  tol_seg_width: number;           // M5 - количество сегментов в ширину составной матрицы
  tol_seg_height: number;          // M6 - количество сегментов в высоту составной матрицы
  tol_seg_type: number;            // M7 - соединение сегментов составной матрицы: 0 - зигзаг, 1 - параллельная
  tol_seg_angle: number;           // M8 - угол 1-го сегмента мета-матрицы: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
  tol_seg_dir: number;             // M9 - направление следующих сегментов мета-матрицы из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
  map_idx_list: ComboBoxItem[];    // M10 - список доступных файлов карт индексов
  map_idx: number;                 // M11 - индекс файла карты индексов в массиве mapFiles (при M2 == 2)
  effect_time: number;             // PD - продолжительность режима в секундах до смены на другой
  autochange_time: number;         // IT - время бездействия в минутах до перехода в демо-режим или 0 - выключено
  weather_type: number;            // WU	- Использовать получение погоды с сервера: 0 - выключено; 1 - Yandex; 2 - OpenWeatherMap
  weather_yandex: number;          // WR	- Регион погоды Yandex
  weather_owm: number;             // WS - Регион погоды OpenWeatherMap
  weather_update: number;          // WT	- Период запроса сведений о погоде в минутах
  curr_limit: number;              // PW	- Ограничения по току (миллиампер) или 0 - выключено
  fs_allow: boolean;               // FS - Доступность внутренней файловой системы микроконтроллера для хранения файлов: 0 - нет, 1 - да
  sd_allow: boolean;               // SX - Наличие и доступность SD карты в системе: Х = 0 - нат SD карты; 1 - SD карта доступна
  backup_place: number;            // EE - Наличие сохраненных настроек EEPROM на SD-карте или в файловой системе МК: 0 - нет 1 - есть в FS; 2 - есть на SD; 3 - есть в FS и на SD
  backup_file: string;             // BF - Имя файла сохраненных настроек EEPROM
  mode1_time: string;              // AM1T	- 'HH MM'	час 00..23 и минуты 00..59 включения режима 1, разделенные пробелом
  mode1_effect: number;            // AM1A	- NN	номер эффекта режима 1: -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  mode2_time: string;              // AM2T	- 'HH MM'	час 00..23 и минуты 00..59 включения режима 2, разделенные пробелом
  mode2_effect: number;            // AM2A	- NN	номер эффекта режима 2: -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  mode3_time: string;              // AM3T	- 'HH MM'	час 00..23 и минуты 00..59 включения режима 3, разделенные пробелом
  mode3_effect: number;            // AM3A	- NN	номер эффекта режима 3: -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  mode4_time: string;              // AM4T	- 'HH MM'	час 00..23 и минуты 00..59 включения режима 4, разделенные пробелом
  mode4_effect: number;            // AM4A	- NN	номер эффекта режима 4: -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  mode5_effect: number;            // AM5A	- NN	номер эффекта режима Рассвет: -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  mode6_effect: number;            // AM6A	- NN	номер эффекта режима Закат: -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  time_sunrise: string;            // T1	- 'HH:MM' время рассвета - час 00..23 и минуты 00..59 включения режима Закат, разделенные пробелом
  time_sunset: string;             // T2	- 'HH:MM' время заката 	час 00..23 и минуты 00..59 включения режима Рассвет, разделенные пробелом
  alarming: boolean;               // AL	- AL:X	сработал будильник 0-нет, 1-да
  alarm_effect: number;            // AE	- AE:число	эффект, использующийся для будильника
  dawn_duration: number;           // AD	- AD:число	продолжительность рассвета, мин
  alarm_time: string;              // AT	- AT: DW HH MM	часы-минуты времени будильника для дня недели DW 1..7 -> например "AT:1 09 15"
  alarm_weekday: number;           // AW	- AW:число	битовая маска дней недели будильника b6..b0: b0 - пн .. b7 - вс
  alarm_sound: number;             // MA	- MA:число	номер файла звука будильника из папки SD:/01
  dawn_sound: number;              // MB	- MB:число	номер файла звука рассвета из папки SD:/02
  alarm_duration: number;          // MD	- MD:число	сколько минут звучит будильник, если его не отключили
  alarm_playing_file: string;      // MP	- MP:папка~файл	номер папки и файла звука который проигрывается, номер папки и звука разделены '~'
  alarm_use_sound: boolean;        // MU	- MU:X	использовать звук в будильнике 0-нет, 1-да
  alarm_volume: number;            // MV	- MV:число	максимальная громкость будильника
  clock_allow_horizontal: boolean; // CH	- CH:X - доступны горизонтальные часы allowHorizontal
  clock_allow_vertical: boolean;   // CV	- CV:X - доступны вертикальные часы allowVertical
  clock_use_overlay: boolean;      // CE	- CE:X - оверлей часов вкл/выкл, где Х = 0 - выкл; 1 - вкл (использовать часы в эффектах) "X" - недоступно (!(allowVertical || allowHorizontal))
  clock_orientation: number;       // CO	-	CO:X - ориентация часов: 0 - горизонтально, 1 - вертикально
  clock_color_mode: number;        // CС	-	CС:X - режим цвета часов оверлея: 0,1,2
  clock_size: number;              // CK	-	CK:X - размер горизонтальных часов, где Х = 0 - авто; 1 - малые 3x5; 2 - большие 5x7
  clock_show_variant: number;      // CSV - CSV:X - флаги варианта размещения часов/календаря/температуры на поле матрицы
  clock_show_alignment: number;    // CSA - CSA:X - флаги варианта выравнивания часов/календаря/температуры в области отображения
  calendar_width: number;          // CF	-	CF:X - отображать в горизонтальном календаре 0/2/4 цифры годв
  clock_show_date: boolean;        // DC	-	DC:X - показывать дату вместе с часами 0-нет, 1-да
  clock_cycle_T1: number;          // T1	- T1:число - интервал показа даты при отображении часов (в секундах)
  clock_cycle_T2: number;          // T2	- T2:число - время показа даты при отображении часов (в секундах)
  clock_cycle_F1: number;          // F1	- F1:число - флаги отображения часов/календаря в циклах T1-T4
  clock_cycle_F2: number;          // F2	- F2:число - флаги отображения температуры в циклах T1-T4
  clock_show_temp: boolean;        // DW	- DW:X - показывать температуру вместе с малыми часами 0-нет, 1-да
  clock_temp_color_day: boolean;   // WC	- WC:X - показывать цветную температуру днем 0-нет, 1-да
  clock_temp_color_night: boolean; // WN	- WN:X - показывать цветную температуру ночью 0-нет, 1-да
  clock_night_color: number;       // NC	- NС:Х - цвет ночных часов, где Х = 0 - R; 1 - G; 2 - B; 3 - C; 4 - M; 5 - Y;
  clock_use_ntp: boolean;          // NP	- NP:Х - использовать NTP, где Х = 0 - выкл; 1 - вкл
  clock_ntp_server: string;        // NS	- NS:текст - сервер NTP
  clock_time_zone: string;         // NZ	- NZ:текст - правило часовой зоны
  clock_tm1627_off: boolean;       // OF	- OF:X - выключать часы вместе с лампой 0-нет, 1-да
  clock_scroll_speed: number;      // SC	- SC:число - скорость смещения часов оверлея
  clock_dot_width: number;         // CD  - CD:число - ширина точки в больших часах 2 или 1
  clock_dot_space: number;         // CS  - CS:X - наличие пробела между разделительной точкой больших часов и окружающими цифрами 0-нет, 1-да
  clock_offset_x: number;          // СX  - CX:число - смещение часов по оси X
  clock_offset_y: number;          // СY  - CY:число - смещение часов по оси Y
  temp_offset_x: number;           // СXT - СXT:число - смещение температуры по оси X
  temp_offset_y: number;           // СYT - СYT:число - смещение температуры по оси Y
  calendar_offset_x: number;       // СXC - СXC:число - смещение календаря по оси X
  calendar_offset_y: number;       // СYC - СYC:число - смещение календаря по оси Y
  text_offset_y: number;           // СYL - СYL:число - смещение бегущей строки по оси Y
  text_color_mode: number;         // CT  - CT:X - режим цвета текстовой строки: 0 - монохром, 1 - радуга, 2 - каждая буква своим цветом
  text_use_overlay: boolean;       // TE  - TE:X - оверлей текста бегущей строки вкл/выкл, где Х = 0 - выкл; 1 - вкл (использовать бегущую строку в эффектах)
  text_interval: number;           // TI  - TI:число - интервал отображения текста бегущей строки
  text_free_memory: number;        // OM  - OM:X - сколько ячеек осталось свободно для хранения строк
  text_scroll_speed: number;       // ST  - ST:число - скорость смещения бегущей строки
  text_cells_type: string;         // TS  - TS:строка	- строка состояния кнопок выбора текста из массива строк: 36 символов 0..5, где
                                   //   - 0 - серый   - пустая
                                   //   - 1 - черный  - отключена
                                   //   - 2 - зеленый - активна - просто текст, без макросов
                                   //   - 3 - голубой - активна, содержит макросы кроме даты
                                   //   - 4 - синий   - активная, содержит макрос даты
                                   //   - 5 - красный - для строки 0 - это управляющая строка
  hide_on_text_running: number;    // HTR  - HTR:число - скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
  text_edit: string;               // TY  - TY:I:Z > текст  - текст для строки, с указанным индексом I 0..35, Z 0..9,A..Z. текст ответа в формате: 'I:Z > текст';
  text_color: string;              // C2 - C2:цвет - цвет режима "монохром" бегущей строки; цвет: 192,96,96 - R,G,B
  image_line: string;              // IR - IR N|RRGGBB,RRGGBB,...,RRGGBB - прием строки изображения; IC - IC N|RRGGBB,RRGGBB,...,RRGGBB - прием колонки изображения
  sync_master_x: number;           // EMX - EMX:X - позиция X левого верхнего угла мастера, откуда на приемнике идет захват изображения с Мастера
  sync_master_y: number;           // EMY - EMY:X - позиция Y левого верхнего угла мастера, откуда на приемнике идет захват изображения с Мастера
  sync_local_x: number;            // ELX - ELX:X - позиция X левого верхнего угла окна приемника, куда идет вывод изображения с Мастера
  sync_local_y: number;            // ELY - ELY:X - позиция Y левого верхнего угла окна приемника, куда идет вывод изображения с Мастера
  sync_local_w: number;            // ELW - ELW:X - ширина окна приемника, куда идет вывод изображения с Мастера
  sync_local_h: number;            // ELH - ELH:X - высота окна приемника, куда идет вывод изображения с Мастера

  controller: string;              // MC	MC:текст	тип микроконтроллера "ESP32", "NodeMCU", "Wemos d1 mini"
  led_line_1: string;              // 2306:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  led_line_2: string;              // 2307:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  led_line_3: string;              // 2308:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  led_line_4: string;              // 2309:U P S L     подключение матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  device_type: number;             // 2310:X           тип матрицы vDEVICE_TYPE: 0 - труба; 1 - панель
  button_type: number;             // 2311:X Y         X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
  button_pin: number;              // 2311:X Y         X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
  power_level: number;             // 2312:X Y         X - GPIO пин управления питанием; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
  power_pin: number;               // 2312:X Y         X - GPIO пин управления питанием; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
  wait_play_finished: boolean;     // 2313:X Y         X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
  repeat_play: boolean;            // 2313:X Y         X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
  debug_serial: boolean;           // 2314:X           vDEBUG_SERIAL
  player_tx_pin: number;           // 2315:X Y         X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
  player_rx_pin: number;           // 2315:X Y         X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
  tm1637_dio_pin: number;          // 2316:X Y         X - GPIO пин DIO на TM1637; Y - GPIO пин CLK на TM1637
  tm1637_clk_pin: number;          // 2316:X Y         X - GPIO пин DIO на TM1637; Y - GPIO пин CLK на TM1637
  power_alarm_pin: number;         // 2317:X Y         X - GPIO пин управления питанием по будильнику; Y - уровень управления питанием по будильнику - 0 - LOW; 1 - HIGH
  power_alarm_level: number;       // 2317:X Y         X - GPIO пин управления питанием по будильнику; Y - уровень управления питанием по будильнику - 0 - LOW; 1 - HIGH
  power_aux_pin: number;           // 2318:X Y         X - GPIO пин управления питанием по доп.каналу; Y - уровень управления питанием по доп.каналу - 0 - LOW; 1 - HIGH
  power_aux_level: number;         // 2318:X Y         X - GPIO пин управления питанием по доп.каналу; Y - уровень управления питанием по доп.каналу - 0 - LOW; 1 - HIGH
  param2: string;                  // EDIT:текст       параметр param2 (SQ) для редактирования эффекта
  // PN:список   список пинов, разделенный запятыми: SS,MOSI,MISO,SCK,SDA,SCL,TX,RX
  hw_ss:number;                    // SS (CS) - SPI Chip Select
  hw_mosi:number;                  // MOSI    - SPI Master Output Slave Input
  hw_miso:number;                  // MISO    - SPI Master Input Slave Output
  hw_sck:number;                   // SCK     - SPI Clock
  hw_sda:number;                   // SDA     - I2C Data
  hw_scl:number;                   // SCL     - I2C Clock
  hw_tx:number;                    // TX      - UART TX
  hw_rx:number;                    // RX      - UART RX
  time12h: boolean;                // C12:X             X = 0 24H; X=1 12H
  small_font_type: number;         // C35:X             X = 0 квадратный; X=1 скругленный шрифт 3х5
  weather_farenheit: boolean;      // TF:X              X = 0 Celsius; X=1 Fahrenheit
  show_temp_props: number;         // WV:x              X - битовая карта b0 -  показ в температуре в часах C/F; b1 - показ значка градуса
  show_temp_text_props: number;    // WW:x              X - битовая карта b0 -  показ в температуре в макросе {WT} C/F; b1 - показ значка градуса
  debug: boolean;
  debug_hour: number;
  debug_minutes: number;
  debug_temperature: number;
  debug_day: number;
  debug_month: number;
  debug_year: number;
  debug_cross: boolean;
  debug_frame: boolean;
  text_action: string;
}

export class StateModel implements IStateModel {
  public version = '';
  public hostName = '';
  public last_error = '';
  public last_info = '';
  public width = 0;
  public height = 0;
  public supportTM1637 = false;
  public supportWeather = false;
  public supportMP3 = false;
  public supportPlayer = false;
  public supportE131 = false;
  public supportSD = false;
  public supportButton = false;
  public supportPower = false;
  public supportAlarmPower = false;
  public supportAuxPower = false;
  public brightness = 25;
  public game_speed = 127;
  public game_button_speed = 50;
  public power = false;
  public isAutomatic = false;
  public isRandom = false;
  public isAuxActive = false;
  public auxPowerModes = 0;
  public effect = -1;
  public special_effect = -1;
  public effectName = '';
  public color: string = '#000000';
  public uptime = 0;
  public freeMemory = 0;
  public ssid = '';
  public password = '';
  public ip = '';
  public ipc = '';
  public useDHCP = false;
  public apName = '';
  public apPass = '';
  public useAP = false;
  public e131_mode = 0;
  public e131_type = 0;
  public e131_group = 0;
  public e131_streaming: boolean | undefined = undefined;
  public mtx_seg_width = 16;
  public mtx_seg_height = 16;
  public mtx_seg_type = 0;
  public mtx_seg_angle = 0;
  public mtx_seg_dir = 0;
  public tol_seg_width = 1;
  public tol_seg_height = 1;
  public tol_seg_type = 0;
  public tol_seg_angle = 0;
  public tol_seg_dir = 0;
  public map_idx_list = new Array<ComboBoxItem>();
  public map_idx = -1;
  public effect_time = 30;
  public autochange_time = 0;
  public weather_type = 0;
  public weather_yandex = 0;
  public weather_owm = 0;
  public weather_update = 60;
  public curr_limit = 1000;
  public fs_allow = false;
  public sd_allow = false;
  public backup_place = 0;
  public backup_file = '';
  public mode1_time = "";
  public mode1_effect = -3;
  public mode2_time = "";
  public mode2_effect = -3;
  public mode3_time = "";
  public mode3_effect = -3;
  public mode4_time = "";
  public mode4_effect = -3;
  public mode5_effect = -3;
  public mode6_effect = -3;
  public time_sunrise = "";
  public time_sunset = "";
  public alarming = false;
  public alarm_effect = -1;
  public dawn_duration = 10;
  public alarm_time = '';
  public alarm_weekday = 0;
  public alarm_sound = -1;
  public dawn_sound = -1;
  public alarm_duration = 1;
  public alarm_playing_file = '';
  public alarm_use_sound = false;
  public alarm_volume = 30;
  public clock_allow_horizontal = true;
  public clock_allow_vertical = true;
  public clock_use_overlay = false;
  public clock_orientation = 0;
  public clock_color_mode = 0;
  public clock_size = 0;
  public calendar_width = 2;
  public clock_show_variant = 0;
  public clock_show_alignment   = 0;
  public clock_show_date = false;
  public clock_cycle_T1 = 240;
  public clock_cycle_T2 = 2;
  public clock_cycle_F1 = 0;
  public clock_cycle_F2 = 0;
  public clock_show_temp = false;
  public clock_temp_color_day = false;
  public clock_temp_color_night = false;
  public clock_night_color = 0;
  public clock_use_ntp = false;
  public clock_ntp_server = 'ru.pool.ntp.org';
  public clock_time_zone = '';
  public clock_tm1627_off = true;
  public clock_scroll_speed = 255;
  public clock_dot_width = 2;
  public clock_dot_space = 1;
  public clock_offset_x = 0;
  public clock_offset_y = 0;
  public hide_on_text_running = 0;
  public temp_offset_x = 0;
  public temp_offset_y = 0;
  public calendar_offset_x = 0;
  public calendar_offset_y = 0;
  public text_offset_y = 0;
  public text_color_mode = 0;
  public text_use_overlay = false;
  public text_interval = 600;
  public text_free_memory = 0;
  public text_scroll_speed = 128;
  public text_cells_type = '';
  public text_edit = '';
  public text_color = '';
  public image_line = '';
  public sync_master_x = 0;
  public sync_master_y = 0;
  public sync_local_x = 0;
  public sync_local_y = 0;
  public sync_local_w = 16;
  public sync_local_h = 16;
  public controller = "";
  public led_line_1 = "";
  public led_line_2 = "";
  public led_line_3 = "";
  public led_line_4 = "";
  public device_type = 1;
  public button_type = 1;
  public button_pin = -1;
  public power_level = 1;
  public power_alarm_level = 1;
  public power_aux_level = 1;
  public power_pin = -1;
  public power_alarm_pin = -1;
  public power_aux_pin = -1;
  public wait_play_finished = true;
  public repeat_play = true;
  public debug_serial = true;
  public player_tx_pin = -1;
  public player_rx_pin = -1;
  public tm1637_dio_pin = -1;
  public tm1637_clk_pin = -1;
  public param2 = '';
  public hw_ss = -1;
  public hw_mosi = -1;
  public hw_miso = -1;
  public hw_sck = -1;
  public hw_sda = -1;
  public hw_scl = -1;
  public hw_tx = -1;
  public hw_rx = -1;
  public time12h = false;
  public small_font_type = 0;
  public weather_farenheit = false;
  public show_temp_props = 0;
  public show_temp_text_props = 0;
  public debug_hour = 0;
  public debug_minutes = 0;
  public debug_temperature = 0;
  public debug_day = 0;
  public debug_month = 0;
  public debug_year = 0;
  public debug = false;
  public debug_cross = false;
  public debug_frame = false;
  public text_action = '';

  constructor() {
    const date= new Date();
    this.debug_hour = date.getHours();
    this.debug_minutes = date.getMinutes();
    this.debug_temperature = 0;
    this.debug_day = date.getDate();
    this.debug_month = date.getMonth() + 1;
    this.debug_year = date.getFullYear();
    this.debug_cross = false;
    this.debug_frame = false;
  }

  setValue(key: string, value: any): any {
    // @ts-ignore
    this[key] = value;
  }

  getValueByKey(key: string): any {
    // @formatter:off
    switch (key) {
      case 'W':    return this.width;
      case 'H':    return this.height;
      case 'C':    return this.image_line;
      case 'L':    return this.image_line;
      case 'IR':   return this.image_line;
      case 'IC':   return this.image_line;
      case 'VR':   return this.version;
      case 'HN':   return this.hostName;
      case 'ER':   return this.last_error;
      case 'NF':   return this.last_info;
      case 'TM':   return this.supportTM1637;
      case 'WZ':   return this.supportWeather;
      case 'MX':   return this.supportMP3;
      case 'MZ':   return this.supportPlayer;
      case 'SZ':   return this.supportSD;
      case 'UB':   return this.supportButton;
      case 'PZ0':  return this.supportPower;
      case 'PZ1':  return this.supportAlarmPower;
      case 'PZ2':  return this.supportAuxPower;
      case 'BR':   return this.brightness;
      case 'BS':   return this.game_button_speed;
      case 'SE':   return this.game_speed;
      case 'PS':   return this.power;
      case 'SM':   return this.special_effect;
      case 'EF':   return this.effect;
      case 'EN':   return this.effectName;
      case 'DM':   return this.isAutomatic;
      case 'RM':   return this.isRandom;
      case 'FK':   return this.isAuxActive;
      case 'FG':   return this.auxPowerModes;
      case 'UP':   return this.uptime;
      case 'FM':   return this.freeMemory;
      case 'CL':   return this.color;
      case 'NW':   return this.ssid;
      case 'NA':   return this.password;
      case 'NX':   return this.password;
      case 'IP':   return this.ip;
      case 'IPС':  return this.ipc;
      case 'DH':   return this.useDHCP;
      case 'AN':   return this.apName;
      case 'AB':   return this.apPass;
      case 'AU':   return this.useAP;
      case 'E0':   return this.supportE131;
      case 'E1':   return this.e131_mode;
      case 'E2':   return this.e131_type;
      case 'E3':   return this.e131_group;
      case 'E4':   return this.e131_streaming;
      case 'M0':   return this.mtx_seg_width;
      case 'M1':   return this.mtx_seg_height;
      case 'M2':   return this.mtx_seg_type;
      case 'M3':   return this.mtx_seg_angle;
      case 'M4':   return this.mtx_seg_dir;
      case 'M5':   return this.tol_seg_width;
      case 'M6':   return this.tol_seg_height;
      case 'M7':   return this.tol_seg_type;
      case 'M8':   return this.tol_seg_angle;
      case 'M9':   return this.tol_seg_dir;
      case 'M10':  return this.map_idx_list;
      case 'M11':  return this.map_idx;
      case 'PD':   return this.effect_time;
      case 'IT':   return this.autochange_time;
      case 'WU':   return this.weather_type;
      case 'WR':   return this.weather_yandex;
      case 'WS':   return this.weather_owm;
      case 'WT':   return this.weather_update;
      case 'PW':   return this.curr_limit;
      case 'FS':   return this.fs_allow;
      case 'SX':   return this.sd_allow;
      case 'EE':   return this.backup_place;
      case 'BF':   return this.backup_file;
      case 'AM1T': return this.mode1_time;
      case 'AM1A': return this.mode1_effect;
      case 'AM2T': return this.mode2_time;
      case 'AM2A': return this.mode2_effect;
      case 'AM3T': return this.mode3_time;
      case 'AM3A': return this.mode3_effect;
      case 'AM4T': return this.mode4_time;
      case 'AM4A': return this.mode4_effect;
      case 'AM5A': return this.mode5_effect;
      case 'AM6A': return this.mode6_effect;
      case 'T1':   return this.time_sunrise;
      case 'T2':   return this.time_sunset;
      case 'AL':   return this.alarming;
      case 'AE':   return this.alarm_effect;
      case 'AD':   return this.dawn_duration;
      case 'AT':   return this.alarm_time;
      case 'AW':   return this.alarm_weekday;
      case 'MA':   return this.alarm_sound;
      case 'MB':   return this.dawn_sound;
      case 'MD':   return this.alarm_duration;
      case 'MP':   return this.alarm_playing_file;
      case 'MU':   return this.alarm_use_sound;
      case 'MV':   return this.alarm_volume;
      case 'CH':   return this.clock_allow_horizontal;
      case 'CV':   return this.clock_allow_vertical;
      case 'CE':   return this.clock_use_overlay;
      case 'CO':   return this.clock_orientation;
      case 'CC':   return this.clock_color_mode;
      case 'CK':   return this.clock_size;
      case 'CSV':  return this.clock_show_variant;
      case 'CSA':  return this.clock_show_alignment;
      case 'HTR':  return this.hide_on_text_running;
      case 'CF':   return this.calendar_width;
      case 'DC':   return this.clock_show_date;
      case 'CT1':  return this.clock_cycle_T1;
      case 'CT2':  return this.clock_cycle_T2;
      case 'CF1':  return this.clock_cycle_F1;
      case 'CF2':  return this.clock_cycle_F2;
      case 'DW':   return this.clock_show_temp;
      case 'WC':   return this.clock_temp_color_day;
      case 'WN':   return this.clock_temp_color_night;
      case 'NC':   return this.clock_night_color;
      case 'NP':   return this.clock_use_ntp;
      case 'NS':   return this.clock_ntp_server;
      case 'NZ':   return this.clock_time_zone;
      case 'OF':   return this.clock_tm1627_off;
      case 'SC':   return this.clock_scroll_speed;
      case 'CD':   return this.clock_dot_width;
      case 'CS':   return this.clock_dot_space;
      case 'CX':   return this.clock_offset_x;
      case 'CY':   return this.clock_offset_y;
      case 'CXT':  return this.temp_offset_x;
      case 'CYT':  return this.temp_offset_y;
      case 'CXC':  return this.calendar_offset_x;
      case 'CYC':  return this.calendar_offset_y;
      case 'CYL':  return this.text_offset_y;
      case 'CT':   return this.text_color_mode;
      case 'TE':   return this.text_use_overlay;
      case 'TI':   return this.text_interval;
      case 'OM':   return this.text_free_memory;
      case 'ST':   return this.text_scroll_speed;
      case 'TS':   return this.text_cells_type;
      case 'TY':   return this.text_edit;
      case 'C2':   return this.text_color;
      case 'EMX':  return this.sync_master_x;
      case 'EMY':  return this.sync_master_y;
      case 'ELX':  return this.sync_local_x;
      case 'ELY':  return this.sync_local_y;
      case 'ELW':  return this.sync_local_w;
      case 'ELH':  return this.sync_local_h;
      case 'TF':   return this.weather_farenheit;
      case 'WV':   return this.show_temp_props;
      case 'WW':   return this.show_temp_text_props;
      case 'C12':  return this.time12h;
      case 'C35':  return this.small_font_type;
      case 'TXT':  return this.text_action;

      case 'MC':   return this.controller;
      case '2306': return this.led_line_1;
      case '2307': return this.led_line_2;
      case '2308': return this.led_line_3;
      case '2309': return this.led_line_4;
      case '2310': return this.device_type;
      case '2311': return `${this.button_pin} ${this.button_type}`;             // 2311:X Y  -> X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
      case '2312': return `${this.power_pin} ${this.power_level}`;              // 2312:X Y  -> X - GPIO пин управления питанием; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
      case '2313': return `${this.wait_play_finished} ${this.repeat_play}`;     // 2313:X Y  -> X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
      case '2314': return this.debug_serial;
      case '2315': return `${this.player_tx_pin} ${this.player_rx_pin}`;        // 2315:X Y  -> X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
      case '2316': return `${this.tm1637_dio_pin} ${this.tm1637_clk_pin}`;      // 2316:X Y  -> X - GPIO пин DIO на TM1637; Y - GPIO пин CLK на TM1637
      case '2317': return `${this.power_alarm_pin} ${this.power_alarm_level}`;  // 2317:X Y  -> X - GPIO пин управления питанием по будильнику; Y - уровень управления питанием по будильнику - 0 - LOW; 1 - HIGH
      case '2318': return `${this.power_aux_pin} ${this.power_aux_level}`;      // 2318:X Y  -> X - GPIO пин управления питанием по доп.каналу; Y - уровень управления питанием по доп.каналу - 0 - LOW; 1 - HIGH
      case 'EDIT': return this.param2;
    }
    // @formatter:on
    return null;
  }

  setValueByKey(key: string, value: any): any {
    // @formatter:off
    switch (key) {
      case 'W':    this.width = Number(value);                                    break;
      case 'H':    this.height = Number(value);                                   break;
      case 'C':    this.image_line = '' + value;                                  break;
      case 'L':    this.image_line = '' + value;                                  break;
      case 'IR':   this.image_line = '' + value;                                  break;
      case 'IC':   this.image_line = '' + value;                                  break;
      case 'VR':   this.version = '' + value;                                     break;
      case 'HN':   this.hostName = '' + value;                                    break;
      case 'ER':   this.last_error = '' + value;                                  break;
      case 'NF':   this.last_info = '' + value;                                   break;
      case 'TM':   this.supportTM1637 = Number(value) === 1;                      break;
      case 'WZ':   this.supportWeather = Number(value) === 1;                     break;
      case 'MX':   this.supportMP3 = Number(value) === 1;                         break;
      case 'MZ':   this.supportPlayer = Number(value) === 1;                      break;
      case 'SZ':   this.supportSD = Number(value) === 1;                          break;
      case 'UB':   this.supportButton = Number(value) === 1;                      break;
      case 'PZ0':  this.supportPower = Number(value) === 1;                       break;
      case 'PZ1':  this.supportAlarmPower = Number(value) === 1;                  break;
      case 'PZ2':  this.supportAuxPower = Number(value) === 1;                    break;
      case 'BR':   this.brightness = Number(value);                               break;
      case 'BS':   this.game_button_speed = Number(value);                        break;
      case 'SE':   this.game_speed = Number(value);                               break;
      case 'PS':   this.power = Number(value) === 1;                              break;
      case 'SM':   this.special_effect = Number(value);                           break;
      case 'EF':   this.effect = Number(value);                                   break;
      case 'EN':   this.effectName = '' + value;                                  break;
      case 'DM':   this.isAutomatic = Number(value) === 1;                        break;
      case 'RM':   this.isRandom = Number(value) === 1;                           break;
      case 'FK':   this.isAuxActive = Number(value) === 1;                        break;
      case 'FG':   this.auxPowerModes = Number(value);                            break;
      case 'UP':   this.uptime = Number(value);                                   break;
      case 'FM':   this.freeMemory = Number(value);                               break;
      case 'CL':   this.color = '#' + value;                                      break;
      case 'NW':   this.ssid = '' + value;                                        break;
      case 'NA':   this.password = '' + value;                                    break;
      case 'NX':   this.password = '' + value;                                    break;
      case 'IP':   this.ip = '' + value;                                          break;
      case 'IPC':  this.ipc = '' + value;                                         break;
      case 'DH':   this.useDHCP = Number(value) === 1;                            break;
      case 'AN':   this.apName = '' + value;                                      break;
      case 'AB':   this.apPass = '' + value;                                      break;
      case 'AU':   this.useAP = Number(value) === 1;                              break;
      case 'E0':   this.supportE131 = Number(value) === 1;                        break;
      case 'E1':   this.e131_mode = Number(value);                                break;
      case 'E2':   this.e131_type = Number(value);                                break;
      case 'E3':   this.e131_group = Number(value);                               break;
      case 'E4':   this.e131_streaming = Number(value) === 1;                     break;
      case 'M0':   this.mtx_seg_width = Number(value);                            break;
      case 'M1':   this.mtx_seg_height = Number(value);                           break;
      case 'M2':   this.mtx_seg_type = Number(value);                             break;
      case 'M3':   this.mtx_seg_angle = Number(value);                            break;
      case 'M4':   this.mtx_seg_dir = Number(value);                              break;
      case 'M5':   this.tol_seg_width = Number(value);                            break;
      case 'M6':   this.tol_seg_height = Number(value);                           break;
      case 'M7':   this.tol_seg_type = Number(value);                             break;
      case 'M8':   this.tol_seg_angle = Number(value);                            break;
      case 'M9':   this.tol_seg_dir = Number(value);                              break;
      case 'M10': {
          this.map_idx_list = new Array<ComboBoxItem>();
          const parts = ('' + value).split(',');
          let idx = 0;
          parts.forEach(val => {
            const itm = new ComboBoxItem(val, idx++);
            this.map_idx_list.push(itm);
          });
        }
        break;
      case 'M11':  this.map_idx = Number(value);                                  break;
      case 'PD':   this.effect_time = Number(value);                              break;
      case 'IT':   this.autochange_time = Number(value);                          break;
      case 'WU':   this.weather_type = Number(value);                             break;
      case 'WR':   this.weather_yandex = Number(value);                           break;
      case 'WS':   this.weather_owm = Number(value);                              break;
      case 'WT':   this.weather_update = Number(value);                           break;
      case 'PW':   this.curr_limit = Number(value);                               break;
      case 'FS':   this.fs_allow = Number(value) === 1;                           break;
      case 'SX':   this.sd_allow = Number(value) === 1;                           break;
      case 'EE':   this.backup_place = Number(value);                             break;
      case 'BF':   this.backup_file = '' + value;                                 break;
      case 'AM1T': this.mode1_time = '' + value;                                  break;
      case 'AM1A': this.mode1_effect = Number(value);                             break;
      case 'AM2T': this.mode2_time = '' + value;                                  break;
      case 'AM2A': this.mode2_effect = Number(value);                             break;
      case 'AM3T': this.mode3_time = '' + value;                                  break;
      case 'AM3A': this.mode3_effect = Number(value);                             break;
      case 'AM4T': this.mode4_time = '' + value;                                  break;
      case 'AM4A': this.mode4_effect = Number(value);                             break;
      case 'AM5A': this.mode5_effect = Number(value);                             break;
      case 'AM6A': this.mode6_effect = Number(value);                             break;
      case 'T1':   this.time_sunrise = '' + value;                                break;
      case 'T2':   this.time_sunset = '' + value;                                 break;
      case 'AL':   this.alarming = Number(value) === 1;                           break;
      case 'AE':   this.alarm_effect = Number(value);                             break;
      case 'AD':   this.dawn_duration = Number(value);                            break;
      case 'AT':   this.alarm_time = '' + value;                                  break;
      case 'AW':   this.alarm_weekday = Number(value);                            break;
      case 'MA':   this.alarm_sound = Number(value);                              break;
      case 'MB':   this.dawn_sound = Number(value);                               break;
      case 'MD':   this.alarm_duration = Number(value);                           break;
      case 'MP':   this.alarm_playing_file = '' + value;                          break;
      case 'MU':   this.alarm_use_sound = Number(value) === 1;                    break;
      case 'MV':   this.alarm_volume = Number(value);                             break;
      case 'CH':   this.clock_allow_horizontal = Number(value) === 1;             break;
      case 'CV':   this.clock_allow_vertical = Number(value) === 1;               break;
      case 'CE':   this.clock_use_overlay = Number(value) === 1;                  break;
      case 'CO':   this.clock_orientation = Number(value);                        break;
      case 'CC':   this.clock_color_mode = Number(value);                         break;
      case 'CK':   this.clock_size = Number(value);                               break;
      case 'CSV':  this.clock_show_variant = Number(value);                       break;
      case 'CSA':  this.clock_show_alignment = Number(value);                     break;
      case 'HTR':  this.hide_on_text_running = Number(value);                     break;
      case 'CF':   this.calendar_width = Number(value);                           break;
      case 'DC':   this.clock_show_date = Number(value) === 1;                    break;
      case 'CT1':  this.clock_cycle_T1 = Number(value);                           break;
      case 'CT2':  this.clock_cycle_T2 = Number(value);                           break;
      case 'CF1':  this.clock_cycle_F1 = Number(value);                           break;
      case 'CF2':  this.clock_cycle_F2 = Number(value);                           break;
      case 'DW':   this.clock_show_temp = Number(value) === 1;                    break;
      case 'WC':   this.clock_temp_color_day = Number(value) === 1;               break;
      case 'WN':   this.clock_temp_color_night = Number(value) === 1;             break;
      case 'NC':   this.clock_night_color = Number(value);                        break;
      case 'NP':   this.clock_use_ntp = Number(value) === 1;                      break;
      case 'NS':   this.clock_ntp_server = '' + value;                            break;
      case 'NZ':   this.clock_time_zone = '' + value;                             break;
      case 'OF':   this.clock_tm1627_off = Number(value) === 1;                   break;
      case 'SC':   this.clock_scroll_speed = Number(value);                       break;
      case 'CD':   this.clock_dot_width = Number(value);                          break;
      case 'CS':   this.clock_dot_space = Number(value);                          break;
      case 'CX':   this.clock_offset_x = Number(value);                           break;
      case 'CY':   this.clock_offset_y = Number(value);                           break;
      case 'CXT':  this.temp_offset_x = Number(value);                            break;
      case 'CYT':  this.temp_offset_y = Number(value);                            break;
      case 'CXC':  this.calendar_offset_x = Number(value);                        break;
      case 'CYC':  this.calendar_offset_y = Number(value);                        break;
      case 'CYL':  this.text_offset_y = Number(value);                            break;
      case 'CT':   this.text_color_mode = Number(value);                          break;
      case 'TE':   this.text_use_overlay = Number(value) === 1;                   break;
      case 'TI':   this.text_interval = Number(value);                            break;
      case 'OM':   this.text_free_memory = Number(value);                         break;
      case 'ST':   this.text_scroll_speed = Number(value);                        break;
      case 'TS':   this.text_cells_type = '' + value;                             break;
      case 'TY':   this.text_edit = '' + value;                                   break;
      case 'C2':   this.text_color = '' + value;                                  break;
      case 'TF':   this.weather_farenheit = Number(value) === 1;                  break;
      case 'WV':   this.show_temp_props = Number(value);                          break;
      case 'WW':   this.show_temp_text_props = Number(value);                     break;
      case 'C12':  this.time12h = Number(value) === 1;                            break;
      case 'C35':  this.small_font_type = Number(value);                          break;
      case 'TXT':  this.text_action = '' + value;                                 break;

      case 'EMX':  this.sync_master_x = Number(value);                            break;
      case 'EMY':  this.sync_master_y = Number(value);                            break;
      case 'ELX':  this.sync_local_x = Number(value);                             break;
      case 'ELY':  this.sync_local_y = Number(value);                             break;
      case 'ELW':  this.sync_local_w = Number(value);                             break;
      case 'ELH':  this.sync_local_h = Number(value);                             break;
      case 'EDIT': this.param2 = '' + value;                                      break;

      case 'MC':   this.controller = '' + value;                                  break;
      case '2306': this.led_line_1 = '' + value;                                  break;
      case '2307': this.led_line_2 = '' + value;                                  break;
      case '2308': this.led_line_3 = '' + value;                                  break;
      case '2309': this.led_line_4 = '' + value;                                  break;
      case '2310': this.device_type = Number(value);                              break;
      case '2311': {
          // 2311:X Y         X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
          const parts = ('' + value).split(' ');
          this.button_pin = Number(parts[0]);
          this.button_type = Number(parts[1]);
        }
        break;
      case '2312': {
          // 2312:X Y         X - GPIO пин управления питанием; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
          const parts = ('' + value).split(' ');
          this.power_pin = Number(parts[0]);
          this.power_level = Number(parts[1]);
        }
        break;
      case '2313': {
          // 2313:X Y         X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
          const parts = ('' + value).split(' ');
          this.wait_play_finished = Number(parts[0]) === 1 || parts[0].trim() === '1';
          this.repeat_play = Number(parts[1]) === 1 || parts[1].trim() == '1';
        }
        break;
      case '2314':   this.debug_serial = Number(value) === 1; break;
      case '2315': {
          // 2315:X Y         X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
          const parts = ('' + value).split(' ');
          this.player_tx_pin = Number(parts[0]);
          this.player_rx_pin = Number(parts[1]);
        }
        break;
      case '2316': {
          // 2316:X Y         X - GPIO пин DIO на TM1637; Y - GPIO пин CLK на TM1637
          const parts = ('' + value).split(' ');
          this.tm1637_dio_pin = Number(parts[0]);
          this.tm1637_clk_pin = Number(parts[1]);
        }
        break;
      case '2317': {
        // 2317:X Y         X - GPIO пин управления питанием по будильнику; Y - уровень управления питанием по будильнику - 0 - LOW; 1 - HIGH
        const parts = ('' + value).split(' ');
        this.power_alarm_pin = Number(parts[0]);
        this.power_alarm_level = Number(parts[1]);
      }
      break;
      case '2318': {
        // 2318:X Y         X - GPIO пин управления питанием по доп.каналу; Y - уровень управления питанием по доп.каналу - 0 - LOW; 1 - HIGH
        const parts = ('' + value).split(' ');
        this.power_aux_pin = Number(parts[0]);
        this.power_aux_level = Number(parts[1]);
      }
      break;
      case 'PN': {
        // PN:список   список пинов, разделенный запятыми: SS,MOSI,MISO,SCK,SDA,SCL,TX,RX
        // Эти пины в цифровом назначении зависят от выбранного типа платы микроконтроллера
        const parts = ('' + value).split(',');
        this.hw_ss   = Number(parts[0]);
        this.hw_mosi = Number(parts[1]);
        this.hw_miso = Number(parts[2]);
        this.hw_sck  = Number(parts[3]);
        this.hw_sda  = Number(parts[4]);
        this.hw_scl  = Number(parts[5]);
        this.hw_tx   = Number(parts[6]);
        this.hw_rx   = Number(parts[7]);
      }
      break;
      case 'DBG':
        const str = ('' + value);
        this.debug       = str.length > 0 && str[0] === '1';
        this.debug_frame = str.length > 1 && str[1] === '1';
        this.debug_cross = str.length > 2 && str[2] === '1';
        break;
    }
    // @formatter:on
  }

  isNightClockRunnung(): boolean {
    return this.special_effect === 3;
  }

  isGameRunning(): boolean {
    return this.isGameMaze() || this.isGameTetris() || this.isGameSnake() || this.isGameArkanoid();
  }

  isGameMaze(): boolean {
    return this.effect === 27;    // MC_MAZE
  }

  isGameTetris(): boolean {
    return this.effect === 29;    // MC_TETRIS
  }

  isGameArkanoid(): boolean {
    return this.effect === 30;    // MC_ARKANOID
  }

  isGameSnake(): boolean {
    return this.effect === 28;    // MC_SNAKE
  }

  isDrawingActive(): boolean {
    return this.effect === 123 || this.effect === 124;  // MC_DRAW || MC_LOADIMAGE
  }
}
