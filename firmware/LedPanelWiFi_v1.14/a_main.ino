// ----------------------------------------------------
#include "timeProcessor.h"

// Контроль времени цикла
uint32_t last_ms = millis();  
int32_t ccnt;

#if (DEBUG_MEM_STP == 1)
int32_t mem_prv, mem_now, mem_dff;
#endif

// Отладка приема пакетов E1.31
#if (USE_E131 == 1)
bool flag_1 = false;
bool flag_2 = false;
uint32_t last_sent_alarm_sync; 
uint32_t last_sent_aux_sync;
uint32_t last_sent_brightness_sync;
uint32_t last_sent_screen_sync;
#endif

// Forward declaration
String getStateValue(const String& key, int8_t effect, bool small = false);

void process() {  

  // Время прохода одного цикла  
  //uint32_t duration = millis() - last_ms;
  //DEBUG(F("duration="));
  //DEBUGLN(duration);
  last_ms = millis();
    
  #if (DEBUG_MEM_STP == 1)
    mem_now = ESP.getFreeHeap();
    if (mem_prv > 0) {
      printMemoryDiff(mem_prv, mem_now, F("alloc: "));
    }
    mem_prv = mem_now;
  #endif
    
  // принимаем данные
  parsing();
  
  #if (USE_E131 == 1)
    streaming = e131 != NULL && (workMode == MASTER || (workMode == SLAVE && (e131_wait_command || ((!e131->isEmpty() || (millis() - e131_last_packet <= E131_TIMEOUT)))))); 
    if (!e131_streaming && streaming) {
      flag_1 = false;
      flag_2 = false;
      
      addKeyToChanged("E4");
      
      if (workMode == MASTER) {
        
        DEBUGLN(F("Запуск вещания E1.31 потока"));
        commandSetDimension(pWIDTH, pHEIGHT);
        commandTurnOnOff(isTurnedOff);
        commandSetBrightness(deviceBrightness);
        commandSetSpecialBrightness(specialBrightness);
        if (syncMode == COMMAND) {
          // Установить  на клиентах время мастера, текущий цвет рисования, скорости прокрутки текста и часов,
          // включить на клиентах тот же эффект или спец-режим, что сейчас на мастере
          commandSetCurrentTime(hour(), minute(), second(), day(), month(), year());
          commandSetGlobalColor(globalColor);
          commandSetTextSpeed(textScrollSpeed);
          commandSetClockSpeed(clockScrollSpeed);
          if (specialMode)
            commandSetSpecialMode(specialModeId);
          else
            commandSetMode(thisMode);
        }
        
      } else {
        
        masterWidth = 0;
        masterHeight = 0;
        isRemoteAlarm = false;
        isRemoteAuxActive = false;
        
        if (isTurnedOff) {
          FastLED.setBrightness(getMaxBrightness());
        }
        
        if (syncMode == COMMAND) {
          DEBUGLN(F("Ожидание поступления потока команд E1.31...\n"));        
          e131_wait_command = true;
        } else {
          DEBUGLN(F("Ожидание поступления потока данных E1.31...\n"));
          e131_wait_command = false;
        }

        // Включить специальный режим - эффекты не рисовать, ресурсов не занимать
        saveSpecialMode = specialMode;
        saveSpecialModeId = specialModeId;
        saveMode = thisMode;
        set_globalColor(0x000000);
        set_thisMode(MC_FILL_COLOR);
        setTimersForMode(thisMode);
        idleTimer.stopTimer();
                
      }

      prevWorkMode = workMode;
      prevSyncMode = syncMode;
      prevSyncGroup = syncGroup;
      
      doc.clear();
      doc["act"]   = String(sE131);
      doc["mode"]  = String(workMode == MASTER ? sMASTER : sSLAVE);
      doc["type"]  = String(syncMode == PHYSIC ? sPHYSIC : (syncMode == LOGIC ? sLOGIC : sCOMMAND));
      doc["group"] = syncGroup;
      doc["run"]   = true;

      String out;
      serializeJson(doc, out); 
      doc.clear();

      SendWeb(out, TOPIC_E131);
      FastLED.clear();
    }
    
    if (e131_streaming && !streaming) {

      isRemoteAlarm = false;
      e131_wait_command = false;
      
      if (prevWorkMode == MASTER) {
        DEBUGLN(F("Останов вещания E1.31 потока"));
      } else {
        DEBUGLN(F("Останов слушателя E1.31 потока"));
        FastLED.setBrightness(getMaxBrightness());
      }

      // При начале слушания вещания с мастера ресурсы были освобождены, включен режим черного экрана, чтобы память не расходовалась
      // Сейчас вещание выключено - включаем режим тот, что был до перехода к приему трансляции
      if (saveSpecialMode && saveSpecialModeId != 0 && saveSpecialModeId < SPECIAL_EFFECTS_START) {
        setSpecialMode(saveSpecialModeId);
      } else {
        setManualModeTo(getAutoplay());
        setEffect(saveMode);
      }

      addKeyToChanged("E4");
      
      doc.clear();
      doc["act"]   = String(sE131);
      doc["mode"]  = String(prevWorkMode == MASTER ? sMASTER : sSLAVE);
      doc["type"]  = String(prevSyncMode == PHYSIC ? sPHYSIC : (prevSyncMode == LOGIC ? sLOGIC : sCOMMAND));
      doc["group"] = prevSyncGroup;
      doc["run"]   = false;
      
      String out;
      serializeJson(doc, out);      
      doc.clear();

      SendWeb(out, TOPIC_E131);
    }
    e131_streaming = streaming;
  #endif

  // Если включен эффект с кодом большим кол-ва эффектов (но меньшим кода специальных эффектов) - 
  // выбрать случайный эффект, иначе будет отображаться черный экран или застывший предыдущий эффект
  if (thisMode >= MAX_EFFECT && thisMode < SPECIAL_EFFECTS_START) {
    setRandomMode(); 
  }

  // Эффект сменился?  tmpSaveMode - эффект, который был на предыдущем шаге цикла, thisMode - текущий эффект
  if (tmpSaveMode != thisMode) {
    if (effect_name.length() == 0) {
      // Если режим отсутствует в списке эффектов - имя пустое - включить случайный, 
      // на следующем цикле tmpSaveMode != thisMode - имя будет определено снова
      setRandomMode(); 
    } else {
      #if (USE_E131 == 1)
        if (!(streaming && workMode == SLAVE)) {
          DEBUG(F("Режим: ")); 
          DEBUG(effect_name);
          #if (DEBUG_MEM_EFF == 1)
            printMemoryInfo();
          #endif
          DEBUGLN();       
        }
      #else
        DEBUG(F("Режим: ")); 
        DEBUG(effect_name);
        #if (DEBUG_MEM_EFF == 1)
          printMemoryInfo();
        #endif
        DEBUGLN();       
      #endif
      tmpSaveMode = thisMode;    
    }               
  }

  // Раз в час выполнять пересканирование текстов бегущих строк на наличие события непрерывного отслеживания.
  // При сканировании события с нечеткими датами (со звездочками) просматриваются не далее чем на сутки вперед
  // События с более поздним сроком не попадут в отслеживание. Поэтому требуется периодическое перестроение списка.
  // Сканирование требуется без учета наличия соединения с интернетом и значения флага useNTP - время может быть установлено вручную с телефона
  if (needRescanText || (init_time && (millis() - textCheckTime > SECS_PER_HOUR * 1000L))) {
    rescanTextEvents();
  }

  if (wifi_connected) {

    #if (USE_WEATHER == 1)  
      if (useWeather > 0) {   
        // Если настройки программы предполагают получение сведений о текущей погоде - выполнить обновление данных с погодного сервера
        if ((weather_t > 0) && getWeatherInProgress && (millis() - weather_t > 5000)) {
          DEBUGLN(F("Таймаут запроса погоды!"));
          getWeatherInProgress = false;
          weather_cnt++;
          refresh_weather = true; weather_t = 0;
          if (init_weather && weather_cnt >= TRY_GET_WEATHER_CNT) {
            DEBUGLN(F("Не удалось установить соединение с сервером погоды."));  
            refresh_weather = false;
            
            doc.clear();
            doc["act"]    = String(sWEATHER);
            doc["region"] = useWeather == 1 ? regionID : regionID2;
            doc["result"] = String(sTIMEOUT);

            String out;              
            serializeJson(doc, out);      
            doc.clear();
            
            SendWeb(out, TOPIC_WTR);
          }
        }
        
        bool timeToGetWeather = weatherTimer.isReady(); 
        if (timeToGetWeather) { weather_cnt = 0; weather_t = 0; refresh_weather = true; }
        // weather_t - время последней отправки запроса. Запрашивать погоду если weather_t обнулено или если последний (неудачный) запрос произошел не менее чем минуту назад
        // иначе слишком частые запросы нарушают коммуникацию с приложением - устройство все время блокирующе запрашивает данные с сервера погоды
        if (timeToGetWeather || (refresh_weather && (weather_t == 0 || (millis() - weather_t > 60000)) && weather_cnt < TRY_GET_WEATHER_CNT)) {
          weather_t = millis();
          getWeatherInProgress = true;
          getWeather();
          if (weather_cnt >= TRY_GET_WEATHER_CNT) {
            if (!init_weather) {
              // Если погода еще не была инициализирована - следующий запрос брать по меньшему из 5 минут или SYNC_WEATHER_PERIOD
              weatherTimer.setInterval(1000 * 60 * min((uint16_t)5, SYNC_WEATHER_PERIOD));
            }
          }        
        }            
      }
    #endif
  } 

  #if (USE_WEATHER == 1)  
    // Если погода не смогла обновиться за TRY_GET_WEATHER_CNT попыток за период обновления погоды + 30сек "запаса" - считать погоду неинициализированной и не отображать погоду в часах
    if (useWeather > 0 && init_weather && (millis() - weather_time > weatherActualityDuration * 3600L * 1000L)) {
      init_weather = false;
      refresh_weather = true;
    }
  #endif

  // Проверить необходимость отправки сообщения об изменении состояния клиенту Web
  if (changed_keys.length() > 1) {
    SendCurrentState(changed_keys);
    changed_keys.clear();
  }
      
  bool needProcessEffect = true;
  
  #if (USE_E131 == 1)

  // В режиме MASTER или STANDALONE эффект отрабатывать, если устройство не выключено или если сработал будильник
  needProcessEffect = (workMode != SLAVE && !isTurnedOff) || isAlarming || isPlayAlarmSound || e131_wait_command || (workMode == SLAVE && !streaming);
  
  // Если сработал будильник - отрабатывать его эффект, даже если идет стриминг с мастера
  if (workMode == SLAVE && (e131_streaming || e131_wait_command) && (!(isAlarming || isPlayAlarmSound))) {      
    // Если идет прием потока данных с MASTER-устройства - проверить наличие пакета от мастера
    if (e131 && !e131->isEmpty()) {
      // Получен пакет данных. 
      e131_last_packet = millis();
      e131->pull(e131_packet);                
      
      idleTimer.reset();
      
      uint16_t CURRENT_UNIVERSE = htons(e131_packet->universe);      

      /*
      // Отладка: информация о полученном E1.31 пакете
      Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u\n",
                CURRENT_UNIVERSE,                             // The Universe for this packet
                htons(e131_packet->property_value_count) - 1, // Start code is ignored, we're interested in dimmer data
                e131->stats.num_packets,                      // Packet counter
                e131->stats.packet_errors);                   // Packet error counter
      */     
           
      /*
      // Отладка: вывод данных структуры полученного пакета E1.31 для первых двух вселенных
      if (!flag_1 && CURRENT_UNIVERSE == 1 || !flag_2 && CURRENT_UNIVERSE == 2) {
        printE131packet(e131_packet);
        flag_1 |= CURRENT_UNIVERSE == 1;
        flag_2 |= CURRENT_UNIVERSE == 2;
      }
      */
      /*
      Serial.print(CURRENT_UNIVERSE);
      Serial.print(",");
      ccnt++; 
      if (ccnt>60) {
        Serial.println();
        ccnt=0;
      }
      */
      
      bool isCommand = isCommandPacket(e131_packet);
              
      // Если задан расчет FPS выводимых данных потока E1.31 - рассчитать и вывести в консоль
      if (syncMode != COMMAND && E131_FPS_INTERVAL > 0) {
        if (CURRENT_UNIVERSE == START_UNIVERSE && !isCommand) {
          frameCnt++;
        }
        if (millis() - last_fps_time >= E131_FPS_INTERVAL) {
          last_fps_time = millis();
          DEBUG(F("FPS: "));
          DEBUGLN(1000.0 * frameCnt / E131_FPS_INTERVAL);
          frameCnt = 0;
        }
      }

      // Если пакет содержит команду - обработать ее
      if (isCommand) {
        processCommandPacket(e131_packet);          
        needProcessEffect = syncMode == COMMAND;
      } else
      
      // Если режим стрима - PHYSIC или LOGIC - вывести принятые данные на матрицу
      // Физический вывод на матрицу выполнять при получении ПЕРВОГО (НАЧАЛЬНОГО) пакета группы,
      // подразумевая что на предыдущем шаге все принятые пакеты уже разобраны и цвета помещены в массив leds[] - то есть полный кадр сформирован        
      // Для всех остальных пакетов - просто разбирать их и помещать принятые данные части кадра в массив leds[];
      // Если дожидаться последнего пакета группы и только потом выводить - для MASTER матриц чей размер меньше SLAVE - последний пакет не придет никогда
      // и будет создаваться впечатление "зависшей" матрицы - нет вывода. Конечно, в этом случае при несовпадении размеров матриц MASTER и SLAVE
      // будет выведена "каша", но хотя бы видно, что устройство не зависло...
      
      if (syncMode == PHYSIC || syncMode == LOGIC) {          
        if (CURRENT_UNIVERSE == START_UNIVERSE) {
          // При получении первого фрейма группы - вывести на матрицу накопленный кадр
          FastLEDshow();
        }
        // Поместить полученный кадр в буфер светодиодов
        drawE131frame(e131_packet, syncMode);
        yield();
      }
    }
  }

  #endif

  if (needProcessEffect) {
    // Сформировать и вывести на матрицу текущий демо-режим
    // При яркости = 1 остаются гореть только красные светодиоды и все эффекты теряют вид.
    // поэтому отображать эффект "ночные часы"
    if (isTurnedOff && !(isAlarming || isPlayAlarmSound)) {
      // Черный экран - незачем часто обновлять. Делаем задержку в 1000 мс
      if (abs((long long)(millis() - prevShowTimer)) > 1000) {
        FastLED.clear();  
        FastLEDshow();
      }
    } else {
      uint8_t br = specialMode ? specialBrightness : deviceBrightness;
      if (br == 1 && !(loadingFlag || isAlarming || thisMode == MC_TEXT || thisMode == MC_DRAW || thisMode == MC_LOADIMAGE)) {
        if (allowHorizontal || allowVertical) {
          customRoutine(MC_CLOCK);    
        } else {
          // Черный экран - незачем часто обновлять. Делаем задержку в 1000 мс
          if (abs((long long)(millis() - prevShowTimer)) > 1000) {
            FastLED.clear();  
            FastLEDshow();
          }
        }
      } else {            
        customRoutine(thisMode);
      }
    }
  } else 
  #if (USE_E131 == 1)
    if (isTurnedOff && workMode != SLAVE) {
      // Черный экран - незачем часто обновлять. Делаем задержку в 1000 мс
      if (abs((long long)(millis() - prevShowTimer)) > 1000) {
        FastLED.clear();  
        FastLEDshow();
      }
    }
  #else
    if (isTurnedOff) {
      // Черный экран - незачем часто обновлять. Делаем задержку в 1000 мс
      if (abs((long long)(millis() - prevShowTimer)) > 1000) {
        FastLED.clear();  
        FastLEDshow();
      }
    }
  #endif
  
  clockTicker();

  #if (USE_TM1637 == 1)
    if (display != nullptr) {
      if (lastDisplay[0] != currDisplay[0] || lastDisplay[1] != currDisplay[1] || lastDisplay[2] != currDisplay[2] || lastDisplay[3] != currDisplay[3]) {
        display->displayByte(currDisplay[0], currDisplay[1], currDisplay[2], currDisplay[3]);
        lastDisplay[0] = currDisplay[0];
        lastDisplay[1] = currDisplay[1];
        lastDisplay[2] = currDisplay[2];
        lastDisplay[3] = currDisplay[3];
      }
      if (lastDotState != currDotState) {
        display->point(currDotState);
        lastDotState = currDotState;
      }
      if (lastDisplayBrightness != currDisplayBrightness) {
        display->setBrightness(currDisplayBrightness);
        lastDisplayBrightness = currDisplayBrightness;
      }
    }
  #endif    
  
  checkAlarmTime();
  checkAutoMode1Time();
  checkAutoMode2Time();
  checkAutoMode3Time();
  checkAutoMode4Time();
  #if (USE_WEATHER == 1)      
  checkAutoMode5Time();
  checkAutoMode6Time();
  #endif  

  // --------------------- Управление линиями питания -------------------------

  // Прочие клики работают только если не выключено
  #if (USE_POWER == 1)
    if (isTurnedOff) {
      // Выключить питание матрицы
      if (vPOWER_PIN >= 0) {
        if (!isAlarming) {
          if (line_power_state != vPOWER_OFF) digitalWrite(vPOWER_PIN, vPOWER_OFF);
          line_power_state = vPOWER_OFF;
        } else {
          if (line_power_state != vPOWER_ON) digitalWrite(vPOWER_PIN, vPOWER_ON);
          line_power_state = vPOWER_ON;
        }
      }  
    } else {      
      // Включить питание матрицы
      if (vPOWER_PIN >= 0) {
        if (line_power_state != vPOWER_ON) digitalWrite(vPOWER_PIN, vPOWER_ON);
        line_power_state = vPOWER_ON;
      }
    }
  #endif

  #if (USE_ALARM == 1)
    if (vALARM_PIN >= 0) {
      if (!isAlarmStopped && (isPlayAlarmSound || isAlarming || isRemoteAlarm)) {
        if (line_alarm_state != vALARM_ON) digitalWrite(vALARM_PIN, vALARM_ON);
        line_alarm_state = vALARM_ON;
      } else {
        if (line_alarm_state != vALARM_OFF) digitalWrite(vALARM_PIN, vALARM_OFF);
        line_alarm_state = vALARM_OFF;
      }
      // Иногда клиент пропускает сигнал срабатывания будильника или клиент перезагрузился. 
      // Тогда на мастере лампа активного будильника горит, а на клиентах нет. 
      // Во избежание такого сценария послыать активность будильника каждые 1000 мс 
      #if (USE_E131 == 1)
        if (abs((long long)(millis() - last_sent_alarm_sync)) > 1000) {
          commandAlarming(isAlarming || isPlayAlarmSound);
          last_sent_alarm_sync = millis();
        }          
      #endif
    }  
  #endif      

  #if (USE_AUX == 1)
    if (vAUX_PIN >= 0) {
      if (isAuxActive || isRemoteAuxActive) {
        if (line_aux_state != vAUX_ON) digitalWrite(vAUX_PIN, vAUX_ON);
        line_aux_state = vAUX_ON;
      } else {
        if (line_aux_state != vAUX_OFF) digitalWrite(vAUX_PIN, vAUX_OFF);
        line_aux_state = vAUX_OFF;
      }        
      // Иногда клиент пропускает сигнал включения доп.линии или клиент перезагрузился. 
      // Тогда на мастере лампа активного AUX горит, а на клиентах нет. 
      // Во избежание такого сценария послыать активность AUX каждые 1000 мс 
      #if (USE_E131 == 1)
        if (abs((long long)(millis() - last_sent_aux_sync)) > 1000) {
          commandAuxActive(isAuxActive);
          last_sent_aux_sync = millis();
        }        
      #endif
    }  
  #endif      

  // Текущий уровень яркости. Если ведомое устройство перезагрузится когда оно было выключено, а
  // мастер в это время транслирует спец-режим (например ноачные часы), то приемнику не придет яркость
  // и он останется с черным экраном, хотя на самом деле мастер что-то передает
  #if (USE_E131 == 1)
    if (abs((long long)(millis() - last_sent_brightness_sync)) > 1000) {
      uint8_t value = specialMode ? specialBrightness : deviceBrightness;
      commandSetBrightness(value);
      last_sent_brightness_sync = millis();
    }        
  #endif      
      
  // --------------------- Опрос нажатий кнопки -------------------------

  #if (USE_BUTTON == 1)
    if (butt != nullptr) {
      
      // Один клик
      if (butt->isSingle()) clicks = 1;    
      // Двойной клик
      if (butt->isDouble()) clicks = 2;
      // Тройной клик
      if (butt->isTriple()) clicks = 3;
      // Четверной и более клик
      if (butt->hasClicks()) clicks = butt->getClicks();
  
      // Максимальное количество нажатий - 5-кратный клик
      // По неизвестными причинам (возможно ошибка в библиотеке GyverButtons) 
      // getClicks() возвращает 179, что абсолютная ерунда
      if (clicks > 5) clicks = 0;

      if (clicks == 1) {
        if (one_click_time == 0) {
          one_click_time = (uint32_t)millis();
        }
      } else if (clicks > 1) {
        one_click_time = 0;
      }
      
      if (butt->isPress()) {
        // Состояние - кнопку нажали  
      }
      
      if (butt->isRelease()) {
        // Состояние - кнопку отпустили
        isButtonHold = false;
      }
          
      if (clicks > 0 && !clicks_printed) {
        DEBUG(F("Кнопка нажата "));  
        DEBUG(clicks);
        DEBUGLN(F(" раз"));  
        clicks_printed = true;
      }
  
      // Любое нажатие кнопки останавливает будильник
      if ((isAlarming || isPlayAlarmSound) && (isButtonHold || clicks > 0)) {
        DEBUG(F("Кнопка: Выключение будильника"));  
        stopAlarm();
        clicks_printed = false;
        clicks = 0;
      }
      
      bool isHolded = butt->isHolded();
      if (isHolded) {
        clicks = butt->getHoldClicks();
        DEBUGLN(F("Кнопка: удержание"));  
        isButtonHold = true;
      }

      // Одинарный клик - включить / выключить панель
      // Одинарный клик + удержание
      //  из "включено"  - ночные часы; если ночные часы уже включены - включить дневные часы 
      //  из "выключено" - ночник лампа на минимальной яркости
      if (clicks == 1 && (((uint32_t)(millis()) - one_click_time) > 350)) {
        if (isHolded) {          
          if (isTurnedOff) {
            // Клик + удержание в выключенном состоянии - лампа на минимальной яркости
            // Включить панель - белый цвет
            DEBUGLN(F("Кнопка: ночник"));
            isButtonHold = false;
            setSpecialMode(5);
          } else {
            // Клик + удержание во включенном состоянии - включить ночные часы
            // Если ночные часы уже включены - вулюяить обычные часы
            if (isNightClock) {
              DEBUGLN(F("Кнопка: дневные часы"));
              setSpecialMode(4);
            } else {
              DEBUGLN(F("Кнопка: ночные часы"));
              setSpecialMode(3);
            }
          }
          clicks = 255;
          one_click_time = 0;
          clicks_printed = false;
       } else {
         // Однократное короткое нажатие без удержания вкл/выкл лампу
         if (isTurnedOff) { 
           DEBUGLN(F("Кнопка: включение"));
         } else {
           DEBUGLN(F("Кнопка: выключение"));
         }  
         turnOnOff();
         clicks_printed = false;
         clicks = 0;
         one_click_time = 0;
       }          
       return;
     }

     if (isTurnedOff) {
      
        // Включить питание матрицы
        //  - двойной клик из выключенного состояния включает яркий белый свет
        if (clicks == 2) {
          // Включить панель - белый цвет
          DEBUGLN(F("Кнопка: яркая лампа"));
          setSpecialMode(1);
          isButtonHold = false;
          clicks = 0;
          one_click_time = 0;
          return;
        }     
             
     } else {
        
        // Прочие клики работают только если не выключено
        // Включить питание матрицы
        if (clicks == 0 && isHolded) {
          // Управление яркостью - только если нажата и удерживается без предварительного короткого нажатия
          isButtonHold = true;
          if (deviceBrightness == 255)
            brightDirection = false;
          else if (deviceBrightness <= 4) {
            brightDirection = true;
            deviceBrightness = 4;
          } else { 
            brightDirection = !brightDirection;
          }
        }
       
        // Был двойной клик - следующий эффект, сброс автоматического переключения
        if (clicks == 2) {
          DEBUGLN(F("Кнопка: следующий режим"));
          bool tmpSaveSpecial = specialMode;
          resetModes();  
          setManualModeTo(true);
          if (tmpSaveSpecial) {
            setRandomMode();        
          } else {
            nextMode();
          }
          clicks = 0;
          clicks_printed = false;
          return;
        }
  
        // Тройное нажатие - включить случайный режим с автосменой
        else if (clicks == 3) {
          // Включить демо-режим
          DEBUGLN(F("Кнопка: демо-режим"));
          resetModes();          
          setManualModeTo(false);        
          setRandomMode();
          clicks = 0;
          clicks_printed = false;
          return;
        }

        // Четырехкратное нажатие - показать текущий IP WiFi-соединения            
        else if (clicks == 4) {
          DEBUGLN(F("Кнопка: IP-адрес"));
          showCurrentIP();
          clicks = 0;
          clicks_printed = false;
          return;
        }      

        // Пятикратное нажатие - показать версию прошивки
        else if (clicks == 5) {
          DEBUGLN(F("Кнопка: версия прошивки"));
          showCurrentVersion();
          clicks = 0;
          clicks_printed = false;
          return;
        }      
            
        // ... и т.д.
        
        // Обработка нажатой и удерживаемой кнопки
        if (clicks == 0 && isButtonHold && butt->isStep() && thisMode != MC_DAWN_ALARM) {      
          // Удержание кнопки повышает / понижает яркость панели (лампы)
          processButtonStep();
        }            
      }
    }  
  #endif

  // ------------------------------------------------------------------------

  // Проверить - если долгое время не было ручного управления - переключиться в автоматический режим
  if (!(isAlarming || isPlayAlarmSound)) checkIdleState();

  // Если есть несохраненные в EEPROM данные - сохранить их
  if (saveSettingsTimer.isReady()) {
    saveSettings();
  }
}

