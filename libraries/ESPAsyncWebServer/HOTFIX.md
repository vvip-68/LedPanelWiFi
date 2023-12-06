### Fixes merged in this branch and (still) missing in upstream

 - make AsyncWebServerResponse header size buff build-time redefinable via ASYNC_RESPONSE_HDR_BUFF_SIZE
 - Passing _fs by value will make a copy of fs::FS object, instead of using existing SPIFFS object [#1039](https://github.com/me-no-dev/ESPAsyncWebServer/pull/1040)
 - Fix macro invocation with post-increment index as argument [#1125](https://github.com/me-no-dev/ESPAsyncWebServer/pull/1125)
 - Frame number was not initialised in AsyncWebSocketClient. [#829](https://github.com/me-no-dev/ESPAsyncWebServer/pull/829)
 - Handle edge-case in which we receive a partial WS header [#953](https://github.com/me-no-dev/ESPAsyncWebServer/pull/953)
 - Make LinkedList iterator safe against removal of cur item, fix crashes on serving static files and
   websocket buff releases *(critical for esp32-c3)* [#952](https://github.com/me-no-dev/ESPAsyncWebServer/pull/952)
 - src/AsyncWebSocket.cpp return type error, fix build error for esp32-c3 [#970](https://github.com/me-no-dev/ESPAsyncWebServer/pull/970)

