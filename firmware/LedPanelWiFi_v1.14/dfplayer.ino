// SD карточка в MP3 плеере (DFPlayer) содержит в корне три папки - "01","02" и "03"
// Папка "01" содержит MP3 файлы звуков, проигрываемых при наступлении события будильника
// Папка "02" содержит MP3 файлы звуков, проигрываемых при наступлении события рассвета
// Папка "03" содержит MP3 файлы звуков, проигрываемых в макросе {A} бегущей строки
// DFPlayer не имеет возможности считывать имена файлов, только возможность получить количество файлов в папке.
// Команда на воспроизведение звука означает - играть файл с индексом (номером) N из папки M
// Номера файлов определяются таблицей размещения файлов SD-карты (FAT) и формируются в порядке очереди записи файлов на чистую флэшку
// Так, файл записанный в папку первым получает номер 1, второй - 2 и так далее и никак не зависит от имен файлов
// Данные массивы содержат отображаемые в приложении имена звуков в порядке, соответствующем нумерации записанных в папки файлов.
//
// Для формирования правильного порядка файлов звуков переименуйте их на компьютере во временной папке так, чтобы они 
// формировали нужный порядок, например - задайте им просто числовые имена вроде 001.mp3, 002.mp3 и так далее или 
// задайте числовой префикс существующему имени, например 01_birds.mpr, 02_thunder.mp3 и так далее
// Файлы в папке должны быть отсортированы в порядке имен.
// Далее создайте на чистой microSD-карте папку в которую будут помещены звуковые файлы и скопируйте их в 
// упорядоченном списке.

void InitializeDfPlayer1() {
#if (USE_MP3 == 1)
  DEBUGLN(F("\nИнициализация MP3-плеера..."));
  isDfPlayerOk = true;
  #if (CONFIG_IDF_TARGET_ESP32)
    // ESP32-WROOM-32
    // Используются аппаратный UART2 RX2/TX2 - GPIO 16/17 - mp3Serial это аппаратный Serial2
    mp3Serial.begin(9600);
  #else  
    // ESP8266, ESP32-S2/S3/C3
    // Используются назначенные пины - mp3Serial это программный SoftwareSerial для ESP8266 или аппаратный с назначением пинов на ESP32-S2/S3/C3
    int8_t srx_pin = getDFPlayerSRXPin();
    int8_t stx_pin = getDFPlayerSTXPin();
    if (srx_pin >= 0 && stx_pin >= 0) {
      #if defined(ESP8266)
        mp3Serial.begin(9600, SWSERIAL_8N1, srx_pin, stx_pin);    
      #else  
        mp3Serial.begin(9600, SERIAL_8N1, srx_pin, stx_pin);    
      #endif  
    } else {
      return;
    }
  #endif
  mp3Serial.setTimeout(1000);
  
  dfPlayer.begin(9600);
  delay(250);  
  
  // Попытка СЧИТАТЬ данные с плеера, чтобы если плеера нет - получить ошибку, т.к.
  // при отправке команд в плеер библиотека плеера не ждет ответа (подтверждения выполнения команды)
  // и нет возможности выяснить есть ли плеер на самом деле.
  uint32_t ms1 = millis();    
  dfPlayer.getVolume(); Delay(GUARD_DELAY); 
  uint32_t ms2 = millis();      
  if (ms2 - ms1 > 5000) {
    set_isDfPlayerOk(false);  
  } else {
    dfPlayer.setPlaybackSource(DfMp3_PlaySource_Sd); Delay(GUARD_DELAY);
    dfPlayer.setEq(DfMp3_Eq_Normal);                 Delay(GUARD_DELAY);
    dfPlayer.setVolume(1);                           Delay(GUARD_DELAY);    
  }
#endif  
}

void InitializeDfPlayer2() {    
#if (USE_MP3 == 1)
  int8_t srx_pin = getDFPlayerSRXPin();
  int8_t stx_pin = getDFPlayerSTXPin();
  if (srx_pin >= 0 && stx_pin >= 0) {
    refreshDfPlayerFiles();
    DEBUG(F("Звуков будильника найдено: "));
    DEBUGLN(alarmSoundsCount);
    DEBUG(F("Звуков рассвета найдено: "));
    DEBUGLN(dawnSoundsCount);
    DEBUG(F("Звуков сообщений найдено: "));
    DEBUGLN(noteSoundsCount);
    DEBUGLN();
    set_isDfPlayerOk(alarmSoundsCount + dawnSoundsCount + noteSoundsCount > 0);
  } else {
    set_isDfPlayerOk(false);
  }
#else  
  set_isDfPlayerOk(false);
#endif  
}

void Delay(int16_t duration) {
  delay(duration);
}

#if (USE_MP3 == 1)