String getEffectName(int8_t mode) {
  String ef_name;
  switch (mode) {
    case MC_CLOCK:      
      if (isNightClock)
        ef_name = FPSTR(MODE_NIGHT_CLOCK);
      else
        ef_name = FPSTR(MODE_CLOCK);
      break;
    case MC_TEXT:
      ef_name = FPSTR(MODE_RUNNING_TEXT);
      break;
    case MC_LOADIMAGE:
      ef_name = FPSTR(MODE_LOAD_PICTURE);
      break;
    case MC_DRAW:
      ef_name = FPSTR(MODE_DRAW);
      break;
    case -1:      
      ef_name.clear();
      break;
    default:
      // Определить какой эффект включился
      ef_name = GetToken(EFFECT_LIST, mode + 1, ',');
      break;
  } 
  return ef_name;
}

#if (USE_BUTTON == 1)
void processButtonStep() {
  if (isTurnedOff) return;
  if (brightDirection) {
    if (deviceBrightness < 10) set_globalBrightness(deviceBrightness + 1);
    else if (deviceBrightness < 250) set_globalBrightness(deviceBrightness + 5);
    else {
      set_globalBrightness(255);
    }
  } else {
    if (deviceBrightness > 15) set_globalBrightness(deviceBrightness - 5);
    else if (deviceBrightness > 2) set_globalBrightness(deviceBrightness - 1);
    else {
      set_globalBrightness(2);
    }
  }
  set_specialBrightness(deviceBrightness);  
}
#endif

// ********************* ПРИНИМАЕМ ДАННЫЕ **********************

