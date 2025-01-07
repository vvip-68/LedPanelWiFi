// *************************************************************************

#define EEPROM_MAX    1536                  // Максимальный размер EEPROM доступный для использования (4096)
#define EFFECT_EEPROM  800                  // Начальная ячейка eeprom с параметрами эффектов, 10 байт на эффект

// *************************************************************************

void initializeEEPROM() {

  // Инициализация EEPROM? подготовка к работе
  
  EEPROM.begin(EEPROM_MAX);
  delay(100);

  eeprom_id = EEPROMread(0);
  isEEPROMInitialized =  eeprom_id == EEPROM_OK;
  
  if (!isEEPROMInitialized) {
    clearEEPROM();
    vDEBUG_SERIAL = DEBUG_SERIAL;
  } else {
    vDEBUG_SERIAL = getDebugSerialEnable();  
  }

}

void loadSettings() {

  // Адреса в EEPROM:
  //    0 - если EEPROM_OK - EEPROM инициализировано, если другое значение - нет                             // EEPROMread(0)                 // EEPROMWrite(0, EEPROM_OK)
  //    1 - максимальная яркость ленты 1-255                                                                 // getMaxBrightness()            // putMaxBrightness(deviceBrightness)
  //    2 - автосмена режима в демо: вкл/выкл                                                                // getAutoplay();                // putAutoplay(manualMode)
  //    3 - время автосмены режимов в сек                                                                    // getAutoplayTime()             // putAutoplayTime(autoplayTime / 1000L)     // autoplayTime - мс; в ячейке - в сек
  //    4 - время бездействия до переключения в авторежим в минутах                                          // getIdleTime()                 // putIdleTime(idleTime / 60L / 1000L)       // idleTime - мс; в ячейке - в мин
  //    5 - использовать синхронизацию времени через NTP                                                     // getUseNtp()                   // putUseNtp(useNtp)
  //    6 - 12/24-часовой формат времени                                                                     // getTime12()                   // putTime12(time_h12) 
  //    7 - градусы Сельсия / Фаренгейта                                                                     // getIsFarenheit()              // putIsFarengeit(isFarenheit)
  //    8 - шрифт 3х5 - 0 - квадратный, 1 - круглый                                                          // getSmallFontType()            // putSmallFontType(use_round_3x5)
  //    9 - выключать индикатор часов при выключении лампы true - выключать / false - не выключать           // getTurnOffClockOnLampOff()    // putTurnOffClockOnLampOff(needTurnOffClock)
  //   10 - IP[0]                                                                                            // getStaticIP()                 // putStaticIP(IP_STA[0], IP_STA[1], IP_STA[2], IP_STA[3])
  //   11 - IP[1]                                                                                            // - " -                         // - " -
  //   12 - IP[2]                                                                                            // - " -                         // - " -
  //   13 - IP[3]                                                                                            // - " -                         // - " -
  //   14 - Использовать режим точки доступа                                                                 // getUseSoftAP()                // putUseSoftAP(useSoftAP)
  //   15 - ориентация часов горизонтально / вертикально                                                     // getClockOrientation()         // putClockOrientation(CLOCK_ORIENT)
  // **16 - не используется
  //   17 - Интервал отображения часов фазы T1/T3                                                            // getClockCycleT1()             // putClockCycleT1(clock_cycle_T1)
  //   18 - Интервал отображения часов фазы T2/T4                                                            // getpClockCycleT2()            // putClockCycleT2(clock_cycle_T2)
  //   19 - тип часов горизонтальной ориентации 0-авто 1-малые 3х5 2 - большие 5х7                           // getClockSize()                // putClockSize(CLOCK_SIZE)
  //   20 - Будильник, дни недели                                                                            // getAlarmWeekDay()             // putAlarmParams(alarmWeekDay, dawnDuration, alarmEffect, alarmDuration)
  //   21 - Будильник, продолжительность "рассвета"                                                          // getDawnDuration()             // putAlarmParams(alarmWeekDay, dawnDuration, alarmEffect, alarmDuration)
  //   22 - Будильник, эффект "рассвета"                                                                     // getAlarmEffect()              // putAlarmParams(alarmWeekDay, dawnDuration, alarmEffect, alarmDuration)
  //   23 - Будильник, использовать звук                                                                     // getUseAlarmSound()            // putAlarmSounds(useAlarmSound, maxAlarmVolume, alarmSound, dawnSound)
  //   24 - Будильник, играть звук N минут после срабатывания                                                // getAlarmDuration()            // putAlarmParams(alarmWeekDay, dawnDuration, alarmEffect, alarmDuration)
  //   25 - Будильник, Номер мелодии будильника (из папки 01 на SD карте)                                    // getAlarmSound()               // putAlarmSounds(useAlarmSound, maxAlarmVolume, alarmSound, dawnSound)
  //   26 - Будильник, Номер мелодии рассвета (из папки 02 на SD карте)                                      // getDawnSound()                // putAlarmSounds(useAlarmSound, maxAlarmVolume, alarmSound, dawnSound)
  //   27 - Будильник, Максимальная громкость будильника                                                     // getMaxAlarmVolume()           // putAlarmSounds(useAlarmSound, maxAlarmVolume, alarmSound, dawnSound)
  //   28 - Номер последнего активного спец-режима или -1, если были включены обычные эффекты                // getCurrentSpecMode()          // putCurrentSpecMode(xxx)                     
  //   29 - Номер последнего активированного вручную режима                                                  // getCurrentManualMode()        // putCurrentManualMode(xxx)
  //   30 - Отображать часы оверлеем в режимах                                                               // getClockOverlayEnabled()      // putClockOverlayEnabled(clockOverlayEnabled)
  //   31 - Использовать случайную последовательность в демо-режиме                                          // getRandomMode()               // putRandomMode(useRandomSequence)
  // **32 -  не используется
  //   33 - Режим 1 по времени - часы                                                                        // getAM1hour()                  // putAM1hour(AM1_hour)
  //   34 - Режим 1 по времени - минуты                                                                      // getAM1minute()                // putAM1minute(AM1_minute) 
  //   35 - Режим 1 по времени - -3 - выкл. (не исп.); -2 - выкл. (черный экран); -1 - ночн.часы, 0 - случ., // getAM1effect()                // putAM1effect(AM1_effect_id)
  //   36 - Режим 2 по времени - часы                                      ^^^ 1,2..N - эффект EFFECT_LIST   // getAM2hour()                  // putAM2hour(AM2_hour) 
  //   37 - Режим 2 по времени - минуты                                                                      // getAM2minute()                // putAM2minute(AM2_minute)
  //   38 - Режим 2 по времени - = " = как для режима 1                                                      // getAM2effect()                // putAM2effect(AM2_effect_id)
  //   39 - Цвет ночных часов:  0 - R; 1 - G; 2 - B; 3 - C; 4 - M; 5 - Y; 6 - W;                             // getNightClockColor()          // putNightClockColor(nightClockColor)          
  //   40 - Будильник, время: понедельник : часы                                                             // getAlarmHour(1)               // putAlarmTime(1, alarmHour[0], alarmMinute[0])  // for (byte i=0; i<7; i++) alarmHour[i] = getAlarmHour(i+1)
  //   41 - Будильник, время: понедельник : минуты                                                           // getAlarmMinute(1)             // putAlarmTime(1, alarmHour[0], alarmMinute[0])  // for (byte i=0; i<7; i++) alarmMinute[i] = getAlarmMinute(i+1)
  //   42 - Будильник, время: вторник : часы                                                                 // getAlarmHour(2)               // putAlarmTime(2, alarmHour[1], alarmMinute[1])  // for (byte i=0; i<7; i++) putAlarmTime(i+1, alarmHour[i], alarmMinute[i])
  //   43 - Будильник, время: вторник : минуты                                                               // getAlarmMinute(2)             // putAlarmTime(2, alarmHour[1], alarmMinute[1])
  //   44 - Будильник, время: среда : часы                                                                   // getAlarmHour(3)               // putAlarmTime(3, alarmHour[2], alarmMinute[2])
  //   45 - Будильник, время: среда : минуты                                                                 // getAlarmMinute(3)             // putAlarmTime(3, alarmHour[2], alarmMinute[2])
  //   46 - Будильник, время: четверг : часы                                                                 // getAlarmHour(4)               // putAlarmTime(4, alarmHour[3], alarmMinute[3])
  //   47 - Будильник, время: четверг : минуты                                                               // getAlarmMinute(4)             // putAlarmTime(4, alarmHour[3], alarmMinute[3])
  //   48 - Будильник, время: пятница : часы                                                                 // getAlarmHour(5)               // putAlarmTime(5, alarmHour[4], alarmMinute[4])
  //   49 - Будильник, время: пятница : минуты                                                               // getAlarmMinute(5)             // putAlarmTime(5, alarmHour[4], alarmMinute[4])
  //   50 - Будильник, время: суббота : часы                                                                 // getAlarmHour(6)               // putAlarmTime(6, alarmHour[5], alarmMinute[5])
  //   51 - Будильник, время: суббота : минуты                                                               // getAlarmMinute(6)             // putAlarmTime(6, alarmHour[5], alarmMinute[5])
  //   52 - Будильник, время: воскресенье : часы                                                             // getAlarmHour(7)               // putAlarmTime(7, alarmHour[6], alarmMinute[6])
  //   53 - Будильник, время: воскресенье : минуты                                                           // getAlarmMinute(7)             // putAlarmTime(7, alarmHour[6], alarmMinute[6])
  //  54-63   - имя точки доступа    - 10 байт                                                               // getSoftAPName().toCharArray(apName, 10)       // putSoftAPName(String(apName))       // char apName[11] = ""
  //  64-79   - пароль точки доступа - 16 байт                                                               // getSoftAPPass().toCharArray(apPass, 16)       // putSoftAPPass(String(apPass))       // char apPass[17] = "" 
  //   80 - ширина сегмента матрицы
  //   81 - высота сегмента матрицы
  //   82 - тип сегмента матрицы (параллельная / зигзаг)
  //   83 - угол подключения к сегменту матрицы
  //   84 - направление линейки диодов из угла
  //   85 - количество сегментов по ширине матрицы
  //   86 - количество сегментов по высоте матрицы
  //   87 - тип мета-матрицы (параллельная / зигзаг)
  //   88 - угол подключения к мета-матрице
  //   89 - направление сегментов из угла мета-матрицы
  //   90 - режим работы - Standalone/Master/Slave
  //   91 - тип данных в пакете - Phisic/Logic/Command
  //   92 - группа синхронизации
  //   94 - полная ширина матрицы при использовании карты индексов
  //   95 - полная высота матрицы при использовании карты индексов
  //   96 - Отображение температуры в часах - битовая карта b0: 0 - не рисовать C/F, 1 - рисовать C/F; b1: 0 - не рисовать заначок градуса; 1 - рисовать значок градуса   // getShowTempProps()  putShowTempProps()
  //   97 - время задержки повтора нажатия кнопки в играх 10..100                                            // getGameButtonSpeed();          // putGameButtonSpeed(val)
  //   98 - masterX - трансляция экрана с MASTER - координата X мастера с которой начинается прием изображения
  //   99 - masterY - трансляция экрана с MASTER - координата Y мастера с которой начинается прием изображения
  //  100 - localX  - трансляция экрана с MASTER - локальная координата X куда начинается вывод изображения
  //  101 - localY  - трансляция экрана с MASTER - локальная координата Y куда начинается вывод изображения
  //  102 - localW  - трансляция экрана с MASTER - ширина окна вывода изображения
  //  103 - localH  - трансляция экрана с MASTER - высота окна вывода изображения
  //**104-119  - не используется
  //  120-149 - имя NTP сервера      - 30 байт                                                               // getNtpServer().toCharArray(ntpServerName, 31) // putNtpServer(String(ntpServerName)) // char ntpServerName[31] = ""
  //  150,151 - лимит по току в миллиамперах                                                                 // getPowerLimit()                // putPowerLimit(CURRENT_LIMIT)
  //  152 - globalClockColor.r -  цвет часов в режиме MC_COLOR, режим цвета "Монохром"                       // getGlobalClockColor()          // putGlobalClockColor(globalClockColor)              // uint32_t globalClockColor
  //  153 - globalClockColor.g                                                                               // - " -                          // - " -
  //  154 - globalClockColor.b                                                                               // - " -                          // - " -
  //  155 - globalTextColor.r -  цвет текста в режиме MC_TEXT, режим цвета "Монохром"                        //  getGlobalTextColor()          // putGlobalTextColor(globalTextColor)                // uint32_t globalTextColor 
  //  156 - globalTextColor.g                                                                                // - " -                          // - " -
  //  157 - globalTextColor.b                                                                                // - " -                          // - " -
  //  158 - globalColor.r     -  цвет панели в режиме "лампа"                                                // getGlobalColor()               // putGlobalColor(globalColor)                        // uint32_t globalColor
  //  159 - globalColor.g                                                                                    // - " -                          // - " -
  //  160 - globalColor.b                                                                                    // - " -                          // - " -
  //  161 - Режим 3 по времени - часы                                                                        // getAM3hour()                   // putAM3hour(AM3_hour)
  //  162 - Режим 3 по времени - минуты                                                                      // getAM3minute()                 // putAM3minute(AM3_minute) 
  //  163 - Режим 3 по времени - так же как для режима 1                                                     // getAM3effect()                 // putAM3effect(AM3_effect_id)
  //  164 - Режим 4 по времени - часы                                                                        // getAM4hour()                   // putAM4hour(AM4_hour)
  //  165 - Режим 4 по времени - минуты                                                                      // getAM4minute()                 // putAM4minute(AM4_minute)
  //  166 - Режим 4 по времени - так же как для режима 1                                                     // getAM4effect()                 // putAM4effect(AM4_effect_id)
  //  167,168 - интервал отображения текста бегущей строки                                                   // getTextInterval()              // putTextInterval(TEXT_INTERVAL)
  //  169 - Режим цвета оверлея часов X: 0,1,2,3                                                             // getClockColor()                // putClockColor(COLOR_MODE)
  //  170 - Скорость прокрутки оверлея часов                                                                 // getClockScrollSpeed()          // putClockScrollSpeed(speed_value)  
  //  171 - Режим цвета оверлея текста X: 0,1,2,3                                                            // getTextColor()                 // putTextColor(COLOR_TEXT_MODE)  
  //  172 - Скорость прокрутки оверлея текста                                                                // getTextScrollSpeed()           // putTextScrollSpeed(speed_value)  
  //  173 - Отображать бегущую строку оверлеем в режимах                                                     // getTextOverlayEnabled()        // putTextOverlayEnabled(textOverlayEnabled)
  //  174 - Использовать сервис получения погоды 0- нет, 1 - Yandex; 2 - OpenWeatherMap                      // getUseWeather()                // putUseWeather(useWeather)
  //  175 - Период запроса информации о погоде в минутах                                                     // getWeatherInterval()           // putWeatherInterval(SYNC_WEATHER_PERIOD)
  // 176,177,178,179 - Код региона Yandex для получения погоды (4 байта - uint32_t)                          // getWeatherRegion()             // putWeatherRegion(regionID)
  //  180 - цвет температуры в дневных часах: 0 - цвет часов; 1 - цвет в зависимости от температуры          // getUseTemperatureColor()       // putUseTemperatureColor(useTemperatureColor)
  //  181 - цвет температуры в ночных часах:  0 - цвет часов; 1 - цвет в зависимости от температуры          // getUseTemperatureColorNight()  // putUseTemperatureColorNight(useTemperatureColorNight)
  // **182-238 - не используется
  //  239 - textOffsetY = 0;        // Смещение бегущей строки относительно центра (коррекция положения по оси Y)  - большие часы
  //  240 - яркость ночных часов                                                                             // getNightClockBrightness()      // putNightClockBrightness(nightClockBrightness)
  //  241 - Отображение года в горизонтальных часах 0/2/4 цифры  (малые часы, младшие 4 бита)                // getCalendarYearWidth()         // putCalendarYearWidth(сalendarYearWidth)
  //  241 - Отображение года в 1 строку или 2 строки - (малые часы, старшие 4 бита) 
  //  242 - Отображение года в горизонтальных часах 0/2/4 цифры  (большие часы) (младшие 4 бита)
  //  242 - Отображение года в 1 строку или 2 строки - (большие часы, старшие 4 бита) 
  //  243 - Режим по времени "Рассвет" - так же как для режима 1                                             // getAM5effect()                 // putAM5effect(dawn_effect_id)
  // 244,245,246,247 - Код региона OpenWeatherMap для получения погоды (4 байта - uint32_t)                  // getWeatherRegion2()            // putWeatherRegion2(regionID2)
  //  248 - Режим по времени "Закат" - так же как для режима 1                                               // getAM6effect()                 // putAM6effect(dawn_effect_id)  
  //  249 - clockOffsetX = 0;       // Смещение часов относительно центра (коррекция положения по оси X) - малые часы
  //  250 - clockOffsetY = 0;       // Смещение часов относительно центра (коррекция положения по оси Y) - малые часы
  //  251 - tempOffsetX = 0;        // Смещение температуры относительно центра (коррекция положения по оси X) - малые часы
  //  252 - tempOffsetY = 0;        // Смещение температуры относительно центра (коррекция положения по оси Y) - малые часы
  //  253 - clockDotWidth = 2;      // Ширина разделительных точек в больших часах 1 или 2, если позволяет ширина матрицы
  //  254 - clockDotSpace = 1;      // Точки в больших часах отделены от цифр пробелом (если позволяет ширина матрицы) - 0 или 1
  //  255 - useDHCP = false;        // Использовать DHCP вместо статического адреса
  //  256 - Отображение температуры в макросе {WT} бегущей строки - битовая карта b0: 0 - не рисовать C/F, 1 - рисовать C/F; b1: 0 - не рисовать заначок градуса; 1 - рисовать значок градуса
  //  257 - calendarOffsetX = 0;    // Смещение календаря относительно центра (коррекция положения по оси X) - малые часы
  //  258 - calendarOffsetY = 0;    // Смещение календаря относительно центра (коррекция положения по оси Y) - малые часы
  //  259 - textOffsetY = 0;        // Смещение бегущей строки относительно центра (коррекция положения по оси Y)
  //  260 - clockOffsetX = 0;       // Смещение часов относительно центра (коррекция положения по оси X) - большие часы
  //  261 - clockOffsetY = 0;       // Смещение часов относительно центра (коррекция положения по оси Y) - большие часы
  //  262 - tempOffsetX = 0;        // Смещение температуры относительно центра (коррекция положения по оси X) - большие часы
  //  263 - tempOffsetY = 0;        // Смещение температуры относительно центра (коррекция положения по оси Y) - большие часы
  //  264 - calendarOffsetX = 0;    // Смещение календаря относительно центра (коррекция положения по оси X) - большие часы
  //  265 - calendarOffsetY = 0;    // Смещение календаря относительно центра (коррекция положения по оси Y) - большие часы
  //  266 -                         // Фазы включения отображения часов (младшие 4 бита) и календаря (Старишие 4 бита) T4T3T2T1 T4T3T2T1 в режиме малых часов 
  //  267 -                         // Фазы включения отображения часов (младшие 4 бита) и календаря (Старишие 4 бита) T4T3T2T1 T4T3T2T1 в режиме больших часов 
  //  268 - флаги b7b6b5b4b3b2b1b0 - b3b2b1b0 - при показе бегущей строки b0 -скрывть часы; b1 - скрывать календарь; b2 - скрывть температуру; b7b6b5b4b3 - зарезервировано - в малых часах
  //  269 - флаги b7b6b5b4b3b2b1b0 - b3b2b1b0 - при показе бегущей строки b0 -скрывть часы; b1 - скрывать календарь; b2 - скрывть температуру; b7b6b5b4b3 - зарезервировано - в больших часах
  //  270 - флаги b7b6b5b4b3b2b1b0 - b1b0 - выравнивание часов       00 - центр 01 - лево 10 - право 11 - зарезервировано  малые часы
  //                               - b3b2 - выравнивание календаря   00 - центр 01 - лево 10 - право 11 - зарезервировано
  //                               - b5b4 - выравнивание температуры 00 - центр 01 - лево 10 - право 11 - зарезервировано
  //                               - b7b6 - зарезервировано
  //  271 - флаги b7b6b5b4b3b2b1b0 - b1b0 - выравнивание часов       00 - центр 01 - лево 10 - право 11 - зарезервировано  большие часы
  //                               - b3b2 - выравнивание календаря   00 - центр 01 - лево 10 - право 11 - зарезервировано
  //                               - b5b4 - выравнивание температуры 00 - центр 01 - лево 10 - право 11 - зарезервировано
  //                               - b7b6 - зарезервировано
  //  272 - флаги b7b6b5b4b3b2b1b0 - b1b0 - отображ календаря 00 - в позиции часов, 11 - произвольно; 01 и 10 - зарезервировано
  //                                 b3b2 - отображение температуры 00 - под часами, 01 - справа от часов; 10 - зарезервировано; 11 - произвольно
  //  273 - флаги b7b6b5b4b3b2b1b0 - b1b0 - отображ календаря 0 - в позиции часов, 1 - произвольно; 01 и 10 - зарезервировано
  //                                 b3b2 - отображение температуры 00 - под часами, 01 - справа от часов; 10 - зарезервировано; 11 - произвольно
  //  274 -                         // Фазы включения отображения температуры (младшие 4 бита) T4T3T2T1 в режиме малых часов      Старшие 4 бита не используются
  //  275 -                         // Фазы включения отображения температуры (младшие 4 бита) T4T3T2T1 в режиме больших часов    Старшие 4 бита не используются
  // 276 - порядок цвета для ленты на линии 1
  // 277 - порядок цвета для ленты на линии 2
  // 278 - порядок цвета для ленты на линии 3
  // 279 - порядок цвета для ленты на линии 4
  // 280 - битовая маска b0-b3 - использование каналов вывода на ленту, b6 - DEBUG_SERIAL; b7 - флаг инициализации
  // 281 - пин вывода на ленту линии 1          LED_PIN
  // 282,283 - начальный индекс диодов линии 1 
  // 284,285 - количество диодов линии 1
  // 286 - пин вывода на ленту линии 2
  // 287,288 - начальный индекс диодов линии 2
  // 289,290 - количество диодов линии 2
  // 291 - пин вывода на ленту линии 3
  // 292,293 - начальный индекс диодов линии 3
  // 294,295 - количество диодов линии 3
  // 296 - пин вывода на ленту линии 4
  // 297,298 - начальный индекс диодов линии 4
  // 299,300 - количество диодов линии 4
  // 301 - тип матрицы DEVICE_TYPE = 0 - свернута в трубу для лампы; 1 - плоская матрица в рамке
  // 302 - тип кнопки BUTTON_TYPE = 0 - сенсорная 1 - тактовая
  // 303 - пин подключения кнопки PIN_BTN 
  // 304 - WAIT_PLAY_FINISHED - алгоритм проигрывания эффекта SD-карта
  // 305 - REPEAT_PLAY - алгоритм проигрывания эффекта SD-карта
  // 306 - POWER_PIN - пин управления реле питания
  // 307 - DFPlayer STX
  // 308 - DFPlayer SRX
  // 309 - DIO TM1637
  // 310 - CLK TM1627
  // 311 - активный уровень сигнала управления питанием  
  // 312 - ALARM_PIN - пин вывода при срабатывании будильника
  // 313 - активный уровень сигнала на пине управления будильника
  // 314 - AUX_PIN - пин вывода дополнительного управления
  // 315 - активный уровень сигнала на пине дополнительного управления
  // 316,317 - битовая маска включения дополнительного пина по режимам 00 00 00 00, где  b0,b2,b4,b6 0 - включить пин, 1 - выключить пин; b1,b3,b5,b7 - включен режим времени (1) или нет (0). b1,b0 - Режим 1 .. b7,b6 - Режим 4
  // 318 - текущее состояние дополнительной линии 0 - выключена; 1 - включена
  // 319-350 - 16 символов UTF8 имени устройства (строка)
  // 351-399 - 48 символов правила формирования TIME ZONE для сервиса NTP  
  // 400-431 - имя сети (ssid) до 32 симворов
  // 432-495 - пароль к сети до 64 символов
  //*496-799 - не используется
  // 800 - 800+(Nэфф*10)   - скорость эффекта
  // 801 - 800+(Nэфф*10)+1 - b0 - 1 - использовать текст поверх эффекта; 0 - не использовать; b1 - 1 - использовать часы поверх эффекта; 0 - не использовать;
  // 802 - 800+(Nэфф*10)+2 - специальный параметр эффекта #1
  // 803 - 800+(Nэфф*10)+3 - специальный параметр эффекта #2
  // 804 - 800+(Nэфф*10)+4 - контраст эффекта
  // 805 - 800+(Nэфф*10)+5 - индекс порядка воспроизведения (order)
  // 806 - 800+(Nэфф*10)+6 - зарезервировано
  // 807 - 800+(Nэфф*10)+7 - зарезервировано
  // 808 - 800+(Nэфф*10)+8 - зарезервировано
  // 809 - 800+(Nэфф*10)+9 - зарезервировано
  // ---
  //********
  // EEPROM_MAX-1 - верхняя граница выделенного EEPROM
  //********

  // Инициализировано ли EEPROM
  bool isInitialized = EEPROMread(0) == EEPROM_OK;  
  
  if (isInitialized) {    

    mapWIDTH              = getMatrixMapWidth();
    mapHEIGHT             = getMatrixMapHeight();

    sWIDTH                = getMatrixSegmentWidth();
    sHEIGHT               = getMatrixSegmentHeight();
    sMATRIX_TYPE          = getMatrixSegmentType();
    sCONNECTION_ANGLE     = getMatrixSegmentAngle();
    sSTRIP_DIRECTION      = getMatrixSegmentDirection();

    mWIDTH                = getMetaMatrixWidth();
    mHEIGHT               = getMetaMatrixHeight();
    mTYPE                 = getMetaMatrixType();
    mANGLE                = getMetaMatrixAngle();
    mDIRECTION            = getMetaMatrixDirection();

    if (sMATRIX_TYPE == 2) {
      pWIDTH              = mapWIDTH;
      pHEIGHT             = mapHEIGHT;
    } else {
      pWIDTH              = sWIDTH * mWIDTH;
      pHEIGHT             = sHEIGHT * mHEIGHT;
    }
    
    NUM_LEDS              = pWIDTH * pHEIGHT;
    maxDim                = max(pWIDTH, pHEIGHT);
    minDim                = min(pWIDTH, pHEIGHT);
    
    deviceBrightness      = getMaxBrightness();
    isAuxActive           = getAuxLineState(); 
    auxLineModes          = getAuxLineModes();

    autoplayTime          = getAutoplayTime();
    idleTime              = getIdleTime();

    useNtp                = getUseNtp();
    time_h12              = getTime12();
    use_round_3x5         = getSmallFontType() == 1;
    
    clockOverlayEnabled   = getClockOverlayEnabled();
    textOverlayEnabled    = getTextOverlayEnabled();

    manualMode            = getAutoplay();
    CLOCK_ORIENT          = getClockOrientation();
    COLOR_MODE            = getClockColor();
    CLOCK_SIZE            = getClockSize();
    COLOR_TEXT_MODE       = getTextColor();
    CURRENT_LIMIT         = getPowerLimit();
    TEXT_INTERVAL         = getTextInterval();

    loadPlacementOptions();                               // Загрузить опции размещения часов/календаря/температуры в соотвествии с текущим размером часов (большие / малые)

    useRandomSequence     = getRandomMode();
    nightClockColor       = getNightClockColor();
    nightClockBrightness  = getNightClockBrightness();

    alarmWeekDay          = getAlarmWeekDay();
    alarmEffect           = getAlarmEffect();
    alarmDuration         = getAlarmDuration();
    dawnDuration          = getDawnDuration();

    needTurnOffClock      = getTurnOffClockOnLampOff();

    // Загрузить недельные будильники / часы, минуты /
    for (uint8_t i=0; i<7; i++) {
      alarmHour[i] = getAlarmHour(i+1);
      alarmMinute[i] = getAlarmMinute(i+1);
    }
 
    // Загрузить параметры эффектов #1, #2
    for (uint8_t i=0; i<MAX_EFFECT; i++) {
      effectScaleParam[i] = getScaleForEffect(i); 
      effectScaleParam2[i]= getScaleForEffect2(i);
      effectContrast[i]   = getEffectContrast(i);
      effectSpeed[i]      = getEffectSpeed(i);
    }

    #if (USE_MP3 == 1)
      useAlarmSound       = getUseAlarmSound();
      alarmSound          = getAlarmSound();
      dawnSound           = getDawnSound();
      maxAlarmVolume      = getMaxAlarmVolume();
    #endif

    #if (USE_E131 == 1)
      workMode            = getSyncWorkMode();
      syncMode            = getSyncDataMode();
      syncGroup           = getSyncGroup();;    
    #endif
    
    globalColor           = getGlobalColor();         // цвет лампы, задаваемый пользователем
    globalClockColor      = getGlobalClockColor();    // цвет часов в режиме MC_COLOR, режим цвета "Монохром"
    globalTextColor       = getGlobalTextColor();     // цвет часов бегущей строки в режиме цвета "Монохром"

    useSoftAP             = getUseSoftAP();
    
    AM1_hour              = getAM1hour();
    AM1_minute            = getAM1minute();
    AM1_effect_id         = getAM1effect();
    AM2_hour              = getAM2hour();
    AM2_minute            = getAM2minute();
    AM2_effect_id         = getAM2effect();
    AM3_hour              = getAM3hour();
    AM3_minute            = getAM3minute();
    AM3_effect_id         = getAM3effect();
    AM4_hour              = getAM4hour();
    AM4_minute            = getAM4minute();
    AM4_effect_id         = getAM4effect();
    dawn_effect_id        = getAM5effect();
    dusk_effect_id        = getAM6effect();

    #if (USE_WEATHER == 1)     
      useWeather          = getUseWeather();
      regionID            = getWeatherRegion();
      regionID2           = getWeatherRegion2();
      SYNC_WEATHER_PERIOD = getWeatherInterval();
      useTemperatureColor = getUseTemperatureColor();
      useTemperatureColorNight = getUseTemperatureColorNight();
      isFarenheit         = getIsFarenheit();
      int8_t props        = getShowTempProps();
      showTempDegree      = (props & 0x02) > 0;    
      showTempLetter      = (props & 0x01) > 0;
      props               = getShowTempTextProps();
      showTempTextDegree  = (props & 0x02) > 0;    
      showTempTextLetter  = (props & 0x01) > 0;
    #endif  

    getStaticIP();

    // Загрузить список отобранных эффектов и последовательность их фоспроизведения
    loadEffectOrder();
    
  } else {

    for (uint8_t i = 0; i < MAX_EFFECT; i++) {
      effectScaleParam[i] = 50;             // среднее значение для параметра. Конкретное значение зависит от эффекта
      effectScaleParam2[i]= 0;              // второй параметр эффекта по умолчанию равен 0. Конкретное значение зависит от эффекта
      effectContrast[i]   = 96;             // контраст эффекта
      effectSpeed[i]      = D_EFFECT_SPEED; // скорость эффекта
    }

    putGameButtonSpeed(50);

    // После первой инициализации значений - сохранить их принудительно
    saveDefaults();
    saveSettings();
    
    loadPlacementOptions();                 // Загрузить опции размещения часов/календаря/температуры в соотвествии с текущим размером часов (большие / малые)

    DEBUGLN();
  }  
}

