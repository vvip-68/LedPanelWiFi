/*
 Модуль формирования отображения часов, погоды и календаря
*/

// ****************** НАСТРОЙКИ ЧАСОВ *****************
#define MIN_COLOR CRGB::White          // цвет минут
#define HOUR_COLOR CRGB::White         // цвет часов
#define DOT_COLOR CRGB::White          // цвет точек

#define NORMAL_CLOCK_COLOR CRGB::White // Нормальный цвет монохромных цветов

#define CONTRAST_COLOR_1 CRGB::Orange  // контрастный цвет часов
#define CONTRAST_COLOR_2 CRGB::Green   // контрастный цвет часов
#define CONTRAST_COLOR_3 CRGB::Yellow  // контрастный цвет часов

#define HUE_STEP 5                     // шаг цвета часов в режиме радужной смены
#define HUE_GAP 30                     // шаг цвета между цифрами в режиме радужной смены

// ****************** ДЛЯ РАЗРАБОТЧИКОВ ****************
uint8_t clockHue;

CRGB clockLED[5] = {HOUR_COLOR, HOUR_COLOR, DOT_COLOR, MIN_COLOR, MIN_COLOR};

String clockCurrentText() {
  
  hrs = hour();
  uint8_t hh = hrs;
  
  if (time_h12) {
    if (hh > 12) hh -= 12;
    if (hh == 0) hh = 12;
  }
  mins = minute();

  String str(padNum(hh, 2));
  str += ':';
  str += padNum(mins, 2);
  
  return str;
}

void clockColor() {
  
  int8_t color_idx = 0;
  switch (thisMode) {
    case MC_CLOCK:
      // Если режим часов включен как "Ночные часы" - цвет определять по настроенному цвету ночных часов
      // Иначе - цвет часов зависит от режима (Монохром, Каждая цифра свой цвет, Цвет часов-точек-минут" и т.д)
      color_idx = isNightClock ? -2 : COLOR_MODE;
      break;
    case MC_COLORS:
    case MC_FILL_COLOR:
      color_idx = -1;
      break;
    default:
      color_idx = COLOR_MODE;
      break;
  }
    
  if (color_idx == -2) {
    
    // Цвет по индексу настроек текущего ночного цвета     
    CRGB color = getNightClockColorByIndex(nightClockColor);
    for (uint8_t i = 0; i < 5; i++) clockLED[i] = color;
    
  } else if (color_idx == -1) {     
    
    // Инверсный от основного цвет
    CRGB color = globalColor == 0xFFFFFF
      ? color = CRGB::Navy
      : -CRGB(globalColor);
    for (uint8_t i = 0; i < 5; i++) clockLED[i] = color;  
    
  } else if (color_idx == 0) {     
    
    // Монохромные часы  
    uint8_t hue = effectScaleParam[MC_CLOCK];
    CHSV color = hue <= 1 ? CHSV(255, 0, 255): CHSV(hue, 255, 255);
    for (uint8_t i = 0; i < 5; i++) clockLED[i] = color;
    
  } else if (color_idx == 1) {
    
    // Каждая цифра своим цветом, плавная смена цвета
    for (uint8_t i = 0; i < 5; i++) clockLED[i] = CHSV(clockHue + HUE_GAP * i, 255, 255);
    clockLED[2] = CHSV(clockHue + 128 + HUE_GAP * 1, 255, 255); // точки делаем другой цвет
    
  } else if (color_idx == 2) {
    
    // Часы, точки, минуты своим цветом, плавная смена цвета
    clockLED[0] = CHSV(clockHue + HUE_GAP * 0, 255, 255);
    clockLED[1] = CHSV(clockHue + HUE_GAP * 0, 255, 255);
    clockLED[2] = CHSV(clockHue + 128 + HUE_GAP * 1, 255, 255); // точки делаем другой цвет
    clockLED[3] = CHSV(clockHue + HUE_GAP * 2, 255, 255);
    clockLED[4] = CHSV(clockHue + HUE_GAP * 2, 255, 255);
    
  } else {
    
    CRGB color = getGlobalClockColor();
    for (uint8_t i = 0; i < 5; i++) clockLED[i] = color;
    
  }
}

uint8_t getClockSizeType() {
  uint8_t clock_size = CLOCK_SIZE;
  // Если часы авто или большие - определить - а поместятся ли они на матрицу по ширине при горизонтальном режиме / по высоте при вертикальном
  // Большие часы для шрифта 5x7 требуют 4*5 /цифры/ + 4 /двоеточие/ + 2 /пробел между цифрами часов и минут / = 23, 25 или 26 колонки (одинарные / двойные точки в часах) если 23 - нет пробела вокруг точек
  if ((clock_size == 0 || clock_size == 2) && ((CLOCK_ORIENT == 0 && pWIDTH < 23) || (CLOCK_ORIENT == 1 && pHEIGHT < 15))) clock_size = 1;
  if (clock_size == 0) clock_size = 2;
  return clock_size;
}

// Вычисление позиции отрисовки пикселя для часов, сдвигающихся по кругу.
int8_t getClockX(int8_t x) {
  return vDEVICE_TYPE == 1 ? x : (x < 0 ? pWIDTH - (abs(x) % pWIDTH) : (x >= pWIDTH ? (x % pWIDTH) : x));
}

int8_t getClockY() {
  
  // Позиция по верткали - относительно центра матрицы. 
  // Вертикальные часы, шрифт 3x5 - 11 точек, шрифт 5х7 - 15 точек
  // Горизонтальные часы, шрифт 3x5 - 5 точек, шрифт 5х7 - 7 точек
  // Значение 0 - нижний ряд матрицы

  // В два ряда (вертикальные часы) - только если позволяет высота матрицы
  int8_t height = CLOCK_ORIENT == 1 && allow_two_row 
     ? (c_size == 1 ? 11 : 15) 
     : (c_size == 1 ? 5 : 7);  
     
  // Часы - нечетное количество строк, матрица - четное или нечетное количество строк
  // Для матриц с четным количеством - сдинуть на один ряд вверх
  int8_t y = ((pHEIGHT - height) / 2) + ((pHEIGHT + 1) % 2);
  
  return y;
}

#if (USE_WEATHER == 1)      

int8_t getTemperatureY() {
  
  // Позиция отображения температуры по вертикали - относительно центра матрицы
  // Значение 0 - нижний ряд матрицы
  int8_t height = c_size == 1 ? 5 : 7;  
  
  // Температура - нечетное количество строк, матрица - четное или нечетное количество строк
  // Для матриц с четным количеством - сдинуть на один ряд вверх
  int8_t y = ((pHEIGHT - height) / 2) + ((pHEIGHT + 1) % 2);
  
  return y;  
}

#endif

int8_t getCalendarY() {
 
  // Позиция отображения календаря по вертикали - отображается в два ряда
  // Позиционирование - по центру матрицы; Два ряда цифр, шрифт 3x5 - 11 точек, шрифт 5х7 - 15 точек
  // Значение 0 - нижний ряд матрицы
  // В два ряда - только если позволяет высота матрицы
  
  uint8_t height =  allow_two_row ? (c_size == 1 ? 11 : 15) : (c_size == 1 ? 5 : 7);  
  
  // Температура - нечетное количество строк, матрица - четное или нечетное количество строк
  // Для матриц с четным количеством - сдинуть на один ряд вверх
  int8_t y = ((pHEIGHT - height) / 2) + ((pHEIGHT + 1) % 2);
  
  return y;
}

