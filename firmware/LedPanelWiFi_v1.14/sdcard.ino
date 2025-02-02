// Эффекты на SD-карте
//
// Сортировка загруженных эффектов: от пользователя 'alex-3ton'

#if (USE_SD == 1)

File fxdata;

int8_t file_idx;    // Служебное - для определения какой следующий файл воспроизводить
String fileName;

void InitializeSD() {

  set_isSdCardReady(false);
  set_isSdCardExist(false);
  
  #if (USE_SD == 1 && FS_AS_SD == 0)
    DEBUGLN(F("Инициализация SD-карты..."));  
    sd_card_ok = SD.begin(SD_CS_PIN);
    set_isSdCardExist(sd_card_ok);
  #else
    DEBUGLN(F("Эмуляция SD-карты в FS."));  
    sd_card_ok = true;
    set_isSdCardExist(true);
  #endif

  DEBUG(F("SD-карта "));
  if (!sd_card_ok) {
    DEBUGLN(F("не "));
  }
  DEBUGLN(F("подключена."));

  if (!sd_card_ok) return;

  #if (USE_SD == 1 && FS_AS_SD == 0)
    File file;
  
    #if defined(ESP32)
      String file_name(F("/t.t"));
    #else
      String file_name(F("t.t"));
    #endif

    file = SD.open(file_name, FILE_WRITE);
    bool ok = file != 0;
  
    if (ok) {    
      file.println(FIRMWARE_VER);
      file.close();
      ok = SD.remove(file_name);
    }
    
    if (!ok) {    
      DEBUGLN(F("SD-карта только для чтения"));    
    }
  #endif
  
  DEBUGLN();
}

