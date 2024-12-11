
// ----------------------------------------------------

void customRoutine(uint8_t aMode) {
  doEffectWithOverlay(aMode);
}

void doEffectWithOverlay(uint8_t aMode) {
  
  bool clockReady = clockTimer.isReady();
  bool textReady = textTimer.isReady();

  bool effectReady = aMode == MC_IMAGE || effectTimer.isReady(); // "Анимация" использует собственные "таймеры" для отрисовки - отрисовка без задержек; Здесь таймер опрашивать нельзя - он после опроса сбросится. 
                                                                 //  А должен читаться в эффекте анимации, проверяя не пришло ли время отрисовать эффект фона    
                                                                 
  // Проверить есть ли активное событие текста? 
  // Если нет - после проверки  momentTextIdx = -1 и momentIdx = -1
  // Если есть - momentTextIdx - индекс текста для вывода в зависимости от ДО или ПОСЛЕ события текущее время; momentIdx - активная позиция в массиве событий moments[] 
  if (init_time) {

    checkMomentText();
    if (momentTextIdx >= 0 && currentTextLineIdx != momentTextIdx) {
      // В момент смены стоки с ДО на ПОСЛЕ - строка ПОСЛЕ извлеченная из массива содержит признак отключенности - '-' в начале или "{-}" в любом месте
      // Если передать строку с макросом отключения - processMacrosInText() вернет любую другую строку вместо отключенной
      // Чтобы это не произошло - нужно удалить признак отключенности
      currentTextLineIdx = momentTextIdx;
      macrosTextLineIdx = -1;
      specialTextEffect = -1;

      String text(getTextByIndex(currentTextLineIdx));
      if (text.length() > 0 && text[0] == '-') text = text.substring(1);
      
      while (text.indexOf("{-}") >= 0) text.replace("{-}","");
      currentText = text;
      ignoreTextOverlaySettingforEffect = textOverlayEnabled;
      loadingTextFlag = true;
    }
    
  }

  // Оверлей нужен для всех эффектов, иначе при малой скорости эффекта и большой скорости часов поверх эффекта буквы-цифры "смазываются"
  bool textOvEn  = ((textOverlayEnabled && (getEffectTextOverlayUsage(aMode))) || ignoreTextOverlaySettingforEffect) && !isTurnedOff && (!isNightClock || (isNightClock && textIndecies.length() > 0)) && thisMode < MAX_EFFECT;
  bool clockOvEn = clockOverlayEnabled && getEffectClockOverlayUsage(aMode) && thisMode != MC_CLOCK && thisMode != MC_DRAW && thisMode != MC_LOADIMAGE;
  bool needStopText = false;
  
  String out;

  // Если команда отображения текущей строки передана из приложения или
  // Если есть активная строка, связанная с текущим отслеживаемым по времени событием или
  // Если пришло время отображения очередной бегущей строки поверх эффекта
  // Переключиться в режим бегущей строки оверлеем
  if (!showTextNow && textOvEn && thisMode != MC_TEXT && (momentTextIdx >= 0 || ignoreTextOverlaySettingforEffect || (!e131_wait_command && (millis() - textLastTime) > (TEXT_INTERVAL  * 1000L)))) {

    // Обработать следующую строку для отображения, установить параметры;
    // Если нет строк к отображению - продолжать отображать оверлей часов
    
    if (prepareNextText(currentText)) {
      moment_active = momentTextIdx >= 0;      
      fullTextFlag = false;
      loadingTextFlag = false;
      showTextNow = true;                  // Флаг переключения в режим текста бегущей строки 
      textCurrentCount = 0;                // Сбросить счетчик количества раз, сколько строка показана на матрице;
      textStartTime = millis();            // Запомнить время начала отображения бегущей строки

      if (moment_active) {
        ignoreTextOverlaySettingforEffect = true;
      } else {
        pTextCount = 0;
        macrosTextLineIdx = -1;
      }

      #if (USE_E131 == 1)
        commandSetTextSpeed(textScrollSpeed);
        commandSetImmediateText(syncText); // Присваивается в prepareNextText - currentText из которого НЕ удалены макросы
      #endif

      String outText;
      doc.clear();
      doc["act"] = F("TEXT");
      doc["run"] = true;
      
      if (textHasDateTime) {
        outText = processDateMacrosInText(currentText);         // Обработать строку, превратив макросы даты в отображаемые значения
        if (outText.length() == 0) {                            // Если дата еще не инициализирована - вернет другую строку, не требующую даты
          outText = currentText;
        }      
      } else {
        outText = currentText;
      }

      doc["text"] = outText;
      serializeJson(doc, out);    
      doc.clear();
      
      SendWeb(out, TOPIC_TXT);

      #if (USE_MP3 == 1)
        // Если воспроизводился звук указанный для строки - остановить его
        if (playingTextSound >= 0 || runTextSound >= 0) {
          dfPlayer.stop(); Delay(GUARD_DELAY);
        }
        runTextSound = -1;
        playingTextSound = -1;
        runTextSoundRepeat = false;
        runTextSoundFirst = true;
        runTextSoundTime = millis();
      #endif      
    }
    
  } else

  // Если строка отображается, но флаг разрешения сняли - прекратить отображение
  if (showTextNow && !textOvEn) {
    ignoreTextOverlaySettingforEffect = false;
    needStopText = true;
  } else 
  
  // Если находимся в режиме бегущей строки и строка полностью "прокручена" или сняли разрешение на отображение бегущей строки - переключиться в режим отображения часов оверлеем
  // С учетом того, показана ли строка нужное количество раз или указанную продолжительность по времени
  if (showTextNow && fullTextFlag && !moment_active) {  
    if (textShowTime > 0) {
      // Показ строки ограничен по времени
      if ((millis() - textStartTime) > (textShowTime * 1000UL)) {
        needStopText = true;
      }
    } else {
      // Ограничение по количеству раз отображения строки
      // Увеличить счетчик показа бегущей строки; Сколько раз нужно показать - инициализируется в textShowCount во время получения и обработки очередной бегущей строки
      textCurrentCount++;                  
      // Если текст уже показан нужное количество раз - переключиться в режим отображения часов поверх эффектов
      if (textCurrentCount >= textShowCount) {
        needStopText = true;
      } else {
        fullTextFlag = false;
      }
    }
  }

  // Нет активного события, но флаг что оно отображается стоит?
  if (moment_active && momentTextIdx < 0) {
    moment_active = false;
    needStopText = true;
    rescanTextEvents();
  }

  // Отображался IP-адрес или версия прошики и показ строки завершен - включить режим 
  if (thisMode == MC_TEXT && (wifi_print_ip_text || wifi_print_version) && fullTextFlag) {   
    wifi_print_ip = false;
    wifi_print_ip_text = false;
    wifi_print_version = false;
    if (saveSpecialMode && saveSpecialModeId != 0 && saveSpecialModeId < SPECIAL_EFFECTS_START) 
      setSpecialMode(saveSpecialModeId);
    else {
      if (saveMode >= MAX_EFFECT) {
        setRandomMode(); 
      } else {
        setEffect(saveMode);
      }
    }
  }

  // Нужно прекратить показ текста бегущей строки
  if (needStopText || mandatoryStopText) {    
    showTextNow = false; 
    mandatoryStopText = false;
    ResetRunningTextFlags();
    currentText.clear();
    
    ignoreTextOverlaySettingforEffect = nextTextLineIdx >= 0;
    specialTextEffectParam = -1;
    macrosTextLineIdx = -1;

    wifi_print_ip = false;
    wifi_print_ip_text = false;
    wifi_print_version = false;

    #if (USE_E131 == 1)
      commandStopText();
    #endif

    // Если строка показывалась на фоне специального эффекта для строки или специальной однотонной заливки - восстановить эффект, который был до отображения строки
    if (saveEffectBeforeText >= 0 || useSpecialBackColor) {
      loadingFlag = specialTextEffect != saveEffectBeforeText || useSpecialBackColor;  // Восстановленный эффект надо будет перезагрузить, т.к. иначе эффекты с оверлеем будут использовать оставшийся от спецэффекта/спеццвета фон
      saveEffectBeforeText = -1;                                                       // Сбросить сохраненный / спецэффект
      specialTextEffect = -1;    
      useSpecialBackColor = false;
    }

    // Текст мог быть на фоне другого эффекта, у которого свой таймер.
    // После остановки отображения текста на фоне эффекта, установить таймер текущего эффекта
    setTimersForMode(thisMode);
    
    // Если к показу задана следующая строка - установить время показа предыдущей в 0, чтобы
    // следующая строка начала показываться немедленно, иначе - запомнить время окончания показа строки,
    // от которого отсчитывается когда начинать следующий показ
    textLastTime = nextTextLineIdx >= 0 ? 0 : millis();

    #if (USE_MP3 == 1)
      // Если воспроизводился звук указанный для строки - остановить его
      if (playingTextSound >= 0 || runTextSound >= 0) {
        dfPlayer.stop(); Delay(GUARD_DELAY);
      }
      runTextSound = -1;
      playingTextSound = -1;
      runTextSoundRepeat = false;
      runTextSoundFirst = true;
      runTextSoundTime = 0;
    #endif
    
    doc.clear();
    doc["act"] = F("TEXT");
    doc["run"] = false;

    serializeJson(doc, out);    
    doc.clear();
    
    SendWeb(out, TOPIC_TXT);

    // Если показ завершен и к отображению задана следующая строка - не нужно рисовать эффекты и все прочее - иначе экран мелькает
    // Завершить обработку - на следующем цикле будет выполнен показ следующей строки
    if (nextTextLineIdx >= 0) {
      return;
    }
  }

  // Нужно сохранять оверлей эффекта до отрисовки часов или бегущей строки поверх эффекта?
  bool needOverlay =
       (aMode == MC_CLOCK) ||                                                         // Если включен режим "Часы" (ночные часы)
       (aMode == MC_TEXT) ||                                                          // Если включен режим "Бегущая строка" (show IP address)       
      (!showTextNow && clockOvEn) || 
       (showTextNow && textOvEn);

  bool needShowScreen = false;
  bool needShowClock = false;
  bool needShowTemperature = false;
  bool needShowCalendar = false;

  if (effectReady) {
    if (showTextNow) {
      
      #if (USE_MP3 == 1)
        if (runTextSound >= 0 && playingTextSound != runTextSound) {
          if (isDfPlayerOk && noteSoundsCount > 0) {            
            if (runTextSoundFirst && (millis() - runTextSoundTime > 100)) { 
              playingTextSound = runTextSound;
              runTextSoundFirst = false;
              dfPlayer.setVolume(constrain(maxAlarmVolume,1,30));  Delay(GUARD_DELAY);
              dfPlayer.playFolderTrack(3, runTextSound);           Delay(GUARD_DELAY);
              runTextSoundTime = millis();
            }
          } else {
            runTextSound = -1;
            playingTextSound = -1;
            runTextSoundFirst = true;
            runTextSoundRepeat = false;
            runTextSoundTime = millis();
          }
        }
      #endif      

      // Если указан другой эффект, поверх которого бежит строка - отобразить его
      if (!isNightClock) {
        if (specialTextEffect >= 0) {
          if (currentTimerEffectId != specialTextEffect) {
            // Если заказанный эффект не тот же, что сейчас воспроизводится или если эффект имеет вариант - выполнить инициализацию эффекта
            setTimersForMode(specialTextEffect);
            loadingFlag = (specialTextEffect != saveEffectBeforeText) || (specialTextEffectParam >= 0 && getParam2ForMode(specialTextEffect).charAt(0) != 'X');
          }        
          processEffect(specialTextEffect);
        } else if (useSpecialBackColor) {
          fillAll(specialBackColor);
        } else {
          // Отобразить текущий эффект, поверх которого будет нарисована строка
          processEffect(aMode);
        }
      }
    } else {
      // Иначе отрисовать текущий эффект
      processEffect(aMode);
    }
    needShowScreen = true;
  }

  // Смещение бегущей строки
  if (textReady && (showTextNow || aMode == MC_TEXT)) {
    // Сдвинуть позицию отображения бегущей строки
    shiftTextPosition();
    needShowScreen = true;
  }

  // Смещение движущихся часов - движется центр отображения, для часов - позиция по разделительным точкам ЧЧ:MM,
  // для календаря - точка разделения ДД.ММ
  if (clockReady && debug_hours < 0) {
    CLOCK_MOVE_CNT--;
    if (CLOCK_MOVE_CNT <= 0) {
      CLOCK_XC--;
      CLOCK_MOVE_CNT = CLOCK_MOVE_DIVIDER;
      
      // Взять максимальную ширину из блока часов или календаря
      // Если правый край часов/календаря ушел за левый край матрицы - считать снова с правого края матрицы
      uint8_t width = max( clockW, calendarW);
      #if (USE_WEATHER == 1)
      if (useWeather > 0 && init_weather && weather_ok) width = max(width, temperatureW);
      #endif
      if (CLOCK_XC < 0) {
        if (vDEVICE_TYPE == 0) {
          CLOCK_XC = pWIDTH - 1;
        } else {
          if (CLOCK_XC + width / 2 < 0) {
            CLOCK_XC = pWIDTH + width / 2 - 1;
          }
        }
      }           
    }
  }

  // Смещение движущихся часов - движется центр отображения, для часов - позиция по разделительным точкам ЧЧ:MM,
  // для календаря - точка разделения ДД.ММ
  if (debug_hours >= 0 && debug_move != 0) {    
    CLOCK_XC += debug_move;
    // Взять максимальную ширину из блока часов или календаря
    // Если правый край часов/календаря ушел за левый край матрицы - считать снова с правого края матрицы
    // Если левый край часов/календаря ушел за правый край матрицы - считать снова с нуля
    uint8_t width = max(clockW, calendarW);    
    #if (USE_WEATHER ==1)
    if (useWeather > 0 && init_weather && weather_ok) width = max(width, temperatureW);    
    #endif
    if (CLOCK_XC < 0) {
      if (vDEVICE_TYPE == 0) {
        CLOCK_XC = pWIDTH - 1;
      } else {
        if (CLOCK_XC + width / 2 < 0) {
          CLOCK_XC = pWIDTH + width / 2 - 1;
        }
      }
    } else          
    if (CLOCK_XC >= pWIDTH) {
      if (vDEVICE_TYPE == 0) {
        CLOCK_XC = 0;
      } else {
        if (CLOCK_XC - width / 2 >= pWIDTH) {
          CLOCK_XC = -(width / 2);
        }
      }
    }           
    debug_move = 0;
  }

  // Пришло время отобразить дату (календарь) в малых часах или температуру / календарь в больших?
  checkCalendarState();

  // Нарисовать отладочный крест - вертикаль - текущая позиция центра смещения, горизонталь - середина эурана
  if (debug_cross) {
    uint8_t Y = pHEIGHT / 2;
    for (int8_t i = 0; i < pWIDTH; i++) {
      drawPixelXY(i, Y, CRGB::Yellow);      
    }
    for (int8_t i = 0; i < pHEIGHT; i++) {
      drawPixelXY(getClockX(CLOCK_XC), i, CRGB::Yellow);      
    }    
  }
  
  // Если время инициализировали и пришло время его показать - нарисовать часы поверх эффекта
  if (init_time && (((clockOvEn || aMode == MC_CLOCK) && !showTextNow && aMode != MC_TEXT && thisMode != MC_DRAW && thisMode != MC_LOADIMAGE))) {    

    // Контрастные цвета часов для отображения поверх эффекта
    setOverlayColors();

    // Вычислить позиции блоков Часов, Температуры, Календаря, размеры блока под оверлей
    calcClockPosition();
    #if (USE_WEATHER == 1)
    calcTemperaturePosition();
    #endif
    calcCalendarPosition();

    // Есть два режима отображения часов / температуры / календаря
    // 1. Традиционный - часы, температура и календарь отображаюься яединым блоком:
    //    - Часы в верхней строке при двухстрочном режиме, 
    //    - Температура в нижней строке  при двухстрочном режиме, 
    //    - Календарь отображается попеременно с блоком часов / температуры в две строки
    //    - Если часы вертикальные - часы, температура и календарь отображаются попеременно, если позволяет ширина экрана.
    //      Если ширина экрана не позволяет (температура шире) - выводятся только вертикальные часы и ЧЧ/MM и календарь ДД/MM попеременно
    //    Допускается смещение блоков относительно центра экрана - в настройках
    // 2. Произвольное размещение:
    //    - Часы, Температура и Календарь отображаются независимо в указанных позициях, задаваемых настройками смещения относительно центра экрана
    // В обоих режимах центр по вертикали - линия CLOCK_XC может сдвигаться
    // Сдвиг - в зависимости от режима Панель/Труба
    //  - Труба  - сдвиг закольцован, как только изображение уходит за левый край матрицы - тут же появляется с правого края
    //  - Панель - сдвиг выполняется с учетом ширины максимального блока часов / температуры / календаря - после того как точка изображения
    //    уйдет за левый край матрицы, справа появится только после того, епе правй край макс. широкого блока уйдет за левый край матрицы
    //
    // Бегущая строка отображается в двух режимах - обычная и строка информера. Страка информера маркируется макросом {I} в строке
    // Позиция отображения строки по вертикали и скорость прокрутки задается отдельно для обычных строк и строк информера. 
    // Возможно задание скорости прокрутки отдельной строки макросом скорости текста внутри нее, тогда общие настройки скорости игнорируются
    // И обычная строка и строка информера имеют настройки - скрывать ли блок Часов / Температуры / Календаря при показе бегущей строки или
    // эти сущности отображаются одновременно с бегущей строкой. Такой подход позволяет реализовать режим информера, когда, например,
    // часы продолжают отображаться, а вместо блока температуры в ее позиции по вертикали пробегает строка информера

    // -----------------------------------------------------------------------------
    // Время отрисовки календаря или температуры в разметке совместного отображения 
    // часов / температуры / календаря - разметка "Традиционная"
    // -----------------------------------------------------------------------------
    
    bool cal_or_temp_processed = false;
    // В больших часах календарь и температура показываются в той же позиции, что и часы и совпадают по формату - ЧЧ:MM и ДД.MM - одинаковый размер
    // В малых вертикальных часах - нет.
    if (showDateState && (showDateInClock || (!allow_two_row && (init_weather && showWeatherInClock && showWeatherState)))) {
      if (showDateInClock && showDateState && !showWeatherState) {
        // Календарь
        needShowCalendar = true;
        cal_or_temp_processed = true;
      } else {
        // Температура, когда чередуется с часами - только при горизонтальной ориентации часов и если она по высоте не входит в отображение ВМЕСТЕ с часами
        #if (USE_WEATHER == 1)       
          if ((init_weather || debug_hours >= 0) && showWeatherInClock && showDateState && showWeatherState && CLOCK_ORIENT == 0 && !allow_two_row) {
            needShowTemperature = true;
            cal_or_temp_processed = true;
          } else {   
            // Если показ календаря в часах включен - показать календарь, иначе - вместо календаря снова показать температуру, если она включена
            if (showDateInClock || !(init_weather || debug_hours >= 0)) {
              needShowCalendar = true;
              cal_or_temp_processed = true;
            } else if (showWeatherInClock && !allow_two_row) {
              needShowTemperature = true;
              cal_or_temp_processed = true;
            }
          }
        #else
          needShowCalendar = true;
          cal_or_temp_processed = true;
        #endif
      }
      needShowScreen = true;
    } 

    // Если календарь или температура по условиям не могут быть нарисованы - рисовать часы
    if (!cal_or_temp_processed) {

      needShowClock = true; 

      // Если это часы с температурой - рисовать температуру (если это доступно)
      #if (USE_WEATHER == 1)       
        needShowTemperature = (init_weather || debug_hours >= 0) && showWeatherInClock && allow_two_row && CLOCK_ORIENT == 0;
     
        // Если отображение в режиме часов с темепературой - пересчитать позиции взаимного размещения часов
        // и температуры и сдвинуть их относительно друг друга.
        // По умолчанию расчет позиций календаря и температуры выполнен относительно центра экрана
        if (needShowClock && needShowTemperature) {
          // Сдвиг по вертикали - часы в верхний ряд, температуру в нижний
          int8_t dy = (c_size == 1 ? 5 : 7) / 2 + 1;
  
          // Сдвиг по горизонтали - выбираем наиболее широкий блок, центрируем его на экране (относительно позиции CLOCK_XC),
          // затем полее короткий блок выравниваем по правому краю длинного блока
          int8_t dx_c = 0, dx_t = 0;
          if (clockW > temperatureW) {
            dx_t = (clockW - temperatureW) / 2;
            while (temperatureX + temperatureW + dx_t < clockX + clockW) dx_t++;
          } 
          if (clockW < temperatureW) {
            dx_c = (temperatureW - clockW) / 2;
            while (clockX + clockW + dx_c < temperatureX + temperatureW) dx_c++;
          }           
          
          shiftTemperaturePosition(dx_t, -dy); 
          shiftClockPosition(dx_c, dy);          
        }
      #endif
      
      needShowScreen = true;
    }

    // Сохранить оверлей - область узора на экране поверх которого будут выведены часы / температура / календарь 
    if (needOverlay) {
      if (needShowClock)       saveClockOverlay();          
      #if (USE_WEATHER == 1)
      if (needShowTemperature) saveTemperatureOverlay();          
      #endif  
      if (needShowCalendar)    saveCalendarOverlay();          
    }
    
    // Нарисовать часы / температура / календарь 
    if (needShowClock)       drawClock();
    #if (USE_WEATHER == 1)
    if (needShowTemperature) drawTemperature();
    #endif
    if (needShowCalendar)    drawCalendar();    
  }

  // Если цикл не "холостой", а была отрисовка эффекта - и пришло время перерисовать текст - сделать это
  // MC_CLOCK - ночные/дневные часы; MC_TEXT - показ IP адреса - всё на черном фоне
  if (needShowScreen && (showTextNow || aMode == MC_TEXT)) {   
    // Нарисовать оверлеем текст бегущей строки
    calcTextRectangle();                                 // Вычислить позицию и размеры блока под оверлей бегущей строки
    if (needOverlay) saveTextOverlay();                  // Сохранить оверлей - область узора на экране поверх которого будет выведена бегущая строка
    runningText();                                       // Нарисовать бегущую строку
  }

  // Вывести на матрицу подготовленное изображение
  if (needShowScreen) {
    FastLEDshow();    
  }

  // Восстановить пиксели эффекта сохраненные перед наложением часов / температуры / календаря / текста бегущей строки - (оверлей)
  if (needOverlay) {
    restoreTextOverlay();
    restoreClockOverlay();
    #if (USE_WEATHER == 1)
    restoreTemperatureOverlay();
    #endif
    restoreCalendarOverlay();
  }
  
}