void calcClockPosition() {
  
  // Вычислить прямоугольник, в котором будет выведены часы
  // Позиция двоеточия часов считается относительно - текущего центра отображения - CLOCK_XC; 
  // Позиционирование - по центру матрицы, горизонтальному (текущая плозиция CLOCK_XC) и вертикальному
  
  int8_t pHrs = hrs, pMins = mins;

  // Для отладки позиционирования часов с температурой
  bool debug = debug_hours >= 0 && debug_mins >= 0;
  if (debug) {
    pHrs = debug_hours; pMins = debug_mins; 
  }

  if (time_h12) {
    if (pHrs > 12) pHrs -= 12;
    if (pHrs == 0) pHrs = 12;
  }

  uint8_t h10 = pHrs / 10;
  uint8_t h01 = pHrs % 10;
  uint8_t m10 = pMins / 10;
  uint8_t m01 = pMins % 10;  

  clockY = getClockY();  

  // Позиция цифр по ВЕРТИКАЛИ
  if (CLOCK_ORIENT == 0 || !allow_two_row) {
    // Горизонтальные часы
    clockY_H10_pos = clockY_H01_pos = clockY_M10_pos = clockY_M01_pos = clockY;
  } else {
    // Вертикальные часы
    if (c_size == 1) {
     clockY_H10_pos = clockY_H01_pos = clockY + 6;    // Шрифт 3x5 + 1 разделительная строка
     clockY_M10_pos = clockY_M01_pos = clockY;
    } else {
     clockY_H10_pos = clockY_H01_pos = clockY + 8;    // Шрифт 5x7 + 1 разделительная строка
     clockY_M10_pos = clockY_M01_pos = clockY;
    }    
  }

  // Позиция цифр по ГОРИЗОНТАЛИ
  if (CLOCK_ORIENT == 0 || !allow_two_row) {
    
    // Горизонтальные часы
    show_clockH10_zero = h10 > 0;
    clockX_dot_pos = CLOCK_XC;
    
    if (c_size == 1) {
      
      // Шрифт 3x5
      // Часы
      clockX_H01_pos = CLOCK_XC - (use_round_3x5 ? 4 : 3); 
      clockX_H10_pos = CLOCK_XC - (use_round_3x5 ? 8 : 7);
      if (h10 == 1 && h01 != 1) clockX_H10_pos++; 
      if (h01 == 1) clockX_H10_pos++; 
      // Минуты
      clockX_M10_pos = CLOCK_XC + (use_round_3x5 ? 2 : 1);
      clockX_M01_pos = CLOCK_XC + (use_round_3x5 ? 6 : 5);
      if (use_round_3x5) {
        if (h01 == 1) clockX_H01_pos++;
        if (h10 == 1 && h01 == 1) clockX_H10_pos++;
        if (m10 == 1) clockX_M01_pos--;
        if (m01 == 1) clockX_M01_pos++;
        if (m10 == 1 && m01 != 1) clockX_M01_pos++;
      }
      if (m10 == 1 || m01 == 1) clockX_M01_pos--;
      // Ширина и позиция - если незначащий ноль в часах - позиция по единицам часов
      clockX = h10 == 0 ? clockX_H01_pos : clockX_H10_pos;
      // Если десятки часов = 1 - позиция начала блока смещается на 1 пикс вправо, т.к. единица - узкая
      if (h10 == 1 && !use_round_3x5) clockX++;
      if (h10 == 0 && h01 == 1 && !use_round_3x5) clockX++; 
      // Ширина часов - от левой позиции вывода до последней + ширина последнего символа
      clockW = clockX_M01_pos - clockX + 3;
      // Если ед минут = 1 - ширина уменьшается на 1, т.к. цифра узкая
      if (m01 == 1) clockW--;
      if (use_round_3x5) {
        // В часах с круглыми цифрами расстояние от точек до цифр - по 1 колонке, макс ширина часов - 17 колонок
        // Если ширина матрицы 16 - убираем один пробел между разделительными точками и единицами часов
        if (pWIDTH == 16 && clockW == 17) {
          clockX++;
          clockX_H01_pos++;
          clockX_H10_pos++;
          clockW--;
        }
        // Если матрица 15 - убираем пробел между разделительными точками и десятками минут
        if (pWIDTH == 15) {
          if (clockW >= 16) {
            clockX++;
            clockX_H01_pos++;
            clockX_H10_pos++;
            clockW--;
          }
          // Если все еще не влазим - убираем пробел между разделительными точками и десятками минут
          if (clockW == 16) {          
            clockX_M01_pos--;
            clockX_M10_pos--;
            clockW--;
          }
        }
      }
      
    } else {
      
      // Шрифт 5x7 - изначально на ширину поля 26+
      clockX_dot_pos--;
      // Часы
      if (h10 == 1 && h01 == 1) {
        clockX_H01_pos = CLOCK_XC - 6;
        clockX_H10_pos = CLOCK_XC - 11;
      } else
      if (h10 != 1 && h01 == 1) {
        clockX_H01_pos = CLOCK_XC - 6;
        clockX_H10_pos = CLOCK_XC - 11;
      } else
      if (h10 == 1 && h01 != 1) {
        clockX_H01_pos = CLOCK_XC - 7;
        clockX_H10_pos = CLOCK_XC - 12;
      } else {
        clockX_H01_pos = CLOCK_XC - 7;
        clockX_H10_pos = CLOCK_XC - 13;
      }
      // Минуты
      if (m10 == 1 && m01 == 1) {
        clockX_M10_pos = CLOCK_XC + 1;
        clockX_M01_pos = CLOCK_XC + 6;
      } else
      if (m10 != 1 && m01 == 1) {
        clockX_M10_pos = CLOCK_XC + 2;
        clockX_M01_pos = CLOCK_XC + 7;
      } else
      if (m10 == 1 && m01 != 1) {
        clockX_M10_pos = CLOCK_XC + 1;
        clockX_M01_pos = CLOCK_XC + 6;
      } else {
        clockX_M10_pos = CLOCK_XC + 2;
        clockX_M01_pos = CLOCK_XC + 8;
      }
      // Если ширина поля 25 - ширина точек не 2 колонки, а одна - позиции минут сдвигаются влево на 1 колонку
      if (pWIDTH == 25) {
        clockX_M01_pos--;
        clockX_M10_pos--;
      }
      // Если ширина поля 23 или 24 - ширина точек не 2 колонки, а одна, пробела между цифрой и точками нет
      // - позиции минут сдвигаются влево на 2 колонки, позиция часов сдвигается вправо на 1 колонку
      if (pWIDTH <= 24) {
        if (h10 > 0) {
          clockX_M01_pos -= 2;
          clockX_M10_pos -= 2;
          clockX_H01_pos++;
          clockX_H10_pos++;
        } else {
          clockX_M01_pos -= 1;
          clockX_M10_pos -= 1;
        }        
      }
      
      // Ширина и позиция - если незначащий ноль в часах - позиция по единицам часов
      clockX = h10 == 0 ? clockX_H01_pos : clockX_H10_pos;
      // Если десятки часов = 1 - позиция начала блока смещается на 1 пикс вправо, т.к. единица - узкая
      if (h10 == 1) clockX++;
      // Если единицы часов = 1 - позиция начала блока смещается на 1 пикс вправо, т.к. единица - узкая
      if (h10 == 0 && h01 == 1) clockX++;
      // Ширина часов - от левой позиции вывода до последней + ширина последнего символа
      clockW = clockX_M01_pos - clockX  + 5;
      // Если ед минут = 1 - ширина уменьшается на 1, т.к. цифра узкая
      if (m01 == 1) clockW--;
      
    }
        
  } else {
    
    // Вертикальные часы
    show_clockH10_zero = true;

    if (c_size == 1) {
      
      // Шрифт 3x5
      clockW = 7;
      clockX_M10_pos = CLOCK_XC - 3;
      clockX_H10_pos = CLOCK_XC - 3;
      clockX_M01_pos = CLOCK_XC + 1;
      clockX_H01_pos = CLOCK_XC + 1;     
      clockX = clockX_H10_pos;      
      
      // Если десятки часов и десятки минут обе = 1 - т.к. цифра узкая - позиция вывода смещается на 1 вправо, ширина уменьшается на 1
      if (h10 == 1 && m10 == 1 && !use_round_3x5) {
        clockX++;
        clockW--;
      }
      
      // Если единицы часов и ед. минут обе = 1 - т.к. цифра узкая -  ширина уменьшается на 1
      if (h01 == 1 && m01 == 1) {
        clockW--;
      }
      
      clockX_dot_pos = CLOCK_XC;
      
    } else {
      
      // Шрифт 5x7
      clockW = 11;
      clockX_M10_pos = CLOCK_XC - 5;
      clockX_H10_pos = CLOCK_XC - 5;
      clockX_M01_pos = CLOCK_XC + 1;
      clockX_H01_pos = CLOCK_XC + 1;     
      clockX = clockX_H10_pos;      
      
      // Если десятки часов и десятки минут обе = 1 - т.к. цифра узкая - позиция вывода смещается на 1 вправо, ширина уменьшается на 1
      if (h10 == 1 && m10 == 1) {
        clockX++;
        clockW--;
      }
      
      // Если единицы часов и ед. минут обе = 1 - т.к. цифра узкая -  ширина уменьшается на 1
      if (h01 == 1 && m01 == 1) {
        clockW--;
      }
      
      clockX_dot_pos = CLOCK_XC - 1;
      
    }    
  }

  clockH = CLOCK_ORIENT == 1
    ? (c_size == 1 ? 11 : 15) 
    : (c_size == 1 ? 5 : 7);  

  // Центрировать часы относительно линии CLOCK_XC
  int8_t offset  = (pWIDTH - clockW) / 2;
  int8_t offset2 = CLOCK_XC - (pWIDTH / 2);
  if (pWIDTH % 2 == 0 && clockW % 2 == 1) offset++;
  if (pWIDTH % 2 == 1 && clockW % 2 == 0) offset++;
  int8_t dx = offset - clockX + offset2;
  if (dx != 0) shiftClockPosition(dx, 0);
    
}

#if (USE_WEATHER == 1)      

