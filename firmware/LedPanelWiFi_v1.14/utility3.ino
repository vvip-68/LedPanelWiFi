// ------------------------------------------

// нарисовать цифру шрифт 3х5 квадратный в буфер
void drawDigit3x5_buf(uint8_t digit, int8_t X, int8_t Y, uint8_t W, uint8_t H, uint8_t* buf) {
  // Буфер - targetArray - массив зазмером WxH, левый нижний угол - точка 0,0, индексация ппо рядам члева направо, снизу вверх
  if (digit > 9) return;
  for (uint8_t x = 0; x < 3; x++) {
    uint8_t thisByte = pgm_read_byte(&(font3x5[digit][x]));
    int8_t xx = x + X;
    if (xx < 0 || xx >= W) continue;
    for (uint8_t y = 0; y < 5; y++) {
      int8_t yy = y + Y;
      if (yy < 0 || yy >= H) continue;
      uint16_t idx = W * yy + xx;
      if (thisByte & (1 << y)) buf[idx] = 1;
    }
  }
}

// нарисовать цифру шрифт 3х5 скруглённый
void drawDigit3x5_sbuf(uint8_t digit, int8_t X, int8_t Y, uint8_t W, uint8_t H, uint8_t* buf) {
  if (digit > 9) return;
  for (uint8_t x = 0; x < 3; x++) {
    uint8_t thisByte = pgm_read_byte(&(font3x5_s[digit][x]));
    int8_t xx = x + X;
    if (xx < 0 || xx >= W) continue;
    for (uint8_t y = 0; y < 5; y++) {
      int8_t yy = y + Y;
      if (yy < 0 || yy >= H) continue;
      uint16_t idx = W * yy + xx;
      if (thisByte & (1 << y)) buf[idx] = 1;
    }
  }
}

// нарисовать цифру шрифт 5х7
void drawDigit5x7_buf(uint8_t digit, int8_t X, int8_t Y, uint8_t W, uint8_t H, uint8_t* buf) {
  if (digit > 9) return;
  int16_t sz = W * H;
  for (uint8_t x = 0; x < 5; x++) {
    uint8_t thisByte = pgm_read_byte(&(font5x7[digit][x]));
    int8_t xx = x + X;
    if (xx < 0 || xx >= W) continue;
    for (uint8_t y = 0; y < 7; y++) {
      int8_t yy = (6 - y) + Y;
      if (yy < 0 || yy >= H) continue;
      int16_t idx = W * yy + xx;
      if (idx < 0 || idx >= sz) continue;
      if (thisByte & (1 << y)) buf[idx] = 1;
    }
  }
}

// функция отрисовки точки по координатам X Y
void drawPixelXY_buf(int8_t x, int8_t y, uint8_t W, uint8_t H, uint8_t* buf) {
  if (leds == nullptr) return;
  if (x < 0 || x >= W || y < 0 || y >= H) return;
  uint16_t idx = W * y + x;
  buf[idx] = 1;
}

// ------------------------------------------