void FastLEDshow() { 
  // Если при трансляции отправлять пакеты чаще E131_FRAME_DELAY мс - они забивают сеть до зависания роутера
  #if (USE_E131 == 1)
    if (abs((long long)(millis() - prevSendTimer)) > E131_FRAME_DELAY) {
      if (workMode == MASTER && (syncMode == PHYSIC || syncMode == LOGIC)) {
        sendE131Screen();
        delay(e131_send_delay);
      }
      prevSendTimer = millis();
    }
  #endif
  
  FastLED.show();

  #if defined(ESP8266) // Почему-то на ESP8266 часто вместо выврда на матрицу просто мерцает первый светодиод
  FastLED.show();      // В issues FastLED пигут что это случается у многих. Объяснений нет, но говорят часто помогает
  #endif               // пнуть вывод на матрицк второй раз сразу же после первого

  prevShowTimer = millis();
}

void FastLEDsetBrightness(uint8_t value) {
  #if (USE_E131 == 1)
  if (workMode == MASTER) {
    commandSetCurrentBrightness(value);
  }
  #endif
  addKeyToChanged("BR");
  FastLED.setBrightness(value);
}

void processEffect(uint8_t aMode) {
  
  // Эффект сменился?  resourcesMode - эффект, который был на предыдущем шаге цикла для которого были выделены ресурсы памяти, aMode - текущий эффект  
  if (resourcesMode != aMode && aMode < MAX_EFFECT) {
    // Освободить ресурсы (в основном динамическое выделение памяти под работу эффекта)    
    releaseEffectResources(resourcesMode);
    resourcesMode = aMode;        
    loadingFlag = true;
  }  
  
  #if (DEBUG_MEM_EFF == 1)
    bool saveLoadingFlag = loadingFlag;
    int32_t mem_bef, mem_aft, mem_dif;
    if (saveLoadingFlag) {
      mem_bef = ESP.getFreeHeap();
    }
  #endif 

  switch (aMode) {
    case MC_NOISE_MADNESS:       madnessNoise(); break;
    case MC_NOISE_CLOUD:         cloudNoise(); break;
    case MC_NOISE_LAVA:          lavaNoise(); break;
    case MC_NOISE_PLASMA:        plasmaNoise(); break;
    case MC_NOISE_RAINBOW:       rainbowNoise(); break;
    case MC_NOISE_RAINBOW_STRIP: rainbowStripeNoise(); break;
    case MC_NOISE_ZEBRA:         zebraNoise(); break;
    case MC_NOISE_FOREST:        forestNoise(); break;
    case MC_NOISE_OCEAN:         oceanNoise(); break;
    case MC_PAINTBALL:           lightBallsRoutine(); break;
    case MC_SNOW:                snowRoutine(); break;
    case MC_SPARKLES:            sparklesRoutine(); break;
    case MC_CYCLON:              cyclonRoutine(); break;
    case MC_FLICKER:             flickerRoutine(); break;
    case MC_PACIFICA:            pacificaRoutine(); break;
    case MC_SHADOWS:             shadowsRoutine(); break;
    case MC_SHADOWS2:            shadows2Routine(); break;
    case MC_MATRIX:              matrixRoutine(); break;
    case MC_STARFALL:            starfallRoutine(); break;
    case MC_BALL:                ballRoutine(); break;
    case MC_WORMS:               wormsRoutine(); break;
    case MC_RAINBOW:             rainbowRoutine(); break;      // rainbowHorizontal(); // rainbowVertical(); // rainbowDiagonal(); // rainbowRotate();
    case MC_FIRE:                fireRoutine(); break;
    case MC_FILL_COLOR:          fillColorProcedure(); break;
    case MC_COLORS:              colorsRoutine(); break;
    case MC_LIGHTERS:            lightersRoutine(); break;
    case MC_SWIRL:               swirlRoutine(); break;
    case MC_MAZE:                mazeRoutine(); break;
    case MC_SNAKE:               snakeRoutine(); break;
    case MC_TETRIS:              tetrisRoutine(); break;
    case MC_ARKANOID:            arkanoidRoutine(); break;
    case MC_PALETTE:             paletteRoutine(); break;
    case MC_MUNCH:               munchRoutine(); break;
    case MC_ANALYZER:            analyzerRoutine(); break;
    case MC_PRIZMATA:            prizmataRoutine(); break;
    case MC_PRIZMATA2:           prizmata2Routine(); break;
    case MC_RAIN:                rainRoutine(); break;
    case MC_FIRE2:               fire2Routine(); break;
    case MC_ARROWS:              arrowsRoutine(); break;
    case MC_TEXT:                runningText(); break;
    case MC_CLOCK:               clockRoutine(); break;
    case MC_DAWN_ALARM:          dawnProcedure(); break;
    case MC_PATTERNS:            patternRoutine(); break;
    case MC_RUBIK:               rubikRoutine(); break;
    case MC_STARS:               starsRoutine(); break;
    case MC_STARS2:              stars2Routine(); break;
    case MC_TRAFFIC:             trafficRoutine(); break;
    case MC_FIREWORKS:           fireworksRoutine(); break;

    #if (USE_ANIMATION == 1)
    case MC_IMAGE:               animationRoutine(); break;
    case MC_WEATHER:             weatherRoutine(); break;
    case MC_SLIDE:               slideRoutine(); break;
    #endif

    #if (USE_SD == 1)
    case MC_SDCARD:              sdcardRoutine(); break;
    #endif

    // Спец.режимы так же как и обычные вызываются в customModes (MC_DAWN_ALARM_SPIRAL и MC_DAWN_ALARM_SQUARE)
    case MC_DAWN_ALARM_SPIRAL:   dawnLampSpiral(); break;
    case MC_DAWN_ALARM_SQUARE:   dawnLampSquare(); break;
  }

  #if (DEBUG_MEM_EFF == 1)
    if (saveLoadingFlag) {
      mem_aft = ESP.getFreeHeap();
      printMemoryDiff(mem_bef, mem_aft, F("alloc: "));
    }
  #endif
}