void parsing() {  
  /*
    Протокол связи, посылка начинается с режима. Режимы:
    1 - Включение / выключение устройства
     - $1 0; - выключить
     - $1 1; - включить
     - $1 2; - переключить
    2 - Настройка типа и размера матрицы, угла ее подключения, количество сегментов
       - $2 M0 M1 M2 M3 M4 M5 M6 M7 M8 M9 M11
             M0  - ширина сегмента матрицы 1..128
             M1  - высота сегмента матрицы 1..128
             M2  - тип сегмента матрицы - 0 - зигзаг; 1 _ параллельная; 2 - использовать карту индексов
             M3  - угол подключения диодов в сегменте: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3
             M4  - направление ленты из угла сегмента: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
             M5  - количество сегментов в ширину составной матрицы
             M6  - количество сегментов в высоту составной матрицы
             M7  - соединение сегментов составной матрицы: 0 - зигзаг, 1 - параллельная
             M8  - угол 1-го сегмента мета-матрицы: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
             M9  - направление следующих сегментов мета-матрицы из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
             M11 - индекс текущей карты индексов в случае M2 == 2
    3 - управление играми из приложение WiFi Panel Player
      - $3 0;   включить на устройстве демо-режим
      - $3 1;   включить игру "Лабиринт" в режиме ожидания начала игры
      - $3 2;   включить игру "Змейка" в режиме ожидания начала игры
      - $3 3;   включить игру "Тетрис" в режиме ожидания начала игры
      - $3 4;   включить игру "Арканоид" в режиме ожидания начала игры
      - $3 10 - кнопка вверх
      - $3 11 - кнопка вправо
      - $3 12 - кнопка вниз
      - $3 13 - кнопка влево
      - $3 14 - центральная кнопка джойстика (ОК)
      - $3 15 N; - Скорость повтора нажатия кнопки * 10 (10 .. 100) => 100..1000 мс 
    4 - яркость - 
      $4 0 value   установить текущий уровень общей яркости
    5 - рисование
      - $5 0 RRGGBB; - установить активный цвет рисования в формате RRGGBB
      - $5 1;        - очистить матрицу (заливка черным)
      - $5 2;        - заливка матрицы активным активным цветом рисования 
      - $5 3 X Y;    - рисовать точку активным цветом рисования в позицию X Y
      - $5 4 T N;    - запрос текущей картинки с матрицы в телефон: T - 0 - запрос строки с номером N; T - 1- запрос колонки с номером N
      - $5 5 X;      - запрос списка файлов картинок X: 0 - с FS, 1 - SD
    6 - текст $6 N|some text, где N - назначение текста;
        0 - текст бегущей строки $6 0|X|text - X - 0..9,A..Z - индекс строки
        1 - имя сервера NTP
        2 - SSID сети подключения
        3 - пароль для подключения к сети 
        4 - имя точки доступа
        5 - пароль к точке доступа
        6 - настройки будильников
        7 - строка запрашиваемых параметров для процедуры getStateString(), например - "CE|CC|CO|CK|NC|SC|C1|DC|DD|DI|NP|NT|NZ|NS|DW|OF"
       10 - строка выбранной зоны часового пояса
       11 - картинка построчно $6 11|Y colorHEX,colorHEX,...,colorHEX
       12 - картинка по колонкам $6 12|X colorHEX,colorHEX,...,colorHEX
       14 - текст бегущей строки для немедленного отображения без сохранения $6 14|text
       15 - Загрузить пользовательскую картинку из файла на матрицу; $6 15|ST|filename; ST - "FS" - файловая система; "SD" - карточка
       16 - Сохранить текущее изображение с матрицы в файл $6 16|ST|filename; ST - "FS" - файловая система; "SD" - карточка
       17 - Удалить файл $6 16|ST|filename; ST - "FS" - файловая система; "SD" - карточка
       18 - строка избранных эффектов в порядке следования. Каждый символ строки 0..9,A..Z,a..z - кодирует ID эффекта 0..MAX_EFFECT; Позиция в строке - очередность воспроизведения
       19 - отображаемое имя системы
    8 - эффект
      - $8 0 N; включить эффект N
      - $8 1 N D; D -> параметр #1 для эффекта N;
      - $8 3 N D; D -> параметр #2 для эффекта N;
      - $8 4 N X; вкл/выкл оверлей текста поверх эффекта; N - номер эффекта, X=0 - выкл X=1 - вкл 
      - $8 5 N X; вкл/выкл оверлей часов поверх эффекта; N - номер эффекта, X=0 - выкл X=1 - вкл 
      - $8 6 N D; D -> контрастность эффекта N;
      - $8 7 N;   Запрос текущих параметров эффекта N, возвращается JSON с параметрами. Используется для редактирования эффекта в Web-интерфейсе;
      - $8 8 N D; D -> скорость эффекта N;
    12 - Настройки погоды
      - $12 2 X;   - использовать Цельсий/Фаренгейт: X=0 - Цельсий; X=1 - Фаренгейт
      - $12 3 X;   - использовать цвет для отображения температуры X=0 - выкл X=1 - вкл в дневных часах
      - $12 4 X;   - использовать получение погоды с погодного сервера
      - $12 5 I С C2; - интервал получения погоды с сервера в минутах (I) и код региона C - Yandex и код региона C2 - OpenWeatherMap
      - $12 6 X;   - использовать цвет для отображения температуры X=0 - выкл X=1 - вкл в ночных часах
      - $12 7 X;   - отображение температуры в часах: X - битовая карта, b0 - отображать  C/F, b1 = отображать значок градуса
    13 - Настройки бегущей cтроки  
      - $13 1 N;   - активация прокручивания строки с номером N 0..35
      - $13 2 N;   - запросить текст бегущей строки с индексом N 0..35 как есть, без обработки макросов - ответ TY
      - $13 3 X;   - отображение температуры в макросе {WT}: X - битовая карта, b0 - отображать  C/F, b1 = отображать значок градуса
      - $13 9 D;   - сохранить настройку I - интервал в секундах отображения бегущей строки
      - $13 11 X;  - Режим цвета бегущей строки X: 0,1,2,           
      - $13 13 X;  - скорость прокрутки бегущей строки
      - $13 15 00FFAA; - цвет текстовой строки для режима "монохромный", сохраняемый в globalTextColor
      - $13 16;    - Команда "Импорт текстовых строк"
      - $13 17;    - Команда "Экспорт текстовых строк"
      - $13 18 X;  - сохранить настройку X "Бегущая строка в эффектах" (общий, для всех эффектов), где X: 0 - отключено; 1 - включено
    14 - быстрая установка ручных режимов с пред-настройками
       - $14 0;  Черный экран (выкл);  
       - $14 1;  Белый экран (освещение);  
       - $14 2;  Цветной экран;  
       - $14 3;  Часы ночные;
       - $14 4;  Часы простые;
       - $14 5;  Ночник (белая лампа на минимальной яркости);
       - $14 6;  Выключение ночных часов - включить тот эффект что был до включения, восстановить состояние "Автосмена режима";
    15 - скорость $15 скорость таймер; 0 - таймер эффектов
    16 - Режим смены эффектов: $16 N; 
       - $16 0; - ручной режим;  
       - $16 1; - авторежим; 
       - $16 2; - PrevMode; 
       - $16 3; - NextMode; 
       - $16 5; - вкл/выкл случайный выбор следующего режима
    17 - Время автосмены эффектов и бездействия: $17 сек сек;
    18 - Отладка вывода часов / календаря / температуры;
      - $18 1 HH MM TT DD MM YYYY; - Установить указанное время HH:MM и температуру TT - для отладки позиционирования часов с температурой
      - $18 2 X; - Вкл/выкл отображение креста
      - $18 3 X; - Сдвиг позиции вывода часов при скроллинге на X колонок
      - $18 4;   - Сброс позиции вывода часов при скроллинге в начальную позицию
      - $18 5 X; - Вкл/выкл отображение рамки областей вывода часов/температуры/календаря - полной и фактической
    19 - работа с настройками часов
      - $19 1 X;   - сохранить настройку X "Часы в эффектах"
      - $19 2 X;   - Использовать синхронизацию часов NTP  X: 0 - нет, 1 - да
      - $19 4 X;   - Выключать индикатор TM1637 при выключении экрана X: 0 - нет, 1 - да
      - $19 5 X;   - Режим цвета часов оверлея X: 0,1,2,3
      - $19 6 X;   - Ориентация часов  X: 0 - горизонтально, 1 - вертикально
      - $19 7 X;   - Размер часов X: 0 - авто, 1 - малые 3х5, 2 - большие 5x7
      - $19 8 YYYY MM DD HH MM; - Установить текущее время YYYY.MM.DD HH:MM
      - $19 9 X Y;  - Флаги отображения часов/календаря в циклах T1-T4  X: часы - младшие 4 бита, календарт - старшие 4 бита; Y - календарь - младшие 4 бита
      - $19 10 X;  - Цвет ночных часов:  0 - R; 1 - G; 2 - B; 3 - C; 3 - M; 5 - Y; 6 - W;
      - $19 11 X;  - Яркость ночных часов:  0..255
      - $19 12 X;  - скорость прокрутки часов оверлея или 0, если часы остановлены по центру
      - $19 13 X;  - Показывать 2/4 цифры годв  X: 0 или 2 или 4; в одну или в две строки
      - $19 14 00FFAA; - цвет часов оверлея, сохраняемый в globalClockColor
      - $19 15 X;  - флаги выраснивани часов/календаря/температуры в области их отображения
      - $19 16 X;  - флаги вариантов отображения селандаря и темепратур ы - относительно часов или произвольно по указанномусмещению
      - $19 17 T1 T2; - Продолжительность отображения даты / часов в секундах - время интервалов фазы T1/T3 и T2/T4
      - $19 18 X;   - флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру b3b4b5b6b7 - зарезервировано
      - $19 19 X;   - Показывать время в 12/24 часовом формате
      - $19 20 X;   - Показывать малые часы шрифтом 3х5 - 0 квадратный шрифт, 1 - круглый шрифт
      - $19 21 X Y; - Смещение часов по X,Y
      - $19 22 X Y; - Ширина разделительных точек в больших часах (X = 1|2), Y - есть ли пробелы между цифрами часов и точками 0|1
      - $19 23 X Y; - Смещение температуры по X,Y
      - $19 24 X Y; - Смещение календаря по X,Y
      - $19 25 Y;   - Смещение текста по Y
    20 - настройки и управление будильников
      - $20 0;       - отключение будильника (сброс состояния isAlarming)
      - $20 2 X VV MA MB;
           X    - исп звук будильника X=0 - нет, X=1 - да 
          VV    - максимальная громкость
          MA    - номер файла звука будильника
          MB    - номер файла звука рассвета
      - $20 3 X NN VV; - пример звука будильника
           X - 1 играть 0 - остановить
          NN - номер файла звука будильника из папки SD:/01
          VV - уровень громкости
      - $20 4 X NN VV; - пример звука рассвета
           X - 1 играть 0 - остановить
          NN - номер файла звука рассвета из папки SD:/02
          VV - уровень громкости
      - $20 5 VV; - установить уровень громкости проигрывания примеров (когда уже играет)
          VV - уровень громкости
    21 - настройки подключения к сети / точке доступа
      - $21 0 X; - использовать точку доступа: X=0 - не использовать X=1 - использовать
      - $21 1 IP1 IP2 IP3 IP4; - установить статический IP адрес подключения к локальной WiFi сети, пример: $21 1 192 168 0 106
      - $21 2;   - Выполнить переподключение к сети WiFi
      - $21 3 X; - Использовать DHCP X=0|1
    22 - настройки включения режимов матрицы в указанное время
       - $22 HH1 MM1 NN1 HH2 MM2 NN2 HH3 MM3 NN3 HH4 MM4 NN4;
             HHn - час срабатывания
             MMn - минуты срабатывания
             NNn - эффект: -3 - выключено; -2 - выключить матрицу; -1 - ночные часы; 0 - случайный режим и далее по кругу; 1 и далее - список режимов EFFECT_LIST 
    23 - прочие настройки
       - $23 0 VAL;  - лимит по потребляемому току
       - $23 1 ST;   - Сохранить EEPROM в файл    ST = 0 - внутр. файл. систему; 1 - на SD-карту
       - $23 2 ST;   - Загрузить EEPROM из файла  ST = 0 - внутр. файл. системы; 1 - на SD-карты
       - $23 3 E1 E2 E3;   - Установка режима работы панели и способа трактовки полученных данных синхронизации
             E1 - режим работы 0 - STANDALONE; 1 - MASTER; 2 - SLAVE
             E2 - данные в кадре: 0 - физическое расположение цепочки диодов
                                  1 - логическое расположение - X,Y - 0,0 - левый верхний угол и далее вправо по X, затем вниз по Y
                                  2 - payload пакета - строка команды
             E3 - группа синхронизации 0..9                      
       - $23 4; - перезапуск устройства (reset/restart/reboot)
       - $23 5 MX, MY, LX, LY, LW, LH; - настройка окна отображения трансляции с MASTER на SLAVE
             MX - логическая координата X мастера с которого начинается вывод на локальную матрицу    X,Y - левый верхний угол начала трансляции мастера
             MY - логическая координата Y мастера с которого начинается вывод на локальную матрицу
             LX - логическая координата X приемника на которую начинается вывод на локальную матрицу  X,Y - левый верхний угол начала отображения на приемнике
             MY - логическая координата Y приемника на которого начинается вывод на локальную матрицу
             LW - ширина окна отображения на локальной матрице
             LH - высота окна отображения на локальной матрице
        - $23 6 U1,P1,S1,L1,C1; - подключение матрицы светодиодов линия 1
        - $23 7 U2,P2,S2,L2,C2; - подключение матрицы светодиодов линия 2
        - $23 8 U3,P3,S3,L3,C3; - подключение матрицы светодиодов линия 3
        - $23 9 U4,P4,S4,L4,C4; - подключение матрицы светодиодов линия 4
             Ux - 1 - использовать линию, 0 - линия не используется
             Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
             Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
             Lx - длина цепочки светодиодов, подключенной к линии x
        - $23 10 X; - X - DEVICE_TYPE - 0 - труба, 1 - плоская панель
        - $23 11 X Y; - кнопка 
             X - GPIO пин к которому подключена кнопка
             Y - BUTTON_TYPE - 0 - сенсорная кнопка, 1 - тактовая кнопка
        - $23 12 X Y; - управление питанием матрицы
             X - GPIO пин к которому подключено реле управления питанием
             Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
        - $23 13 X Y; - для SD-карты - алгоритм воспроизведения ролика
             X - WAIT_PLAY_FINISHED - алгоритм проигрывания эффекта SD-карта
             Y - REPEAT_PLAY - алгоритм проигрывания эффекта SD-карта
        - $23 14 X;  - Вкл/выкл отладочного вывода в монитор порта DEBUG_SERIAL - 1-вкл, 0-выкл 
        - $23 15 X Y;  - Подключение пинов MP3 DFPlayer
             X - STX - GPIO пин контроллера TX -> к RX плеера
             Y - SRX - GPIO пин контроллера RX -> к TX плеера
        - $23 16 X Y;  - Подключение пинов TM1637
             X - DIO - GPIO пин контроллера к DIO индикатора
             Y - CLK - GPIO пин контроллера к CLK индикатора
        - $23 17 X Y; - управление питанием по линии будтльника
             X - GPIO пин к которому подключено реле управления питанием
             Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
        - $23 18 X Y; - управление питанием по дополнительной линии
             X - GPIO пин к которому подключено реле управления питанием
             Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
        - $23 19 VAL;  - ВКЛ/ВЫКЛ дополнительную линию питания; 0 - выключить; 1 - включить
        - $23 20 VAL;  - битовая маска состояния режимов управления дополнительным каналом питания по времени
  */  

  // ****************** ОБРАБОТКА *****************
  String  incomeBuffer, str, str1, str2, pictureLine;

  bool    err = false;
  int32_t pntX, pntY, pntColor, idx;
  uint8_t alarmHourVal, alarmMinuteVal, b_tmp{0};
  int8_t  tmp_eff, tmp_idx;

  // ****************** ПАРСИНГ *****************

  // Есть поступившие команды в очереди?
  if (queueLength == 0) return;

  // Извлечь
  incomeBuffer = cmdQueue[queueReadIdx++];
  if (queueReadIdx >= QSIZE_IN) queueReadIdx = 0;
  queueLength--;
  
  yield();

  // Это команда (признак начала '$')?
  if (incomeBuffer.length() == 0 || incomeBuffer.charAt(0) != '$') return;

  // "$6 7|FM|UP" - 'та команда используется в качестве ping, посылается каждые 2-3 секунда, ее выводить не нужно, чтобы не забивать вывод в лог
  if (incomeBuffer != "$6 7|FM|UP") {

    DEBUG(F("WEB пакeт размером "));
    DEBUGLN(incomeBuffer.length());

    DEBUG(F("<-- "));
    DEBUGLN(incomeBuffer);

    // При поступлении каждой команды вывести в консоль информацию о свободной памяти
    printMemoryInfo();
  }
  
  // Блок команд $6 содержит переданные строки      
  if (incomeBuffer.charAt(1) == '6') {                         // '$6 N|текст' если нужно принять строку - принимаем всю
    
    // Получена строка - используем ее как есть без парсинга.
    // Разбор будет выполнен в блоке обработки $6      
    intData[0] = 6;      
    
  } else {
    
    // Удалить завершающий символ - ';'
    incomeBuffer.setCharAt(incomeBuffer.length() - 1, '\0');
    
    // Прочие команды представляют собой набор целых чисел, разделенных пробелами
    // Некоторые команды могут содержать в себе цвет. Цвет будет извлечен в блоке обработки этой команды.
    // При парсинге этой части - будет 0 <-- '$5 0 RRGGBB;' - '$13 15 00FFAA;' - '$19 14 00FFAA;'
    // В строке в настоящее время может быть только одна команда, начинающаяся с '$' и заканчивающаяся ';'
    // Все остальное нужно сложить в массив intData[] -> intData[0] - номер группы команд; intData[1] - команда; intData[2] и далее - параметры команды
    
    for (uint8_t i = 0; i < PARSE_AMOUNT; i++) intData[i] = 0;

    idx  = incomeBuffer.indexOf(' ');
    if (idx == -1) return;

    uint8_t param_idx = 1;
    String str, str1; 
    
    str = incomeBuffer.substring(1, idx);
    str.trim();
    intData[0] = str.toInt();

    str = incomeBuffer.substring(idx + 1);
    str1.reserve(10);
    
    while (str.length() > 0) {
      idx = str.indexOf(' ');
      if (idx == -1) {
        str.trim();
        intData[param_idx++] = str.toInt();
        break;
      } else {
        str1 = str.substring(0, idx);        
        str = str.substring(idx + 1);
        str.trim();
        str1.trim();
        intData[param_idx++] = str1.toInt();
      }      
    }
  }

  // Известные команды - в диапазоне 1..23
  if (intData[0] < 1 || intData[0] > 23) {
    notifyUnknownCommand(incomeBuffer);
    return;
  }

  // Режимs 18 и 6.7  не сбрасывают idleTimer
  // Другие режимы сбрасывают таймер бездействия
  if (!(intData[0] == 18 || (intData[0] == 6 && intData[1] == 7))) {
    idleTimer.reset();
  }

  switch (intData[0]) {

    // ----------------------------------------------------
    // 1 - Включение / выключение устройства
    //   - $1 0; - выключить
    //   - $1 1; - включить
    //   - $1 2; - переключить

    case 1:
      if (intData[1] == 0) {
        // Выключить устройство
        turnOff();
      } else
      if (intData[1] == 1) {
        // Включить устройство
        turnOn();
      } else
      if (intData[1] == 2) {
        // Переключить устройство
        turnOnOff();
      } 
      else {
        err = true;
        notifyUnknownCommand(incomeBuffer);
      }
              
      break;

    // ----------------------------------------------------
    // 2 - Настройка типа и размера матрицы, угла ее подключения, количество сегментов
    //     - $2 M0 M1 M2 M3 M4 M5 M6 M7 M8 M9 M11
    //       M0  - ширина сегмента матрицы 1..128
    //       M1  - высота сегмента матрицы 1..128
    //       M2  - тип сегмента матрицы - 0 - зигзаг; 1 - параллельная; 2 - карта индексов
    //       M3  - угол подключения диодов в сегменте: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
    //       M4  - направление ленты из угла сегмента: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
    //       M5  - количество сегментов в ширину составной матрицы
    //       M6  - количество сегментов в высоту составной матрицы
    //       M7  - соединение сегментов составной матрицы: 0 - зигзаг, 1 - параллельная
    //       M8  - угол 1-го сегмента мета-матрицы: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
    //       M9  - направление следующих сегментов мета-матрицы из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
    //       M11 - индекс файла карты индексов в массиве mapFiles (при M2 == 2)

    case 2:
      // Установить новые параметры, которые вступят в силу после перезагрузки устройства
      putMatrixSegmentWidth(intData[1]);
      putMatrixSegmentHeight(intData[2]);
      putMatrixSegmentType(intData[3]);
      putMatrixSegmentAngle(intData[4]);
      putMatrixSegmentDirection(intData[5]);
    
      putMetaMatrixWidth(intData[6]);
      putMetaMatrixHeight(intData[7]);
      putMetaMatrixType(intData[8]);
      putMetaMatrixAngle(intData[9]);
      putMetaMatrixDirection(intData[10]);

      idx = (byte)intData[11];
      if (mapListLen > 0 && idx >= 0 && idx < mapListLen) {
        String file_name(mapFiles[idx]); // 32x16
        int8_t p = file_name.lastIndexOf('x');
        if (p >= 0) {
          mapWIDTH = file_name.substring(0, p).toInt();
          mapHEIGHT = file_name.substring(p + 1).toInt();
          if (mapWIDTH < 8) mapWIDTH = 8;
          if (mapHEIGHT < 8) mapHEIGHT = 8;
          if (mapWIDTH > 128) mapWIDTH = 128;
          if (mapHEIGHT > 128) mapHEIGHT = 128;
          putMatrixMapWidth(mapWIDTH);
          putMatrixMapHeight(mapHEIGHT);
        }
      }

      // Сохранить измененные настройки
      saveSettings();

      needRestart = true;
      needRestartTime = millis();
      break;

    // ----------------------------------------------------
    // 3 - управление играми из приложение WiFi Panel Player
    //   - $3 0;  включить на устройстве демо-режим
    //   - $3 1;  включить игру "Лабиринт" в режиме ожидания начала игры
    //   - $3 2;  включить игру "Змейка" в режиме ожидания начала игры
    //   - $3 3;  включить игру "Тетрис" в режиме ожидания начала игры
    //   - $3 4;  включить игру "Арканоид" в режиме ожидания начала игры
    //   - $3 10    - кнопка вверх
    //   - $3 11    - кнопка вправо
    //   - $3 12    - кнопка вниз
    //   - $3 13    - кнопка влево
    //   - $3 14    - центральная кнопка джойстика (ОК)
    //   - $3 15 N; - скорость повтора нажатия кнопки * 10 (10 .. 100) => 100..1000 мс 
    // ----------------------------------------------------

    case 3:
      if (intData[1] == 0) {
        // Включить на устройстве демо-режим
        FastLED.clear();
        FastLEDshow();
        delay(50);
        setManualModeTo(false);
        set_CurrentSpecialMode(-1);
        nextMode();
      } else
      if (intData[1] == 1) {
        // Игра "Лабиринт"
        setManualModeTo(true);
        setEffect(MC_MAZE);
        // Установили игру в ручном режиме. Для игры в ручном режиме скорость таймера должна быть минимальна для 
        // максимально быстрой реакции на кнопки. Обычное включение игры в демо-режиме будет использовать настроенный таймер скорости игры
        effectTimer.setInterval(10);
        gameDemo = false;
        gamePaused = true;
        set_CurrentSpecialMode(-1);          
      } else
      if (intData[1] == 2) {
        // Игра "Змейка"
        setManualModeTo(true);
        setEffect(MC_SNAKE);
        gameDemo = false;
        gamePaused = true;
        set_CurrentSpecialMode(-1);
      } else
      if (intData[1] == 3) {
        // Игра "Тетрис"
        setManualModeTo(true);
        setEffect(MC_TETRIS);
        gameDemo = false;
        gamePaused = true;
        set_CurrentSpecialMode(-1);
      } else
      if (intData[1] == 4) {
        // Игра "Арканоид"
        setManualModeTo(true);
        setEffect(MC_ARKANOID);
        gameDemo = false;
        gamePaused = true;
        set_CurrentSpecialMode(-1);
      } else
      if (intData[1] == 10) {
        // кнопка вверх
        buttons = 0;
        gamePaused = false;
      } else
      if (intData[1] == 11) {
        // кнопка вправо
        buttons = 1;
        gamePaused = false;
      } else
      if (intData[1] == 12) {
        // кнопка вниз
        buttons = 2;
        gamePaused = false;
      } else
      if (intData[1] == 13) {
        // кнопка влево
        buttons = 3;
        gamePaused = false;
      } else
      if (intData[1] == 14) {
        // кнопка влево
        buttons = 5;
        gamePaused = false;
      } else
      if (intData[1] == 15) {
        // кнопка влево
        set_GameButtonSpeed(intData[2]);
        gamePaused = false;
      }
      else {
        err = true;
        notifyUnknownCommand(incomeBuffer);
      }        
              
      break;

    // ----------------------------------------------------
    // 4 - яркость - 
    //  $4 0 value   установить текущий уровень общей яркости / яркости ночных часов
    // ----------------------------------------------------
    
    case 4:
      if (intData[1] == 0) {
        // При включенном режиме ночных часов ползунок яркости регулирует только яркость ночных часов
        // Для прочих режимов - общую яркость системы
        if (isNightClock) {
          set_nightClockBrightness(intData[2]); // setter            
          set_specialBrightness(nightClockBrightness);
        } else {
          set_globalBrightness(intData[2]);
          if (specialMode) set_specialBrightness(deviceBrightness);
          if (thisMode == MC_DRAW || thisMode == MC_LOADIMAGE) {
            FastLEDshow();
          }
        }
      } else {
        err = true;
        notifyUnknownCommand(incomeBuffer);
      }
              
      break;

    // ----------------------------------------------------
    // 5 - рисование
    //   $5 0 RRGGBB; - установить активный цвет рисования в формате RRGGBB
    //   $5 1;        - очистить матрицу (заливка черным)
    //   $5 2;        - заливка матрицы активным активным цветом рисования 
    //   $5 3 X Y;    - рисовать точку активным цветом рисования в позицию X Y
    //   $5 4 T N;    - запрос текущей картинки с матрицы в телефон: T - 0 - запрос строки с номером N; T - 1 - запрос колонки с номером N
    //   $5 5 X;      - запрос списка файлов картинок X: 0 - с FS, 1 - SD
    //   $5 6 X Y;    - рисовать точку черным в позицию X Y (стереть точку - ластик)
    // ----------------------------------------------------

    case 5:
      if (thisMode != MC_DRAW) {
        set_thisMode(MC_DRAW);
        setManualModeTo(true);
      }      
      if (intData[1] == 0) {
        // Установить активный цвет рисования - incomeBuffer = '$5 0 00FF00;'
        set_drawColor(HEXtoInt(incomeBuffer.substring(5,11)));
      } else
      if (intData[1] == 1) {
        // Очистка матрицы (заливка черным)
        FastLED.clear();
      } else
      if (intData[1] == 2) {
        // Залить матрицу активным цвет ом рисования
        fillAll(gammaCorrection(drawColor));
      } else
      if (intData[1] == 3) {
        // Нарисовать точку активным цветом рисования в позиции X Y
        drawPixelXY(intData[2], intData[3], gammaCorrection(drawColor));
      } else
      if (intData[1] == 4) {
        // intData[2] - T=0 - запрос строки; T=1 - запрос колонки; 
        // intData[3] - номер строки / колонки; 
        sendImageLine((uint8_t)intData[2], (uint8_t)intData[3]);
      } else
      if (intData[1] == 5) {
        String tmp("FL"); tmp += (intData[2] == 0 ? 0 : 1);               
        addKeyToChanged(tmp);
      } else
      if (intData[1] == 6) {
        // Нарисовать точку черным в позиции X Y - стереть точку, инструмент ластик
        drawPixelXY(intData[2], intData[3], 0);
      } else {
        err = true;
        notifyUnknownCommand(incomeBuffer);
      }
      break;

    // ----------------------------------------------------
    // 6 - прием строки: строка принимается в формате N|text, где N:
    //   0 - принятый текст бегущей строки $6 0|X|text - X - 0..9,A..Z - индекс строки
    //   1 - имя сервера NTP
    //   2 - имя сети (SSID)
    //   3 - пароль к сети
    //   4 - имя точки доступа
    //   5 - пароль точки доступа
    //   6 - настройки будильника в формате $6 6|DD EF WD HH1 MM1 HH2 MM2 HH3 MM3 HH4 MM4 HH5 MM5 HH6 MM6 HH7 MM7        
    //   7 - строка запрашиваемых параметров, например - "$6 7|CE CC CO CK NC SC C1 DC DD DI NP NT NZ NS DW OF"
    //  10 - строка выбранной зоны часового пояса
    //  11 - картинка построчно $6 11|Y colorHEX,colorHEX,...,colorHEX
    //  12 - картинка по колонкам $6 12|X colorHEX,colorHEX,...,colorHEX
    //  14 - текст бегущей строки для немедленного отображения без сохранения $6 14|text
    //  15 - Загрузить пользовательскую картинку из файла на матрицу; $6 15|ST|filename; ST - "FS" - файловая система; "SD" - карточка
    //  16 - Сохранить текущее изображение с матрицы в файл $6 16|ST|filename; ST - "FS" - файловая система; "SD" - карточка
    //  17 - Удалить файл $6 16|ST|filename; ST - "FS" - файловая система; "SD" - карточка
    //  18 - строка избранных эффектов в порядке следования. Каждый символ строки 0..9,A..Z,a..z - кодирует ID эффекта 0..MAX_EFFECT; Позиция в строке - очередность воспроизведения
    //  19 - отображаемое имя системы
    // ----------------------------------------------------

    case 6:
      tmp_idx = incomeBuffer.indexOf('|');                     // '$6 12|text'
      if (tmp_idx > 0) {

         str = incomeBuffer.substring(1, tmp_idx);             // '6 12';
         idx = str.indexOf(' ');                              
         b_tmp = incomeBuffer.substring(idx + 1).toInt();      // '12' -> 12
         str = incomeBuffer.substring(tmp_idx + 1);            // 'text'
         
         switch(b_tmp) {

          case 0: {
              char c = str.charAt(0);
              str = str.substring(2);
              str.replace('\r', ' ');
              str.trim();              
              tmp_eff = getTextIndex(c); // c: '0'..'9','A'..'Z' -> 0..35
              if (tmp_eff == 0) {
                textIndecies = getTextIndeciesLine(str);
                if (!isFirstLineControl())  {
                  textIndecies.clear();
                  sequenceIdx = -1;
                  saveTextLine(c, str);
                } else {
                  saveTextLine(c, str);
                }                 
              } else if (tmp_eff > 0) {
                saveTextLine(c, str);
              }
              textStates[tmp_eff] = getTextStateChar(tmp_eff, str);
              
              preparedTextIdx = tmp_eff;
              preparedText = str;
  
              // Если сохраняемая строка содержит макрос {P} - зависящий от времени - выполнить перерасчет времени наблюдаемых событий
              if (str.indexOf("{P") >= 0) {          
                // Если строки еще нет в списке индексов строк, содержащих макрос {P} - добавить      
                if (textWithEvents.indexOf(c) < 0) {
                  textWithEvents += c;
                }
                rescanTextEvents();
              } else {
                // Если макроса {P} в строке нет, но она есть в списке индексов строк, содержащих макрос {P} - удалить
                if (textWithEvents.indexOf(c) >= 0) {                 
                  textWithEvents.replace(String(c), "");
                }
              }
  
              // Скорректировать список непустых строк. Поиск какую строку показывать следующей будет
              // выполняться по этому списку, а не перебором всех строк с открытием и чтением содержимого файла) в поисках не пустой для сокращения времени выполнения операции.
              if (str.length() > 0) {
                if (textsNotEmpty.indexOf(c) < 0) {
                  textsNotEmpty += c;
                }
              } else {
                if (textsNotEmpty.indexOf(c) >= 0) {                 
                  textsNotEmpty.replace(String(c), "");
                }                
              }
  
              // Отправить уведомление об изменениях в строке
              editIdx = tmp_eff;
              addKeyToChanged("TY");         // Отправить также строку в канал WEB
              addKeyToChanged("TS");         // Текст в строке мог быть изменен - отправить в канал WEB состояние строк
            }
            break;

          case 1:
            set_ntpServerName(str);
            break;

          case 2:
            set_Ssid(str);
            break;

          case 3:
            set_pass(str);
            break;

          case 4:
            set_SoftAPName(str);
            break;

          case 5:
            set_SoftAPPass(str);
            // Передается в одном пакете - использовать SoftAP, имя точки и пароль
            // После получения пароля - перезапустить создание точки доступа
            if (useSoftAP) startSoftAP();
            break;
            
          case 6:
            // Настройки будильника в формате $6 6|DD EF WD AD HH1 MM1 HH2 MM2 HH3 MM3 HH4 MM4 HH5 MM5 HH6 MM6 HH7 MM7
            // DD    - установка продолжительности рассвета (рассвет начинается за DD минут до установленного времени будильника)
            // EF    - установка эффекта, который будет использован в качестве рассвета
            // WD    - установка дней пн-вс как битовая маска
            // AD    - продолжительность "звонка" сработавшего будильника
            // HHx   - часы дня недели x (1-пн..7-вс)
            // MMx   - минуты дня недели x (1-пн..7-вс)
            //
            // Остановить будильник, если он сработал
            #if (USE_MP3 == 1)
            if (isDfPlayerOk) {
              dfPlayer.stop(); Delay(GUARD_DELAY);
            }
            set_soundFolder(0);
            set_soundFile(0);
            #endif
            
            set_isAlarming(false);
            set_isAlarmStopped(false);

            // Настройки содержат 18 элементов (см. формат выше)
            tmp_eff = CountTokens(str, ' ');
            if (tmp_eff == 18) {
            
              set_dawnDuration(constrain(GetToken(str, 1, ' ').toInt(),1,59));
              set_alarmEffect(GetToken(str, 2, ' ').toInt());
              set_alarmWeekDay(GetToken(str, 3, ' ').toInt());
              set_alarmDuration(constrain(GetToken(str, 4, ' ').toInt(),1,10));
              
              for(uint8_t i=0; i<7; i++) {
                alarmHourVal = constrain(GetToken(str, i*2+5, ' ').toInt(), 0, 23);
                alarmMinuteVal = constrain(GetToken(str, i*2+6, ' ').toInt(), 0, 59);
                set_alarmTime(i+1, alarmHourVal, alarmMinuteVal);
              }

              // Рассчитать время начала рассвета будильника
              calculateDawnTime();            
            }
            break;
            
          case 7:
            // Запрос значений параметров, требуемых приложением вида str="CE|CC|CO|CK|NC|SC|C1|DC|DD|DI|NP|NT|NZ|NS|DW|OF"
            // Каждый запрашиваемый приложением параметр - для заполнения соответствующего поля в приложении 
            // Передать строку для формирования, затем отправить параметры в приложение
            //
            // Сейчас строка с запросом не помещается в очередь обработки, а обрабатывается сразу
            // после приема из Web-сокета в web.ino -> handleWebSocketMessage()            
            break;
            
          // Прием строки зоны часового пояса
          case 10:
            set_ntpTimeZone(str);
            // Пересчитать / скорректировать время срабатывания будильников
            calculateDawnTime();      
            // Пересчетать очередь ближайших событий {P} в бегущей строке
            rescanTextEvents();
            break;

          // Прием строки передаваемого отображения по строкам  $6 11|Y colorHEX,colorHEX,...,colorHEX;
          case 11:
            pictureLine = str; pictureLine += ',';
            
            if (thisMode != MC_LOADIMAGE) {
              setManualModeTo(true);
              set_thisMode(MC_LOADIMAGE);
              FastLED.clear();
              FastLEDshow();
              delay(50);
            }
    
            // Разбираем СТРОКУ из принятого буфера формата 'Y colorHEX,colorHEX,...,colorHEX'
            // Получить номер строки (Y) для которой получили строку с данными (номер строки - сверху вниз, в то время как матрица - индекс строки снизу вверх)
            b_tmp = pictureLine.indexOf(' ');
            str = pictureLine.substring(0, b_tmp);
            pntX = 0;
            pntY = str.toInt();
            pictureLine = pictureLine.substring(b_tmp+1);

            // начало картинки - очистить матрицу
            if (pntY == 0) {
              FastLED.clear(); 
              FastLEDshow();
            }
    
            idx = pictureLine.indexOf(',');
            while (idx > 0 && pntX < pWIDTH) {
              str = pictureLine.substring(0, idx);
              pictureLine = pictureLine.substring(idx + 1);
              pntColor = HEXtoInt(str);
              drawPixelXY(pntX, pHEIGHT - pntY - 1, gammaCorrection(pntColor));
              pntX++;
            }
            
            // Прием строки закончен - вывести матрицу              
            FastLEDshow();
            
            // Строка подтверждения приема строки изображения              
            SendWebKey("L", String(pntY));

            break;

          // Прием строки передаваемого отображения по колонкам $6 12|X colorHEX,colorHEX,...,colorHEX;
          case 12:
            pictureLine = str; pictureLine += ',';

            if (thisMode != MC_LOADIMAGE) {
              setManualModeTo(true);
              set_thisMode(MC_LOADIMAGE);
              FastLED.clear();
              FastLEDshow();
              delay(50);
            }
    
            // Разбираем СТРОКУ из принятого буфера формата 'X colorHEX,colorHEX,...,colorHEX'
            // Получить номер колонки (X) для которой получили строку с данными (номер строки - сверху вниз, в то время как матрица - индекс строки снизу вверх)
            b_tmp = pictureLine.indexOf(' ');
            str = pictureLine.substring(0, b_tmp);
            pntY = 0;
            pntX = str.toInt();
            pictureLine = pictureLine.substring(b_tmp+1);

            // начало картинки - очистить матрицу
            if (pntX == 0) {
              FastLED.clear(); 
              FastLEDshow();
            }

            idx = pictureLine.indexOf(',');
            while (idx > 0 && pntY < pHEIGHT) {
              str = pictureLine.substring(0, idx);
              pictureLine = pictureLine.substring(idx + 1);
              pntColor = HEXtoInt(str);
              drawPixelXY(pntX, pHEIGHT - pntY - 1, gammaCorrection(pntColor));
              pntY++;
            }

            // Прием строки закончен - вывести матрицу              
            FastLEDshow();

            // Строка подтверждения приема строки изображения              
            SendWebKey("C", String(pntX));
            break;

          case 14:
            // текст бегущей строки для немедленного отображения без сохранения               
            setImmediateText(str);
            break;

          case 15:
            // Загрузка картинки: str = "TS|file", где TS - "FS" - внутр. память МК, "SD" - SD-карта; file - имя картинки без расширения
            if (thisMode != MC_DRAW) {
              set_thisMode(MC_DRAW);
              setManualModeTo(true);
            }      
            str1 = USE_SD == 1 && FS_AS_SD == 0 ? str.substring(0,2) : String(F("FS")); // хранилище: если нет реальной SD-карты (эмуляция в FS) - использовать хранилище FS
            str2 = str.substring(3);                                                    // Имя файла картинки без расширения
            str = openImage(str1, str2, nullptr, false, pWIDTH, pHEIGHT);
            // Отправка уведомления или сообщения об ошибке в Web канал
            if (str.length() == 0) {
              // Файл загружен
              SendWebInfo(MSG_FILE_LOADED);
            } else {
              // Ошибка загрузки файла
              SendWebError(str);
            }
            break;

          case 16:
            // Сохранение картинки: str = "TS|file", где TS - "FS" - внутр. память МК, "SD" - SD-карта; file - имя картинки без расширения
            if (thisMode != MC_DRAW) {
              set_thisMode(MC_DRAW);
              setManualModeTo(true);
            }      
            str1 = USE_SD == 1 && FS_AS_SD == 0 ? str.substring(0,2) : String(F("FS")); // хранилище: если нет реальной SD-карты (эмуляция в FS) - использовать хранилище FS
            str2 = str.substring(3);                                                    // Имя файла картинки без расширения
            str = saveImage(str1, str2, pWIDTH, pHEIGHT);              
            // Отправка уведомления или сообщения об ошибке в Web канал
            if (str.length() == 0) {
              // Файл сохранен
              SendWebInfo(MSG_FILE_SAVED);
            } else {
              // Ошибка сохранения файла
              SendWebError(str);
            }
            break;

          case 17:
            // Удаление файла: str = "TS|file", где TS - "FS" - внутр. память МК, "SD" - SD-карта; file - имя картинки без расширения
            if (thisMode != MC_DRAW) {
              set_thisMode(MC_DRAW);
              setManualModeTo(true);
            }      
            str1 = USE_SD == 1 && FS_AS_SD == 0 ? str.substring(0,2) : String(F("FS")); // хранилище: если нет реальной SD-карты (эмуляция в FS) - использовать хранилище FS
            str2 = str.substring(3);                                                    // Имя файла картинки без расширения
            str = deleteImage(str1, str2, pWIDTH, pHEIGHT);              
            // Отправка уведомления или сообщения об ошибке в Web канал
            // При успешно завершившейся операции возвращается пустая строка
            if (str.length() == 0) {
              // Файл удален
              SendWebInfo(MSG_FILE_DELETED);
            } else {
              // Ошибка удаления файла
              SendWebError(str);
            }
            break;

          case 18:
            effect_order = str;
            effect_order_idx = 0;
            if (effect_order.length() == 0) {
              effect_order = "0";
            }
            saveEffectOrder();
            if (!specialMode) {                
              int8_t newMode = String(IDX_LINE).indexOf(effect_order[0]);
              if (newMode >= 0 && newMode < MAX_EFFECT) {
                setEffect(newMode);
              }
            }
            break;   

          case 19:
            set_SystemName(str);
            break;
         }
      }

      // При сохранении текста бегущей строки (b_tmp == 0) не нужно сразу автоматически сохранять ее в EEPROM - Сохранение будет выполнено по таймеру. 
      // При получении запроса параметров (b_tmp == 7) ничего сохранять не нужно - просто отправить требуемые параметры
      // При получении очередной строки изображения (b_tmp == 11 или b_tmp == 12) ничего сохранять не нужно
      // При получении текста без сохранения (b_tmp == 14) ничего сохранять не нужно
      // При загрузка / сохранение картинки (b_tmp == 15 или b_tmp == 16) ничего сохранять не нужно
      // При удалении строки (b_tmp == 17) ничего сохранять не нужно
      // При упорядочивании списка эффектов (b_tmp == 18) ничего сохранять не нужно
      // Остальные полученные строки - сохранять сразу, ибо это настройки сети, будильники и другая критически важная информация
      switch (b_tmp) {
        case 0:
        case 7:
        case 11:
        case 12:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
          // Ничего делать не нужно
          break;
        default:  
          saveSettings();
          break;
      }
      
      if (b_tmp > 18) {
        notifyUnknownCommand(incomeBuffer);
      }        
      break;

    // ----------------------------------------------------
    // 8 - эффект
    //  - $8 0 N; включить эффект N
    //  - $8 1 N D; D -> параметр #1 для эффекта N;
    //  - $8 3 N D; D -> параметр #2 для эффекта N;
    //  - $8 4 N X; вкл/выкл оверлей текста поверх эффекта; N - номер эффекта, X=0 - выкл X=1 - вкл 
    //  - $8 5 N X; вкл/выкл оверлей часов поверх эффекта; N - номер эффекта, X=0 - выкл X=1 - вкл 
    //  - $8 6 N D; D -> контрастность эффекта N;
    //  - $8 7 N;   Запрос текущих параметров эффекта N, возвращается JSON с параметрами. Используется для редактирования эффекта в Web-интерфейсе;
    //  - $8 8 N D; D -> скорость эффекта N;
    // ----------------------------------------------------

    case 8:      
      tmp_eff = intData[2];
      // intData[1] : действие -> 0 - выбор эффекта;  1 - параметры #1 и #2; 2 - вкл/выкл "использовать в демо-режиме"
      // intData[2] : номер эффекта
      // intData[3] : действие = 1: значение параметра #1 или #2
      //              действие = 2: 0 - выкл; 1 - вкл;
      if (intData[1] == 0) {    
        // Включить эффект      
        // Если в приложении выбраны эффект "Ночные часы" / "Дневные часы", но они недоступны из-за размеров матрицы - выключить матрицу
        // Если в приложении выбраны часы, но они недоступны из-за размеров матрицы - брать другой случайный эффект
        if (tmp_eff == MC_CLOCK){
          if (!(allowHorizontal || allowVertical)) {
            setRandomMode();
          } else {
            setSpecialMode(4);    // Дневные часы. Для ночных - 3
          }
        } else {
          loadingFlag = true;
          setEffect(tmp_eff);
          putCurrentManualMode(tmp_eff);
          if (tmp_eff == MC_FILL_COLOR && globalColor == 0x000000) set_globalColor(0xffffff);
        }
        setManualModeTo(true);
      } else 
      
      if (intData[1] == 1) {          
        // Параметр #1 эффекта 
        if (tmp_eff == MC_CLOCK){
           // Параметр "Вариант" меняет цвет часов. 
           if (isNightClock) {
             // Для ночных часов - полученное значение -> в map 0..6 - код цвета ночных часов
             set_nightClockColor(map(intData[3], 0,255, 0,6));
             set_specialBrightness(nightClockBrightness);
           } else {
             // Для дневных часов - меняется цвет часов (параметр HUE цвета, hue < 2 - белый)
             set_EffectScaleParam(tmp_eff, intData[3]);
           }
        } else {
          set_EffectScaleParam(tmp_eff, intData[3]);
          if (thisMode == tmp_eff && tmp_eff == MC_FILL_COLOR) {  
            set_globalColor(getColorInt(CHSV(effectSpeed[MC_FILL_COLOR], effectScaleParam[MC_FILL_COLOR], 255)));
          } else 
          if (thisMode == tmp_eff && tmp_eff == MC_WORMS) {
            // При получении параметра эффекта "Червячки" (кол-во червячков) - надо переинициализировать эффект
            loadingFlag = true;
          } else 
          if (thisMode == tmp_eff && tmp_eff == MC_RUBIK) {
            // При получении параметра эффекта "Кубик Рубика" (размер плашки) - надо переинициализировать эффект
            loadingFlag = true;
          }
        }
      } else 
              
      if (intData[1] == 3) {
        // Параметр #2 эффекта
        set_EffectScaleParam2(tmp_eff, intData[3]);
        if (thisMode == tmp_eff && tmp_eff == MC_RAINBOW) {
          // При получении параметра 2 эффекта "Радуга" (тип радуги) - надо переинициализировать эффект
          // Если установлен тип радуги - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_ARROWS) {
          // При получении параметра 2 эффекта "Стрелки" -  вид - надо переинициализировать эффект
          // Если установлен тип - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_PAINTBALL) {
          // При получении параметра 2 эффекта "Пэйнтбол" (сегменты) - надо переинициализировать эффект
          loadingFlag = true;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_SWIRL) {
          // При получении параметра 2 эффекта "Водоворот" (сегменты) - надо переинициализировать эффект
          loadingFlag = true;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_CYCLON) {
          // При получении параметра 2 эффекта "Циклон" (сегменты) - надо переинициализировать эффект
          loadingFlag = true;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_PATTERNS) {
          // При получении параметра 2 эффекта "Узоры" -  вид - надо переинициализировать эффект
          // Если установлен узор - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_RUBIK) {
          // При получении параметра 2 эффекта "Кубик рубика" -  вид - надо переинициализировать эффект
          // Если установлен вариант - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_STARS) {
          // При получении параметра 2 эффекта "Звездочки" -  вид - надо переинициализировать эффект
          // Если установлен вариант - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_TRAFFIC) {
          // При получении параметра 2 эффекта "Трафик" -  вид - надо переинициализировать эффект
          // Если установлен вариант - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_IMAGE) {
          // При получении параметра 2 эффекта "Анимация" -  вид - надо переинициализировать эффект
          // Если установлена анимация - "случайная" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } else
        if (thisMode == tmp_eff && tmp_eff == MC_FIREWORKS) {
          // При получении параметра 2 эффекта "Фейерврк" -  вид - надо переинициализировать эффект
          // Если установлена анимация - "случайная" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;
        } 
        #if (USE_SD == 1)
        else
        if (thisMode == tmp_eff && tmp_eff == MC_SDCARD) {
          // При получении параметра 2 эффекта "SD-карта" -  вид - надо переинициализировать эффект
          // Если установлен эффект - "случайный" - продолжаем показывать тот что был
          loadingFlag = effectScaleParam2[tmp_eff] != 0;  // effectScaleParam2[tmp_eff]: 0 - случайно; 1 - последовательно; 2 и далее - привести к индексу массива списка файлов 
        }          
        #endif
      } else

      if (intData[1] == 4) {
        // Вкл/выкл оверлей бегущей строки поверх эффекта
        set_EffectTextOverlayUsage(tmp_eff, intData[3] == 1);           
      } else 

      if (intData[1] == 5) {
        // Вкл/выкл оверлей часов поверх эффекта
        set_EffectClockOverlayUsage(tmp_eff, intData[3] == 1);           
      } else

      if (intData[1] == 6) {
        // Контрастность эффекта
        set_EffectContrast(tmp_eff, intData[3]);          
      } else

      // Запрос параметров эффекта в виде объекта JSON (Web-интерфейс)
      if (intData[1] == 7) {
        String param2 = getStateValue("SQ", tmp_eff, true);
        
        doc.clear();
        doc["act"]        = F("EDIT");
        doc["id"]         = String(tmp_eff);
        doc["name"]       = getEffectName(tmp_eff);          
        doc["allowClock"] = getStateValue("UC", tmp_eff, true);
        doc["allowText"]  = getStateValue("UT", tmp_eff, true);
        doc["speed"]      = getStateValue("SE", tmp_eff, true);
        doc["contrast"]   = getStateValue("BE", tmp_eff, true);
        // -----------------          
        doc["paramName1"] = getParamNameForMode(tmp_eff);
        doc["paramName2"] = getParam2NameForMode(tmp_eff);
        doc["param1"]     = getStateValue("SS", tmp_eff, true);
        doc["param2"]     = param2;       

        // Для некоторых эффектов параметр2 может быть слишком длинным и весь собранный json объект не влезет в буфер
        // Если влезает - посылаем параметр в данном пакете json
        // Если не влезает - отправляем ниже вторым и следующим запросами по частям

        // Если объект не удалось сериализовать - строка значения param2 будет null
        // Тогда нужно удалить doc["param2"] - затем ниже отправить его отдельным пакетом
        String out;
        serializeJson(doc, out);

        // Сначала отправляем параметр для редактирования param2...
        if (out.indexOf("\"param2\":null") != -1) {
          SendWebKey("EDIT", param2);
        }
                
        // затем отправляем структуру с остальными параметрами редактирования
        // к тому моменту когда клиент получит структуру - недостающий параметр уже будет готов
        // для формирования значений в окне данных
        SendWeb(out, TOPIC_EDT);
        
      } else

      // Скорость для эффекта 
      if (intData[1] == 8) {
        if (intData[3] == 255) intData[3] = 254;          
        set_EffectSpeed(tmp_eff, 255 - intData[3]);
        if (tmp_eff == thisMode) { 
          if (thisMode == MC_FILL_COLOR) { 
            set_globalColor(getColorInt(CHSV(effectSpeed[MC_FILL_COLOR], effectScaleParam[MC_FILL_COLOR], 255)));
          }
          setTimersForMode(thisMode);
        }
      }

      // Для "1","2","4","5","6","8" - отправляются параметры, подтверждение отправлять не нужно. Для остальных - нужно
      if (intData[1] > 8) {
        notifyUnknownCommand(incomeBuffer);
      }
      break;

    // ----------------------------------------------------
    // 12 - Настройки погоды
    // - $12 2 X;      - использовать Цельсий/Фаренгейт: X=0 - Цельсий; X=1 - Фаренгейт
    // - $12 3 X;      - использовать цвет для отображения температуры X=0 - выкл X=1 - вкл в дневных часах
    // - $12 4 X;      - использовать получение погоды с погодного сервера
    // - $12 5 I С C2; - интервал получения погоды с сервера в минутах (I) и код региона C - Yandex и код региона C2 - OpenWeatherMap
    // - $12 6 X;      - использовать цвет для отображения температуры X=0 - выкл X=1 - вкл в ночных часах
    // - $12 7 X;      - отображение температуры в часах: X - битовая карта, b0 - отображать  C/F, b1 = отображать значок градуса
    // ----------------------------------------------------

    #if (USE_WEATHER == 1)                  
    case 12:
       switch (intData[1]) {
         case 2:               // $12 2 X; - использовать Цельсий/Фаренгейт: X=0 - Цельсий; X=1 - Фаренгейт
           set_IsFarenheit(intData[2] != 0);
           break;
         case 3:               // $12 3 X; - Использовать отображение температуры цветом 0 - нет; 1 - да
           set_useTemperatureColor(intData[2] == 1);
           break;
         case 4:               // $12 4 X; - Использовать получение погоды с сервера 0 - нет; 1 - Yandex 2 - OpenWeatherMap
           set_useWeather(intData[2]);
           if (wifi_connected) {
             refresh_weather = true; weather_t = 0; weather_cnt = 0; init_weather = false; 
           }
           if (thisMode == MC_WEATHER && !useWeather) {
             loadingFlag = true;
           }
           break;
         case 5:               // $12 5 I C C2; - Интервал обновления погоды с сервера в минутах, Код региона Yandex, Код региона OpenWeatherMap 
           // Из WebUI прихдит СНАЧАЛА регион погоды '$12 5', затем тут же - установка вкл/выкл - '$12 4'
           // Чтобы два раза не вызывать обновление погоды - здесь ничего не делаем, а в '$12 4' выше сбрасываем таймеры и флаги необходимости обновления погоды
           set_SYNC_WEATHER_PERIOD(intData[2]);
           set_regionID(intData[3]);
           set_regionID2(intData[4]);
           SendWebInfo(MSG_OP_SUCCESS);
           break;
         case 6:               // $12 6 X; - Использовать отображение температуры цветом 0 - нет; 1 - да
           set_useTemperatureColorNight(intData[2] == 1);
           break;
         case 7:               // $12 7 X; - Отображение температуры в малых часах: X - битовая карта, b0 - отображать  C/F, b1 = отображать значок градуса
           set_ShowTempProps(intData[2]);
           break;
        default:
          err = true;
          notifyUnknownCommand(incomeBuffer);
          break;
      }
      break;
    #endif

    // ----------------------------------------------------
    // 13 - Настройки бегущей строки
    // - $13 1 N;   - активация прокручивания строки с номером N 0..35
    // - $13 2 N;   - запросить текст бегущей строки с индексом N 0..35 как есть, без обработки макросов - ответ TY
    // - $13 3 X;   - отображение температуры в макросе {WT}: X - битовая карта, b0 - отображать  C/F, b1 = отображать значок градуса
    // - $13 9 D;   - сохранить настройку D - интервал в секундах отображения бегущей строки
    // - $13 11 X;  - Режим цвета бегущей строки X: 0,1,2,           
    // - $13 13 X;  - скорость прокрутки бегущей строки
    // - $13 15 00FFAA; - цвет текстовой строки для режима "монохромный", сохраняемый в globalTextColor
    // - $13 16;    - Команда "Импорт текстовых строк"
    // - $13 17;    - Команда "Экспорт текстовых строк"
    // - $13 18 X;  - сохранить настройку X "Бегущая строка в эффектах" (общий, для всех эффектов), где X: 0 - отключено; 1 - включено
    // ----------------------------------------------------

    case 13:
       switch (intData[1]) {
         case 1:               // $13 1 N; установить строку N = 0..35 как активную отображаемую строку          
           // Если строка 0 - управляющая - ее нельзя включать принудительно.
           b_tmp = intData[2] > 0 || (intData[2] == 0 && textIndecies.length() > 0 && textIndecies[0] != '#');
           if (b_tmp) {              
             if (thisMode == MC_TEXT){
               setRandomMode();
             }
             nextTextLineIdx = intData[2];  // nextTextLineIdx - индекс следующей принудительно отображаемой строки
             ignoreTextOverlaySettingforEffect = true;
             fullTextFlag = true;
             textLastTime = 0;
           }
           break;
         case 2:               // $13 2 I; - Запросить исходный текст бегущей строки с индексом I 0..35 без обработки макросов
           if (intData[2] >= 0 && intData[2] <= 35) {
             editIdx = intData[2];
             addKeyToChanged("TY");                  // Отправить также строку в канал WEB
           }
           break;
         #if (USE_WEATHER == 1)                  
         case 3:               // $13 3 X;   - отображение температуры в макросе {WT}: X - битовая карта, b0 - отображать  C/F, b1 = отображать значок градуса
           set_ShowTempTextProps(intData[2]);
           break;
         #endif  
         case 9:               // $13 9 D; - Периодичность отображения бегущей строки (в секундах D)
           set_TEXT_INTERVAL(intData[2]);
           SendWebInfo(MSG_OP_SUCCESS);
           break;
         case 11:               // $13 11 X; - Режим цвета бегущей строкой X: 0,1,2,           
           set_COLOR_TEXT_MODE(intData[2]);
           break;
         case 13:               // $13 13 X; - скорость прокрутки бегущей строки
           set_textScrollSpeed(255 - intData[2]);             
           setTimersForMode(thisMode);
           break;
         case 15:               // $13 15 00FFAA;
           // В строке цвет - "$13 15 00FFAA;" - цвет часов текстовой строкой для режима "монохромный", сохраняемый в globalTextColor
           set_globalTextColor((uint32_t)HEXtoInt(incomeBuffer.substring(7,13)));
           SendWebInfo(MSG_OP_SUCCESS);
           break;
         case 16:               // $13 16; - имортировать файл со строками
           importTextLines();
           break;
         case 17:               // $13 17; - экспортировать файл со строками
           exportTextLines();
           break;
         case 18:               // $13 18 X; - сохранить настройку X "Бегущая строка в эффектах"
           set_textOverlayEnabled(intData[2] == 1);
           if (!textOverlayEnabled) {
             showTextNow = false;
             ignoreTextOverlaySettingforEffect = false;
           }
           break;
        default:
          err = true;
          notifyUnknownCommand(incomeBuffer);
          break;
      }
      break;
      
    // ----------------------------------------------------
    //  14 - быстрая установка ручных режимов с пред-настройками
    // - $14 0;  Черный экран (выкл);  
    // - $14 1;  Белый экран (освещение - яркая лампа);  
    // - $14 2;  Цветной экран;  
    // - $14 3;  Часы ночные;
    // - $14 4;  Часы простые;  
    // - $14 5;  Ночник - белая лампа на малой яркости;  
    // - $14 6;  Выключение ночных часов - включить тот эффект что был до включения, восстановить состояние "Автосмена режима";
    // ----------------------------------------------------

    case 14:
      if (intData[1] < 0 || intData[1] >= MAX_SPEC_EFFECT) {
        notifyUnknownCommand(incomeBuffer);
      } else {
        if (intData[1] == 2) {
           // Если в строке цвет - "$14 2 00FFAA;" - цвет лампы, сохраняемый в globalColor
           set_globalColor((uint32_t)HEXtoInt(incomeBuffer.substring(6,12)));
        }        
        setSpecialMode(intData[1]);
      }
      break;
    
    // ----------------------------------------------------
    // 15 - скорость $15 скорость таймер; 0 - таймер эффектов
    // ----------------------------------------------------

    case 15: 
      if (intData[2] == 0) {
        if (intData[1] == 255) intData[1] = 254;
        set_EffectSpeed(thisMode,255 - intData[1]); 
        if (thisMode == MC_FILL_COLOR) { 
          set_globalColor(getColorInt(CHSV(effectSpeed[MC_FILL_COLOR], effectScaleParam[MC_FILL_COLOR], 255)));
        }
        setTimersForMode(thisMode);           
      } else {
        notifyUnknownCommand(incomeBuffer);
      }
      break;

    // ----------------------------------------------------
    // 16 - Режим смены эффектов: $16 N; 
    //    - $16 0; - ручной режим;  
    //    - $16 1; - авторежим; 
    //    - $16 2; - PrevMode; 
    //    - $16 3; - NextMode; 
    //    - $16 5; - вкл/выкл случайный выбор следующего режима
    // ----------------------------------------------------

    case 16:
      if      (intData[1] == 0) setManualModeTo(true);
      else if (intData[1] == 1) { resetModesExt(); setManualModeTo(false); }
      else if (intData[1] == 2) prevMode();
      else if (intData[1] == 3) nextMode();
      else if (intData[1] == 5) set_useRandomSequence(intData[2] == 1);

      putCurrentManualMode(manualMode ? (int8_t)thisMode : -1);
      
      if (manualMode) {
        set_CurrentSpecialMode(-1);
      }

      if (intData[1] == 1) {
        set_idleTime(getIdleTime());    
        setIdleTimer();             
        if (thisMode == MC_FILL_COLOR && globalColor == 0x000000) {
          // Было выключено, режим "Лампа" с черным цветом - включить случайный режим
          setRandomMode();
        }
      }  
      break;

    // ----------------------------------------------------
    // 17 - Время автосмены эффектов и бездействия: $17 сек
    // ----------------------------------------------------

    case 17: 
      set_autoplayTime((uint32_t)intData[1] * 1000UL);   // секунды -> миллисек 
      set_idleTime((uint32_t)intData[2] * 60 * 1000UL);  // минуты -> миллисек
      if (!manualMode) {
        autoplayTimer = millis();
      }
      setIdleTimer();             
      SendWebInfo(MSG_OP_SUCCESS);
      break;

    // ----------------------------------------------------
    // 18 - отладка часов / календаря / температуры, др
    //   $18 1 HH MM TT DD MM YYYY; - Установить указанное время HH:MM и температуру TT - для отладки позиционирования часов с температурой
    //   $18 2 X; - Вкл/выкл отображение креста
    //   $18 3 X; - Сдвиг позиции вывода часов при скроллинге на X колонок
    //   $18 4;   - Сброс позиции вывода часов при скроллинге в начальную позицию
    //   $18 5 X; - Вкл/выкл отображение рамки областей вывода часов/температуры/календаря - полной и фактической
    // ----------------------------------------------------
    
    case 18: 
       switch (intData[1]) {
         case 1:               // $18 1 HH MM TT DD MM YYYY; - Установить указанное время HH:MM и температуру TT, день DD месяц MM год YYYY  - для отладки позиционирования часов с температурой
           debug_hours = intData[2];
           debug_mins = intData[3];
           debug_temperature = intData[4];
           debug_days = intData[5];
           debug_month = intData[6];
           debug_year = intData[7];
           break;
         case 2:               // $18 2 X; - Вкл/выкл отображение креста
           debug_cross = intData[2] != 0;
           break;
         case 3:               // $18 3 X; - Сдвиг позиции вывода часов при скроллинге на X колонок
           debug_move = intData[2];
           break;
         case 4:               // $18 4;   - Сброс позиции вывода часов при скроллинге в начальную позицию
           CLOCK_XC = pWIDTH / 2;
           break;
         case 5:               // $18 5 X; - Вкл/выкл отображение рамки
           debug_frame = intData[2] != 0;
           break;
         default:
           err = true;
           notifyUnknownCommand(incomeBuffer);
           break;
       }
       break;
       
    // ----------------------------------------------------
    // 19 - работа с настройками часов
    //   $19 1 X;   - сохранить настройку X "Часы в эффектах" (общий, для всех эффектов)
    //   $19 2 X;   - Использовать синхронизацию часов NTP  X: 0 - нет, 1 - да
    //   $19 4 X;   - Выключать индикатор TM1637 при выключении экрана X: 0 - нет, 1 - да
    //   $19 5 X;   - Режим цвета часов оверлея X: 0,1,2,3
    //   $19 6 X;   - Ориентация часов  X: 0 - горизонтально, 1 - вертикально
    //   $19 7 X;   - Размер часов X: 0 - авто, 1 - малые 3х5, 2 - большие 5x7
    //   $19 8 YYYY MM DD HH MM; - Установить текущее время YYYY.MM.DD HH:MM
    //   $19 9 X Y;  - Флаги отображения часов/календаря в циклах T1-T4  X: часы - младшие 4 бита, календарт - старшие 4 бита; Y - календарь - младшие 4 бита
    //   $19 10 X;   - Цвет ночных часов:  0 - R; 1 - G; 2 - B; 3 - C; 3 - M; 5 - Y; 6 - W;
    //   $19 11 X;   - Яркость ночных часов:  0..255
    //   $19 12 X;   - скорость прокрутки часов оверлея или 0, если часы остановлены по центру
    //   $19 13 X;   - Отображение года в календаре 2/4 цифры, в одну или в две строки
    //   $19 14 00FFAA; - цвет часов оверлея, сохраняемый в globalClockColor
    //   $19 15 X;   - флаги выраснивани часов/календаря/температуры в области их отображения
    //   $19 16 X;   - флаги вариантов отображения селандаря и темепратур ы - относительно часов или произвольно по указанномусмещению
    //   $19 17 D I; - Продолжительность отображения даты / часов (в секундах)
    //   $19 18 X;   - флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
    //   $19 19 X;   - Показывать время в 12/24 часовом формате  X: 0 - 24, 1 - 12
    //   $19 20 X;   - Показывать малые часы шрифтом 3х5 - 0 квадратный шрифт, 1 - круглый шрифт
    //   $19 21 X Y; - Смещение часов по X,Y
    //   $19 22 X Y; - Ширина разделительных точек в больших часах (X = 1|2), Y - есть ли пробелы между цифрами часов и точками 0|1
    //   $19 23 X Y; - Смещение температуры по X,Y
    //   $19 24 X Y; - Смещение календаря по X,Y
    //   $19 25 Y;   - Смещение текста по Y
    // ----------------------------------------------------

    case 19: 
       switch (intData[1]) {
         case 1:               // $19 1 X; - сохранить настройку X "Часы в эффектах"
           set_clockOverlayEnabled(((CLOCK_ORIENT == 0 && allowHorizontal) || (CLOCK_ORIENT == 1 && allowVertical)) ? intData[2] == 1 : false);
           if (specialMode) specialClock = clockOverlayEnabled;
           break;
         case 2:               // $19 2 X; - Использовать синхронизацию часов NTP  X: 0 - нет, 1 - да
           set_useNtp(intData[2] == 1);   // this will enable NTP sync also
           break;
         case 4:               // $19 4 X; - Выключать индикатор TM1637 при выключении экрана X: 0 - нет, 1 - да
           set_needTurnOffClock(intData[2] == 1);
           break;
         case 5:               // $19 5 X; - Режим цвета часов оверлея X: 0,1,2,3
           set_COLOR_MODE(intData[2]);
           break;
         case 6:               // $19 6 X; - Ориентация часов  X: 0 - горизонтально, 1 - вертикально
           set_CLOCK_ORIENT(intData[2] == 1 ? 1  : 0);
           if (allowHorizontal || allowVertical) {
             if (CLOCK_ORIENT == 0 && !allowHorizontal) set_CLOCK_ORIENT(1);
             if (CLOCK_ORIENT == 1 && !allowVertical) set_CLOCK_ORIENT(0);              
           } else {
             set_clockOverlayEnabled(false);
           }             
           // Центрируем часы по горизонтали/вертикали по ширине / высоте матрицы
           loadPlacementOptions();           
           checkClockOrigin();
           break;
         case 7:               // $19 7 X; - Размер часов  X: 0 - авто, 1 - малые 3х5, 2 - большие 5х7
           set_CLOCK_SIZE(intData[2]);
           loadPlacementOptions();           
           checkClockOrigin();
           // Шрифт отображения температуры в эффекте Погода зависит от установленного размера часов 
           if (thisMode == MC_WEATHER) {
             loadingFlag = true;
           }
           break;
         case 8:               // $19 8 YYYY MM DD HH MM; - Установить текущее время YYYY.MM.DD HH:MM
           // Установить полученное с веб-морды время (кнока "установить время вручную"),
           // Выполнить перерасчет времени будильников и ожидателей времени наступления события в макросах бегущей строки
           // Если идет вещание на клиентов - передать текущее (полученное) время клиентам
           setCurrentTime((uint8_t)intData[5],(uint8_t)intData[6],0,(uint8_t)intData[4],(uint8_t)intData[3],(uint16_t)intData[2]);
           SendWebInfo(MSG_OP_SUCCESS);
           break;
         case 9:               
           // $19 9 X Y;  - Флаги отображения часов/календаря в циклах T1-T4  часы - младшие 4 бита, календарь - старшие 4 бита
           set_clockCycleF1(intData[2]);
           set_clockCycleF2(intData[3]);
           break;
         case 10:               // $19 10 X; - Цвет ночных часов:  0 - R; 1 - G; 2 - B; 3 - C; 3 - M; 5 - Y; 6 - W;
           set_nightClockColor(intData[2]);
           if (isNightClock) {
              set_specialBrightness(nightClockBrightness);
           }             
           break;
         case 11:               // $19 11 X; - Яркость ночных часов:  1..255;
           set_nightClockBrightness(intData[2]); // setter             
           if (isNightClock) {
              set_specialBrightness(nightClockBrightness);
           }             
           break;
         case 12:               // $19 12 X; - скорость прокрутки часов оверлея или 0, если часы остановлены по центру
           set_clockScrollSpeed(255 - intData[2]);
           setTimersForMode(thisMode);
           break;
         case 13:               // $19 13 X; - Отображение года в горизонтальном календаре 0/2/4 цифры, 1/2 строки
           set_CalendarYearWidth(intData[2]);          
           checkClockOrigin(); 
           break;
         case 14:               // $19 14 00FFAA;
           // В строке цвет - "$19 14 00FFAA;" - цвет часов оверлея, сохраняемый в globalClockColor
           set_globalClockColor((uint32_t)HEXtoInt(incomeBuffer.substring(7,13)));
           break;
         case 15:
           // $19 15 X;  - флаги выраснивани часов/календаря/температуры в области их отображения
           set_clockShowAlignment(intData[2]);
           break;
         case 16:
           // $19 16 X;  - флаги вариантов отображения селандаря и темепратуры - относительно часов или произвольно по указанномусмещению
           set_clockShowVariant(intData[2]);
           checkClockOrigin();
           break;
         case 17:               // $19 17 T1 T2; - Продолжительность отображения даты / часов (в секундах)
           set_clockCycleT1(intData[2]);
           set_clockCycleT2(intData[3]);
           SendWebInfo(MSG_OP_SUCCESS);
           break;
         case 18:               //   $19 18 X; - Флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
           set_hideOnTextRunning(intData[2]);
           break;
         case 19:               //   $19 19 X; - Показывать время в 12/24 часовом формате  X: 0 - 24, 1 - 12
           set_Time12(intData[2] == 1);
           break;
         case 20:               //   $19 20 X; - Показывать малые часы шрифтом 3х5 - 0 квадратный шрифт, 1 - круглый шрифт
           set_SmallFontType(intData[2]);
           break;
         case 21:               //   $19 21 X Y; - Смещение часов по X,Y
           set_ClockOffsetX(intData[2]);
           set_ClockOffsetY(intData[3]);
           break;
         case 22:               //   $19 22 X Y; - Ширина разделительных точек в больших часах (X = 1|2), Y - есть ли пробелы между цифрами часов и точками 0|1
           set_clockDotWidth(intData[2]);
           set_clockDotSpace(intData[3] != 0);
           checkClockOrigin();
           break;
         case 23:               //   $19 23 X Y; - Смещение температуры по X,Y
           set_TempOffsetX(intData[2]);
           set_TempOffsetY(intData[3]);
           break;
         case 24:               //   $19 24 X Y; - Смещение календаря по X,Y
           set_CalendarOffsetX(intData[2]);
           set_CalendarOffsetY(intData[3]);
           break;
         case 25:               //   $19 25 Y;    - Смещение бегущей строки по Y
           set_TextOffsetY(intData[2]);
           break;
         default:
           err = true;
           notifyUnknownCommand(incomeBuffer);
           break;
      }
      break;

    // ----------------------------------------------------
    //  20 - настройки и управление будильников
    // - $20 0;       - отключение будильника (сброс состояния isAlarming)
    // - $20 2 X VV MA MB;
    //      X    - исп звук будильника X=0 - нет, X=1 - да 
    //     VV    - максимальная громкость
    //     MA    - номер файла звука будильника
    //     MB    - номер файла звука рассвета
    // - $20 3 X NN VV; - пример звука будильника
    //      X - 1 играть 0 - остановить
    //     NN - номер файла звука будильника из папки SD:/01
    //     VV - уровень громкости
    // - $20 4 X NN VV; - пример звука рассвета
    //      X - 1 играть 0 - остановить
    //     NN - номер файла звука рассвета из папки SD:/02
    //     VV - уровень громкости
    // - $20 5 VV; - установит уровень громкости проигрывания примеров (когда уже играет)
    //     VV - уровень громкости
    // ----------------------------------------------------
    
    case 20:
      switch (intData[1]) { 
        case 0:  
          // $20 0;       - отключение будильника (сброс состояния isAlarming)
          if (isAlarming || isPlayAlarmSound) stopAlarm();            
          break;
        case 2:
          #if (USE_MP3 == 1)          
          if (isDfPlayerOk) {
            // $20 2 X VV MA MB;
            //    X    - исп звук будильника X=0 - нет, X=1 - да 
            //   VV    - максимальная громкость
            //   MA    - номер файла звука будильника
            //   MB    - номер файла звука рассвета
            dfPlayer.stop(); Delay(GUARD_DELAY);
            set_soundFolder(0);
            set_soundFile(0);
            set_isAlarming(false);
            set_isAlarmStopped(false);
            set_useAlarmSound(intData[2] == 1);              
            set_maxAlarmVolume(constrain(intData[3],0,30));
            set_alarmSound(intData[4] - 2);  // Индекс от приложения: 0 - нет; 1 - случайно; 2 - 1-й файл; 3 - ... -> -1 - нет; 0 - случайно; 1 - 1-й файл и т.д
            set_dawnSound(intData[5] - 2);   // Индекс от приложения: 0 - нет; 1 - случайно; 2 - 1-й файл; 3 - ... -> -1 - нет; 0 - случайно; 1 - 1-й файл и т.д
          }
          #endif
          break;
        case 3:
          #if (USE_MP3 == 1)
          if (isDfPlayerOk) {
            // $20 3 X NN VV; - пример звука будильника
            //  X  - 1 играть 0 - остановить
            //  NN - номер файла звука будильника из папки SD:/01
            //  VV - уровень громкости
            if (intData[2] == 0) {
              dfPlayer.stop(); Delay(GUARD_DELAY);
              set_soundFolder(0);
              set_soundFile(0);
            } else {
              b_tmp = intData[3] - 2;  // Знач: -1 - нет; 0 - случайно; 1 и далее - файлы; -> В списке индексы: 1 - нет; 2 - случайно; 3 и далее - файлы
              if (b_tmp > 0 && b_tmp <= alarmSoundsCount) {
                dfPlayer.stop(); Delay(GUARD_DELAY);
                set_soundFolder(1);
                set_soundFile(b_tmp);
                dfPlayer.setVolume(constrain(intData[4],0,30));   Delay(GUARD_DELAY);
                dfPlayer.playFolderTrack(soundFolder, soundFile); Delay(GUARD_DELAY);
                dfPlayer.setRepeatPlayCurrentTrack(true);         Delay(GUARD_DELAY);
              } else {
                set_soundFolder(0);
                set_soundFile(0);
              }
            }
          }  
          #endif
          break;
        case 4:
          #if (USE_MP3 == 1)
          if (isDfPlayerOk) {
            // $20 4 X NN VV; - пример звука рассвета
            //    X  - 1 играть 0 - остановить
            //    NN - номер файла звука рассвета из папки SD:/02
            //    VV - уровень громкости
            if (intData[2] == 0) {
              dfPlayer.stop(); Delay(GUARD_DELAY);
              set_soundFolder(0);
              set_soundFile(0);
            } else {
              dfPlayer.stop(); Delay(GUARD_DELAY);
              b_tmp = intData[3] - 2; // Знач: -1 - нет; 0 - случайно; 1 и далее - файлы; -> В списке индексы: 1 - нет; 2 - случайно; 3 и далее - файлы
              if (b_tmp > 0 && b_tmp <= dawnSoundsCount) {
                set_soundFolder(2);
                set_soundFile(b_tmp);
                dfPlayer.setVolume(constrain(intData[4],0,30));   Delay(GUARD_DELAY);
                dfPlayer.playFolderTrack(soundFolder, soundFile); Delay(GUARD_DELAY);
                dfPlayer.setRepeatPlayCurrentTrack(true);         Delay(GUARD_DELAY);
              } else {
                set_soundFolder(0);
                set_soundFile(0);
              }
            }
          }
          #endif
          break;
        case 5:
          #if (USE_MP3 == 1)
          if (isDfPlayerOk && soundFolder > 0) {
           // $20 5 VV; - установить уровень громкости проигрывания примеров (когда уже играет)
           //    VV - уровень громкости
           set_maxAlarmVolume(constrain(intData[2],0,30));
           dfPlayer.setVolume(maxAlarmVolume); Delay(GUARD_DELAY);
          }
          #endif
          break;
        default:
          err = true;
          notifyUnknownCommand(incomeBuffer);
          break;
      }
      break;

    // ----------------------------------------------------
    // 21 - настройки подключения к сети / точке доступа
    //   $21 0 0; - не использовать точку доступа $21 0 1; - использовать точку доступа
    //   $21 1 IP1 IP2 IP3 IP4; - установить статический IP адрес подключения к локальной WiFi сети, пример: $21 1 192 168 0 106;
    //   $21 2; Выполнить переподключение к сети WiFi
    //   $21 3 X; - Использовать DHCP X=0|1
    // ----------------------------------------------------

    case 21:
      // Настройки подключения к сети
      switch (intData[1]) { 
        // $21 0 0 - не использовать точку доступа $21 0 1 - использовать точку доступа
        case 0:  
          set_useSoftAP(intData[2] == 1);
          if (useSoftAP && !ap_connected) 
            startSoftAP();
          else if (!useSoftAP && ap_connected) {
            if (wifi_connected) { 
              ap_connected = false;              
              WiFi.softAPdisconnect(true);
              DEBUGLN(F("Точка доступа отключена."));
            }
          }      
          SendWebInfo(MSG_OP_SUCCESS);
          break;
        case 1:  
          // $21 1 IP1 IP2 IP3 IP4 - установить статический IP адрес подключения к локальной WiFi сети, пример: $21 1 192 168 0 106
          // Локальная сеть - 10.х.х.х или 172.16.х.х - 172.31.х.х или 192.168.х.х
          // Если задан адрес не локальной сети - сбросить его в 0.0.0.0, что означает получение динамического адреса 
          if (!(intData[2] == 10 || (intData[2] == 172 && intData[3] >= 16 && intData[3] <= 31) || (intData[2] == 192 && intData[3] == 168))) {
            set_StaticIP(0, 0, 0, 0);
          }
          set_StaticIP(intData[2], intData[3], intData[4], intData[5]);
          break;
        case 2:  
          // $21 2; Выполнить переподключение к сети WiFi
          saveSettings();
          delay(10);
          FastLED.clear();
          startWiFi(30000);     // Время ожидания подключения 30 сек
          showCurrentIP();
          break;
        case 3:
          // $21 3 X; - Использовать DHCP X=0|1
          set_useDHCP(intData[2] != 0);
          break;
        default:
          err = true;
          notifyUnknownCommand(incomeBuffer);
          break;
      }
      break;

    // ----------------------------------------------------
    // 22 - настройки включения режимов матрицы в указанное время NN5 - Действие на "Рассвет", NN6 - действие на "Закат"
    // - $22 HH1 MM1 NN1 HH2 MM2 NN2 HH3 MM3 NN3 HH4 MM4 NN4 NN5 NN6;
    //     HHn - час срабатывания
    //     MMn - минуты срабатывания
    //     NNn - эффект: -3 - выключено; -2 - выключить матрицу; -1 - ночные часы; 0 - случайный режим и далее по кругу; 1 и далее - список режимов EFFECT_LIST 
    // ----------------------------------------------------

    case 22:
      set_AM1_hour(intData[1]);
      set_AM1_minute(intData[2]);
      set_AM1_effect_id(intData[3]);

      set_AM2_hour(intData[4]);
      set_AM2_minute(intData[5]);
      set_AM2_effect_id(intData[6]);

      set_AM3_hour(intData[7]);
      set_AM3_minute(intData[8]);
      set_AM3_effect_id(intData[9]);

      set_AM4_hour(intData[10]);
      set_AM4_minute(intData[11]);
      set_AM4_effect_id(intData[12]);

      set_dawn_effect_id(intData[13]);   // Действие на время "Рассвет"
      set_dusk_effect_id(intData[14]);   // Действие на время "Закат"

      saveSettings();
      
      
      SendWebInfo(MSG_OP_SUCCESS);
      break;

    // ----------------------------------------------------
    // 23 - прочие настройки
    // - $23 0 VAL;  - лимит по потребляемому току
    // - $23 1 ST;   - Сохранить EEPROM в файл    ST = 0 - внутр. файл. систему; 1 - на SD-карту
    // - $23 2 ST;   - Загрузить EEPROM из файла  ST = 0 - внутр. файл. системы; 1 - c SD-карты
    // - $23 3 E1 E2 E3;   - Установка режима работы панели и способа трактовки полученных данных синхронизации
    //       E1 - режим работы 0 - STANDALONE; 1 - MASTER; 2 - SLAVE
    //       E2 - данные в кадре: 0 - физическое расположение цепочки диодов
    //                            1 - логическое расположение - X,Y - 0,0 - левый верхний угол и далее вправо по X, затем вниз по Y
    //                            2 - payload пакета - строка команды
    //       E3 - группа синхронизации 0..9                      
    // - $23 4; - перезагрузить устройство
    // - $23 5 MX, MY, LX, LY, LW, LH; - настройуа окна отображения трансляции с MASTER на SLAVE
    //       MX - логическая координата X мастера с которого начинается вывод на локальную матрицу    X,Y - левый верхний угол начала трансляции мастера
    //       MY - логическая координата Y мастера с которого начинается вывод на локальную матрицу
    //       LX - логическая координата X приемника на которую начинается вывод на локальную матрицу  X,Y - левый верхний угол начала отображения на приемнике
    //       MY - логическая координата Y приемника на которого начинается вывод на локальную матрицу
    //       LW - ширина окна отображения на локальной матрице
    //       LH - высота окна отображения на локальной матрице
    // - $23 6 U1,P1,S1,L1,C1; - подключение матрицы светодиодов линия 1
    // - $23 7 U2,P2,S2,L2,C2; - подключение матрицы светодиодов линия 2
    // - $23 8 U3,P3,S3,L3,C3; - подключение матрицы светодиодов линия 3
    // - $23 9 U4,P4,S4,L4,C4; - подключение матрицы светодиодов линия 4
    //     Ux - 1 - использовать линию, 0 - линия не используется
    //     Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
    //     Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
    //     Lx - длина цепочки светодиодов, подключенной к линии x
    // - $23 10 X; - X - DEVICE_TYPE - 0 - труба, 1 - плоская панель
    // - $23 11 X Y; - кнопка 
    //     X - GPIO пин к которому подключена кнопка
    //     Y - BUTTON_TYPE - 0 - сенсорная кнопка, 1 - тактовая кнопка
    // - $23 12 X Y; - управление питанием матрицы
    //     X - GPIO пин к которому подключено реле управления питанием
    //     Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
    // - $23 13 X Y; - для SD-карты - алгоритм воспроизведения ролика
    //     X - WAIT_PLAY_FINISHED - алгоритм проигрывания эффекта SD-карта
    //     Y - REPEAT_PLAY - алгоритм проигрывания эффекта SD-карта
    // - $23 14 X;  - Вкл/выкл отладочного вывода в монитор порта DEBUG_SERIAL - 1-вкл, 0-выкл 
    // - $23 15 X Y;  - Подключение пинов MP3 DFPlayer
    //     X - STX - GPIO пин контроллера TX -> к RX плеера
    //     Y - SRX - GPIO пин контроллера RX -> к TX плеера
    // - $23 16 X Y;  - Подключение пинов TM1637
    //     X - DIO - GPIO пин контроллера к DIO индикатора
    //     Y - CLK - GPIO пин контроллера к CLK индикатора
    // - $23 17 X Y; - управление питанием по линии будильника
    //     X - GPIO пин к которому подключено реле управления питанием
    //     Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
    // - $23 18 X Y; - управление питанием по дополнительной линии управления
    //     X - GPIO пин к которому подключено реле управления питанием по дополнительной линии
    //     Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
    // - $23 19 VAL;  - ВКЛ/ВЫКЛ дополнительную линию питания; 0 - выключить; 1 - включить
    // - $23 20 VAL;  - битовая маска состояния режимов управления дополнительным каналом питания по времени
    // ----------------------------------------------------

    case 23: {
      bool needSendAck = true;
      switch(intData[1]) {
        case 0:
          set_CURRENT_LIMIT(intData[2]);
          FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT == 0 ? 100000 : CURRENT_LIMIT); 
          break;
        case 1:
          err = !saveEepromToFile(intData[2] == 1 ? F("SD") : F("FS"));
          if (err) {
            SendWebError(MSG_BACKUP_SAVE_ERROR);
          } else {
            SendWebInfo(MSG_BACKUP_SAVE_OK);
          }
          addKeyToChanged("EE");
          needSendAck = false;
          break;
        case 2:
          err = !loadEepromFromFile(intData[2] == 1 ? F("SD") : F("FS"));
          if (err) {
            SendWebError(MSG_BACKUP_LOAD_ERROR);
          } else {
            SendWebInfo(MSG_BACKUP_LOAD_OK);
          }
          needSendAck = false;
          // Если настройки загружены без ошибок - перезагрузить устройство
          if (!err) {
            needRestart = true;
            needRestartTime = millis();
          }
          break;
        case 3:
          #if (USE_E131 == 1)
          // Переинициализировать не нужно, если изменился только syncMode с PHYSIC на LOGIC или наоборот
          {
            bool needReInitialize = workMode != (eWorkModes)intData[2] || syncGroup != intData[4] || !((syncMode == PHYSIC && ((eSyncModes)intData[3] == LOGIC)) || (syncMode == LOGIC && (eSyncModes)intData[3] == PHYSIC));
            set_SyncWorkMode((eWorkModes)intData[2]);
            set_SyncDataMode((eSyncModes)intData[3]);
            set_SyncGroup(intData[4]);
            saveSettings();
            if (needReInitialize) {
              DisposeE131();
              InitializeE131();
            } else {
              printWorkMode();  
            }
          }
          #endif
          break;
        case 4:     
          // Сохранить несохраненные настройки перед перезапуском устройства  
          saveSettings();
          delay(100);     
          // Погасить экран, перезапустить контроллер
          needRestart = true;
          needRestartTime = millis();
          break;
        case 5:
          #if (USE_E131 == 1)
          {
            masterX = intData[2];
            masterY = intData[3];
            localX  = intData[4];
            localY  = intData[5];
            localW  = intData[6];
            localH  = intData[7];

            // Сохранить настройки Viewport
            set_SyncViewport(masterX, masterY, localX, localY, localW, localH);  
            saveSettings();                  
          }
          #endif
          break;
        case 6:
          // - $23 6 U1,P1,S1,L1,C1; - подключение матрицы светодиодов линия 1
          putLedLineUsage(1, intData[2] == 1);  // Ux - 1 - использовать линию, 0 - линия не используется
          putLedLinePin(1, intData[3]);         // Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
          putLedLineStartIndex(1, intData[4]);  // Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
          putLedLineLength(1, intData[5]);      // Lx - длина цепочки светодиодов, подключенной к линии x
          putLedLineRGB(1, intData[6]);         // Cx - порядок цвета RGB в ленте, подключеннной к линии x
          break;
        case 7:
          // - $23 7 U2,P2,S2,L2,C2; - подключение матрицы светодиодов линия 2
          putLedLineUsage(2, intData[2] == 1);  // Ux - 1 - использовать линию, 0 - линия не используется
          putLedLinePin(2, intData[3]);         // Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
          putLedLineStartIndex(2, intData[4]);  // Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
          putLedLineLength(2, intData[5]);      // Lx - длина цепочки светодиодов, подключенной к линии x
          putLedLineRGB(2, intData[6]);         // Cx - порядок цвета RGB в ленте, подключеннной к линии x
          break;
        case 8:
          // - $23 8 U3,P3,S3,L3,C3; - подключение матрицы светодиодов линия 3
          putLedLineUsage(3, intData[2] == 1);  // Ux - 1 - использовать линию, 0 - линия не используется
          putLedLinePin(3, intData[3]);         // Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
          putLedLineStartIndex(3, intData[4]);  // Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
          putLedLineLength(3, intData[5]);      // Lx - длина цепочки светодиодов, подключенной к линии x
          putLedLineRGB(3, intData[6]);         // Cx - порядок цвета RGB в ленте, подключеннной к линии x
          break;
        case 9:
          // - $23 9 U4,P4,S4,L4,C4; - подключение матрицы светодиодов линия 4
          putLedLineUsage(4, intData[2] == 1);  // Ux - 1 - использовать линию, 0 - линия не используется
          putLedLinePin(4, intData[3]);         // Px - пин GPIO на который назначен вывод сигнала на матрицу для линии х
          putLedLineStartIndex(4, intData[4]);  // Sx - начальный индекс в цепочке светодиодов (в массиве leds) с которого начинается вывод на матрицу с линии x
          putLedLineLength(4, intData[5]);      // Lx - длина цепочки светодиодов, подключенной к линии x
          putLedLineRGB(4, intData[6]);         // Cx - порядок цвета RGB в ленте, подключеннной к линии x
          break;
        case 10:
          // - $23 10 X; - X - DEVICE_TYPE - 0 - труба, 1 - плоская панель
          putDeviceType(intData[2]);
          break;
        case 11:
          // - $23 11 X Y; - кнопка 
          //     X - GPIO пин к которому подключена кнопка
          //     Y - BUTTON_TYPE - 0 - сенсорная кнопка, 1 - тактовая кнопка
          putButtonPin(intData[2]);
          putButtonType(intData[3]);
          break;
        case 12:
          // - $23 12 X Y; - управление питанием
          //     X - GPIO пин к которому подключено реле управления питанием
          //     Y - 0 - активный уровень управления питанием - LOW, 1 - активный уровень управления питанием HIGH
          putPowerPin(intData[2]);
          putPowerActiveLevel(intData[3]);
          break;
        case 13:
          // - $23 13 X Y; - для SD-карты - алгоритм воспроизведения ролика
          //     X - WAIT_PLAY_FINISHED - алгоритм проигрывания эффекта SD-карта
          //     Y - REPEAT_PLAY - алгоритм проигрывания эффекта SD-карта
          putWaitPlayFinished(intData[2] == 1);
          putRepeatPlay(intData[3] == 1);
          break;
        case 14:
          // - $23 14 X;  - Вкл/выкл отладочного вывода в монитор порта DEBUG_SERIAL - 1-вкл, 0-выкл 
          putDebugSerialEnable(intData[2] == 1);
          break;
        case 15:
          // - $23 15 X Y;  - Подключение пинов MP3 DFPlayer
          //     X - STX - GPIO пин контроллера TX -> к RX плеера
          //     Y - SRX - GPIO пин контроллера RX -> к TX плеера
          putDFPlayerSTXPin(intData[2]);
          putDFPlayerSRXPin(intData[3]);
          break;
        case 16:
          // - $23 16 X Y;  - Подключение пинов TM1637
          //     X - DIO - GPIO пин контроллера к DIO индикатора
          //     Y - CLK - GPIO пин контроллера к CLK индикатора
          putTM1637DIOPin(intData[2]);
          putTM1637CLKPin(intData[3]);
          break;
        case 17:
          // - $23 17 X Y; - управление дополнительной линией будильника
          //     X - GPIO пин к которому подключено реле управления линии будильника
          //     Y - 0 - активный уровень управления - LOW, 1 - активный уровень управления HIGH
          putAlarmPin(intData[2]);
          putAlarmActiveLevel(intData[3]);
          break;
        case 18:
          // - $23 18 X Y; - управление дополнительной линией
          //     X - GPIO пин к которому подключено реле управления линии
          //     Y - 0 - активный уровень управления - LOW, 1 - активный уровень управления HIGH
          putAuxPin(intData[2]);
          putAuxActiveLevel(intData[3]);
          break;
        case 19:
          // - $23 19 VAL;  - ВКЛ/ВЫКЛ дополнительную линию питания; 0 - выключить; 1 - включить
          set_isAuxActive(intData[2] != 0);
          saveSettings();
          break;
        case 20:
          // - $23 20 VAL;  - битовая маска состояния режимов управления дополнительным каналом питания по времени
          set_AuxLineModes(intData[2]);
          break;
        default:
          notifyUnknownCommand(incomeBuffer);
          break;
      }

      if (needSendAck) {
         
         SendWebInfo(MSG_OP_SUCCESS);
      }
      break;
    }

    // ----------------------------------------------------
    default:
      notifyUnknownCommand(incomeBuffer);
      break;
  }
}

