// --------------- WEB-SERVER CALLBACK ----------------

// Эта страница отображается, если по какой-то причине файловая система не активирована
// например, прошивка загружена, а файлы WebServer`a не загружены или прошивка скомпилирована
// без выделения места под файловую систему
static const char PGindex_page[] PROGMEM = R"===(
<!doctype html><html><head><meta charset="utf-8"><title>Server not active</title><style>
* {margin:0;padding:0;font-family:Arial,Helvetica,sans-serif;}
section {position:relative;width:100%;height:100vh;}
section .box {position:absolute;top:0;left:0;width:100%;height:100%;display:flex;justify-content:center;align-items:center;}
section .box.box2 {background:#7f54ce;clip-path:polygon(0 0,100% 0,100% 50%,0 50%);}
section .box h2 {color:#7f54ce;font-size:6vw;text-align:center;animation:anim 3s ease-in-out infinite;}
section .box.box2 h2 {color:#fff;}
@keyframes anim {0%,45%{transform:translateY(-70%);} 55%,90%{transform:translateY(70%);} 100%{transform:translateY(-70%);}}</style></head>
<body><section><div class="box box1"><h2>Server not active</h2></div><div class="box box2"><h2>Сервер не активен</h2></div></section></body></html>
)===";

void handleNotActive(AsyncWebServerRequest *request) {
  request->send_P(200, F("text/html"), PGindex_page);
}

void handleNotFound(AsyncWebServerRequest *request) {
  if (!(spiffs_ok && web_ok)) {
    handleNotActive(request);
    return;  
  }  

  String message(F("File Not Found\nURI: ")); message += request->url();
  request->send(404, F("text/plain"), message);
  return;
}

// --------------- WEB-SOCKET CALLBACK ----------------

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client) { 
  if (client == nullptr) return;
  // обрабатываем получаемые сообщения
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

    doc.clear();
    DeserializationError error = deserializeJson(doc,  (const char*)data, len);  // deserialize with deep-copy
    if (error) {
      DEBUG(F("Ошибка в JSON: '"));
      DEBUGWR(data, len);
      DEBUG("'; err: ");
      DEBUGLN(error.f_str());
      return;
    }
    
    if (!isQueueInitialized) InitializeQueues();

    String event(doc["e"].as<const char*>());
    String command(doc["d"].as<const char*>()); 
    
    if (event == "cmd" && command.length() > 0) {  
      // В одном сообщении может быть несколько команд. Каждая команда начинается с '$' и заканчивается ';'/ Пробелы между ';' и '$' НЕ допускаются.
      command.replace("\n", "~");
      command.replace(";$", "\n");
      uint32_t count = CountTokens(command, '\n');

      for (uint8_t i = 1; i <= count; i++) {
        String cmd(GetToken(command, i, '\n'));
        cmd.replace('~', '\n');
        cmd.trim();
        
        // После разделения команд во 2 и далее строке '$' (начало команды) удален - восстановить
        if (!cmd.startsWith(F("$"))) {
          cmd = '$' + cmd;
        }
        // После разделения команд во 2 и далее строке ';' (конец команды) удален - восстановить
        // Команда '$6 ' не может быть в пакете и признак ';' (конец команды) не используется - не восстанавливать
        if (!cmd.endsWith(";") && !cmd.startsWith(F("$6 "))) {
          cmd += ';';
        }        
        if (cmd.length() > 0) {
          if (queueLength < QSIZE_IN) {
            queueLength++;
            cmdQueue[queueWriteIdx++] = cmd;
            if (queueWriteIdx >= QSIZE_IN) queueWriteIdx = 0;
          } else {
            DEBUG(F("Переполнение очереди входящих команд - "));
            DEBUGLN(QSIZE_IN);
            DEBUG(F("Free: "));
            DEBUG(ESP.getFreeHeap());
            DEBUG(F(" Max: "));
            #if defined(ESP8266)
            DEBUG(ESP.getMaxFreeBlockSize());
            DEBUG(F("  Frag: "));
            DEBUG(ESP.getHeapFragmentation());
            #else
            DEBUG(ESP.getMaxAllocHeap());        
            #endif     
            DEBUGLN();                   
          }
        }
      }    
    }    
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (server == nullptr || client == nullptr) return;
  switch (type) {
    case WS_EVT_CONNECT:
      web_client_count++;
      last_web_client_id = client->id();
      Serial.printf("WebSocket клиент #%u с адреса %s\n", last_web_client_id, client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      web_client_count--;
      Serial.printf("WebSocket клиент #%u отключен\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len, client);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// ----------------------------------------------------

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

/**
 * @brief подготовить и отправить сообщение активным вебсокет-клиентам
 * 
 * @param message - строка с телом сообщения
 * @param topic - топик
 */
void prepareAndSendMessage(const String& message, const String& topic) {
  // здесь создаётся вложенный сериализованный джейсон-объект, некрасиво, но так работает бэкэнд 
  doc.clear();
  doc["e"] = topic.c_str();    // т.к. объект короткоживущий - создаем через указатель
  doc["d"] = message.c_str();

  wsSrvSendAll(&ws, doc);  
  doc.clear();
}

void putOutQueueW(const String& topic, const String& message) {
  
  // Если нет подключенных клиентов - ничего никому отправлять не нужно
  if (web_client_count == 0) {
    outWQueueReadIdx = 0;
    outWQueueWriteIdx = 0;
    outWQueueLength = 0;
    return;
  }  

  // Помещаем сообщение в очередь отправки
  if (outWQueueLength < QSIZE_OUT) {    
    outWQueueLength++;
    outWQueue[outWQueueWriteIdx] = message;
    tpcWQueue[outWQueueWriteIdx] = topic;
    outWQueueWriteIdx++;
    if (outWQueueWriteIdx >= QSIZE_OUT) outWQueueWriteIdx = 0;
  } else {
    DEBUG(F("Переполнение очереди исходящих команд - "));
    DEBUGLN(QSIZE_OUT);
  }
}

void SendWeb(const String& message, const String& topic) {
  if (web_client_count < 0) web_client_count = 0;
  if (web_client_count == 0) return;

  putOutQueueW(topic, message);
}

void SendWebError(const String& message) {
  SendWebKey("ER", message);
}

void SendWebInfo(const String& message) {
  SendWebKey("NF", message);
}

void SendWebKey(const String& key, const String& value) {
  String topic(TOPIC_STT);

  doc.clear();
  doc[key] = value;

  String out;
  serializeJson(doc, out);      
  doc.clear();
  
  // Если key - ключ с возможно большим значением - отправлять как готовый json-объект
  // Если key - обычный ключ - удалить начальную и конечную фигурные скобки - тогда это будет
  // просто составная часть для накопительного объекта json
  if (!isBigSizeKey(key)) {
    out = out.substring(1, out.length() - 1);  
    SendWeb(out, topic);  
  } else {
    bool canWrite = ws.availableForWriteAll(); 
    if (canWrite) {
      prepareAndSendMessage(out, topic);
    } else {
      SendWeb(out, topic);
    }
  }
}

bool isClosed = false;

// Отправка сообщений из очереди в Web-сокет
void processOutQueueW() {

  long interval = millis() - lastWebSend;
  if (interval < 250) return;

  String outMessage;
    
  // Пока есть сообщения в очереди и WebSocket способен их отправлять (не заполнена внутренняя очередь сокета)
  // выполнить отправку накопившихся сообшщений из очереди
  while (outWQueueLength > 0 && web_client_count > 0) {
    
    bool canWrite = ws.availableForWriteAll();      
    if (!canWrite) {
      if (!isClosed) {        
        DEBUGLN(DELIM_LINE);                                              // !!!
        DEBUGLN(F("Запись в сокет недоступна!"));                         // !!!
        DEBUG(F("Клиентов подключено: "));                                // !!!
        DEBUGLN( ws.count());                                             // !!!
        DEBUGLN(F("Перезапуск сокета..."));                               // !!!
        int8_t cnt = 0;
        while (ws.count() > 0 && cnt < 100) {
          ws.cleanupClients(0);
          delay(10);
          cnt++;
        }
        DEBUG(F("Клиентов подключено: ")); // !!!
        DEBUGLN(ws.count());               // !!!
        isClosed = true;        
      }
      break;
    }
    
    isClosed = false;
    
    // Cодержимое отправляемого сообщения
    String message(outWQueue[outWQueueReadIdx]);
    String topic(tpcWQueue[outWQueueReadIdx]);

    if (message.length() > 0) {
      // Если message начинается с '{' - это готовое сообщение для отправки клиентам, только обернуть в конверт
      // Если message НЕ начинается с '{' - это строка часть JSON-объекта пара ключ-значение
      if (message[0] == '{') {
        // {"act":"TIME","server_name":"ru.pool.ntp.org","server_ip":"85.21.78.23","result":"REQUEST"}
        // {FM":"9008"}
        // Такие сообщения могут быть а в любом топике
        prepareAndSendMessage(message, topic);
        lastWebSend = millis();  
      } else {
        // "FM":"9008" -- выделить значение ключа
        // Такие сообщения могут быть только в топике TOPIC_STT
        outMessage += message + ',';
      }
    }
    
    // Очищаем строку в очереди
    outWQueue[outWQueueReadIdx].clear();  
    tpcWQueue[outWQueueReadIdx].clear();
    outWQueueReadIdx++;
    
    if (outWQueueReadIdx >= QSIZE_OUT) outWQueueReadIdx = 0;
    outWQueueLength--;  
  }

  if (outMessage.length() > 0) {
    // Были добавлены ключи-значения в накопительный объект -
    // сформировать сообщение и отправить клиентам
    outMessage = '{' + outMessage.substring(0, outMessage.length() - 1) + '}';
    // Такие сообщения могут быть а в любом топике
    prepareAndSendMessage(outMessage, TOPIC_STT);
    lastWebSend = millis();  
  }
  
}

bool checkWebDirectory() {

  String directoryName(BASE_WEB);
  
  DEBUG(F("Папка '"));
  DEBUG(directoryName);
  
  if (LittleFS.exists(directoryName)) {    
    DEBUGLN(F("' обнаружена."));
  } else {
    DEBUGLN(F("' не обнаружена."));
    return false;
  }
  
  DEBUGLN(F("Cпискок файлов web-интерфейса:"));  

  String dir_name(directoryName);
  int16_t p = dir_name.lastIndexOf("/");
  if (p >= 0) dir_name = dir_name.substring(p + 1);

  checkWebSubDir(1, directoryName, dir_name);  

  return true;
}

void checkWebSubDir(uint8_t level, const String& full_dir_name, const String& dir_name) {
  
  uint32_t file_size;
  uint8_t  lvl;

  DEBUG(padRight(" ", level * 3)); DEBUG('['); DEBUG(dir_name); DEBUGLN(']');
  
  #if defined(ESP32)
    File folder = LittleFS.open(full_dir_name);
  #else
    Dir  folder = LittleFS.openDir(full_dir_name);
  #endif
  
  while (true) {

    #if defined(ESP32)
      File entry = folder.openNextFile();
      if (!entry) break;
      String file_name(entry.name());
      // Полученное имя файла содержит имя папки (на ESP32 это так, на ESP8266 - только имя файла) - оставить только имя файла
      int16_t p = file_name.lastIndexOf("/");
      if (p >= 0) file_name = file_name.substring(p + 1);
    #else        
      if (!folder.next()) break;
      File entry = folder.openFile("r");
      String file_name(entry.name());
    #endif

    if (!entry.isDirectory()) {
            
      file_size = entry.size();
      String fs(file_size);
      
      if (file_size == 0) {
        entry.close();
        continue;    
      }

      String sp(padRight(" ", (level + 1) * 3)); sp += padRight(file_name, 26);
      DEBUG(padRight(sp, 40));      
      DEBUGLN(padLeft(fs, 8));
      
    } else {
      lvl = level + 1;
      String str(full_dir_name); str += '/'; str += file_name;
      checkWebSubDir(lvl, str, file_name);
    }
    entry.close();
  }
}

/**
 * @brief serialize JsonVarian data directly to WebSocket buffer and send to ALL clients connected to WS Server
 * функция сериализует объект непосредственно в буфер вебсокет-сервера (если достаточно памяти)
 * и не требует создания промежуточной копии сериализованной строки в памяти
 * 
 * @param ws - pointer to ws server instance
 * @param data - json object to send
 */
void wsSrvSendAll(AsyncWebSocket *ws, const JsonVariantConst& data){

  if (!ws->count()) return;   // no client connected, nowhere to send

  size_t length = measureJson(data);
  auto buffer = ws->makeBuffer(length);

  if (!buffer) {
    DEBUGLN(F("Недостаточно памяти для буфера отправки в WebSocket"));
    return;    // not enough mem to send data
  }
  
  serializeJson(data, reinterpret_cast<char*>(buffer->get()), length);
  ws->textAll(buffer);
}