void drawClock_buf(uint8_t W, uint8_t H, uint8_t* buf) {
  if (!(allowVertical || allowHorizontal)) return;

  bool debug = debug_hours >= 0 && debug_mins >= 0;
  if (debug) {
    hrs = debug_hours; mins = debug_mins; 
  }

  // 12/24-часовой формат
  if (time_h12) {
    if (hrs > 12) hrs -= 12;
    if (hrs == 0) hrs = 12;
  }

  uint8_t h10 = hrs / 10;
  uint8_t h01 = hrs % 10;
  uint8_t m10 = mins / 10;
  uint8_t m01 = mins % 10;
  
  if (c_size == 1) {
    
    // Малые часы (горизонтальные и вертикальные)
    if (use_round_3x5) {
      if (show_clockH10_zero) drawDigit3x5_sbuf(h10, clockX_H10_pos, clockY_H10_pos, W, H, buf);
      drawDigit3x5_sbuf(h01, clockX_H01_pos, clockY_H01_pos, W, H, buf);
      drawDigit3x5_sbuf(m10, clockX_M10_pos, clockY_M10_pos, W, H, buf);
      drawDigit3x5_sbuf(m01, clockX_M01_pos, clockY_M01_pos, W, H, buf);
    } else {
      if (show_clockH10_zero) drawDigit3x5_buf(h10, clockX_H10_pos, clockY_H10_pos, W, H, buf);
      drawDigit3x5_buf(h01, clockX_H01_pos, clockY_H01_pos, W, H, buf);
      drawDigit3x5_buf(m10, clockX_M10_pos, clockY_M10_pos, W, H, buf);
      drawDigit3x5_buf(m01, clockX_M01_pos, clockY_M01_pos, W, H, buf);
    }

    // Разделительные точки часов - минут
    if (CLOCK_ORIENT == 0) {
      // Горизонтальные часы
      drawPixelXY_buf(clockX_dot_pos, clockY + 1, W, H, buf);
      drawPixelXY_buf(clockX_dot_pos, clockY + 3, W, H, buf);
    } else {
      // Вертикальные часы
      drawPixelXY_buf(clockX_dot_pos, clockY + 5, W, H, buf);
    }
    
  } else {

    // Большие часы (горизонтальные и вертикальные)
    if (show_clockH10_zero) drawDigit5x7_buf(h10, clockX_H10_pos, clockY_H10_pos, W, H, buf);
    drawDigit5x7_buf(h01, clockX_H01_pos, clockY_H01_pos, W, H, buf);
    drawDigit5x7_buf(m10, clockX_M10_pos, clockY_M10_pos, W, H, buf);
    drawDigit5x7_buf(m01, clockX_M01_pos, clockY_M01_pos, W, H, buf);

    // Разделительные точки часов - минут
    if (CLOCK_ORIENT == 0) {
      // Горизонтальные часы
      // Для ширины матрицы в 25 колонок - рисовать одинарные точки разделения часов/минут, если больше - сдвоенные
      drawPixelXY_buf(clockX_dot_pos, clockY + 1, W, H, buf);         
      drawPixelXY_buf(clockX_dot_pos, clockY + 2, W, H, buf);
      drawPixelXY_buf(clockX_dot_pos, clockY + 4, W, H, buf);
      drawPixelXY_buf(clockX_dot_pos, clockY + 5, W, H, buf);
      // Для ширины матрицы в 26 колонок - рисовать одинарные точки разделения часов/минут
      if (pWIDTH > 25) {
        drawPixelXY_buf(clockX_dot_pos + 1, clockY + 1, W, H, buf);
        drawPixelXY_buf(clockX_dot_pos + 1, clockY + 2, W, H, buf);
        drawPixelXY_buf(clockX_dot_pos + 1, clockY + 4, W, H, buf);
        drawPixelXY_buf(clockX_dot_pos + 1, clockY + 5, W, H, buf);
      }
    } else {
      // Вертикальные часы
      for (uint8_t i = 0; i < 3; i++) {
        drawPixelXY_buf(clockX_dot_pos + i, clockY + 7, W, H, buf);        
      }
    }
    
  }
}

#if (USE_WEATHER == 1)      

