// Гайд по постройке матрицы: https://alexgyver.ru/matrix_guide/
// Страница проекта на GitHub: https://github.com/vvip-68/LedPanelWiFi
// Автор идеи, начальный проект - GyverMatrixBT: AlexGyver Technologies, 2019 (https://alexgyver.ru/gyvermatrixbt/)
// Дальнейшее развитие: vvip-68, 2019-2024
//
// Дополнительные ссылки для Менеджера плат ESP8266 и ESP32 в Файл -> Настройки
// https://raw.githubusercontent.com/esp8266/esp8266.github.io/master/stable/package_esp8266com_index.json
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#define FIRMWARE_VER F("WiFiPanel v.1.14с.2024.0127")

// -------------------------------------------------------------------------------------------------------
//
// Внимание!!!
// Проект разрабатывался и тестировался для плат разработчика на базе 
//   ESP8266 - 'NodeMCU ESP-12 DevKit v1.0' с CH340, CH341, CP2102, 'Wemos d1 mini' различных вариаций - у них у всех одинаковое назначение пинов
//   ESP32   - 'ESP32-WROOM-32 DevKit' 30,32,38-pin - у них одинаковое назначение пинов, но возможно не все выведены на гребенку
//
// Также микроконтроллер ESP32 представлен на рынке другими наиболее распространенными вариациями ESP32 S2 / S2-mini / S3 / S3-mini / С3 / C3-mini и некоторыми другими
// Эти микроконтроллеры отличаются от WROOM-32/WROVER-32 назначением и доступностью пинов, которые не совпадают с платами на базе ESP32 DevKit
// Назначение пинов для перечисленных типов плат бралось отсюда:
// ESP32         рекомендовано  https://github.com/espressif/arduino-esp32/blob/master/variants/esp32/pins_arduino.h            ESP32 Dev Module, ESP32-WROOM-DA Module, ESP32 Wrover Module   https://aliexpress.ru/item/32864722159.html,     https://aliexpress.ru/item/32836372640.html,  
// ESP32-S2                     https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s2/pins_arduino.h          ESP32S2 Dev Module                                             https://aliexpress.ru/item/1005002247116977.html
// ESP32-S2-mini                https://github.com/espressif/arduino-esp32/blob/master/variants/lolin_s2_mini/pins_arduino.h    LOLIN S2 Mini                                                  https://aliexpress.ru/item/1005004691697002.html
// ESP32-S3                     https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s3/pins_arduino.h          ESP32S3 Dev Module                                             https://aliexpress.ru/item/1005005383895711.html
// ESP32-S3-mini                https://github.com/espressif/arduino-esp32/blob/master/variants/lolin_s3_mini/pins_arduino.h    LOLIN S3 Mini                                                  https://aliexpress.ru/item/1005005449219195.html
// ESP32-C3                     https://github.com/espressif/arduino-esp32/blob/master/variants/esp32c3/pins_arduino.h          ESP32С3 Dev Module                                             https://aliexpress.ru/item/1005005653439200.html
// ESP32-C3-mini                https://github.com/espressif/arduino-esp32/blob/master/variants/lolin_c3_mini/pins_arduino.h    LOLIN С3 Mini                                                  https://aliexpress.ru/item/1005006109082351.html
//
// Проверена и рекомендлвано к использованию ESP32-плата из первой строчки таблицы выше
// https://aliexpress.ru/item/32959541446.html
// https://aliexpress.ru/item/32836372640.html
//
// Для остальных плат возможно потребуется перепроверка назначения пинов с выяснением какие комбинации работают, а какие нет.
//
// Рабочие комбинации версий ядра и библиотек:
//
// Версия FastLED      - 3.6.0  // 3.4, 3.5, 3.6 - смотри комментарий ниже в секции FastLED
// Версия ядра ESP8266 - 3.1.2
// Версия ядра ESP32   - 2.0.14 // На версиях 2.x.x (кроме 2.0.14) скетчи может не собираться совсем или собирается, но
//                              // нет вывода на матрицу (до 2.0.6) или цифровые артефакты и подергивание эффектов и бегущей строки.
//
// -------------------------------------------------------------------------------------------------------
//
// Для ядра ESP8266 v3.1.2
//   тип микроконтроллера в меню "Инструменты -> Плата" для ESP8266:
//     - для устройств на базе NodeMCU выбирать       -- "NodeMCU 1.0 (ESP12E Module)"
//     - для устройств на базе Wemos d1 mini выбирать -- "LOLIN(WEMOS) D1 mini (clone)" 
//     - для выделения места под файловую систему в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства
//       для стандарного контроллера с 4МБ флэш-памяти памяти на борту устройства выберите вариант: "Flash Size: 4MB(FS:2MB OTA:~1019KB)"
//
// Для ядра ESP32 v2.0.14
//   тип микроконтроллера в меню "Инструменты -> Плата" 
//     - для большинства контроллеров выбирать "ESP32 Dev Module" 
//     - для разновидностей ESP32S2 выбирать "ESP32S2 Dev Module" или соответствующее плате значение, содержащее 'S2' 
//     - для разновидностей ESP32S3 выбирать "ESP32S3 Dev Module" или соответствующее плате значение, содержащее 'S3' 
//     - для разновидностей ESP32C3 выбирать "ESP32C3 Dev Module" или соответствующее плате значение, содержащее 'C3'
//
//   для выделения места под файловую систему в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства
//       для стандарного контроллера с 4МБ флэш-памяти памяти на борту устройства выберите вариант: "Partition scheme: Default 4MB with spiff(1.2MB APP/1.5MB SPIFFS)";
//       Если включена поддержка всех возможностий и компилятор ругается на недостаток памяти - придется отказаться от
//       возможности обновления "по воздуху" (OTA, Over The Air) и выбрать вариант распределения памяти устройства "Partition scheme: No OTA (2MB APP/2MB SPIFFS)";
//
// // -------------------------------------------------------------------------------------------------------
//
// Настройки ArduinoIDE 
//   для Wemos, NodeMCU и ESP32 смотри на скриншотах в корневой папке проекта - settings-wemos.png, settings-nodemcu.png и settings-esp32.png 
//
//   Внимание!!! 
//     Настройки сборки в меню "Инструменты" для Wemos приведены на указанном скриншоте испробованы и в большинстве случаев работают нормально.           
//     Однако мне встречались экземпляры, которые при данной настройке даже не запускались - в мониторе порта при старте контроллера только немного мусора.
//     Для таких плат выберите значение "Flash Mode: DIO". Если не запустится и с ним - выбирайте значение "Flash Mode: DOUT (compatible)"
//     Если и дальше будут проблемы - пробуйте вариант "Flash frequency" понижать с 80 MHz до 40 MHz
//     Вероятно с какими-то комбинациями этих настроек контроллер запустится
//     
//     Второй момент: большинство микроконтроллеров нормально работают при установки частоты CPU в 160MHz.
//     Если на частоте 160MHz не запускается - попробуйте снизить частоту CPU в меню "Интсрументы", "CPU Frequency" до стандартных 80MHz
//
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// #define FASTLED_ALLOW_INTERRUPTS 0
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//     Третий момент: Прошивка достигла некоторого предела, при котором при всех включенных возможностях скетч на некоторых контроллерах ESP32 не умещается в памяти.
//     Если при компиляции выдвется ошибка - размер скетча превысил размер доступной памяти и составляет (104%) - вам придется
//     - либо выбирать плату с Большим размером доступной памяти - например 8MB вместо стандартных 4MB
//     - либо чем-то жертвовать, отключая поддержку того или иного оборудования и/или возможности - например, обновление по воздуху - ОТА 
//       При отключении возможности обновления по воздуху
//         1. Установите в настройках скетча USE_OTA = 0
//         2. В меню "Инструменты", "Partition Scheme" - выберите "No OTA (2MB App / 2MB SPIFFS)"
//
// -------------------------------------------------------------------------------------------------------
//
// *** FastLED
// 
// Последние эксперименты показали, что наиболее стабильная работа достигается при использовании ядер версии 
// ESP8266 - 3.1.2, ESP32 - 2.0.14, библиотеки FastLED версии 3.6, 
// параметр в строке 12 этого файла #define FASTLED_ALLOW_INTERRUPTS 0 закомментирован, то есть не используется
//
// Впрочем, похоже, поведение зависит от фаз луны и совокупности прочих настроек в меню "Инструменты", относящимся к параметрам компиляции.
// Можете поэкспериментировать с включением / отключением параметра FASTLED_ALLOW_INTERRUPTS, а также различными версиями FastLED - 3.4-3.6
// При одном и том же неизменном коде поведение устройства может меняться, например:
// - нет осмысленного вывода на матрицу, горит только первый светодиод                                                          FastLED 3.4, FASTLED_ALLOW_INTERRUPTS закомментирован
// - вывод на матрицу есть, первый светодиод горит нормально, текст покаывается плавно> периодические падения с сообщением 
//  "Exception 4: Level1Interrupt: Level-1 interrupt as indicated by set level-1 bits in the INTERRUPT register"                FastLED 3.4, FASTLED_ALLOW_INTERRUPTS раскомментирован
// - вывод на матрицу есть, но первый светодиод все равно горит не в такт основному изображению                                 FastLED 3.5
// - вывод на матрицу есть, первый светодиод горит нормально, но текст показывается рывками + иногда полное замирание эффектов
//   и первый светодиод начинает мерцать                                                                                        FastLED 3.5, 3.6, фазы луны
//
// -------------------------------------------------------------------------------------------------------
//
// *** MP3 DFPlayer
//
// Используйте версию библиотеки DFPlayer_Mini_Mp3_by_Makuna из папки libraries проекта - это версия 1.1.1
// Более новые версии этой библиотеки из предлагаемых в Library Manager of Arduino IDE (например, 1.2.0) - в отличии от 1.1.1 
// почему-то не находят файлов звуков на SD-карте - скетч при запуске скажет, что MP3 плеер недоступен.
//
// Будет ли работать MP3 DFPlayer - чистая лотерея, зависит от чипа и фаз луны
// В некоторых случаях может быть циклическая перезагрузка после подключения к сети,
// в некоторых случаях - перезагрузка микроконтроллера при попытке проиграть мелодию будильника из приложения
// Но, может и работать нормально.
//
// Вероятнее всего нестабильность работы плеера зависит от версии библиотеки SoftwareSerial, входящей в состав ядра.
// Используйте только рекомендованные выше проверенные версии ядра и библиотеки из папки libraries проекта.
//
// Другая вероятная причина возможных сбоев плеера - (не)стабильность или недостаточное (или завышенное) 
// напряжение питание платы плеера или недостаток тока, предоставляемого БП для питания плеера.
// По Datasheet напряжение питания DFPlayer - 4.2 вольта (с допустимым заявленным диапазоном 3.3В..5В), однако при использовании напряжения 
// 5 вольт или чуть выше - плеер работает нестабильно (зависит от чипа - как повезет).
//
// Общая рекомендация - питать всю систему от напряжения 4.9 вольта при необходимости используя подпирающий диод между GND блока питания (матрицы) 
// и пином GND микроконтроллера / DFPlayer`a
//
// -------------------------------------------------------------------------------------------------------
//
// *** Дополнительный индикатор TM1637
//
// Библиотеку TM1637 следует обязательно устанавливать из папки проекта. В ней сделаны исправления, позволяющие
// компилироваться проекту для микроконтроллера ESP32. Со стандартной версией библиотеки из менеджера библиотек
// проект не будет компилироваться.
//
// -------------------------------------------------------------------------------------------------------
//
// *** Бегущая строка - скорость движения
//
// В a_hard_def.h найдите параметр TEXT_SHIFT. Это значение - на сколько колонок сдвигать текст за один проход
// Скорость сдвига строки зависит от скорости вывода картинки на матьрицу. На больших матрицах требуется больше времени на вывод одного кадра
// Если вам кажется что на максимальной скорости строка все равно движется медленно - укажите параметр TEXT_SHIFT равным 2 или 3. 
// Тогда каждый сдвиг текста будет выполняться сразу на 2 или три колонки. Текст будет двигаться быстрее, но визуально менее плавно
//
// --------------------------------------------------------
//
// *** Синхронизация устройств по протоколу E1.31
//
// Библиотеку ESPAsyncE131 следует обязательно устанавливать из папки проекта. В ней исправлены ошибки стандартной
// библиотеки (добавлен деструктор и освобождение выделяемых ресурсов), а также добавлен ряд функций, позволяющих
// осуществлять передачу сформированных пакетов в сеть. Со стандартной версией библиотеки из менеджера библиотек
// проект не будет компилироваться. Вещание ведется в Multicast UDP в локальной сети. 
// В настройках роутера Multicast должен быть разрешен, роутер обладать достаточной пропускной способностью.
//
// При недостаточной пропускной способности роутера вывод трансляции может проходить неравномерно, с рывками.
// Некоторые роутеры (например TPLink Archer C80) имеют кривую реализацию протокола маршрутизации multicast
// трафика, отдают полученные пакеты в сеть неравномерно. При большом входящем трафике роутер может "зависать", тогда
// компьютеры в локальной сети будут писать "нет подключения к интернет". Так же на этом роутере наблюдалось полное зависание сети,
// от 1 до 10 минут или до перезагрузки роутера при внезапном отключении одного из приемников multicat трафика.
//
// --------------------------------------------------------
//
// *** Web-интерфейс
//
// Библиотеку ESPAsyncWebServer владельцам IPhone следует обязательно устанавливать из папки проекта. 
// В ней в файле WebResponses.cpp в строчках 538, 569 закомментарено добавление заголовка addHeader("Content-Disposition", buf)
// Точнее перенесено в область if() else - для download=true.
// Почему-то наличие этого заголовка в айфонах не открывает запрошенную страничку в браузере, а предлагает сохранить
// загружаемый файл intex.html.gz на диск как обычный download-файл.
//
// Сама библиотека с хотфиксами взята отсюда: https://github.com/vortigont/ESPAsyncWebServer/tree/hotfixes
// Но в ней нет переноса addHeader("Content-Disposition", buf) что описано выше
//
// Прошивка требует компиляции с выделением места под файловую систему LittleFS, в которой хранятся в файлах некоторые настроки,
// файлы резервного копирования, файл карты индексов адресации светодиодов матрицы, файлы Web-интерфейса, а также файлы картинок, 
// нарисованные пользователем в режиме рисования и используемые в эффекте "Слайды"
//
// Для выделения места под файловую систему в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства выберите вариант:
//   Для микроконтроллеров ESP8266 с 4МБ флэш-памяти рекомендуется вариант "Flash Size: 4MB(FS:2MB OTA:~1019KB)"
//   Для микроконтроллеров ESP32   с 4МБ флэш-памяти рекомендуется вариант "Partition scheme: Default 4MB with spiff(1.2MB APP/1.5MB SPIFFS)"; 
//
// После того, как прошивка будет загружена в микроконтроллер - не забудьте загрузить файлы из подпапки data в файловую систему микроконтроллера
// https://github.com/vvip-68/LedPanelWiFi/wiki/Загрузка-данных-в-файловую-систему-МК
//
// --------------------------------------------------------
//
// *** SD-карта
//
// Некоторые SD-shield требуют напряжения питания 5 вольт, некоторые - 3.3 вольта
// Если на SD-shield подать напряжение, не соответствующее его характеристикам - файлы с SD карты также будут не видны.
// При использовании "матрешки" из Wemos d1 mini и соотвествующего ей Shield SD-card рекомендается распаивать ОБА пина питания - и +5В и +3.3В 
//
// Рекомендуемый к использованию шилд SD-карты: https://aliexpress.ru/item/32578362865.html
// Он удобно устанавливается "матрешкой" на платы микроконтроллеров
//   ESP8266 - Wemos d1 mini       - https://aliexpress.ru/item/32630518881.html
//   ESP32   - Wemos d1 mini esp32 - https://aliexpress.ru/item/32858054775.html
//
// --------------------------------------------------------
//
// *** OTA - обновление "по воздуху" - Over The Air - без подключения к USB
//
// Обычно для обновления по воздуху, требуется, чтобы в меню "Инструменты" - "Flash Size" - была выбрана разметка файловой системы 
// с выделением места под OTA:
//   Для микроконтроллеров ESP8266 с 4МБ флэш-памяти рекомендуется вариант "Flash Size: 4MB(FS:2MB OTA:~1019KB)"
//   Для микроконтроллеров ESP32   с 4МБ флэш-памяти рекомендуется вариант "Partition scheme: Default 4MB with spiff(1.2MB APP/1.5MB SPIFFS)"; 
//
// Однако размер прошивки достиг (пред)критического размера, когда при поддержке всех включенных возможностей раздел приложения занимает 99%
// flash-памяти (смотри сообщения компилятора при провверке скетча). В этом случае прошивка может загрузиться на плату, но при старте не запустится
// на выполнение, а уйдет в вечный бесконечный цикл перезагрузки.
//
// Если такое случилось, для запуски прошивки нужно пойти по одному из следующих путей:
// - Взять плату с бОльшим размером установленной флэш-памяти - 8MB или 16Ьии указать соответствующий размер в меню "Инструменты" - выбрать значения, соответствующие вашей плате
// - Отключить поддержку некоторых возможностей, установив в настройках скетча константы USE_XXXX в 0 - поддержка отключена (USE_TP1637, USE_MP3, USE_SD, USE_ANIMATION, INITIALIZE_TEXTS и др.)
// - Отказаться от обновления по воздуху, выбрав в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства выберите вариант: 
//     Для микроконтроллеров ESP8266  - перейти на микроконтроллер ESP32
//     Для микроконтроллеров ESP32    - с 4МБ флэш-памяти рекомендуется вариант "Partition scheme: No OTA (2MB App/2MB SPIFFS)"; 
//   а также установив в настройках скетча константу USE_OTA в значение 0
//
// -------------------------------------------------------------------------------------------------------
//
// *** Настройка параметров прошивки под вашу конфигурацию оборудования
//
// Для начала работы выполните настройки параметров прошивки - укажите нужные значения в файлах 
//  a_def_hard.h - параметры вашей матрицы, наличие дополнительных модулей, пины подключения, использование возможностей прошивки
//  a_def_soft.h - параметры подулючения к сети - имя сети, пароль, временная зона, коды для получения погоды и т.д.
// Большинство параметров в последствии могут быть изменены в Web-интерфейсе при подключении к устройству или в приложении на смартфоне, 
// если таковое уже есть для текущей версии прошивки.
//
// -------------------------------------------------------------------------------------------------------
// Версии (что нового): 
// -------------------------------------------------------------------------------------------------------
//
// v1.14c - много оптимизации по работе с памятью, изменилась карта память EEPROM - выделено место под новые параметры (потребуется перенастройка параметров)
//          новое: поддержка плат ESP32: S2, S2-mini, S3, S3-mini, C3, C3-mini, 12/24-часовой формат времени, Температура - в Цельсиях или Фаренгейтах
// v1.14b - много оптимизации по работе с памятью, использование системных функций ядра для синхронизации времени серверами NTP. 
// v1.14а - попытка оптимизировать работу с памятью. 
// v1.14  - добавлено управление через Web-интерфейс.
//          Android-приложения для настройки и управления устройством более несовместимы с этой версией и работать с ней не будут.
//          Управление устройством будет осуществляться только через Web-интерфейс.
//          Файлы web-интерфейса находятся в подпапке 'data/web' в папке с файлами скетча прошивки и загружаются в микроконтроллер отдельно
//          после компиляции и загрузки скетча. Загрузка фвйлов из папки 'data' выполняется плагином 'LittleFS DataUpload' для соответствующего 
//          типа микроконтроллера - ESP32 или ESP8266.
//
//           Прошивка стабильно работает на микроконтроллерах ESP32, предпочтительно собирать устройство на нем.
//           Микроконтроллер ESP8266 все еще поддерживается для матриц размером до 500-700 диодов, но устройство может перезагружаться при попытке
//           открытия Web-интерфейса в браузере из за нехватки оперативной памяти у этого типа микроконтроллеров.
//           По той же причине недостатка оперативной памяти Web-интерфейс может вообще не открываться - передача требуемых файлов Web-странички прерывается,
//           если микроконтроллеру недостаточно оперативной памяти. Проблемы начинаются, когда свободной оперативной памяти остается менее 12-15 килобайт.
//           Сколько памяти остается свободной - зависит от размера матрицы и включенных функций - MP3 Player, индикатор TM1637, поддержка E131 и т.д.
//
//          Изменилась карта распределения хранения настроек в постоянной памяти EEPROM
//          После сборки и загрузки скомпилированной прошивки 1.14 вам придется заново перенастраивать все эффекты
//          и прочие настройки программы, в том числе набор текстов бегущей строки. Ввиду несовместимости расположения 
//          сохраненных настроек, их восстановление из файла резервной копии также недоступно или приведет к
//          сбоям в работе прошивки. Первая загрузка прошивки 1.14 поверх более ранней опции должна быть выполнена
//          с настройкой в меню "Инструменты" - "Erase Flash: Erase All"
//
// *******************************************************************************************************
// *                                              WIFI ПАНЕЛЬ                                            *
// *******************************************************************************************************