void releaseEffectResources(uint8_t aMode) {
  #if (DEBUG_MEM_EFF == 1)
    int32_t mem_bef, mem_aft, mem_dif;
    mem_bef = ESP.getFreeHeap();
  #endif
  
  switch (aMode) {
    case MC_NOISE_MADNESS:       releaseNoise(); break;
    case MC_NOISE_CLOUD:         releaseNoise(); break;
    case MC_NOISE_LAVA:          releaseNoise(); break;
    case MC_NOISE_PLASMA:        releaseNoise(); break;
    case MC_NOISE_RAINBOW:       releaseNoise(); break;
    case MC_NOISE_RAINBOW_STRIP: releaseNoise(); break;
    case MC_NOISE_ZEBRA:         releaseNoise(); break;
    case MC_NOISE_FOREST:        releaseNoise(); break;
    case MC_NOISE_OCEAN:         releaseNoise(); break;
    case MC_PAINTBALL:           break;
    case MC_SNOW:                break;
    case MC_SPARKLES:            break;
    case MC_CYCLON:              break;
    case MC_FLICKER:             break;
    case MC_PACIFICA:            break;
    case MC_SHADOWS:             break;
    case MC_SHADOWS2:            shadows2RoutineRelease(); break;
    case MC_MATRIX:              break;
    case MC_STARFALL:            break;
    case MC_BALL:                break;
    case MC_WORMS:               break;
    case MC_RAINBOW:             break;
    case MC_FIRE:                fireRoutineRelease(); break;
    case MC_FILL_COLOR:          break;
    case MC_COLORS:              break;
    case MC_LIGHTERS:            lighters2RoutineRelease(); break;
    case MC_SWIRL:               break;
    case MC_MAZE:                mazeRoutineRelease(); break;
    case MC_SNAKE:               break;
    case MC_TETRIS:              break;
    case MC_ARKANOID:            break;
    case MC_PALETTE:             paletteRoutineRelease(); break;
    case MC_MUNCH:               break;
    case MC_ANALYZER:            analyzerRoutineRelease(); break;
    case MC_PRIZMATA:            break;
    case MC_PRIZMATA2:           prizmata2RoutineRelease(); break;
    case MC_RAIN:                rainRoutineRelease(); break;
    case MC_FIRE2:               fire2RoutineRelease(); break;
    case MC_ARROWS:              break;
    case MC_TEXT:                break;
    case MC_CLOCK:               break;
    case MC_DAWN_ALARM:          break;
    case MC_PATTERNS:            break;
    case MC_RUBIK:               rubikRoutineRelease(); break;
    case MC_STARS:               break;
    case MC_STARS2:              stars2RoutineRelease(); break;
    case MC_TRAFFIC:             trafficRoutineRelease(); break;
    case MC_FIREWORKS:           fireworksRoutineRelease(); break;

    #if (USE_ANIMATION == 1)
    case MC_IMAGE:               break;
    case MC_WEATHER:             break;
    case MC_SLIDE:               slideRoutineRelease(); break;
    #endif

    #if (USE_SD == 1)
    case MC_SDCARD:              sdcardRoutineRelease(); break;
    #endif

    // Спец.режимы так же как и обычные вызываются в customModes (MC_DAWN_ALARM_SPIRAL и MC_DAWN_ALARM_SQUARE)
    case MC_DAWN_ALARM_SPIRAL:   break;
    case MC_DAWN_ALARM_SQUARE:   break;
  }

  #if (DEBUG_MEM_EFF == 1)
    mem_aft = ESP.getFreeHeap();
    printMemoryDiff(mem_bef, mem_aft, F("free: "));
  #endif
}