void calcTemperaturePosition() {
  
  // Вычислить прямоугольник, в котором будет выведена температура
  // Позиция температуры по умолчанию - по центру матрицы

  bool   debug = debug_hours >= 0 && debug_mins >= 0;
  int8_t tempr = debug ? debug_temperature : temperature;

  uint8_t t = abs((isFarenheit ? (round(tempr * 9 / 5) + 32) : tempr));
  uint8_t dec_t = t / 10;
  uint8_t edc_t = t % 10;

  // Начальное позиционирование - от правого края матрицы
  uint8_t right = pWIDTH  - 1;

  show_tempCF = showTempLetter;
  show_tempDG = showTempDegree;

  // Размер - шрифт 3x5 как в малых часах
  if (c_size == 1) {
        
    // Если температура равна 0 - рисуется маленькая буква 'C' размером 2x3 или 'F' размером 3x5  
    if (t == 0) {
      
      show_tempCF = true;
      temperatureW = 7;
      if (show_tempDG) temperatureW += 2;
      temperatureX_CF_pos = right - 2;
      temperatureX_DG_pos = show_tempDG ? temperatureX_CF_pos - 2: temperatureX_CF_pos;
      temperatureX_T01_pos = temperatureX_DG_pos - 4;
      
    } else {

      // Полная ширина температуры с отображением значка градуса и буквы C/F = 11 + 6 = 17;
      // Для матриц шириной 16 - не рисуем пробел между десятками градусов и знаком +/- - это сделаем вконце, когда позиция отрисовки знака будет вычислена
      // Если не влазим по ширине 15 колонок - значок градуса шириной 2 не рисовать - тогда 11 + 4 = 15;
      if (pWIDTH == 15 && show_tempCF && show_tempDG) {
        show_tempDG = false;
      }
      // Если не влазим по ширине 14 колонок - букву C/F шириной 3 не рисовать - тогда 11 + 3 = 14;
      if (pWIDTH <= 14 && show_tempCF && show_tempDG) {
        show_tempCF = false;
      }

      temperatureW = 0;      
            
      // Буква C/F
      if (show_tempCF) {
        // Температура в малых часах, когда показываются градусы и/или C/F
        temperatureW += 3;
        temperatureX_CF_pos = right - 2;                     // позиция C/F
      } else {
        temperatureX_CF_pos = right + 1;        
      }

      if (show_tempDG) {
        // Температура в малых часах, когда показываются градусы и/или C/F
        temperatureW += 2;
        temperatureX_DG_pos = temperatureX_CF_pos - 2;       // позиция градусов 
      } else {
        temperatureX_DG_pos = temperatureX_CF_pos;
      }

      // Единицы градусов
      if (edc_t == 1) {
        temperatureX_T01_pos = temperatureX_DG_pos -  (show_tempCF || show_tempDG ? 3 : 2);      // единицы градусов == 1
        temperatureW += show_tempCF || show_tempDG ? (use_round_3x5 ? 4 : 3) : (use_round_3x5 ? 3 : 2);
      } else {
        temperatureX_T01_pos = temperatureX_DG_pos - (show_tempCF || show_tempDG ? 4 : 3);      // единицы градусов != 1
        temperatureW += (show_tempCF || show_tempDG ? 4 : 3);
      }     
      
      if (dec_t > 0) {                                 
        // Десятки градусов больше нуля - отображаются
        
        if (dec_t == 1) {
          temperatureX_T10_pos = temperatureX_T01_pos - (edc_t == 1 ? 3 : 3);     // десятки градусов не равны 0
          temperatureX_SG_pos = temperatureX_T10_pos - (use_round_3x5 ? 4 : 3);      // Позиция знака, если десятки градусов == 1
          temperatureW += (edc_t == 1 ? 6 : (use_round_3x5 ? 7 : 6));
        } else {
          temperatureX_T10_pos = temperatureX_T01_pos - (use_round_3x5 ? 4 : (edc_t == 1 ? 3 : 4));     // десятки градусов не равны 0
          temperatureX_SG_pos = temperatureX_T10_pos - 4;      // Позиция знака, если десятки градусов != 1
          temperatureW += (edc_t == 1 ? 7 : 8);
        }          
      } else {
        // Десятки градусов равны 0 - не отображаются 
        if (edc_t == 1) {
          temperatureX_SG_pos = temperatureX_T01_pos - (use_round_3x5 ? 4 : 3);      // Позиция знака, если единицы градусов == 1
          temperatureW += 3;
        } else {
          temperatureX_SG_pos = temperatureX_T01_pos - 4;      // Позиция знака, если единицы градусов != 1
          temperatureW += 4;
        }          
      }
    }

    // Полная ширина температуры с отображением значка градуса и буквы C/F = 11 + 6 = 17;
    // Если не влазим по ширине - удаляем пробер между десятками градусов и знаком +/-
    if (pWIDTH <= 16 && show_tempCF && show_tempDG && dec_t > 1) {
      temperatureX_SG_pos++;
      temperatureW--;
    }
    
    temperatureH = 5;
    
  } else {
    
    if (t == 0) {
      
      show_tempCF = true;
      temperatureW = (isFarenheit ? 10 : 11);
      if (show_tempDG) temperatureW += 4;
      temperatureX_CF_pos = right - (isFarenheit ? 3 : 4);
      temperatureX_DG_pos = show_tempDG ? temperatureX_CF_pos - 4 : temperatureX_CF_pos;
      temperatureX_T01_pos = temperatureX_DG_pos - 6;
      
    } else {

      temperatureW = 0;
      right++;
            
      // Буква C/F
      if (show_tempCF) {
        // Температура в малых часах, когда показываются градусы и/или C/F
        temperatureW += (isFarenheit ? 5 : 6);
        temperatureX_CF_pos = right - (isFarenheit ? 3 : 4);
      } else {
        temperatureX_CF_pos = right + 2;        
      }

      if (show_tempDG) {
        // Температура в малых часах, когда показываются градусы и/или C/F
        temperatureW += 4;
        temperatureX_DG_pos = temperatureX_CF_pos - 4;       // позиция градусов 
      } else {
        temperatureX_DG_pos = temperatureX_CF_pos;
      }

      // Единицы градусов      
      if (edc_t == 1) {
        temperatureX_T01_pos = temperatureX_DG_pos - 5;      // единицы градусов == 1
        temperatureW += 3;
      } else {
        temperatureX_T01_pos = temperatureX_DG_pos - 6;      // единицы градусов != 1
        temperatureW += 5;
      }     

      if (dec_t > 0) {                                 
        // Десятки градусов больше нуля - отображаются
        if (dec_t == 1) {
          if (edc_t == 1) {
            temperatureX_T10_pos = temperatureX_T01_pos - 5;
            temperatureW += 6;
          } else {
            temperatureX_T10_pos = temperatureX_T01_pos - 5;
            temperatureW += 5;
          }
        } else {
          if (edc_t == 1) {
            temperatureX_T10_pos = temperatureX_T01_pos - 6;
            temperatureW += 7;
          } else {
            temperatureX_T10_pos = temperatureX_T01_pos - 6;
            temperatureW += 6;
          }
        }

        if (dec_t == 1) {
          temperatureX_SG_pos = temperatureX_T10_pos - 3;    // Позиция знака, если десятки градусов == 1
          temperatureW += 3;
        } else {
          temperatureX_SG_pos = temperatureX_T10_pos - 4;    // Позиция знака, если десятки градусов != 1
          temperatureW += 4;
        }          
                  
      } else {
        
        // Десятки градусов равны 0 - не отображаются 
        if (edc_t == 1) {
          temperatureX_SG_pos = temperatureX_T01_pos - 3;    // Позиция знака, если единицы градусов == 1
          temperatureW += 4;
        } else {
          temperatureX_SG_pos = temperatureX_T01_pos - 4;    // Позиция знака, если единицы градусов != 1
          temperatureW += 4;
        }          
        
      }
    }

    if (c_size != 1) {
      if (pWIDTH <= 24 && temperatureW > 24) { 
        // Если ширина матрицы <= 24 - нет пробела между градусом и "C" - сдвинуть все на одну позицию вправо
        temperatureX_DG_pos++;
        temperatureX_T01_pos++;
        temperatureX_T10_pos++;
        temperatureX_SG_pos++;
        temperatureW--;
      }
      if (pWIDTH <= 23 && temperatureW > 23) { 
        // Если ширина матрицы <= 23 - нет пробела между единицами градуса и градусом - сдвинуть все на одну позицию вправо
        temperatureX_T01_pos++;
        temperatureX_T10_pos++;
        temperatureX_SG_pos++;
        temperatureW--;
      }
    }

    temperatureH = 7;      
  }
        
  temperatureY = getTemperatureY();
  temperatureX = right - temperatureW + 1;

  // Центрировать температуру относительно линии CLOCK_XC
  int8_t offset  = (pWIDTH - temperatureW) / 2;
  int8_t offset2 = CLOCK_XC - (pWIDTH / 2);
  if (pWIDTH % 2 == 0 && temperatureW % 2 == 1) offset++;
  if (pWIDTH % 2 == 1 && temperatureW % 2 == 0) offset++;
  int8_t dx = offset - temperatureX + offset2;
  shiftTemperaturePosition(dx, 0);
}

#endif 

void calcCalendarPosition() {
  
  // Вычислить прямоугольник, в котором будет выведен календарь
  // Центр календаря (точка между числом и месяцем) отрисовывается по текущему центру отображения - CLOCK_XC

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
//uint8_t d2 = (pYear / 100) % 10;
//uint8_t d3 = (pYear / 10) % 10;
  uint8_t d4 =  pYear % 10;
  
  calendarY = getCalendarY();
  calendarX_DT_pos = CLOCK_XC;
  
  // Позиция цифр по ВЕРТИКАЛИ
  // Вертикальный календарь - в два ряда по 4 цифры для матриц >= 23 колонки - ДД.MM/ГГГГ
  // или для матриц < 23 колонок - в два ряда по 2 цифры - ДД/MM
  if (c_size == 1) {
    
    // Шрифт 3x5 + 1 разделительная строка
    calendarY_D10_pos = calendarY_D01_pos = calendarY_M10_pos = calendarY_M01_pos = calendarY + 6;
    calendarY_Y1000_pos = calendarY_Y0100_pos = calendarY_Y0010_pos = calendarY_Y0001_pos = calendarY;
    
  } else {
    
    // Шрифт 5x7 + 1 разделительная строка
    if (pWIDTH >= 23) {
      
      // Влазят все 4 цифры года
      calendarY_D10_pos = calendarY_D01_pos = calendarY_M10_pos = calendarY_M01_pos = calendarY + 8;
      calendarY_Y1000_pos = calendarY_Y0100_pos = calendarY_Y0010_pos = calendarY_Y0001_pos = calendarY;      
      
    } else {
      
      // Влязят только день в верхней строке, месяц - в нижней
      calendarY_D10_pos = calendarY_D01_pos = calendarY + 8;
      calendarY_M10_pos = calendarY_M01_pos = calendarY;      
      
    }
    
  }    

  // Позиция цифр по ГОРИЗОНТАЛИ
  if (c_size == 1) {
    
    // Шрифт 3x5 - календарь в 2 строки - вверху ДД/ММ, внизу ГГГГ
    calendarW           = 15;
    calendarX_D10_pos   = CLOCK_XC - 7;
    calendarX_D01_pos   = CLOCK_XC - 3;
    calendarX_M01_pos   = CLOCK_XC + 5;
    calendarX_M10_pos   = CLOCK_XC + 1;     
    calendarX_Y1000_pos = CLOCK_XC - 7;
    calendarX_Y0100_pos = CLOCK_XC - 3;
    calendarX_Y0010_pos = CLOCK_XC + 1;
    calendarX_Y0001_pos = CLOCK_XC + 5;    
    calendarX           = calendarX_D10_pos;      

    // Если и день (десятки) и год (тысячи == 1) - 1 узкая цифра, сдвигаем позицию календаря по X, уменьшаем общую ширину
    if (d10 == 1 && d1 == 1) {
      calendarX++;
      calendarW--;
    }    

    // Если и месяц (единицы) и год (единицы == 1) - 1 узкая цифра, уменьшаем общую ширину
    if (m01 == 1 && d4 == 1) {
      calendarW--;
    }    

  } else {
    
    // Шрифт 5x7 
    if (pWIDTH < 23) {
      
      // Ширина матрицы менее 23 колонок - календарь в 2 ряда. Вверху - ДД, внизу - ММ
      calendarW           = 11;
      calendarX_D10_pos   = CLOCK_XC - 5;
      calendarX_D01_pos   = CLOCK_XC + 1;
      calendarX_M10_pos   = CLOCK_XC - 5;
      calendarX_M01_pos   = CLOCK_XC + 1;    
      calendarX           = calendarX_D10_pos;      
      
      // Если и день (десятки) и месяц (десятки == 1) - 1 узкая цифра, сдвигаем позицию календаря по X, уменьшаем общую ширину
      if (d10 == 1 && m10 == 1) {
        calendarX++;
        calendarW--;
      }    
  
      // Если и день (единицы) и месяц (единицы == 1) - 1 узкая цифра, уменьшаем общую ширину
      if (d01 == 1 && m01 == 1) {
        calendarW--;
      }    
       
    } else {
      
      // Ширина матрицы 23 колонки и шире - календарь в 2 ряда. Вверху - ДД.MM, внизу - ГГГГ
      calendarW           = 23;
      calendarX_D10_pos   = CLOCK_XC - 11;
      calendarX_D01_pos   = CLOCK_XC - 5;
      calendarX_M10_pos   = CLOCK_XC + 1;     
      calendarX_M01_pos   = CLOCK_XC + 7;
      calendarX_Y1000_pos = CLOCK_XC - 11;
      calendarX_Y0100_pos = CLOCK_XC - 5;
      calendarX_Y0010_pos = CLOCK_XC + 1;
      calendarX_Y0001_pos = CLOCK_XC + 7;    
      calendarX           = calendarX_D10_pos;      

      // Если и день (десятки) и год (тысячи == 1) - 1 узкая цифра, сдвигаем позицию календаря по X, уменьшаем общую ширину
      if (d10 == 1 && d1 == 1) {
        calendarX++;
        calendarW--;
      }    
  
      // Если и месяц (единицы) и год (единицы == 1) - 1 узкая цифра, уменьшаем общую ширину
      if (m01 == 1 && d4 == 1) {
        calendarW--;
      }    
      
    }
  }    
  
  calendarH = (c_size == 1 ? 11 : 15);
}


