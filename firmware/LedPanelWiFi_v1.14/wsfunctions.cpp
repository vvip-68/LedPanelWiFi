// функции работы с http и вебсокетом

#include "wsfunctions.h"

void wsSrvSendAll(AsyncWebSocket *ws, const JsonVariantConst& data){
    if (!ws->count()) return;   // no client connected, nowhere to send

    size_t length = measureJson(data);
    auto buffer = ws->makeBuffer(length);
    if (!buffer) return;    // not enough mem to send data
    serializeJson(data, reinterpret_cast<char*>(buffer->get()), length);
    ws->textAll(buffer);
}