void loadDirectory() {

  String directoryName('/');
  directoryName += pWIDTH;
  directoryName += 'x';
  directoryName += pHEIGHT;

  DEBUG(F("Папка с эффектами "));
  DEBUG(directoryName);
  
  #if (USE_SD == 1 && FS_AS_SD == 0)
  if (SD.exists(directoryName)) {
  #else    
  if (LittleFS.exists(directoryName)) {    
  #endif
    DEBUGLN(F(" обнаружена."));
  } else {
    DEBUGLN(F(" не обнаружена."));
    return;
  }

  DEBUGLN(F("Загрузка списка файлов с эффектами..."));

  uint32_t file_size;
  
  #if (USE_SD == 1 && FS_AS_SD == 0)
    File folder = SD.open(directoryName);     
    while (folder) {
      File entry =  folder.openNextFile();
      
      // Очередной файл найден? Нет - завершить
      if (!entry) break;
          
      if (!entry.isDirectory()) {
              
        file_size = entry.size();
        {
          String file_name(entry.name());
          String fn(file_name);
          fn.toLowerCase();
          
          if (!fn.endsWith(".out") || file_size == 0) {
            entry.close();
            continue;    
          }
      
          // Если полученное имя файла содержит имя папки (на ESP32 это так, на ESP8266 - только имя файла) - оставить только имя файла
          int16_t p = file_name.lastIndexOf("/");
          if (p >= 0) file_name = file_name.substring(p + 1);
          p = file_name.lastIndexOf(".");
          if (p >= 0) file_name = file_name.substring(0, p);
                      
          nameFiles[countFiles++] = file_name;
        }
        
        if (countFiles >= MAX_FILES) {
          DEBUG(F("Максимальное количество эффектов: "));        
          DEBUGLN(MAX_FILES);
          break;
        }
      }
          
      entry.close();
    }
    folder.close();
    
  #else

    #if defined(ESP32)
      File folder = LittleFS.open(directoryName);
    #else
      Dir  folder = LittleFS.openDir(directoryName);
    #endif
      
    while (true) {

      #if defined(ESP32)
        File entry = folder.openNextFile();
        if (!entry) break;
      #else        
        if (!folder.next()) break;
        File entry = folder.openFile("r");
      #endif
                     
      if (!entry.isDirectory()) {
              
        file_size = entry.size();
        {
          String file_name(entry.name());
          String fn(file_name);
          fn.toLowerCase();
          
          if (!fn.endsWith(".out") || file_size == 0) {
            entry.close();
            continue;    
          }
    
          // Если полученное имя файла содержит имя папки (на ESP32 это так, на ESP8266 - только имя файла) - оставить только имя файла
          int16_t p = file_name.lastIndexOf("/");
          if (p >= 0) file_name = file_name.substring(p + 1);
          p = file_name.lastIndexOf(".");
          if (p >= 0) file_name = file_name.substring(0, p);
                      
          nameFiles[countFiles++] = file_name;
        }
        
        if (countFiles >= MAX_FILES) {
          DEBUG(F("Максимальное количество эффектов: "));        
          DEBUGLN(MAX_FILES);
          break;
        }
      }
      entry.close();
    }        
    #if defined(ESP32)
    folder.close();
    #endif
  #endif

  if (countFiles == 0) {
    DEBUGLN(F("Доступных файлов эффектов не найдено"));
  } else {
    sortAndShow(directoryName);
  }
}

// ----------------------------------
// --------- alex-3ton part ---------

void bbSort(String* arr, int sz) {
  // Простейшая сортировка (пузырёк)
  // Находим наименьшее значение и определяем на первую позицию,
  // Когда поиск наименьшего значения закончен следующая позиция
  //   устанавливается следующей по положению.
  // И т.д. тупым перебором
  if (sz > 1) {
    for( int i=0; i<sz; i++)  {
      for( int j=i+1; j<sz; j++)  {
        String a(arr[i]); a.toLowerCase();
        String b(arr[j]); b.toLowerCase();
        if( b < a) {
          String q(arr[i]);
          arr[i] = arr[j];
          arr[j] = q;
        }
      }
    }
  }
}

String fileSizeToString(uint32_t file_size){
  uint8_t  sz = 0;
  float    fsize = file_size;
  String   fs_out("байт");
  String   str;
  
  if (fsize > 1024) { fsize /= 1024.0; fs_out = "К"; sz++;}
  if (fsize > 1024) { fsize /= 1024.0; fs_out = "М"; sz++;}
  if (fsize > 1024) { fsize /= 1024.0; fs_out = "Г"; sz++;}

  if (sz == 0) {
    str += file_size; str += ' '; str += fs_out;
  } else {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", fsize);
    str += buf; str += ' '; str += fs_out;    
  }

  return str;
}

void sortAndShow(const String& directoryName) {

  // сортируем
  bbSort( nameFiles, countFiles );

  bool     error = false;
  uint32_t file_size;
  uint16_t frame_sz = NUM_LEDS * 3 + 1;

  for (uint8_t x = 0; x < countFiles; x++) {
    String fileName(directoryName); fileName += '/'; fileName += nameFiles[x]; fileName += ".out";
    
    #if (USE_SD == 1 && FS_AS_SD == 0)    
    fxdata = SD.open(fileName);
    #else
    fxdata = LittleFS.open(fileName, "r");
    #endif
    
    if (!fxdata) {
      error = true;
      DEBUG("   "); DEBUG(padRight(nameFiles[x],16)); DEBUGLN(F("ERR - ошибка загрузки"));
      nameFiles[x] = "";
      continue;
    }

    file_size = fxdata.size();
    fxdata.close();

    uint16_t frames = file_size / frame_sz;
    uint16_t cnt = file_size % frame_sz;
    String s_fsize = fileSizeToString(file_size);
    s_fsize = padLeft(s_fsize, 10);

    DEBUG("   ");
    DEBUG(padRight(nameFiles[x], 16));
    if (cnt == 0) {
      s_fsize += ", "; s_fsize += frames; s_fsize += F(" кадр.");
      DEBUG(padRight(s_fsize, 29));
      DEBUGLN(F("OK"));
    } else {
      DEBUG(padRight(s_fsize, 25));
      DEBUGLN(F("ERR - ошибка формата"));
      nameFiles[x] = "";
    }
  }

  // Если были ошибки - удалить ошибочные файлы эффектов
  if (error) {
    uint8_t idx = 0;
    for (uint8_t x = 0; x < countFiles; x++) {
      String file_name(nameFiles[x]);
      if (file_name.length() > 0) {
        if (x != idx) {
          nameFiles[idx] = file_name;
        }
        idx++;
      }
    }
    for (uint8_t x = idx; x < countFiles; x++) {
      nameFiles[x].clear();
    }
    countFiles = idx;
    DEBUGLN(F("Файлы с ошибками пропущены."));
  }
  DEBUG(F("Доступно ")); DEBUG(countFiles); DEBUGLN(F(" файлов эффектов."));
}

// --------- alex-3ton part ---------
// ----------------------------------

void sdcardRoutine() {
 
 if (loadingFlag || play_file_finished) {
   //modeCode = MC_SDCARD;

   // Если карта не готова (нт файлов эффектов) - перейти к следующему режиму
   if (!isSdCardReady) {
     if (fxdata) fxdata.close();
     nextMode();
     return;
   }

   // Выбор другого файла - только если установлен loadingFlag
   // Если сюда попали по play_file_finished - блок if (loading) не выполняется - file_idx остается прежним - просто вернуться к началу проигранного файла и воспроизвести его еще раз.
   // Это позволит длительное время "играть" эффект использую зацикленные короткие фрагменты 
   // effectScaleParam2[MC_SDCARD]: 0 - случайный файл; 1 - последовательный перебор; 2 и далее - привести к индексы в массиве nameFiles
   
   if (loadingFlag) {

     loadingFlag = false;
     int8_t currentFile = -1;
     // Указан специальный эффект для бегущей строки? - брать его 
     if (specialTextEffectParam >= 0)
       currentFile = specialTextEffectParam - 1;
     // Указано случайное воспроизведение файлов с карты?
     else if (getEffectScaleParamValue2(MC_SDCARD) == 0)
       currentFile = -2;                                       // Случайный порядок
     else if (getEffectScaleParamValue2(MC_SDCARD) == 1)
       currentFile = -1;                                       // Последовательное воспроизведение
     else
       currentFile = getEffectScaleParamValue2(MC_SDCARD) - 2; // Указанный выбранный файл эффектов

     if (currentFile < 0 || currentFile >= countFiles) {
        if (countFiles == 1) {
          file_idx = 0;
        } else if (countFiles == 2) {
          file_idx = (file_idx != 1) ? 0 : 1;
        } else if (currentFile == -1) {
          // Последовательный перебор файлов с SD-карты
          if (sf_file_idx < 0) sf_file_idx = 0;
          if (sf_file_idx >= countFiles) sf_file_idx = countFiles - 1;
          file_idx = sf_file_idx;
        } else {
          // Случайный с SD-карты
          file_idx = random16(0, countFiles);
          if (file_idx >= countFiles) file_idx = countFiles - 1;
        }
      } else {
        file_idx = currentFile;
      }
    }

    // При загрузке имен файлов с SD-карты в nameFiles только имя файла внутри выбранной папки -- чтобы получить полное имя файла для загрузки  нужно к имени файла добавить имя папки
    fileName.clear();
    fileName += '/'; fileName += pWIDTH; fileName += 'x'; fileName += pHEIGHT; fileName += '/'; fileName += nameFiles[file_idx]; fileName += ".out";

    play_file_finished = false;
    DEBUG(F("Загрузка файла эффекта: '"));
    DEBUG(fileName);

    bool error = false;

    if (fxdata) fxdata.close();
    
    #if (USE_SD == 1 && FS_AS_SD == 0)    
    fxdata = SD.open(fileName);
    #else
    fxdata = LittleFS.open(fileName, "r");
    #endif
    
    if (fxdata) {
      DEBUGLN(F("' -> ok"));
    } else {
      DEBUGLN(F("' -> ошибка"));
      error = true;
    }

    doc.clear();
    doc["act"] = F("SDCARD");
    if (error) {
      doc["result"] = F("ERROR");
    } else {
      doc["result"] = F("OK");
    }
    doc["file"] = fileName;

    String out;
    serializeJson(doc, out);
    doc.clear();

    SendWeb(out, TOPIC_SDC);

    FastLED.clear();
  }

  // Карта присутствует и файл открылся правильно?
  // Что-то пошло не так - перейти к следующему эффекту
  if (!(isSdCardReady && fxdata)) {
    if (fxdata) fxdata.close();
    nextMode();
    return;
  }

  if (fxdata.available()) {
    char tmp;
    fxdata.readBytes(&tmp, 1);  // ??? какой-то байт в начале последовательности - отметка начала кадра / номер канала кадрового потока, передаваемого на устройство ???
    char* ptr = reinterpret_cast<char*>(&leds[0]);
    int16_t cnt = fxdata.readBytes(ptr, NUM_LEDS * 3);  // 3 байта на цвет RGB
    play_file_finished = (cnt != NUM_LEDS * 3);
  } else {
    play_file_finished = true;
  }

  if (play_file_finished) {
    DEBUG("'"); DEBUG(fileName); DEBUGLN(F("' - завершено"));
    fxdata.close();
    fileName.clear();

    /*
    if (currentFile >= 0) {
      currentFile++; 
      if (currentFile > countFiles - 1) {
        currentFile = 0;
      }
    }
    */
  }
}

void sdcardRoutineRelease() {
  if (fxdata) fxdata.close();
}
#endif