void refreshDfPlayerFiles() {
  // Чтение почему-то не всегда работает, иногда возвращает 0 или число от какого-то предыдущего запроса
  // Для того, чтобы наверняка считать значение - первое прочитанное игнорируем, потом читаем несколько раз до повторения.
  DEBUGLN(F("Поиск файлов на карте плеера..."));
  // Папка с файлами для будильника
  int16_t cnt = 0, val = 0, new_val = 0; 
  do {
    // Если запрос выполнялся более 5 секунд - DFPlayer не ответил - он неработоспособен
    // Выполнять чтение дальше смысла нет - это только "завешивает" прошивку.
    mp3Serial.setTimeout(1000);
    uint32_t ms1 = millis();    
    val = dfPlayer.getFolderTrackCount(1);       Delay(GUARD_DELAY);
    uint32_t ms2 = millis();  
    // Плеер не ответил за отведенное время? - Завершить процедуру опроса.      
    if (ms2 - ms1 > 1000) return;
    new_val = dfPlayer.getFolderTrackCount(1);   Delay(GUARD_DELAY);    
    if (val == new_val && val != 0) break;
    cnt++;
  } while ((val == 0 || new_val == 0 || val != new_val) && cnt < 3);
  alarmSoundsCount = val < 0 ? 0 : val;
  
  // Папка с файлами для рассвета
  cnt = 0, val = 0, new_val = 0; 
  do {
    val = dfPlayer.getFolderTrackCount(2);       Delay(GUARD_DELAY);
    new_val = dfPlayer.getFolderTrackCount(2);   Delay(GUARD_DELAY);     
    if (val == new_val && val != 0) break;
    cnt++;
  } while ((val == 0 || new_val == 0 || val != new_val) && cnt < 3);    
  dawnSoundsCount = val < 0 ? 0 : val;

  // Папка с файлами для звуков в бегущей строке
  cnt = 0, val = 0, new_val = 0; 
  do {
    val = dfPlayer.getFolderTrackCount(3);       Delay(GUARD_DELAY);
    new_val = dfPlayer.getFolderTrackCount(3);   Delay(GUARD_DELAY);     
    if (val == new_val && val != 0) break;
    cnt++;
  } while ((val == 0 || new_val == 0 || val != new_val) && cnt < 3);    
  noteSoundsCount = val < 0 ? 0 : val;

  DEBUGLN();    
}

#endif

void PlayAlarmSound() {
  
  if (!isDfPlayerOk) return;

  #if (USE_MP3 == 1)  
  int8_t sound = alarmSound;
  // Звук будильника - случайный?
  if (sound == 0) {
    sound = random8(1, alarmSoundsCount);     // -1 - нет звука; 0 - случайный; 1..alarmSoundsCount - звук
  }
  // Установлен корректный звук?
  if (sound > 0) {
    dfPlayer.stop();                                    Delay(GUARD_DELAY);  // Без этих задержек между вызовами функция dfPlayer прошивка может крашится.
    dfPlayer.setVolume(constrain(maxAlarmVolume,1,30)); Delay(GUARD_DELAY);
    dfPlayer.playFolderTrack(1, sound);                 Delay(GUARD_DELAY);
    dfPlayer.setRepeatPlayCurrentTrack(true);           Delay(GUARD_DELAY);    
    alarmSoundTimer.setInterval(alarmDuration * 60L * 1000L);
    alarmSoundTimer.reset();
    set_isPlayAlarmSound(true);
  } else {
    // Звука будильника нет - плавно выключить звук рассвета
    StopSound(1000);
  }
  #endif
}

void PlayDawnSound() {
  if (!isDfPlayerOk) return;

  #if (USE_MP3 == 1)
  // Звук рассвета отключен?
  int8_t sound = dawnSound;
  // Звук рассвета - случайный?
  if (sound == 0) {
    sound = random8(1, dawnSoundsCount);     // -1 - нет звука; 0 - случайный; 1..alarmSoundsCount - звук
  }
  // Установлен корректный звук?
  if (sound > 0) {
    dfPlayer.stop();                          Delay(GUARD_DELAY); // Без этих задержек между вызовами функция dfPlayer приложение крашится.
    dfPlayer.setVolume(1);                    Delay(GUARD_DELAY);
    dfPlayer.playFolderTrack(2, sound);       Delay(GUARD_DELAY);
    dfPlayer.setRepeatPlayCurrentTrack(true); Delay(GUARD_DELAY);
    // Установить время приращения громкости звука - от 1 до maxAlarmVolume за время продолжительности рассвета realDawnDuration
    fadeSoundDirection = 1;   
    fadeSoundStepCounter = maxAlarmVolume;
    if (fadeSoundStepCounter <= 0) fadeSoundStepCounter = 1;
    fadeSoundTimer.setInterval(realDawnDuration * 60L * 1000L / fadeSoundStepCounter);
    alarmSoundTimer.stopTimer();
  } else {
    StopSound(1000);
  }
  #endif
}