void drawTemperature_buf(uint8_t W, uint8_t H, uint8_t* buf) {

  // Вычисление позиции отрисовки температуры относительно центральной линии матрицы.

  bool   debug = debug_hours >= 0 && debug_mins >= 0;
  int8_t tempr = debug ? debug_temperature : temperature;

  int8_t  th = (isFarenheit ? (round(tempr * 9 / 5) + 32) : tempr);
  uint8_t t = abs(th);
  uint8_t dec_t = t / 10;
  uint8_t edc_t = t % 10;

  if (c_size == 1) {

    // Градусы C/F
    if (show_tempCF) {
    
      if (isFarenheit) {
        // буква F
        for(uint8_t i = 0; i < 5; i++) {
          drawPixelXY_buf(temperatureX_CF_pos, temperatureY + i, W, H, buf);
        }
        for(uint8_t i = 0; i < 2; i++) {
          drawPixelXY_buf(temperatureX_CF_pos + 1 + i, temperatureY + 4, W, H, buf);
        }     
        drawPixelXY_buf(temperatureX_CF_pos + 1, temperatureY + 2, W, H, buf);
      } else {
        // буква C
        if (use_round_3x5) {
          for(uint8_t i = 1; i < 3; i++) {
            drawPixelXY_buf(temperatureX_CF_pos, temperatureY + i, W, H, buf);      
          }
        } else {
          for(uint8_t i = 0; i < 4; i++) {
            drawPixelXY_buf(temperatureX_CF_pos, temperatureY + i, W, H, buf);
          }
        }
        drawPixelXY_buf(temperatureX_CF_pos + 1, temperatureY, W, H, buf);      
        drawPixelXY_buf(temperatureX_CF_pos + 2, temperatureY, W, H, buf);      
        drawPixelXY_buf(temperatureX_CF_pos + 1, temperatureY + 3, W, H, buf);      
        drawPixelXY_buf(temperatureX_CF_pos + 2, temperatureY + 3, W, H, buf);      
      }
    }

    // Значок градуса
    if (show_tempDG) {
      // При температуре = 0 - рисуем маленький значок C / F
      drawPixelXY_buf(temperatureX_DG_pos, temperatureY + 4, W, H, buf);      
      drawPixelXY_buf(temperatureX_DG_pos, temperatureY + 3, W, H, buf);      
      drawPixelXY_buf(temperatureX_DG_pos + 1, temperatureY + 4, W, H, buf);      
      drawPixelXY_buf(temperatureX_DG_pos + 1, temperatureY + 3, W, H, buf);            
    }

    // Единицы градусов
    if (use_round_3x5)
      drawDigit3x5_sbuf(edc_t, temperatureX_T01_pos, temperatureY, W, H, buf);
    else
      drawDigit3x5_buf(edc_t, temperatureX_T01_pos, temperatureY, W, H, buf);
    
    // Десятки градусов
    if (dec_t != 0) {
      if (use_round_3x5)
        drawDigit3x5_sbuf(dec_t, temperatureX_T10_pos, temperatureY, W, H, buf);
      else  
        drawDigit3x5_buf(dec_t, temperatureX_T10_pos, temperatureY, W, H, buf);
    }

    // Нарисовать '+' или '-' если температура не 0
    // Горизонтальная черта - общая для '-' и '+'
    if (th != 0) {
      for(uint8_t i = 0; i < 3; i++) {
        drawPixelXY_buf(temperatureX_SG_pos + i, temperatureY + 2, W, H, buf);      
      }
      
      // Для плюcа - вертикальная черта
      if (th > 0) {
        drawPixelXY_buf(temperatureX_SG_pos + 1, temperatureY + 1, W, H, buf);
        drawPixelXY_buf(temperatureX_SG_pos + 1, temperatureY + 3, W, H, buf);
      }
    }    
    
  } else {

    // Буква 'C'/'F'
    if (show_tempCF) {
      if (isFarenheit) {
        for(uint8_t i=0; i<7; i++) drawPixelXY_buf(temperatureX_CF_pos, temperatureY + i, W, H, buf);
        for(uint8_t i=0; i<3; i++) drawPixelXY_buf(temperatureX_CF_pos + 1 + i, temperatureY + 6, W, H, buf);
        for(uint8_t i=0; i<2; i++) drawPixelXY_buf(temperatureX_CF_pos + 1 + i, temperatureY + 3, W, H, buf);
      } else {
        for(uint8_t i=0; i<5; i++) drawPixelXY_buf(temperatureX_CF_pos, temperatureY + 1 + i, W, H, buf);
        for(uint8_t i=0; i<3; i++) drawPixelXY_buf(temperatureX_CF_pos + 1 + i, temperatureY, W, H, buf);
        for(uint8_t i=0; i<3; i++) drawPixelXY_buf(temperatureX_CF_pos + 1 + i, temperatureY + 6, W, H, buf);
        drawPixelXY_buf(temperatureX_CF_pos + 4, temperatureY + 5, W, H, buf);
        drawPixelXY_buf(temperatureX_CF_pos + 4, temperatureY + 1, W, H, buf);
      }
    }
    
    // Знак градусов; если ширина <= 24 - пробела между градусом и С нет
    if (show_tempDG) {
      drawPixelXY_buf(temperatureX_DG_pos,     temperatureY + 5, W, H, buf);
      drawPixelXY_buf(temperatureX_DG_pos,     temperatureY + 4, W, H, buf);
      drawPixelXY_buf(temperatureX_DG_pos + 1, temperatureY + 6, W, H, buf);
      drawPixelXY_buf(temperatureX_DG_pos + 1, temperatureY + 3, W, H, buf);
      drawPixelXY_buf(temperatureX_DG_pos + 2, temperatureY + 5, W, H, buf);
      drawPixelXY_buf(temperatureX_DG_pos + 2, temperatureY + 4, W, H, buf);
    }
    
    // Единицы температуры; если ширина <= 23 - пробела между цифрой и градусом нет
    drawDigit5x7_buf(edc_t, temperatureX_T01_pos, temperatureY, W, H, buf);

    // Десятки температуры
    if (dec_t != 0) {
      drawDigit5x7_buf(dec_t, temperatureX_T10_pos, temperatureY, W, H, buf);
    }

    // Если температура не нулевая - рисуем знак '+' или '-'
    if (th != 0) {
      for(uint8_t i=0; i<3; i++) drawPixelXY_buf(temperatureX_SG_pos + i, temperatureY + 3, W, H, buf);
      if (th > 0) {
        for(uint8_t i=0; i<3; i++) drawPixelXY_buf(temperatureX_SG_pos + 1, temperatureY + 2 + i, W, H, buf);
      }
    }    
  }
  
}