String getStateValue(const String& key, int8_t effect, bool shrt) {

  yield();
  
  // W:число     ширина матрицы
  // H:число     высота матрицы
  // AB:текст    пароль точки доступа - Web-канал
  // AD:число    продолжительность рассвета, мин
  // AE:число    эффект, использующийся для будильника
  // AL:X        сработал будильник 0-нет, 1-да
  // AM1T:HH MM  час 00..23 и минуты 00..59 включения режима 1, разделенные пробелом:      "AM1T:23 15"
  // AM1A:NN     номер эффекта режима 1:   -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  // AM2T:HH MM  час 00..23 и минуты 00..59 включения режима 1, разделенные пробелом:      "AM2T:23 15"
  // AM2A:NN     номер эффекта режима 2:   -3 - не используется; -2 - выключить матрицу; -1 - ночные часы;  0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  // AM3T:HH MM  час 00..23 и минуты 00..59 включения режима 1, разделенные пробелом:      "AM3T:23 15"
  // AM3A:NN     номер эффекта режима 3:   -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  // AM4T:HH MM  час 00..23 и минуты 00..59 включения режима 1, разделенные пробелом:      "AM4T:23 15"
  // AM4A:NN     номер эффекта режима 4:   -3 - не используется; -2 - выключить матрицу; -1 - ночные часы;  0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  // AM5A:NN     номер эффекта режима по времени "Рассвет":   -3 - не используется; -2 - выключить матрицу; -1 - ночные часы;  0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  // AM6A:NN     номер эффекта режима по времени "Закат":     -3 - не используется; -2 - выключить матрицу; -1 - ночные часы;  0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
  // AN:текст    имя точки доступа
  // AT: DW HH MM  часы-минуты времени будильника для дня недели DW 1..7 -> например "AT:1 09 15"
  // AU:X        создавать точку доступа 0-нет, 1-да
  // AW:число    битовая маска дней недели будильника b6..b0: b0 - пн .. b7 - вс
  // BE:число    контрастность эффекта
  // BF:текст    имя файла с резервной копией настроек
  // BR:число    яркость
  // BS:число    задержка повтора нажатия кнопки в играх
  // C1:цвет     цвет режима "монохром" часов оверлея; цвет: 192,96,96 - R,G,B
  // C2:цвет     цвет режима "монохром" бегущей строки; цвет: 192,96,96 - R,G,B
  // C12:X       12/24 часовой формат времени 0 - 24 часа, 1 -12 часов
  // C35:X       тип шрифта 3х5 - 0 - квадратный, 1 - скругленный
  // CС:X        режим цвета часов оверлея: 0,1,2
  // CD:X        ширина точки в больших часах
  // CE:X        оверлей часов вкл/выкл, где Х = 0 - выкл; 1 - вкл (использовать часы в эффектах)
  // CF:X        отображение года в календаре - 2/4 цифры
  // CH:X        доступны горизонтальные часы (по размерам), где Х = 0 - не доступны; 1 - доступны
  // CK:X        размер горизонтальных часов, где Х = 0 - авто; 1 - малые 3x5; 2 - большие 5x7 
  // CL:X        цвет рисования в формате RRGGBB
  // CO:X        ориентация часов: 0 - горизонтально, 1 - вертикально
  // CS:X        ширина пробела вокруг точки в больших часах
  // CSV:X       флаги вариантов размещения календаря и темепратуры - относительно часов или произвольно по указанному смещению
  // CSA:X       флаги выравнивани часов/календаря/температуры в области их отображения;                
  // CX:X        смещение часов по оси X
  // CY:X        смещение часов по оси Y
  // CRS1        контрольная сумма списка звуков будильника S1
  // CRS2        контрольная сумма списка звуков рассвета S2
  // CRS3        контрольная сумма списка звуков нотификации бегущей строки S3
  // CRLE        контрольная сумма списка эффектов LE
  // CRF0        контрольная сумма списка картинок с внутренней файловой системы FL0
  // CRF1        контрольная сумма списка картинок с SD-карты FL1
  // CRLF        контрольная сумма списка роликов с SD-карты LF   
  // CT:X        режим цвета текстовой строки: 0,1,2
  // CT1:число   интервал показа даты при отображении часов (в секундах) - фаза T1/T3
  // CT2:число   время показа даты при отображении часов (в секундах) - фаза T2/T4
  // CF1:число   фазы цикла отображения часов/календаря в циклах T1-T4 - младшие 4 бита - часы, старшие 4 бита- календарь
  // CF2:число   фазы цикла отображения температуры в циклах T1-T4 - младшие 4 бита
  // CV:X        доступны вертикальные часы (по размерам), где Х = 0 - не доступны; 1 - доступны
  // DC:X        показывать дату вместе с часами 0-нет, 1-да
  // DH:число    использовать DHCP: 0 - нет, 1 - да
  // DM:Х        демо режим, где Х = 0 - ручное управление; 1 - авторежим
  // DW:X        показывать температуру вместе с часами 0-нет, 1-да
  // E0:X        поддержка протокола E1.31 0-нет, 1-есть
  // E1:X        режим работы 0 - STANDALONE, 1 - MASTER, 2 - SLAVE  
  // E2:X        тип данных работы 0 - PHYSIC, 1 - LOGIC, 2 - COMMAND
  // E3:X        группа синхронизации 0..9
  // E4:X        0 - вещание не идет (нет приема или передачи в режиме MASTER/SLAVE или текущий режим STANDALONE); 1 - идет вещание мастера, слушатели получают пакеты от мастера
  // EMX:X       X-позиция окна захвата изображения с мастера
  // EMY:X       Y-позиция окна захвата изображения с мастера
  // ELX:X       X-позиция окна вывода изображения с мастера в локальный viewport
  // ELY:X       Y-позиция окна вывода изображения с мастера в локальный viewport
  // ELW:X       ширина окна вывода изображения с мастера в локальный viewport
  // ELH:X       высота окна вывода изображения с мастера в локальный viewport  
  // EE:X        Наличие сохраненных настроек EEPROM на SD-карте или в файловой системе МК: 0 - нет 1 - есть в FS; 2 - есть на SD; 3 - есть в FS и на SD
  // EF:число    текущий эффект - id
  // EN:текст    текущий эффект - название
  // ER:текст    отправка клиенту сообщения инфо/ошибки последней операции (WiFiPanel - сохр. резервной копии настроек; WiFiPlayer - сообщение операции с изображением)
  // FG:X        состояние линии дополнительного управления битовая маска по режимам Use-OnOff
  // FK:X        состояние линии дополнительного управления 0: выключенаж 1 - включена
  // FL0:список  список файлов картинок нарисованных пользователем с внутренней памяти, разделенный запятыми
  // FL1:список  список файлов картинок нарисованных пользователем с SD-карты, разделенный запятыми
  // FM:X        количество свободной памяти
  // FS:X        доступность внутренней файловой системы микроконтроллера для хранения файлов: 0 - нет, 1 - да
  // FV:текст    строка использовать/не использовать эффект вида '0010011100...010', 0 - не использовать, 1 - использовать. Длина - по кол-ву эффектов, позиция в строке - индекс эффекта
  // HN:текст    Имя устройства (Host Name)
  // HTR:X       Флаг: флаг скрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
  // IP:xx.xx.xx.xx Статический IP адрес для подключения к WiFi, заданный в настройках
  // IPC:xx.xx.xx.xx Текущий IP адрес WiFi соединения в сети
  // IT:число    время бездействия в секундах
  // LE:список   список эффектов, разделенный запятыми, ограничители [] обязательны
  // LF:список   список файлов эффектов с SD-карты, разделенный запятыми, ограничители [] обязательны
  // LG:текст    идентификатор языка интерфейса, ограничители [] обязательны
  // M0:число    ширина одного сегмента матрицы 1..128
  // M1:число    высота одного сегмента матрицы 1..128
  // M2:число    тип соединения диодов в сегменте матрицы: 0 - зигзаг, 1 - параллельная
  // M3:число    угол подключения диодов в сегменте: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
  // M4:число    направление ленты из угла сегмента: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
  // M5:число    количество сегментов в ширину составной матрицы 1..15
  // M6:число    количество сегментов в высоту составной матрицы 1..15
  // M7:число    соединение сегментов составной матрицы: 0 - зигзаг, 1 - параллельная
  // M8:число    угол 1-го сегмента составной матрицы: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
  // M9:число    направление следующих сегментов составной матрицы из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
  // M10:список  список файлов карт индексов, разделенный запятыми, ограничители [] обязательны
  // M11:число   индекс текущей используемой карты индексов
  // MA:число    номер файла звука будильника из SD:/01
  // MB:число    номер файла звука рассвета из SD:/02
  // MC:текст    тип микроконтроллера "ESP32", "ESP32S2", "NodeMCU", "Wemos d1 mini"
  // MD:число    сколько минут звучит будильник, если его не отключили
  // MP:папка~файл  номер папки и файла звука который проигрывается, номер папки и звука разделены '~'
  // MU:X        использовать звук в будильнике 0-нет, 1-да
  // MV:число    максимальная громкость будильника
  // MX:X        MP3 плеер доступен для использования 0-нет, 1-да - USE_MP3 == 1 и подключение обнаружено, найдены файлы звуков
  // MZ:X        прошивка поддерживает MP3 плеер 0-нет, 1-да  - USE_MP3 == 1
  // NB:Х        яркость цвета ночных часов, где Х = 1..255
  // NС:Х        цвет ночных часов, где Х = 0 - R; 1 - G; 2 - B; 3 - C; 4 - M; 5 - Y;
  // NP:Х        использовать NTP, где Х = 0 - выкл; 1 - вкл
  // NS:текст    сервер NTP, ограничители [] обязательны
  // NW:текст    SSID сети подключения
  // NX:текст    пароль подключения к сети - в Web канал
  // NZ:текст    правило временной зоны
  // OF:X        выключать часы TM1637 вместе с лампой 0-нет, 1-да
  // PD:число    продолжительность режима в секундах
  // PN:список   список пинов, разделенный запятыми: SS,MOSI,MISO,SCK,SDA,SCL,TX,RX 
  // PS:X        состояние программного вкл/выкл панели 0-выкл, 1-вкл
  // PW:число    ограничение по току в миллиамперах
  // PZ0:X       использовать управление питанием матрицы 0-нет, 1-да --> USE_POWER
  // PZ1:X       использовать управление питанием по линии будильника 0-нет, 1-да --> USE_ALARM
  // PZ2:X       использовать управление питанием по дополнительной линии 0-нет, 1-да --> USE_AUX
  // RM:Х        смена режимов в случайном порядке, где Х = 0 - выкл; 1 - вкл
  // S1:список   список звуков будильника, разделенный запятыми
  // S2:список   список звуков рассвета, разделенный запятыми
  // S3:список   список звуков для макроса {A} бегущей строки
  // SC:число    скорость смещения часов оверлея
  // SD:X        наличие и доступность SD карты: Х = 0 - нат SD карты; 1 - SD карта доступна
  // SE:число    скорость эффектов
  // SM:число    текущий "специальный режим" или -1 если работа в обычном режиме 
  // SS:число    параметр #1 эффекта
  // SQ:спец     параметр #2 эффекта; спец - "L>val>item1,item2,..itemN" - список, где val - текущее, далее список; "C>x>title" - чекбокс, где x=0 - выкл, x=1 - вкл; title - текст чекбокса
  // ST:число    скорость смещения бегущей строки
  // SX:число    наличие и доступность SD карты в системе: Х = 0 - нeт SD карты; 1 - SD карта доступна
  // SZ:число    поддержка прошивкой функционала SD карты в системе - USE_SD: Х = 0 - USE_SD = 0; USE_SD = 1
  // TE:X        оверлей текста бегущей строки вкл/выкл, где Х = 0 - выкл; 1 - вкл (использовать бегущую строку в эффектах)
  // T1:[ЧЧ:MM]  время рассвета, полученное с погодного сервера
  // T2:[ЧЧ:MM]  время заката, полученное с погодного сервера
  // TF:X        Температура C/F: 0 - Цельсий; 1 - Фаренгейт 
  // TI:число    интервал отображения текста бегущей строки
  // TM:X        в системе присутствует индикатор TM1637, где Х = 0 - нет; 1 - есть
  // TS:строка   строка состояния кнопок выбора текста из массива строк: TEXTS_MAX_COUNT символов 0..5, где
  //               0 - серый - пустая
  //               1 - черный - отключена
  //               2 - зеленый - активна - просто текст? без макросов
  //               3 - голубой - активна, содержит макросы кроме даты
  //               4 - синий - активная, содержит макрос даты
  //               5 - красный - для строки 0 - это управляющая строка
  // TY:[Z:текст] текст для строки, с указанным индексом I 0..35, Z 0..9,A..Z. Ограничители [] обязательны; текст ответа в формате: 'I:Z > текст'; 
  // UВ:X        использовать кнопку 0-нет, 1-да --> USE_BUTTON
  // UC:X        использовать часы поверх эффекта 0-нет, 1-да
  // UP:число    uptime системы в секундах
  // UT:X        использовать бегущую строку поверх эффекта 0-нет, 1-да
  // VR:строка   версия прошивки
  // W1:текст    текущая погода ('ясно','пасмурно','дождь' и т.д.)
  // W2:число    текущая температура
  // WC:X        Использовать цвет для отображения температуры в дневных часах  X: 0 - выключено; 1 - включено
  // WN:X        Использовать цвет для отображения температуры в ночных часах  X: 0 - выключено; 1 - включено
  // WR:число    Регион погоды Yandex
  // WS:число    Регион погоды OpeenWeatherMap
  // WT:число    Период запроса сведений о погоде в минутах
  // WU:X        Использовать получение погоды с сервера: 0 - выключено; 1 - включено
  // WV:X        Как показывать температуры в часах (битовая карта) b0 - показывать C/F, b1 - показывать знак градуса
  // WW:X        Как показывать температуры в макросе {WT} (битовая карта) b0 - показывать C/F, b1 - показывать знак градуса
  // WZ:X        Прошивка поддерживает погоду USE_WEATHER == 1 - 0 - выключено; 1 - включено
  //
  // 2306:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2307:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2308:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2309:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка
  // 2310:X       тип матрицы vDEVICE_TYPE: 0 - труба; 1 - панель
  // 2311:X Y    X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
  // 2312:X Y    X - GPIO пин управления питанием матрицы; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
  // 2313:X Y    X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
  // 2314:X      vDEBUG_SERIAL
  // 2315:X Y    X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
  // 2316:X Y    X - GPIO пин DIO на TN1637; Y - GPIO пин CLK на TN1637
  // 2317:X Y    X - GPIO пин управления питанием по линии будильника; Y - уровень управления питанием по линии будильника - 0 - LOW; 1 - HIGH
  // 2318:X Y    X - GPIO пин управления дополнительной линии питания; Y - уровень управления дополнительной линии питания - 0 - LOW; 1 - HIGH
 
  CRGB c;

  if (key == "CRLE") {
    uint16_t len = strlen_P(EFFECT_LIST); 
    char buf[len + 1];
    strcpy_P(buf, EFFECT_LIST); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRLE:");
    str += crc;
    return str; 
  }

  // Список эффектов прошивки
  if (key == "LE") {
    if (shrt) {
      return FPSTR(EFFECT_LIST);
    }
    String str("LE:");
    str += FPSTR(EFFECT_LIST);
    return str; 
  }

  // Контрольная сумма списка картинок с внутренней файловой системы
  if (key == "CRF0") {
    String tmp(getStoredImages("FS", pWIDTH, pHEIGHT));
    uint16_t len = tmp.length(); 
    char buf[len + 1];
    strcpy(buf, tmp.c_str()); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRF0:");
    str += crc;
    return str; 
  }

  // Список картинок с внутренней файловой системы
  if (key == "FL0") {
    if (shrt) {
      return getStoredImages("FS", pWIDTH, pHEIGHT);
    }
    String str("FL:");
    str += getStoredImages("FS", pWIDTH, pHEIGHT);
    return str; 
  }

#if (USE_SD == 1)

  if (key == "CRF1") {
    String tmp(getStoredImages("SD", pWIDTH, pHEIGHT));
    uint16_t len = tmp.length(); 
    char buf[len + 1];
    strcpy(buf, tmp.c_str()); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRF1:");
    str += crc;
    return str; 
  }

  // Список картинок с карты SD
  if (key == "FL1") {
    if (shrt) {
      return getStoredImages("SD", pWIDTH, pHEIGHT);
    }
    String str("FL:");
    str += getStoredImages("SD", pWIDTH, pHEIGHT);
    return str; 
  }

  // Cписок файлов эффектов с SD-карты, разделенный запятыми
  if (key == "CRLF") {
    String tmp;
    for (uint8_t i=0; i < countFiles; i++) {
      tmp += "," + nameFiles[i];
    }
    if (tmp.length() > 0) tmp = tmp.substring(1);
    uint16_t len = tmp.length(); 
    char buf[len + 1];
    strcpy(buf, tmp.c_str()); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRLF:");
    str += crc;
    return str; 
  }

  // Cписок файлов эффектов с SD-карты, разделенный запятыми
  if (key == "LF") {    
    String tmp;
    for (uint8_t i=0; i < countFiles; i++) {
      tmp += "," + nameFiles[i];
    }
    if (tmp.length() > 0) tmp = tmp.substring(1);
    if (shrt) {
      return tmp;
    }
    String str("LF:");
    str += tmp;
    return str; 
  }
#endif

#if (USE_MP3 == 1)

  // Запрос контролльной суммы списка звуков будильника
  if (key == "CRS1") {
    uint16_t len = strlen_P(ALARM_SOUND_LIST); 
    char buf[len + 1];
    strcpy_P(buf, ALARM_SOUND_LIST); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRS1:");
    str += crc;
    return str; 
  }

  // Запрос контролльной суммы списка звуков рассвета
  if (key == "CRS2") {
    uint16_t len = strlen_P(DAWN_SOUND_LIST); 
    char buf[len + 1];
    strcpy_P(buf, DAWN_SOUND_LIST); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRS2:");
    str += crc;
    return str; 
  }

  // Запрос контролльной суммы списка звуков бегущей строки
  if (key == "CRS3") {
    uint16_t len = strlen_P(NOTIFY_SOUND_LIST); 
    char buf[len + 1];
    strcpy_P(buf, NOTIFY_SOUND_LIST); 
    uint8_t crc = getCrc8(reinterpret_cast<uint8_t*>(buf), len);
    if (shrt) {
      return String(crc);
    }
    String str("CRS3:");
    str += crc;
    return str; 
  }

  // Запрос звуков будильника
  if (key == "S1") {
    if (shrt) {
      return FPSTR(ALARM_SOUND_LIST);
    }
    String str("S1:");
    str += FPSTR(ALARM_SOUND_LIST);;
    return str; 
  }

  // Запрос звуков рассвета
  if (key == "S2") {
    if (shrt) {
      return FPSTR(DAWN_SOUND_LIST);
    }
    String str("S2:");
    str += FPSTR(DAWN_SOUND_LIST);
    return str;
  }

  // Запрос звуков бегущей строки
  if (key == "S3") {
    if (shrt) {
      return FPSTR(NOTIFY_SOUND_LIST);
    }
    String str("S3:");
    str += FPSTR(NOTIFY_SOUND_LIST);
    return str; 
  }
#endif

  // Версия прошивки
  if (key == "VR") {
    if (shrt) {
      return FIRMWARE_VER;
    }
    String str("VR:");
    str += FIRMWARE_VER;;
    return str;
  }

  // Имя устройства
  if (key == "HN") {
    String tmp(getSystemName());
    if (tmp.length() == 0) tmp = host_name;
    if (shrt) {
      return tmp;
    }
    String str("HN:");
    str += tmp;
    return str;
  }

  // Идентификатор языка интерфейса
  if (key == "LG") {
    if (shrt) {
      return UI;
    }
    String str("LG:");
    str += UI;
    return str;
  }

  // Ширина матрицы
  if (key == "W")  {
    if (shrt) {
      return String(pWIDTH);
    }
    String str("W:");
    str += pWIDTH;
    return str;
  }

  // Высота матрицы
  if (key == "H")  {
    if (shrt) {
      return String(pHEIGHT);
    }
    String str("H:");
    str += pHEIGHT;
    return str;
  }

  // Программное вкл/выкл устройства
  if (key == "PS") {
    if (shrt) {
      return String(!isTurnedOff);
    }
    String str("PS:");
    str += !isTurnedOff;
    return str;
  }

  // Текущее состояние дополнительной линии управления питанием
  if (key == "FK") {
    if (shrt) {
      return String(isAuxActive);
    }
    String str("FK:");
    str += isAuxActive;
    return str;
  }

  // Битовая маска режимов использования дополнительной линии управления питанием
  if (key == "FG") {
    if (shrt) {
      return String(auxLineModes);
    }
    String str("FG:");
    str += auxLineModes;
    return str;
  }

  // Текущая яркость
  if (key == "BR") {
    uint8_t br = (isNightClock ? nightClockBrightness : (specialMode ? specialBrightness : deviceBrightness));
    if (shrt) {
      return String(br);
    }
    String str("BR:");
    str += br;
    return str;
  }

  if (key == "BS") {
    uint8_t val = getGameButtonSpeed();
    if (shrt) {
      return String(val);
    }
    String str("BS:");
    str += val;
    return str;
  }

  // Ручной / Авто режим
  if (key == "DM") {
    if (shrt) {
      return String(!manualMode);
    }
    String str("DM:");
    str += !manualMode;
    return str;
  }

  // Продолжительность режима в секундах
  if (key == "PD") {
    if (shrt) {
      return String(autoplayTime / 1000);
    }
    String str("PD:");
    str += (autoplayTime / 1000);
    return str; 
  }

  // PN:список   список пинов, разделенный запятыми: SS,MOSI,MISO,SCK,SDA,SCL,TX,RX
  // Эти пины в цифровом назначении зависят от выбранного типа платы микроконтроллера
  if (key == "PN") {
    String tmp(SS); tmp += ','; tmp += MOSI; tmp += ','; tmp += MISO; tmp += ','; tmp += SCK; tmp += ','; tmp += SDA; tmp += ','; tmp += SCL; tmp += ','; tmp += TX; tmp += ','; tmp += RX;
    if (shrt) {
      return tmp;
    }
    String str("PN:");
    str += tmp;
    return str; 
  }

  // Время бездействия в минутах
  if (key == "IT") {
    if (shrt) {
      return String(idleTime / 60 / 1000);
    }
    String str("IT:");
    str += (idleTime / 60 / 1000);
    return str;
  }

  // Сработал будильник 0-нет, 1-да
  if (key == "AL") {
    if (shrt) {
      return String((isAlarming || isPlayAlarmSound) && !isAlarmStopped);
    }
    String str("AL:");
    str +=  ((isAlarming || isPlayAlarmSound) && !isAlarmStopped);
    return str; 
  }

  // Смена режимов в случайном порядке, где Х = 0 - выкл; 1 - вкл
  if (key == "RM") {
    if (shrt) {
      return String(useRandomSequence);
    }
    String str("RM:");
    str += useRandomSequence;
    return str;
  }

  // Ограничение по току в миллиамперах
  if (key == "PW") {
    if (shrt) {
      return String(CURRENT_LIMIT);
    }
    String str("PW:");
    str += CURRENT_LIMIT;
    return str;
  }

  // Прошивка поддерживает погоду 
  if (key == "WZ") {
    if (shrt) {
      return String(USE_WEATHER == 1);
    }
    String str("WZ:");
    str += (USE_WEATHER == 1);
    return str;
  }

#if (USE_WEATHER == 1)               
  // Температура в Цельсиях / Фаренгейте
  if (key == "TF") {
    if (shrt) {
      return String(isFarenheit);
    }
    String str("TF:");
    str += isFarenheit;
    return str;
  }
   
  // Использовать получение погоды с сервера: 0 - выключено; 1 - Yandex; 2 - OpenWeatherMap
  if (key == "WU") {
    if (shrt) {
      return String(useWeather);
    }
    String str("WU:");
    str += useWeather;
    return str;
  }

  // Период запроса сведений о погоде в минутах
  if (key == "WT") {
    if (shrt) {
      return String(SYNC_WEATHER_PERIOD);
    }
    String str("WT:");
    str += SYNC_WEATHER_PERIOD;
    return str;
  }

  // Регион погоды Yandex
  if (key == "WR") {
    if (shrt) {
      return String(regionID);
    }
    String str("WR:");
    str += regionID;
    return str;
  }

  // Регион погоды OpenWeatherMap
  if (key == "WS") {
    if (shrt) {
      return String(regionID2);
    }
    String str("WS:");
    str += regionID2;
    return str;
  }

  // Использовать цвет для отображения температуры в дневных часах: 0 - выключено; 1 - включено
  if (key == "WC") {
    if (shrt) {
      return String(useTemperatureColor);
    }
    String str("WC:");
    str += useTemperatureColor;
    return str;
  }

  // Использовать цвет для отображения температуры в ночных часах: 0 - выключено; 1 - включено
  if (key == "WN") {
    if (shrt) {
      return String(useTemperatureColorNight);
    }
    String str("WN:");
    str += useTemperatureColorNight;
    return str;
  }

  // Текущая погода
  if (key == "W1") {
    if (shrt) {
      return weather;
    }
    String str("W1:");
    str += weather;
    return str;
  }

  // Текущая температура
  if (key == "W2") {
    if (shrt) {
      return String(temperature);
    }
    String str("W2:");
    str += temperature;
    return str;
  }
  
  // Время рассвета, полученное с погодного сервера
  if (key == "T1") {
    String t1(padNum(dawn_hour,2)); t1 += ':'; t1 += padNum(dawn_minute,2);
    if (shrt) {
      return t1;
    }
    String str("T1:");
    str += t1;
    return str;
  }

  // Время заката, полученное с погодного сервера
  if (key == "T2") {
    String t2(padNum(dusk_hour,2)); t2 += ':'; t2 += padNum(dusk_minute,2);
    if (shrt) {
      return t2;
    }
    String str("T2:");
    str += t2;
    return str;
  }

  // Отображать в температуре малым шрифтом значок градуса и букву C/F - в часах
  if (key == "WV") {
    int8_t value = (showTempDegree ? 0x02 : 0x00) | (showTempLetter ? 0x01 : 0x00);
    if (shrt) {
      return String(value);
    }
    String str("WV:");
    str += value;
    return str;
  }
  
  // Отображать в температуре малым шрифтом значок градуса и букву C/F - в макросе {WT} бегущей строки
  if (key == "WW") {
    int8_t value = (showTempTextDegree ? 0x02 : 0x00) | (showTempTextLetter ? 0x01 : 0x00);
    if (shrt) {
      return String(value);
    }
    String str("WW:");
    str += value;
    return str;
  }
#endif  

  // Текущий эффект - id
  if (key == "EF") {
    if (shrt) {
      return String(effect);
    }
    String str("EF:");
    str += effect;
    return str;
  }

  // Текущий эффект - название
  if (key == "EN") {
    if (effect_name.length() == 0) {
      effect_name = getEffectName(effect);
    }
    if (shrt) {
      return effect_name;
    }
    String str("EN:");
    str+= effect_name;
    return str;
  }

  // Использование эффектов - строка кодирования порядка следования эффектов в случае их последовательного воспроизведения
  // Номер эффекта (ID) - кодируется буквой 0..9,A..Z,a..z - строка IDX_LINE, порядок воспроизведения - строкой effect_order
  // Эффекты, которые отсутствуют в строке effect_order - не используются совсем. Пример строки - "FR1H5aShik";
  // Например буква F соответствует эффекту с ID=15 - MC_NOISE_RAINBOW, воспроизводится первым в опорядке, далее - эффект 'R' и т.д до конца строки
  if (key == "FV") {
    if (shrt) {
      return effect_order;
    }
    String str("FV:");
    str += effect_order;
    return str;
  }
  
  // Оверлей бегущей строки
  if (key == "UT") {
    String tmp(getEffectTextOverlayUsage(effect));
    if (shrt) {
      if (effect == MC_MAZE || effect == MC_SNAKE || effect == MC_TETRIS || effect == MC_ARKANOID) {
        return String("X");
      }
      return tmp;
    }
    String str("UT:");
    str += (effect == MC_MAZE || effect == MC_SNAKE || effect == MC_TETRIS || effect == MC_ARKANOID) 
      ? String("X") 
      : tmp;
    return str;
  }

  // Оверлей часов   
  if (key == "UC") {
    if (shrt) {
       if (effect == MC_MAZE || effect == MC_SNAKE || effect == MC_TETRIS || effect == MC_ARKANOID || effect == MC_CLOCK) {
         return "X"; 
       }
       return String(getEffectClockOverlayUsage(effect));
    }
    String str("UC:");
    str += (effect == MC_MAZE || effect == MC_SNAKE || effect == MC_TETRIS || effect == MC_ARKANOID || effect == MC_CLOCK) 
      ? String("X") 
      : String(getEffectClockOverlayUsage(effect));
    return str;     
  }

  // Настройка скорости
  if (key == "SE") {
    if (shrt) {
      if (effect == MC_PACIFICA || effect == MC_SHADOWS || effect == MC_CLOCK || effect == MC_FIRE2 || effect == MC_IMAGE) {
        return String("X");
      }
      return String(255 - constrain(map(getEffectSpeed(effect), D_EFFECT_SPEED_MIN,D_EFFECT_SPEED_MAX, 0,255), 0,255));
    }
    String str("SE:");
    str += (effect == MC_PACIFICA || effect == MC_SHADOWS || effect == MC_CLOCK || effect == MC_FIRE2 || effect == MC_IMAGE 
         ? String("X")
         : String(255 - constrain(map(getEffectSpeed(effect), D_EFFECT_SPEED_MIN,D_EFFECT_SPEED_MAX, 0,255), 0,255)));
    return str;     
  }

  // Контраст
  if (key == "BE") {
    String tmp(getEffectContrast(effect));
    if (shrt) {
      if (effect == MC_PACIFICA || effect == MC_DAWN_ALARM || effect == MC_MAZE || effect == MC_SNAKE || effect == MC_TETRIS || effect == MC_ARKANOID || effect == MC_CLOCK || effect == MC_SDCARD) {
        return String("X");
      }  
      return tmp;
    }
    String str("BE:");
    str += (effect == MC_PACIFICA || effect == MC_DAWN_ALARM || effect == MC_MAZE || effect == MC_SNAKE || effect == MC_TETRIS || effect == MC_ARKANOID || effect == MC_CLOCK || effect == MC_SDCARD)
         ? String("X") 
         : tmp;
    return str;     
  }

  // Эффекты не имеющие настройки вариации (параметр #1) отправляют значение "Х" - программа делает ползунок настройки недоступным
  if (key == "SS") {
    if (shrt) {
      return String(getParamForMode(effect));
    }
    String str("SS:");
    str += getParamForMode(effect);
    return str;
  }

  // Эффекты не имеющие настройки вариации (параметр #2) отправляют значение "Х" - программа делает ползунок настройки недоступным
  if (key == "SQ") {
    if (shrt) {
      return String(getParam2ForMode(effect));
    }
    String str("SQ:");
    str += getParam2ForMode(effect);
    return str;
  }

  // Разрешен оверлей бегущей строки
  if (key == "TE") {
    if (shrt) {
      return String(textOverlayEnabled);
    }
    String str("TE:");
    str += textOverlayEnabled;
    return str;
  }

  // Интервал показа бегущей строки
  if (key == "TI") {
    if (shrt) {
      return String(TEXT_INTERVAL);
    }
    String str("TI:");
    str += TEXT_INTERVAL;
    return str;
  }

  // Режим цвета отображения бегущей строки
  if (key == "CT") {
    if (shrt) {
      return String(COLOR_TEXT_MODE);
    }
    String str("CT:");
    str += COLOR_TEXT_MODE;
    return str;
  }

  // Скорость прокрутки текста
  if (key == "ST") {
    if (shrt) {
      return String(255 - textScrollSpeed);
    }
    String str("ST:");
    str += (255 - textScrollSpeed);
    return str;
  }

  // Цвет режима "монохром" часов
  if (key == "C1") {
    c = CRGB(globalClockColor);
    String tmp(c.r); tmp += ','; tmp += c.g; tmp += ','; tmp += c.b;
    if (shrt) {
      return tmp;
    }
    String str("C1:");
    str += tmp;
    return str;
  }

  // Цвет режима "монохром" бегущей строки
  if (key == "C2") {
    c = CRGB(globalTextColor);
    String tmp(c.r); tmp += ','; tmp += c.g; tmp += ','; tmp += c.b;
    if (shrt) {
      return tmp;
    }
    String str("C2:");
    str += tmp;
    return str;
  }

  // 12/24 часовой формат времени
  if (key == "C12") {
    if (shrt) {
      return String(time_h12 ? '1' : '0');
    }
    String str("C12:");
    str += (time_h12 ? '1' : '0');
    return str;
  }

  // Тип шривта 3х5
  if (key == "C35") {
    if (shrt) {
      return String(use_round_3x5 ? '1' : '0');
    }
    String str("C35:");
    str += (use_round_3x5 ? '1' : '0');
    return str;
  }

  // Строка состояния заполненности строк текста
  if (key == "TS") {
    if (shrt) {
      return textStates;
    }
    String str("TS:");
    str += textStates;
    return str;
  }

  // Исходная строка с индексом editIdx без обработки для отправки в приложение для формирования
  if (key == "TY") {
    String tmp; tmp.reserve(150);
    if (editIdx >= 0 && editIdx < TEXTS_MAX_COUNT) {
      tmp += editIdx; tmp += ':'; tmp += getAZIndex(editIdx); tmp += " > "; tmp += getTextByIndex(editIdx);
    } else {
      tmp += "-1: >";      
    }
    if (shrt) {
      return tmp;
    }
    String str("TY:");
    str += tmp;
    return str;
  }

  // Оверлей часов вкл/выкл
  if (key == "CE") {
    if (shrt) {
      if (allowVertical || allowHorizontal) {
        return String(getClockOverlayEnabled());
      } 
      return String("X");
    }
    String str("CE:");
    str += (allowVertical || allowHorizontal) ? String(getClockOverlayEnabled()) : String("X");
    return str;
  }

  // Отображение года в кадендаре - 2/4 цифры
  if (key == "CF") {
    if (shrt) {
      return String(getCalendarYearWidth());
    }
    String str("CF:");
    str += getCalendarYearWidth();
    return str;
  }

  // Режим цвета часов оверлея
  if (key == "CC") {
    if (shrt) {
      return String(COLOR_MODE);
    }
    String str("CC:");
    str += COLOR_MODE;
    return str;
  }

  // Ширина точки в больших часах
  if (key == "CD") {
    if (shrt) {
      return String(clockDotWidth);
    }
    String str("CD:");
    str += clockDotWidth;
    return str;
  }
  
  // Наличие пробелов между цифрами и точкой в больих часах
  if (key == "CS") {
    if (shrt) {
      return String(clockDotSpace);
    }
    String str("CS:");
    str += clockDotSpace;
    return str;
  }

  // Флаги вариантов размещения календаря и темепратуры - относительно часов или произвольно по указанному смещению
  if (key == "CSV") {
    if (shrt) {
      return String(clock_show_variant);
    }
    String str("CSV:");
    str += clock_show_variant;
    return str;
  }

  // Флаги выравнивани часов/календаря/температуры в области их отображения;
  if (key == "CSA") {
    if (shrt) {
      return String(clock_show_alignment);
    }
    String str("CSA:");
    str += clock_show_alignment;
    return str;
  }

  // Флагскрывать при бегущей строке b0 - часы  b1 - календарь  b2 - температуру
  if (key == "HTR") {
    if (shrt) {
      return String(hide_on_text_running);
    }
    String str("HTR:");
    str += hide_on_text_running;
    return str;
  }

  // Доступны горизонтальные часы
  if (key == "CH") {
    if (shrt) {
      return String(allowHorizontal);
    }
    String str("CH:");
    str += allowHorizontal;
    return str;
  }

  // Доступны вертикальные часы
  if (key == "CV") {
    if (shrt) {
      return String(allowVertical);
    }
    String str("CV:");
    str += allowVertical;
    return str;
  }

  // Цвет рисования
  if (key == "CL") {
    if (shrt) {
      String tmp(IntToHex(drawColor));
      return tmp;
    }
    String str("CL:");
    str += IntToHex(drawColor);
    return str;
  }

  // Ориентация часов
  if (key == "CO") {
    if (shrt) {
      if (allowVertical || allowHorizontal) {
        return String(CLOCK_ORIENT);
      }
      return String("X");
    }
    String str("CO:");
    str += (allowVertical || allowHorizontal) ? String(CLOCK_ORIENT) : String("X");
    return str;
  }

  // Размер (режим) горизонтальных часов
  if (key == "CK") {
    if (shrt) {
      return String(CLOCK_SIZE);
    }
    String str("CK:");
    str += CLOCK_SIZE;
    return str;
  }

  // Смещение часов по горизонтали (по оси Х)
  if (key == "CX") {
    if (shrt) {
      return String(clockOffsetX);
    }
    String str("CX:");
    str += clockOffsetX;
    return str;
  }

  // Смещение часов по вертикали (по оси Y)
  if (key == "CY") {
    if (shrt) {
      return String(clockOffsetY);
    }
    String str("CY:");
    str += clockOffsetY;
    return str;
  }

  // Смещение температуры по горизонтали (по оси Х)
  if (key == "CXT") {
    if (shrt) {
      return String(tempOffsetX);
    }
    String str("CXT:");
    str += tempOffsetX;
    return str;
  }

  // Смещение температуры по вертикали (по оси Y)
  if (key == "CYT") {
    if (shrt) {
      return String(tempOffsetY);
    }
    String str("CYT:");
    str += tempOffsetY;
    return str;
  }

  // Смещение календаря по горизонтали (по оси Х)
  if (key == "CXC") {
    if (shrt) {
      return String(calendarOffsetX);
    }
    String str("CXC:");
    str += calendarOffsetX;
    return str;
  }

  // Смещение календаря по вертикали (по оси Y)
  if (key == "CYC") {
    if (shrt) {
      return String(calendarOffsetY);
    }
    String str("CYC:");
    str += calendarOffsetY;
    return str;
  }

  // Смещение бегущей строки по вертикали (по оси Y)
  if (key == "CYL") {
    if (shrt) {
      return String(textOffsetY);
    }
    String str("CYL:");
    str += textOffsetY;
    return str;
  }

  // Яркость цвета ночных часов
  if (key == "NB") {
    if (shrt) {
      return String(nightClockBrightness);
    }
    String str("NB:");
    str += nightClockBrightness;
    return str;
  }

  // Код цвета ночных часов
  if (key == "NC") {
    if (shrt) {
      return String(nightClockColor);
    }
    String str("NC:");
    str += nightClockColor;
    return str;
  }

  // Скорость смещения (прокрутки) часов оверлея
  if (key == "SC") {
    if (shrt) {
      return String(255 - clockScrollSpeed);
    }
    String str("SC:");
    str += (255 - clockScrollSpeed);
    return str;
  }

  // Интервал отображения даты часов
  if (key == "CT1") {
    if (shrt) {
      return String(clock_cycle_T1);
    }
    String str("CT1:");
    str += clock_cycle_T1;
    return str;
  }

  // Продолжительность отображения даты в режиме часов (сек)
  if (key == "CT2") {
    if (shrt) {
      return String(clock_cycle_T2);
    }
    String str("CT2:");
    str += clock_cycle_T2;
    return str;
  }

  // Флаги отображения часов/календаря в циклах T1-T4
  if (key == "CF1") {
    if (shrt) {
      return String(clock_cycle_F1);
    }
    String str("CF1:");
    str += clock_cycle_F1;
    return str;
  }

  // Флаги отображения температуры в циклах T1-T4
  if (key == "CF2") {
    if (shrt) {
      return String(clock_cycle_F2);
    }
    String str("CF2:");
    str += clock_cycle_F2;
    return str;
  }

  // Использовать получение времени с интернета
  if (key == "NP") {
    if (shrt) {
      return String(useNtp);
    }
    String str("NP:");
    str += useNtp;
    return str;
  }

  // Отправка в UI правила временной зоны
  if (key == "NZ") { 
    String tmp(getTimeZone());
    if (tmp.length() == 0) tmp = TZONE;
    if (shrt) {
      return tmp;
    }
    String str("NP:");
    str += tmp;
    return str;
  }
  
  // Имя сервера NTP (url)
  if (key == "NS") {
    String tmp(getNtpServer());
    if (shrt) {
      return tmp;
    }
    String str("NS:");
    str += tmp;
    return str;
  }

  // Выключать часы TM1637 вместе с лампой 0-нет, 1-да
  if (key == "OF") {
    if (shrt) {
      return String(needTurnOffClock);
    }
    String str("OF:");
    str += needTurnOffClock;
    return str;
  }

  // Продолжительность рассвета, мин
  if (key == "AD") {
    if (shrt) {
      return String(dawnDuration);
    }
    String str("AD:");
    str += dawnDuration;
    return str;
  }

  // Битовая маска дней недели будильника
  if (key == "AW") {
    if (shrt) {
      return String(alarmWeekDay);
    }
    String str("AW:");
    str.reserve(22);
    for (uint8_t i=0; i<7; i++) {
       if (((alarmWeekDay>>i) & 0x01) == 1) str += "1"; else str += "0";  
       if (i<6) str += '.';
    }
    return str;
  }

  // Часы-минуты времени будильника по дням недели
  if (key == "AT") {
    String str;
    str.reserve(80);
    if (shrt) {
      for (uint8_t i=0; i<7; i++) {      
        str += "|";
        str += i + 1; 
        str += ' ';
        str += alarmHour[i]; 
        str += ' ';
        str += alarmMinute[i];
      }
      return str.substring(1);
    }
    for (uint8_t i=0; i<7; i++) {      
      str += "|AT:";
      str += i + 1;
      str += ' ';
      str += alarmHour[i];
      str += ' ';
      str += alarmMinute[i];
    }
    // Убрать первый '|'
    return str.substring(1);
  }

  // Эффект применяемый в рассвете: Индекс в списке в приложении смартфона начинается с 1
  if (key == "AE") {
    if (shrt) {
      return String(alarmEffect);
    }
    String str("AE:");
    str += alarmEffect;
    return str;
  }

  // Сколько минут звучит будильник, если его не отключили
  if (key == "MD") {
    if (shrt) {
      return String(alarmDuration);
    }
    String str("MD:");
    str += alarmDuration;
    return str; 
  }

  // Текущий "специальный режим" или -1 если нет активного
  if (key == "SM") {
    if (shrt) {
      return String(specialModeId);
    }
    String str("SM:");
    str += specialModeId;
    return str; 
  }

  // Доступность MP3-плеера - разрешен в прошивке USE_MP3? неважно подключен или нет
  if (key == "MZ") {
    if (shrt) {
      return String(USE_MP3 == 1);
    }
    String str("MZ:");
    str +=(USE_MP3 == 1);
    return str;
  }

  // Доступность MP3-плеера - разрешен в прошивке USE_MP3 и обнаружено подключение    
  if (key == "MX") {
    if (shrt) {
      return String(isDfPlayerOk);
    }
    String str("MX:");
    str += isDfPlayerOk;
    return str;
  }

  // Наличие индикатора TM1637    
  if (key == "TM") {
    if (shrt) {
      return String(USE_TM1637 == 1);
    }
    String str("TM:");
    str += (USE_TM1637 == 1);
    return str;
  }

#if (USE_MP3 == 1)
  // Использовать звук будильника
  if (key == "MU") {
    if (shrt) {
      return String(useAlarmSound); 
    }
    String str("MU:");
    str += useAlarmSound; 
    return str; 
  }

  // Максимальная громкость будильника
  if (key == "MV") {
    if (shrt) {
      return String(maxAlarmVolume);
    }
    String str("MV:");
    str += maxAlarmVolume;
    return str; 
  }

  // Номер файла звука будильника из SD:/01
  if (key == "MA") {
    if (shrt) {
      return String(alarmSound);
    }
    String str("MA:");
    str += (alarmSound + 2);
    return str;                      // Знач: -1 - нет; 0 - случайно; 1 и далее - файлы; -> В списке индексы: 1 - нет; 2 - случайно; 3 и далее - файлы
  }

  // Номер файла звука рассвета из SD:/02
  if (key == "MB") {
    if (shrt) {
      return String(dawnSound);
    }
    String str("MB:");
    str += (dawnSound + 2);
    return str;                       // Знач: -1 - нет; 0 - случайно; 1 и далее - файлы; -> В списке индексы: 1 - нет; 2 - случайно; 3 и далее - файлы
  }

  // Номер папки и файла звука который проигрывается
  if (key == "MP") {
    String tmp(soundFolder); tmp += '~'; tmp += (soundFile + 2);
    if (shrt) {
      return tmp;
    }
    String str("MP:");
    str += tmp;
    return str; 
  }
#endif

  // uptime - время работы системы в секундах
  if (key == "UP") {
    uint32_t upt = millis()/1000;
    if (shrt) {
      return String(upt);  
    }
    String str("UP:");
    str += upt;
    return str;
  }

  // freemem - количество свободной памяти
  if (key == "FM") {
    int32_t freemem = ESP.getFreeHeap();
    if (shrt) {
      return String(freemem);  
    }
    String str("FM:");
    str += freemem;
    return str;
  }

  // создавать точку доступа
  if (key == "AU") {
    if (shrt) {
      return String(useSoftAP);  
    }
    String str("AU:");
    str += useSoftAP;
    return str;
  }

  // Имя точки доступа
  if (key == "AN") {
    String tmp(getSoftAPName());
    if (shrt) {
      return tmp;
    }
    String str("AN:");
    str += tmp;
    return str;
  }

  // Пароль точки доступа - Web-канал
  if (key == "AB") {
    String tmp(getSoftAPPass());
    if (shrt) {
      return tmp;
    }
    String str("AB:");
    str += tmp;
    return str;
  }

  // Имя локальной сети (SSID)
  if (key == "NW") {
    String tmp(getSsid());
    if (shrt) {
      return tmp;
    }
    String str("NW:");
    str += tmp;
    return str;
  }

  // Пароль к сети - Web канал
  if (key == "NX") {
    String tmp(getPass());
    if (shrt) {
      return tmp;
    }
    String str("NX:");
    str += tmp;
    return str;
  }

  // IP адрес - настройки
  if (key == "IP") {
    String tmp;

    if (IP_STA[0] + IP_STA[1] + IP_STA[2] + IP_STA[3] > 0) {
      tmp  = String(IP_STA[0]);   tmp += ".";
      tmp += IP_STA[1];           tmp += ".";
      tmp += IP_STA[2];           tmp += ".";
      tmp += IP_STA[3];
    } else {
      tmp = String(wifi_connected ? WiFi.localIP().toString() : "");
    }
    
    if (shrt) {
      return tmp;
    }

    String str("IP:");
    str += tmp;
    return str;
  }

  // IP адрес - текущий
  if (key == "IPC") {
    String tmp(wifi_connected ? WiFi.localIP().toString() : "");    
    if (shrt) {
      return tmp;
    }
    String str("IPC:");
    str += tmp;
    return str;
  }

  // Использовать DHCP
  if (key == "DH") {
    bool useDHCP = getUseDHCP();
    if (shrt) {
      return String(useDHCP);  
    }
    String str("DH:");
    str += useDHCP;
    return str;
  }

  // Время Режима №1
  if (key == "AM1T") {
    String tmp(padNum(AM1_hour,2)); tmp += ' '; tmp += padNum(AM1_minute, 2);
    if (shrt) {
      return tmp;
    }
    String str("AM1T:");
    str += tmp;
    return str;
  }

  // Действие Режима №1
  if (key == "AM1A") {
    if (shrt) {
      return String(AM1_effect_id);
    }
    String str("AM1A:");
    str += AM1_effect_id;
    return str;
  }

  // Время Режима №2
  if (key == "AM2T") {
    String tmp(padNum(AM2_hour,2)); tmp += ' '; tmp += padNum(AM2_minute, 2);
    if (shrt) {
      return tmp;
    }
    String str("AM2T:");
    str += tmp;
    return str;
  }

  // Действие Режима №2
  if (key == "AM2A") {
    if (shrt) {
      return String(AM2_effect_id);
    }
    String str("AM2A:");
    str += AM2_effect_id;
    return str;
  }

  // Время Режима №3
  if (key == "AM3T") {
    String tmp(padNum(AM3_hour,2)); tmp += ' '; tmp += padNum(AM3_minute,2);
    if (shrt) {
      return tmp;
    }
    String str("AM3T:");
    str += tmp;
    return str;
  }

  // Действие Режима №3
  if (key == "AM3A") {
    if (shrt) {
      return String(AM3_effect_id);
    }
    String str("AM3A:");
    str += AM3_effect_id;
    return str;
  }

  // Время Режима №4
  if (key == "AM4T") {
    String tmp(padNum(AM4_hour,2)); tmp += ' '; tmp += padNum(AM4_minute,2);
    if (shrt) {
      return tmp;
    }
    String str("AM4T:");
    str += tmp;
    return str;
  }

  // Действие Режима №4
  if (key == "AM4A") {
    if (shrt) {
      return String(AM4_effect_id);
    }
    String str("AM4A:");
    str += AM4_effect_id;
    return str;
  }

  // Действие Режима "Рассвет"
  if (key == "AM5A") {
    if (shrt) {
      return String(dawn_effect_id);
    }
    String str("AM5A:");
    str += dawn_effect_id;
    return str;
  }

  // Действие Режима "Закат"
  if (key == "AM6A") {
    if (shrt) {
      return String(dusk_effect_id);
    }
    String str("AM6A:");
    str += dusk_effect_id;
    return str;
  }

  // Наличие резервной копии EEPROM
  if (key == "EE") {
    if (shrt) {
      return String(eeprom_backup);
    }
    String str("EE:");
    str += eeprom_backup;
    return str; 
  }

  // Имя файла резервной копии EEPROM
  if (key == "BF") {
    String fileName(F("eeprom_0x")); fileName += IntToHex(EEPROM_OK, 2); fileName += ".hex";  
    if (shrt) {
      return fileName;
    }
    String str("BF:");
    str += fileName;
    return str; 
  }

  // Доступность внутренней файловой системы
  if (key == "FS") {
    if (shrt) {
      return String(spiffs_ok);
    }
    String str("FS:");
    str += spiffs_ok;
    return str; 
  }

  // Поддержка функционала SD-карты
  if (key == "SZ") {
    if (shrt) {
      return String(USE_SD == 1);
    }
    String str("SZ:");
    str += (USE_SD == 1);
    return str;
  }

#if (USE_SD == 1)
  // Наличие в системе SD карты (в т.ч и эмуляция в FS)
  if (key == "SX") {
    if (shrt) {
      return String(isSdCardExist);
    }
    String str("SX:");
    str += isSdCardExist;
    return str; 
  }

  // Доступность на SD карте файлов эффектов (в т.ч и эмуляция в FS)
  if (key == "SD") {
    if (shrt) {
      return String(isSdCardReady);
    }
    String str("SD:");
    str += isSdCardReady;
    return str; 
  }

#endif

  // ширина одного сегмента матрицы
  if (key == "M0") {
    String tmp(sWIDTH);
    if (shrt) {
      return tmp;
    }
    String str("M0:");
    str += tmp;
    return str;
  }

  // высота одного сегмента матрицы
  if (key == "M1") {
    String tmp(sHEIGHT);
    if (shrt) {
      return tmp;
    }
    String str("M1:");
    str += tmp;
    return str;
  }

  // тип соединения диодов в сегменте матрицы
  if (key == "M2") {
    String tmp(sMATRIX_TYPE);
    if (shrt) {
      return tmp;
    }
    String str("M2:");
    str += tmp;
    return str;
  }

  // угол подключения диодов в сегменте
  if (key == "M3") {
    String tmp(sCONNECTION_ANGLE);
    if (shrt) {
      return tmp;
    }
    String str("M3:");
    str += tmp;
    return str;
  }

  // направление ленты из угла сегмента
  if (key == "M4") {
    String tmp(sSTRIP_DIRECTION);
    if (shrt) {
      return tmp;
    }
    String str("M4:");
    str += tmp;
    return str;
  }

  // количество сегментов в ширину составной матрицы
  if (key == "M5") {
    String tmp(mWIDTH);
    if (shrt) {
      return tmp;
    }
    String str("M5:");
    str += tmp;
    return str;
  }

  // количество сегментов в высоту составной матрицы
  if (key == "M6") {
    String tmp(mHEIGHT);
    if (shrt) {
      return tmp;
    }
    String str("M6:");
    str += tmp;
    return str;
  }

  // соединение сегментов составной матрицы
  if (key == "M7") {
    String tmp(mTYPE);
    if (shrt) {
      return tmp;
    }
    String str("M7:");
    str += tmp;
    return str;
  }

  // угол 1-го сегмента составной матрицы
  if (key == "M8") {
    String tmp(mANGLE);
    if (shrt) {
      return tmp;
    }
    String str("M8:");
    str += tmp;
    return str;
  }

  // направление следующих сегментов составной матрицы из угла
  if (key == "M9") {
    String tmp(mDIRECTION);
    if (shrt) {
      return tmp;
    }
    String str("M9:");
    str += tmp;
    return str;
  }

  // Cписок файлов карт индексов, разделенный запятыми, ограничители [] обязательны
  if (key == "M10") {   
    String tmp; 
    for (uint8_t i=0; i < mapListLen; i++) {
      tmp += "," + mapFiles[i];
    }
    if (tmp.length() > 0) tmp = tmp.substring(1);
    if (shrt) {
      return tmp;
    }
    String str("M10:");
    str += tmp;
    return str; 
  }

  // Индекс текущей используемой карты индексов (элемента в массиве mapFiles)
  if (key == "M11") {
    int8_t idx = -1;
    String curr(pWIDTH); curr += 'x'; curr += pHEIGHT;
    for (uint8_t i=0; i<mapListLen; i++) {
      if (mapFiles[i] == curr) {
        idx = i;
        break;
      }
    }
    String tmp(idx);
    if (shrt) {
      return tmp;
    }
    String str("M11:");
    str += tmp;
    return str;
  }

  // Поддержка протокола E1.31
  if (key == "E0") {
    if (shrt) {
      return String(USE_E131 == 1);
    }
    String str("E0:");
    str += (USE_E131 == 1);
    return str;
  }

  #if (USE_E131 == 1)
  
  // режим работы 0 - STANDALONE, 1 - MASTER, 2 - SLAVE
  if (key == "E1") {
    String tmp(workMode);
    if (shrt) {
      return tmp;
    }
    String str("E1:");
    str += tmp;
    return str;
  }
  
  // тип данных работы 0 - PHYSIC, 1 - LOGIC, 2 - COMMAND
  if (key == "E2") {
    String tmp(syncMode);
    if (shrt) {
      return tmp;
    }
    String str("E2:");
    str += tmp;
    return str;
  }
  
  // Группа синхронизации 0..9
  if (key == "E3") {
    String tmp(syncGroup);
    if (shrt) {
      return tmp;
    }
    String str("E3:");
    str += tmp;
    return str;
  }

  // Идет вещание в режиме мастера или пакуты от мастера поступают - 1; Режим STANDALONE или вещение прервалось - 0
  if (key == "E4") {
    if (shrt) {
      return String(streaming);
    }
    String str("E4:");
    str += streaming;
    return str;
  }

  // Позиция X окна трансляции мастера
  if (key == "EMX") {
    String tmp(masterX);
    if (shrt) {
      return tmp;
    }
    String str("EMX:");
    str +=  tmp;
    return str;
  }
  
  // Позиция Y окна трансляции мастера
  if (key == "EMY") {
    String tmp(masterY);
    if (shrt) {
      return tmp;
    }
    String str("EMY:");
    str += tmp;
    return str;
  }
  
  // Позиция X локального окна окна приема трансляции с мастера
  if (key == "ELX") {
    String tmp(localX);
    if (shrt) {
      return tmp;
    }
    String str("ELX:");
    str += tmp;
    return str;
  }
  
  // Позиция Y локального окна окна приема трансляции с мастера
  if (key == "ELY") {
    String tmp(localY);
    if (shrt) {
      return tmp;
    }
    String str("ELY:");
    str += tmp;
    return str;
  }
  
  // Ширина локального окна окна приема трансляции с мастера
  if (key == "ELW") {
    String tmp(localW);
    if (shrt) {
      return tmp;
    }
    String str("ELW:");
    str += tmp;
    return str;
  }
  
  // Высота локального окна окна приема трансляции с мастера
  if (key == "ELH") {
    String tmp(localH);
    if (shrt) {
      return tmp;
    }
    String str("ELH:");
    str += tmp;
    return str;
  }
  
  #endif

  // Поддержка управления питанием матрицы
  if (key == "PZ0") {
    if (shrt) {
      return String(USE_POWER == 1);
    }
    String str("PZ0:");
    str += (USE_POWER == 1);
    return str;
  }

  // Поддержка управления питанием линии будильника
  if (key == "PZ1") {
    if (shrt) {
      return String(USE_ALARM == 1);
    }
    String str("PZ1:");
    str += (USE_ALARM == 1);
    return str;
  }

    // Поддержка управления питанием дополнительной линии управления
  if (key == "PZ2") {
    if (shrt) {
      return String(USE_AUX == 1);
    }
    String str("PZ2:");
    str += (USE_AUX == 1);
    return str;
  }

  // Тип микроконтроллера
  if (key == "MC") {
    String mcType(MCUType());
    if (shrt) {
      return mcType;
    }
    String str("MC:");
    str += mcType;
    return str;
  }

  // Наличие кнопки
  if (key == "UB") {
    if (shrt) {
      return String(USE_BUTTON == 1);
    }
    String str("UB:");
    str += (USE_BUTTON == 1);
    return str;
  }

  // 2306:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка; С - цветовой порядок
  if (key == "2306") {
    uint16_t led_start = getLedLineStartIndex(1);
    uint16_t led_count = getLedLineLength(1);
    if (led_start + led_count > NUM_LEDS) {
      led_count = NUM_LEDS - led_start;
    }
    int8_t led_rgb = getLedLineRGB(1);
    String tmp(getLedLineUsage(1) ? 1 : 0); tmp += ' '; tmp += getLedLinePin(1); tmp += ' '; tmp += led_start; tmp += ' '; tmp += led_count; tmp += ' '; tmp += led_rgb;
    if (shrt) {
      return tmp;
    }
    String str("2306:");
    str += tmp;
    return str;
  }

  // 2307:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка; С - цветовой порядок
  if (key == "2307") {
    uint16_t led_start = getLedLineStartIndex(2);
    uint16_t led_count = getLedLineLength(2);
    if (led_start + led_count > NUM_LEDS) {
      led_count = NUM_LEDS - led_start;
    }
    int8_t led_rgb = getLedLineRGB(2);
    String tmp(getLedLineUsage(2) ? 1 : 0); tmp += ' '; tmp += getLedLinePin(2); tmp += ' '; tmp += led_start; tmp += ' '; tmp += led_count; tmp += ' '; tmp += led_rgb;
    if (shrt) {
      return tmp;
    }
    String str("2307:");
    str += tmp;
    return str;
  }

  // 2308:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка; С - цветовой порядок
  if (key == "2308") {
    uint16_t led_start = getLedLineStartIndex(3);
    uint16_t led_count = getLedLineLength(3);
    if (led_start + led_count > NUM_LEDS) {
      led_count = NUM_LEDS - led_start;
    }
    int8_t led_rgb = getLedLineRGB(3);
    String tmp(getLedLineUsage(3) ? 1 : 0); tmp += ' '; tmp += getLedLinePin(3); tmp += ' '; tmp += led_start; tmp += ' '; tmp += led_count; tmp += ' '; tmp += led_rgb;
    if (shrt) {
      return tmp;
    }
    String str("2308:");
    str += tmp;
    return str;
  }

  // 2309:U P S L Подключние матрицы линии 1; U: 1/0 - вкл/выкл; P - GPIO пин; S - старт индекс; L - длина участка; С - цветовой порядок
  if (key == "2309") {
    uint16_t led_start = getLedLineStartIndex(4);
    uint16_t led_count = getLedLineLength(4);
    if (led_start + led_count > NUM_LEDS) {
      led_count = NUM_LEDS - led_start;
    }
    int8_t led_rgb = getLedLineRGB(4);
    String tmp(getLedLineUsage(4) ? 1 : 0); tmp += ' '; tmp += getLedLinePin(4); tmp += ' '; tmp += led_start; tmp += ' '; tmp += led_count; tmp += ' '; tmp += led_rgb;
    if (shrt) {
      return tmp;
    }
    String str("2309:");
    str += tmp;
    return str;
  }

  // 2310:X       тип матрицы vDEVICE_TYPE: 0 - труба; 1 - панель
  if (key == "2310") {
    if (shrt) {
      return String(vDEVICE_TYPE);
    }
    String str("2310:");
    str += vDEVICE_TYPE;
    return str;
  }

  // 2311:X Y    X - GPIO пин кнопки, Y - тип кнопки vBUTTON_TYPE: 0 - сенсорная; 1 - тактовая
  if (key == "2311") {
    int8_t pin = USE_BUTTON == 1 ? getButtonPin() : -1; 
    String tmp(pin); tmp += ' '; tmp += vBUTTON_TYPE;
    if (shrt) {
      return String(tmp);
    }
    String str("2311:");
    str += tmp;
    return str;
  }

  // 2312:X Y    X - GPIO пин управления питанием; Y - уровень управления питанием - 0 - LOW; 1 - HIGH
  if (key == "2312") {
    int8_t pin = USE_POWER == 1 ? getPowerPin() : -1; 
    String tmp(pin); tmp += ' '; tmp += getPowerActiveLevel();
    if (shrt) {
      return String(tmp);
    }
    String str("2312:");
    str += tmp;
    return str;
  }

  // 2313:X Y    X - vWAIT_PLAY_FINISHED, Y - vREPEAT_PLAY
  if (key == "2313") {
    String tmp(getWaitPlayFinished()); tmp += ' '; tmp += getRepeatPlay();
    if (shrt) {
      return String(tmp);
    }
    String str("2313:");
    str += tmp;
    return str;
  }

  // 2314:X      vDEBUG_SERIAL
  if (key == "2314") {
    if (shrt) {
      return String(vDEBUG_SERIAL);
    }
    String str("2314:");
    str += vDEBUG_SERIAL;
    return str;
  }

  // 2315:X Y    X - GPIO пин TX на DFPlayer; Y - GPIO пин RX на DFPlayer
  if (key == "2315") {
    int8_t pin1 = USE_MP3 == 1 ? getDFPlayerSTXPin() : -1; 
    int8_t pin2 = USE_MP3 == 1 ? getDFPlayerSRXPin() : -1; 
    String tmp(pin1); tmp += ' '; tmp += pin2;
    if (shrt) {
      return String(tmp);
    }
    String str("2315:");
    str += tmp;
    return str;
  }

  // 2316:X Y    X - GPIO пин DIO на TN1637; Y - GPIO пин CLK на TN1637
  if (key == "2316") {
    int8_t pin1 = USE_TM1637 == 1 ? getTM1637DIOPin() : -1; 
    int8_t pin2 = USE_TM1637 == 1 ? getTM1637CLKPin() : -1; 
    String tmp(pin1); tmp += ' '; tmp += pin2;
    if (shrt) {
      return String(tmp);
    }
    String str("2316:");
    str += tmp;
    return str;
  }

  // 2317:X Y    X - GPIO пин управления питанием линии будильника; Y - уровень управления питанием линии будильника - 0 - LOW; 1 - HIGH
  if (key == "2317") {
    int8_t pin = USE_ALARM == 1 ? getAlarmPin() : -1; 
    String tmp(pin); tmp += ' '; tmp += getAlarmActiveLevel();
    if (shrt) {
      return String(tmp);
    }
    String str("2317:");
    str += tmp;
    return str;
  }
    
  // 2318:X Y    X - GPIO пин управления дополнительной линии питания; Y - уровень управления дополнительной линии питания - 0 - LOW; 1 - HIGH
  if (key == "2318") {
    int8_t pin = USE_AUX == 1 ? getAuxPin() : -1; 
    String tmp(pin); tmp += ' '; tmp += getAuxActiveLevel();
    if (shrt) {
      return String(tmp);
    }
    String str("2318:");
    str += tmp;
    return str;
  }

  if (key == "DBG") {
    bool debug = debug_hours >= 0 && debug_mins >= 0;
    String s; s.reserve(3);
    s += (debug ? "1" : "0");
    s += (debug_frame ? "1" : "0");
    s += (debug_cross ? "1" : "0");
    if (shrt) {
      return s; 
    }
    String str("DBG:");
    str += s;
    return str;
  }  

  // Запрошенный ключ не найден - вернуть пустую строку
  return "";
}

