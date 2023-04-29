
// Набор параметров, который требуется отправлять MQTT и WEB - клиентам на изменение их состояния
// STATE_KEYS начинается с '|' и заканчивается на '|' для удобства поиска / проверки наличия ключа в строке,
// которые должны быть удалены перед использованием далее для перебора ключей
// Если вам не нужен на стороне MQTT клиента полный перечень параметров - оставьте только те, что вам нужны

// Если вы не хотите отправлять пароли в канал MQTT - удалите из списка разрешенных ключей следующие:
//   AA - пароль точки доступа
//   NA - пароль подключения к сети - в NQTT канал
//   NX - пароль подключения к сети - в Web канал
//   QW - пароль к MQTT брокеру

#define STATE_KEYS F("|W|H|VR|HN|DM|PS|PD|IT|AL|RM|PW|BR|WU|WT|WR|WS|WC|WN|WZ|EF|EN|UT|UC|SE|SS|SQ|BE|CE|CC|CO|CH|CK|CV|NC|SC|C1|DC|DD|DI" \
                     "|NP|NT|NZ|NM|NS|DW|OF|TM|AW|AT|AD|AE|MX|MU|MD|MV|MA|MB|MP|AN|AB|AU|NW|NX|IP|QZ|QA|QP|QS|QU|QR|TE|TI|TS|CT|C2|CL|ST|TY|LG" \
                     "|AM1T|AM1A|AM2T|AM2A|AM3T|AM3A|AM4T|AM4A|AM5A|AM6A|UP|FM|T1|T2|M0|M1|M2|M3|M4|M5|M6|M7|M8|M9|M10|M11" \
                     "|QW|E0|E1|E2|E3|LE|FV|S1|S2|S3|SD|SX|FS|EE|SM|BS|FL0|FL1|EMX|EMY|ELX|ELY|ELW|ELH|")  // !!! завершающая "|" - обязательна !!!        
#if (USE_MQTT == 1)

// ------------------ MQTT CALLBACK -------------------

void callback(char* topic, uint8_t* payload, uint32_t length) {
  if (stopMQTT) return;
  // проверяем из нужного ли нам топика пришли данные
  DEBUG("MQTT << topic='" + String(topic) + "'");
  if (strcmp(topic, mqtt_topic(TOPIC_CMD).c_str()) == 0) {
    memset(incomeCmdBuffer, 0, BUF_CMD_SIZE);
    memcpy(incomeCmdBuffer, payload, length);
    
    DEBUG(F("; cmd='"));
    DEBUG(incomeCmdBuffer);
    DEBUG("'");
    
    // В одном сообщении может быть несколько команд. Каждая команда начинается с '$' и заканчивается ';'/ Пробелы между ';' и '$' НЕ допускаются.
    String command = String(incomeCmdBuffer);    
    command.replace("\n", "~");
    command.replace(";$", "\n");
    uint32_t count = CountTokens(command, '\n');
    
    for (uint8_t i=1; i<=count; i++) {
      String cmd = GetToken(command, i, '\n');
      cmd.replace('~', '\n');
      cmd.trim();
      // После разделения команд во 2 и далее строке '$' (начало команды) удален - восстановить
      if (!cmd.startsWith("$")) {
        cmd = "$" + cmd;
      }
      // После разделения команд во 2 и далее строке ';' (конец команды) удален - восстановить
      // Команда '$6 ' не может быть в пакете и признак ';' (конец команды) не используется - не восстанавливать
      if (!cmd.endsWith(";") && !cmd.startsWith(F("$6 "))) {
        cmd += ";";
      }        
      if (cmd.length() > 0 && queueLength < QSIZE_IN) {
        queueLength++;
        cmdQueueType[queueWriteIdx] = 'M';
        cmdQueue[queueWriteIdx++] = cmd;
        if (queueWriteIdx >= QSIZE_IN) queueWriteIdx = 0;
      }
    }    
  }
  DEBUGLN();
}

// ----------------------------------------------------

// Формирование топика сообщения
String mqtt_topic(String topic) {
  String ret_topic = mqtt_prefix;
  if (ret_topic.length() > 0 && !ret_topic.endsWith("/")) ret_topic += "/";
  return ret_topic + topic;
}

void SendMQTT(String &message, String topic) {
  if (stopMQTT) return;
  putOutQueue(mqtt_topic(topic), message);
}

// Поместить сообщения для отправки на сервер в очередь
void putOutQueue(String topic, String message) {
  putOutQueue(topic, message, false);
}

void putOutQueue(String topic, String message, bool retain) {
  if (stopMQTT) return;
  bool ok = false;
  ok = mqtt.beginPublish(topic.c_str(), message.length(), retain);
  if (ok) {
    // Если инициация отправки была успешной - заполняем буфер отправки передаваемой строкой сообщения
    mqtt.print(message.c_str());
    // Завершаем отправку. Если пакет был отправлен - возвращается 1, если ошибка отправки - возвращается 0
    ok = mqtt.endPublish() == 1;
    if (ok) {
      // Отправка прошла успешно
      DEBUG(F("MQTT >> OK >> ")); 
      if (retain)
        DEBUG(F("[r] "));
      else   
        DEBUG(F("[ ] "));
      DEBUG(topic);
      DEBUG(F("\t >> ")); 
      DEBUGLN(message);
    }
  }
  // Если отправка не произошла и в очереди есть место - помещаем сообщение в очередь отправки
  if (!ok && outQueueLength < QSIZE_OUT) {
    outQueueLength++;
    tpcQueue[outQueueWriteIdx] = topic;
    outQueue[outQueueWriteIdx] = message;
    rtnQueue[outQueueWriteIdx] = retain;
    outQueueWriteIdx++;
    if (outQueueWriteIdx >= QSIZE_OUT) outQueueWriteIdx = 0;
  }
}

