// *************************** Поток / Полосы **************************

// По мотивам предложенного пользователем Zordog

#include "palettes.h"

uint8_t gCurrentGradientPaletteNumber = 0;

CRGBPalette16 gCurrentGradientPalette; 
CRGBPalette16 gTargetGradientPalette;

CRGB draw_color = CRGB::White;

struct CRGB *sav = nullptr;
uint8_t     *buf = nullptr; 

uint8_t  row_idx = 0;        // индекс строки, выводимой из буфера
uint8_t  img_idx = 0;        // 0 - часы; 1 - температура; 2 - календарь
int32_t  show_interval = 0;  // интервал показа изображения в половинках высоты матрицы
int8_t   offsetX = 0;
int8_t   offsetY = 0;
uint8_t  pictureW = 0;
uint8_t  pictureH = 0;

#define PIC_NUM 5;           // 0 - часы; 1 - температура; 2 - календарь Если хотим, чтобы через заданные (короткие) интервалы показадись сущности 0,1,2? а далее какое-то время ничего не показывалось -
                             // ставим тут бОльшее число - пустые эффекты. Когда интервал показа часов / температуры / календаря пройдет - дальше будут "пустые", ничего не выводящие на экран сущности
                             // так при занченимм, 5 будет Часы - Температура - Дата - пусто - пусто - Часы - Темепратура - Дата - пусто - пусто
void prizmata2Routine() {
  processStreamRoutine(1);
}

void shadows2Routine() {
  processStreamRoutine(2);
}