void loadPlacementOptions() {

    uint8_t value = getCalendarYearWidth();
    calendarYearWidth = value & 0x0F;
    calendarYearLines = (value & 0xF0) >> 4;
    if (!(calendarYearWidth == 0 || calendarYearWidth == 2 || calendarYearWidth == 4)) calendarYearWidth = 2;
    if (calendarYearLines < 1) calendarYearLines = 1;
    if (calendarYearLines > 2) calendarYearLines = 2;

    clockDotWidth         =  getClockDotWidth();
    clockDotSpace         =  getClockDotSpace();
    clock_show_variant    =  getClockShowVariant();
    clock_show_alignment  =  getClockShowAlignment();
    hide_on_text_running  =  getHideOnTextRunning();
    clockOffsetX          =  getClockOffsetX();
    clockOffsetY          =  getClockOffsetY();
    tempOffsetX           =  getTempOffsetX();
    tempOffsetY           =  getTempOffsetY();
    calendarOffsetX       =  getCalendarOffsetX();
    calendarOffsetY       =  getCalendarOffsetY();
    textOffsetY           =  getTextOffsetY();
    
    clock_cycle_T1        =  getClockCycleT1();
    clock_cycle_T2        =  getClockCycleT2();
    clock_cycle_F1        =  getClockCycleF1();
    clock_cycle_F2        =  getClockCycleF2();

}