void shiftClockPosition(int8_t dx, int8_t dy) {
  if (dx != 0) {
    clockX         += dx;
    clockX_H10_pos += dx;
    clockX_H01_pos += dx;
    clockX_M10_pos += dx;
    clockX_M01_pos += dx;
    clockX_dot_pos += dx;
  }  
  
  if (dy != 0) {
    clockY         += dy;
    clockY_H10_pos += dy;
    clockY_H01_pos += dy;
    clockY_M10_pos += dy;
    clockY_M01_pos += dy;
  }
}

#if (USE_WEATHER == 1)      

void shiftTemperaturePosition(int8_t dx, int8_t dy) {
  if (dx != 0) {
    temperatureX         += dx; 
    temperatureX_T10_pos += dx; 
    temperatureX_T01_pos += dx;
    temperatureX_SG_pos  += dx;  
    temperatureX_DG_pos  += dx;
    temperatureX_CF_pos  += dx;
  }
  temperatureY         += dy;
}

#endif

void shiftCalendarPosition(int8_t dx, int8_t dy) {
  if (dx != 0) {
    calendarX           += dx;
    calendarX_D10_pos   += dx;
    calendarX_D01_pos   += dx;
    calendarX_M10_pos   += dx;
    calendarX_M01_pos   += dx;
    calendarX_Y0001_pos += dx;
    calendarX_Y0010_pos += dx;
    calendarX_Y0100_pos += dx;
    calendarX_Y1000_pos += dx;
    calendarX_DT_pos += dx;
  }

  if (dy != 0) {
    calendarY           += dy;
    calendarY_D10_pos   += dy;
    calendarY_D01_pos   += dy;
    calendarY_M10_pos   += dy;
    calendarY_M01_pos   += dy;
    calendarY_Y1000_pos += dy;
    calendarY_Y0100_pos += dy;
    calendarY_Y0010_pos += dy;
    calendarY_Y0001_pos += dy;
  }
}


// нарисовать часы
void drawClock() {
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
      if (show_clockH10_zero) drawDigit3x5_s(h10, getClockX(clockX_H10_pos), clockY_H10_pos, clockLED[0]);    
      drawDigit3x5_s(h01, getClockX(clockX_H01_pos), clockY_H01_pos, clockLED[1]);
      drawDigit3x5_s(m10, getClockX(clockX_M10_pos), clockY_M10_pos, clockLED[3]);
      drawDigit3x5_s(m01, getClockX(clockX_M01_pos), clockY_M01_pos, clockLED[4]);
    } else {
      if (show_clockH10_zero) drawDigit3x5(h10, getClockX(clockX_H10_pos), clockY_H10_pos, clockLED[0]);    
      drawDigit3x5(h01, getClockX(clockX_H01_pos), clockY_H01_pos, clockLED[1]);
      drawDigit3x5(m10, getClockX(clockX_M10_pos), clockY_M10_pos, clockLED[3]);
      drawDigit3x5(m01, getClockX(clockX_M01_pos), clockY_M01_pos, clockLED[4]);
    }

    // Разделительные точки часов - минут
    if (dotFlag) {
      if (CLOCK_ORIENT == 0) {
        // Горизонтальные часы
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 1, clockLED[2]);
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 3, clockLED[2]);
      } else {
        // Вертикальные часы
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 5, clockLED[2]);
      }
    }
    
  } else {

    // Большие часы (горизонтальные и вертикальные)
    if (show_clockH10_zero) drawDigit5x7(h10, getClockX(clockX_H10_pos), clockY_H10_pos, clockLED[0]);
    drawDigit5x7(h01, getClockX(clockX_H01_pos), clockY_H01_pos, clockLED[1]);
    drawDigit5x7(m10, getClockX(clockX_M10_pos), clockY_M10_pos, clockLED[3]);
    drawDigit5x7(m01, getClockX(clockX_M01_pos), clockY_M01_pos, clockLED[4]);

    // Разделительные точки часов - минут
    if (dotFlag) {
      if (CLOCK_ORIENT == 0) {
        // Горизонтальные часы
        // Для ширины матрицы в 25 колонок - рисовать одинарные точки разделения часов/минут, если больше - сдвоенные
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 1, clockLED[2]);         
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 2, clockLED[2]);
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 4, clockLED[2]);
        drawPixelXY(getClockX(clockX_dot_pos), clockY + 5, clockLED[2]);
        // Для ширины матрицы в 26 колонок - рисовать одинарные точки разделения часов/минут
        if (pWIDTH > 25) {
          drawPixelXY(getClockX(clockX_dot_pos + 1), clockY + 1, clockLED[2]);
          drawPixelXY(getClockX(clockX_dot_pos + 1), clockY + 2, clockLED[2]);
          drawPixelXY(getClockX(clockX_dot_pos + 1), clockY + 4, clockLED[2]);
          drawPixelXY(getClockX(clockX_dot_pos + 1), clockY + 5, clockLED[2]);
        }
      } else {
        // Вертикальные часы
        for (uint8_t i = 0; i < 3; i++) {
          drawPixelXY(getClockX(clockX_dot_pos + i), clockY + 7, clockLED[2]);        
        }
      }
    }    
    
  }

  // Рисовать границы прямоугольника области часов
  if (debug) {
    drawPixelXY(getClockX(clockX), clockY, CRGB::Green);      
    drawPixelXY(getClockX(clockX), clockY + clockH - 1, CRGB::Green);      
    drawPixelXY(getClockX(clockX + clockW - 1), clockY, CRGB::Red);
    drawPixelXY(getClockX(clockX + clockW - 1), clockY + clockH - 1, CRGB::Red);
  }  
}

#if (USE_WEATHER == 1)      

void drawTemperature() {

  // Вычисление позиции отрисовки температуры относительно центральной линии матрицы.

  bool   debug = debug_hours >= 0 && debug_mins >= 0;
  int8_t tempr = debug ? debug_temperature : temperature;

  int8_t  th = (isFarenheit ? (round(tempr * 9 / 5) + 32) : tempr);
  uint8_t t = abs(th);
  uint8_t dec_t = t / 10;
  uint8_t edc_t = t % 10;

  // Получить цвет отображения значения температуры
  CRGB color;          
  if (isNightClock) 
    color = useTemperatureColorNight ? (th < -3 ? nightClockBrightness : (th > 3 ? nightClockBrightness << 16 : nightClockBrightness << 16 | nightClockBrightness << 8 | nightClockBrightness)) : getNightClockColorByIndex(nightClockColor);
  else
    color = useTemperatureColor ? CRGB(HEXtoInt(getTemperatureColor(th))) : clockLED[0]; 

  if (c_size == 1) {

    // Градусы C/F
    if (show_tempCF) {
    
      // При температуре = 0 - рисуем маленький значок C / F
      if (!(useTemperatureColor || isNightClock)) color = clockLED[2];     
      
      if (isFarenheit) {
        // буква F
        for(uint8_t i = 0; i < 5; i++) {
          drawPixelXY(getClockX(temperatureX_CF_pos), temperatureY + i, color);      
        }
        for(uint8_t i = 0; i < 2; i++) {
          drawPixelXY(getClockX(temperatureX_CF_pos + 1 + i), temperatureY + 4, color); 
        }     
        drawPixelXY(getClockX(temperatureX_CF_pos + 1), temperatureY + 2, color);              
      } else {
        // буква C
        if (use_round_3x5) {
          for(uint8_t i = 1; i < 3; i++) {
            drawPixelXY(getClockX(temperatureX_CF_pos), temperatureY + i, color);      
          }
        } else {
          for(uint8_t i = 0; i < 4; i++) {
            drawPixelXY(getClockX(temperatureX_CF_pos), temperatureY + i, color);      
          }
        }
        drawPixelXY(getClockX(temperatureX_CF_pos + 1), temperatureY, color);      
        drawPixelXY(getClockX(temperatureX_CF_pos + 2), temperatureY, color);      
        drawPixelXY(getClockX(temperatureX_CF_pos + 1), temperatureY + 3, color);      
        drawPixelXY(getClockX(temperatureX_CF_pos + 2), temperatureY + 3, color);      
      }
    }

    // Значок градуса
    if (show_tempDG) {
      // При температуре = 0 - рисуем маленький значок C / F
      if (!(useTemperatureColor || isNightClock)) color = clockLED[2];     
      
      drawPixelXY(getClockX(temperatureX_DG_pos), temperatureY + 4, color);      
      drawPixelXY(getClockX(temperatureX_DG_pos), temperatureY + 3, color);      
      drawPixelXY(getClockX(temperatureX_DG_pos + 1), temperatureY + 4, color);      
      drawPixelXY(getClockX(temperatureX_DG_pos + 1), temperatureY + 3, color);            
    }

    // Единицы градусов
    if (!(useTemperatureColor || isNightClock)) color = clockLED[0]; 
    if (use_round_3x5)
      drawDigit3x5_s(edc_t, getClockX(temperatureX_T01_pos), temperatureY, color);
    else
      drawDigit3x5(edc_t, getClockX(temperatureX_T01_pos), temperatureY, color);
    
    // Десятки градусов
    if (dec_t != 0) {
      if (!(useTemperatureColor || isNightClock)) color = clockLED[0]; 
      if (use_round_3x5)
        drawDigit3x5_s(dec_t, getClockX(temperatureX_T10_pos), temperatureY, color);
      else  
        drawDigit3x5(dec_t, getClockX(temperatureX_T10_pos), temperatureY, color);
    }

    // Нарисовать '+' или '-' если температура не 0
    // Горизонтальная черта - общая для '-' и '+'
    if (th != 0) {
      if (!(useTemperatureColor || isNightClock)) color = clockLED[3]; 
      for(uint8_t i = 0; i < 3; i++) {
        drawPixelXY(getClockX(temperatureX_SG_pos + i), temperatureY + 2, color);      
      }
      
      // Для плюcа - вертикальная черта
      if (th > 0) {
        drawPixelXY(getClockX(temperatureX_SG_pos + 1), temperatureY + 1, color);
        drawPixelXY(getClockX(temperatureX_SG_pos + 1), temperatureY + 3, color);
      }
    }    
    
  } else {

    // Буква 'C'/'F'
    if (show_tempCF) {
      if (!(useTemperatureColor || isNightClock))color = clockLED[2]; 
      if (isFarenheit) {
        for(uint8_t i=0; i<7; i++) drawPixelXY(getClockX(temperatureX_CF_pos), temperatureY + i, color);
        for(uint8_t i=0; i<3; i++) drawPixelXY(getClockX(temperatureX_CF_pos + 1 + i), temperatureY + 6, color);
        for(uint8_t i=0; i<2; i++) drawPixelXY(getClockX(temperatureX_CF_pos + 1 + i), temperatureY + 3, color);
      } else {
        for(uint8_t i=0; i<5; i++) drawPixelXY(getClockX(temperatureX_CF_pos), temperatureY + 1 + i, color);
        for(uint8_t i=0; i<3; i++) drawPixelXY(getClockX(temperatureX_CF_pos + 1 + i), temperatureY, color);
        for(uint8_t i=0; i<3; i++) drawPixelXY(getClockX(temperatureX_CF_pos + 1 + i), temperatureY + 6, color);
        drawPixelXY(getClockX(temperatureX_CF_pos + 4), temperatureY + 5, color);
        drawPixelXY(getClockX(temperatureX_CF_pos + 4), temperatureY + 1, color);
      }
    }
    
    // Знак градусов; если ширина <= 24 - пробела между градусом и С нет
    if (show_tempDG) {
      if (!(useTemperatureColor || isNightClock)) color = clockLED[2]; 
      drawPixelXY(getClockX(temperatureX_DG_pos),     temperatureY + 5, color);
      drawPixelXY(getClockX(temperatureX_DG_pos),     temperatureY + 4, color);
      drawPixelXY(getClockX(temperatureX_DG_pos + 1), temperatureY + 6, color);
      drawPixelXY(getClockX(temperatureX_DG_pos + 1), temperatureY + 3, color);
      drawPixelXY(getClockX(temperatureX_DG_pos + 2), temperatureY + 5, color);
      drawPixelXY(getClockX(temperatureX_DG_pos + 2), temperatureY + 4, color);
    }
    
    // Единицы температуры; если ширина <= 23 - пробела между цифрой и градусом нет
    if (!(useTemperatureColor || isNightClock))color = clockLED[0]; 
    drawDigit5x7(edc_t, getClockX(temperatureX_T01_pos), temperatureY, color);

    // Десятки температуры
    if (dec_t != 0) {
    if (!(useTemperatureColor || isNightClock))color = clockLED[0]; 
      drawDigit5x7(dec_t, getClockX(temperatureX_T10_pos), temperatureY, color);
    }

    // Если температура не нулевая - рисуем знак '+' или '-'
    if (th != 0) {
      if (!(useTemperatureColor || isNightClock))color = clockLED[3]; 
      for(uint8_t i=0; i<3; i++) drawPixelXY(getClockX(temperatureX_SG_pos + i), temperatureY + 3, color);
      if (th > 0) {
        for(uint8_t i=0; i<3; i++) drawPixelXY(getClockX(temperatureX_SG_pos + 1), temperatureY + 2 + i, color);
      }
    }    
  }

  // Рисовать границы прямоугольника области температуры
  if (debug) {
    drawPixelXY(getClockX(temperatureX), temperatureY, CRGB::Green);      
    drawPixelXY(getClockX(temperatureX), temperatureY + temperatureH - 1, CRGB::Green);      
    drawPixelXY(getClockX(temperatureX + temperatureW - 1), temperatureY, CRGB::Red);
    drawPixelXY(getClockX(temperatureX + temperatureW - 1), temperatureY + temperatureH - 1, CRGB::Red);
  }  
  
}