void processStreamRoutine(uint8_t aType) {
  
  if (loadingFlag) {
    // modeCode = MC_PRIZMATA2;
    // modeCode = MC_SHADOWS2;
    loadingFlag = false;
    switch (aType) {
      case 1:
        gCurrentGradientPaletteNumber = random8(gGradientPrizmataPaletteCount);
        gCurrentGradientPalette = gGradientPrizmataPalettes[ gCurrentGradientPaletteNumber ]; // CRGB::Black
        gCurrentGradientPaletteNumber = random8(gGradientPrizmataPaletteCount);
        gTargetGradientPalette = gGradientPrizmataPalettes[ gCurrentGradientPaletteNumber ];
        break;
      case 2:
        gCurrentGradientPaletteNumber = random8(gGradientShadowsPaletteCount);
        gCurrentGradientPalette = gGradientShadowsPalettes[ gCurrentGradientPaletteNumber ]; // CRGB::Black
        gCurrentGradientPaletteNumber = random8(gGradientShadowsPaletteCount);
        gTargetGradientPalette = gGradientShadowsPalettes[ gCurrentGradientPaletteNumber ];
        break;
    }
    
    // Значение параметра может быть от 0 до 255; Если более 252 - выключено    
    // Иначе - первый показ - через два полных сдвигов матрицы; 
    show_interval = (uint8_t)getEffectScaleParamValue2(thisMode);
    show_interval = show_interval > 252 ? -1 : pHEIGHT + map8(show_interval, 1, 16) * pHEIGHT / 2;

    pictureW = 0;
    pictureH = 0;
    row_idx = 0;
    img_idx = 0;
    
    FastLED.clear();
  }

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, getEffectContrastValue(thisMode));
  uint8_t sinbpm = map8(getEffectScaleParamValue(thisMode),1,242);
  
  EVERY_N_SECONDS(12) { chooseNextPalette(aType); }
  EVERY_N_MILLISECONDS(10) { nblendPaletteTowardPalette( gCurrentGradientPalette, gTargetGradientPalette, 12); }

  // Если показ часов/температуры/календаря был выключен (show_interval == -1, проверяется при запуске эффекта), но потом его включили (пfраметр2 <= 252) - включить
  // Проверять на каждой итерации не включили ли?  
  if (show_interval < 0) {
    // Если включили - задать через сколько строк показывать. Если параметр более 252 - все еще выключен показ
    // Если меньше или равер 252 - вычислить количество строк сдвига до показа часов/температуры/календаря - минимально одна высота матрицы, плюс указанное число половинок экрана по высоте
    show_interval = (uint8_t)getEffectScaleParamValue2(thisMode);
    show_interval = show_interval > 252 ? -1 : pHEIGHT + map8(show_interval, 1, 16) * pHEIGHT / 2;
  }

  // Вывода блока изображения нет - уменьшаем счетчик интервала показа.
  // Когда дойдет до нуля - нужно начинать показывать блок часов/температуры/календаря/пустого блока
  // Все эти блоки имеют ненулевую высоту и пока они все не будут выведены на матрицу - уменьшать счетчик интервала показа не нужно
  if (pictureH == 0) show_interval--;
  
  // Пришло время показа блока часов / температуры / календаря 
  if (show_interval == 0) {

    // Интервал показа блоков - в начальное значение. Если параметр более 252 - все еще выключен показ
    // Если меньше или равер 252 - вычислить количество строк сдвига до показа часов/температуры/календаря - минимально одна высота матрицы, плюс указанное число половинок экрана по высоте
    show_interval = (uint8_t)getEffectScaleParamValue2(thisMode);
    show_interval = show_interval > 252 ? -1 : pHEIGHT + map8(show_interval, 1, 16) * pHEIGHT / 2;
    
    // - вычислить позиции элементов рисунка и размер необходимого буфера, сбросить счетчик показа строк буфера рисунка
    row_idx = 0;
    
    // Наличие пустых вариантов кроме Часы/Температура/Календарь - позволит также создавать интервалы между группами показа  Ч-Т-К------Ч-Т-К------Ч-Т-К------Ч-Т-К
    pictureW = 0;    // Для пустых картинок (пропусков, если они есть) задаем фиктивную высоту буфера - 10 строкю Ширина 0 означает что реальный буфер выделять не нужно
    pictureH = 10;   // Это нужно, чтобы работал счетчик вывода строк, буфера, по завершению которого сменялся img_idx и был переход к выводу следующей сущности
    bool found = false;
    
    while (!found) {

      switch (img_idx) {
        case 0: {
            // часы
            if (init_time) {
              calcClockPosition();
              pictureW = clockW;
              pictureH = clockH;
              offsetX = clockX;
              offsetY = clockY;
              found = true;
            } else {
              // Время не инициализировано - перейти к следующему блоку
              img_idx++;
            }
          }
          break;
        
        case 1: {
          #if (USE_WEATHER == 1)  
            if (useWeather > 0 && init_weather && weather_ok) {
              // температура - полностью, с градусом и знаком C/F
              bool tmp_show_tempCF = showTempLetter;
              bool tmp_show_tempDG = showTempDegree;
              showTempLetter = true;
              showTempDegree = true;;
              calcTemperaturePosition();
              pictureW = temperatureW;
              pictureH = temperatureH;
              offsetX = temperatureX;
              offsetY = temperatureY;
              showTempLetter = tmp_show_tempCF;
              showTempDegree = tmp_show_tempDG;
              found = true;
            } else {
              // Погода отключена программно или не получена - перейти к следующему блоку
              img_idx++;
            }
          #else  
            // Погода отключена настройками сборки или не получена - перейти к следующему блоку
            img_idx++;
          #endif
          }
          break;
          
        case 2: {
            // календарь
            if (init_time) {
              calcCalendarPosition();
              pictureW = calendarW;
              pictureH = calendarH;
              offsetX = calendarX;
              offsetY = calendarY;
              found = true;
            } else {
              // Время не инициализировано - перейти к следующему блоку
              img_idx++;
            }
          }
          break;
          
        default:  
            // При других значениях - пока ничего не рисовать. Количество вариантов - в PIC_NUM
            // Может быть далее какие-нибудь короткие надписи, влезающие целиком в ширину экрана ("Yes!", "No!", "Да!", "Нет!" и т.д), шрифт обычно 5x7 + 1 пробел - 6 колонок на буквы -> при ширине 32 точки влезет 5 букв
            // Или обратный отсчет 9-8-7-6-5-4-3-2-1-0...
            found = true;
          break;
      }
    }

    // Если Ширина не равна нулю - требуется реальный буфер под вывод изображения часов / температуры / календаря
    // Высота - всегда не нулевая - высота блока часов/температуры/календаря или 10 для фиктивных пустых блоков
    if (pictureW > 0) {
      // Буфер под верхнюю строку (оверлей)
      sav = new CRGB[pictureW];
      if (sav != nullptr) {
        for(uint8_t x = 0; x < pictureW; x++) {
           int16_t thisPixel = getPixelNumber(offsetX + x, pHEIGHT - 1);
          if (thisPixel >= 0){
            sav[x] = leds[thisPixel];
          }
        }
      }
      
      // Буфер для отрисовки часов / температуры / календаря
      buf = (uint8_t*)malloc(pictureW * pictureH);

      // Если не удалось выделить буферы - блоки поверх эффекта не выводить
      if (buf == nullptr && sav != nullptr) {
        delete[] sav; sav = nullptr;   
      }      
    }
    
    if (buf != nullptr) {

      // Очистить буфер
      memset(buf, 0, pictureW * pictureH);
      
      // - нарисовать рисунок в буфер вывода. Буфер заполнен 0 (это прозрачный). Отрисовка - 1 - непрозрачный    
      switch (img_idx) {
        case 0:
          if (init_time) {
            // часы
            shiftClockPosition(-offsetX, -offsetY);
            drawClock_buf(pictureW, pictureH, buf);
          }
          break;
          
        #if (USE_WEATHER == 1)
        case 1:
          // температура
          if (useWeather > 0 && init_weather && weather_ok) {
            shiftTemperaturePosition(-offsetX, -offsetY);
            drawTemperature_buf(pictureW, pictureH, buf);
          }
          break;
          #endif

        case 2:
          if (init_time) {
            // календарь
            shiftCalendarPosition(-offsetX, -offsetY);
            drawCalendar_buf(pictureW, pictureH, buf);
          }
          break;

        default:
          // Пока тут нечего делать
          break;  
      }
    }
  }

  // Сдвигаем всю матрицу на одну строку вниз
  shiftDown();

  // В верхней строке восстанавливаем пиксели, которые были до наложения сверху часов / температуры / календаря
  // Если не восстанавливать - остается шлейф в тех точках, где после сдвига эффект не нарисовал новые пиксели
  if (sav != nullptr) {
    
    for(uint8_t x = 0; x < pictureW; x++) {
      int16_t thisPixel = getPixelNumber(offsetX + x, pHEIGHT - 1);
      if (thisPixel >= 0){
        leds[thisPixel] = sav[x];
      }
    }
    
    // Если вся картинка из буфера выведена (буфер был освобожден на предыдущей итерации и теперь buf == nullptr), 
    // больше буфер оверлея верхней строки не нужен - также освободить;    
    if (buf == nullptr) {
      delete[] sav; sav = nullptr;      
      pictureW = 0; pictureH = 0;     // Нулевые ширина и высота блока картинки означают, что сейчас нет вывода блока (фаза ожидания)
    }    
    
  }

  // Изменяем узор эффекта
  for (uint8_t x = 0; x < pWIDTH; x++) {
    switch (aType) {
      case 1: {
          // "Поток"
          uint8_t y = beatsin8(sinbpm + x, 0, pWIDTH - 1);
          drawPixelXY(y, pHEIGHT - 1, ColorFromPalette(gCurrentGradientPalette, x * pWIDTH / 2, effectBrightness));
        }
        break;
      case 2: {
          // Тени-2
          drawPixelXY(x, pHEIGHT - 1, ColorFromPalette(gCurrentGradientPalette, beatsin8(sinbpm + x, 0, 242), effectBrightness));
        }
        break;
    }
  }

  // Если буфер создан - в нем отрисованы часы / температура / календарь
  if (buf != nullptr) {

    // Перед наложением - сохраняем в буфер пиксели верхней строки до того как наложили календарь
    if (sav != nullptr) {
      for(uint8_t x = 0; x < pictureW; x++) {
        int16_t thisPixel = getPixelNumber(offsetX + x, pHEIGHT - 1);
        if (thisPixel >= 0){
          sav[x] = leds[thisPixel];
        }
      }
    }
  
    // Рисовать строку вывода буфера часов / температуры / календаря
    int16_t ix = row_idx * pictureW;
    for(uint8_t i = 0; i < pictureW; i++) {
      if (buf[ix + i] > 0) {
        int16_t thisPixel = getPixelNumber(offsetX + i, pHEIGHT - 1);
        if (thisPixel >= 0){                   // Часы / температуру / календарь рисовать
                              
          leds[thisPixel] = -leds[thisPixel];  // - инверсным цветом (??? почему-то все равно рисует белым, хотя FastLED в доках обещал инверсию цвета)
          //leds[thisPixel] = CRGB::White;     // - белым цветом
          //leds[thisPixel] = CRGB::Black;     // - черным цветом
          
        }
      }
    }
  }

  // Счетчик строк буфера увеличиваем даже если реально картинки нет (пропуск) и буфер фиктивен - 
  // иначе после однократного вывода реальной картинки больше ничего показываться не будет 
  if (pictureH > 0) {
    row_idx++;
    if (row_idx >= pictureH) {
      // Если весь буфер часов / температуры / календаря выведен - на этой итерации выведена последняя верхняя строка изображения) - освободить память буфера
      if (buf != nullptr) {
        free(buf); buf = nullptr;
      }
      // Выключаем отсчет строк выводимого блока
      pictureH = 0; row_idx = 0;
      // Переход к следкющему блоку отображения
      img_idx = (img_idx + 1) % PIC_NUM;
    }
  }

}