void clearEEPROM() {
  for (uint16_t addr = 1; addr < EEPROM_MAX; addr++) {
    EEPROM.write(addr, 0);
  }
}

void saveDefaults() {

  putMatrixMapWidth(mapWIDTH);
  putMatrixMapHeight(mapHEIGHT);
  
  putMatrixSegmentWidth(sWIDTH);
  putMatrixSegmentHeight(sHEIGHT);
  putMatrixSegmentType(sMATRIX_TYPE);
  putMatrixSegmentAngle(sCONNECTION_ANGLE);
  putMatrixSegmentDirection(sSTRIP_DIRECTION);

  putMetaMatrixWidth(mWIDTH);
  putMetaMatrixHeight(mHEIGHT);
  putMetaMatrixType(mTYPE);
  putMetaMatrixAngle(mANGLE);
  putMetaMatrixDirection(mDIRECTION);

  putMaxBrightness(deviceBrightness);

  putAutoplayTime(autoplayTime / 1000L);
  uint32_t _idleTime = idleTime / 60L / 1000L;
  putIdleTime(_idleTime > 255 ? 255 : _idleTime);

  putUseNtp(useNtp);
  putTimeZone(TZONE);
  putClockOverlayEnabled(clockOverlayEnabled);
  putTextOverlayEnabled(textOverlayEnabled);
  putTime12(time_h12);
  putSmallFontType(use_round_3x5 ? 1 : 0);

  putAutoplay(manualMode);

  // Смещения часов / календаря / температуры нет ( в объявлении переменные инициализируются нулями)
  putClockOffsetX(clockOffsetX);
  putClockOffsetY(clockOffsetY);
  putTempOffsetX(tempOffsetX);
  putTempOffsetY(tempOffsetY);
  putCalendarOffsetX(calendarOffsetX);
  putCalendarOffsetY(calendarOffsetY);
  putTextOffsetY(textOffsetY);

  // Расположение часов в зависимости от размера матрицы
  
  clock_cycle_T1        = 30;
  clock_cycle_T2        =  5;
  clock_show_alignment  =  0;                     // Все объекты выравниваются по центру их блоков.
  hide_on_text_running  =  7;                     // Скрывать часы / температуру / календарь при показе бегущей строки

  clockDotWidth         = pWIDTH <= 25 ? 1 : 2;   // Ширина разделительных точек в больших часах 1 или 2, если позволяет ширина маnрицы
  clockDotSpace         = pWIDTH > 25;            // Точки в больших часах отделены от цифр пробелом (если позволяет ширина матрицы)
  calendarYearWidth = 2;                          // Отображение года в календаре - 0/2/4 цифры
  calendarYearLines = 2;                          // Отображение года в календаре - 1/2 строки

  CLOCK_ORIENT          =  0;

  // При сохранении     начальных хначений размещения часов / календаря / температуры сохранение в ячейку идеит в зависимости от текущего значения CLOCK_SIZE
  // получаемого через функцию getClockSizeType(), которая в свою очередть базируется на текущем значении ширины / высоты матрицы. 
  // То есть, если даже установить CLOCK_SIZE = 2 (большие часы), но размеры матрицы позволяют только малые часы - функция getClockSizeType() вернет "малые часы" и сохранение 
  // значения будет не в ту ячейку. Чтобы избежать такого поведения - временно меняем значение шрины/высоты матрицы так, чтобы в нее помещались и большие и малые часы, 
  // предварительно сохранив реальные значения. После сохранения - восстановим реальные размеры матрицы
  uint8_t saveWIDTH     = pWIDTH;  pWIDTH  = 32;
  uint8_t saveHEIGHT    = pHEIGHT; pHEIGHT = 32;
  
  // ------------------------------------------------------------

  // По умолчанию   подоазумеаем что   доступны и часы и календарь в две строки
                                      
  // Малые часы.
  CLOCK_SIZE            = 1;
  clock_cycle_F1        = 165;    // T1 - чаcы+температура T2 - календарь T3 - чаcы+температура T4 - календарь
  clock_cycle_F2        = 5;
  clock_show_variant    = 0;

  // Если высота малых часов не позволяет размещать температуру ПОД часами - но позволяет размещать ЗА часами - инициировать такое размещение
  if (saveHEIGHT < 11 && saveWIDTH >= 30)  {
    clock_show_variant  = 4;      // Календарь в позиции часов, температура справа от часов
  } else

  // Если высота малых часов не позволяет размещать температуру ПОД часами, для температуры ЗА часами тоже нет места -
  if (saveHEIGHT < 11 || saveWIDTH < 15)  {
    clock_show_variant  = 0;      // Календарь в позиции часов, температура сама по себе
    clock_cycle_F1      = 133;    // T1 - чаcы T2 - температура T3 - чаcы T4 - календарь
    clock_cycle_F2      = 2;
  }

  putClockCycleF1(clock_cycle_F1);
  putClockCycleF2(clock_cycle_F2);
  putClockShowVariant(clock_show_variant);
  putClockShowAlignment(clock_show_alignment);
  putHideOnTextRunning(hide_on_text_running);
  putCalendarYearWidth((calendarYearLines << 4) | calendarYearWidth);

  // ------------------------------------------------------------

  // Большие часы
  CLOCK_SIZE            = 2;
  clock_cycle_F1        = 165;     // T1 - чаcы+температура T2 - календарь T3 - чаcы+температура T4 - календарь
  clock_cycle_F2        = 5;
  clock_show_variant    = 0;

  // Если высота больших часов не позволяет размещать температуру под часами - но позволяет размещать за часами - инициировать таекое размещение
  if (saveHEIGHT < 11 && saveWIDTH >= 43)  {
    clock_show_variant  =  4;      // Календарь в позиции часов, температура справа от часов
  } else

  // Вывод часов 5x7 - только в вертикальном формате.
  if (saveHEIGHT < 15 || saveWIDTH < 23)  {
    clock_show_variant  = 0;      // Календарь в позиции часов, температура сама по себе
    clock_cycle_F1      = 133;    // T1 - чаcы T2 - температура T3 - чаcы T4 - календарь
    clock_cycle_F2      = 2;
  }

  putClockCycleF1(clock_cycle_F1);
  putClockCycleF2(clock_cycle_F2);
  putClockShowVariant(clock_show_variant);
  putClockShowAlignment(clock_show_alignment);
  putHideOnTextRunning(hide_on_text_running);
  putCalendarYearWidth((calendarYearLines << 4) | calendarYearWidth);

  // ------------------------------------------------------------

  // Размер часов - автоб восстановить реальные значения ширины и высоты матрицы
  CLOCK_SIZE = 0;
  pWIDTH  = saveWIDTH;
  pHEIGHT = saveHEIGHT;

  putClockDotWidth(clockDotWidth);
  putClockDotSpace(clockDotSpace);

  putClockCycleT1(clock_cycle_T1);
  putClockCycleT2(clock_cycle_T2);  

  putClockSize(CLOCK_SIZE);
  putClockOrientation(CLOCK_ORIENT);

  putNightClockColor(nightClockColor);  // Цвет ночных часов: 0 - R; 1 - G; 2 - B; 3 - C; 4 - M; 5 - Y;
  putNightClockBrightness(nightClockBrightness);
  
  putTurnOffClockOnLampOff(needTurnOffClock);

  // ------------------------------------------------------------

  putPowerLimit(CURRENT_LIMIT);
  putTextInterval(TEXT_INTERVAL);
  
  putRandomMode(useRandomSequence);
  
  putAlarmParams(alarmWeekDay,dawnDuration,alarmEffect,alarmDuration);
  #if (USE_MP3 == 1)
    putAlarmSounds(useAlarmSound, maxAlarmVolume, alarmSound, dawnSound);
  #endif

  for (uint8_t i=0; i<7; i++) {
    putAlarmTime(i+1, alarmHour[i], alarmMinute[i]);
  }

  // Настройки по умолчанию для эффектов
  for (uint8_t i = 0; i < MAX_EFFECT; i++) {
    putEffectParams(i, effectSpeed[i], true, true, effectScaleParam[i], effectScaleParam2[i], effectContrast[i], i);
  }

  // Специальные настройки отдельных эффектов
    
  putEffectTextOverlayUsage(MC_CLOCK, false); 
  putEffectTextOverlayUsage(MC_MAZE, false);
  putEffectTextOverlayUsage(MC_SNAKE, false);
  putEffectTextOverlayUsage(MC_TETRIS, false);
  putEffectTextOverlayUsage(MC_ARKANOID, false);
  putEffectTextOverlayUsage(MC_WEATHER, false);
  putEffectTextOverlayUsage(MC_IMAGE, false);
  putEffectTextOverlayUsage(MC_SLIDE, false);
  putEffectTextOverlayUsage(MC_PRIZMATA2, false);
  putEffectTextOverlayUsage(MC_SHADOWS2, false);

  putEffectClockOverlayUsage(MC_CLOCK, false);
  putEffectClockOverlayUsage(MC_MAZE, false);
  putEffectClockOverlayUsage(MC_SNAKE, false);
  putEffectClockOverlayUsage(MC_TETRIS, false);
  putEffectClockOverlayUsage(MC_ARKANOID, false);
  putEffectClockOverlayUsage(MC_WEATHER, false);
  putEffectClockOverlayUsage(MC_IMAGE, false);
  putEffectClockOverlayUsage(MC_SLIDE, false);
  putEffectClockOverlayUsage(MC_PRIZMATA2, false);
  putEffectClockOverlayUsage(MC_SHADOWS2, false);

  putClockScrollSpeed(255);
  putTextScrollSpeed(186);
  
  putScaleForEffect(MC_FIRE, 0);            // Огонь красного цвета
  putScaleForEffect2(MC_PAINTBALL, 1);      // Использовать сегменты для эффекта Пэйнтбол на широких матрицах
  putScaleForEffect2(MC_SWIRL, 1);          // Использовать сегменты для эффекта Водоворот на широких матрицах
  putScaleForEffect2(MC_RAINBOW, 0);        // Использовать рандомный выбор эффекта радуга 0 - random; 1 - диагональная; 2 - горизонтальная; 3 - вертикальная; 4 - вращающаяся  

  uint8_t ball_size = minDim / 4;
  if (ball_size > 5) ball_size = 5;
  putScaleForEffect(MC_BALL, ball_size);    // Размер кубика по умолчанию
  
  putGlobalColor(globalColor);              // Цвет панели в режиме "Лампа"
  putGlobalClockColor(globalClockColor);    // Цвет часов в режиме "Монохром" 
  putGlobalTextColor(globalTextColor);      // Цвет текста в режиме "Монохром"

  putUseSoftAP(useSoftAP);
  putUseDHCP(false);

  #if (USE_E131 == 1)
    workMode = STANDALONE;                  // По умолчанию - самостоятельный режим работы
    syncMode = LOGIC;                       // По умолчанию - размещение данных в логическом порядке - левый верхний угол, далее вправо и вниз.
    syncGroup = 0;
    putSyncWorkMode(workMode);
    putSyncDataMode(syncMode);
    putSyncGroup(syncGroup);
  #endif

  putSoftAPName(DEFAULT_AP_NAME);
  putSoftAPPass(DEFAULT_AP_PASS);
  putSsid(NETWORK_SSID);
  putPass(NETWORK_PASS);
  putNtpServer(DEFAULT_NTP_SERVER);

  putAM1hour(AM1_hour);                 // Режим 1 по времени - часы
  putAM1minute(AM1_minute);             // Режим 1 по времени - минуты
  putAM1effect(AM1_effect_id);          // Режим 1 по времени - действие: -3 - выключено (не используется); -2 - выключить матрицу (черный экран); -1 - огонь, 0 - случайный, 1 и далее - эффект EFFECT_LIST
  putAM2hour(AM2_hour);                 // Режим 2 по времени - часы
  putAM2minute(AM2_minute);             // Режим 2 по времени - минуты
  putAM2effect(AM2_effect_id);          // Режим 2 по времени - действие: -3 - выключено (не используется); -2 - выключить матрицу (черный экран); -1 - огонь, 0 - случайный, 1 и далее - эффект EFFECT_LIST
  putAM3hour(AM3_hour);                 // Режим 3 по времени - часы
  putAM3minute(AM3_minute);             // Режим 3 по времени - минуты
  putAM3effect(AM3_effect_id);          // Режим 3 по времени - действие: -3 - выключено (не используется); -2 - выключить матрицу (черный экран); -1 - огонь, 0 - случайный, 1 и далее - эффект EFFECT_LIST
  putAM4hour(AM4_hour);                 // Режим 4 по времени - часы
  putAM4minute(AM4_minute);             // Режим 4 по времени - минуты
  putAM4effect(AM4_effect_id);          // Режим 4 по времени - действие: -3 - выключено (не используется); -2 - выключить матрицу (черный экран); -1 - огонь, 0 - случайный, 1 и далее - эффект EFFECT_LIST
  putAM5effect(dawn_effect_id);         // Режим по времени "Рассвет" - действие: -3 - выключено (не используется); -2 - выключить матрицу (черный экран); -1 - огонь, 0 - случайный, 1 и далее - эффект EFFECT_LIST
  putAM6effect(dusk_effect_id);         // Режим по времени "Закат"   - действие: -3 - выключено (не используется); -2 - выключить матрицу (черный экран); -1 - огонь, 0 - случайный, 1 и далее - эффект EFFECT_LIST

  #if (USE_WEATHER == 1)       
    putUseWeather(useWeather);
    putWeatherRegion(regionID);
    putWeatherRegion2(regionID2);
    putWeatherInterval(SYNC_WEATHER_PERIOD);
    putUseTemperatureColor(useTemperatureColor);
    putIsFarenheit(isFarenheit);
    putShowTempProps((showTempDegree ? 0x02 : 0x00) | (showTempLetter ? 0x01 : 0x00));
    putShowTempTextProps((showTempTextDegree ? 0x02 : 0x00) | (showTempTextLetter ? 0x01 : 0x00));
  #endif
       
  putStaticIP(IP_STA[0], IP_STA[1], IP_STA[2], IP_STA[3]);
  putUseDHCP(IP_STA[0] + IP_STA[1] + IP_STA[2] + IP_STA[3] == 0);

  // Инициализировать строку использования эффектов - по умолчанию - все, порядок - в порядке определения)
  effect_order = String(IDX_LINE).substring(0,MAX_EFFECT);
  saveEffectOrder();

  putCurrentSpecMode(-1);               // Текущий спец-режим - это не спец-режим
  putCurrentManualMode(-1);             // Текущий вручную включенный режим
}

void saveSettings() {

  saveSettingsTimer.reset();
  if (!eepromModified) return;
  
  // Поставить отметку, что EEPROM инициализировано параметрами эффектов
  EEPROMwrite(0, EEPROM_OK);
  
  EEPROM.commit();
  DEBUGLN(F("Настройки сохранены в EEPROM"));
  eepromModified = false;
}

