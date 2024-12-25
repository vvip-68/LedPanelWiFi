
void allocateLeds() {
  if (leds != nullptr) {
    freeLeds();
  }

  int32_t freeMemory1 = ESP.getFreeHeap();
  int32_t requireMemory = NUM_LEDS * sizeof(CRGB);
  if (leds == nullptr && freeMemory1 <= requireMemory) {
     DEBUG(F("Недостаточно памяти для LEDS: нужно "));
     DEBUG(requireMemory);
     DEBUG(F(" байт, есть "));
     DEBUG(freeMemory1);
     DEBUGLN(F(" байт"));
     return;
  }

  leds = new CRGB[NUM_LEDS];

  //FastLED.addLeds<LED_CHIP, D2, COLOR_ORDER>(leds, 256).setCorrection( TypicalLEDStrip );
  //FastLED.addLeds<LED_CHIP, D3, COLOR_ORDER>(leds, 256, 256).setCorrection( TypicalLEDStrip );

  if (leds != NULL) {
    for (int i = 1; i <= 4; i++) {
      // К сожалению функция FastLED.addLeds() в качестве чипсета, пина подключения и порядка цвета принимает только константы времени компиляции
      // Эти параметры не могут быть заданы переменными. ТО есть настройка LED_CHIP и COLOR_ORDER - в файле a_def_hard.h, LED_PIN - из переменной, но в switch - в каждую строку подставлена константа
      
      bool     isLineUsed = getLedLineUsage(i);
      int8_t   led_pin = getLedLinePin(i);
      int16_t  led_start = getLedLineStartIndex(i);
      int16_t  led_count = getLedLineLength(i);
      int8_t   led_rgb = getLedLineRGB(i);

      EOrder   color_order = COLOR_ORDER;

      switch (led_rgb) {
        case 0:  color_order = GRB; break;
        case 1:  color_order = RGB; break;
        case 2:  color_order = RBG; break;
        case 3:  color_order = GBR; break;
        case 4:  color_order = BRG; break;
        case 5:  color_order = BGR; break;
        default: color_order = GRB; break;
      }

      if (led_start + led_count > NUM_LEDS) {
        led_count = NUM_LEDS - led_start;
      }

      isLineUsed &= led_pin >= 0 && led_start >= 0 && led_start < NUM_LEDS && led_count > 0 && led_start <= NUM_LEDS;
      
      if (isLineUsed) {
        #if defined(ESP8266) 
          // Для NodeMCU и Wemos d1 mini для подключения доступны следующие пины: Dx (GPIO)
          // D0(16), D1(5), D2(4), D3(0), D4(2), D5(14), D6(12), D7(13), D8(15), D9/RX(3), D10/TX(1)
          // D5-D8 - работают, но это аппаратные SPI, которые использует SD-карта 
          // D9/RX, D10/TX - RX/TX - если включен отладочный вывод в COM-порт - лента работать не будет
          switch (led_pin) {
            case D0:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D0, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D0, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D0, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D0, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D0, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D0, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break;               
            case D1:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D1, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D1, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D1, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D1, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D1, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D1, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D2: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D2, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D2, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D2, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D2, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D2, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D2, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D3: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D3, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D3, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D3, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D3, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D3, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D3, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D4:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D4, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D4, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D4, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D4, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D4, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D4, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D5:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D5, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D5, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D5, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D5, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D5, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D5, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D6:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D6, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D6, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D6, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D6, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D6, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D6, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D7: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D7, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D7, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D7, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D7, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D7, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D7, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            case D8:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, D8, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, D8, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, D8, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, D8, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, D8, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, D8, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            // Для вывода сигнала на ленту нельзя использовать выводы RX/TX
            // case  3: /* D9/RX  */ FastLED.addLeds<LED_CHIP, RX, COLOR_ORDER>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
            // case  1: /* D10/TX */ FastLED.addLeds<LED_CHIP, TX, COLOR_ORDER>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
           }
        #endif
        #if defined(ESP32) 
          // Для ESP32 безопасное подключение ленты к следующим пинам GPIO:
          // 1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33
          // 1/TX, 3/RX - - если включен отладочный вывод в COM-порт - лента работать не будет 
          // 21/SDA, 22/SCL - шина I2С - предпочтительна для использования TM1637
          switch (led_pin) {

            #if (CONFIG_IDF_TARGET_ESP32C3)
            case  0:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 0, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 0, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 0, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 0, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 0, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 0, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            #endif
            
            case  1: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 1, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 1, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 1, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 1, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 1, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 1, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case  2: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 2, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 2, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 2, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 2, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 2, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 2, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case  3: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 3, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 3, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 3, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 3, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 3, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 3, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case  4: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 4, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 4, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 4, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 4, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 4, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 4, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case  5: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 5, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 5, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 5, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 5, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 5, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 5, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            // Для ESP32C3 в отличие от остальных определены пины 6,7,8,9,10,20
            #if (CONFIG_IDF_TARGET_ESP32C3)
            case  6: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 6, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 6, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 6, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 6, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 6, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 6, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
              
            case  7:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 7, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 7, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 7, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 7, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 7, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 7, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            
            case  8:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 8, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 8, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 8, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 8, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 8, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 8, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case  9:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 9, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 9, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 9, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 9, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 9, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 9, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 10:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 10, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 10, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 10, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 10, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 10, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 10, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 20: FastLED.addLeds<LED_CHIP, 20, COLOR_ORDER>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break; 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 20, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 20, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 20, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 20, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 20, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 20, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            #endif
            
            // Для ESP32C3 пины 12,13,14,15,16,17,26,33 не определены (они отсутствуют)
            #if !(CONFIG_IDF_TARGET_ESP32C3)
            case 12:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 12, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 12, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 12, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 12, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 12, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 12, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 13:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 13, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 13, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 13, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 13, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 13, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 13, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 14:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 14, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 14, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 14, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 14, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 14, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 14, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 15:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 15, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 15, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 15, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 15, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 15, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 15, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 16:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 16, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 16, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 16, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 16, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 16, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 16, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 17:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 17, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 17, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 17, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 17, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 17, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 17, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
              
            case 26:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 26, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 26, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 26, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 26, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 26, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 26, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break;

            case 33:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 33, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 33, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 33, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 33, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 33, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 33, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            #endif
            
            case 18:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 18, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 18, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 18, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 18, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 18, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 18, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 19:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 19, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 19, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 19, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 19, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 19, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 19, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 21: 
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 21, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 21, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 21, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 21, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 21, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 21, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            
            // Для ESP32S2, ESP32S3, ESP32C3 пины 22,23,25,27,32 не определены (они отсутствуют)
            #if !(CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3  || CONFIG_IDF_TARGET_ESP32C3)
            case 22:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 22, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 22, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 22, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 22, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 22, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 22, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 23:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 23, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 23, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 23, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 23, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 23, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 23, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 25:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 25, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 25, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 25, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 25, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 25, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 25, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 27:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 27, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 27, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 27, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 27, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 27, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 27, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 

            case 32:
              switch (color_order) {
                case RGB: FastLED.addLeds<LED_CHIP, 32, RGB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case RBG: FastLED.addLeds<LED_CHIP, 32, RBG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GBR: FastLED.addLeds<LED_CHIP, 32, GBR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case GRB: FastLED.addLeds<LED_CHIP, 32, GRB>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BRG: FastLED.addLeds<LED_CHIP, 32, BRG>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
                case BGR: FastLED.addLeds<LED_CHIP, 32, BGR>(leds, led_start, led_count).setCorrection( TypicalLEDStrip ); break;
              }
              break; 
            #endif
          }
        #endif
      }
    }
    // Максимальная частота вывода на ленту - 40 Гц
    FastLED.setMaxRefreshRate(40, true);
  }
  
  int32_t freeMemory2 = ESP.getFreeHeap();  
  printMemoryDiff(freeMemory1, freeMemory2, F("Выделение памяти для LEDS   : "));
}