#endif

// нарисовать дату календаря
void drawCalendar() {

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
      drawDigit3x5_s(d10, getClockX(calendarX_D10_pos), calendarY_D10_pos, clockLED[0]);    
      drawDigit3x5_s(d01, getClockX(calendarX_D01_pos), calendarY_D01_pos, clockLED[0]);
      drawDigit3x5_s(m10, getClockX(calendarX_M10_pos), calendarY_M10_pos, clockLED[1]);
      drawDigit3x5_s(m01, getClockX(calendarX_M01_pos), calendarY_M01_pos, clockLED[1]);
      // Нижний ряд - ГГГГ  
      if (allow_two_row) {
        drawDigit3x5_s(d1, getClockX(calendarX_Y1000_pos), calendarY_Y1000_pos, clockLED[3]);    
        drawDigit3x5_s(d2, getClockX(calendarX_Y0100_pos), calendarY_Y0100_pos, clockLED[3]);
        drawDigit3x5_s(d3, getClockX(calendarX_Y0010_pos), calendarY_Y0010_pos, clockLED[4]);
        drawDigit3x5_s(d4, getClockX(calendarX_Y0001_pos), calendarY_Y0001_pos, clockLED[4]);
      }

    } else {
      
      // Верхий ряд - ДД.MM
      drawDigit3x5(d10, getClockX(calendarX_D10_pos), calendarY_D10_pos, clockLED[0]);    
      drawDigit3x5(d01, getClockX(calendarX_D01_pos), calendarY_D01_pos, clockLED[0]);
      drawDigit3x5(m10, getClockX(calendarX_M10_pos), calendarY_M10_pos, clockLED[1]);
      drawDigit3x5(m01, getClockX(calendarX_M01_pos), calendarY_M01_pos, clockLED[1]);
      // Нижний ряд - ГГГГ  
      if (allow_two_row) {
        drawDigit3x5(d1, getClockX(calendarX_Y1000_pos), calendarY_Y1000_pos, clockLED[3]);    
        drawDigit3x5(d2, getClockX(calendarX_Y0100_pos), calendarY_Y0100_pos, clockLED[3]);
        drawDigit3x5(d3, getClockX(calendarX_Y0010_pos), calendarY_Y0010_pos, clockLED[4]);
        drawDigit3x5(d4, getClockX(calendarX_Y0001_pos), calendarY_Y0001_pos, clockLED[4]);
      }

    }

    // Разделительная точка между числом и месяцем
    if (dotFlag) {
      drawPixelXY(getClockX(calendarX_DT_pos), calendarY_D01_pos + 1, clockLED[2]);
      drawPixelXY(getClockX(calendarX_DT_pos), calendarY_D01_pos,     clockLED[2]);
    }
    
  } else {
    
    if (pWIDTH < 23) {
      
      // Ширина матрицы позволяет рисовать только день ДД (число) вверху и месяц MM внизу
      drawDigit5x7(d10, getClockX(calendarX_D10_pos), calendarY_D10_pos, clockLED[0]);
      drawDigit5x7(d01, getClockX(calendarX_D01_pos), calendarY_D01_pos, clockLED[1]);
      drawDigit5x7(m10, getClockX(calendarX_M10_pos), calendarY_M10_pos, clockLED[3]);
      drawDigit5x7(m01, getClockX(calendarX_M01_pos), calendarY_M01_pos, clockLED[4]);

      // Разделительная точка
      if (dotFlag) {
        drawPixelXY(getClockX(calendarX_DT_pos - 1), calendarY_D10_pos - 1, clockLED[2]);
        drawPixelXY(getClockX(calendarX_DT_pos + 1), calendarY_D10_pos - 1, clockLED[2]);
      }
      
    } else {
      
      // Ширина матрицы позволяет рисовать день и месяц ДД.MM вверху, год ГГГГ внизу
      
      drawDigit5x7(d10, getClockX(calendarX_D10_pos), calendarY_D10_pos, clockLED[0]);
      drawDigit5x7(d01, getClockX(calendarX_D01_pos), calendarY_D01_pos, clockLED[0]);
      drawDigit5x7(m10, getClockX(calendarX_M10_pos), calendarY_M10_pos, clockLED[1]);
      drawDigit5x7(m01, getClockX(calendarX_M01_pos), calendarY_M01_pos, clockLED[1]);

      drawDigit5x7(d1, getClockX(calendarX_Y1000_pos), calendarY_Y1000_pos, clockLED[3]);
      drawDigit5x7(d2, getClockX(calendarX_Y0100_pos), calendarY_Y0100_pos, clockLED[3]);
      drawDigit5x7(d3, getClockX(calendarX_Y0010_pos), calendarY_Y0010_pos, clockLED[4]);
      drawDigit5x7(d4, getClockX(calendarX_Y0001_pos), calendarY_Y0001_pos, clockLED[4]);      
      
      // Разделительная точка
      if (dotFlag) {
        drawPixelXY(getClockX(calendarX_DT_pos), calendarY_D10_pos,     clockLED[2]);
        drawPixelXY(getClockX(calendarX_DT_pos), calendarY_D10_pos + 1, clockLED[2]);
      }
    }
  }

  // Рисовать границы прямоугольника области часов
  if (debug) {
    drawPixelXY(getClockX(calendarX), calendarY, CRGB::Green);      
    drawPixelXY(getClockX(calendarX), calendarY + calendarH - 1, CRGB::Green);      
    drawPixelXY(getClockX(calendarX + calendarW - 1), calendarY, CRGB::Red);
    drawPixelXY(getClockX(calendarX + calendarW - 1), calendarY + calendarH - 1, CRGB::Red);
  }  

}

void clockRoutine() {
  
  if (loadingFlag) {
    
    loadingFlag = false;
    // modeCode = MC_CLOCK;
    
    if (!init_time && !isNightClock && !manualMode) {
      nextMode();
      return;
    }
  }

  // Очистка экрана. Сама отрисовка часов производится как
  // наложение оверлея на черный экран в doEffectWithOverlay()
  FastLED.clear();  
}