#include "a_def_hard.h"     // Определение параметров матрицы, пинов подключения и т.п
#include "a_def_soft.h"     // Определение параметров эффектов, переменных программы и т.п.
#include "timeProcessor.h"  // Класс менеджмента времени (NTP, работа с временем)

void setup() {
  #if defined(ESP8266)
    ESP.wdtEnable(WDTO_8S);
  #endif

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
  #endif

  // пинаем генератор случайных чисел
  #if defined(ESP8266) && defined(TRUE_RANDOM)
  uint32_t seed = (int)RANDOM_REG32;
  #else
  uint32_t seed = (int)(analogRead(0) ^ micros());
  #endif
  randomSeed(seed);
  random16_set_seed(seed);

  MAX_EFFECT = CountTokens(EFFECT_LIST, ',');

  initializeEEPROM();

  if (vDEBUG_SERIAL) {
    Serial.begin(115200);
    delay(300);
  }

  bool isWireInitialized = getWiringInitialized();
  if (!(isWireInitialized && isEEPROMInitialized)) {
    initializeWiring();
  }
  loadWiring();

  host_name = HOST_NAME;
  host_name.replace(' ','_');
  
  DEBUGLN();
  DEBUGLN(FIRMWARE_VER);
  DEBUG(F("CRC: "));
  
  DEBUG(F("Контроллер: "));
  DEBUGLN(MCUTypeEx());

  {      
    #if defined(ESP32) && defined(ARDUINO_ESP32_RELEASE)
      String core_type = F("ESP32");
      String core_version(ARDUINO_ESP32_RELEASE);
    #elif defined(ESP8266) && defined(ARDUINO_ESP8266_RELEASE)
      String core_type = F("ESP8266");
      String core_version(ARDUINO_ESP8266_RELEASE);
    #endif
    if (core_version.length() > 0) {
      DEBUG(F("Версия ядра: "));
      DEBUG(core_type);
      core_version.replace("_", ".");
      DEBUG(F(" v"));
      DEBUGLN(core_version);
    }
  }
  
  {
    String fv(FASTLED_VERSION);
    String fv_maj(fv.substring(1,4).toInt());
    String fv_min(fv.substring(4).toInt());
    DEBUG(F("FastLED: ")); DEBUG(fv[0]); DEBUG('.');  DEBUG(fv_maj); DEBUG('.'); DEBUG(fv_min);    
    DEBUGLN();
  }

  if (!isEEPROMInitialized) {
    // Сама инициализация выполняется выше по коду. Тут - просто вывод сообщения о событии? когда уже разрешен вывод в Serial
    DEBUGLN(F("Инициализация EEPROM..."));
  }  
  
  DEBUG(F("Версия EEPROM: 0x"));
  DEBUGLN(IntToHex(eeprom_id, 2));  
  if (eeprom_id != EEPROM_OK) {
    DEBUG(F("Обновлено до: 0x"));
    DEBUGLN(IntToHex(EEPROM_OK, 2));  
  }

  {
    String system_name(getSystemName());
    if (system_name.length() == 0) {
      system_name = host_name;
      system_name.replace('_',' ');
    }
    
    DEBUG(F("Host: '"));
    DEBUG(host_name);  
    DEBUG(F("\'\nИмя : '"));
    DEBUG(system_name);
    DEBUGLN(F("\'\nИнициализация файловой системы... "));
  }
  
  spiffs_ok = LittleFS.begin();
  if (!spiffs_ok) {
    DEBUGLN(F("Выполняется разметка файловой системы... "));
    LittleFS.format();
    spiffs_ok = LittleFS.begin();
  }

  PGM_P sUsed  = PSTR("Использовано "); 
  PGM_P sFrom  = PSTR(" из "); 
  PGM_P sByte  = PSTR(" байт"); 
  
  #if defined(ESP8266)
  PGM_P sError = PSTR("Ошибка получения сведений о файловой системе."); 
  #endif
  
  if (spiffs_ok) {
    #if defined(ESP32)
      spiffs_total_bytes = LittleFS.totalBytes();
      spiffs_used_bytes  = LittleFS.usedBytes();
      DEBUG(sUsed); DEBUG(spiffs_used_bytes); DEBUG(F(sFrom)); DEBUG(spiffs_total_bytes); DEBUGLN(sByte);
    #else
      FSInfo fs_info;
      if (LittleFS.info(fs_info)) {
        spiffs_total_bytes = fs_info.totalBytes;
        spiffs_used_bytes  = fs_info.usedBytes;
        DEBUG(sUsed); DEBUG(spiffs_used_bytes); DEBUG(sFrom); DEBUG(spiffs_total_bytes); DEBUGLN(sByte);
      } else {
        DEBUGLN(sError);
    }
    #endif
  } else {
    DEBUGLN(F("Файловая система недоступна."));
    DEBUGLN(F("Управление через Web-канал недоступно."));
    DEBUGLN(F("Функционал \"Бегущая строка\" недоступен."));
  }
 
  loadSettings();

  FastLED.setBrightness(globalBrightness);
  if (CURRENT_LIMIT > 0) {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  }

  // Настройки ленты
  allocateLeds();         // leds =  new CRGB[NUM_LEDS];          

  DEBUGLN();

  delay(10);
  
  FastLED.clear(true);
  FastLED.show();

  // Настройка кнопки
  #if (USE_BUTTON  == 1)
    int8_t pin_btn = getButtonPin();
    if (pin_btn >= 0) {
      if (vBUTTON_TYPE == 0)
        butt = new GButton(pin_btn, LOW_PULL, NORM_OPEN);    // Для сенсорной кнопки
      else
        butt = new GButton(pin_btn, HIGH_PULL, NORM_OPEN);   // Для обычной кнопки
    }
    if (butt != nullptr) {
      butt->setStepTimeout(100);
      butt->setClickTimeout(250);
      butt->setDebounce(50);
      butt->tick();
      butt->resetStates();
    }
  #endif

  // -----------------------------------------  
  // Вывод основных возможностей: поддержка в прошивке - 
  // включена или выключена + некоторые параметры
  // -----------------------------------------  

  PGM_P sDirection = PSTR("  Направление: ");
  PGM_P sType      = PSTR("  Тип: ");
  PGM_P sLeftDown  = PSTR("левый нижний"); 
  PGM_P sLeftUp    = PSTR("левый верхний"); 
  PGM_P sRightDown = PSTR("правый нижний"); 
  PGM_P sRightUp   = PSTR("правый верхний"); 
  PGM_P sToRight   = PSTR("вправо"); 
  PGM_P sToLeft    = PSTR("влево"); 
  PGM_P sToUp      = PSTR("вверх"); 
  PGM_P sToDown    = PSTR("вниз"); 
  PGM_P sZigzag    = PSTR("зигзаг"); 
  PGM_P sParallel  = PSTR("параллельная"); 
  
  DEBUG(F("Матрица: "));
  if (vDEVICE_TYPE == 0) DEBUG(F("труба "));
  if (vDEVICE_TYPE == 1) DEBUG(F("плоская "));
  DEBUG(pWIDTH); DEBUG('x'); DEBUGLN(pHEIGHT);

  if (sMATRIX_TYPE == 2) {
    DEBUGLN(F("Aдресация: карта индексов"));
  } else {
    DEBUGLN(F("Адресация: по подключению"));
    DEBUG(F("  Угол: ")); 
    if (sCONNECTION_ANGLE == 0) { DEBUGLN(sLeftDown); } else
    if (sCONNECTION_ANGLE == 1) { DEBUGLN(sLeftUp); } else
    if (sCONNECTION_ANGLE == 2) { DEBUGLN(sRightUp); } else
    if (sCONNECTION_ANGLE == 3) { DEBUGLN(sRightDown); }
    DEBUG(sDirection);
    if (sSTRIP_DIRECTION == 0) { DEBUGLN(sToRight); } else
    if (sSTRIP_DIRECTION == 1) { DEBUGLN(sToUp); } else
    if (sSTRIP_DIRECTION == 2) { DEBUGLN(sToLeft); } else
    if (sSTRIP_DIRECTION == 3) { DEBUGLN(sToDown); } 
    DEBUG(sType);
    if (sMATRIX_TYPE == 0) { DEBUGLN(sZigzag); } else
    if (sMATRIX_TYPE == 1) { DEBUGLN(sParallel); }
    
    if (mWIDTH > 1 || mHEIGHT > 1) {
      DEBUG(F("  Размер сегмента: "));
      DEBUG(WIDTH); DEBUG('x'); DEBUGLN(HEIGHT);
      DEBUG(F("Cегменты: "));
      DEBUG(mWIDTH); DEBUG('x'); DEBUGLN(mHEIGHT);
      DEBUG(F("  Угол: ")); 
      if (mANGLE == 0) { DEBUGLN(sLeftDown); } else
      if (mANGLE == 1) { DEBUGLN(sLeftUp); } else
      if (mANGLE == 2) { DEBUGLN(sRightUp); } else
      if (mANGLE == 3) { DEBUGLN(sRightDown); }
      DEBUG(sDirection);
      if (mDIRECTION == 0) { DEBUGLN(sToRight); } else
      if (mDIRECTION == 1) { DEBUGLN(sToUp); } else
      if (mDIRECTION == 2) { DEBUGLN(sToLeft); } else
      if (mDIRECTION == 3) { DEBUGLN(sToDown); }
      DEBUG(sType);
      if (mTYPE == 0) { DEBUGLN(sZigzag); } else
      if (mTYPE == 1) { DEBUGLN(sParallel); }
    }
  }
  
  // ---------- Пины подключения ленты --------------
  
  bool hasLedOut = getLedLineUsage(1) || getLedLineUsage(2) || getLedLineUsage(3) || getLedLineUsage(4);
  if (hasLedOut) {
    DEBUGLN(F("Вывод на ленту:"));  
    for (uint8_t i = 1; i <= 4; i++) {
      bool isLineUsed = getLedLineUsage(i);
      if (isLineUsed) {
        int8_t    led_pin = getLedLinePin(i);
        uint16_t  led_start = getLedLineStartIndex(i);
        uint16_t  led_count = getLedLineLength(i);
        if (led_start + led_count > NUM_LEDS) {
          led_count = NUM_LEDS - led_start;
        }      
        int8_t    led_rgb = getLedLineRGB(i);
        DEBUG(F("  Линия ")); DEBUG(i); DEBUG(F(" PIN=")); DEBUG(pinName(led_pin)); DEBUG(F(", START=")); DEBUG(led_start); DEBUG(F(", COUNT=")); DEBUG(led_count); DEBUG(F(", COLORS=")); DEBUGLN(getColorOrderName(led_rgb));
      }
    }
  } else {
    DEBUGLN(F("Вывод на ленту: нет назначенных пинов вывода"));  
  }
  
  // ------------------------------------------------

  DEBUGLN();
  DEBUGLN(F("Доступные возможности:"));
  
  DEBUG(F("+ Язык интерфейса: "));
  DEBUGLN(UI);
  
  DEBUG(F("+ Бегущая строка: шрифт "));
  if (BIG_FONT == 0)
    DEBUGLN(F("5x8"));
  if (BIG_FONT == 2)
    DEBUGLN(F("8x13"));
  if (BIG_FONT == 1)
    DEBUGLN(F("10x16"));

  #if (USE_BUTTON == 1)
    DEBUG(F("+ Кнопка управления: "));
    if (vBUTTON_TYPE == 0) DEBUG(F("сенсорная PIN="));
    if (vBUTTON_TYPE == 1) DEBUG(F("тактовая PIN="));
    DEBUGLN(pinName(getButtonPin()));
  #else
    DEBUGLN(F("- Кнопка управления"));
  #endif

  DEBUGLN(F("+ Синхронизация времени с сервером NTP"));

  DEBUG((USE_POWER == 1 ? '+' : '-'));
  DEBUG(F(" Управление питанием матрицы"));
  #if (USE_POWER == 1)
    DEBUG(F(" PIN=")); DEBUG(pinName(getPowerPin()));
    if (isTurnedOff) { DEBUGLN(F("; ВЫКЛ")); } else { DEBUGLN(F("; ВКЛ")); }
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_ALARM == 1 ? '+' : '-'));
  DEBUG(F(" Управление питанием (линия будильника)"));
  #if (USE_ALARM == 1)
    DEBUG(F(" PIN=")); DEBUGLN(pinName(getAlarmPin()));
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_AUX == 1 ? '+' : '-'));
  DEBUG(F(" Управление питанием (дополнительная линия)"));
  #if (USE_AUX == 1)
    DEBUG(F(" PIN=")); DEBUG(pinName(getAuxPin()));
    if (isAuxActive) { DEBUGLN(F("; ВКЛ")); } else { DEBUGLN(F("; ВЫКЛ")); }
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_WEATHER == 1 ? '+' : '-'));
  DEBUGLN(F(" Получение информации о погоде"));

  if (spiffs_ok) {
    DEBUGLN(F("+ Управление через Web-канал"));
  }

  DEBUG((USE_E131 == 1 ? '+' : '-'));
  DEBUGLN(F(" Групповая синхронизация по протоколу E1.31"));

  DEBUG((USE_TM1637 == 1 ? '+' : '-'));
  DEBUG(F(" Дополнительный индикатор TM1637"));
  #if (USE_TM1637 == 1)
    DEBUG(F(" DIO=")); DEBUG(pinName(getTM1637DIOPin())); DEBUG(F(" CLK=")); DEBUGLN(pinName(getTM1637CLKPin()));
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_SD == 1 ? '+' : '-'));
  DEBUG(F(" Эффекты Jinx! с SD-карты"));
  DEBUG(USE_SD == 1 && FS_AS_SD == 1 ? String(F(" (эмуляция в FS)")) : "");
  #if (USE_SD == 1 && FS_AS_SD == 0)
    // CS (chip select) может быть изменен и задается в настройках
    // CLK, MISO, MOSI - аппаратные пины, фиксированы и SD-карта может быть подключена только к ним
    DEBUG(F(" CS=")); DEBUG(pinName(SD_CS_PIN));
    #if defined(ESP8266)
      DEBUGLN(F(", CLK=D5, MISO=D6, MOSI=D7"));
    #else
      DEBUG(F(", CLK=G"));
      DEBUG(SCK);
      DEBUG(F(", MISO=G"));
      DEBUG(MISO);
      DEBUG(F(", MOSI=G"));
      DEBUGLN(MOSI);
    #endif
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_MP3 == 1 ? '+' : '-'));
  DEBUG(F(" Поддержка MP3 Player"));
  #if (USE_MP3 == 1)
    DEBUG(F(" STX=")); DEBUG(pinName(getDFPlayerSTXPin())); DEBUG(F(" SRX=")); DEBUGLN(pinName(getDFPlayerSRXPin()));
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_OTA == 1 ? '+' : '-'));
  DEBUGLN(F(" Поддержка OTA"));

  DEBUG((USE_TEXT_CACHE == 1 ? '+' : '-'));
  DEBUG(F(" Кэш текстов бегущей строки: "));
  if (USE_TEXT_CACHE == 1) {
    DEBUG(TEXT_CACHE_LINE_SIZE); DEBUGLN(F(" символов на строку"));
  } else {
    DEBUGLN(F(" отключен"));
  }

  DEBUGLN();

  // Инициализация SD-карты
  #if (USE_SD == 1)
    InitializeSD();
  #endif

  // Проверить наличие резервной копии настроек EEPROM в файловой системе MK и/или на SD-карте
  eeprom_backup = checkEepromBackup();
  if (eeprom_backup > 0) {
    // Имя файла соответствует версии EEPROM
    String fileName(F("eeprom_0x")); fileName += IntToHex(EEPROM_OK, 2); fileName += ".hex";  
    // Найдено на SD?
    if ((eeprom_backup & 0x02) > 0) {
      String fullName(SD_BACK_STORAGE);
      if (!fullName.endsWith("/")) fullName += '/'; 
      fullName += fileName;
      DEBUG(F("Найдены сохраненные настройки: SD:/"));
      DEBUGLN(fullName);
    }
    // Найдено в FS?
    if ((eeprom_backup & 0x01) > 0) {
      String fullName(FS_BACK_STORAGE);
      if (!fullName.endsWith("/")) fullName += '/'; 
      fullName += fileName;
      DEBUG(F("Найдены сохраненные настройки: FS:/"));
      DEBUGLN(fullName);
    }
    DEBUGLN();
  }

  if (sMATRIX_TYPE == 2) {
    pWIDTH = mapWIDTH;
    pHEIGHT = mapHEIGHT;
  } else {
    pWIDTH = sWIDTH * mWIDTH;
    pHEIGHT = sHEIGHT * mHEIGHT;
  }

  NUM_LEDS = pWIDTH * pHEIGHT;
  maxDim   = max(pWIDTH, pHEIGHT);
  minDim   = min(pWIDTH, pHEIGHT);

  // Если Wiring еще не был инициализирован при старте программы - он инициализируется выше, в начале скетча
  // Но на тот момент неизвестно общее количество светодиодов в матрице - оно становится известным после выполнения loadSettings()
  // При инициализации включается активной только линия 1 и все светодиоды назначаются на нее
  if (!isWireInitialized) {
    putLedLineLength(1, NUM_LEDS);
  }

  // -----------------------------------------
  // В этом блоке можно принудительно устанавливать параметры, которые должны быть установлены при старте микроконтроллера
  // -----------------------------------------
  
  // -----------------------------------------  

  #if (USE_BUTTON  == 1)
    if (butt != nullptr) butt->tick();
  #endif

  // Настраиваем снхронизацию времени
  TimeProcessor::getInstance().setcustomntp(getNtpServer().c_str());
  applyTimeZone(TZONE);
  // Назначаем коллбэк на событие синхронизации времени
  TimeProcessor::getInstance().attach_callback(ntpGotTimeCB);
  useNtp ? TimeProcessor::getInstance().enable() : TimeProcessor::getInstance().disable();
  DEBUGLN();
  
  // Создать массив для карты индексов адресации светодиодов в ленте
  bool ok = loadIndexMap();
  if (sMATRIX_TYPE == 2 && (!ok || mapListLen == 0)) {
    sMATRIX_TYPE = 0;
    putMatrixSegmentType(sMATRIX_TYPE);
  }
  
  #if (USE_POWER == 1)
    if (vPOWER_PIN >= 0) {
      pinMode(vPOWER_PIN, OUTPUT);
    }
  #endif

  #if (USE_ALARM == 1)
    if (vALARM_PIN >= 0) {
      pinMode(vALARM_PIN, OUTPUT);
    }
  #endif

  #if (USE_AUX == 1)
    if (vAUX_PIN >= 0) {
      pinMode(vAUX_PIN, OUTPUT);
    }
  #endif

  // Распечатать список активных эффектов
  printEffectUsage();

  #if (USE_ANIMATION == 1)
    // Поиск доступных анимаций
    initAnimations();
    // Поиск картинок, пригодных для эффекта "Слайды"
    initializePictures();
  #endif
   

  #if (USE_MP3 == 1)
    // Первый этап инициализации плеера - подключение и основные настройки
    InitializeDfPlayer1();
    {
      String message(F("MP3 плеер недоступен."));
      if (isDfPlayerOk) {
        InitializeDfPlayer2();
        if (!isDfPlayerOk) {
          DEBUGLN(message);
        }
      } else {
          DEBUGLN(message);
      }
    }
  #endif

  // Сформировать список файлов эффектов на SD-карте
  #if (USE_SD == 1)
    if (isSdCardExist) {
      DEBUGLN(F("Поиск файлов эффектов Jinx!.."));    
      if (sd_card_ok) {
        loadDirectory();
        sd_card_ok = countFiles > 0;
        set_isSdCardReady(sd_card_ok);
      }    
      if (!sd_card_ok) {
        DEBUGLN(F("Эффекты Jinx! на SD-карте не обнаружены"));
      }
    }
  #endif

  // Подготовить тексты бегущей строки
  InitializeTexts();  

  // Проверить и распечатать список файлов для Web-управления
  web_ok = checkWebDirectory();
        
  // В процессе подготовки файлов списков размер свободного места в файловой системе мог значительно уменьшиться
  if (spiffs_ok) {
    DEBUGLN(DELIM_LINE);
    DEBUGLN(F("Файловая система: "));
    #if defined(ESP32)
      spiffs_total_bytes = LittleFS.totalBytes();
      spiffs_used_bytes  = LittleFS.usedBytes();
      DEBUG(sUsed); DEBUG(spiffs_used_bytes); DEBUG(F(sFrom)); DEBUG(spiffs_total_bytes); DEBUGLN(sByte);
    #else
      FSInfo fs_info;
      if (LittleFS.info(fs_info)) {
        spiffs_total_bytes = fs_info.totalBytes;
        spiffs_used_bytes  = fs_info.usedBytes;
        DEBUG(sUsed); DEBUG(spiffs_used_bytes); DEBUG(sFrom); DEBUG(spiffs_total_bytes); DEBUGLN(sByte);
      } else {
        DEBUGLN(sError);
    }
    #endif
    DEBUGLN(DELIM_LINE);
  }

  PGM_P sMemFree  = PSTR("Свободно памяти: "); 
  DEBUG(sMemFree);
  printMemoryInfo();
  DEBUGLN(DELIM_LINE);

  // Подключение к сети
  connectToNetwork();

  // Обработчики событий web-сервера
  server.serveStatic("/", LittleFS, BASE_WEB)
    .setDefaultFile("index.html")
    .setCacheControl("public, max-age=3600, must-revalidate");  // 1 hr for caching, then revalidate based on etag/IMS headers

  server.onNotFound(handleNotFound);

  // preflight cors check
  server.on("/", HTTP_OPTIONS, [](AsyncWebServerRequest * request)
  {
      AsyncWebServerResponse* response = request->beginResponse(204);
      response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
      response->addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, FileSize");
      response->addHeader("Access-Control-Allow-Credentials", "false");
      request->send(response);
  });

  server.on("/", HTTP_POST, 
    []([[maybe_unused]] AsyncWebServerRequest * request){ },
    [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) 
    {
      static unsigned long startTimer;
      static File file;
      static bool error;
      
      if (!index) {
        error = false;
        startTimer = millis();

        if (file) file.close();
        String fileName('/'); fileName += filename;
        if (LittleFS.exists(fileName)) LittleFS.remove(fileName);
        file = LittleFS.open(fileName, "w");
        if (file) {
          DEBUGLOG(printf_P, PSTR("Создан файл: '%s'\n"), filename.c_str());
        } else {
          DEBUGLOG(printf_P, PSTR("Ошибка загрузки файла: '%s'\n"), filename.c_str());
          error = true;
        }        
      }

      if (!error) {
        DEBUGLOG(printf_P, PSTR("файл: '%s' получено %i байт\tвсего: %i\n"), filename.c_str(), len, index + len);
        if (file) {
          size_t wlen = file.write(data, len);
          error = wlen != len;
          if (error) {
            request->send(400, MIMETYPE_HTML, "File save error!");
            request->client()->close();
            DEBUGLOG(printf_P, PSTR("файл: '%s'. Ошибка записи.\n"), filename.c_str());
            file.close();
          }
        }
  
        if (final) {
          if (!error && file) {
            file.close();
            // Загруженное имя файла передается от сервера. Стандартное имя 'eeprom_0xXX.hex' могло быть переименовано, 
            // чтобы было понятно от какого устройства бэкап. Например dev_lamp.hex. Под этим же именем оно и загрузилось в корень FS
            String srcFile('/'); srcFile += filename;
            // Реальное имя файла сохраненных настроек должно быть 'eeprom_0xXX.hex', где XX - значение константы версия EEPROM_OK
            // и файл с настройками должен быть размещен в папке '/web/assets'.
            String dstFile(FS_BACK_STORAGE); 
            if (!dstFile.endsWith("/")) dstFile += '/'; 
            dstFile += F("eeprom_0x"); dstFile += IntToHex(EEPROM_OK, 2); dstFile += ".hex";  
            // Если файл со старыми настройками есть в папке назначения - удалить его
            if (LittleFS.exists(dstFile)) LittleFS.remove(dstFile);
            // Теперь переносим только что загруженный файл в место постоянного размещения и с новым имененм
            error = !LittleFS.rename(srcFile, dstFile);  
          }
          if (error) {
            // Если была ошибка - сообщаем о неудачном завершении операци
            request->send(400, MIMETYPE_HTML, "File save error!");
            request->client()->close();
            DEBUGLOG(printf_P, PSTR("файл: '%s'. Ошибка записи.\n"), filename.c_str());
          } else {
            // Если всё прошло ок - сообщаем об успешном завершении операции
            request->send(200, MIMETYPE_HTML, "File upload completed!");
            request->client()->close();
            DEBUGLOG(printf_P, PSTR("Загрузка выполнена: %i байт за %.2f сек ( %.2f КБ/сек ).\n"), index + len, (millis() - startTimer) / 1000.0, 1.0 * (index + len) / (millis() - startTimer));
            eeprom_backup = checkEepromBackup();
            addKeyToChanged("EE");
          }
        }
      }
    });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  
  server.begin();
  
  if (spiffs_ok) {
    DEBUGLN(F("HTTP server started"));  
  }

  #if (USE_E131 == 1)
    InitializeE131();
  #endif

  #if (USE_OTA == 1)
    ArduinoOTA.setHostname(host_name.c_str());
   
    // Заливка скетча по сетевому порту в ArduinoIDE 2.2.1 запрашивает пароль и пустой пароль не принимает.
    // На текущий момент для Arduino IDE 2.x плагинов для загрузки файловой системы на ESP32 - не существует (на ESP8266 есть)
    // Заливка скетча по сетевому порту в ArduinoIDE 1.8.19 пароль не обязателен - можно закомментировать. Если указан - спросит
    // Однако при наличии установленного пароля не выполняется загрузка файловой системы - пароль не спрашивает, просто говорит что ошибка аутентификации.
    // Без установленного пароля загрузка файловой системы плагинами 'ESP32 LittleFS Data Upload' / 'ESP8266 LittleFS Data Upload' загружается успешно
    // ArduinoOTA.setPassword((const char *)"12341234");   
   
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = F("скетча...");
      else // U_SPIFFS
        type = F("файловой системы SPIFFS...");
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      DEBUG(F("Начато обновление "));    
      DEBUGLN(type);    
    });
  
    ArduinoOTA.onEnd([]() {
      DEBUGLN(F("\nОбновление завершено"));
    });
  
    ArduinoOTA.onProgress([](uint32_t progress, uint32_t total) {
      if (vDEBUG_SERIAL) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      }
    });
  
    ArduinoOTA.onError([](ota_error_t error) {
      DEBUG(F("Ошибка: "));
      DEBUGLN(error);
      if      (error == OTA_AUTH_ERROR)    { DEBUGLN(F("Неверное имя/пароль сети")); }
      else if (error == OTA_BEGIN_ERROR)   { DEBUGLN(F("Не удалось запустить обновление")); }
      else if (error == OTA_CONNECT_ERROR) { DEBUGLN(F("Не удалось установить соединение")); }
      else if (error == OTA_RECEIVE_ERROR) { DEBUGLN(F("Не удалось получить данные")); }
      else if (error == OTA_END_ERROR)     { DEBUGLN(F("Ошибка завершения сессии")); }
    });
  
    ArduinoOTA.begin();
  #endif
  
  // Открываем Web-сокет управления через web-интерфейс
  initWebSocket();

  // Настройка внешнего дисплея TM1637
  #if (USE_TM1637 == 1)
    int8_t clk_pin = getTM1637CLKPin(); 
    int8_t dio_pin = getTM1637DIOPin();
    if (clk_pin >= 0 && dio_pin >= 0) {
      display = new TM1637Display(clk_pin, dio_pin, 75);  // 75 - 75 мкс - задержка импульсов при выводе. Default - 100; если на дисплее не отображается или неверные символы - увеличивайте до 100
      lastDisplay[0] = _empty;
      lastDisplay[1] = _empty;
      lastDisplay[2] = _empty;
      lastDisplay[3] = _empty;
      lastDisplayBrightness = 7;
      lastDotState = false;
      display->setBrightness(7);
      display->point(false);
      display->displayByte(_empty, _empty, _empty, _empty);
    }
  #endif

  DEBUG(sMemFree);
  printMemoryInfo();

  #if (USE_WEATHER == 1)     
  // Таймер получения погоды
  weatherTimer.setInterval(1000 * 60 * SYNC_WEATHER_PERIOD);
  #endif

  // Таймер рассвета
  dawnTimer.stopTimer();
  
  // Проверить соответствие позиции вывода часов размерам матрицы
  // При необходимости параметры отображения часов корректируются в соответствии с текущими аппаратными возможностями
  // Начальная позиция отображения центра часов - по центру доступной области матрицы 
  checkClockOrigin();
  CLOCK_XC = pWIDTH / 2 + 1;
  
  // Если был задан спец.режим во время предыдущего сеанса работы матрицы - включить его
  // Номер спец-режима запоминается при его включении и сбрасывается при включении обычного режима или игры
  // Это позволяет в случае внезапной перезагрузки матрицы (например по wdt), когда был включен спец-режим (например ночные часы или выкл. лампы)
  // снова включить его, а не отображать случайный обычный после включения матрицы
  int8_t spc_mode = getCurrentSpecMode();
  int8_t ord_mode = getCurrentManualMode();
 
  if (spc_mode >= 0 && spc_mode < MAX_SPEC_EFFECT) {
    setSpecialMode(spc_mode);
    set_isTurnedOff(spc_mode == 0);
    set_isNightClock(spc_mode == 3);
  } else {
    set_thisMode(ord_mode);
    if (thisMode < 0 || thisMode == MC_TEXT || thisMode >= SPECIAL_EFFECTS_START) {
      setRandomMode();
    } else {
      setEffect(thisMode);        
    }
  }

  autoplayTimer = millis();
  
  setIdleTimer();
}