void StopSound([[maybe_unused]] int16_t duration) {

  if (!isDfPlayerOk) return;

  #if (USE_MP3 == 1)
  
  set_isPlayAlarmSound(false);

  if (duration <= 0) {
    dfPlayer.stop();       Delay(GUARD_DELAY);
    dfPlayer.setVolume(0); Delay(GUARD_DELAY);
    return;
  }
  
  // Чтение текущего уровня звука часто глючит и возвращает 0. Тогда использовать maxAlarmVolume
  fadeSoundStepCounter = dfPlayer.getVolume(); Delay(GUARD_DELAY);
  if (fadeSoundStepCounter <= 0) fadeSoundStepCounter = maxAlarmVolume;
  if (fadeSoundStepCounter <= 0) fadeSoundStepCounter = 1;
    
  fadeSoundDirection = -1;     
  fadeSoundTimer.setInterval(duration / fadeSoundStepCounter);  
  
  #endif
}

#if (USE_MP3 == 1)

class Mp3Notify {
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        DEBUG(F("SD-карта, "));
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        DEBUG(F("USB диск, "));
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        DEBUG(F("Flash-память, "));
    }
    DEBUGLN(action);
  }
  
  static void printErrorDetail(uint8_t type){
    DEBUG(F("DFPlayerError: "));
    switch (type) {
      case DfMp3_Error_RxTimeout:
        DEBUGLN(F("Таймаут!"));
        break;
      case DfMp3_Error_SerialWrongStack:
        DEBUGLN(F("Ошибка стека!"));
        break;
      case DfMp3_Error_Busy:
        DEBUGLN(F("Нет карты"));
        break;
      case DfMp3_Error_Sleeping:
        DEBUGLN(F("Ожидание..."));
        break;
      case DfMp3_Error_CheckSumNotMatch:
        DEBUGLN(F("Ошибка контрольной суммы"));
        break;
      case DfMp3_Error_FileIndexOut:
        DEBUGLN(F("Неверный индекс файла"));
        break;
      case DfMp3_Error_FileMismatch:
        DEBUGLN(F("Файл не найден"));
        break;
      case DfMp3_Error_Advertise:
        DEBUGLN(F("Реклама"));
        break;
      case DfMp3_Error_PacketSize:
        DEBUGLN(F("Неверный размер пакета команды"));
        break;
      case DfMp3_Error_PacketHeader:
        DEBUGLN(F("Неверный заголовок пакета команды"));
        break;
      case DfMp3_Error_PacketChecksum:
        DEBUGLN(F("Ошибка CRC пакета команды"));
        break;
      default:
        DEBUGLN(F("Что-то пошло не так..."));
        break;
    }
  }
    
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    printErrorDetail(errorCode);
  }
  
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
  {
    DEBUG(F("Трек завершен #")); DEBUGLN(track);      
    if (!(isAlarming || isPlayAlarmSound) && soundFolder == 0 && soundFile == 0 && runTextSound <= 0) {
      dfPlayer.stop(); Delay(GUARD_DELAY);
    }
    // Перезапустить звук, если установлен его повтор
    if (runTextSound >= 0 && runTextSoundRepeat) {

      // Вот тут просто так перезапустить звук не удается - плеер просто игнорирует 
      // команды перезапуска и повторного воспроизведения звука не наблюдаетмя :(
      
      // dfPlayer.setVolume(constrain(maxAlarmVolume,1,30));  Delay(GUARD_DELAY);
      // dfPlayer.playFolderTrack(3, runTextSound);           Delay(GUARD_DELAY);
      // playingTextSound = runTextSound;

      // Поэтому просто сбрасываем Id текущего играющего звука playingTextSound
      // Сам Id звука который нужно играть - в runTextSound остается нетронутым,
      // Флаг повтора воспроизведения runTextSoundRepeat также остается нетронутым 
      // Также устанавливаем время когда закончил воспроизводиться звук.
      // По-видимому если попытаться включить воспроизведение не выдержав некоторой hgfeps - плеер проигнорирует команду
      // Перезапуск звука дожен произойти в основном цикле в custom.ino, когда таймаут пройдет, обнаружит флаг необходимости повтора runTextSoundRepeat
      // и что текущий играемый звук playingTextSound == -1 не совпадает со звуком, который нужно играть runTextSound - запустится команда старта воспроизведения
      
      playingTextSound = -1;
      runTextSoundTime = millis();
    }    
  }
  
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "готова");
    InitializeDfPlayer2();
    if (!isDfPlayerOk) DEBUGLN(F("MP3 плеер недоступен."));    
  }
  
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "вставлена");
    InitializeDfPlayer2();
    if (!isDfPlayerOk) DEBUGLN(F("MP3 плеер недоступен."));    
  }
  
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "удалена");
    // Карточка "отвалилась" - делаем недоступным все что связано с MP3 плеером
    set_isDfPlayerOk(false);
    alarmSoundsCount = 0;
    dawnSoundsCount = 0;
    noteSoundsCount = 0;
    DEBUGLN(F("MP3 плеер недоступен."));    
  }
  
};
#endif