void clockTicker() {  

  hrs = hour();
  mins = minute();
  aday = day();
  amnth = month();
  ayear = year();

  if (isTurnedOff && needTurnOffClock && init_time) {
    #if (USE_TM1637 == 1)
    if (display != nullptr) {
      currDisplay[0] = _empty; // display->displayByte(_empty, _empty, _empty, _empty);
      currDisplay[1] = _empty;
      currDisplay[2] = _empty;
      currDisplay[3] = _empty;
      currDotState = false;    // display->point(false);
    }
    #endif
    return;
  }

  bool halfSec = halfsecTimer.isReady();
  if (halfSec) {    
    clockHue += HUE_STEP;
    setOverlayColors();
    dotFlag = !dotFlag;
  }

#if (USE_TM1637 == 1)

  if (display != nullptr) {
    if (isButtonHold || bCounter > 0) {
      // Удержание кнопки - изменение яркости + 2 сек после того как кнопка отпущена - 
      // отображать показание текущего значения яркости в процентах 0..99
      if (isButtonHold) bCounter = 4;
      if (!isButtonHold && bCounter > 0 && halfSec) bCounter--;
      uint8_t prcBrightness = map8(globalBrightness,0,99);
      uint8_t m10 = getByteForDigit(prcBrightness / 10);
      uint8_t m01 = getByteForDigit(prcBrightness % 10);
      currDisplay[0] = _b_;  // display->displayByte(_b_, _r_, m10, m01);
      currDisplay[1] = _r_;
      currDisplay[2] = m10;
      currDisplay[3] = m01;
      currDotState = false;  // display->point(false);
      
    } else if (wifi_print_ip) {
      // Четырехкратное нажатие кнопки запускает отображение по частям текущего IP лампы  
      if (dotFlag && halfSec) {
        if (wifi_print_idx <= 3) {
          String ip(WiFi.localIP().toString());
          int16_t value = atoi(GetToken(ip, wifi_print_idx + 1, '.').c_str()); 
          display->encodeInt(value, currDisplay);   // display->displayInt(value);
          currDotState = false;                     // display->point(false);
          wifi_print_idx++;
        } else {
          wifi_print_idx = 0; 
          wifi_print_ip = false;
        }
      }
    } else {
      // Если время еще не получено - отображать прочерки
      if (!init_time) {
        if (halfSec) {          
          currDisplay[0] = _dash;  // display->displayByte(_dash, _dash, _dash, _dash);
          currDisplay[1] = _dash;
          currDisplay[2] = _dash;
          currDisplay[3] = _dash;
        }
      } else if (!isAlarmStopped && (isPlayAlarmSound || isAlarming)) {
        // Сработал будильник (звук) - плавное мерцание текущего времени      
        if (halfSec) {
          uint8_t hh = hour();
          if (time_h12) {
            if (hh > 12) hh -= 12;
            if (hh == 0) hh = 12;
          }
          display->encodeClock(hh, minute(), currDisplay);   // display->displayClock(hour(),minute());
        }
        if (millis() - fade_time > 65) {
          fade_time = millis();
          currDisplayBrightness = aCounter; // display->setBrightness(aCounter);        
          if (aDirection) aCounter++; else aCounter--;
          if (aCounter > 7) {
            aDirection = false;
            aCounter = 7;
          }
          if (aCounter == 0) {
            aDirection = true;
          }
        }
      } else {
        // Время получено - отображать часы:минуты  
        #if (USE_WEATHER == 1)
        if (useWeather > 0 && init_weather && weather_ok && (((second() + 10) % 30) >= 28)) {
          int8_t  th = (isFarenheit ? (round(temperature * 9 / 5) + 32) : temperature);
          uint8_t t = abs(th);
          uint8_t atH = t / 10;
          uint8_t atL = t % 10;
          currDotState = false;   // display->point(false);
          if (atH == 0) {            
            currDisplay[0] = _empty;  // display->displayByte(_empty, (th >= 0) ? _empty : _dash, display->encodeDigit(atL), _degree);
            currDisplay[1] = (th >= 0) ? _empty : _dash;
            currDisplay[2] = display->encodeDigit(atL);
            currDisplay[3] = _degree;
          }
          else {            
            currDisplay[0] = (th >= 0) ? _empty : _dash;  // display->displayByte((th >= 0) ? _empty : _dash, display->encodeDigit(atH), display->encodeDigit(atL), _degree);
            currDisplay[1] = display->encodeDigit(atH);
            currDisplay[2] = display->encodeDigit(atL);
            currDisplay[3] = _degree;
          }
        } else 
        #endif
        {
          uint8_t hh = hour();
          if (time_h12) {
            if (hh > 12) hh -= 12;
            if (hh == 0) hh = 12;
          }
          display->encodeClock(hh, minute(), currDisplay);   // display->displayClock(hour(),minute());   
          // Отображение часов - разделительное двоеточие...
          if (halfSec) currDotState = dotFlag;  // display->point(dotFlag);
        }
        currDisplayBrightness = isTurnedOff ? 1 : 7; // display->setBrightness(isTurnedOff ? 1 : 7);
      }
    }
  }
#endif  
}

void checkCalendarState() {
  if (millis() - showDateStateLastChange > (showDateState ? showDateDuration : showDateInterval) * 1000L) {
    showDateStateLastChange = millis();
    showDateState = !showDateState;
    // В однострочных часах чередовать отображение даты и температуры
    if (!allow_two_row && init_weather && showDateState) {
      if (showDateInClock) {
        showWeatherState = !showWeatherState;
      } else {
        showWeatherState = showDateState;
      }
    }
  }  
}

void contrastClock() {  
  for (uint8_t i = 0; i < 5; i++) clockLED[i] = NORMAL_CLOCK_COLOR;
}

void contrastClockA() {  
  for (uint8_t i = 0; i < 5; i++) clockLED[i] = CONTRAST_COLOR_1;
}

void contrastClockB() {
  for (uint8_t i = 0; i < 5; i++) clockLED[i] = CONTRAST_COLOR_2;
}

void contrastClockC(){
  CRGB color = CONTRAST_COLOR_3;
  CRGB gc = CRGB(globalColor);
  if (color == gc) color = -color;
  for (uint8_t i = 0; i < 5; i++) clockLED[i] = color;  
}

void setOverlayColors() {
  if (COLOR_MODE == 0 && !(thisMode == MC_CLOCK || thisMode == MC_TEXT || thisMode == MC_FILL_COLOR || thisMode == MC_COLORS)) {
    switch (thisMode) {
      case MC_SNOW:
      case MC_NOISE_ZEBRA: 
      case MC_NOISE_MADNESS:
      case MC_NOISE_CLOUD:
      case MC_NOISE_FOREST:
      case MC_NOISE_OCEAN: 
      case MC_RAIN:      
        contrastClockA(); // оранжевые
        break;
      case MC_NOISE_LAVA:
        contrastClockB(); // зеленые
        break;
      case MC_DAWN_ALARM:
        contrastClockC(); // желтые или инверсные
        break;      
      default:
        contrastClock();  // белые
        break;
    }
  } else {
    clockColor();
  }
}

void calculateDawnTime() {

  uint8_t alrmHour;
  uint8_t alrmMinute;
  
  dawnWeekDay = 0;
  if (!init_time || alarmWeekDay == 0) return;       // Время инициализировано? Хотя бы на один день будильник включен?

  int8_t alrmWeekDay = weekday();                    // day of the week, Sunday is day 0   
  if (alrmWeekDay == 0) alrmWeekDay = 7;             // Sunday is day 7, Monday is day 1;

  uint8_t h = hour();
  uint8_t m = minute();
  uint8_t w = weekday();
  if (w == 0) w = 7;

  uint8_t cnt = 0;
  
  while (cnt < 2) {
    cnt++;
    while ((alarmWeekDay & (1 << (alrmWeekDay - 1))) == 0) {
      alrmWeekDay++;
      if (alrmWeekDay == 8) alrmWeekDay = 1;
    }
      
    alrmHour = alarmHour[alrmWeekDay-1];
    alrmMinute = alarmMinute[alrmWeekDay-1];
  
    // "Сегодня" время будильника уже прошло? 
    if (alrmWeekDay == w && (h * 60L + m > alrmHour * 60L + alrmMinute)) {
      if (alrmWeekDay == w && cnt == 0) alrmWeekDay++;
      if (alrmWeekDay == 8) alrmWeekDay = cnt == 1 ? 1 : 7;
    }
  }

  // расчёт времени рассвета
  if (alrmMinute > dawnDuration) {                  // если минут во времени будильника больше продолжительности рассвета
    dawnHour = alrmHour;                            // час рассвета равен часу будильника
    dawnMinute = alrmMinute - dawnDuration;         // минуты рассвета = минуты будильника - продолж. рассвета
    dawnWeekDay = alrmWeekDay;
  } else {                                          // если минут во времени будильника меньше продолжительности рассвета
    dawnWeekDay = alrmWeekDay;
    dawnHour = alrmHour - 1;                        // значит рассвет будет часом раньше
    if (dawnHour < 0) {
      dawnHour = 23;     
      dawnWeekDay--;
      if (dawnWeekDay == 0) dawnWeekDay = 7;                           
    }
    dawnMinute = 60 - (dawnDuration - alrmMinute);  // находим минуту рассвета в новом часе
    if (dawnMinute == 60) {
      dawnMinute=0; dawnHour++;
      if (dawnHour == 24) {
        dawnHour=0; dawnWeekDay++;
        if (dawnWeekDay == 8) dawnWeekDay = 1;
      }
    }
  }

  DEBUG(F("Следующий рассвет в "));
  DEBUG(padNum(dawnHour,2));
  DEBUG(':');
  DEBUG(padNum(dawnMinute,2));
  DEBUG(", ");
  DEBUGLN(getWeekdayString(dawnWeekDay));
}

