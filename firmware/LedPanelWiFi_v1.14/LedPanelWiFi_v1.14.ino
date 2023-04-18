// Гайд по постройке матрицы: https://alexgyver.ru/matrix_guide/
// Страница проекта на GitHub: https://github.com/vvip-68/LedPanelWiFi
// Автор идеи, начальный проект - GyverMatrixBT: AlexGyver Technologies, 2019 (https://alexgyver.ru/gyvermatrixbt/)
// Дальнейшее развитие: vvip-68, 2019-2023
//
// Дополнительные ссылки для Менеджера плат ESP8266 и ESP32 в Файл -> Настройки
// https://raw.githubusercontent.com/esp8266/esp8266.github.io/master/stable/package_esp8266com_index.json
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#define FIRMWARE_VER F("WiFiPanel v.1.14.2023.0418")

// --------------------------   -----------------------------------------------------------------------------
//
// Внимание !!!
//
// Тип микроконтроллера в меню "Инструменты -> Плата" для ESP8266 выбирать "NodeMCU 1.0 (ESP12E Module)" даже
// с случае использования микроконтроллера семейства Wemos d1 mini. При выборе другого типа микроконтроллера
// скорее всего пин вывода на ленту переназначится компилятором на другой пин, отличный от D2, вероятнее всего - на D4.
// Пин D4 используется данной прошивкой (в стандартном варианте) для подключения кнопки. Такое совпадение пинов
// приведет к невозможности регулировки яркости кнопкой, а сама яркость будет автоматически плавно за несколько секунд
// уменьшаться до нуля, и вместо изображения эффектов вы увидите либо черную матрицу, либо тусклые "ночные" часы.
//
// Внимание!!!
// Рабочие комбинации версий ядра и библиотек:
//
// Версия ядра ESP8266 - 2.7.4    
// Версия ядра ESP32   - 2.0.6 (или 2.0.7, но 2.0.6 - стабильнее)
// Версия FastLED      - 3.5.0 (или 3.4.0)
//
// -------------------------------------------------------------------------------------------------------
//
// Для микроконтроллера на базе ESP8266 пин вывода на ленту LED_PIN обозначен как 2 (GPIO2) или D4
// Тем не менее при указанной выше комбинации - NodeMCU v1.0 + Ядро 2.7.4 + библиотека FastLed 3.5 - назначает вывод на пин D2 микроконтроллера
// Если после запуска прошивке на микроконтроллеря светится (быстро мигает) встроенный синий светодиод - значит вы что-то сделали не так и вывод
// сигнала на ленту назначился на физический пин D4
//
// С остальными комбинациями версии ядра ESP8266 и выбранных плат скорее всего стабильно работать не будет:
//  -> Постоянно горит самый первый светодиод в цепочке синим или зелёным
//  -> некоторые эффекты сильно мерцают или замирают совсем или наблюдается подергивание эффектов (нет плавности воспроизведения).
//  -> пин ленты с D2 переназначается ядром на D4
//
// -------------------------------------------------------------------------------------------------------
//
// Если используется плеер MP3 DFPlayer - будет ли он работать - чистая лотерея, зависит от чипа и фаз луны
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
// Общая рекомендация - питать всю систему от напряжения 4.8 вольта при необходимости используя подпирающий диод между GND блока питания (матрицы) 
// и пином GND микроконтроллера (DFPlayer`a)
//
// -------------------------------------------------------------------------------------------------------
//
// Библиотеку TM1637 следует обязательно устанавливать из папки проекта. В ней сделаны исправления, позволяющие
// компилироваться проекту для микроконтроллера ESP32. Со стандартной версией библиотеки из менеджера библиотек
// проект не будет компилироваться
//
// Библиотеку ESPAsyncE131 следует обязательно устанавливать из папки проекта. В ней исправлены ошибки стандартной
// библиотеки (добавлен деструктор и освобождение выделяемых ресурсов), а также добавлен ряд функций, позволяющих
// осуществлять передачу сформированных пакетов в сеть. Со стандартной версией библиотеки из менеджера библиотек
// проект не будет компилироваться
//
// Библиотеку ESPAsyncWebServer владельцам Iphone следует обязательно устанавливать из папки проекта. 
// В ней в файле WebResponses.cpp в строчках 538, 569 закомментарено добавление заголовка addHeader("Content-Disposition", buf)
// Точнее перенесено в область if() else - для download=true.
// Почему-то наличие этого заголовка в айфонах не открывает запрошенную страничку в браузере, а предлагает мохранить
// загружаемый файл intex.html.gz на диск как обычный download- файл.
//
// -------------------------------------------------------------------------------------------------------
//
// Для начала работы выполните настройки параметров прошивки - укажите нужные значения в файлах 
//  a_def_hard.h - параметры вашей матрицы, наличие дополнительных модулей, пины подключения, использование возможностей прошивки
//  a_def_soft.h - параметры подулючения к сети - имя сети, пароль, настройки MQTT если  используется, временная зона, коды для получения погоды и т.д.
// Большинство параметров в последствии могут быть изменены в Web-интерфейсе при подключении к устройству или в приложении на смартфоне, 
// если таковое уже есть для текущей версии прошивки.
//
// -------------------------------------------------------------------------------------------------------
// Версии (что нового): 
// -------------------------------------------------------------------------------------------------------
//
// v1.14 - добавлено управление через Web-интерфейс.
//         Android-приложения для настройки и управления устройством более несовместимы с этой версией и работать с ней не будут.
//         Управление устройством будет осуществляться только через Web-интерфейс.
//         Файлы web-интерфейса находятся в подпапке 'data/web' в папке с файлами скетча прошивки и загружаются в микроконтроллер отдельно
//         после компиляции и загрузки скетча. Загрузка фвйлов из папки 'data' выполняется плагином 'LittleFS DataUpload' для соответствующего 
//         типа микроконтроллера - ESP32 или ESP8266.
//
//           Прошивка стабильно работает на микроконтроллерах ESP32, предпочтительно собирать устройство на нем.
//           Микроконтроллер ESP8266 все еще поддерживается для матриц размером до 500-700 диодов, но устройство может перезагружаться при попытке
//           открытия Web-интерфейса в браузере из за нехватки оперативной памяти у этого типа микроконтроллеров.
//           По той же причине недостатка оперативной памяти Web-интерфейс может вообще не открываться - передача требуемых файлов Web-странички прерывается,
//           если микроконтроллеру недостаточно оперативной памяти. Проблемы начинаются, когда свободной оперативной памяти остается менее 12-15 килобайт.
//           Сколько памяти остается свободной - зависит от размера матрицы и включенных функций - MQTT, MP3 Player, индикатор TM1637, поддержка E131 и т.д.
//
//         Управление через UDP и MQTT каналы прошивкой поддерживается, но в настоящий момент нет готового приложения, поддерживающего управления через UDP/MQTT.
//          
//         Изменилась карта распределения хранения настроек в постоянной памяти EEPROM
//         После сборки и загрузки скомпилированной прошивки 1.14 вам придется заново перенастраивать все эффекты
//         и прочие настройки программы, в том числе набор текстов бегущей строки. Ввиду несовместимости расположения 
//         сохраненных настроек, их восстановление из файла резервной копии также недоступно или приведет к
//         сбоям в работе прошивки. Первая загрузка прошивки 1.14 поверх более ранней опции должна быть выполнена
//         с настройкой в меню "Инструменты" - "Erase Flash: Erase All"
//
//
// **************************************************************
// *                        WIFI ПАНЕЛЬ                         *
// **************************************************************