// Загрузка параметров приложения, описывающих типы оборудования в переменные
void loadWiring() {
  // Пины подключения в переменные загружать не нужно - они будут считываться перед использованием, т.к их применение происходит один раз при инициализации
  // Загрузка переменных, описывающих поведение "перефирийных" устройств в зависимости от их значения - эти значения используются во многих местах программы
  vDEVICE_TYPE = getDeviceType();              // DEVICE_TYPE
  vBUTTON_TYPE = getButtonType();              // BUTTON_TYPE
  vWAIT_PLAY_FINISHED = getWaitPlayFinished(); // WAIT_PLAY_FINISHED
  vREPEAT_PLAY = getRepeatPlay();              // REPEAT_PLAY  

  vPOWER_ON = getPowerActiveLevel();           // POWER_ON 
  vPOWER_OFF = vPOWER_ON == HIGH ? LOW : HIGH; // POWER_OFF
  vPOWER_PIN = getPowerPin();                  // POWER_PIN

  vALARM_ON = getAlarmActiveLevel();           // ALARM_ON 
  vALARM_OFF = vALARM_ON == HIGH ? LOW : HIGH; // ALARM_OFF
  vALARM_PIN = getAlarmPin();                  // ALARM_PIN

  vAUX_ON = getAuxActiveLevel();               // AUX_ON 
  vAUX_OFF = vAUX_ON == HIGH ? LOW : HIGH;     // AUX_OFF
  vAUX_PIN = getAuxPin();                      // AUX_PIN  
}

// Инициализация параметров приложения, описывающих типы оборудования и пины подключения
void initializeWiring() { 
  
  // Инициализировать начальные значения "переферийных" устройств и пины подключения из define определенных пользователем для оборудования 
  putDebugSerialEnable(DEBUG_SERIAL == 1);
  putDeviceType(DEVICE_TYPE);
  putButtonType(BUTTON_TYPE);
  putWaitPlayFinished(WAIT_PLAY_FINISHED == 1);
  putRepeatPlay(REPEAT_PLAY == 1);

  // Инициализировать пины подключения
  // По умолчанию - используется единственная линия, все светодиоды назначены на нее
  // Линии вывода сигнала на светодиод 2..4 - отключены.
  putLedLineUsage(1, true);                         // Линия 1 - включена
  putLedLinePin(1, LED_PIN);                        // Вывод назначен на LED_PIN
  putLedLineStartIndex(1, 0);                       // Начало вывода на ленту - с 0 индекса светодиодов в массиве  
  putLedLineLength(1, NUM_LEDS);                    // реальное значение будет инициализировано в setup() после того как из настроек будет считан текущий размер матрицы
  putLedLineRGB(1, 0);                              // цветовой порядок
  
  putLedLineUsage(2, false);                        // Линия 2 - отключена
  putLedLinePin(2, -1);                             // Вывод не назначен ни на один из пинов
  putLedLineStartIndex(2, 0);                       // Индекс начала вывода - N/A   
  putLedLineLength(2, NUM_LEDS);                    // Длина сегмента - N/A
  putLedLineRGB(2, 0);                              // цветовой порядок
 
  putLedLineUsage(3, false);                        // Линия 3 - отключена
  putLedLinePin(3, -1);                             // Вывод не назначен ни на один из пинов
  putLedLineStartIndex(3, 0);                       // Индекс начала вывода - N/A   
  putLedLineLength(3, NUM_LEDS);                    // Длина сегмента - N/A
  putLedLineRGB(3, 0);                              // цветовой порядок
  
  putLedLineUsage(4, false);                        // Линия 4 - отключена
  putLedLinePin(4, -1);                             // Вывод не назначен ни на один из пинов
  putLedLineStartIndex(4, 0);                       // Индекс начала вывода - N/A   
  putLedLineLength(4, NUM_LEDS);                    // Длина сегмента - N/A
  putLedLineRGB(4, 0);                              // цветовой порядок

  #if (USE_BUTTON == 1)
    putButtonPin(PIN_BTN);                          // Пин подключения кнопки
  #else
    putButtonPin(-1);                               // Пин подключения кнопки
  #endif

  #if (USE_MP3 == 1)
    putDFPlayerSTXPin(STX);                         // Пин TX для отправки на DFPlayer - подключено к RX плеера
    putDFPlayerSRXPin(SRX);                         // Пин RX для получения данных с DFPlayer - подключено к TX плеера
  #else
    putDFPlayerSTXPin(-1);                          // Пин TX для отправки на DFPlayer - подключено к RX плеера
    putDFPlayerSRXPin(-1);                          // Пин RX для получения данных с DFPlayer - подключено к TX плеера
  #endif
  
  #if (USE_TM1637 == 1)
    putTM1637DIOPin(DIO);                           // Пин DIO индикатора TM1637
    putTM1637CLKPin(CLK);                           // Пин CLK индикатора TM1637
  #else
    putTM1637DIOPin(-1);                            // Пин DIO индикатора TM1637
    putTM1637CLKPin(-1);                            // Пин CLK индикатора TM1637
  #endif


  #if (USE_POWER == 1)
    putPowerPin(POWER_PIN);                         // Пин подключения управления питанием матрицы
    putPowerActiveLevel(POWER_ON);                  // Активный уровень HIGH (1) / LOW (0) для POWER_ON; POWER_OFF - противоположное значение 
  #else
    putPowerPin(-1);                                // Пин подключения управления питанием матрицы
  #endif

  
  #if (USE_ALARM == 1)
    putAlarmPin(ALARM_PIN);                         // Пин подключения управления питанием - линия будильника
    putAlarmActiveLevel(ALARM_ON);                  // Активный уровень HIGH (1) / LOW (0) для ALARM_ON; ALARM_OFF - противоположное значение 
  #else
    putAlarmPin(-1);                                // Пин подключения управления питанием - линия будильника
  #endif

  #if (USE_ALARM == 1)
    putAuxPin(AUX_PIN);                             // Пин подключения управления питанием - линия дополнительного управления
    putAuxActiveLevel(AUX_ON);                      // Активный уровень HIGH (1) / LOW (0) для AUX_ON; AUX_OFF - противоположное значение 
    putAuxLineState(false);                         // Начальное состояние дополнительной линии управления питанием - выключено  
    putAuxLineModes(0);                             // Использование дополнительной линии управления питанием - выключено для всех режимов  
  #else
    putAuxPin(-1);                                  // Пин подключения управления питанием - линия дополнительного управления
  #endif  
}

void putMatrixMapWidth(uint8_t width) {
  if (width < 8) width = 8;
  if (width > 128) width = 128;
  if (width != getMatrixMapWidth()) {
    EEPROMwrite(94, width);
  }  
}

uint8_t getMatrixMapWidth() {
  uint8_t value = EEPROMread(94);
  if (value < 8) value = 8;
  if (value > 128) value = 128;
  return value;
}

void putMatrixMapHeight(uint8_t height) {
  if (height < 8) height = 8;
  if (height > 128) height = 128;  
  if (height != getMatrixMapHeight()) {
    EEPROMwrite(95, height);
  }  
}
uint8_t getMatrixMapHeight() {
  uint8_t value = EEPROMread(95);
  if (value < 8) value = 8;
  if (value > 128) value = 128;
  return value;
}

void putMatrixSegmentWidth(uint8_t width) {
  if (width == 0 && width > 128) width = 16;
  if (width != getMatrixSegmentWidth()) {
    EEPROMwrite(80, width);
  }  
}
uint8_t getMatrixSegmentWidth() {
  uint8_t value = EEPROMread(80);
  if (value == 0 || value > 128) value = 16;
  return value;
}

void putMatrixSegmentHeight(uint8_t height) {
  if (height == 0 || height > 128) height = 16;
  if (height != getMatrixSegmentHeight()) {
    EEPROMwrite(81, height);
  }  
}
uint8_t getMatrixSegmentHeight() {
  uint8_t value = EEPROMread(81);
  if (value == 0 || value > 128) value = 16;
  return value;
}

void putMatrixSegmentType(uint8_t type) {
  if (type > 2) type = 0;
  if (type != getMatrixSegmentType()) {
    EEPROMwrite(82, type);
  }  
}
uint8_t getMatrixSegmentType() {
  uint8_t value = EEPROMread(82);
  if (value > 2) value = 0;
  return value;  
}

void putMatrixSegmentAngle(uint8_t angle) {
  if (angle > 3) angle = 0;
  if (angle != getMatrixSegmentAngle()) {
    EEPROMwrite(83, angle);
  }  
}
uint8_t getMatrixSegmentAngle() {
  uint8_t value = EEPROMread(83);
  if (value > 3) value = 0;
  return value;  
}

void putMatrixSegmentDirection(uint8_t dir) {
  if (dir > 3) dir = 0;
  if (dir != getMatrixSegmentDirection()) {
    EEPROMwrite(84, dir);
  }  
}
uint8_t getMatrixSegmentDirection() {
  uint8_t value = EEPROMread(84);
  if (value > 3) value = 0;
  return value;  
}

void putMetaMatrixWidth(uint8_t width) {
  if (width == 0 || width > 15) width = 1;
  if (width != getMetaMatrixWidth()) {
    EEPROMwrite(85, width);
  }  
}
uint8_t getMetaMatrixWidth() {
  uint8_t value = EEPROMread(85);
  if (value == 0 || value > 15) value = 1;
  return value;
}

void putMetaMatrixHeight(uint8_t height) {
  if (height == 0 || height > 15) height = 1;
  if (height != getMetaMatrixHeight()) {
    EEPROMwrite(86, height);
  }  
}
uint8_t getMetaMatrixHeight() {
  uint8_t value = EEPROMread(86);
  if (value == 0 || value > 15) value = 1;
  return value;
}

void putMetaMatrixType(uint8_t type) {
  if (type > 1) type = 0;
  if (type != getMetaMatrixType()) {
    EEPROMwrite(87, type);
  }  
}
uint8_t getMetaMatrixType() {
  uint8_t value = EEPROMread(87);
  if (value > 1) value = 0;
  return value;
}

void putMetaMatrixAngle(uint8_t angle) {
  if (angle > 3) angle = 0;
  if (angle != getMetaMatrixAngle()) {
    EEPROMwrite(88, angle);
  }  
}
uint8_t getMetaMatrixAngle() {
  uint8_t value = EEPROMread(88);
  if (value > 3) value = 0;
  return value;
}

void putMetaMatrixDirection(uint8_t dir) {
  if (dir > 3) dir = 0;
  if (dir != getMetaMatrixDirection()) {
    EEPROMwrite(89, dir);
  }  
}
uint8_t getMetaMatrixDirection() {
  uint8_t value = EEPROMread(89);
  if (value > 3) value = 0;
  return value;
}

void putEffectParams(uint8_t effect, uint8_t spd, bool use_text_overlay, bool use_clock_overlay, uint8_t value1, uint8_t value2, uint8_t contrast, uint8_t order) {
  if (effect >= MAX_EFFECT) return;
  uint8_t value = 0;

  if (use_text_overlay)  value |= 0x01;
  if (use_clock_overlay) value |= 0x02;
  
  EEPROMwrite(EFFECT_EEPROM + effect*10, constrain(map(spd, D_EFFECT_SPEED_MIN, D_EFFECT_SPEED_MAX, 0, 255), 0, 255));        // Скорость эффекта  
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 1, value);                                                                          // b0 - Вкл/выкл оверлей текста; ; b1 - Вкл/выкл оверлей часов   
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 2, value1);                                                                         // Параметр эффекта #1 
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 3, value2);                                                                         // Параметр эффекта #2 
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 4, contrast);                                                                       // Контраст эффекта 
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 5, order);                                                                          // Номер в порядке воспроизведения эффектов. 255 - эффект не используется
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 6, 0);                                                                              // Зарезервировано
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 7, 0);                                                                              // Зарезервировано
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 8, 0);                                                                              // Зарезервировано
  EEPROMwrite(EFFECT_EEPROM + effect*10 + 9, 0);                                                                              // Зарезервировано
  effectScaleParam[effect] = value1;
  effectScaleParam2[effect] = value2;
  effectContrast[effect] = contrast;
  effectSpeed[effect] = spd;
}

bool getEffectTextOverlayUsage(uint8_t effect) {
  if (effect >= MAX_EFFECT) return false;
  uint8_t value = EEPROMread(EFFECT_EEPROM + effect*10 + 1);
  return (value & 0x01) != 0;                                            // b1 - использовать в эффекте бегущую строку поверх эффекта
}

void putEffectTextOverlayUsage(uint8_t effect, bool use) {
  if (effect >= MAX_EFFECT) return;
  uint8_t value = EEPROMread(EFFECT_EEPROM + effect*10 + 1);
  uint8_t new_value = use ? (value | 0x01) : (value & ~0x01);
  if (value != new_value) {
    EEPROMwrite(EFFECT_EEPROM + effect*10 + 1, new_value);               // b1 - использовать в эффекте бегущую строку поверх эффекта
  }
}

bool getEffectClockOverlayUsage(uint8_t effect) {
  if (effect >= MAX_EFFECT) return false;
  uint8_t value = EEPROMread(EFFECT_EEPROM + effect*10 + 1);
  return (value & 0x02) != 0;                                            // b2 - использовать в эффекте часы поверх эффекта
}

void putEffectClockOverlayUsage(uint8_t effect, bool use) {
  if (effect >= MAX_EFFECT) return;
  uint8_t value = EEPROMread(EFFECT_EEPROM + effect*10 + 1);
  uint8_t new_value = use ? (value | 0x02) : (value & ~0x02);
  if (value != new_value) {
    EEPROMwrite(EFFECT_EEPROM + effect*10 + 1, new_value);               // b2 - использовать в эффекте часы поверх эффекта
  }
}

void putScaleForEffect(uint8_t effect, uint8_t value) {
  if (effect >= MAX_EFFECT) return;
  if (value != getScaleForEffect(effect)) {
    EEPROMwrite(EFFECT_EEPROM + effect*10 + 2, value);
    effectScaleParam[effect] = value;
  }  
}

uint8_t getScaleForEffect(uint8_t effect) {
  if (effect >= MAX_EFFECT) return 0;
  uint8_t value = EEPROMread(EFFECT_EEPROM + effect*10 + 2);
  effectScaleParam[effect] = value;
  return value;
}

void putScaleForEffect2(uint8_t effect, uint8_t value) {
  if (effect >= MAX_EFFECT) return;
  if (value != getScaleForEffect2(effect)) {
    EEPROMwrite(EFFECT_EEPROM + effect*10 + 3, value);
    effectScaleParam2[effect] = value;
  }  
}

uint8_t getScaleForEffect2(uint8_t effect) {
  if (effect >= MAX_EFFECT) return 0;
  uint8_t value = EEPROMread(EFFECT_EEPROM + effect*10 + 3);
  effectScaleParam2[effect] = value;
  return value;
}

uint8_t getEffectContrast(uint8_t effect) {
  if (effect >= MAX_EFFECT) return 0;
  uint8_t contrast = constrain(EEPROMread(EFFECT_EEPROM + effect*10 + 4),10,255);
  effectContrast[effect] = contrast;
  return contrast;
}

void putEffectContrast(uint8_t effect, uint8_t contrast) {
  if (effect >= MAX_EFFECT) return;
  if (contrast != getEffectContrast(effect)) {
    effectContrast[effect] = contrast < 10 ? 10 : contrast;
    EEPROMwrite(EFFECT_EEPROM + effect*10 + 4, effectContrast[effect]);
  }  
}

void putEffectSpeed(uint8_t effect, uint8_t speed) {
  if (effect >= MAX_EFFECT) return;
  if (speed != getEffectSpeed(effect)) {
    effectSpeed[effect] = constrain(map(speed, D_EFFECT_SPEED_MIN, D_EFFECT_SPEED_MAX, 0, 255), 0, 255);
    EEPROMwrite(EFFECT_EEPROM + effect*10, effectSpeed[effect]);        // Скорость эффекта    
  }
}