// Проверка времени срабатывания будильника
void checkAlarmTime() {

  uint8_t h = hour();
  uint8_t m = minute();
  uint8_t w = weekday();
  if (w == 0) w = 7;

  // Будильник включен?
  if (init_time && dawnWeekDay > 0) {

    // Время срабатывания будильника после завершения рассвета
    uint8_t alrmWeekDay = dawnWeekDay;
    uint8_t alrmHour = dawnHour;
    uint8_t alrmMinute = dawnMinute + dawnDuration;
    if (alrmMinute >= 60) {
      alrmMinute = 60 - alrmMinute;
      alrmHour++;
    }
    if (alrmHour > 23) {
      alrmHour = 24 - alrmHour;
      alrmWeekDay++;
    }
    if (alrmWeekDay > 7) alrmWeekDay = 1;

    // Текущий день недели совпадает с вычисленным днём недели рассвета?
    if (w == dawnWeekDay) {
       // Часы / минуты начала рассвета наступили? Еще не запущен рассвет? Еще не остановлен пользователем?
       if (!isAlarming && !isAlarmStopped && ((w * 1000L + h * 60L + m) >= (dawnWeekDay * 1000L + dawnHour * 60L + dawnMinute)) && ((w * 1000L + h * 60L + m) < (alrmWeekDay * 1000L + alrmHour * 60L + alrmMinute))) {
         // Сохранить параметры текущего режима для восстановления после завершения работы будильника
         saveSpecialMode = specialMode;
         saveSpecialModeId = specialModeId;
         saveMode = thisMode;
         // Включить будильник
         set_CurrentSpecialMode(-1);
         set_isAlarming(true);
         set_isAlarmStopped(false);
         loadingFlag = true;  
         #if (USE_BUTTON == 1 || USE_TM1637 == 1)       
         isButtonHold = false;
         #endif
         set_thisMode(MC_DAWN_ALARM);
         setTimersForMode(thisMode);
         // Реальная продолжительность рассвета
         realDawnDuration = (alrmHour * 60L + alrmMinute) - (dawnHour * 60L + dawnMinute);
         if (realDawnDuration > dawnDuration) realDawnDuration = dawnDuration;
         // Отключить таймер автоперехода в демо-режим
         DEBUGLN(F("Переход в авторежим отключен"));
         idleTimer.stopTimer();
         #if (USE_MP3 == 1)
         if (useAlarmSound) PlayDawnSound();
         #endif
         DEBUG(F("Рассвет ВКЛ в ")); DEBUG(padNum(h,2)); DEBUG(":"); DEBUGLN(padNum(m,2));

         doc.clear();
         doc["act"]   = F("ALARM");
         doc["state"] = F("on");
         doc["type"]  = F("dawn");
         
         String out;
         serializeJson(doc, out);      
         doc.clear();
         
         SendWeb(out, TOPIC_ALM);
       }
    }
    
    yield();
    
    // При наступлении времени срабатывания будильника, если он еще не выключен пользователем - запустить режим часов и звук будильника
    if (alrmWeekDay == w && alrmHour == h && alrmMinute == m && isAlarming) {
      DEBUG(F("Рассвет Авто-ВЫКЛ в ")); DEBUG(padNum(h,2)); DEBUG(":"); DEBUGLN(padNum(m,2));

      set_isAlarming(false);
      set_isAlarmStopped(false);
      set_isPlayAlarmSound(true);
      setSpecialMode(1);      
      alarmSoundTimer.setInterval(alarmDuration * 60000L);
      alarmSoundTimer.reset();
      // Играть звук будильника
      // Если звук будильника не используется - просто запустить таймер.
      // До окончания таймера индикатор TM1637 будет мигать, лампа гореть ярко белым.
      #if (USE_MP3 == 1)
      if (useAlarmSound) {
        PlayAlarmSound();
      }
      #endif      
      
      doc.clear();
      doc["act"]   = F("ALARM");
      doc["state"] = F("on");
      doc["type"]  = F("alarm");

      String out;
      serializeJson(doc, out);      
      doc.clear();

      SendWeb(out, TOPIC_ALM);
    }

    yield();

    // Если рассвет начинался и остановлен пользователем и время начала рассвета уже прошло - сбросить флаги, подготовив их к следующему циклу
    if (isAlarmStopped && ((w * 1000L + h * 60L + m) > (alrmWeekDay * 1000L + alrmHour * 60L + alrmMinute + alarmDuration))) {
      set_isAlarming(false);
      set_isAlarmStopped(false);
      StopSound(0);
    }
  }
  
  // Подошло время отключения будильника - выключить, вернуться в предыдущий режим
  if (alarmSoundTimer.isReady()) {

    // Во время работы будильника индикатор плавно мерцает.
    // После завершения работы - восстановить яркость индикатора
    #if (USE_TM1637 == 1)
    if (display != nullptr) {
      currDisplayBrightness = 7; // display->setBrightness(7);
    }
    #endif
    DEBUG(F("Будильник Авто-ВЫКЛ в ")); DEBUG(padNum(h,2)); DEBUG(":"); DEBUGLN(padNum(m,2));

    alarmSoundTimer.stopTimer();
    set_isPlayAlarmSound(false);
    StopSound(1000);   

    calculateDawnTime();
    
    resetModes();  
    setManualModeTo(false);

    if (saveSpecialMode){

      setSpecialMode(saveSpecialModeId);
      
      // Если во время срабатывания будильника шла трансляция с мастера, после остановки будильника, если устройство выключено -
      // специальный режим выключит его и сбросит яркость в 0, так что возобновленная трансляция будет не видна.
      // Восстановить уровень яркости, чтобы трансляция нормально отображалась
      if (isTurnedOff && streaming) {
        // Кадр выведен на экран - очистить матрицу перед вормированием нового кадра
        // Это нужно для случая -  кадр автономной работы по размерам больше кадра получаемого с мастера
        // Если не очищать матрицу - при включении вещания мы получим изображение с мастера, но в остальной области останется
        // изображение кадра с автономной работы           
        FastLED.clear();  
        FastLED.setBrightness(getMaxBrightness());
      }

    } else {             
       setEffect(saveMode);       
    }

    doc.clear();
    doc["act"]   = F("ALARM");
    doc["state"] = F("off");
    doc["type"]  = F("auto");

    String out;
    serializeJson(doc, out);      
    doc.clear();

    SendWeb(out, TOPIC_ALM);
  }

  yield();

  #if (USE_MP3 == 1)
  // Плавное изменение громкости будильника
  if (fadeSoundTimer.isReady()) {
    if (fadeSoundDirection > 0) {
      // увеличение громкости
      dfPlayer.increaseVolume(); Delay(GUARD_DELAY);
      fadeSoundStepCounter--;
      if (fadeSoundStepCounter <= 0) {
        fadeSoundDirection = 0;
        fadeSoundTimer.stopTimer();
      }
    } else if (fadeSoundDirection < 0) {
      // Уменьшение громкости
      dfPlayer.decreaseVolume(); Delay(GUARD_DELAY);
      fadeSoundStepCounter--;
      if (fadeSoundStepCounter <= 0) {
        set_isPlayAlarmSound(false);
        fadeSoundDirection = 0;
        fadeSoundTimer.stopTimer();
        StopSound(0);
      }
    }
  }
  #endif
    
  yield();
}

void stopAlarm() {
  if ((isAlarming || isPlayAlarmSound) && !isAlarmStopped) {
    DEBUG(F("Рассвет ВЫКЛ в "));
    DEBUG(padNum(hour(),2));
    DEBUG(':');
    DEBUGLN(padNum(minute(),2));
    
    set_isAlarming(false);
    set_isAlarmStopped(true);
    set_isPlayAlarmSound(false);

    alarmSoundTimer.stopTimer();

    // Во время работы будильника индикатор плавно мерцает.
    // После завершения работы - восстановить яркость индикатора

    #if (USE_TM1637 == 1)
    if (display != nullptr) {
      currDisplayBrightness = 7; // display->setBrightness(7);
    }
    #endif

    StopSound(1000);

    resetModes();  
    setManualModeTo(false);
    calculateDawnTime();
    
    if (saveSpecialMode){
      setSpecialMode(saveSpecialModeId);
      // Если во время срабатывания будильника шла трансляция с мастера, после остановки будильника, если устройство выключено -
      // специальный режим выключит его и сбросит яркость в 0, так что возобновленная трансляция будет не видна.
      // Восстановить уровень яркости, чтобы трансляция нормально отображалась
      if (isTurnedOff && streaming) {
       FastLED.setBrightness(getMaxBrightness());
      }
    } else {
       setEffect(saveMode);
    }
    yield();

    doc.clear();
    doc["act"]   = F("ALARM");
    doc["state"] = F("off");
    doc["type"]  = F("stop");

    String out;
    serializeJson(doc, out);      
    doc.clear();

    // Если идет прием вещания и был рассвет / будильник - картинка будильника рисуется в приоритете.
    // После остановки будильника восстанавливается вывод картинки вещания, которая по размеру может быть 
    // меньше чем целая матрица. Чтобы остатки картинки будильника стереть с экрана - очистить матрицу
    
    #if (USE_E131 == 1)
    if (workMode == SLAVE && streaming) {
      FastLED.clear();
    }
    #endif
    
    SendWeb(out, TOPIC_ALM);
  }
}

// Проверка необходимости включения режима 1 по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void checkAutoMode1Time() {

  hrs = hour();
  mins = minute();
  sec = second();

  // Пришло время и режим "Использовать" включен?
  if (AM1_hour == hrs && AM1_minute == mins && sec < 2) {
    if ((auxLineModes & 1) != 0) {
      bool newAuxActive = (auxLineModes & 2) != 0;
      if (newAuxActive != isAuxActive) {
        set_isAuxActive(newAuxActive);
        saveSettings();
      }
    }
  }

  if (AM1_effect_id <= -3 || AM1_effect_id >= MAX_EFFECT || !init_time) return;
  
  // Режим по времени включен (enable) и настало время активации режима - активировать
  if (!AM1_running && AM1_hour == hrs && AM1_minute == mins) {
    AM1_running = true;
    SetAutoMode(1);
  }

  // Режим активирован и время срабатывания режима прошло - сбросить флаг для подготовки к следующему циклу
  if (AM1_running && (AM1_hour != hrs || AM1_minute != mins)) {
    AM1_running = false;
  }
}

// Проверка необходимости включения режима 2 по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void checkAutoMode2Time() {

  hrs = hour();
  mins = minute();
  sec = second();

  // Пришло время и режим "Использовать" включен?
  if (AM2_hour == hrs && AM2_minute == mins && sec < 2) {
    if ((auxLineModes & 4) != 0) {
      bool newAuxActive = (auxLineModes & 8) != 0;
      if (newAuxActive != isAuxActive) {
        set_isAuxActive(newAuxActive);
        saveSettings();
      }
    }
  }
  
  // Действие отличается от "Нет действия" и время установлено?
  if (AM2_effect_id <= -3 || AM2_effect_id >= MAX_EFFECT || !init_time) return;

  // Если сработал будильник - рассвет - режим не переключать - остаемся в режиме обработки будильника
  if ((isAlarming || isPlayAlarmSound) && !isAlarmStopped) return;

  // Режим по времени включен (enable) и настало время активации режима - активировать
  if (!AM2_running && AM2_hour == hrs && AM2_minute == mins) {
    AM2_running = true;
    SetAutoMode(2);
  }

  // Режим активирован и время срабатывания режима прошло - сбросить флаг для подготовки к следующему циклу
  if (AM2_running && (AM2_hour != hrs || AM2_minute != mins)) {
    AM2_running = false;
  }
}