// Первый параметр эффекта thisMode для отправки на телефон параметра "SS:"
String getParamForMode(uint8_t mode) {
 // Эффекты не имеющие настройки "Вариант" (параметр #1) отправляют значение "Х" - программа делает ползунок настройки недоступным 
 String str; 
 switch (mode) {
   case MC_DAWN_ALARM:
   case MC_COLORS:
   case MC_SWIRL:
   case MC_FLICKER:
   case MC_PACIFICA:
   case MC_SHADOWS:
   case MC_MAZE:
   case MC_SNAKE:
   case MC_TETRIS:
   case MC_ARKANOID:
   case MC_PALETTE:
   case MC_ANALYZER:
   case MC_PRIZMATA:
   case MC_MUNCH:
   case MC_ARROWS:
   case MC_WEATHER:
   case MC_FIRE2:
   case MC_SDCARD:
   case MC_IMAGE:
     str = "X";
     break;
   case MC_CLOCK:
     str = isNightClock 
       ? map(nightClockColor, 0,6, 1,255)
       : effectScaleParam[mode];
     break;
   default:
     str = effectScaleParam[mode];
     break;
 }
 return str;   
}

// Название первого параметра эффекта для отображения в WEB-интерфейсе
String getParamNameForMode(uint8_t mode) {
 // Эффекты не имеющие настройки "Вариант" (параметр #1) отправляют значение "Х" - программа делает ползунок настройки недоступным 
 String str; 
 switch (mode) {
   case MC_DAWN_ALARM:
   case MC_COLORS:
   case MC_SWIRL:
   case MC_FLICKER:
   case MC_PACIFICA:
   case MC_SHADOWS:
   case MC_MAZE:
   case MC_SNAKE:
   case MC_TETRIS:
   case MC_ARKANOID:
   case MC_PALETTE:
   case MC_ANALYZER:
   case MC_PRIZMATA:
   case MC_MUNCH:
   case MC_ARROWS:
   case MC_WEATHER:
   case MC_FIRE2:
   case MC_SDCARD:
   case MC_IMAGE:
     str = "X";
     break;
   case MC_FILL_COLOR:
     str = F("Насыщенность");
     break;
   case MC_PRIZMATA2:
   case MC_SHADOWS2:
     str = F("Изменчивость");
     break;
   case MC_FIREWORKS:
     str = F("Затухание");
     break;
   case MC_CLOCK:
     str = F("Цвет часов");
     break;
   default:
     str = F("Вариант");
     break;
 }
 return str;   
}