uint8_t getEffectSpeed(uint8_t effect) {
  if (effect >= MAX_EFFECT) return 0;
  uint8_t speed = map8(EEPROMread(EFFECT_EEPROM + effect*10),D_EFFECT_SPEED_MIN,D_EFFECT_SPEED_MAX);  
  effectSpeed[effect] = speed; 
  return speed; 
}

uint8_t getEffectOrder(uint8_t effect) {
  if (effect >= MAX_EFFECT) return 0;
  uint8_t order = EEPROMread(EFFECT_EEPROM + effect*10 + 5);
  if (order >= MAX_EFFECT) order = 255;
  return order;
}

void putEffectOrder(uint8_t effect, uint8_t order) {
  if (effect >= MAX_EFFECT) return;
  if (order >= MAX_EFFECT) order = 255;
  if (order != getEffectOrder(effect)) {
    EEPROMwrite(EFFECT_EEPROM + effect*10 + 5, order);
  }  
}

uint8_t getMaxBrightness() {
  return EEPROMread(1);
}

void putMaxBrightness(uint8_t brightness) {
  if (brightness != getMaxBrightness()) {
    EEPROMwrite(1, brightness);
  }
}

void putAutoplay(bool value) {
  if (value != getAutoplay()) {
    EEPROMwrite(2, value ? 1 : 0);
  }  
}

bool getAutoplay() {
  return EEPROMread(2) != 0;
}

void putAutoplayTime(uint32_t value) {
  if (value != getAutoplayTime()) {
    int32_t _value = value / 1000UL;    
    EEPROMwrite(3, _value > 255 ? 255 : _value);
  }
}

uint32_t getAutoplayTime() {
  uint32_t time = EEPROMread(3) * 1000UL;  
  if (time == 0) time = ((uint32_t)AUTOPLAY_PERIOD * 1000UL);
  return time;
}

void putIdleTime(uint32_t value) {
  if (value != getIdleTime()) {
    int32_t _value = value / 60 / 1000UL;
    EEPROMwrite(4, _value > 255 ? 255 : _value);
  }
}

uint16_t getUpTimeSendInterval() {
  return EEPROM_int_read(280);
}

void putUpTimeSendInterval(uint16_t value) {
  if (value != getUpTimeSendInterval()) {
    EEPROM_int_write(280, value);
  }
}

uint32_t getIdleTime() {
  uint32_t time = EEPROMread(4) * 60 * 1000UL;  
  return time;
}

bool getClockOverlayEnabled() {
  return EEPROMread(30) != 0;
}

void putClockOverlayEnabled(bool enabled) {
  if (enabled != getClockOverlayEnabled()) {
    EEPROMwrite(30, enabled ? 1 : 0);
  }
}

bool getTextOverlayEnabled() {
  return (EEPROMread(173) != 0);
}

void putTextOverlayEnabled(bool enabled) {
  if (enabled != getTextOverlayEnabled()) {
    EEPROMwrite(173, enabled ? 1 : 0);
  }
}

void putUseNtp(bool value) {
  if (value != getUseNtp()) {
    EEPROMwrite(5, value);
  }
}

bool getUseNtp() {
  return EEPROMread(5) != 0;
}

void putGameButtonSpeed(int8_t value) {
  if (value < 10) value = 10;
  if (value > 100) value = 100;

  if (value != getGameButtonSpeed()) {
    EEPROMwrite(97, (uint8_t)value);
  }
}

uint8_t getGameButtonSpeed() {
  uint8_t val = EEPROMread(97);
  if (val < 10) val = 10;
  if (val > 100) val = 100;
  return val;
}

String getTimeZone() {
  return EEPROM_string_read(351, 48);
}

void putTimeZone(const String& tz) {
  if (tz != getTimeZone()) {
    EEPROM_string_write(351, tz, 48);
  }
}

bool getTurnOffClockOnLampOff() {
  return EEPROMread(9) != 0;
}

void putTurnOffClockOnLampOff(bool flag) {
  if (flag != getTurnOffClockOnLampOff()) {
    EEPROMwrite(9, flag ? 1 : 0);
  }  
}

uint8_t getClockOrientation() {  
  uint8_t val = EEPROMread(15) != 0 ? 1 : 0;
  return val;
}

void putClockOrientation(uint8_t orientation) {
  if (orientation != getClockOrientation()) {
    EEPROMwrite(15, orientation != 0 ? 1 : 0);
  }
}

// Время в секундах фазы отображения часов T1/T3
uint8_t getClockCycleT1() {
  return EEPROMread(17);
}

// Время в секундах фазы отображения часов T1/T3
void putClockCycleT1(uint8_t value) {
  if (value != getClockCycleT1()) {
    EEPROMwrite(17, value);
  }
}

// Время в секундах фазы отображения часов T2/T4
uint8_t getClockCycleT2() {
  return EEPROMread(18);
}

// Время в секундах фазы отображения часов T2/T4
void putClockCycleT2(uint8_t value) {
  if (value != getClockCycleT2()) {
    EEPROMwrite(18, value);
  }
}

// Видимость часов/календаря в фазах T1-T4
uint8_t getClockCycleF1() {
  uint16_t addr = getClockSizeType() == 1 ? 266 : 267;
  return EEPROMread(addr);
}

// Видимость часов/календаря в фазах T1-T4
void putClockCycleF1(uint8_t value) {
  if (value != getClockCycleF1()) {
    uint16_t addr = getClockSizeType() == 1 ? 266 : 267;
    EEPROMwrite(addr, value);
  }
}

// Видимость температуры в фазах T1-T4
uint8_t getClockCycleF2() {
  uint16_t addr = getClockSizeType() == 1 ? 274 : 275;
  return EEPROMread(addr);
}

// Видимость температуры в фазах T1-T4
void putClockCycleF2(uint8_t value) {
  if (value != getClockCycleF2()) {
    uint16_t addr = getClockSizeType() == 1 ? 274 : 275;
    EEPROMwrite(addr, value);
  }
}

uint8_t getClockSize() {
  return EEPROMread(19);
}

void putClockSize(uint8_t c_size) {
  if (c_size != getClockSize()) {
    EEPROMwrite(19, c_size);
  }
}

void putAlarmParams(uint8_t alarmWeekDay, uint8_t dawnDuration, uint8_t alarmEffect, uint8_t alarmDuration) {  
  if (alarmWeekDay != getAlarmWeekDay()) {
    EEPROMwrite(20, alarmWeekDay);
  }
  if (dawnDuration != getDawnDuration()) {
    EEPROMwrite(21, dawnDuration);
  }
  if (alarmEffect != getAlarmEffect()) {
    EEPROMwrite(22, alarmEffect);
  }
  //   24 - Будильник, длительность звука будильника, минут
  if (alarmDuration != getAlarmDuration()) {
    EEPROMwrite(24, alarmDuration);
  }
}

uint8_t getAlarmHour(uint8_t day) { 
  return constrain(EEPROMread(40 + 2 * (day - 1)), 0, 23);
}

uint8_t getAlarmMinute(uint8_t day) { 
  return constrain(EEPROMread(40 + 2 * (day - 1) + 1), 0, 59);
}

void putAlarmTime(uint8_t day, uint8_t hour, uint8_t minute) { 
  if (hour != getAlarmHour(day)) {
    EEPROMwrite(40 + 2 * (day - 1), hour > 23 ? 23 : hour);
  }
  if (minute != getAlarmMinute(day)) {
    EEPROMwrite(40 + 2 * (day - 1) + 1, minute > 59 ? 59 : minute);
  }
}

uint8_t getAlarmWeekDay() { 
  return EEPROMread(20);
}

uint8_t getDawnDuration() { 
  return constrain(EEPROMread(21),1,59);
}

void putAlarmSounds(bool useSound, uint8_t maxVolume, int8_t alarmSound, int8_t dawnSound) {
  //   23 - Будильник звук: вкл/выкл 1 - вкл; 0 -выкл
  //   25 - Будильник, мелодия будильника
  //   26 - Будильник, мелодия рассвета
  //   27 - Будильник, максимальная громкость
  if (alarmEffect != getAlarmEffect()) {
    EEPROMwrite(22, alarmEffect);
  }
  if (useSound != getUseAlarmSound()) {
    EEPROMwrite(23, useSound ? 1 : 0);
  }
  if (alarmSound != getAlarmSound()) {
    EEPROMwrite(25, (uint8_t)alarmSound);
  }
  if (dawnSound != getDawnSound()) {
    EEPROMwrite(26, (uint8_t)dawnSound);
  }
  if (maxVolume != getMaxAlarmVolume()) {
    EEPROMwrite(27, maxVolume);
  }
}

uint8_t getAlarmEffect() { 
  return EEPROMread(22);
}

bool getUseAlarmSound() { 
  return EEPROMread(23) != 0;
}

uint8_t getAlarmDuration() { 
  return constrain(EEPROMread(24),1,10);
}

uint8_t getMaxAlarmVolume() { 
  return constrain(EEPROMread(27),0,30);
}

int8_t getAlarmSound() { 
  return constrain((int8_t)EEPROMread(25),-1,127);
}

int8_t getDawnSound() { 
  return constrain((int8_t)EEPROMread(26),-1,127);
}

bool getUseSoftAP() {
  return EEPROMread(14) != 0;
}

void putUseSoftAP(bool use) {  
  if (use != getUseSoftAP()) {
    EEPROMwrite(14, use ? 1 : 0);
  }
}

String getSoftAPName() {
  return EEPROM_string_read(54, 10);
}

void putSoftAPName(const String& SoftAPName) {
  if (SoftAPName != getSoftAPName()) {
    EEPROM_string_write(54, SoftAPName, 10);
  }
}

String getSoftAPPass() {
  return EEPROM_string_read(64, 16);
}

void putSoftAPPass(const String& SoftAPPass) {
  if (SoftAPPass != getSoftAPPass()) {
    EEPROM_string_write(64, SoftAPPass, 16);
  }
}

// ssid – имя локальной сети в виде символьной строки, которая может содержать до 32 символов
String getSsid() {
  return EEPROM_string_read(400, 32);
}

void putSsid(const String& ssid) {
  if (ssid != getSsid()) {
    EEPROM_string_write(400, ssid, 32);
  }
}

// пароль к локальной сети в виде символьной строки, которая может содержать от 8 до 64 символов
String getPass() {
  return EEPROM_string_read(432, 64);
}

void putPass(const String& pass) {
  if (pass != getPass()) {
    EEPROM_string_write(432, pass, 64);
  }
}

// -----------------------------

String getNtpServer() {
  return EEPROM_string_read(120, 30);
}

void putNtpServer(const String& server) {
  if (server != getNtpServer()) {
    EEPROM_string_write(120, server, 30);
  }
}

void putAM1params(uint8_t hour, uint8_t minute, int8_t effect) { 
  putAM1hour(hour);
  putAM1minute(minute);
  putAM1effect(effect);
}

uint8_t getAM1hour() { 
  uint8_t hour = EEPROMread(33);
  if (hour>23) hour = 0;
  return hour;
}

void putAM1hour(uint8_t hour) {
  if (hour != getAM1hour()) {
    EEPROMwrite(33, hour);
  }
}

uint8_t getAM1minute() {
  uint8_t minute = EEPROMread(34);
  if (minute > 59) minute = 0;
  return minute;
}

void putAM1minute(uint8_t minute) {
  if (minute != getAM1minute()) {
    EEPROMwrite(34, minute);
  }
}

int8_t getAM1effect() {
  int8_t value = (int8_t)EEPROMread(35);
  if (value < -3) value = -3;
  return value;
}

void putAM1effect(int8_t effect) {
  if (effect != getAM1effect()) {
    EEPROMwrite(35, (uint8_t)effect);
  }
}

void putAM2params(uint8_t hour, uint8_t minute, int8_t effect) { 
  putAM2hour(hour);
  putAM2minute(minute);
  putAM2effect(effect);
}

uint8_t getAM2hour() { 
  uint8_t hour = EEPROMread(36);
  if (hour>23) hour = 0;
  return hour;
}

void putAM2hour(uint8_t hour) {
  if (hour != getAM2hour()) {
    EEPROMwrite(36, hour);
  }
}

uint8_t getAM2minute() {
  uint8_t minute = EEPROMread(37);
  if (minute > 59) minute = 0;
  return minute;
}

void putAM2minute(uint8_t minute) {
  if (minute != getAM2minute()) {
    EEPROMwrite(37, minute);
  }
}

int8_t getAM2effect() {
  int8_t value = (int8_t)EEPROMread(38);
  if (value < -3) value = -3;
  return value;
}

void putAM2effect(int8_t effect) {
  if (effect != getAM2effect()) {
    EEPROMwrite(38, (uint8_t)effect);
  }
}

void putAM3params(uint8_t hour, uint8_t minute, int8_t effect) { 
  putAM3hour(hour);
  putAM3minute(minute);
  putAM3effect(effect);
}

uint8_t getAM3hour() { 
  uint8_t hour = EEPROMread(161);
  if (hour>23) hour = 0;
  return hour;
}

void putAM3hour(uint8_t hour) {
  if (hour != getAM3hour()) {
    EEPROMwrite(161, hour);
  }
}

uint8_t getAM3minute() {
  uint8_t minute = EEPROMread(162);
  if (minute > 59) minute = 0;
  return minute;
}

void putAM3minute(uint8_t minute) {
  if (minute != getAM3minute()) {
    EEPROMwrite(162, minute);
  }
}

int8_t getAM3effect() {
  int8_t value = (int8_t)EEPROMread(163);
  if (value < -3) value = -3;
  return value;
}

void putAM3effect(int8_t effect) {
  if (effect != getAM3effect()) {
    EEPROMwrite(163, (uint8_t)effect);
  }
}

void putAM4params(uint8_t hour, uint8_t minute, int8_t effect) { 
  putAM4hour(hour);
  putAM4minute(minute);
  putAM4effect(effect);
}

uint8_t getAM4hour() { 
  uint8_t hour = EEPROMread(164);
  if (hour>23) hour = 0;
  return hour;
}

void putAM4hour(uint8_t hour) {
  if (hour != getAM4hour()) {
    EEPROMwrite(164, hour);
  }
}

uint8_t getAM4minute() {
  uint8_t minute = EEPROMread(165);
  if (minute > 59) minute = 0;
  return minute;
}

void putAM4minute(uint8_t minute) {
  if (minute != getAM4minute()) {
    EEPROMwrite(165, minute);
  }
}

int8_t getAM4effect() {
  int8_t value = (int8_t)EEPROMread(166);
  if (value < -3) value = -3;
  return value;
}

void putAM4effect(int8_t effect) {
  if (effect != getAM4effect()) {
    EEPROMwrite(166, (uint8_t)effect);
  }
}

int8_t getAM5effect() {
  int8_t value = (int8_t)EEPROMread(243);
  if (value < -3) value = -3;
  return value;
}

void putAM5effect(int8_t effect) {
  if (effect != getAM5effect()) {
    EEPROMwrite(243, (uint8_t)effect);
  }
}

int8_t getAM6effect() {
  int8_t value = (int8_t)EEPROMread(248);
  if (value < -3) value = -3;
  return value;
}

void putAM6effect(int8_t effect) {
  if (effect != getAM6effect()) {
    EEPROMwrite(248, (uint8_t)effect);
  }
}

int8_t getCurrentManualMode() {
  return (int8_t)EEPROMread(29);
}

void putCurrentManualMode(int8_t mode) {
  if (mode != getCurrentManualMode()) {
    EEPROMwrite(29, (uint8_t)mode);
  }
}

void getStaticIP() {
  IP_STA[0] = EEPROMread(10);
  IP_STA[1] = EEPROMread(11);
  IP_STA[2] = EEPROMread(12);
  IP_STA[3] = EEPROMread(13);
}

void putStaticIP(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4) {
  EEPROMwrite(10, p1);
  EEPROMwrite(11, p2);
  EEPROMwrite(12, p3);
  EEPROMwrite(13, p4);
}