void loop() {

  // В случаях, когда требуется программный перезапуск контроллера - выполняется очистка вызовом FastLED.clear(true);
  // Однако, если сразу после этого вызвать ESP.restart() - FastLED не успевает отправить на матрицу сигнал очистки матрицы
  // Поэтому там вызывается очистка матрицы и устанавливается флаг необходимости перезапуска, засекается время
  // Только спустя 1000 мс - когда матрица уже должна очиститься - вызывается перезапуск микроконтроллера
  if (needRestart) {
    if (abs((long long)(millis() - needRestartTime)) > 1000) {
      ESP.restart();
    }
    // Перед перезагрузкой - очистить матрицу
    FastLED.clear();
    FastLED.show();
    delay(100);
    return;
  }

  #if (USE_BUTTON  == 1)
    if (butt != nullptr) {
      butt->tick();
    }
  #endif

  #if (USE_OTA == 1)
    if (wifi_connected) {
      ArduinoOTA.handle();
    }
  #endif

  ws.cleanupClients();

  process();
}

// -----------------------------------------

void startWiFi(uint32_t waitTime) {
  
  WiFi.disconnect(true);
  set_wifi_connected(false);
  
  delay(10);               // Иначе получаем Core 1 panic'ed (Cache disabled but cached memory region accessed)
  WiFi.setHostname(host_name.c_str());
  WiFi.mode(WIFI_STA);
 
  // Пытаемся соединиться с роутером в сети
  String ssid(getSsid());
  String pass(getPass());
  if (ssid.length() > 0 && pass.length() > 0) {
    DEBUG(F("\nПодключение к "));
    DEBUG(ssid);

    if (IP_STA[0] + IP_STA[1] + IP_STA[2] + IP_STA[3] > 0) {      
      WiFi.config(IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], IP_STA[3]),  // 192.168.0.106
                  IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], GTW),        // 192.168.0.1
                  IPAddress(255, 255, 255, 0),                            // Mask
                  IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], GTW),        // DNS1 192.168.0.1
                  IPAddress(8, 8, 8, 8));                                 // DNS2 8.8.8.8                  
      DEBUG(F(" -> "));
      DEBUG(IP_STA[0]);
      DEBUG('.');
      DEBUG(IP_STA[1]);
      DEBUG('.');
      DEBUG(IP_STA[2]);
      DEBUG('.');
      DEBUG(IP_STA[3]);                  
    } else {
      WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
      DEBUG(F(" -> DCHP "));
    }

    #if (USE_BUTTON == 1)
      DEBUGLN(F("\nНажмите на кнопку для прерывания подключения\nи переход в режим точки доступа"));
    #else
      DEBUGLN(F("\nЕсли подключение не будет установлено в течение 3 минут -\nбудет создана точка доступа"));
    #endif

    FastLED.clear();
    int8_t width = constrain(pWIDTH / 3, 4, 9);
    int8_t offset_x = (pWIDTH - width) / 2;
    int8_t y = pHEIGHT / 2;
    int8_t cnt2 = 0, dir = 1;
    
    WiFi.begin(ssid.c_str(), pass.c_str());
  
    // Проверка соединения (таймаут 180 секунд, прерывается при необходимости нажатием кнопки)
    // Такой таймаут нужен в случае, когда отключают электричество, при последующем включении устройство стартует быстрее
    // чем роутер успеет загрузиться и создать сеть. При коротком таймауте устройство не найдет сеть и создаст точку доступа,
    // не сможет получить время, погоду и т.д.
    bool     stop_waiting = false;
    uint32_t start_wifi_check = millis();
    uint32_t last_wifi_check = 0;
    int16_t  cnt = 0;
    uint8_t  cnt1 = 0;
    
    while (!(stop_waiting || wifi_connected)) {
      yield();
      
      #if (USE_BUTTON  == 1)
        if (butt != nullptr) butt->tick();
      #endif      
      
      if (millis() - last_wifi_check > 100) {
        last_wifi_check = millis();
        set_wifi_connected(WiFi.status() == WL_CONNECTED); 
        if (wifi_connected) {
          // Подключение установлено
          DEBUGLN();
          DEBUG(F("WiFi подключен. IP адрес: "));
          DEBUGLN(WiFi.localIP());
          if (mdns.begin(host_name)) {
            DEBUGLN(F("MDNS - запущен"));
          } else {
            DEBUGLN(F("MDNS - ошибка запуска"));
          }
          break;
        }
        
        if (cnt1++ > 3) {
          cnt1 = 0; cnt++;
          if (cnt > 0 && (cnt % 50 == 0)) {
            DEBUGLN();
          }
          DEBUG('.');          
        }

        fadeToBlackBy(leds, NUM_LEDS, 35);
        drawPixelXY(offset_x + cnt2, y, CRGB::Green);
        if (dir > 0) {
          if (cnt2++ >= width) dir = -1;
        } else {
          if (cnt2-- < 0) dir = 1;
        }
        FastLED.show();
      }
      
      if (millis() - start_wifi_check > waitTime) {
        // Время ожидания подключения к сети вышло
        break;
      }
      
      // Опрос состояния кнопки
      #if (USE_BUTTON  == 1)
        yield();
        if (butt != nullptr && butt->hasClicks()) {
          if (butt->hasClicks()) {
            DEBUGLN();
            DEBUGLN(F("\nНажата кнопка. Ожидание подключения к сети WiFi прервано."));  
            stop_waiting = true;            
            break;
          }
        }
      #endif
    }
    DEBUGLN();

    if (!wifi_connected && !stop_waiting) {
      DEBUGLN(F("Не удалось подключиться к сети WiFi."));
    }
  }  
}