void chooseNextPalette(uint8_t aType) {
    switch (aType) {
      case 1:
        gCurrentGradientPaletteNumber = random8(gGradientPrizmataPaletteCount);
        gTargetGradientPalette = gGradientPrizmataPalettes[ gCurrentGradientPaletteNumber ];
        break;
      case 2:
        gCurrentGradientPaletteNumber = random8(gGradientShadowsPaletteCount);
        gTargetGradientPalette = gGradientShadowsPalettes[ gCurrentGradientPaletteNumber ];
        break;
    }
}

void prizmata2RoutineRelease() {
  streamRoutineRelease();
}

void shadows2RoutineRelease() {
  streamRoutineRelease();
}

void streamRoutineRelease() {
  if (buf != nullptr) {
    free(buf);
    buf = nullptr;
  }
  if (sav != nullptr) {
    delete[] sav;
    sav = nullptr;
  }
}

// ********************* Фейерверк ******************

// Предложено пользователем Zordog

typedef struct  {
  bool     the_figState;   // true - фигура отображается; false - фигура не отображается
  uint8_t  the_figMode;    // тип фигуры 0 - круг; 1 - снежинка;
  uint8_t  the_figX;
  uint8_t  the_figY;
  uint8_t  the_figSize;    // текущий размер фигуры
  uint8_t  the_figSizeMax; // максимальный размер фигуры
  uint8_t  the_figHue;
  uint16_t the_figStart;
  uint16_t the_figPos;
} Firework;