bool subscribeMqttTopicCmd() {
  bool ok = false;
  if (mqtt.connected()) {
    DEBUG(F("Подписка на topic='cmd' >> "));
    ok = mqtt.subscribe(mqtt_topic(TOPIC_CMD).c_str());
    if (ok) DEBUGLN(F("OK"));
    else    DEBUGLN(F("FAIL"));
  }
  return ok;
}

void checkMqttConnection() {

  // Ели нет соединения  интернетом - незачем проверять наличие подключения к MQTT-серверу
  if (!wifi_connected) return;

  if (!mqtt.connected() && millis() < nextMqttConnectTime) return;

  // Проверить - выполнена ли подписка на топик команд, если нет - подписаться
  if (!stopMQTT && !mqtt_topic_subscribed) {
    mqtt_topic_subscribed = subscribeMqttTopicCmd();
  }
  // Отправить сообщение из очереди, если очередь содержит сообщения
  // Проверка наличия подключения и защитного интервала отправки проверяется внутри вызова
  if (!stopMQTT && mqtt_topic_subscribed) {
    processOutQueue();
  }

  // Если связь с MQTT сервером не установлена - выполнить переподключение к серверу
  // Слишком частая проверка мешает приложению на смартфоне общаться с программой - запрос блокирующий и при неответе MQTT сервера
  // слишком частые попытки подключения к MQTT серверу не дают передаваться данным из / в приложение - приложение "отваливается"
  if (!stopMQTT && !mqtt.connected() && (mqtt_conn_last == 0 || (millis() - mqtt_conn_last > 2500))) {
    if (!mqtt_connecting) {
      DEBUG(F("\nПодключаемся к MQTT-серверу '"));
      DEBUG(mqtt_server);
      DEBUG(":");
      DEBUG(mqtt_port);
      DEBUG(F("'; ClientID -> '"));
      DEBUG(mqtt_client_name);
      DEBUG(F("' ..."));
    }
    mqtt_topic_subscribed = false;
    mqtt_conn_last = millis();

    String topic = mqtt_topic(TOPIC_PWR);
    uint32_t t = millis();

    if (mqtt.connect(mqtt_client_name.c_str(), mqtt_user, mqtt_pass, topic.c_str(), 0, true, "offline")) {
      stopMQTT = false;
      mqtt_connecting = false;      
      DEBUGLN(F("\nПодключение к MQTT-серверу выполнено."));
      if (outQueueLength > 0) {
        DEBUG(F("Сообщений в очереди отправки: "));  
        DEBUGLN(outQueueLength);  
      }
      putOutQueue(topic, "online", true);
    } else {      
      DEBUG(".");
      mqtt_connecting = true;
      mqtt_conn_cnt++;
      if (mqtt_conn_cnt == 80) {
        mqtt_conn_cnt = 0;
        DEBUGLN();
      }
    }

    // Если сервер недоступен и попытка соединения отвалилась по таймауту - следующую попытку подключения осуществлять не ранее чем через минут.
    // Попытка соединения - операция блокирующая и когда сервер недоступен - пауза примерно 18 секунд и все замирает.
    // Пока не знаю как уменьшить таймаут соединения, но так будет все замирать хотя бы раз в минуту
    if (millis() - t > MQTT_CONNECT_TIMEOUT) {
      nextMqttConnectTime = millis() + MQTT_RECONNECT_PERIOD;
    }
  }
}

// Отправка сообщений из очереди на сервер
void processOutQueue() {
  if (stopMQTT) {
    outQueueReadIdx = 0;
    outQueueWriteIdx = 0;
    outQueueLength = 0;
    return;
  }

  if (mqtt.connected() && outQueueLength > 0) {    
    // Топик и содержимое отправляемого сообщения
    String topic = tpcQueue[outQueueReadIdx];
    String message = outQueue[outQueueReadIdx];
    bool   retain = rtnQueue[outQueueReadIdx];
    // Пытаемся отправить. Если инициализация отправки не удалась - возвращается false; Если удалась - true
    bool ok = mqtt.beginPublish(topic.c_str(), message.length(), retain);
    if (ok) {
      // Если инициация отправки была успешной - заполняем буфер отправки передаваемой строкой сообщения
      mqtt.print(message.c_str());
      // Завершаем отправку. Если пакет был отправлен - возвращается 1, если ошибка отправки - возвращается 0
      ok = mqtt.endPublish() == 1;
    }
    if (ok) {      
      // Отправка прошла успешно
      DEBUG(F("MQTT >> OK >> ")); 
      if (retain)
        DEBUG(F("[r] "));
      else   
        DEBUG(F("[ ] "));
      DEBUG(topic);
      DEBUG(F("\t >> ")); 
      DEBUGLN(message);
      // Извлекаем сообщение из очереди
      tpcQueue[outQueueReadIdx] = "";
      outQueue[outQueueReadIdx] = "";
      rtnQueue[outQueueReadIdx] = false;
      outQueueReadIdx++;
      if (outQueueReadIdx >= QSIZE_OUT) outQueueReadIdx = 0;
      outQueueLength--;
    } else {
      // Отправка не удалась
      DEBUG(F("MQTT >> FAIL >> ")); 
      DEBUG(topic);
      DEBUG(F("\t >> ")); 
      DEBUGLN(message);
    }
  }  
}
#else

void SendMQTT(String &message, String topic) {

}

void putOutQueue(String topic, String message) {

}

#endif