#include "a_def_hard.h"     // Определение параметров матрицы, пинов подключения и т.п
#include "a_def_soft.h"     // Определение параметров эффектов, переменных программы и т.п.

void setup() {
  #if defined(ESP8266)
    ESP.wdtEnable(WDTO_8S);
  #endif

  // Инициализация EEPROM и загрузка начальных значений переменных и параметров
  EEPROM.begin(EEPROM_MAX);

  #if (DEBUG_SERIAL == 1)
    Serial.begin(115200);
    delay(300);
  #endif

  // пинаем генератор случайных чисел
  #if defined(ESP8266) && defined(TRUE_RANDOM)
  uint32_t seed = (int)RANDOM_REG32;
  #else
  uint32_t seed = (int)(analogRead(0) ^ micros());
  #endif
  randomSeed(seed);
  random16_set_seed(seed);

  #ifdef DEVICE_ID
    host_name = String(HOST_NAME) + '-' + String(DEVICE_ID);
  #else
    host_name = String(HOST_NAME);
  #endif

  uint8_t eeprom_id = EEPROMread(0);

  DEBUGLN();
  DEBUG(FIRMWARE_VER);
  #if defined(ESP32)
  DEBUGLN(F("-esp32"));
  #else
  DEBUGLN(F("-esp8266"));
  #endif
  DEBUG(F("Версия EEPROM: "));
  DEBUGLN(String(F("0x")) + IntToHex(eeprom_id, 2));  
  if (eeprom_id != EEPROM_OK) {
    DEBUG(F("Обновлено до: "));
    DEBUGLN(String(F("0x")) + IntToHex(EEPROM_OK, 2));  
  }
  DEBUGLN(String(F("Host: '")) + host_name + '\'');
  DEBUGLN();
  
  DEBUGLN(F("Инициализация файловой системы... "));
  
  spiffs_ok = LittleFS.begin();
  if (!spiffs_ok) {
    DEBUGLN(F("Выполняется разметка файловой системы... "));
    LittleFS.format();
    spiffs_ok = LittleFS.begin();    
  }

  if (spiffs_ok) {
    #if defined(ESP32)
      spiffs_total_bytes = LittleFS.totalBytes();
      spiffs_used_bytes  = LittleFS.usedBytes();
      DEBUGLN(String(F("Использовано ")) + String(spiffs_used_bytes) + String(F(" из ")) + String(spiffs_total_bytes) + String(F(" байт")));
    #else
      FSInfo fs_info;
      if (LittleFS.info(fs_info)) {
        spiffs_total_bytes = fs_info.totalBytes;
        spiffs_used_bytes  = fs_info.usedBytes;
        DEBUGLN(String(F("Использовано ")) + String(spiffs_used_bytes) + String(F(" из ")) + String(spiffs_total_bytes) + String(F(" байт")));
      } else {
        DEBUGLN(F("Ошибка получения сведений о файловой системе."));
    }
    #endif
  } else {
    DEBUGLN(F("Файловая система недоступна."));
    DEBUGLN(F("Управление через Web-канал недоступно."));
    DEBUGLN(F("Функционал \"Бегущая строка\" недоступен."));
  }
  DEBUGLN();
 
  loadSettings();

  // -----------------------------------------  
  // Вывод основных возможностей: поддержка в прошивке - 
  // включена или выключена + некоторые параметры
  // -----------------------------------------  

  DEBUGLN();
  DEBUG(F("Матрица: "));
  if (DEVICE_TYPE == 0) DEBUG(F("труба "));
  if (DEVICE_TYPE == 1) DEBUG(F("плоская "));
  DEBUGLN(String(pWIDTH) + 'x' + String(pHEIGHT));

  if (sMATRIX_TYPE == 2) {
    DEBUGLN(F("Aдресация: карта индексов"));
  } else {
    DEBUGLN(F("Адресация: по подключению"));
    DEBUG(F("  Угол: ")); 
    if (sCONNECTION_ANGLE == 0) DEBUGLN(F("левый нижний")); else
    if (sCONNECTION_ANGLE == 1) DEBUGLN(F("левый верхний")); else
    if (sCONNECTION_ANGLE == 2) DEBUGLN(F("правый верхний")); else
    if (sCONNECTION_ANGLE == 3) DEBUGLN(F("правый нижний"));
    DEBUG(F("  Направление: "));
    if (sSTRIP_DIRECTION == 0) DEBUGLN(F("вправо")); else
    if (sSTRIP_DIRECTION == 1) DEBUGLN(F("вверх")); else
    if (sSTRIP_DIRECTION == 2) DEBUGLN(F("влево")); else
    if (sSTRIP_DIRECTION == 3) DEBUGLN(F("вниз"));
    DEBUG(F("  Тип: "));
    if (sMATRIX_TYPE == 0) DEBUGLN(F("зигзаг")); else
    if (sMATRIX_TYPE == 1) DEBUGLN(F("параллельная"));
    
    if (mWIDTH > 1 || mHEIGHT > 1) {
      DEBUG(F("  Размер сегмента: "));
      DEBUGLN(String(WIDTH) + 'x' + String(HEIGHT));
      DEBUG(F("Cегменты: "));
      DEBUGLN(String(mWIDTH) + 'x' + String(mHEIGHT));
      DEBUG(F("  Угол: ")); 
      if (mANGLE == 0) DEBUGLN(F("левый нижний")); else
      if (mANGLE == 1) DEBUGLN(F("левый верхний")); else
      if (mANGLE == 2) DEBUGLN(F("правый верхний")); else
      if (mANGLE == 3) DEBUGLN(F("правый нижний"));
      DEBUG(F("  Направление: "));
      if (mDIRECTION == 0) DEBUGLN(F("вправо")); else
      if (mDIRECTION == 1) DEBUGLN(F("вверх")); else
      if (mDIRECTION == 2) DEBUGLN(F("влево")); else
      if (mDIRECTION == 3) DEBUGLN(F("вниз"));
      DEBUG(F("  Тип: "));
      if (mTYPE == 0) DEBUGLN(F("зигзаг")); else
      if (mTYPE == 1) DEBUGLN(F("параллельная"));
    }
  }

  DEBUGLN();
  DEBUGLN(F("Доступные возможности:"));
  
  DEBUG(F("+ Бегущая строка: шрифт "));
  if (BIG_FONT == 0)
    DEBUGLN(F("5x8"));
  if (BIG_FONT == 2)
    DEBUGLN(F("8x13"));
  if (BIG_FONT == 1)
    DEBUGLN(F("10x16"));

  DEBUG(F("+ Кнопка управления: "));
  if (BUTTON_TYPE == 0)
    DEBUGLN(F("сенсорная"));
  if (BUTTON_TYPE == 1)
    DEBUGLN(F("тактовая"));

  DEBUGLN(F("+ Синхронизация времени с сервером NTP"));

  DEBUG((USE_POWER == 1 ? '+' : '-'));
  DEBUGLN(F(" Управление питанием"));

  DEBUG((USE_WEATHER == 1 ? '+' : '-'));
  DEBUGLN(F(" Получение информации о погоде"));

  if (spiffs_ok) {
    DEBUGLN(F("+ Управление через Web-канал"));
  }

  DEBUG((USE_MQTT == 1 ? '+' : '-'));
  DEBUGLN(F(" Управление по каналу MQTT"));

  DEBUG((USE_E131 == 1 ? '+' : '-'));
  DEBUGLN(F(" Групповая синхронизация по протоколу E1.31"));

  DEBUG((USE_TM1637 == 1 ? '+' : '-'));
  DEBUGLN(F(" Дополнительный индикатор TM1637"));

  DEBUG((USE_SD == 1 ? '+' : '-'));
  DEBUG(F(" Эффекты Jinx! с SD-карты"));
  DEBUGLN(USE_SD == 1 && FS_AS_SD == 1 ? String(F(" (эмуляция в FS)")) : "");

  DEBUG((USE_MP3 == 1 ? '+' : '-'));
  DEBUGLN(F(" Поддержка MP3 Player"));

  DEBUGLN();

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

  // -----------------------------------------
  // В этом блоке можно принудительно устанавливать параметры, которые должны быть установлены при старте микроконтроллера
  // -----------------------------------------
  
  // -----------------------------------------  
    
  // Настройки ленты
  allocateLeds();         // leds =  new CRGB[NUM_LEDS];          
  allocateOverlay();      // overlayLEDs = new CRGB[OVERLAY_SIZE];
  DEBUGLN();

  // Распечатать список активных эффектов
  printEffectUsage();
  DEBUGLN();
  
  // Создать массив для карты индексов адресации светодиодов в ленте
  bool ok = loadIndexMap();
  if (sMATRIX_TYPE == 2 && (!ok || mapListLen == 0)) {
    sMATRIX_TYPE = 0;
    putMatrixSegmentType(sMATRIX_TYPE);
  }
  
  /*
  // Это пример, как выводить на две матрицы 16х16 (сборная матрица 32х16) через два пина D2 и D3
  // Чтобы вывод был именно на D2 и D3 - в меню Инструменты - плату выбирать "Wemos D1 mini pro" - при выбранной плате NodeMCU назначение пинов куда-то "съезжает" на другие - нужно искать куда. 
  // Убедитесь в правильном назначении адресации диодов матриц в сборной матрице используя индексные файлы или сьорную матрицу из матриц одного размера и подключения сегментов.
  FastLED.addLeds<WS2812, D2, COLOR_ORDER>(leds, 256).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812, D3, COLOR_ORDER>(leds, 256, 256).setCorrection( TypicalLEDStrip );
  
  FastLED.setBrightness(globalBrightness);
  if (CURRENT_LIMIT > 0) {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  }
  FastLED.clear();
  FastLED.show();
  */

  // Инициализация SD-карты
  #if (USE_SD == 1)
    InitializeSD1();
  #endif

  // Инициализация SD-карты
  #if (USE_SD == 1)
    InitializeSD2();
  #endif

  // Проверить наличие резервной копии настроек EEPROM в файловой системе MK и/или на SD-карте
  eeprom_backup = checkEepromBackup();
  if ((eeprom_backup & 0x02) > 0) {
    DEBUG(F("Найдены сохраненные настройки: "));
    DEBUGLN(F("SD://eeprom.bin"));
  }
  if ((eeprom_backup & 0x01) > 0) {
    DEBUG(F("Найдены сохраненные настройки: "));
    DEBUGLN(F("FS://eeprom.bin"));
  }

  // Поиск доступных анимаций
  initAnimations();

  // Поиск картинок, пригодных для эффекта "Слайды"
  initialisePictures();

  checkWebDirectory();
   
  #if (USE_POWER == 1)
    pinMode(POWER_PIN, OUTPUT);
  #endif
     
  // Первый этап инициализации плеера - подключение и основные настройки
  #if (USE_MP3 == 1)
    InitializeDfPlayer1();
  #endif

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
  #endif

  // Настройка кнопки
  butt.setStepTimeout(100);
  butt.setClickTimeout(500);

  // Второй этап инициализации плеера - проверка наличия файлов звуков на SD карте
  #if (USE_MP3 == 1)
    if (isDfPlayerOk) {
      InitializeDfPlayer2();
      if (!isDfPlayerOk) {
        DEBUGLN(F("MP3 плеер недоступен."));
      }
    } else
        DEBUGLN(F("MP3 плеер недоступен."));
  #endif

  InitializeTexts();

  DEBUG(F("Свободно памяти: "));
  DEBUGLN(ESP.getFreeHeap());

  // Подключение к сети
  connectToNetwork();

  // Обработчики событий web-сервера
  server.on("/", handleRoot); 
  server.onNotFound(handleNotFound);  
  server.begin();
  
  if (spiffs_ok) {
    DEBUGLN(F("HTTP server started"));  
  }

  #if (USE_E131 == 1)
    InitializeE131();
  #endif

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
 
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host_name.c_str());
 
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
 
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
    #if (DEBUG_SERIAL == 1)
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    #endif
  });

  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG(F("Ошибка: "));
    DEBUGLN(error);
    if      (error == OTA_AUTH_ERROR)    DEBUGLN(F("Неверное имя/пароль сети"));
    else if (error == OTA_BEGIN_ERROR)   DEBUGLN(F("Не удалось запустить обновление"));
    else if (error == OTA_CONNECT_ERROR) DEBUGLN(F("Не удалось установить соединение"));
    else if (error == OTA_RECEIVE_ERROR) DEBUGLN(F("Не удалось получить данные"));
    else if (error == OTA_END_ERROR)     DEBUGLN(F("Ошибка завершения сессии"));
  });

  ArduinoOTA.begin();

  // UDP-клиент на указанном порту
  udp.begin(localPort);

  // Открываем Web-сокет управления через web-интерфейс
  initWebSocket();

  DEBUG(F("Свободно памяти: "));
  DEBUGLN(ESP.getFreeHeap());

  // Настройка внешнего дисплея TM1637
  #if (USE_TM1637 == 1)
  display.setBrightness(7);
  display.displayByte(_empty, _empty, _empty, _empty);
  #endif

  // Таймер синхронизации часов
  ntpSyncTimer.setInterval(1000 * 60 * SYNC_TIME_PERIOD);

  #if (USE_WEATHER == 1)     
  // Таймер получения погоды
  weatherTimer.setInterval(1000 * 60 * SYNC_WEATHER_PERIOD);
  #endif

  // Таймер рассвета
  dawnTimer.stopTimer();
  
  // Проверить соответствие позиции вывода часов размерам матрицы
  // При необходимости параметры отображения часов корректируются в соответствии с текущими аппаратными возможностями
  checkClockOrigin();
  
  // Если был задан спец.режим во время предыдущего сеанса работы матрицы - включить его
  // Номер спец-режима запоминается при его включении и сбрасывается при включении обычного режима или игры
  // Это позволяет в случае внезапной перезагрузки матрицы (например по wdt), когда был включен спец-режим (например ночные часы или выкл. лампы)
  // снова включить его, а не отображать случайный обычный после включения матрицы
  int8_t spc_mode = getCurrentSpecMode();

  if (spc_mode >= 0 && spc_mode < MAX_SPEC_EFFECT) {
    setSpecialMode(spc_mode);
    set_isTurnedOff(spc_mode == 0);
    set_isNightClock(spc_mode == 8);        
  } else {
    set_thisMode(getCurrentManualMode());
    if (thisMode < 0 || thisMode == MC_TEXT || thisMode >= SPECIAL_EFFECTS_START) {
      setRandomMode();
    } else {
      setEffect(thisMode);        
    }
  }
  autoplayTimer = millis();


  #if (USE_MQTT == 1)
  // Настройка соединения с MQTT сервером
  stopMQTT = !useMQTT;
  changed_keys = "";
  mqtt_client_name = host_name + '-' + String(random16(), HEX);
  last_mqtt_server = mqtt_server;
  last_mqtt_port = mqtt_port;
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  mqtt.setSocketTimeout(1);
  uint32_t t = millis();

  checkMqttConnection();
  if (millis() - t > MQTT_CONNECT_TIMEOUT) {
    nextMqttConnectTime = millis() + MQTT_RECONNECT_PERIOD;
  }
  String msg = F("START");
  SendMQTT(msg, TOPIC_STA);
  #endif

  // При активном состоянии MQRTT/WEB канала отправить текущее состояние на сервер
  sendStartState(MQTT);

  setIdleTimer();
}