uint32_t getGlobalColor() {
  uint8_t r,g,b;
  r = EEPROMread(158);
  g = EEPROMread(159);
  b = EEPROMread(160);
  return (uint32_t)r<<16 | (uint32_t)g<<8 | (uint32_t)b;
}

void putGlobalColor(uint32_t color) {
  if (color != getGlobalColor()) {
    CRGB cl = CRGB(color);
    EEPROMwrite(158, cl.r); // R
    EEPROMwrite(159, cl.g); // G
    EEPROMwrite(160, cl.b); // B
  }
}

uint32_t getGlobalClockColor() {
  uint8_t r,g,b;
  r = EEPROMread(152);
  g = EEPROMread(153);
  b = EEPROMread(154);
  return (uint32_t)r<<16 | (uint32_t)g<<8 | (uint32_t)b;
}

void putGlobalClockColor(uint32_t color) {
  if (color != getGlobalClockColor()) {
    CRGB cl = CRGB(color);
    EEPROMwrite(152, cl.r); // R
    EEPROMwrite(153, cl.g); // G
    EEPROMwrite(154, cl.b); // B
  }
}

uint32_t getGlobalTextColor() {
  uint8_t r,g,b;
  r = EEPROMread(155);
  g = EEPROMread(156);
  b = EEPROMread(157);
  return (uint32_t)r<<16 | (uint32_t)g<<8 | (uint32_t)b;
}

void putGlobalTextColor(uint32_t color) {
  if (color != getGlobalTextColor()) {
    CRGB cl = CRGB(color);
    EEPROMwrite(155, cl.r); // R 
    EEPROMwrite(156, cl.g); // G
    EEPROMwrite(157, cl.b); // B
  }
}

int8_t getCurrentSpecMode() {
  return (int8_t)EEPROMread(28);
}

void putCurrentSpecMode(int8_t mode) {
  if (mode != getCurrentSpecMode()) {
    EEPROMwrite(28, (uint8_t)mode);
  }
}

void putRandomMode(bool randomMode) {
  if (randomMode != getRandomMode()) {
    EEPROMwrite(31, randomMode ? 1 : 0);
  }  
}

bool getRandomMode() {
 return EEPROMread(31) != 0;
}

void putPowerLimit(uint16_t limit) {
  if (limit != getPowerLimit()) {
    EEPROM_int_write(150, limit);
  }
}

uint16_t getPowerLimit() {
  uint16_t val = (uint16_t)EEPROM_int_read(150);
  if (val !=0 && val < 1000) val = 1000;
  return val;
}

void putNightClockColor(uint8_t clr) {
  if (clr != getNightClockColor()) {
    EEPROMwrite(39, clr);
  }  
}

uint8_t getNightClockColor() {
  uint8_t clr = EEPROMread(39);
  if (clr > 6) clr=0;
  return clr;
}

// Интервал включения режима бегущей строки
uint16_t getTextInterval() {
  uint16_t val = (uint16_t)EEPROM_int_read(167);
  return val;
}

void putTextInterval(uint16_t interval) {
  if (interval != getTextInterval()) {
    EEPROM_int_write(167, interval);
  }  
}

// Режим цвета часов оверлея X: 0,1,2,3
void putClockColor(uint8_t clr) {
  if (clr != getClockColor()) {
    EEPROMwrite(169, clr);
  }  
}

uint8_t getClockColor() {
  uint8_t clr = EEPROMread(169);
  if (clr > 3) clr=0;
  return clr;
}

// Скорость прокрутки часов
void putClockScrollSpeed(uint8_t clr) {
  if (clr != getClockScrollSpeed()) {
    EEPROMwrite(170, clr);
  }  
}

uint8_t getClockScrollSpeed() {
  uint8_t clr = EEPROMread(170);
  return clr;
}

// Режим цвета оверлея бегущей строки X: 0,1,2,3
void putTextColor(uint8_t clr) {
  if (clr != getTextColor()) {
    EEPROMwrite(171, clr);
  }  
}

uint8_t getTextColor() {
  uint8_t clr = EEPROMread(171);
  if (clr > 2) clr=0;
  return clr;
}

// Скорость прокрутки бегущей строки
void putTextScrollSpeed(uint8_t spd) {
  if (spd != getTextScrollSpeed()) {
    EEPROMwrite(172, spd);
  }  
}

uint8_t getTextScrollSpeed() {
  uint8_t clr = EEPROMread(172);
  return clr;
}

#if (USE_WEATHER == 1)

uint8_t getUseWeather() {
  return EEPROMread(174);
}

void putUseWeather(uint8_t id) {
  if (id != getUseWeather()) {
    EEPROMwrite(174, id);
  }
}

uint8_t getWeatherInterval() {
  uint8_t value = EEPROMread(175);
  if (value < 10) value = 10;
  return value;
}

// Интервал обновления погоды
void putWeatherInterval(uint8_t interval) {
  if (interval != getWeatherInterval()) {
    EEPROMwrite(175, interval);
  }  
}

uint32_t getWeatherRegion() {
  uint32_t region = EEPROM_long_read(176);  
  return region;
}

void putWeatherRegion(uint32_t value) {
  if (value != getWeatherRegion()) {
    EEPROM_long_write(176, value);
  }
}

uint32_t getWeatherRegion2() {
  uint32_t region = EEPROM_long_read(244);  
  return region;
}

void putWeatherRegion2(uint32_t value) {
  if (value != getWeatherRegion2()) {
    EEPROM_long_write(244, value);
  }
}

bool getUseTemperatureColor() {
  return EEPROMread(180) != 0;
}

void putUseTemperatureColor(bool use) {
  if (use != getUseTemperatureColor()) {
    EEPROMwrite(180, use ? 1 : 0);
  }
}

bool getUseTemperatureColorNight() {
  return EEPROMread(181) != 0;
}

void putUseTemperatureColorNight(bool use) {
  if (use != getUseTemperatureColorNight()) {
    EEPROMwrite(181, use ? 1 : 0);
  }
}

// градусы Цельсия / Фаренгейта
bool getIsFarenheit() {
  uint8_t value = EEPROMread(7);               // 0 - Цельсий, 1 - Фаренгейт
  return  value != 0;
}

// градусы Цельсия / Фаренгейта
void putIsFarenheit(bool isFarenheit) {
  if (isFarenheit != getIsFarenheit()) {
    EEPROMwrite(7, isFarenheit ? 1 : 0);       //  0 - Цельсий, 1 - Фаренгейт
  }
}

#endif

#if (USE_E131 == 1)

eWorkModes getSyncWorkMode() {
  uint8_t value = EEPROMread(90);
  if (value > 2) value = 0;
  return (eWorkModes)value;
}
void putSyncWorkMode(eWorkModes value) {
  uint8_t val = (uint8_t)value;
  if (val > 2) val = 0;
  if (val != (uint8_t)getSyncWorkMode()) {
    EEPROMwrite(90, val);
  }  
}

eSyncModes getSyncDataMode() {
  uint8_t value = EEPROMread(91);
  if (value > 2) value = 1;
  return (eSyncModes)value;
}
void putSyncDataMode(eSyncModes value) {
  uint8_t val = (uint8_t)value;
  if (val > 2) val = 1;
  if (val != (uint8_t)getSyncDataMode()) {
    EEPROMwrite(91, val);
  }  
}

uint8_t getSyncGroup() {
  uint8_t value = EEPROMread(92);
  if (value > 9) value = 9;
  return value;
}
void putSyncGroup(uint8_t value) {
  if (value > 9) value = 9;
  if (value != getSyncGroup()) {
    EEPROMwrite(92, value);
  }  
}

#endif

uint8_t getNightClockBrightness()   {
  uint8_t br = EEPROMread(240);
  if (br < MIN_NIGHT_CLOCK_BRIGHTNESS) br = MIN_NIGHT_CLOCK_BRIGHTNESS;
  return br;
}

void putNightClockBrightness(uint8_t brightness) {
  if (brightness < MIN_NIGHT_CLOCK_BRIGHTNESS) brightness = MIN_NIGHT_CLOCK_BRIGHTNESS;
  if (brightness != getNightClockBrightness()) {
    EEPROMwrite(240, brightness);
  }  
}

// ---------------------------------------------------------------------------------------------------------

void loadEffectOrder() {
  // Перебрать в цикле параметры для каждого эффекта, получить номер в определенном порядке использования
  // Если номер больше чем MAX_EFFECT - эффект не используется
  // Нсли используется - перевести ID эффекта в букву кодирования ID и поместить ее в позицию строки определяющей очередность воспроизведения эффекта
  String codes(IDX_LINE);
  char buffer[MAX_EFFECT + 1];
  memset(buffer,'\0',MAX_EFFECT + 1);
  for(int i = 0; i < MAX_EFFECT; i++) {
    uint8_t order = getEffectOrder(i);
    if (order < MAX_EFFECT) {
      buffer[order] = codes[i];
    }
  }
  
  // Если по какой-то причине строка очередности эффектов пуста - добавить в нее единственный эффект "Часы".
  // Далеее пользователь должен будет настроить использование и порядок эффектов в приложении (web-интерфейсе);
  effect_order = String(buffer);
  if (effect_order.length() == 0) effect_order = "0";  
}

// Для контроля вывести в лог какие эффекты включены и их порядок
void printEffectUsage() {

  int8_t cnt = 0;  
  String codes(IDX_LINE);
  String name_list(EFFECT_LIST);
  String effect_name; effect_name.reserve(40);
  
  DEBUGLN(F("Выбранные эффекты и их порядок: "));

  for (uint8_t i = 0; i < effect_order.length(); i++) {
    #if defined(ESP8266)
    ESP.wdtFeed();
    #else
    delay(1);
    #endif
    char eff = effect_order[i];
    int8_t eff_idx = codes.indexOf(eff);
    if (eff_idx >= 0) {
      effect_name = GetToken(name_list, eff_idx+1, ',');
      cnt++;
      if (effect_name.length() > 0) {
        DEBUG("   "); DEBUG(padNum(cnt, 2)); DEBUG(". "); DEBUGLN(effect_name);
      }
    }
  }

  DEBUGLN(F("\nОтключенные эффекты: "));

  bool found = false;
  for (int i = 0; i < MAX_EFFECT; i++) {
    char eff = codes[i];
    if (effect_order.indexOf(eff) < 0) {
      found = true;
      effect_name = GetToken(name_list, i + 1, ',');      
      if (effect_name.length() > 0) {
        DEBUG("   "); DEBUG(padNum(i + 1, 2)); DEBUG(". "); DEBUGLN(effect_name);
      }
    }
  }  

  if (!found) {
    DEBUGLN(F("   Все эффекты используются"));
  }
  
  DEBUGLN();  
}

void saveEffectOrder() {
  // Строка effect_order определяет порядок воспроизведения эффектов в случае их последовательного воспроизведения.
  // Если эффекта нет в указанной строке - он не используется в демо-режиме.
  // Если строка по какой-то причине пуста (нет определенных к использованию эффектов) - назначить единственнымм используемым эффектом "Часы"
  if (effect_order.length() == 0) effect_order = "0";  
  
  // Сначала сбросить индекс использования для всех эффектов
  for (int i = 0; i < MAX_EFFECT; i++) {
    putEffectOrder(i, 255);
  }
  
  String codes(IDX_LINE);
  
  // Теперь проставить индекс использования в соответствии с текущим содержимым effect_order
  for (uint8_t i = 0; i < effect_order.length(); i++) {
    char eff = effect_order[i];
    int16_t eff_idx = codes.indexOf(eff);
    if (eff_idx >= 0 && eff_idx < MAX_EFFECT) {
      putEffectOrder((uint8_t)eff_idx, i);
    }
  }
}

void putSyncViewport(
  [[maybe_unused]] int8_t masterX, 
  [[maybe_unused]] int8_t masterY, 
  [[maybe_unused]] int8_t localX, 
  [[maybe_unused]] int8_t localY, 
  [[maybe_unused]] int8_t localW, 
  [[maybe_unused]] int8_t localH) {
#if (USE_E131 == 1)
  if (masterX < 0) masterX = 0;
  if (masterY < 0) masterY = 0;
  if (localX < 0 || localX >= pWIDTH) localX = 0;
  if (localY < 0 || localY >= pHEIGHT) localY = 0;
  if (localW < 4 || localW > pWIDTH) localW = pWIDTH;
  if (localH < 4 || localH > pHEIGHT) localH = pHEIGHT;  
  
  EEPROMwrite(98, masterX); //   98 - masterX - трансляция экрана с MASTER - координата X мастера с которой начинается прием изображения
  EEPROMwrite(99, masterY); //   99 - masterY - трансляция экрана с MASTER - координата Y мастера с которой начинается прием изображения
  EEPROMwrite(100, localX); //  100 - localX  - трансляция экрана с MASTER - локальная координата X куда начинается вывод изображения
  EEPROMwrite(101, localY); //  101 - localY  - трансляция экрана с MASTER - локальная координата Y куда начинается вывод изображения
  EEPROMwrite(102, localW); //  102 - localW  - трансляция экрана с MASTER - ширина окна вывода изображения
  EEPROMwrite(103, localH); //  103 - localH  - трансляция экрана с MASTER - высота окна вывода изображения
#endif  
}

void loadSyncViewport() {
#if (USE_E131 == 1)
  masterX = EEPROMread(98);  //   98 - masterX - трансляция экрана с MASTER - координата X мастера с которой начинается прием изображения
  masterY = EEPROMread(99);  //   99 - masterY - трансляция экрана с MASTER - координата Y мастера с которой начинается прием изображения
  localX  = EEPROMread(100); //  100 - localX  - трансляция экрана с MASTER - локальная координата X куда начинается вывод изображения
  localY  = EEPROMread(101); //  101 - localY  - трансляция экрана с MASTER - локальная координата Y куда начинается вывод изображения
  localW  = EEPROMread(102); //  102 - localW  - трансляция экрана с MASTER - ширина окна вывода изображения
  localH  = EEPROMread(103); //  103 - localH  - трансляция экрана с MASTER - высота окна вывода изображения

  // Проверка на корректность
  if (masterX < 0) masterX = 0;
  if (masterY < 0) masterY = 0;
  if (localX < 0 || localX >= pWIDTH) localX = 0;
  if (localY < 0 || localY >= pHEIGHT) localY = 0;
  if (localW < 4 || localW > pWIDTH) localW = pWIDTH;
  if (localH < 4 || localH > pHEIGHT) localH = pHEIGHT;  
#endif
}

// ------------------------------ Подключение пинов - назначение и использование ---------------------------

// Система wiring инициализирована?
bool getWiringInitialized() {
  uint8_t value = EEPROMread(280);
  return (value & 0x80) != 0;                  // b7 - инициализация выполнена
}

void putWiringInitialized(bool use) {
  uint8_t value = EEPROMread(280);
  uint8_t new_value = use ? (value | 0x80) : (value & ~0x80);
  if (value != new_value) {
    EEPROMwrite(280, new_value);               // b7 - инициализация выполнена
  }
}

// Отладка в Serial - включена?
bool getDebugSerialEnable() {
  uint8_t value = EEPROMread(280);
  return (value & 0x40) != 0;                  // b6 - использовать линию 1 подключения светодиодной ленты
}

// Включить / выключить отладку в Serial
void putDebugSerialEnable(bool enable) {
  uint8_t value = EEPROMread(280);
  uint8_t new_value = enable ? (value | 0x40) : (value & ~0x40) | 0x80;
  if (value != new_value) {
    EEPROMwrite(280, new_value);               // b6 - использовать в эффекте бегущую строку поверх эффекта
  }
}

// Использовать линию 1..4 для вывода данных на светодиоды
bool getLedLineUsage(uint8_t line) {
  if (line < 1 || line > 4) return false; 
  uint8_t mask = 0x01 << (line - 1);
  uint8_t value = EEPROMread(280);
  return (value & mask) != 0;                  // b0 - использовать линию 1 подключения светодиодной ленты
}