// Второй параметр эффекта thisMode для отправки на телефон параметра "SQ:"
String getParam2ForMode(uint8_t mode) {
 // Эффекты не имеющие настройки вариации (параметр #2) отправляют значение "Х" - программа делает ползунок настройки недоступным 
 String str; 
 switch (mode) {
   case MC_RAINBOW:
     // Эффект "Радуга" имеет несколько вариантов - список выбора варианта отображения
     // Дополнительный параметр представлен в приложении списком выбора
     //           Маркер типа - список выбора         0,1,2,3,4               0               1                   2                     3                   4
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,Вертикальная радуга,Горизонтальная радуга,Диагональная радуга,Вращающаяся радуга");
     break;
   case MC_ARROWS:
     // Эффект "Стрелки" имеет несколько вариантов - список выбора варианта отображения
     // Дополнительный параметр представлен в приложении списком выбора
     //           Маркер типа - список выбора         0,1,2,3,4               0               1       2       3       4          5
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,1-центр,2-центр,4-центр,2-смещение,4-смещение");
     break;
   case MC_PATTERNS:
     // Эффект "Узоры" имеет несколько вариантов - список выбора варианта отображения
     // Дополнительный параметр представлен в приложении списком выбора
     //           Маркер типа - список выбора         0,1,2,3,4               0               1    2, 3, ...
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,"); str += FPSTR(LANG_PATTERNS_LIST);
     break;
   case MC_RUBIK:
     // Эффект "Кубик рубика" имеет несколько вариантов - список выбора варианта отображения
     // Дополнительный параметр представлен в приложении списком выбора
     //           Маркер типа - список выбора         0,1,2,3,4               0               1      2      3       4
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,На шаг,Полоса,Спираль,Пятнашки");
     break;
   #if (USE_SD == 1)     
   case MC_SDCARD:
     // Эффект "SD-card" имеет несколько вариантов - список выбора файла эффекта
     // Дополнительный параметр представлен в приложении списком выбора
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,Последовательно");
     for (uint8_t i=0; i < countFiles; i++) {
       str += ","; str += nameFiles[i];
     }     
     break;
   #endif
   #if (USE_ANIMATION == 1)  
   case MC_IMAGE:
     // Эффект "Анимация" имеет несколько вариантов - список выбора отображаемой картинки
     // Дополнительный параметр представлен в приложении списком выбора
     //           Маркер типа - список выбора         0,1,2,3,4               0                     1, 2, ...
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,"); str += animations_list;
     break;
   #endif  
   case MC_PAINTBALL:
   case MC_SWIRL:
   case MC_CYCLON:
     // Эффект "Пейнтбол" имеет параметр - использовать сегменты.
     // Эффект "Водоворот" имеет параметр - использовать сегменты.
     // Эффект "Циклон" имеет параметр - использовать сегменты.
     // Выкл - эффект растягивается на всю ширину матрицы
     // Вкл  - эффект квадратный. Если места достаточно для размещения более чем одного квадрата (например 32x16) - отображается два квадратных сегмента
     //        Если ширина матрицы не кратна ширине сегмента - между сегментами добавляются промежутки
     // Дополнительный параметр представлен в приложении чекбоксов - 0-выкл, 1-вкл
     //       Маркер типа - чекбокс                             false true            Текст чекбокса
     str = String(sFC); str += (effectScaleParam2[mode] == 0 ? "0" : "1"); str += F(">Сегменты"); // "C>1>Сегменты"
     break;
   case MC_STARS:
     //           Маркер типа - список выбора         0,1,2,3,4               0               1         2      3      4
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,Без лучей,Лучи '+',Лучи 'X',Лучи '+' и 'X'");
     break;
   case MC_STARS2:
     //           Маркер типа - список выбора         0,1,2,3,4               0         1         2         3
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Вариант 1,Вариант 2,Вариант 3,Вариант 4");
     break;
   case MC_TRAFFIC:
     //           Маркер типа - список выбора         0,1,2,3,4               0               1       2
     str = String(sFL); str += effectScaleParam2[mode]; str += F(">Случайный выбор,Цветной,Монохром");
     break;
   case MC_WORMS:
   case MC_FIREWORKS:
     // Слайдер
     str += effectScaleParam2[mode];
     break;
   default:
     str = 'X';
     break;  
 }
 return str;   
}