void loop() {
  if (wifi_connected) {
    ArduinoOTA.handle();
    #if (USE_MQTT == 1)
      if (!stopMQTT) {
        checkMqttConnection();
        mqtt.loop();
      }
    #endif
  }
  
  ws.cleanupClients();
  processOutQueueW();

  // Если память под светодиоды была временно освобожденя для Web-сервера - рисовать негде. Ничего не делаем, просто ждем
  // пока WebServer закончит отправку файла и освободит память. В это время картинка на матрице "замрет".
  // Если память есть - выделяем ее под массив светодиодов
  if (leds == nullptr) {
    if (ESP.getFreeHeap() > NUM_LEDS * sizeof(CRGB)) {
      allocateLeds();
    }
  }

  if (leds == nullptr) {
    // Памяти нет, массив не создан - ничего не делаем.
    delay (10);
  } else {  
    // Если память под оверлей была временно освобожденя для Web-сервера - оверлей работать не будет. На матрице будут наблюдаться дефекты,
    // особенно на эффектах, работающих с оверлеем и в бегущей строке. Ждем пока WebServer закончит отправку файла и освободит памятью
    // Если свободная память появилась - воссоздаем массив оверлея.
    if (overlayLEDs == nullptr) {
      if (ESP.getFreeHeap() > OVERLAY_SIZE * sizeof(CRGB)) {
        allocateOverlay();
      }
    }
    process();
  }

  // При нехватки памяти для ответа на Web-запрос происходит временное освобождение памяти оверлея и массива диодов
  // На каждом цикле проверяется достаточно ли свободной памяти и попытка выделить память для массива светодиодов и овердея
  // Иногда память настолько фрагменирована, что выделить цельный кусок не удается.
  // После 1000 неудачных попыток - перезагрузить контроллер
  if (restartGuard > 1000) {
    DEBUGLN(F("Не удается выделить достаточно памяти."));
    DEBUGLN(F("Выполняется перезапуск системы."));
    delay(100);
    ESP.restart();
  }
}

