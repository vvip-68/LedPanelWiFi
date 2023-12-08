// функции работы с http и вебсокетом

#include <ESPAsyncWebServer.h>


/**
 * @brief serialize JsonVarian data directly to WebSocket buffer and send to ALL clients connected to WS Server
 * функция сериализует объект непосредственно в буфер вебсокет-сервера (если достаточно памяти)
 * и не требует создания промежуточной копии сериализованной строки в памяти
 * 
 * @param ws - pointer to ws server instance
 * @param data - json object to send
 */
void wsSrvSendAll(AsyncWebSocket *ws, const JsonVariantConst& data);