#endif

// нарисовать дату календаря
void drawCalendar_buf(uint8_t W, uint8_t H, uint8_t* buf) {

  int8_t  pDay = aday;
  int8_t  pMonth = amnth;
  int16_t pYear = ayear;
    
  // Для отладки позиционирования часов с температурой
  bool debug = debug_hours >= 0 && debug_mins >= 0;
  if (debug) {
    pDay = debug_days; pMonth = debug_month; pYear = debug_year;
  }

  uint8_t d10 = pDay / 10;
  uint8_t d01 = pDay % 10;
  uint8_t m10 = pMonth / 10;
  uint8_t m01 = pMonth % 10;

  uint8_t d1 =  pYear / 1000;
  uint8_t d2 = (pYear / 100) % 10;
  uint8_t d3 = (pYear / 10) % 10;
  uint8_t d4 =  pYear % 10;

  if (c_size == 1) {
    
    // Малые часы (горизонтальные и вертикальные)
    if (use_round_3x5) {
      
      // Верхий ряд - ДД.MM
      drawDigit3x5_sbuf(d10, calendarX_D10_pos, calendarY_D10_pos, W, H, buf);
      drawDigit3x5_sbuf(d01, calendarX_D01_pos, calendarY_D01_pos, W, H, buf);
      drawDigit3x5_sbuf(m10, calendarX_M10_pos, calendarY_M10_pos, W, H, buf);
      drawDigit3x5_sbuf(m01, calendarX_M01_pos, calendarY_M01_pos, W, H, buf);
      // Нижний ряд - ГГГГ  
      if (allow_two_row) {
        drawDigit3x5_sbuf(d1, calendarX_Y1000_pos, calendarY_Y1000_pos, W, H, buf);    
        drawDigit3x5_sbuf(d2, calendarX_Y0100_pos, calendarY_Y0100_pos, W, H, buf);
        drawDigit3x5_sbuf(d3, calendarX_Y0010_pos, calendarY_Y0010_pos, W, H, buf);
        drawDigit3x5_sbuf(d4, calendarX_Y0001_pos, calendarY_Y0001_pos, W, H, buf);
      }

    } else {
      
      // Верхий ряд - ДД.MM
      drawDigit3x5_buf(d10, calendarX_D10_pos, calendarY_D10_pos, W, H, buf);    
      drawDigit3x5_buf(d01, calendarX_D01_pos, calendarY_D01_pos, W, H, buf);
      drawDigit3x5_buf(m10, calendarX_M10_pos, calendarY_M10_pos, W, H, buf);
      drawDigit3x5_buf(m01, calendarX_M01_pos, calendarY_M01_pos, W, H, buf);
      // Нижний ряд - ГГГГ  
      if (allow_two_row) {
        drawDigit3x5_buf(d1, calendarX_Y1000_pos, calendarY_Y1000_pos, W, H, buf);    
        drawDigit3x5_buf(d2, calendarX_Y0100_pos, calendarY_Y0100_pos, W, H, buf);
        drawDigit3x5_buf(d3, calendarX_Y0010_pos, calendarY_Y0010_pos, W, H, buf);
        drawDigit3x5_buf(d4, calendarX_Y0001_pos, calendarY_Y0001_pos, W, H, buf);
      }

    }

    // Разделительная точка между числом и месяцем
    drawPixelXY_buf(calendarX_DT_pos, calendarY_D01_pos + 1, W, H, buf);
    drawPixelXY_buf(calendarX_DT_pos, calendarY_D01_pos,     W, H, buf);
    
  } else {
    
    if (pWIDTH < 23) {
      
      // Ширина матрицы позволяет рисовать только день ДД (число) вверху и месяц MM внизу
      drawDigit5x7_buf(d10, calendarX_D10_pos, calendarY_D10_pos, W, H, buf);
      drawDigit5x7_buf(d01, calendarX_D01_pos, calendarY_D01_pos, W, H, buf);
      drawDigit5x7_buf(m10, calendarX_M10_pos, calendarY_M10_pos, W, H, buf);
      drawDigit5x7_buf(m01, calendarX_M01_pos, calendarY_M01_pos, W, H, buf);

      // Разделительная точка
      drawPixelXY_buf(calendarX_DT_pos - 1, calendarY_D10_pos - 1, W, H, buf);
      drawPixelXY_buf(calendarX_DT_pos + 1, calendarY_D10_pos - 1, W, H, buf);
      
    } else {
      
      // Ширина матрицы позволяет рисовать день и месяц ДД.MM вверху, год ГГГГ внизу
      
      drawDigit5x7_buf(d10, calendarX_D10_pos, calendarY_D10_pos, W, H, buf);
      drawDigit5x7_buf(d01, calendarX_D01_pos, calendarY_D01_pos, W, H, buf);
      drawDigit5x7_buf(m10, calendarX_M10_pos, calendarY_M10_pos, W, H, buf);
      drawDigit5x7_buf(m01, calendarX_M01_pos, calendarY_M01_pos, W, H, buf);

      drawDigit5x7_buf(d1, calendarX_Y1000_pos, calendarY_Y1000_pos, W, H, buf);
      drawDigit5x7_buf(d2, calendarX_Y0100_pos, calendarY_Y0100_pos, W, H, buf);
      drawDigit5x7_buf(d3, calendarX_Y0010_pos, calendarY_Y0010_pos, W, H, buf);
      drawDigit5x7_buf(d4, calendarX_Y0001_pos, calendarY_Y0001_pos, W, H, buf);      
      
      // Разделительная точка
      drawPixelXY_buf(calendarX_DT_pos, calendarY_D10_pos,     W, H, buf);
      drawPixelXY_buf(calendarX_DT_pos, calendarY_D10_pos + 1, W, H, buf);
    }
  }

}