// ********************* ОСНОВНОЙ ЦИКЛ РЕЖИМОВ *******************

static void nextMode() {
  gameOverFlag = false;
  nextModeHandler();
}

static void prevMode() {
  gameOverFlag = false;
  prevModeHandler();
}

void nextModeHandler() {

  if (useRandomSequence) {
    setRandomMode();
    return;
  }

  int8_t newMode = thisMode;

  // Берем следующий режим по циклу режимов
  // Если режим - SD-карта и установлено последовательное воспроизведение файлов - брать следующий файл с SD-карты
  #if (USE_SD == 1)
    if (newMode == MC_SDCARD && getEffectScaleParamValue2(MC_SDCARD) == 1) {
      if (sf_file_idx == -2 || sf_file_idx >= countFiles) sf_file_idx = 0;
      else sf_file_idx++;
      if (sf_file_idx >= countFiles) {
        effect_order_idx++;
        if (effect_order_idx >= effect_order.length()) effect_order_idx = 0;
        newMode = (String(IDX_LINE).indexOf(effect_order[effect_order_idx]));
      }
    } else {
      effect_order_idx++;
      if (effect_order_idx >= effect_order.length()) effect_order_idx = 0;
      newMode = (String(IDX_LINE).indexOf(effect_order[effect_order_idx]));
      // Это процедура листания "вперед". Если передистнули на режим SD_CARD, 
      // то при последовательном воспроизведении файлов SD-карты нужно индекс сбросить на начало списка
      if (newMode == MC_SDCARD && getEffectScaleParamValue2(MC_SDCARD) == 1) {
        sf_file_idx = 0;
      }
    }
  #else
    effect_order_idx++;
    if (effect_order_idx >= effect_order.length()) effect_order_idx = 0;
    newMode = (String(IDX_LINE).indexOf(effect_order[effect_order_idx]));
  #endif

  if (newMode < 0) newMode = 0; else
  if (newMode >= MAX_EFFECT) newMode = MAX_EFFECT - 1;
  
  set_thisMode(newMode);  
  
  loadingFlag = true;
  autoplayTimer = millis();
  setTimersForMode(thisMode);
  
  FastLED.clear();
  FastLEDsetBrightness(deviceBrightness);
}