CRGBPalette16 myRainbow_p = my_rainbow_with_white_gp;
uint8_t  the_figCount = 0; // количество фигур в вейерверке - от 4 до (pWIDTH / 4) * (pHEIGHT / 4)

Firework *firework = NULL;
  
void fireworksRoutine() {

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, getEffectContrastValue(MC_FIREWORKS));
  uint8_t the_sizeMax = 0;

  if      (pWIDTH  > pHEIGHT) the_sizeMax = pWIDTH / 2;
  else if (pHEIGHT > pWIDTH)  the_sizeMax = pHEIGHT / 2;
  else                        the_sizeMax = pWIDTH / 2;
  
  if (loadingFlag) {
    // modeCode = MC_FIREWORKS;

    loadingFlag = false;
    fireworksRoutineRelease();    
    
    the_figCount = map8(getEffectScaleParamValue2(MC_FIREWORKS),4,(pWIDTH / 4) * (pHEIGHT / 4));
    if (firework == NULL) { firework = new Firework[the_figCount]; }
    if (firework == NULL) { 
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }
    
    for (uint8_t i = 0; i < the_figCount; i++ ) {
      firework[i].the_figState   = false;
      firework[i].the_figMode    = (random8(0, 99) % 4) == 0 ? 0 : 1;   // Соотношение круговых фейерверков к звездочным - 1:3
      firework[i].the_figX       = random8(1, pWIDTH - 1);
      firework[i].the_figY       = random8(1, pHEIGHT - 1);
      firework[i].the_figSize    = 0;
      firework[i].the_figSizeMax = random8(the_sizeMax / 2 ,the_sizeMax);
      firework[i].the_figHue     = random8(0, 224);
      firework[i].the_figStart   = random16(0, the_sizeMax * 2);
      firework[i].the_figPos     = 0;
    }

    FastLED.clear();  // очистить
  }

  fadeToBlackBy(leds, NUM_LEDS, map8(getEffectScaleParamValue(MC_FIREWORKS), 10, 128));

  for (uint8_t i = 0; i < the_figCount; i++ ) {

    firework[i].the_figPos++;
    if (!firework[i].the_figState && firework[i].the_figPos >= firework[i].the_figStart) {
      firework[i].the_figState = !firework[i].the_figState;
    }

    if (firework[i].the_figState) {

      uint16_t effectBrightnessFaded = (firework[i].the_figSize - firework[i].the_figSizeMax / 2) * (effectBrightness / firework[i].the_figSizeMax * 2);

      if (effectBrightnessFaded >= effectBrightness) effectBrightnessFaded = effectBrightness;

      if      (firework[i].the_figMode == 0) drawCircleBlend    (firework[i].the_figX, firework[i].the_figY, firework[i].the_figSize, ColorFromPalette( myRainbow_p, firework[i].the_figHue, (firework[i].the_figSize > firework[i].the_figSizeMax / 2) ? effectBrightness - effectBrightnessFaded : effectBrightness, LINEARBLEND_NOWRAP));
      else if (firework[i].the_figMode == 1) drawSnowflakesBlend(firework[i].the_figX, firework[i].the_figY, firework[i].the_figSize, ColorFromPalette( myRainbow_p, firework[i].the_figHue, (firework[i].the_figSize > firework[i].the_figSizeMax / 2) ? effectBrightness - effectBrightnessFaded : effectBrightness, LINEARBLEND_NOWRAP));

      firework[i].the_figSize++;
      if (firework[i].the_figSize > firework[i].the_figSizeMax) {
        firework[i].the_figStart   = random16(0, the_sizeMax * 2);
        firework[i].the_figMode    = (random8(0, 99) % 4) == 0 ? 0 : 1;   // Соотношение круговых фейерверков к звездочным - 1:3
        firework[i].the_figX       = random8(1, pWIDTH - 1);
        firework[i].the_figY       = random8(1, pHEIGHT - 1);
        firework[i].the_figSize    = 0;
        firework[i].the_figSizeMax = random8(the_sizeMax/2,the_sizeMax);
        firework[i].the_figHue     = random8(0, 224);
        firework[i].the_figPos     = 0;         
        firework[i].the_figState   = !firework[i].the_figState;
      }
    }
    
  }
  
}