void startSoftAP() {
  WiFi.softAPdisconnect(true);
  ap_connected = false;

  String apName(getSoftAPName());
  String apPass(getSoftAPPass());

  DEBUG(F("Создание точки доступа "));
  DEBUGLN(apName);

  ap_connected = WiFi.softAP(apName.c_str(), apPass.c_str());

  for (uint8_t j = 0; j < 10; j++ ) {    
    yield();
    if (ap_connected) {
      DEBUGLN();
      DEBUG(F("Точка доступа создана. Сеть: '"));
      DEBUG(apName);
      // Если пароль совпадает с паролем по умолчанию - печатать для информации,
      // если был изменен пользователем - не печатать
      if (strcmp(apPass.c_str(), "12341234") == 0) {
        DEBUG(F("'. Пароль: '"));
        DEBUG(apPass);
      }
      DEBUGLN(F("'."));
      DEBUG(F("IP адрес: "));
      DEBUGLN(WiFi.softAPIP());
      break;
    }    
    
    WiFi.enableAP(false);
    WiFi.softAPdisconnect(true);
    delay(500);
    
    DEBUG('.');
    ap_connected = WiFi.softAP(apName.c_str(), apPass.c_str());
  }  
  DEBUGLN();  

  if (!ap_connected) { 
    DEBUGLN(F("Не удалось создать WiFi точку доступа."));
  }
}

void connectToNetwork() {
  // Подключиться к WiFi сети, ожидать подключения 180 сек пока, например, после отключения электричества роутер загрузится и поднимет сеть
  startWiFi(180000);

  // Если режим точки доступа не используется и к WiFi сети подключиться не удалось - создать точку доступа
  if (!wifi_connected){
    WiFi.mode(WIFI_AP);
    startSoftAP();
  }

  if (useSoftAP && !ap_connected) startSoftAP();    
}


/**
 * @brief NTP time adjustment callback
 * функция выызывается при синхронизации системного времени по NTP
 * 
 */
void ntpGotTimeCB(){

  // Флаг - время получено
  init_time = useNtp;

  // Пересчитать / скорректировать время срабатывания будильников
  calculateDawnTime();      

  // Пересчетать очередь ближайших событий {P} в бегущей строке - выставить флаг о необходимости пересчета
  needRescanText = true;    
}