// Проверка необходимости включения режима 1 по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void checkAutoMode3Time() {

  hrs = hour();
  mins = minute();
  sec = second();

  // Пришло время и режим "Использовать" включен?
  if (AM3_hour == hrs && AM3_minute == mins && sec < 2) {
    if ((auxLineModes & 16) != 0) {
      bool newAuxActive = (auxLineModes & 32) != 0;
      if (newAuxActive != isAuxActive) {
        set_isAuxActive(newAuxActive);
        saveSettings();
      }
    }
  }
  
  if (AM3_effect_id <= -3 || AM3_effect_id >= MAX_EFFECT || !init_time) return;
  
  // Режим по времени включен (enable) и настало время активации режима - активировать
  if (!AM3_running && AM3_hour == hrs && AM3_minute == mins) {
    AM3_running = true;
    SetAutoMode(3);
  }

  // Режим активирован и время срабатывания режима прошло - сбросить флаг для подготовки к следующему циклу
  if (AM3_running && (AM3_hour != hrs || AM3_minute != mins)) {
    AM3_running = false;
  }
}

// Проверка необходимости включения режима 1 по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void checkAutoMode4Time() {
  hrs = hour();
  mins = minute();
  sec = second();

  // Пришло время и режим "Использовать" включен?
  if (AM4_hour == hrs && AM4_minute == mins && sec < 2) {
    if ((auxLineModes & 64) != 0) {
      bool newAuxActive = (auxLineModes & 128) != 0;
      if (newAuxActive != isAuxActive) {
        set_isAuxActive(newAuxActive);
        saveSettings();
      }
    }
  }

  if (AM4_effect_id <= -3 || AM4_effect_id >= MAX_EFFECT || !init_time) return;
  
  // Режим по времени включен (enable) и настало время активации режима - активировать
  if (!AM4_running && AM4_hour == hrs && AM4_minute == mins) {
    AM4_running = true;
    SetAutoMode(4);
  }

  // Режим активирован и время срабатывания режима прошло - сбросить флаг для подготовки к следующему циклу
  if (AM4_running && (AM4_hour != hrs || AM4_minute != mins)) {
    AM4_running = false;
  }
}

#if (USE_WEATHER == 1)  
// Проверка необходимости включения режима "Рассвет" по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void checkAutoMode5Time() {

  hrs = hour();
  mins = minute();
  sec = second();

  // Пришло время и режим "Использовать" включен?
  if (dawn_hour == hrs && dawn_minute == mins && sec < 2) {
    if ((auxLineModes & 256) != 0) {
      bool newAuxActive = (auxLineModes & 512) != 0;
      if (newAuxActive != isAuxActive) {
        set_isAuxActive(newAuxActive);
        saveSettings();
      }
    }
  }
  
  if (dawn_effect_id <= -3 || dawn_effect_id >= MAX_EFFECT || !init_weather || useWeather == 0) return;
  
  // Режим по времени включен (enable) и настало время активации режима - активировать
  if (!dawn_running && dawn_hour == hrs && dawn_minute == mins) {
    dawn_running = true;
    SetAutoMode(5);
  }

  // Режим активирован и время срабатывания режима прошло - сбросить флаг для подготовки к следующему циклу
  if (dawn_running && (dawn_hour != hrs || dawn_minute != mins)) {
    dawn_running = false;
  }
}

// Проверка необходимости включения режима "Закат" по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы; 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void checkAutoMode6Time() {
  
  hrs = hour();
  mins = minute();
  sec = second();

  // Пришло время и режим "Использовать" включен?
  if (dusk_hour == hrs && dusk_minute == mins && sec < 2) {
    if ((auxLineModes & 1024) != 0) {
      bool newAuxActive = (auxLineModes & 2048) != 0;
      if (newAuxActive != isAuxActive) {
        set_isAuxActive(newAuxActive);
        saveSettings();
      }
    }
  }

  if (dusk_effect_id <= -3 || dusk_effect_id >= MAX_EFFECT || !init_weather || useWeather == 0) return;
  
  // Режим по времени включен (enable) и настало время активации режима - активировать
  if (!dusk_running && dusk_hour == hrs && dusk_minute == mins) {
    dusk_running = true;
    SetAutoMode(6);
  }

  // Режим активирован и время срабатывания режима прошло - сбросить флаг для подготовки к следующему циклу
  if (dusk_running && (dusk_hour != hrs || dusk_minute != mins)) {
    dusk_running = false;
  }
}
#endif  

// Выполнение включения режима 1,2,3,4,5,6 (amode) по установленному времени
// -3 - не используется; -2 - выключить матрицу; -1 - ночные часы, 0 - включить случайный с автосменой; 1 - номер режима из списка EFFECT_LIST
void SetAutoMode(uint8_t amode) {

  uint8_t   AM_hour, AM_minute;
  int8_t AM_effect_id;
  switch(amode) {
    case 1:  AM_hour = AM1_hour;  AM_minute = AM1_minute;  AM_effect_id = AM1_effect_id;  break;
    case 2:  AM_hour = AM2_hour;  AM_minute = AM2_minute;  AM_effect_id = AM2_effect_id;  break;
    case 3:  AM_hour = AM3_hour;  AM_minute = AM3_minute;  AM_effect_id = AM3_effect_id;  break;
    case 4:  AM_hour = AM4_hour;  AM_minute = AM4_minute;  AM_effect_id = AM4_effect_id;  break;
    case 5:  AM_hour = dawn_hour; AM_minute = dawn_minute; AM_effect_id = dawn_effect_id; break;
    case 6:  AM_hour = dusk_hour; AM_minute = dusk_minute; AM_effect_id = dusk_effect_id; break;
    default: return;
  }
  
  bool   no_action = false;
  
  String mode_name;
  if (amode == 5)
    mode_name += F("'Рассвет'");
  else if (amode == 6)    
    mode_name += F("'Закат'");
  else  
    mode_name += amode;
    
  String text(F("Авторежим "));
  text += mode_name; text += " ["; text += padNum(AM_hour,2); text += ':'; text += padNum(AM_minute,2); text += "] - ";

  int8_t ef = AM_effect_id;

  //ef: -3 - нет действия; 
  //    -2 - выключить панель (черный экран); 
  //    -1 - ночные часы; 
  //     0 - случайный,
  //     1 и далее - эффект из EFFECT_LIST по списку

  // включить указанный режим
  if (ef <= -3 || ef >= MAX_EFFECT) {
    no_action = true;
    text += F("нет действия");
  } else if (ef == -2) {

    // Выключить матрицу (черный экран)
    text += F("выключение панели");
    turnOff();

  } else if (ef == -1) {

    // Ночные часы
    text += F("ночные часы");
    setSpecialMode(3);
    
  } else {
    
    text += F("включение режима ");    
    // Если режим включения == 0 - случайный режим и автосмена по кругу
    resetModes();  
    setManualModeTo(ef != 0);    

    if (ef == 0) {
      // "Случайный" режим и далее автосмена
      text += F("демонстрации эффектов:");
      uint32_t cnt = CountTokens(EFFECT_LIST, ','); 
      ef = random8(0, cnt); 
    } else {
      ef -= 1; // Приведение номера эффекта (номер с 1) к индексу в массиве EFFECT_LIST (индекс c 0)
    }

    text += F(" эффект "); text += '\''; text += GetToken(EFFECT_LIST, ef + 1, ','); text +=  '\'';

    // Включить указанный режим из списка доступных эффектов без дальнейшей смены
    // Значение ef может быть 0..N-1 - указанный режим из списка EFFECT_LIST (приведенное к индексу с 0)      
    setEffect(ef);

    if (manualMode) {
      putCurrentManualMode(ef);
    }
  }

  if (!no_action) {
    DEBUGLN(text);  

    doc.clear();
    doc["act"]       = F("AUTO");
    doc["mode"]      = amode;
    doc["mode_name"] = mode_name;
    doc["text"]      = text;

    String out;
    serializeJson(doc, out);      
    doc.clear();
    
    SendWeb(out, TOPIC_AMD);
  }
}

#if (USE_TM1637 == 1)
uint8_t getByteForDigit(uint8_t digit) {
  switch (digit) {
    case 0: return _0_;
    case 1: return _1_;
    case 2: return _2_;
    case 3: return _3_;
    case 4: return _4_;
    case 5: return _5_;
    case 6: return _6_;
    case 7: return _7_;
    case 8: return _8_;
    case 9: return _9_;
    default: return _empty;
  }
}
#endif

void checkClockOrigin() {

  // Получить текущий тип размера горизонтальных часов: 0 - авто; 1 - шрифт 3х5; 2 - шрифт 5х7;
  c_size = getClockSizeType();
  
  allowVertical   = pWIDTH >= 7  && pHEIGHT >= 11;
  allowHorizontal = pWIDTH >= 15 && pHEIGHT >= 5;

  if (allowVertical || allowHorizontal) {
    // Если ширина матрицы не позволяет расположить часы горизонтально - переключить в вертикальный режим
    if (CLOCK_ORIENT == 1 && !allowVertical) {
      set_CLOCK_ORIENT(0);
    }
    // Если высота матрицы не позволяет расположить часы вертикально - переключить в горизонтальный режим
    if (CLOCK_ORIENT == 0 && !allowHorizontal) {
      set_CLOCK_ORIENT(1);
    }
  } else {
    // Если отображать часы невозможно - отключить отображение часов поверх эффектов
    set_clockOverlayEnabled(false);
    return;
  }

  // Можно ли отображать горизонтальные часы вместе с температурой и календарь - дата с годом - это требует двух строк
  // Если в две строки отображать нельзя - календарь отображается без года, температура - не вместе с часами, а чередуется с календарем
  allow_two_row = (c_size == 1)
    ? pHEIGHT >= 11  // Малые часы - шрифт 3x5 - две строки + пробел между строками - высота 11 строк 
    : pHEIGHT >= 15; // Большие часы - шрифт 5x7 - две строки + пробел между строками - высота 15 строк
}

uint32_t getNightClockColorByIndex(uint8_t idx) {
  // nightClockBrightness - яркость каждой компоненты цвета ночных часов - R,G,B
  // Нужный свет получаем комбинируя яркость каждого из компонентов RGB цвета
  uint32_t color = nightClockBrightness << 16;  // Red
  switch (idx) {
    case 0: color = nightClockBrightness << 16; break;  // Red
    case 1: color = nightClockBrightness << 8;  break;  // Green
    case 2: color = nightClockBrightness;       break;  // Blue
    case 3: color = nightClockBrightness << 8  | nightClockBrightness; break;  // Cyan
    case 4: color = nightClockBrightness << 16 | nightClockBrightness; break;  // Magenta
    case 5: color = nightClockBrightness << 16 | nightClockBrightness << 8; break;  // Yellow
    case 6: color = nightClockBrightness << 16 | nightClockBrightness << 8 | nightClockBrightness; break;  // White
  }  
  return color;
}