void fireworksRoutineRelease() {

  if (firework != NULL) { free(firework); firework = NULL; }

}

void drawSnowflakesBlend(int x0, int y0, int figSize, CRGB color) {

  int16_t pn;
  
  if (figSize == 0) {
    pn = getPixelNumber(x0, y0); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    return;
  }

  pn = getPixelNumber(x0, y0 - figSize); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color,128); // луч вверх
  pn = getPixelNumber(x0 + figSize, y0); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color,128); // луч вправо
  pn = getPixelNumber(x0, y0 + figSize); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color,128); // луч вниз
  pn = getPixelNumber(x0 - figSize, y0); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color,128); // луч влево

  if (figSize > 1) {

    figSize--;
    pn = getPixelNumber(x0, y0 - figSize);           if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вверх
    pn = getPixelNumber(x0 + figSize, y0);           if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вправо
    pn = getPixelNumber(x0, y0 + figSize);           if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вниз
    pn = getPixelNumber(x0 - figSize, y0);           if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч влево
    pn = getPixelNumber(x0 - figSize, y0 - figSize); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вверх & влево
    pn = getPixelNumber(x0 + figSize, y0 - figSize); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вверх & вправо
    pn = getPixelNumber(x0 + figSize, y0 + figSize); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вниз & вправо
    pn = getPixelNumber(x0 - figSize, y0 + figSize); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128); // луч вниз & влево

  }
}

void drawCircleBlend(int x0, int y0, int figSize, CRGB color) {

  int16_t pn;
  int a = figSize, b = 0;
  int radiusError = 1 - a;

  if (figSize == 0) {
    pn = getPixelNumber(x0, y0); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    return;
  }

  while (a >= b) {

    pn = getPixelNumber(a + x0, b + y0);   if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(b + x0, a + y0);   if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(-a + x0, b + y0);  if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(-b + x0, a + y0);  if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(-a + x0, -b + y0); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(-b + x0, -a + y0); if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(a + x0, -b + y0);  if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);
    pn = getPixelNumber(b + x0, -a + y0);  if (pn >= 0 && pn < NUM_LEDS) nblend(leds[pn], color, 128);

    b++;
    if (radiusError < 0) {
      radiusError += 2 * b + 1;
    } else {
      a--;
      radiusError += 2 * (b - a + 1);
    }

  }
}