void prevModeHandler() {

  if (useRandomSequence) {
    setRandomMode();
    return;
  }

  int8_t newMode = thisMode;
    
  // Берем предыдущий режим по циклу режимов
  // Если режим - SD-карта и установлено последовательное воспроизведение файлов - брать предыдущий файл с SD-карты
  #if (USE_SD == 1)
    if (newMode == MC_SDCARD && getEffectScaleParamValue2(MC_SDCARD) == 1) {
      if (sf_file_idx == -2 || sf_file_idx >= countFiles) sf_file_idx = countFiles - 1;
      else sf_file_idx--;
      if (sf_file_idx < 0) {
        sf_file_idx = -2;          
        effect_order_idx--;  // effect_order_idx имеет тип uint8_t и при переходе через 0 значение будет 255 
        if (effect_order_idx >= effect_order.length()) effect_order_idx = effect_order.length() - 1;
        newMode = (String(IDX_LINE).indexOf(effect_order[effect_order_idx]));
      }
    } else {
      effect_order_idx--;  // effect_order_idx имеет тип uint8_t и при переходе через 0 значение будет 255 
      if (effect_order_idx >= effect_order.length()) effect_order_idx = effect_order.length() - 1;
      newMode = (String(IDX_LINE).indexOf(effect_order[effect_order_idx]));
      // Это процедура листания "назад". Если перелистнули на режим SD_CARD, 
      // то при последовательном воспроизведении файлов SD-карты нужно индекс установить на конец списка
      if (newMode == MC_SDCARD && getEffectScaleParamValue2(MC_SDCARD) == 1) {
        sf_file_idx = countFiles - 1;
      }
    }
  #else
    effect_order_idx--;  // effect_order_idx имеет тип uint8_t и при переходе через 0 значение будет 255
    if (effect_order_idx >= effect_order.length()) effect_order_idx = effect_order.length() - 1;
    newMode = (String(IDX_LINE).indexOf(effect_order[effect_order_idx]));
  #endif
      
  if (newMode < 0) newMode = 0; else
  if (newMode >= MAX_EFFECT) newMode = MAX_EFFECT - 1;
  
  set_thisMode(newMode);  

  loadingFlag = true;
  autoplayTimer = millis();
  setTimersForMode(thisMode);
  
  FastLED.clear();
  FastLEDsetBrightness(deviceBrightness);
}