void freeLeds() {
  if (leds == nullptr) return;
  int32_t freeMemory1 = ESP.getFreeHeap();
  delete[] leds;
  leds = nullptr;
  int32_t freeMemory2 = ESP.getFreeHeap();
  printMemoryDiff(freeMemory1, freeMemory2, F("Освобождение памяти для LEDS: "));
}

// Сохранить область экрана поверх которой будет выведена бегущая строка
void saveTextOverlay() {
  // Сохранять только если еще не было сохранения ранее - указатель на буфер оверлея пуст
  if (overlayText != nullptr) return;

  // Высота блока бегущей строки и позиция вывода по Y с учетом диакритических символов вычислена ранее в вызове calcTextRectangle()  
  // Y считается от низа матрицы 
  int16_t overlaySize = (text_overlay_high - text_overlay_low + 1) * pWIDTH;
  overlayText = new CRGB[overlaySize]; 
  if (overlayText == nullptr) return;

  // Ширина вывода текста бегущей строки - вся ширина матрицы
  // Сохранить оверлей в выделенный буфер
  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < pWIDTH; i++) {
    for (uint8_t j = text_overlay_low; j <= text_overlay_high; j++) {
      int16_t pn = getPixelNumber(i,j);
      if (pn >= 0 && pn < NUM_LEDS) {
        overlayText[thisLED] = leds[pn];
      }
      thisLED++;
    }
  }  
}

// Сохранить область экрана поверх которой будут выведены часы
void saveClockOverlay() {
  
  // Сохранять только если еще не было сохранения ранее - указатель на буфер оверлея пуст
  if (overlayClock != nullptr) return;

  // Ширина и Высота блока вывода часов и позиция по X,Y вычислена ранее в вызове calcClockRectangle()  
  // X считается от левого края, Y считается от низа матрицы 

  int16_t overlaySize = clockW * clockH;
  overlayClock = new CRGB[overlaySize]; 
  if (overlayClock == nullptr) return;
  
  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < clockW; i++) {
    for (uint8_t j = 0; j < clockH; j++) {
      int16_t pn = getPixelNumber(wrapX(clockX + i), clockY + j);
      if (pn >= 0 && pn < NUM_LEDS) {
        overlayClock[thisLED] = leds[pn];
      }
      thisLED++;
    }
  } 

}

