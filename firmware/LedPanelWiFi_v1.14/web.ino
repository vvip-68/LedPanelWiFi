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
      String cmd; cmd.reserve(IN_CMD_SIZE);

      for (uint8_t i = 1; i <= count; i++) {
        
        cmd = GetToken(command, i, '\n');
        cmd.replace('~', '\n');
        cmd.trim();
        
        // После разделения команд во 2 и далее строке '$' (начало команды) удален - восстановить
        if (cmd.charAt(0) != '$') {
          cmd = '$' + cmd;
        }
        // После разделения команд во 2 и далее строке ';' (конец команды) удален - восстановить
        // Команда '$6 ' не может быть в пакете и признак ';' (конец команды) не используется - не восстанавливать
        if (!cmd.endsWith(";") && cmd.charAt(1) != '6') {
          cmd += ';';
        }        
        if (cmd.length() > 0) {
          // Команду запроса параметров '$6 7|' в очередь не помещать.
          // Сразу по приходу - разабрать ключи и поместить изх в список запрошенных ключей 
          if (cmd.startsWith("$6 7|")) {
            // "$6 7|FM|UP" - эта команда используется в качестве ping, посылается каждые 2-3 секунда, ее выводить не нужно, чтобы не забивать вывод в лог
            if (cmd != "$6 7|FM|UP") {          
              DEBUG(F("WEB пакeт размером "));
              DEBUGLN(cmd.length());          
              DEBUG(F("<-- "));
              DEBUGLN(cmd);          
              // При поступлении каждой команды вывести в консоль информацию о свободной памяти
              printMemoryInfo();
            }
            // Добавить ключи в список изменившихся параметров, чьи новые значения необходимо отправить на сервер
            cmd.replace("$6 7|",""); 
            cmd.replace(' ','|');
            int16_t pos_start = 0;
            int16_t pos_end = cmd.indexOf('|', pos_start);
            int16_t len = cmd.length();
            String key; key.reserve(6);
            if (pos_end < 0) pos_end = len;
            while (pos_start < len && pos_end >= pos_start) {
              if (pos_end > pos_start) {      
                key = cmd.substring(pos_start, pos_end);
                if (key.length() > 0) addKeyToChanged(key);
              }
              pos_start = pos_end + 1;
              pos_end = cmd.indexOf('|', pos_start);
              if (pos_end < 0) pos_end = len;
            }          
          } else {
            // Поместить пришедшую команду в очередь обработки
            if (queueLength < QSIZE_IN) {
              queueLength++;
              cmdQueue[queueWriteIdx++] = cmd;
              if (queueWriteIdx >= QSIZE_IN) queueWriteIdx = 0;
            } else {
              DEBUG(F("Переполнение очереди входящих команд - ")); DEBUG(QSIZE_IN); DEBUG(F("; ")); DEBUGLN(cmd);
              printMemoryInfo();
            }
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
      Serial.printf(PSTR("WebSocket клиент #%u с адреса %s\n"), (unsigned int)last_web_client_id, client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      web_client_count--;
      Serial.printf(PSTR("WebSocket клиент #%u отключен\n"), (unsigned int)client->id());
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

void SendWeb(const String& message, const String& topic) {
  if (web_client_count < 0) web_client_count = 0;
  if (web_client_count == 0) return;

  prepareAndSendMessage(message, topic);
}

void SendWebError(const String& message) {
  SendWebKey("ER", message);
}

void SendWebInfo(const String& message) {
  SendWebKey("NF", message);
}

void SendWebKey(const String& key, const String& value) {
  String topic(TOPIC_STT);
  String out;

  // Если key - ключ с возможно большим значением - отправлять как готовый json-объект
  // Если key - обычный ключ - удалить начальную и конечную фигурные скобки - тогда это будет
  // просто составная часть для накопительного объекта json
  #if defined(MAX_BUFFER_SIZE)  
    uint16_t max_part_len = MAX_BUFFER_SIZE - 49;                  // <-- ??? должно быть нечетным, иначе почему-то substring(0,max_part_len) рвет UTF8 строку посреди символа и 1-я часть строки в конце получает "обрубок"
  #else
    uint16_t max_part_len = 511;                                   // <-- ??? должно быть нечетным, иначе почему-то substring(0,max_part_len) рвет UTF8 строку посреди символа и 1-я часть строки в конце получает "обрубок"
  #endif
                                                                   //         с нераспознаваемым символом на конце, вторая часть - "обрубок" с нераспазноваемым символом в начале. Потом такая строка упаковывается в JSON 
                                                                   //         принимая которую WebSocket говорит что фрейм не может быть распознан как UTF8 текст и рвет соединение Web-сокета,
                                                                   //         что в логе браузера - постоянные "клиент был отключен" и все новые и новые попытки переподключения 
  // Топик сообщения - основной топик плюс ключ (имя параметра)
  if (value.length() <= max_part_len) {
    // Значение все целиком влазит в буфер - отправляем за раз всю строку
    doc.clear();  
    doc[key] = value;
    serializeJson(doc, out);      
    doc.clear();
    prepareAndSendMessage(out, topic);
  } else {          
    // Значение не влазит в буфер отправки - отправляем частями
    // Первая часть в начале имеет фразу `!`, следуюшие части кроме последней начинаются с "`>`", последняя часть - `#`
    bool first = true;
    String str(value);
    String tmp; tmp.reserve(max_part_len + 6);
    while (true) {
      if (str.length() <= max_part_len) {
        tmp = "`#`"; tmp += str;                      // `#` - признак окончания незавершенной строки
        doc.clear();  
        doc[key] = tmp;        
        serializeJson(doc, out);      
        doc.clear();
        prepareAndSendMessage(out, topic);
        out.clear();        
        break;
      } else {
        tmp = (first ? "`!`" : "`>`");                // '`!`' - признак начала незавершенной строки
        tmp += str.substring(0, max_part_len);        // '`>`' - признак продолжения незавершенной строки
        doc.clear();
        doc[key] = tmp;
        serializeJson(doc, out); 
        doc.clear();   
        prepareAndSendMessage(out, topic);
        out.clear();  
        str = str.substring(max_part_len);
        first = false;
      }
    }  
  }
}

bool isClosed = false;


bool checkWebDirectory() {

  String directoryName(BASE_WEB);
  
  DEBUG(F("Папка '"));
  DEBUG(directoryName);
  
  if (LittleFS.exists(directoryName)) {    
    DEBUGLN(F("' обнаружена."));
  } else {
    DEBUGLN(F("' не обнаружена.\n"));
    return false;
  }
  
  DEBUGLN(F("Cписок файлов web-интерфейса:"));  

  String dir_name(directoryName);
  int16_t p = dir_name.lastIndexOf("/");
  if (p >= 0) dir_name = dir_name.substring(p + 1);

  checkWebSubDir(1, directoryName, dir_name);  
  DEBUGLN();

  return true;
}

void checkWebSubDir(uint8_t level, const String& full_dir_name, const String& dir_name) {
  
  uint32_t file_size;
  uint8_t  lvl;
  String file_name; file_name.reserve(25);
  String fs;        fs.reserve(10);
  String sp;        sp.reserve(60);

  DEBUG(padRight(" ", level * 3)); DEBUG('['); DEBUG(dir_name); DEBUGLN(']');
  
  #if defined(ESP32)
    File folder = LittleFS.open(full_dir_name);
  #else
    Dir  folder = LittleFS.openDir(full_dir_name);
  #endif
  
  while (true) {

    #if defined(ESP8266)
    ESP.wdtFeed();
    #endif
    
    #if defined(ESP32)
      File entry = folder.openNextFile();
      if (!entry) break;
      file_name = entry.name();
      // Полученное имя файла содержит имя папки (на ESP32 это так, на ESP8266 - только имя файла) - оставить только имя файла
      int16_t p = file_name.lastIndexOf("/");
      if (p >= 0) file_name = file_name.substring(p + 1);
    #else        
      if (!folder.next()) break;
      File entry = folder.openFile("r");
      file_name = entry.name();
    #endif

    if (!entry.isDirectory()) {
            
      file_size = entry.size();
      fs = file_size;
      sp = padRight(" ", (level + 1) * 3); sp += padRight(file_name, 26);
      DEBUG(padRight(sp, 40));      
      DEBUGLN(padLeft(fs, 8));
      
    } else {
      lvl = level + 1;
      String str(full_dir_name); str += '/'; str += file_name;
      if (file_name != "txt") { 
        checkWebSubDir(lvl, str, file_name);
      }
    }
    entry.close();
  }
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
  
  #ifndef YUBOXMOD                                                                                                                                              
    serializeJson(data, (char*)buffer->get(), length);                                                                                                        
  #else                                                                                                                                                         
    serializeJson(data, (char*)buffer->data(), length);                                                                                                       
  #endif  
  
  ws->textAll(buffer);
}