void setTimersForMode(uint8_t aMode) {

  if (!(aMode == MC_TEXT || aMode == MC_CLOCK)) {
    uint8_t efSpeed = getEffectSpeedValue(aMode);
    if (efSpeed == 0) efSpeed = 1;
    // Эти режимы смотрятся (работают) только на максимальной скорости;
    if (aMode == MC_PAINTBALL || aMode == MC_SWIRL || aMode == MC_FLICKER || aMode == MC_PACIFICA || 
        aMode == MC_SHADOWS || aMode == MC_PRIZMATA || aMode == MC_FIRE2 ||
        aMode == MC_WEATHER || aMode == MC_ARKANOID || aMode == MC_TETRIS || 
        aMode == MC_PATTERNS || aMode == MC_STARS || aMode == MC_STARS2 || aMode == MC_IMAGE || aMode == MC_SLIDE ||
        aMode == MC_RAINBOW || aMode == MC_CYCLON || aMode == MC_PRIZMATA2 || aMode == MC_SHADOWS2 || aMode == MC_FIREWORKS
        ) {      
      if (aMode == MC_PATTERNS) {
         uint8_t variant = map8(getEffectScaleParamValue(MC_PATTERNS),0,4);
         if (variant == 0) effectTimer.setInterval(50);
         else effectTimer.setInterval(efSpeed);
      } else
      if (aMode == MC_RAINBOW) {
        effectTimer.setInterval(map8(efSpeed,1,128));
      } else
      if (aMode == MC_PRIZMATA2) {
        effectTimer.setInterval(map8(efSpeed,1,40));
      } else
      if (aMode == MC_SHADOWS2) {
        effectTimer.setInterval(map8(efSpeed,1,40));
      } else
      if (aMode == MC_FIREWORKS) {
        effectTimer.setInterval(map8(efSpeed,10,64));
      } else
      if (aMode == MC_CYCLON) {
        effectTimer.setInterval(map8(efSpeed,1,50));
      } else
      if (aMode == MC_STARS2) {
        effectTimer.setInterval(map8(efSpeed,1,50));
      } else
      if (aMode == MC_TETRIS) {
        effectTimer.setInterval(50);
        gameTimer.setInterval(200 + 4 * efSpeed);
      } else
      if (aMode == MC_ARKANOID) {
        effectTimer.setInterval(50);
        gameTimer.setInterval(efSpeed);  
      } else {
        effectTimer.setInterval(10);
      }
    } else if (aMode == MC_MAZE) {
      effectTimer.setInterval(50 + 3 * efSpeed);              
    }
    else
      effectTimer.setInterval(efSpeed);
  } else if (aMode == MC_CLOCK || aMode == MC_FILL_COLOR) {
      effectTimer.setInterval(250);
  }

  currentTimerEffectId = aMode;
  
  if (!e131_wait_command) {
    set_clockScrollSpeed(getClockScrollSpeed());
    if (clockScrollSpeed <= D_CLOCK_SPEED_MIN) set_clockScrollSpeed(D_CLOCK_SPEED_MIN); // Если clockScrollSpeed == 0 - бегущая строка начинает дергаться.
    if (clockScrollSpeed >= D_CLOCK_SPEED_MAX) set_clockScrollSpeed(D_CLOCK_SPEED_MAX);
  }
  if (clockScrollSpeed > 254) {
    clockTimer.stopTimer();
    CLOCK_XC = pWIDTH / 2;
  } else {
    clockTimer.setInterval(clockScrollSpeed * 4); // Самая медленная скорость - 253*4 - примерно раз в секунду
  }

  if (!e131_wait_command) {
    set_textScrollSpeed(getTextScrollSpeed());
    if (textScrollSpeed <= D_TEXT_SPEED_MIN) set_textScrollSpeed(D_TEXT_SPEED_MIN); // Если textScrollSpeed == 0 - бегущая строка начинает дергаться.
    if (textScrollSpeed >= D_TEXT_SPEED_MAX) set_textScrollSpeed(D_TEXT_SPEED_MAX);
  }
  
  textTimer.setInterval(textScrollSpeed);
}