#if (USE_WEATHER == 1)

// Сохранить область экрана поверх которой будет выведена температура
void saveTemperatureOverlay() {
  // Сохранять только если еще не было сохранения ранее - указатель на буфер оверлея пуст
  if (overlayTemperature != nullptr) return;

  // Ширина и Высота блока вывода температуры и позиция по X,Y вычислена ранее в вызове calcTemperatureRectangle()  
  // X считается от левого края, Y считается от низа матрицы 

  int16_t overlaySize = temperatureW * temperatureH;
  overlayTemperature = new CRGB[overlaySize]; 
  if (overlayTemperature == nullptr) return;
  
  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < temperatureW; i++) {
    for (uint8_t j = 0; j <  temperatureH; j++) {
      int16_t pn = getPixelNumber(wrapX(temperatureX + i), temperatureY + j);
      if (pn >= 0 && pn < NUM_LEDS) {
        overlayTemperature[thisLED] = leds[pn];
      }
      thisLED++;
    }
  }    
}

#endif 

// Сохранить область экрана поверх которой будет выведен календарь
void saveCalendarOverlay() {
  // Сохранять только если еще не было сохранения ранее - указатель на буфер оверлея пуст
  if (overlayCalendar != nullptr) return;
  // Ширина и Высота блока вывода температуры и позиция по X,Y вычислена ранее в вызове calcTemperatureRectangle()  
  // X считается от левого края, Y считается от низа матрицы 

  int16_t overlaySize = calendarW * calendarH;
  overlayCalendar = new CRGB[overlaySize]; 
  if (overlayCalendar == nullptr) return;
  
  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < calendarW; i++) {
    for (uint8_t j = 0; j < calendarH; j++) {
      int16_t pn = getPixelNumber(wrapX(calendarX + i), calendarY + j);
      if (pn >= 0 && pn < NUM_LEDS) {
        overlayCalendar[thisLED] = leds[pn];
      }
      thisLED++;
    }
  }    
}

// Восстановить область экрана поверх которой была выведена бегущая строка
void restoreTextOverlay() {
  // Если указатель не указывает на буфер - восстанавливать нечего
  if (overlayText == nullptr) return;

  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < pWIDTH; i++) {
    for (uint8_t j = text_overlay_low; j <= text_overlay_high; j++) {
      int16_t pn = getPixelNumber(i,j);
      if (pn >= 0 && pn < NUM_LEDS) {
        leds[pn] = overlayText[thisLED];        
      }  
      thisLED++; 
    }
  }
  
  delete[] overlayText;
  overlayText = nullptr;
}

// Восстановить область экрана поверх которой были выведены часы
void restoreClockOverlay() {

  // Если указатель не указывает на буфер - восстанавливать нечего
  if (overlayClock == nullptr) return;

  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < clockW; i++) {
    for (uint8_t j = 0; j < clockH; j++) {
      int16_t pn = getPixelNumber(wrapX(clockX + i), clockY + j);
      if (pn >= 0 && pn < NUM_LEDS) {
        leds[pn] = overlayClock[thisLED];
      }  
      thisLED++; 
    }
  }

  delete[] overlayClock;
  overlayClock = nullptr;

}

// Восстановить область экрана поверх которой была выведена температура
#if (USE_WEATHER == 1)

void restoreTemperatureOverlay() {
  // Если указатель не указывает на буфер - восстанавливать нечего
  if (overlayTemperature == nullptr) return;

  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < temperatureW; i++) {
    for (uint8_t j = 0; j < temperatureH; j++) {
      int16_t pn = getPixelNumber(wrapX(temperatureX + i), temperatureY + j);
      if (pn >= 0 && pn < NUM_LEDS) {
        leds[pn] = overlayTemperature[thisLED];
      }  
      thisLED++; 
    }
  }
  
  delete[] overlayTemperature;
  overlayTemperature = nullptr;
}

#endif

// Восстановить область экрана поверх которой был выведен календарь
void restoreCalendarOverlay() {
  // Если указатель не указывает на буфер - восстанавливать нечего
  if (overlayCalendar == nullptr) return;

  int16_t thisLED = 0;  
  for (uint8_t i = 0; i < calendarW; i++) {
    for (uint8_t j = 0; j < calendarH; j++) {
      int16_t pn = getPixelNumber(wrapX(calendarX + i), calendarY + j);
      if (pn >= 0 && pn < NUM_LEDS) {
        leds[pn] = overlayCalendar[thisLED];
      }  
      thisLED++; 
    }
  }
  
  delete[] overlayCalendar;
  overlayCalendar = nullptr;
}

String getColorOrderName(int8_t rgb_idx) {
  switch (rgb_idx) {
    case 1:  return F("RGB");
    case 2:  return F("RBG");
    case 3:  return F("GBR");
    case 4:  return F("BRG");
    case 5:  return F("BGR");
    default: return F("GRB");
  }
}
