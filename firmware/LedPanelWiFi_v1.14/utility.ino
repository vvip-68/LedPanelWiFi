// служебные функции

// шрифт 5х7
const uint8_t font5x7[][5] PROGMEM = {
  // Вариант 1.
//{0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 0x30 48 - 0 перечеркнутый
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 0 0x30 48 - 0 круглый
  {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1 0x31 49
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 0x32 50
  {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3 0x33 51
  {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4 0x34 52
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 0x35 53
  {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6 0x36 54
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 0x37 55
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 0x38 56
  {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9 0x39 57
  // Вариант 2                                  //  В папке проекта tools есть утилита FontMaker.
//{0x3E, 0x41, 0x41, 0x41, 0x3E}, // 0 0x30 48  //  С её помошью можно отредактировать начертания цифр по вашему вкусу.
//{0x00, 0x42, 0x7f, 0x40, 0x00}, // 1 0x31 49
//{0x42, 0x61, 0x51, 0x49, 0x46}, // 2 0x32 50
//{0x21, 0x41, 0x49, 0x4d, 0x37}, // 3 0x33 51
//{0x0F, 0x10, 0x10, 0x7F, 0x10}, // 4 0x34 52
//{0x47, 0x49, 0x49, 0x49, 0x31}, // 5 0x35 53
//{0x3e, 0x41, 0x49, 0x49, 0x30}, // 6 0x36 54
//{0x01, 0x71, 0x09, 0x05, 0x03}, // 7 0x37 55
//{0x36, 0x49, 0x49, 0x49, 0x36}, // 8 0x38 56
//{0x06, 0x49, 0x49, 0x41, 0x3E}, // 9 0x39 57 
};

// шрифт 3х5 квадратный
const uint8_t font3x5[][3] PROGMEM = {
//{0b11111, 0b10001, 0b11111},    // 0
//{0b00000, 0b11111, 0b00000},    // 1
//{0b10111, 0b10101, 0b11101},    // 2
//{0b10101, 0b10101, 0b11111},    // 3
//{0b11100, 0b00100, 0b11111},    // 4
//{0b11101, 0b10101, 0b10111},    // 5
//{0b11111, 0b10101, 0b10111},    // 6
//{0b10000, 0b10000, 0b11111},    // 7
//{0b11111, 0b10101, 0b11111},    // 8
//{0b11101, 0b10101, 0b11111},    // 9
  {0x1F, 0x11, 0x1F},             // 0
  {0x00, 0x1F, 0x00},             // 1
  {0x17, 0x15, 0x1D},             // 2
  {0x11, 0x15, 0x1F},             // 3
  {0x1C, 0x04, 0x1F},             // 4
  {0x1D, 0x15, 0x17},             // 5
  {0x1F, 0x15, 0x17},             // 6
  {0x10, 0x10, 0x1F},             // 7
  {0x1F, 0x15, 0x1F},             // 8
  {0x1D, 0x15, 0x1F},             // 9
};

// шрифт 3х5 скруглённый
const uint8_t font3x5_s[][3] PROGMEM = {
//{0b01111, 0b10001, 0b11110},    // 0
//{0b01000, 0b11111, 0b00000},    // 1
//{0b10011, 0b10101, 0b01001},    // 2
//{0b10001, 0b10101, 0b11111},    // 3
//{0b11100, 0b00100, 0b11111},    // 4
//{0b11101, 0b10101, 0b10111},    // 5
//{0b01111, 0b10101, 0b10111},    // 6
//{0b10000, 0b10011, 0b11100},    // 7
//{0b11111, 0b10101, 0b11111},    // 8
//{0b11101, 0b10101, 0b11110},    // 9
  {0x0E, 0x11, 0x0E},             // 0
  {0x08, 0x1F, 0x00},             // 1
  {0x13, 0x15, 0x09},             // 2
  {0x11, 0x15, 0x0A},             // 3
  {0x18, 0x04, 0x1F},             // 4
  {0x1D, 0x15, 0x16},             // 5
  {0x0F, 0x15, 0x17},             // 6
  {0x10, 0x13, 0x1C},             // 7
  {0x0A, 0x15, 0x0A},             // 8  {0x0F, 0x15, 0x1E}
  {0x1D, 0x15, 0x1E},             // 9
};

// отображаем счёт для игр
void displayScore(uint16_t score) {
  uint8_t score_size = SCORE_SIZE;
  uint8_t score_width = 0, num_one = 0, X = 0, Y = 0;
  String  str(score);
  uint8_t str_len = str.length();
  char c;

  // Считаем сколько '1' в счете. '1' имеет более узкое знакоместо в шрифте - 5x7 - 3 колонки, 3х5 - 1 колонку
  for (uint8_t i=0; i<str.length(); i++) {
    if (str[i] == '1') num_one++; 
  }
  
  if (score_size == 1) {
    // Выбран шрифт 5x7/ Вычисляем сколько нужно для отображения счета
    score_width = str_len * 5 + (str_len - 1) - num_one * 2;   // кол-во цифр 5x7 плюс пробелы между цифрами минус коррекция ширины '1'
    if (score_width > pWIDTH) score_size = 0;
    if (score_size == 1) {
      // Позиция начала вывода счета на матрицу
      X = (pWIDTH - score_width) / 2;
      Y = pHEIGHT / 2 - 4;
      for (uint8_t i=0; i<str.length(); i++) {
        c = str[i];
        drawDigit5x7((c - '0'), X, Y, GLOBAL_COLOR_2);
        if (c == '1') X += 4; else X += 6; 
      }
    }
  }

  if (score_size == 0) {
    score_width = str_len * 3 + (str_len - 1) - num_one * 2;   // кол-во цифр 3x5 плюс пробелы между цифрами минус коррекция ширины '1'
    while (score_width > pWIDTH) {
      // Ширина цифр счета не вмещается в матрицу - удалить левую цифру счета 
      str = str.substring(1);
      str_len = str.length();
      score_width = str_len * 3 + (str_len - 1) - num_one * 2;   // кол-во цифр 3x5 плюс пробелы между цифрами минус коррекция ширины '1'
    }
    
    if (score_size == 0) {
      // Позиция начала вывода счета на матрицу
      X = (pWIDTH - score_width) / 2;
      Y = pHEIGHT / 2 - 3;
      for (uint8_t i=0; i<str.length(); i++) {
        c = str[i];
        drawDigit3x5((c - '0'), X, Y, GLOBAL_COLOR_2);
        if (c == '1') X += 2; else X += 4; 
      }
    }
  }
}

// нарисовать цифру шрифт 3х5 квадратный
void drawDigit3x5(uint8_t digit, int8_t X, int8_t Y, CRGB color) {
  if (digit > 9) return;
  for (uint8_t x = 0; x < 3; x++) {
    uint8_t thisByte = pgm_read_byte(&(font3x5[digit][x]));
    for (uint8_t y = 0; y < 5; y++) {
      if (y + Y > pHEIGHT) continue;
      if (thisByte & (1 << y)) drawPixelXY(getClockX(x + X), y + Y, color);
    }
  }
}

// нарисовать цифру шрифт 3х5 скруглённый
void drawDigit3x5_s(uint8_t digit, int8_t X, int8_t Y, CRGB color) {
  if (digit > 9) return;
  for (uint8_t x = 0; x < 3; x++) {
    uint8_t thisByte = pgm_read_byte(&(font3x5_s[digit][x]));
    for (uint8_t y = 0; y < 5; y++) {
      if (y + Y > pHEIGHT) continue;
      if (thisByte & (1 << y)) drawPixelXY(getClockX(x + X), y + Y, color);
    }
  }
}

// нарисовать цифру шрифт 5х7
void drawDigit5x7(uint8_t digit, uint8_t X, uint8_t Y, CRGB color) {
  if (digit > 9) return;
  for (uint8_t x = 0; x < 5; x++) {
    uint8_t thisByte = pgm_read_byte(&(font5x7[digit][x]));
    for (uint8_t y = 0; y < 7; y++) {
      if (y + Y > pHEIGHT) continue;
      if (thisByte & (1 << (6 - y))) drawPixelXY(getClockX(x + X), y + Y, color);
    }
  }
}

// Заглушка чтения кнопок управления игрой
bool checkButtons() {
  if (buttons != 4) return true;
  return false;
}

void fillAll(uint32_t color) {
  if (leds == nullptr) return;
  fill_solid(leds, NUM_LEDS, color);
}

// залить все
void fillAll(CRGB color) {
  if (leds == nullptr) return;
  fill_solid(leds, NUM_LEDS, color);
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int16_t thisPixel) {
  if (leds == nullptr) return 0;
  if (thisPixel < 0 || thisPixel > NUM_LEDS - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b);
}

// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixColorXY(int8_t x, int8_t y) {
  return getPixColor(getPixelNumber(x, y));
}

// функция отрисовки точки по координатам X Y
void drawPixelXY(int8_t x, int8_t y, CRGB color) {
  if (leds == nullptr) return;
  if (x < 0 || x > pWIDTH - 1 || y < 0 || y > pHEIGHT - 1) return;
  int16_t thisPixel = getPixelNumber(x, y);
  if (thisPixel >= 0) leds[thisPixel] = color;
}

/*
void drawPixelXYF(float x, float y, CRGB color) {
  if (leds == nullptr) return;
  // extract the fractional parts and derive their inverses
  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx, iy = 255 - yy;
  
  // calculate the intensities for each affected pixel
  #define WU_WEIGHT(a,b) ((uint8_t) (((a)*(b)+(a)+(b))>>8))

  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy),
                   WU_WEIGHT(ix, yy), WU_WEIGHT(xx, yy)};

  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = getPixColorXY(xn, yn);
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    drawPixelXY(xn, yn, clr);
  }
}

void drawLine(int x1, int y1, int x2, int y2, CRGB color) {
  int deltaX = abs(x2 - x1);
  int deltaY = abs(y2 - y1);
  int signX = x1 < x2 ? 1 : -1;
  int signY = y1 < y2 ? 1 : -1;
  int error = deltaX - deltaY;

  drawPixelXY(x2, y2, color);
  while (x1 != x2 || y1 != y2) {
      drawPixelXY(x1, y1, color);
      int error2 = error * 2;
      if (error2 > -deltaY) {
          error -= deltaY;
          x1 += signX;
      }
      if (error2 < deltaX) {
          error += deltaX;
          y1 += signY;
      }
  }
}

void drawLineF(float x1, float y1, float x2, float y2, CRGB color){
  float deltaX = std::abs(x2 - x1);
  float deltaY = std::abs(y2 - y1);
  float error = deltaX - deltaY;

  float signX = x1 < x2 ? 0.5 : -0.5;
  float signY = y1 < y2 ? 0.5 : -0.5;

  while (true) {
      if ((signX > 0 && x1 > x2+signX) || (signX < 0 && x1 < x2+signX)) break;
      if ((signY > 0 && y1 > y2+signY) || (signY < 0 && y1 < y2+signY)) break;
      drawPixelXYF(x1, y1, color);
      float error2 = error;
      if (error2 > -deltaY) {
          error -= deltaY;
          x1 += signX;
      }
      if (error2 < deltaX) {
          error += deltaX;
          y1 += signY;
      }
  }
}

void drawCircle(int x0, int y0, int radius, CRGB color){
  int a = radius, b = 0;
  int radiusError = 1 - a;

  if (radius == 0) {
    drawPixelXY(x0, y0, color);
    return;
  }

  while (a >= b)  {
    drawPixelXY(a + x0, b + y0, color);
    drawPixelXY(b + x0, a + y0, color);
    drawPixelXY(-a + x0, b + y0, color);
    drawPixelXY(-b + x0, a + y0, color);
    drawPixelXY(-a + x0, -b + y0, color);
    drawPixelXY(-b + x0, -a + y0, color);
    drawPixelXY(a + x0, -b + y0, color);
    drawPixelXY(b + x0, -a + y0, color);
    b++;
    if (radiusError < 0) {
      radiusError += 2 * b + 1;
    } else {
      a--;
      radiusError += 2 * (b - a + 1);
    }
  }
}

void drawCircleF(float x0, float y0, float radius, CRGB color) {
  float x = 0, y = radius, error = 0;
  float delta = 1 - 2 * radius;

  while (y >= 0) {
    drawPixelXYF(x0 + x, y0 + y, color);
    drawPixelXYF(x0 + x, y0 - y, color);
    drawPixelXYF(x0 - x, y0 + y, color);
    drawPixelXYF(x0 - x, y0 - y, color);
    error = 2 * (delta + y) - 1;
    if (delta < 0 && error <= 0) {
      ++x;
      delta += 2 * x + 1;
      continue;
    }
    error = 2 * (delta - x) - 1;
    if (delta > 0 && error > 0) {
      --y;
      delta += 1 - 2 * y;
      continue;
    }
    ++x;
    delta += 2 * (x - y);
    --y;
  }
}
*/

// получить номер пикселя в ленте по координатам
int16_t getPixelNumber(int8_t x, int8_t y) {

  if (x < 0 || x >= pWIDTH || y < 0 || y >= pHEIGHT) return -1;
  
  uint8_t xx,yy,ww,sx,sy,mx,my,mw,snum,num;

  if (sMATRIX_TYPE == 0 || sMATRIX_TYPE == 1) {
    
    // --------------------------------------------------
    // Матрица состоит из одного сегмента, адресация по углу подключения, типу, направлению
    // --------------------------------------------------

    if (mWIDTH == 1 && mHEIGHT == 1) {
      xx = THIS_X(x, y);
      yy = THIS_Y(x, y);
      ww = THIS_W();
    
      return (yy % 2 == 0 || sMATRIX_TYPE == 1)
          ? yy * ww + xx // если чётная строка
          : yy * ww + ww - xx - 1; // если нечётная строка
    }

    // --------------------------------------------------
    // Матрица - сборная, состоит из нескольких сегментов, адресация по углу подключения, типу, направлению
    // --------------------------------------------------
    
    sx = x / sWIDTH;    // номер сегмента по ширине
    sy = y / sHEIGHT;   // номер сегмента по высоте
    x = x % sWIDTH;     // позиция x относительно сегмента
    y = y % sHEIGHT;    // позиция y относительно сегмента
  
    // Номер сегмента в последовательности сборной матрицы
    mx = THIS_SX(sx, sy);
    my = THIS_SY(sx, sy);
    mw = THIS_SW();
  
    snum = (my % 2 == 0 || mTYPE == 1)
        ? my * mw + mx           // если чётная строка мета-матрицы
        : my * mw + mw - mx - 1; // если нечётная строка мета-матрицы
    
    // Номер дода в сегменте
    xx = THIS_X(x, y);
    yy = THIS_Y(x, y);
    ww = THIS_W();
    
    num = (yy % 2 == 0 || sMATRIX_TYPE == 1)
        ? yy * ww + xx           // если чётная строка 
        : yy * ww + ww - xx - 1; // если нечётная строка  
  
    return snum * sWIDTH * sHEIGHT + num;
  }

  if (sMATRIX_TYPE == 2) {
    int16_t index = -1;
    int16_t arrIndex = x + (pHEIGHT - y - 1) * pWIDTH;
    // --------------------------------------------------
    // Адресация диодов с сипользованием карты индексов
    // --------------------------------------------------
    #if (MATRIX_INDEX == 1)
      // Карта индексов загружена из файла в массив int16_t* imap[]      
      if (imap != NULL && x >= 0 && x < pWIDTH && y >= 0 && y < pHEIGHT && arrIndex < NUM_LEDS) {        
        index = imap[arrIndex];
      }
    #else
      // Карта индексов размещена в скетче в файле index_map.ino как массив const int16_t PROGMEM imap[]
      if (x >= 0 && x < pWIDTH && y >= 0 && y < pHEIGHT && arrIndex < NUM_LEDS) {
        index = pgm_read_word(&(imap[arrIndex]));
      }
    #endif
    return index;
  }
  
  return -1;
}

uint8_t THIS_X(uint8_t x, uint8_t y) {
    /*
        CONNECTION_ANGLE; // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
        STRIP_DIRECTION;  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
     */
    if (sCONNECTION_ANGLE == 0 && sSTRIP_DIRECTION == 0) return x;
    if (sCONNECTION_ANGLE == 0 && sSTRIP_DIRECTION == 1) return y;
    if (sCONNECTION_ANGLE == 1 && sSTRIP_DIRECTION == 0) return x;
    if (sCONNECTION_ANGLE == 1 && sSTRIP_DIRECTION == 3) return sHEIGHT - y - 1;
    if (sCONNECTION_ANGLE == 2 && sSTRIP_DIRECTION == 2) return sWIDTH - x - 1;
    if (sCONNECTION_ANGLE == 2 && sSTRIP_DIRECTION == 3) return sHEIGHT - y - 1;
    if (sCONNECTION_ANGLE == 3 && sSTRIP_DIRECTION == 2) return sWIDTH - x - 1;
    if (sCONNECTION_ANGLE == 3 && sSTRIP_DIRECTION == 1) return y;
    return x;
}

uint8_t THIS_Y(uint8_t x, uint8_t y) {
    /*
        CONNECTION_ANGLE; // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
        STRIP_DIRECTION;  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
     */
    if (sCONNECTION_ANGLE == 0 && sSTRIP_DIRECTION == 0) return y;
    if (sCONNECTION_ANGLE == 0 && sSTRIP_DIRECTION == 1) return x;
    if (sCONNECTION_ANGLE == 1 && sSTRIP_DIRECTION == 0) return sHEIGHT - y - 1;
    if (sCONNECTION_ANGLE == 1 && sSTRIP_DIRECTION == 3) return x;
    if (sCONNECTION_ANGLE == 2 && sSTRIP_DIRECTION == 2) return sHEIGHT - y - 1;
    if (sCONNECTION_ANGLE == 2 && sSTRIP_DIRECTION == 3) return sWIDTH - x - 1;
    if (sCONNECTION_ANGLE == 3 && sSTRIP_DIRECTION == 2) return y;
    if (sCONNECTION_ANGLE == 3 && sSTRIP_DIRECTION == 1) return sWIDTH - x - 1;
    return y;
}

uint8_t THIS_W() {
    /*
        CONNECTION_ANGLE; // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
        STRIP_DIRECTION;  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
     */
    if (sCONNECTION_ANGLE == 0 && sSTRIP_DIRECTION == 0) return sWIDTH;
    if (sCONNECTION_ANGLE == 0 && sSTRIP_DIRECTION == 1) return sHEIGHT;
    if (sCONNECTION_ANGLE == 1 && sSTRIP_DIRECTION == 0) return sWIDTH;
    if (sCONNECTION_ANGLE == 1 && sSTRIP_DIRECTION == 3) return sHEIGHT;
    if (sCONNECTION_ANGLE == 2 && sSTRIP_DIRECTION == 2) return sWIDTH;
    if (sCONNECTION_ANGLE == 2 && sSTRIP_DIRECTION == 3) return sHEIGHT;
    if (sCONNECTION_ANGLE == 3 && sSTRIP_DIRECTION == 2) return sWIDTH;
    if (sCONNECTION_ANGLE == 3 && sSTRIP_DIRECTION == 1) return sHEIGHT;
    return sWIDTH;
}

uint8_t THIS_SX(uint8_t x, uint8_t y) {
    /*
        CONNECTION_ANGLE; // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
        STRIP_DIRECTION;  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
     */
    if (mANGLE == 0 && mDIRECTION == 0) return x;
    if (mANGLE == 0 && mDIRECTION == 1) return y;
    if (mANGLE == 1 && mDIRECTION == 0) return x;
    if (mANGLE == 1 && mDIRECTION == 3) return mHEIGHT - y - 1;
    if (mANGLE == 2 && mDIRECTION == 2) return mWIDTH - x - 1;
    if (mANGLE == 2 && mDIRECTION == 3) return mHEIGHT - y - 1;
    if (mANGLE == 3 && mDIRECTION == 2) return mWIDTH - x - 1;
    if (mANGLE == 3 && mDIRECTION == 1) return y;
    return x;
}

uint8_t THIS_SY(uint8_t x, uint8_t y) {
    /*
        CONNECTION_ANGLE; // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
        STRIP_DIRECTION;  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
     */
    if (mANGLE == 0 && mDIRECTION == 0) return y;
    if (mANGLE == 0 && mDIRECTION == 1) return x;
    if (mANGLE == 1 && mDIRECTION == 0) return mHEIGHT - y - 1;
    if (mANGLE == 1 && mDIRECTION == 3) return x;
    if (mANGLE == 2 && mDIRECTION == 2) return mHEIGHT - y - 1;
    if (mANGLE == 2 && mDIRECTION == 3) return mWIDTH - x - 1;
    if (mANGLE == 3 && mDIRECTION == 2) return y;
    if (mANGLE == 3 && mDIRECTION == 1) return mWIDTH - x - 1;
    return y;
}

uint8_t THIS_SW() {
    /*
        CONNECTION_ANGLE; // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
        STRIP_DIRECTION;  // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
     */
    if (mANGLE == 0 && mDIRECTION == 0) return mWIDTH;
    if (mANGLE == 0 && mDIRECTION == 1) return mHEIGHT;
    if (mANGLE == 1 && mDIRECTION == 0) return mWIDTH;
    if (mANGLE == 1 && mDIRECTION == 3) return mHEIGHT;
    if (mANGLE == 2 && mDIRECTION == 2) return mWIDTH;
    if (mANGLE == 2 && mDIRECTION == 3) return mHEIGHT;
    if (mANGLE == 3 && mDIRECTION == 2) return mWIDTH;
    if (mANGLE == 3 && mDIRECTION == 1) return mHEIGHT;
    return mWIDTH;
}

// функция плавного угасания цвета для всех пикселей
void fader(uint8_t step) {
  for (uint8_t i = 0; i < pWIDTH; i++) {
    for (uint8_t j = 0; j < pHEIGHT; j++) {
      fadePixel(i, j, step);
    }
  }
}

void fadePixel(uint8_t i, uint8_t j, uint8_t step) {     // новый фейдер
  if (leds == nullptr) return;
  int16_t pixelNum = getPixelNumber(i, j);
  if (pixelNum < 0 || getPixColor(pixelNum) == 0) return;

  if (leds[pixelNum].r >= 8 ||
      leds[pixelNum].g >= 8 ||
      leds[pixelNum].b >= 8) {
    leds[pixelNum].fadeToBlackBy(step);
  } else {
    leds[pixelNum] = 0;
  }
}

// hex string to uint32_t
uint32_t HEXtoInt(const String& sValue) {

  String hexValue(sValue);
  hexValue.toUpperCase();
  if (hexValue.charAt(0) == '#') {
    hexValue.replace('#',' ');
    hexValue.trim();
  }

  if (hexValue.startsWith("0X")) {
    hexValue.replace("0X"," ");
    hexValue.trim();
  }

  uint8_t tens, ones, number1, number2, number3;
  tens = (hexValue[0] <= '9') ? hexValue[0] - '0' : hexValue[0] - '7';
  ones = (hexValue[1] <= '9') ? hexValue[1] - '0' : hexValue[1] - '7';
  number1 = (16 * tens) + ones;

  tens = (hexValue[2] <= '9') ? hexValue[2] - '0' : hexValue[2] - '7';
  ones = (hexValue[3] <= '9') ? hexValue[3] - '0' : hexValue[3] - '7';
  number2 = (16 * tens) + ones;

  tens = (hexValue[4] <= '9') ? hexValue[4] - '0' : hexValue[4] - '7';
  ones = (hexValue[5] <= '9') ? hexValue[5] - '0' : hexValue[5] - '7';
  number3 = (16 * tens) + ones;

  return ((uint32_t)number1 << 16 | (uint32_t)number2 << 8 | number3 << 0);
}

// uint32_t to Hex string
String IntToHex(uint32_t value) {
  String sHex("00000");  
  sHex.reserve(8);
  sHex += String(value, HEX);
  uint8_t len = sHex.length();
  if (len > 6) {
    sHex = sHex.substring(len - 6);
    sHex.toUpperCase();
  }
  return sHex;
}

String IntToHex(uint32_t value, uint8_t n) {
  String sHex;
  sHex.reserve(8);
  for(uint8_t i=0; i<n; i++) sHex += "0";
  sHex += String(value, HEX);
  uint8_t len = sHex.length();
  if (len > n) {
    sHex = sHex.substring(len - n);
    sHex.toUpperCase();
  }
  return sHex;
}

uint32_t CountTokens(const String &str, char separator) {

  uint32_t count = 0;
  int16_t  pos = 0;
  String l_str(str);

  l_str.trim();
  
  if (l_str.length() <= 0) return 0;
  pos = l_str.indexOf(separator);
  
  while (pos >= 0) {
    count++;
    pos = l_str.indexOf(separator, pos + 1);
  }
  
  return ++count;
}

String GetToken(const String& str, uint32_t index, char separator) {

  uint32_t count = CountTokens(str, separator);

  if (index < 1 || index > count) return str;
  if (count == 1) return str;

  uint32_t pos_start = 0;
  uint32_t pos_end = str.length();

  count = 0;
  for (uint32_t i = 0; i < pos_end; i++) {
    if (str.charAt(i) == separator) {
      count++;
      if (count == index) {
        pos_end = i;
        break;
      } else {
        pos_start = i + 1;
      }
    }
  }
  
  return str.substring(pos_start, pos_end);
}

uint32_t getColorInt(CRGB color) {
  return color.r << 16 | color.g << 8 | color.b;
}

// Вычисление значения яркости эффектов по отношению к общей яркости
// Общая яркость регулируется через FastLED.setBrightness(); 
// Вычисленная яркость эффекта влияет на компонент яркости V в модели HCSV
uint8_t getBrightnessCalculated([[maybe_unused]] uint8_t brightness, uint8_t contrast) {
  // В данном варианте общая яркость не учитывается в расчете
  return map8(contrast, 16, 255);
}

String getMonthString(uint8_t month) {
  String sMnth;
  switch (month) {
    case  1: sMnth = FPSTR(SMonth_01); break;
    case  2: sMnth = FPSTR(SMonth_02); break;
    case  3: sMnth = FPSTR(SMonth_03); break;
    case  4: sMnth = FPSTR(SMonth_04); break;
    case  5: sMnth = FPSTR(SMonth_05); break;
    case  6: sMnth = FPSTR(SMonth_06); break;
    case  7: sMnth = FPSTR(SMonth_07); break;
    case  8: sMnth = FPSTR(SMonth_08); break;
    case  9: sMnth = FPSTR(SMonth_09); break;
    case 10: sMnth = FPSTR(SMonth_10); break;
    case 11: sMnth = FPSTR(SMonth_11); break;
    case 12: sMnth = FPSTR(SMonth_12); break;
    default: sMnth = "";                break;
  }  
  return sMnth;
}

String getMonthShortString(uint8_t month) {
  String sMnth;
  switch (month) {
    case  1: sMnth = FPSTR(SMnth_01); break;
    case  2: sMnth = FPSTR(SMnth_02); break;
    case  3: sMnth = FPSTR(SMnth_03); break;
    case  4: sMnth = FPSTR(SMnth_04); break;
    case  5: sMnth = FPSTR(SMnth_05); break;
    case  6: sMnth = FPSTR(SMnth_06); break;
    case  7: sMnth = FPSTR(SMnth_07); break;
    case  8: sMnth = FPSTR(SMnth_08); break;
    case  9: sMnth = FPSTR(SMnth_09); break;
    case 10: sMnth = FPSTR(SMnth_10); break;
    case 11: sMnth = FPSTR(SMnth_11); break;
    case 12: sMnth = FPSTR(SMnth_12); break;
    default: sMnth = "";               break;
  }  
  return sMnth;
}

String getWeekdayString(uint8_t wd) {
  String str;
  switch (wd) {
    case  1: str = FPSTR(SDayFull_1); break;
    case  2: str = FPSTR(SDayFull_2); break;
    case  3: str = FPSTR(SDayFull_3); break;
    case  4: str = FPSTR(SDayFull_4); break;
    case  5: str = FPSTR(SDayFull_5); break;
    case  6: str = FPSTR(SDayFull_6); break;
    case  7: str = FPSTR(SDayFull_7); break;
    default: str = "";                 break;
  }  
  return str;
}

String getWeekdayShortString(uint8_t wd) {
  String str;
  switch (wd) {
    case  1: str = FPSTR(SDayShort_1); break;
    case  2: str = FPSTR(SDayShort_2); break;
    case  3: str = FPSTR(SDayShort_3); break;
    case  4: str = FPSTR(SDayShort_4); break;
    case  5: str = FPSTR(SDayShort_5); break;
    case  6: str = FPSTR(SDayShort_6); break;
    case  7: str = FPSTR(SDayShort_7); break;
    default: str = "";                  break;
  }  
  return str;
}

String getWeekdayShortShortString(uint8_t wd) {
  String str;
  switch (wd) {
    case  1: str = FPSTR(SDayShrt_1); break;
    case  2: str = FPSTR(SDayShrt_2); break;
    case  3: str = FPSTR(SDayShrt_3); break;
    case  4: str = FPSTR(SDayShrt_4); break;
    case  5: str = FPSTR(SDayShrt_5); break;
    case  6: str = FPSTR(SDayShrt_6); break;
    case  7: str = FPSTR(SDayShrt_7); break;
    default: str = "";                 break;
  }  
  return str;
}

// ---------- Склонение числительных остатка времени -----------

String WriteDays(uint16_t iDays) {
  uint16_t iDays2 = (iDays / 10) % 10;
  iDays = iDays %10;
  if (iDays2 == 1) return FPSTR(SDayForm_1);                        // F(" дней")
  if (iDays  == 1) return FPSTR(SDayForm_2);                        // F(" день")
  if (iDays  >= 2 && iDays <= 4) return FPSTR(SDayForm_3);          // F(" дня") 
  return FPSTR(SDayForm_1);                                         // F(" дней")
}

String WriteHours(uint8_t iHours) {
  if (iHours == 1 || iHours == 21) return FPSTR(SHourForm_1);                                     // F(" час")
  if ((iHours >= 2 && iHours <= 4) || (iHours >= 22 && iHours <= 24))return FPSTR(SHourForm_2);   // F(" часа")
  return FPSTR(SHourForm_3);                                                                      // F(" часов")
}

String WriteMinutes(uint8_t iMinutes){
  if (iMinutes >= 5 && iMinutes <= 20) return FPSTR(SMinuteForm_1);  // F(" минут")
  iMinutes = iMinutes %10; 
  if (iMinutes == 1) return FPSTR(SMinuteForm_2);                    // F(" минута")
  if (iMinutes >= 2 && iMinutes <= 4) return FPSTR(SMinuteForm_3);   // F(" минуты")
  return FPSTR(SMinuteForm_1);                                       // F(" минут")
}

String WriteSeconds(uint8_t iSeconds){
  if (iSeconds >= 5 && iSeconds <= 20) return FPSTR(SSecondForm_1);  // F(" секунд")
  iSeconds = iSeconds %10;
  if (iSeconds == 1) return FPSTR(SSecondForm_2);                    // F(" секунда")
  if (iSeconds >= 2 && iSeconds <= 4) return FPSTR(SSecondForm_3);   // F(" секунды")
  return FPSTR(SSecondForm_1);                                       // F(" секунд")
}

// ------------------------- CRC16 -------------------------

uint16_t getCrc16(uint8_t * data, uint16_t len)
{
    uint8_t lo;
    union
    {
        uint16_t value;
        struct { uint8_t lo, hi; } bytes;
    } crc;
 
    crc.value = 0xFFFF;
 
    while ( len-- )
    {
        lo = crc.bytes.lo;
        crc.bytes.lo = crc.bytes.hi;
        crc.bytes.hi = lo ^ *data++;
 
        uint8_t mask = 1;
 
        if ( crc.bytes.hi & mask ) crc.value ^= 0x0240;
        if ( crc.bytes.hi & ( mask << 1 ) ) crc.value ^= 0x0480;
        if ( crc.bytes.hi & ( mask << 2 ) ) crc.bytes.hi ^= 0x09;
        if ( crc.bytes.hi & ( mask << 3 ) ) crc.bytes.hi ^= 0x12;
        if ( crc.bytes.hi & ( mask << 4 ) ) crc.bytes.hi ^= 0x24;
        if ( crc.bytes.hi & ( mask << 5 ) ) crc.bytes.hi ^= 0x48;
        if ( crc.bytes.hi & ( mask << 6 ) ) crc.bytes.hi ^= 0x90;
        if ( crc.bytes.hi & ( mask << 7 ) ) crc.value ^= 0x2001;
    }
 
    return crc.value;
}

uint8_t getCrc8(uint8_t * data, uint16_t len) {
  uint16_t crc16 = getCrc16(data, len);
  return (uint8_t)(((crc16 & 0xff) ^ ((crc16 >> 8) & 0xff)) & 0xff);
}

uint8_t wrapX(int8_t x) {
  return (x + pWIDTH) % pWIDTH;
}

uint8_t wrapY(int8_t y) {
  return (y + pHEIGHT) % pHEIGHT;
}

// Сдвиг всей матрицы вниз
void shiftDown() {
  for (uint8_t x = 0; x < pWIDTH; x++) {
    for (uint8_t y = 0; y < pHEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }
}

// Сдвиг всей матрицы вверх
void shiftUp() {
  for (uint8_t x = 0; x < pWIDTH; x++) {
    for (uint8_t y = pHEIGHT - 1; y > 0; y--) {
      drawPixelXY(x, y, getPixColorXY(x, y - 1));
    }
  }
}

// Сдвиг всей матрицы по диагонали
void shiftDiag() {
  for (uint8_t y = 0; y < pHEIGHT - 1; y++) {
    for (uint8_t x = pWIDTH - 1; x > 0; x--) {
      drawPixelXY(x, y, getPixColorXY(x - 1, y + 1));
    }
  }
}

String padRight(const String &str, uint8_t cnt) {
  uint16_t len = str.length();
  if (len >= cnt) return str;
  char data[cnt + 1];
  memset(data, ' ', cnt);
  data[cnt] = '\0';
  strcpy(data, str.c_str());
  data[len] = ' ';
  return String(data);
}

String padLeft(const String &str, uint8_t cnt) {
  uint16_t len = str.length();
  if (len >= cnt) return str;
  char data[cnt + 1];
  memset(data, ' ', cnt);
  data[cnt] = '\0';
  strcpy(&data[cnt - len], str.c_str());
  return String(data);
}

String padNum(int16_t num, uint8_t cnt) {
  char data[12];
  String fmt("%0"); fmt += cnt; fmt += 'd';
  sprintf(data, fmt.c_str(), num);
  return String(data);
}

String getDateTimeString(time_t t) {
  uint8_t hr = hour(t);
  uint8_t mn = minute(t);
  uint8_t sc = second(t);
  uint8_t dy = day(t);
  uint8_t mh = month(t);
  uint16_t yr = year(t);
  String str(padNum(dy,2)); 
  str += '.'; 
  str += padNum(mh,2); 
  str += '.'; 
  str += padNum(yr,4); 
  str += ' '; 
  str += padNum(hr,2); 
  str += ':'; 
  str += padNum(mn,2); 
  str += ':'; 
  str += padNum(sc,2);  
  return str;
}

// ------------------------ Отправка значений в WEB ------------------------

void notifyUnknownCommand(const String& text) {  
  
  String tmp(F("неизвестная команда '"));
  tmp += text; tmp += '\'';    
  SendWeb(tmp, TOPIC_ERR);
}

// Отправка в WEB канал - текущие значения переменных
void SendCurrentState(String& keys) {

  // Эта функция SendCurrentState() вызывается из одного места, после вызова стрка changed_keys, передаваемая сюда по адресу обнуляется
  // Поэтому мы можем не создавать ноаве копии строки, а изменять ее каа нам нужно тут - все равно по вызоде из функции она будет обнклена
  // Если строка начинается с '|' - удалить начальный '|'
  // Если строка заканчивается с '|' - удалить последний '|'
  if (keys.charAt(0) == '|') keys.setCharAt(0,' ');
  if (keys.charAt(keys.length() - 1) == '|') keys.setCharAt(keys.length() - 1, ' ');
  keys.trim();
    
  int16_t pos_start = 0;
  int16_t pos_end = keys.indexOf('|', pos_start);
  int16_t len = keys.length();
  if (pos_end < 0) pos_end = len;

  String key; key.reserve(6);
  String s_tmp; s_tmp.reserve(OUT_CMD_SIZE);
   
  // Строка keys содержит ключи запрашиваемых данных, разделенных знаком '|', например "CE|CC|CO|CK|NC|SC|C1|DC|DD|DI|NP|NT|NZ|NS|DW|OF"
  while (pos_start < len && pos_end >= pos_start) {
    if (pos_end > pos_start) {      
      key = keys.substring(pos_start, pos_end);
      if (key.length() > 0) {
        s_tmp = getStateValue(key, thisMode, true);
        SendWebKey(key, s_tmp);
      }      
    }
    yield();
    pos_start = pos_end + 1;
    pos_end = keys.indexOf('|', pos_start);
    if (pos_end < 0) pos_end = len;
  }
}

String pinName(int8_t pin) {
  #if defined(ESP8266) 
    switch (pin) {
      case 16: return String(F("D0")); /* D0     */
      case  5: return String(F("D1")); /* D1     */ 
      case  4: return String(F("D2")); /* D2     */ 
      case  0: return String(F("D3")); /* D3     */ 
      case  2: return String(F("D4")); /* D4     */ 
      case 14: return String(F("D5")); /* D5     */ 
      case 12: return String(F("D6")); /* D6     */ 
      case 13: return String(F("D7")); /* D7     */ 
      case 15: return String(F("D8")); /* D8     */ 
      #if defined(RX)
      case  3: return String(F("RX")); /* D9/RX  */ 
      case  1: return String(F("TX")); /* D10/TX */ 
      #else
      case  3: return String(F("D9")); /* D9/RX  */ 
      case  1: return String(F("D10"));/* D10/TX */ 
      #endif
      default: return String("N/A");
     }
  #endif
  #if defined(ESP32) 
    // Для ESP32 безопасное подключение ленты к следующим пинам GPIO:
    // 1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27
    // 1/TX, 3/RX - если включен отладочный вывод в COM-порт - лента работать не будет 
    // 21/SDA, 22/SCL - шина I2С - предпочтительна для использования TM1637
    String str; 
    if (pin >=0) {
      str += F("G");
      str += pin;
    } else {
      str += F("N/A");
    }
    return str;
  #else  
    return String(F("N/A"));
  #endif
}

void InitializeQueues() {
  // cmdQueue - массив строк. Каждое присвоение элементу массива строки входящей команды приводит к выделению
  // памяти под строку в Heap, если длина строки помещаемой в элемент очереди больше уже выделенного размера буфера.
  // Память не используется повторно, а выделяется заново в новом участке. Это приводит к фрагментации Heap и
  // невозможности выделить большой целый кусок памяти из-за фрагментации
  // Попытка - изначально зарезервировать память под массив очереди строк входящих команд
  // Если новая команда (длина строки) менее зарезервированной - будет использован тот же кусок памяти без выделения нового
  // Это позволит избежать излишней фрагментации
  if (isQueueInitialized) return;
  
  int32_t mem_prv, mem_now;
  isQueueInitialized = true;
  mem_prv = ESP.getFreeHeap();

  changed_keys.reserve(90);

  for (uint8_t i = 0; i < QSIZE_IN; i++) {
    cmdQueue[i].reserve(IN_CMD_SIZE);
  }
  
  mem_now = ESP.getFreeHeap();
  printMemoryDiff(mem_prv, mem_now, F("Резервирование памяти под очередь сообщений:"));
}

void printMemoryDiff(uint32_t mem_prv, uint32_t mem_now, const String& message) {
  DEBUGLN(DELIM_LINE);
  DEBUGLN(message);
  DEBUG(mem_prv); DEBUG(" - "); DEBUG(mem_now); DEBUG(" -> ");  DEBUGLN(mem_prv - mem_now);
  printMemoryInfo();
  DEBUGLN(DELIM_LINE);        
}

void printMemoryInfo() {
  
  uint32_t memFree;
  uint32_t memMaxBlock;
  
  #if defined(ESP8266)
    uint8_t  memFrag;
    ESP.getHeapStats(&memFree, &memMaxBlock, &memFrag);
  #else
    memFree = ESP.getFreeHeap();
    memMaxBlock = ESP.getMaxAllocHeap();
  #endif     
  
  DEBUG(F("FM: "));
  DEBUG(memFree);
  DEBUG(F("  Max: ")); 
  DEBUG(memMaxBlock); 
  #if defined(ESP8266)
  DEBUG(F("  Frag: ")); DEBUG(memFrag);
  #endif     
    
  DEBUGLN();               
}

String MCUType() {
  String mcType(F("Unknown"));
  #if defined(ESP8266)
    // LED_BUILTIN_AUX определен только для "NodeMCU"
    #if defined(LED_BUILTIN_AUX)
      mcType = F("NodeMCU");
    #else
      mcType = F("Wemos d1 mini");
    #endif
  #endif
  #if defined(ESP32)
    #if defined(CONFIG_IDF_TARGET_ESP32S2)
      mcType = F("ESP32 S2");
    #elif defined(CONFIG_IDF_TARGET_ESP32S3)
      mcType = F("ESP32 S3");
    #elif defined(CONFIG_IDF_TARGET_ESP32C3)
      mcType = F("ESP32 С3");
    #else
      mcType = F("ESP32");
    #endif
  #endif  
  return mcType;
}

String MCUTypeEx() {
  
  // ESP32 поддерживается следующих версий (и у всех у них свой набор доступных пинови их назначение по GPIO):
  // ESP32                   https://github.com/espressif/arduino-esp32/blob/master/variants/esp32/pins_arduino.h
  // ESP32-S2                https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s2/pins_arduino.h
  // ESP32-S2-mini           https://github.com/espressif/arduino-esp32/blob/master/variants/lolin_s2_mini/pins_arduino.h
  // ESP32-S3                https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s3/pins_arduino.h
  // ESP32-S3-mini           https://github.com/espressif/arduino-esp32/blob/master/variants/lolin_s3_mini/pins_arduino.h
  // ESP32-C3                https://github.com/espressif/arduino-esp32/blob/master/variants/esp32c3/pins_arduino.h

  #if defined(ESP32)
    #if (CONFIG_IDF_TARGET_ESP32)
      return F("ESP32-WROOM-D32 / ESP32-D1-MINI");
    #elif (CONFIG_IDF_TARGET_ESP32S2)
      //  MCU      TX   RX   SDA  SCL  SS  MOSI MISO SCK
      //  S2       43   44    8    9   34   35   37   36
      //  S2-mini  39   37    33   35  12   11    9    8
      #if defined(USB_PRODUCT)
        return USB_PRODUCT;            // LOLIN-S2-MINI
      #elif (SCL == 35)   
        return F("LOLIN-S2-MINI");
      #else
        return F("ESP32-S2");
      #endif
    #elif (CONFIG_IDF_TARGET_ESP32S3)
      //  MCU      TX   RX   SDA  SCL  SS  MOSI MISO SCK
      //  S3       43   44    8    9   10   11   13   12
      //  S3-mini  43   44    35  36   10   11   13   12
      #if (SCL == 36)
        return F("ESP32-S3-MINI");
      #else
        return F("ESP32-S3");
      #endif
    #elif (CONFIG_IDF_TARGET_ESP32C3)
      //  MCU      TX   RX   SDA  SCL  SS  MOSI MISO SCK
      //  C3       21   20    8    9   7     6    5    4
      //  C3-mini  21   20    8   10   5     4    3    2
      #if (SCL == 10)
        return F("ESP32-C3-SUPERMINI");
      #else
        return F("ESP32-C3");
      #endif
    #else
      return F("ESP32");
    #endif
  #elif defined(ESP8266) 
    #if defined(ESP8266_NODEMCU)  
      return F("ESP8266 NodeMCU");
    #elif defined(ESP8266_WEMOS)
      return F("ESP8266 Wemos d1 mini");
    #else
      return F("ESP8266");
    #endif
  #else  
    return F("Unknown");
  #endif
}

String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}