void checkIdleState() {

  if (!(manualMode || specialMode)) {
    uint32_t ms = millis();
    if (((ms - autoplayTimer > autoplayTime) // таймер смены режима
           // при окончании игры не начинать ее снова
           || (gameOverFlag && !vREPEAT_PLAY)
           #if (USE_SD == 1)
           // если файл с SD-карты проигрался до конца - сменить эффект
           || (thisMode == MC_SDCARD && play_file_finished)
           #endif
        ) && !(manualMode || e131_wait_command)
      ) {    
      bool ok = true;
      if (
         (thisMode == MC_TEXT     && !fullTextFlag) ||   // Эффект "Бегущая строка" (показать IP адрес) не сменится на другой, пока вся строка не будет показана полностью
      // (showTextNow && !fullTextFlag)             ||   // Если нужно чтобы эффект не менялся, пока не пробежит вся строка оверлеем - раскомментировать эту строку
         (thisMode == MC_MAZE     && !gameOverFlag) ||   // Лабиринт не меняем на другой эффект, пока игра не закончится (не выйдем из лабиринта)
      // (thisMode == MC_SNAKE    && !gameOverFlag) ||   // Змейка долгая игра - не нужно дожидаться окончания, можно прервать
         (thisMode == MC_TETRIS   && !gameOverFlag) ||   // Тетрис не меняем на другой эффект, пока игра не закончится (стакан не переполнится)
      // (thisMode == MC_ARKANOID && !gameOverFlag) ||   // Арканоид долгая игра - не нужно дожидаться окончания, можно прервать
      // (showTextNow && (specialTextEffect >= 0))  ||   // Воспроизводится бегущая строка на фоне указанного эффекта
         (showTextNow)                                   // Воспроизводится бегущая строка - эффект не менять
         #if (USE_SD == 1)
         // Для файла с SD-карты - если указан режим ожидания проигрывания файла до конца, а файл еще не проигрался - не менять эффект
         // Также если файл проигрался, установлен флаг vREPEAT_PLAY и время смены режима еще не пришло - не менять эффект, продолжать проигрывать ролик
         || (thisMode == MC_SDCARD && ((vWAIT_PLAY_FINISHED && !play_file_finished) || ((ms - autoplayTimer <= autoplayTime) && play_file_finished && vREPEAT_PLAY) || loadingFlag))
         #endif
      )
      {        
        // Если бегущая строка или игра не завершены - смены режима не делать        
        ok = false;
      } 

      // Смена режима разрешена
      if (ok) {
        // Если режим не игра и не бегущая строка или один из этих режимов и есть флаг завершения режима -
        // перейти к следующему режиму; если режим был - бегущая строка - зафиксировать время окончания отображения последней бегущей строки        
        autoplayTimer = millis();
        nextMode();
      }
    }
  } else {
    if (idleTimer.isReady() && !isTurnedOff) {      // таймер холостого режима. Если время наступило - включить автосмену режимов 
      DEBUGLN(F("Автоматический режим включен по таймеру бездействия."));
      e131_wait_command = false;
      setManualModeTo(false);      
      nextMode();
    }
  }  
}

void setIdleTimer() {
  // Таймер бездействия  
  if (idleTime == 0 || specialMode) {
    if (!idleTimer.isStopped()) { 
      DEBUGLN(F("Переход в авторежим отключен"));
      idleTimer.stopTimer();
    }
  } else {
    if (idleTimer.getInterval() != idleTime) {
      DEBUG(F("Таймер включения авторежима: "));
      DEBUG(idleTime / 60000UL);
      DEBUGLN(F(" минут(а)"));
      idleTimer.setInterval(idleTime);    
    }
  }
  idleTimer.reset();
}