// Название второго параметра эффекта для отображения в Web-интерфейсе
String getParam2NameForMode(uint8_t mode) {
 // Эффекты не имеющие настройки вариации (параметр #2) отправляют значение "Х" - программа делает ползунок настройки недоступным 
 String str; 
 switch (mode) {
   case MC_RAINBOW:
   case MC_ARROWS:
   case MC_PATTERNS:
   case MC_RUBIK:
   case MC_STARS:
   case MC_STARS2:
   case MC_TRAFFIC:
     str = F("Вариант");
     break;
   case MC_WORMS:
     str = F("Хвост");
     break;
   #if (USE_SD == 1)
   case MC_SDCARD:
     str = F("Ролик");
     break;
     #endif
   case MC_IMAGE:
     str = F("Анимация");
     break;
   case MC_PAINTBALL:
   case MC_SWIRL:
   case MC_CYCLON:
     str = F("Сегменты");
     break;
   case MC_FIREWORKS:
     str = F("Плотность");
     break;
   default:
     str = 'X';
     break;  
 }
 return str;   
}

void setSpecialMode(int8_t spc_mode) {
        
  loadingFlag = true;

  #if (USE_E131 == 1)
    commandSetSpecialMode(spc_mode);
  #endif      

  String str;
  int8_t tmp_eff = -1;
  
  specialClock = getClockOverlayEnabled() && ((CLOCK_ORIENT == 0 && allowHorizontal) || (CLOCK_ORIENT == 1 && allowVertical)); 

  switch(spc_mode) {
    case 0:  // Черный экран (выкл);
      tmp_eff = MC_FILL_COLOR;
      set_globalColor(0x000000);
      set_specialBrightness(0);
      specialClock = false;
      set_isTurnedOff(true); // setter
      break;
      
    case 1:  // Белый экран (освещение);
      tmp_eff = MC_FILL_COLOR;
      set_globalColor(0xffffff);
      set_specialBrightness(255);
      break;
      
    case 2:  // Лампа указанного цвета;
      tmp_eff = MC_FILL_COLOR;
      set_globalColor(getGlobalColor());
      break;
      
    case 3:  // Ночные часы;  
      tmp_eff = MC_CLOCK;
      // Сохранить какой эффект до до включения ночных часов, его яркость, состояние автосмены режима
      saveSpecialMode = specialMode;
      saveSpecialModeId = specialModeId;
      saveMode = thisMode;
      saveManualMode = manualMode;
      // Подготовка к включению ночных часов
      specialClock = false;
      set_isNightClock(true); // setter
      set_specialBrightness(nightClockBrightness);
      FastLED.clear();
      break;
      
    case 4:  // Часы (отдельным эффектом, а не оверлеем);  
      tmp_eff = MC_CLOCK;
      set_globalColor(getGlobalClockColor());
      specialClock = true;
      set_specialBrightness(deviceBrightness);
      break;
      
    case 5:  // Белый экран (ночник);
      tmp_eff = MC_FILL_COLOR;
      set_globalColor(0xffffff);
      set_specialBrightness(25);
      break;
      
    case 6:  // Выключение ночных часов - восстановить эффект который был до включения и состояние "Автосмена режима" и яркость
      if (isNightClock) {
        set_isTurnedOff(false); 
        set_isNightClock(false);        
        if (saveSpecialMode && saveSpecialModeId != 0 && saveSpecialModeId != 3 && saveSpecialModeId < SPECIAL_EFFECTS_START) {
          setSpecialMode(saveSpecialModeId);
        } else {
          saveMode = getCurrentManualMode();
          if (saveMode >= MAX_EFFECT) {
            setRandomMode(); 
          } else {
            if (saveMode == MC_FILL_COLOR && globalColor == 0) set_globalColor(0xFFFFFF);
            setEffect(saveMode);
          }
          setManualModeTo(saveManualMode);
          FastLEDsetBrightness(getMaxBrightness());
        }        
        // Восстановили режим который был до ночных часов, дальше ничего делать не нужно       
        return;
      }
      break;
    default:
      specialModeId = -1;
      break;
  }

  set_isTurnedOff(spc_mode == 0); 
  set_isNightClock(spc_mode == 3);
  
  if (tmp_eff >= 0) {    
    // Дальнейшее отображение изображения эффекта будет выполняться стандартной процедурой customRoutine()
    putCurrentManualMode(thisMode);
    set_thisMode(tmp_eff);
    setTimersForMode(thisMode);
    setIdleTimer();             
    FastLEDsetBrightness(specialBrightness);
  }  

  setManualModeTo(true);
  set_CurrentSpecialMode(spc_mode);
}