// Включить / выключить линию 1..4 для вывода данных на светодиоды
void putLedLineUsage(uint8_t line, bool use) {
  if (line < 1 || line > 4) return;
  uint8_t mask = 0x01 << (line - 1);
  uint8_t value = EEPROMread(280);
  uint8_t new_value = use ? (value | mask) : (value & ~mask) | 0x80;
  if (value != new_value) {
    EEPROMwrite(280, new_value);               // b0 - использовать в эффекте бегущую строку поверх эффекта
  }
}

// Пин вывода данных на ленту - линия 1..4
int8_t getLedLinePin(uint8_t line) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 281; break;
    case 2: index = 286; break;
    case 3: index = 291; break;
    case 4: index = 296; break;
  }
  if (index == 0) return -1;
  return EEPROMread(index);
}

// Пин вывода данных на ленту - линия 1..4
void putLedLinePin(uint8_t line, int8_t new_value) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 281; break;
    case 2: index = 286; break;
    case 3: index = 291; break;
    case 4: index = 296; break;
  }
  if (index == 0) return;
  int8_t value = (int8_t)EEPROMread(index);
  if (value != new_value) {
    EEPROMwrite(index, (uint8_t)new_value);
  }
}

// Начальный индекс светодиода линии 1..4
uint16_t getLedLineStartIndex(uint8_t line) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 282; break;
    case 2: index = 287; break;
    case 3: index = 292; break;
    case 4: index = 297; break;
  }
  if (index == 0) return 0;
  int16_t value = (int16_t)EEPROM_int_read(index);
  if (value < 0) value = 0;
  return (uint16_t)value;
}

// Начальный индекс светодиода линии 1..4
void putLedLineStartIndex(uint8_t line, int16_t new_value) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 282; break;
    case 2: index = 287; break;
    case 3: index = 292; break;
    case 4: index = 297; break;
  }
  if (index == 0) return;
  int16_t value = (int16_t)EEPROM_int_read(index);
  if (new_value < 0) new_value = 0;
  if (value != new_value) {
    EEPROM_int_write(index, (uint16_t)new_value);
  }
}

// Порядок цвета светодиодов в линии 1..4
int8_t getLedLineRGB(uint8_t line) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 276; break;                // 0 - GRB
    case 2: index = 277; break;                // 1 - RGB
    case 3: index = 278; break;                // 2 - RBG 
    case 4: index = 279; break;                // 3 - GBR  
  }                                            // 4 - BRG
  if (index == 0) return 0;                    // 5 - BGR
  int8_t value = EEPROMread(index);
  if (value < 0 || value > 5) value = 0;
  return value;
}

// Порядок цвета светодиодов в линии 1..4
void putLedLineRGB(uint8_t line, int8_t new_value) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 276; break;                // 0 - GRB
    case 2: index = 277; break;                // 1 - RGB
    case 3: index = 278; break;                // 2 - RBG 
    case 4: index = 279; break;                // 3 - GBR  
  }                                            // 4 - BRG
  if (index == 0) return;                      // 5 - BGR
  int8_t value = EEPROMread(index);
  if (new_value < 0 || new_value > 5) new_value = COLOR_ORDER;
  if (value != new_value) {
    EEPROMwrite(index, new_value);
  }
}

// Количество светодиодов в линии 1..4
uint16_t getLedLineLength(uint8_t line) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 284; break;
    case 2: index = 289; break;
    case 3: index = 294; break;
    case 4: index = 299; break;
  }
  if (index == 0) return NUM_LEDS;
  int16_t value = (int16_t)EEPROM_int_read(index);
  if (value < 1 || value > NUM_LEDS) value = NUM_LEDS;
  return (uint16_t)value;
}

// Количество светодиодов в линии 1..4
void putLedLineLength(uint8_t line, int16_t new_value) {
  uint16_t index = 0;
  switch (line) {
    case 1: index = 284; break;
    case 2: index = 289; break;
    case 3: index = 294; break;
    case 4: index = 299; break;
  }
  if (index == 0) return;
  int16_t value = (int16_t)EEPROM_int_read(index);
  if (new_value < 0) new_value = NUM_LEDS;
  if (value != new_value) {
    EEPROM_int_write(index, (uint16_t)new_value);
  }
}

// Тип использования матрицы - труба / плоская
uint8_t getDeviceType() {
  return EEPROMread(301);
}

// Тип использования матрицы - труба / плоская
void putDeviceType(uint8_t value) {
  if (value != getDeviceType()) {
    EEPROMwrite(301, value);
  }
}

// Тип кнопки сенсорная / тактовая
uint8_t getButtonType() {
  return EEPROMread(302);
}

// Тип кнопки сенсорная / тактовая
void putButtonType(uint8_t value) {
  if (value != getButtonType()) {
    EEPROMwrite(302, value);
  }
}

// Пин подключения кнопки
int8_t getButtonPin() {
  return (int8_t)EEPROMread(303);
}

// Пин подключения кнопки
void putButtonPin(int8_t value) {
  if (value != (int8_t)getButtonPin()) {
    EEPROMwrite(303, value);
  }
}

// WAIT_PLAY_FINISHED - алгоритм проигрывания эффектов с SD-карты
bool getWaitPlayFinished() {
  return EEPROMread(304) == 1;
}

// WAIT_PLAY_FINISHED - алгоритм проигрывания эффектов с SD-карты
void putWaitPlayFinished(bool value) {
  if (value != getWaitPlayFinished()) {
    EEPROMwrite(304, value ? 1 : 0);
  }
}

// REPEAT_PLAY - алгоритм проигрывания эффектов с SD-карты
bool getRepeatPlay() {
  return EEPROMread(305) == 1;
}

// REPEAT_PLAY - алгоритм проигрывания эффектов с SD-карты
void putRepeatPlay(uint8_t value) {
  if (value != getRepeatPlay()) {
    EEPROMwrite(305, value ? 1 : 0);
  }
}

// Пин подключения выхода на реле управления питанием матрицы
int8_t getPowerPin() {
  return (int8_t)EEPROMread(306);
}

// Пин подключения выхода на реле управления питанием матрицы
void putPowerPin(int8_t value) {
  if (value != (int8_t)getPowerPin()) {
    EEPROMwrite(306, value);
  }
}

// Пин подключения выхода на реле управления по линии будильника
int8_t getAlarmPin() {
  return (int8_t)EEPROMread(312);
}

// Пин подключения выхода на реле управления по линии будильника
void putAlarmPin(int8_t value) {
  if (value != (int8_t)getAlarmPin()) {
    EEPROMwrite(312, value);
  }
}

// Пин подключения выхода на реле управления по дополнительной линии
int8_t getAuxPin() {
  return (int8_t)EEPROMread(314);
}

// Пин подключения выхода на реле управления по дополнительной линии
void putAuxPin(int8_t value) {
  if (value != (int8_t)getAuxPin()) {
    EEPROMwrite(314, value);
  }
}

// Пин STX DFPlayer
int8_t getDFPlayerSTXPin() {
  return (int8_t)EEPROMread(307);
}

// Пин STX DFPlayer
void putDFPlayerSTXPin(int8_t value) {
  if (value != (int8_t)getDFPlayerSTXPin()) {
    EEPROMwrite(307, value);
  }
}

// Пин SRX DFPlayer
int8_t getDFPlayerSRXPin() {
  return (int8_t)EEPROMread(308);
}

// Пин SRX DFPlayer
void putDFPlayerSRXPin(int8_t value) {
  if (value != (int8_t)getDFPlayerSRXPin()) {
    EEPROMwrite(308, value);
  }
}

// Пин DIO TM1637
int8_t getTM1637DIOPin() {
  return (int8_t)EEPROMread(309);
}

// Пин DIO TM1637
void putTM1637DIOPin(int8_t value) {
  if (value != (int8_t)getTM1637DIOPin()) {
    EEPROMwrite(309, value);
  }
}

// Пин CLK TM1637
int8_t getTM1637CLKPin() {
  return (int8_t)EEPROMread(310);
}

// Пин CLK TM1637
void putTM1637CLKPin(int8_t value) {
  if (value != (int8_t)getTM1637CLKPin()) {
    EEPROMwrite(310, value);
  }
}

// Активный уровень управляющего сигнала на питание - HIGH - 0x01 или LOW - 0x00
uint8_t getPowerActiveLevel() {
  return EEPROMread(311) == HIGH ? HIGH : LOW;
}

void putPowerActiveLevel(uint8_t value) {
  if (value != getPowerActiveLevel()) {
    EEPROMwrite(311, value);
  }
}

// Активный уровень управляющего сигнала на управление по линии будильника - HIGH - 0x01 или LOW - 0x00
uint8_t getAlarmActiveLevel() {
  return EEPROMread(313) == HIGH ? HIGH : LOW;
}

void putAlarmActiveLevel(uint8_t value) {
  if (value != getAlarmActiveLevel()) {
    EEPROMwrite(313, value);
  }
}

// Активный уровень управляющего сигнала на управление по дополнительной линии - HIGH - 0x01 или LOW - 0x00
uint8_t getAuxActiveLevel() {
  return EEPROMread(315) == HIGH ? HIGH : LOW;
}

void putAuxActiveLevel(uint8_t value) {
  if (value != getAuxActiveLevel()) {
    EEPROMwrite(315, value);
  }
}

// Текущее состояние линии дополнительного управлления питанием
bool getAuxLineState() {
  return EEPROMread(318) != 0;    // 0 - выключена; 1 - включено
}

void putAuxLineState(bool value) {
  if (value != getAuxLineState()) {
    EEPROMwrite(318, value ? 1 : 0);
  }
}

// Использование дополнительной линии управления питанием - для всех режимов  
uint16_t getAuxLineModes() {
  return EEPROM_int_read(316);
}

void putAuxLineModes(uint16_t value) {
  if (value != getAuxLineModes()) {
    EEPROM_int_write(316, value);
  }
}

String getSystemName() {
  return EEPROM_string_read(319, 32);
}

void putSystemName(const String& name) {
  if (name != getSystemName()) {
    EEPROM_string_write(319, name, 32);
  }
}

// 12/24-часовой формат времени
bool getTime12() {
  uint8_t value = EEPROMread(6);               // 0 - 24 часа, 1 - 12 часов
  return  value != 0;
}

// 12/24-часовой формат времени
void putTime12(bool time_h12) {
  if (time_h12 != getTime12()) {
    EEPROMwrite(6, time_h12 ? 1 : 0);          // 0 - 24 часа, 1 - 12 часов
  }
}

// Тип шрифта отображения малых часов и температуры
int8_t getSmallFontType() {
  uint8_t value = EEPROMread(8);               // 0 - квадратный, 1 - круглый
  return  value;
}

// Тип шрифта отображения малых часов и температуры
void putSmallFontType(int8_t type) {
  if (type != getSmallFontType()) {
    EEPROMwrite(8, type);                      // 0 - квадратный, 1 - круглый
  }
}

// Отображать значок градуса и букву C/F при отображении температуры в часах
uint8_t getShowTempProps() {
  // b0 - отображать букву C/F
  // b1 - отображать значок градуса
  return EEPROMread(96); ;
}

// Отображать значок градуса и букву C/F при отображении температуры в часах
void putShowTempProps(uint8_t type) {
  // b0 - отображать букву C/F
  // b1 - отображать значок градуса
  if (type != getShowTempProps()) {
    EEPROMwrite(96, type);
  }
}

// Отображение года в календаре: 0/2/4 цифры, в одну или в две строки
int8_t getCalendarYearWidth() {
  uint16_t addr = getClockSizeType() == 1 ? 241 : 242;            // 241 - малые часы; 242 - большие часы
  int8_t value = (int8_t)EEPROMread(addr);
  return value;
}