// -----------------------------------------

void startWiFi(uint32_t waitTime) {
  
  WiFi.disconnect(true);
  set_wifi_connected(false);
  
  delay(10);               // Иначе получаем Core 1 panic'ed (Cache disabled but cached memory region accessed)
  WiFi.mode(WIFI_STA);
 
  // Пытаемся соединиться с роутером в сети
  if (ssid.length() > 0) {
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
    }              
    WiFi.begin(ssid.c_str(), pass.c_str());
  
    // Проверка соединения (таймаут 180 секунд, прерывается при необходимости нажатием кнопки)
    // Такой таймаут нужен в случае, когда отключают электричество, при последующем включении устройство стартует быстрее
    // чем роутер успеет загрузиться и создать сеть. При коротком таймауте устройство не найдет сеть и создаст точку доступа,
    // не сможет получить время, погоду и т.д.
    bool     stop_waiting = false;
    uint32_t start_wifi_check = millis();
    uint32_t last_wifi_check = 0;
    int16_t  cnt = 0;
    while (!(stop_waiting || wifi_connected)) {
      delay(0);
      if (millis() - last_wifi_check > 250) {
        last_wifi_check = millis();
        set_wifi_connected(WiFi.status() == WL_CONNECTED); 
        if (wifi_connected) {
          // Подключение установлено
          DEBUGLN();
          DEBUG(F("WiFi подключен. IP адрес: "));
          DEBUGLN(WiFi.localIP());
          break;
        }
        if (cnt % 50 == 0) {
          DEBUGLN();
        }
        DEBUG('.');
        cnt++;
      }
      if (millis() - start_wifi_check > waitTime) {
        // Время ожидания подключения к сети вышло
        break;
      }
      delay(0);
      // Опрос состояния кнопки
      butt.tick();
      if (butt.hasClicks()) {
        butt.getClicks();
        DEBUGLN();
        DEBUGLN(F("Нажата кнопка.\nОжидание подключения к сети WiFi прервано."));  
        stop_waiting = true;
        break;
      }
      delay(0);
    }
    DEBUGLN();

    if (!wifi_connected && !stop_waiting)
      DEBUGLN(F("Не удалось подключиться к сети WiFi."));
  }  
}

void startSoftAP() {
  WiFi.softAPdisconnect(true);
  ap_connected = false;

  DEBUG(F("Создание точки доступа "));
  DEBUGLN(apName);
  
  ap_connected = WiFi.softAP(apName, apPass);

  for (uint8_t j = 0; j < 10; j++ ) {    
    delay(0);
    if (ap_connected) {
      DEBUGLN();
      DEBUG(F("Точка доступа создана. Сеть: '"));
      DEBUG(apName);
      // Если пароль совпадает с паролем по умолчанию - печатать для информации,
      // если был изменен пользователем - не печатать
      if (strcmp(apPass, "12341234") == 0) {
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
    ap_connected = WiFi.softAP(apName, apPass);
  }  
  DEBUGLN();  

  if (!ap_connected) 
    DEBUGLN(F("Не удалось создать WiFi точку доступа."));
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

  // Сообщить UDP порт, на который ожидаются подключения
  if (wifi_connected || ap_connected) {
    DEBUG(F("UDP-сервер на порту "));
    DEBUGLN(localPort);
  }
}