void resetModes() {
  // Отключение спец-режима перед включением других
  set_isTurnedOff(false);   // setter
  set_isNightClock(false);  // setter
  set_CurrentSpecialMode(-1);
  loadingFlag = false;
  wifi_print_ip = false;
  wifi_print_ip_text = false;
  wifi_print_version = false;
  #if (USE_SD == 1)  
    play_file_finished = false;
  #endif
}

void resetModesExt() {
  resetModes();
  gamePaused = false;
  set_isAlarming(false); 
  set_isPlayAlarmSound(false);
  set_manualMode(false);
  autoplayTimer = millis();
  set_autoplayTime(getAutoplayTime());  
  #if (USE_SD == 1)  
    play_file_finished = false;
  #endif
}

void setEffect(uint8_t eff) {

  resetModes();
  if (eff >= MAX_EFFECT && eff < SPECIAL_EFFECTS_START) {
    return;
  }
  
  if (eff == MC_TEXT)
    loadingTextFlag = true;
  else
    loadingFlag = true;

  set_thisMode(eff);

  setTimersForMode(thisMode);  
  setIdleTimer();

  set_CurrentSpecialMode(-1);

  if (!manualMode){
    autoplayTimer = millis();
  }

  gameDemo = true;
  gamePaused = false;

  char eff_char = String(IDX_LINE)[eff];
  int8_t eff_idx = effect_order.indexOf(eff_char);
  if (eff_idx >= 0) {
    effect_order_idx = eff_idx;
  }

  if (effect_name.length() == 0) {
    effect_name = getEffectName(eff);
  }
  
  if (thisMode != MC_DAWN_ALARM) {
    FastLEDsetBrightness(deviceBrightness);      
  }
}

void showCurrentIP() {
  saveSpecialMode = specialMode;
  saveSpecialModeId = specialModeId;
  saveMode = thisMode;
  setEffect(MC_TEXT);
  textHasMultiColor = false;
  wifi_print_version = false;
  wifi_print_ip = wifi_connected;  
  wifi_print_ip_text = true;
  wifi_print_idx = 0; 
  wifi_current_ip = wifi_connected ? WiFi.localIP().toString() : F("Нет подключения к сети WiFi");
  loadingTextFlag = true;
  fullTextFlag = false;
}

void showCurrentVersion() {
  saveSpecialMode = specialMode;
  saveSpecialModeId = specialModeId;
  saveMode = thisMode;
  setEffect(MC_TEXT);
  textHasMultiColor = false;
  wifi_print_version = true;
  wifi_print_ip = false;  
  wifi_print_ip_text = false;
  loadingTextFlag = true;
  fullTextFlag = false;
}

void setRandomMode() {
  
  int8_t newMode;
  String eff_list = String(IDX_LINE);
  
  #if (USE_SD == 1)  
  // На SD карте содержится более 40 эффектов плюсом к 40 эффектам, заданных в прошивке
  // Когда следующий эффект выбирается случайным образом, вероятность что выпадет SD-карта достаточно мала.
  // Искусственным образом увеличиваем вероятность эффекта с SD-карты
  char eff_sd = eff_list[MC_SDCARD];
  int8_t idx = effect_order.indexOf(eff_sd);  
  if (idx >= 0 && isSdCardReady && (random16(0, 200) % 10 == 0)) {
    effect_order_idx = idx;
    newMode = MC_SDCARD;
    // effectScaleParam2[MC_SDCARD]: 0 - случайный файл; 1 - последовательный перебор; 2 - привести к индексу в массиве файлов SD-карты
    updateSdCardFileIndex();
    setEffect(newMode);
    return;
  }   
  #endif

  // выборку делать только из эффектов, разрешенных к использованию
  char eff = '0';
  if (effect_order.length() == 0) {
    effect_order = "0";
    effect_order_idx = 0;
  }
  if (effect_order.length() == 1) {
    eff = effect_order[0];
    effect_order_idx = 0;
  } else
  if (effect_order.length() == 2) {
    effect_order_idx = effect_order_idx == 0 ? 1 : 0;
    eff = effect_order[0];
  } else {
    effect_order_idx = random8(effect_order.length());
    eff = effect_order[effect_order_idx];
  }
  
  newMode = eff_list.indexOf(eff); 
  if (newMode < 0) newMode = 0; else
  if (newMode >= MAX_EFFECT) newMode = MAX_EFFECT - 1;

  #if (USE_SD == 1)  
  if (newMode == MC_SDCARD && isSdCardReady) {
    updateSdCardFileIndex();
  }   
  #endif
  
  setEffect(newMode);
}

#if (USE_SD == 1)  
void updateSdCardFileIndex() {
  if (effectScaleParam2[MC_SDCARD] == 0) {
    sf_file_idx = random16(0,countFiles);
  } else if (effectScaleParam2[MC_SDCARD] == 1) {
    sf_file_idx++;
    if (sf_file_idx >= countFiles) sf_file_idx = 0;
  } else {
    sf_file_idx = effectScaleParam2[MC_SDCARD] - 2;
  }
}
#endif

void setManualModeTo(bool isManual) {
  set_manualMode(isManual);
  setIdleTimer();             
  autoplayTimer = millis();
}

void sendImageLine(uint8_t typ, uint8_t idx) {
  // src   - откуда пришел запрос
  // typ=0 - отправить строку с номером idx
  // typ=1 - отправить колонку с номером idx
  // idx   - номер строки / колонки; Точка (0,0)  на матрице - левый нижний угол, на телефоне / web - левый верхний угол
  
  // Выполняется отправка изображения с матрицы в приложение на телефоне?
  bool sendByRow = typ == 0;
  String imageLine, sHex, sCmd;

  if (thisMode == MC_DRAW) {
    if (sendByRow) {
      // Отправка по строкам
      sCmd = "IR";
      imageLine = String(idx) + '|';
      for (uint8_t i=0; i<pWIDTH; i++) {
        yield();
        imageLine += IntToHex(gammaCorrectionBack(getPixColorXY(i, pHEIGHT - idx - 1))) + ' ';
      }
    } else {
      // Отправка по колонкам
      sCmd = "IC";
      imageLine = String(idx) + '|';
      for (uint8_t i = 0; i < pHEIGHT; i++) {
        yield();
        imageLine += IntToHex(gammaCorrectionBack(getPixColorXY(idx, pHEIGHT - i - 1))) + ' ';
      }
    }
    imageLine.trim();
    imageLine.replace(" ", ",");    
    SendWebKey(sCmd, imageLine);
  }  
}

void turnOnOff() {
  if (isTurnedOff) {
    turnOn();
  } else {
    turnOff();
  }
}

void turnOff() {
  if (!isTurnedOff) {
    // Выключить панель, запомнив текущий режим
    saveSpecialMode = specialMode;
    saveSpecialModeId = specialModeId;
    saveMode = thisMode;
    bool mm = manualMode;
    #if (USE_E131 == 1)
    // Запомнить текущий используемый уровень яркости.
    uint8_t bright = specialModeId < 0 ? deviceBrightness : specialBrightness;
    #endif
    // Выключить панель - черный экран
    setSpecialMode(0);
    putCurrentManualMode(saveMode);
    putAutoplay(mm);

    FastLED.clear();  
    FastLEDshow();
    
    // Если устройство в режиме приемника и идет прием пакетов от мастера - вызов выше setSpecialMode(0) - выключение устройства
    // Уберет яркость в 0, хотя трансляция с мастера продолжает идти. Следующее правильное значение яркости придет
    // только со сменой эффекта или изменение контролов яркости мастера - когда мастер отсылает текущее значение яркости
    // Чтобы избежать черного экрана после выключения устройства-приемника - восстанавливаем сохраненный выше текущий уровень яркости.
    #if (USE_E131 == 1)
      if (workMode == SLAVE && streaming) {
        // Кадр выведен на экран - очистить матрицу перед вормированием нового кадра
        // Это нужно для случая -  кадр автономной работы по размерам больше кадра получаемого с мастера
        // Если не очищать матрицу - при включении вещания мы получим изображение с мастера, но в остальной области останется
        // изображение кадра с автономной работы           
        FastLED.setBrightness(bright);
      }
    #endif
    saveSettings();
  }
}

void turnOn() {
  if (isTurnedOff) {
    DEBUGLN(F("Режим: Включено"));
    // Если выключен - включить панель, восстановив эффект на котором панель была выключена
    if (saveSpecialMode && saveSpecialModeId != 0 && saveSpecialModeId < SPECIAL_EFFECTS_START) {
      setSpecialMode(saveSpecialModeId);
    } else {
      saveMode = getCurrentManualMode();
      if (saveMode >= MAX_EFFECT) {
        setRandomMode(); 
      } else {
        if (saveMode == MC_FILL_COLOR && globalColor == 0) set_globalColor(0xFFFFFF);
        setManualModeTo(getAutoplay());
        setEffect(saveMode);
      }
      FastLEDsetBrightness(getMaxBrightness());
    }
  }
  prevShowTimer = 0;
}

void setImmediateText(const String& str) {
  // текст бегущей строки для немедленного отображения без сохранения 
  currentText = str;
  showTextNow = false;
  ignoreTextOverlaySettingforEffect = true;
  fullTextFlag = true;
  textLastTime = 0;
  #if (USE_E131 == 1)
    commandSetTextSpeed(textScrollSpeed);
    commandSetImmediateText(str);
  #endif  
}

// Установка времени вручную, аргументы переданы из Web-UI - команда '$19 8 YYYY MM DD HH MM;'
// Параметры передаются в виде отдельных int в массиве 
void setCurrentTime(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t dd, uint8_t nn, uint16_t yy) {

  struct tm tmStruct;
  memset(&tmStruct, 0, sizeof(tmStruct));

  tmStruct.tm_hour = hh;
  tmStruct.tm_min = mm;
  tmStruct.tm_sec = ss;
  tmStruct.tm_mday = dd;
  tmStruct.tm_mon = nn - 1;
  tmStruct.tm_year = yy - 1900;

  time_t time = mktime(&tmStruct);
  timeval tv = { time, 0 };
  settimeofday(&tv, NULL);

  init_time = true;
  
  debug_hours = -1;  debug_mins = -1; debug_temperature = -99;
  rescanTextEvents();
  calculateDawnTime();      
  
  #if (USE_E131 == 1)
  if (workMode == MASTER) {
    commandSetCurrentTime(hh,mm,ss,dd,nn,yy);
  }
  #endif    
}