// Отображение года в календаре: 0/2/4 цифры, в одну или в две строки
void putCalendarYearWidth(int8_t value) {
  if (value != getCalendarYearWidth()) {
    uint16_t addr = getClockSizeType() == 1 ? 241 : 242;           // 241 - малые часы; 242 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение часов относительно центра (коррекция положения по оси X)
int8_t getClockOffsetX() {
  uint16_t addr = getClockSizeType() == 1 ? 249 : 260;             // 249 - малые часы; 260 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение часов относительно центра (коррекция положения по оси X)
void putClockOffsetX(int8_t value) {
  if (value != getClockOffsetX()) {
    uint16_t addr = getClockSizeType() == 1 ? 249 : 260;           // 249 - малые часы; 260 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение часов относительно центра (коррекция положения по оси Y)
int8_t getClockOffsetY() {
  uint16_t addr = getClockSizeType() == 1 ? 250 : 261;             // 250 - малые часы; 261 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение часов относительно центра (коррекция положения по оси Y)
void putClockOffsetY(int8_t value) {
  if (value != getClockOffsetY()) {
    uint16_t addr = getClockSizeType() == 1 ? 250 : 261;           // 250 - малые часы; 261 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение температуры относительно центра (коррекция положения по оси X)
int8_t getTempOffsetX() {
  uint16_t addr = getClockSizeType() == 1 ? 251 : 262;             // 251 - малые часы; 262 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение температуры относительно центра (коррекция положения по оси X)
void putTempOffsetX(int8_t value) {
  if (value != getTempOffsetX()) {
    uint16_t addr = getClockSizeType() == 1 ? 251 : 262;           // 251 - малые часы; 262 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение температуры относительно центра (коррекция положения по оси Y)
int8_t getTempOffsetY() {
  uint16_t addr = getClockSizeType() == 1 ? 252 : 263;             // 252 - малые часы; 263 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение температуры относительно центра (коррекция положения по оси Y)
void putTempOffsetY(int8_t value) {
  if (value != getTempOffsetY()) {
    uint16_t addr = getClockSizeType() == 1 ? 252 : 263;           // 252 - малые часы; 263 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение календаря относительно центра (коррекция положения по оси X)
int8_t getCalendarOffsetX() {
  uint16_t addr = getClockSizeType() == 1 ? 257 : 264;             // 257 - малые часы; 264 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение календаря относительно центра (коррекция положения по оси X)
void putCalendarOffsetX(int8_t value) {
  if (value != getCalendarOffsetX()) {
    uint16_t addr = getClockSizeType() == 1 ? 257 : 264;           // 257 - малыч часы; 264 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение календаря относительно центра (коррекция положения по оси Y)
int8_t getCalendarOffsetY() {
  uint16_t addr = getClockSizeType() == 1 ? 258 : 265;             // 258 - малые часы; 265 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение календаря относительно центра (коррекция положения по оси Y)
void putCalendarOffsetY(int8_t value) {
  if (value != getCalendarOffsetY()) {
    uint16_t addr = getClockSizeType() == 1 ? 258 : 265;           // 258 - малые часы; 265 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Смещение бегущей строки относительно центра (коррекция положения по оси Y)
int8_t getTextOffsetY() {
  uint16_t addr = getClockSizeType() == 1 ? 259 : 239;             // 259 - малые часы; 239 - большие часы
  return (int8_t)EEPROMread(addr);
}

// Смещение бегущей строки относительно центра (коррекция положения по оси Y)
void putTextOffsetY(int8_t value) {
  if (value != getTextOffsetY()) {
    uint16_t addr = getClockSizeType() == 1 ? 259 : 239;           // 259 - малые часы; 239 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Ширина разделительных точек в больших часах 1 или 2, если позволяет ширина маирицы
uint8_t getClockDotWidth() {
  uint8_t value = EEPROMread(253);
  if (value < 1) value = 1;
  if (value > 2) value = 2;
  return value;
}

// Ширина разделительных точек в больших часах 1 или 2, если позволяет ширина маирицы
void putClockDotWidth(uint8_t value) {
  if (value < 1) value = 1;
  if (value > 2) value = 2;
  if (value != getClockDotWidth()) {
    EEPROMwrite(253, value);
  }
}

// Точки в больших часах отделены от цифр пробелом (если позволяет ширина матрицы) - 0 или 1
bool getClockDotSpace() {
  return EEPROMread(254) > 0;
}

// Точки в больших часах отделены от цифр пробелом (если позволяет ширина матрицы) - 0 или 1
void putClockDotSpace(bool value) {
  if (value != getClockDotSpace()) {
    EEPROMwrite(254, value ? 1 : 0);
  }
}

// Использовать DHCP вместо статического адреса
bool getUseDHCP() {
  return EEPROMread(255) != 0;
}

// Использовать DHCP вместо статического адреса
void putUseDHCP(bool value) {
  if (value != getUseDHCP()) {
    EEPROMwrite(255, value ? 1 : 0);
  }
}

// Отображать значок градуса и букву C/F при отображении температуры в макросе {WT} бегущей строки
uint8_t getShowTempTextProps() {
  // b0 - отображать букву C/F
  // b1 - отображать значок градуса
  return  EEPROMread(256);
}

// Отображать значок градуса и букву C/F при отображении температуры в макросе {WT} бегущей строки
void putShowTempTextProps(uint8_t type) {
  // b0 - отображать букву C/F
  // b1 - отображать значок градуса
  if (type != getShowTempTextProps()) {
    EEPROMwrite(256, type);
  }
}

// Флаги вариантов размещения календаря и темепратуры - относительно часов или произвольно по указанному смещению
uint8_t getClockShowVariant() {
  uint16_t addr = getClockSizeType() == 1 ? 272 : 273;           // 272 - малые часы; 273 - большие часы
  return  EEPROMread(addr);
}

// Флаги вариантов размещения календаря и темепратуры - относительно часов или произвольно по указанному смещению
void putClockShowVariant(uint8_t value) {
  if (value != getClockShowVariant()) {
    uint16_t addr = getClockSizeType() == 1 ? 272 : 273;           // 272 - малые часы; 273 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Флаги выравнивани часов/календаря/температуры в области их отображения;
uint8_t getClockShowAlignment() {
  uint16_t addr = getClockSizeType() == 1 ? 270 : 271;           // 270 - малые часы; 271 - большие часы
  return  EEPROMread(addr);
}

// Флаги выравнивани часов/календаря/температуры в области их отображения;
void putClockShowAlignment(uint8_t value) {
  if (value != getClockShowAlignment()) {
    uint16_t addr = getClockSizeType() == 1 ? 270 : 271;           // 270 - малые часы; 271 - большие часы
    EEPROMwrite(addr, value);
  }
}

// Флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
uint8_t getHideOnTextRunning() {
  uint16_t addr = getClockSizeType() == 1 ? 268 : 269;           // 268 - малые часы; 269 - большие часы
  return  EEPROMread(addr);
}

// Флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
void putHideOnTextRunning(uint8_t value) {
  if (value != getHideOnTextRunning()) {
    uint16_t addr = getClockSizeType() == 1 ? 268 : 269;           // 268 - малые часы; 269 - большие часы
    EEPROMwrite(addr, value);
  }
}



// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------

uint8_t EEPROMread(uint16_t addr) {    
  return EEPROM.read(addr);
}

void EEPROMwrite(uint16_t addr, uint8_t value) {    
  EEPROM.write(addr, value);
  eepromModified = true;
  saveSettingsTimer.reset();
}

// чтение uint16_t
uint16_t EEPROM_int_read(uint16_t addr) {    
  uint8_t raw[2];
  for (uint8_t i = 0; i < 2; i++) raw[i] = EEPROMread(addr+i);
  return  *((uint16_t*) raw);
}

// запись uint16_t
void EEPROM_int_write(uint16_t addr, uint16_t num) {
  uint8_t raw[2];
  (uint16_t&)raw = num;
  for (uint8_t i = 0; i < 2; i++) EEPROMwrite(addr+i, raw[i]);
}

// чтение uint32_t
uint32_t EEPROM_long_read(uint16_t addr) {    
  uint8_t raw[4];
  for (uint8_t i = 0; i < 4; i++) raw[i] = EEPROMread(addr+i);
  return  *((uint32_t*) raw);
}

// запись uint32_t
void EEPROM_long_write(uint16_t addr, uint32_t num) {
  uint8_t raw[4];
  (uint32_t&)raw = num;
  for (uint8_t i = 0; i < 4; i++) EEPROMwrite(addr+i, raw[i]);
}

String EEPROM_string_read(uint16_t addr, int16_t len) {
   char buffer[len+1];
   memset(buffer,'\0',len+1);
   int16_t i = 0;
   while (i < len) {
     uint8_t c = EEPROMread(addr+i);
     if (c == 0) break;
     buffer[i++] = c;
   }
   return String(buffer);
}

void EEPROM_string_write(uint16_t addr, const String& buffer, uint16_t max_len) {
  uint16_t len = buffer.length();
  uint16_t i = 0;

  // Принудительно очистить "хвосты от прежнего значения"
  while (i < max_len) {
    EEPROMwrite(addr+i, 0x00);
    i++;
  }
  
  // Обрезать строку, если ее длина больше доступного места
  if (len > max_len) len = max_len;

  // Сохранить новое значение
  i = 0;
  while (i < len) {
    EEPROMwrite(addr+i, buffer[i]);
    i++;
  }
}

// ------------------------------------- EEPROM & Text Backup ----------------------------------------------

// Проверка наличия сохраненной резервной копии
// Возврат: 0 - не найден; 1 - найден в FS микроконтроллера; 2 - найден на SD-карте; 3 - найден в FS и на SD
uint8_t checkEepromBackup() {

  uint8_t existsFS = 0; 
  uint8_t existsSD = 0; 

  // Имя файла соответствует версии EEPROM
  String fileName(F("eeprom_0x")); fileName += IntToHex(EEPROM_OK, 2); fileName += ".hex";  

  // Место хранения на SD и в FS - разное
  String fullName(FS_BACK_STORAGE);
  if (!fullName.endsWith("/")) fullName += '/'; 
  fullName += fileName;


  File file = LittleFS.open(fullName, "r");
  if (file) {
    existsFS = 1;
    file.close();
  }

  #if (USE_SD == 1 && FS_AS_SD == 0)
    fullName = String(SD_BACK_STORAGE);
    if (!fullName.endsWith("/")) fullName += '/'; 
    fullName += fileName;
  
    file = SD.open(fullName);
    if (file) {
      existsSD = 2;
      file.close();
    }
  #endif
  
  return existsFS + existsSD;
}

// Сохранить eeprom в файл
// storage = "FS" - внутренняя файловая система
// storage = "SD" - на SD-карту
// возврат: true - успех; false - ошибка
bool saveEepromToFile(const String& pStorage) {

  bool ok = true;
  size_t buf_size = EEPROM_MAX, len = 0;
  File file;

  // Несохраненные в EEPROM переменные принудительно сохранить
  saveSettings();

  String storage(pStorage);
  String message; message.reserve(256);
    
  if (USE_SD == 0 || (USE_SD == 1 && FS_AS_SD == 1)) storage = "FS";

  uint8_t *buf = (uint8_t*)malloc(buf_size);
  if (buf == nullptr) {
    DEBUGLN(F("Недостаточно памяти для сохранения резервной копии настроек"));
    return false;
  }
  memset(buf, 0, buf_size);

  // Имя файла соответствует версии EEPROM
  String fileName(F("eeprom_0x")); fileName += IntToHex(EEPROM_OK, 2); fileName += ".hex";  
  String fullName(storage == "FS" ? FS_BACK_STORAGE : SD_BACK_STORAGE);
  if (!fullName.endsWith("/")) fullName += '/'; 
  fullName += fileName;

  String backName(fullName); backName.replace(".hex", ".bak");

  DEBUG(F("Сохранение файла: ")); DEBUG(storage); DEBUG(F(":/")); DEBUGLN(fullName);

  if (storage == "FS") {

    // Если файл с таким именем уже есть - переименовать в файл резервной копии
    if (LittleFS.exists(fullName)) {
      ok = LittleFS.rename(fullName, backName);
      if (!ok) {
        DEBUGLN(F("Ошибка создания файла"));
        free(buf);
        return false;
      }
    }
  
    file = LittleFS.open(fullName, "w");
  }

  #if (USE_SD == 1 && FS_AS_SD == 0) 
  if (storage == "SD") {

    // Если файл с таким именем уже есть - переименовать в файл резервной копии
    if (SD.exists(fullName)) {
      ok = SD.rename(fullName, backName);
      if (!ok) {
        DEBUGLN(F("Ошибка создания файла"));
        free(buf);
        return false;
      }
    }

    file = SD.open(fullName, FILE_WRITE);
  }
  #endif

  if (!file) {
    DEBUGLN(F("Ошибка создания файла"));
    free(buf);
    #if (USE_SD == 1 && FS_AS_SD == 0)    
    if (storage == "SD") {
      SD.rename(backName, fullName);
    } else {
      LittleFS.rename(backName, fullName);
    }
    #else
      LittleFS.rename(backName, fullName);
    #endif
    eeprom_backup = checkEepromBackup();
    return false;
  }

  // Скопировать данные из EEPROM в буфер
  for (uint16_t i = 0; i < EEPROM_MAX; i++) {
    buf[i] = EEPROMread(i);
  }

  // Сохранить буфер в файл на диск
  len = file.write(buf, buf_size);
  ok = len == buf_size;

  // Сохраняем тексты бегущей строки в формате idx-len-txt-'/0'
  //   idx - индекc '0'..'9', 'A'..'Z'
  //   len - длина бегущей строки
  //   txt - содержимое бегущей строки
  //   /0  - разделитель cahr(0)

  if (ok) {
    // Сделать резервную копию строк текста бегущей строки
    for (uint8_t i = 0; i < TEXTS_MAX_COUNT; i++) {
      
      yield();
      
      char c = getAZIndex(i);
      String text(getTextByIndex(i));
  
      // Буфер должен вмещать строку, + 1 байт - индекс строки + 2 байта - длину строки + 1 байт терминальный символ
      size_t len_text = text.length(), lenw = 0, lend = len_text + 4;      
      // Если текущий буфер недостаточен для размещения строки - выделить новый
      if (lend > buf_size) {
        free(buf);
        buf_size = lend;
        buf = (uint8_t*)malloc(buf_size);
        if (buf == nullptr) {
          DEBUGLN(F("Недостаточно памяти для сохранения резервной копии настроек"));
          ok = false; 
          break;        
        }
      }
      
      memset(buf, 0, buf_size);
      buf[0] = c;
      if (len_text > 0) {
        buf[1] = (uint8_t)(len_text & 0xff);
        buf[2] = (uint8_t)((len_text >> 8) & 0xff);      
        text.toCharArray(reinterpret_cast<char*>(buf + 3), len);
      }
      lenw = file.write(buf, len_text + 4);
      ok = lenw == len_text + 4;       
      if (!ok) break;
      
      DEBUG(F("Сохранение строки [")); DEBUG(c); DEBUG(F("] : '")); DEBUG(text); DEBUGLN("'");
    }
  }

  file.close();

  if (!ok) {
    DEBUGLN(F("Ошибка записи в файл"));
    free(buf);

    // Данные в файл сохранены не полностью - файл поврежден и непригоден к дальнейшему восстановлению - удалить его
    // Восстановить из сохраненного (переименованного)
    #if (USE_SD == 1 && FS_AS_SD == 0)
      if (storage == "SD") {
        SD.remove(fullName);
        SD.rename(backName, fullName);
      } else {
        LittleFS.remove(fullName);
        LittleFS.rename(backName, fullName);
      }
    #else
      LittleFS.remove(fullName);
      LittleFS.rename(backName, fullName);
    #endif    
        
  } else {

    DEBUGLN(F("Файл сохранен."));

    // Удалить файл предыдущего сохранения
    #if (USE_SD == 1 && FS_AS_SD == 0)
      if (storage == "SD") {
        SD.remove(backName);
      } else {
        LittleFS.remove(backName);
      }
    #else
      LittleFS.remove(backName);
    #endif    
    
  }

  // Проверить что все ок, файл резервной копии на месте  
  eeprom_backup = checkEepromBackup();
  
  return ok;
}

// Загрузить eeprom из файла
// storage = "FS" - внутренняя файловая система
// storage = "SD" - на SD-карту
// возврат: true - успех; false - ошибка
bool loadEepromFromFile(const String& pStorage) {

  bool ok = true;
  String message; message.reserve(256);
  String text; text.reserve(512);
  
  String storage(pStorage);
  if (USE_SD == 0 || (USE_SD == 1 && FS_AS_SD == 1)) storage = "FS";
  
  size_t buf_size = EEPROM_MAX, len = 0;
  File file;

  // Имя файла соответствует версии EEPROM
  String fileName(F("eeprom_0x")); fileName += IntToHex(EEPROM_OK, 2); fileName += ".hex";  
  String fullName(storage == "FS" ? FS_BACK_STORAGE : SD_BACK_STORAGE);
  if (!fullName.endsWith("/")) fullName += '/'; 
  fullName += fileName;

  DEBUG(F("Загрузка файла: ")); DEBUG(storage); DEBUG(F(":/")); DEBUGLN(fullName);

  #if (USE_SD == 1 && FS_AS_SD == 0) 
  if (storage == "SD") {
    file = SD.open(fullName, FILE_READ);
  } else {
    file = LittleFS.open(fullName, "r");
  }
  #else
    file = LittleFS.open(fullName, "r");
  #endif

  if (!file) {
    message = F("Файл '"); message += fullName; message += F("' не найден.");
    DEBUGLN(message);
    return false;
  }

  // Выделить память под буфер загрузки
  uint8_t *buf = (uint8_t*)malloc(buf_size);
  if (buf == nullptr) {
    message = F("Недостаточно памяти для загрузки резервной копии настроек"); 
    DEBUGLN(message);
    return false;
  }
  
  clearEEPROM();
  
  memset(buf, 0, buf_size);
  len = file.read(buf, buf_size);
  ok = (len == buf_size);

  if (!ok) {
    free(buf);
    message = F("Ошибка чтения файла '"); message += fullName; message += '\'';
    DEBUGLN(message);
    return false;
  }          

  for (uint16_t i = 0; i < len; i++) {
    EEPROMwrite(i, buf[i]);
  }

  // Записать в 0 текущее значение EEPROM_OK, иначе при несовпадении версии
  // после перезагрузки будут восстановлены значения по-умолчанию
  EEPROMwrite(0, EEPROM_OK);
  
  saveSettings();

  // Загрузить строки из резервной копии
  // Тексты бегущей строки сохранены в формате idx-len-txt-'/0'
  //   idx - индекc '0'..'9', 'A'..'Z'
  //   len - длина бегущей строки
  //   txt - содержимое бегущей строки
  //   /0  - разделитель cahr(0)
  
  for (uint8_t i = 0; i < TEXTS_MAX_COUNT; i++) {

    yield();    
    memset(buf, 0, buf_size);

    // Читаем индекс строки и ее длину
    len = file.read((uint8_t*)buf, 3);
    if (len != 3) {
      ok = false; 
      break;
    }

    char c = char(buf[0]);
    size_t len_text = (((buf[2] & 0xff) << 8) | buf[1]) +  1; // длина записанной строки + терминальный символ '\0'

    // Если текущий буфер недостаточен для размещения строки - выделить новый
    if (len_text > buf_size) {
      free(buf);
      buf_size = len_text;
      buf = (uint8_t*)malloc(buf_size);
      if (buf == nullptr) {
        message = F("Недостаточно памяти для сохранения резервной копии настроек"); 
        DEBUGLN(message);
        ok = false; 
        break;        
      }
    }
    
    memset(buf, '\0', buf_size);     
    size_t len = file.read((uint8_t*)buf, len_text);
          
    if (len != len_text) {
      ok = false;
      break;   
    }

    text = (char*)buf;    
    saveTextLine(c, text);    

    DEBUG(F("Загружена строка [")); DEBUG(c); DEBUG(F("] : '")); DEBUG(text); DEBUGLN("'");    
  }

  file.close();
  free(buf);

  if (!ok) {
    free(buf);
    message = F("Ошибка восстановления настроек из резервной копии: '"); message += fullName; message += '\'';
    DEBUGLN(message);
    return false;
  }          
  
  return ok;
}

// ----------------------------------------------------------
