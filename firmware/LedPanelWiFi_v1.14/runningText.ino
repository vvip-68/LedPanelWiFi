// --------------------- ДЛЯ РАЗРАБОТЧИКОВ ----------------------

void InitializeTexts() {

  String directoryName(TEXT_STORAGE);
  
  textIndecies = F("##");
  textWithEvents = "";
  textsNotEmpty = "";
  TEXTS_MAX_COUNT = 0;                    // Пока не проверена файловая система - считать что строк недоступно - 0
    
  if (!LittleFS.exists(directoryName)) {
    if (!LittleFS.mkdir(directoryName)) {
      DEBUG(String(MSG_FOLDER_CREATE_ERROR)); DEBUG(F(" '")); DEBUG(directoryName); DEBUGLN('\'');
      return;
    }
  }

  TEXTS_MAX_COUNT = 36;  // Длина - 36 - столько строк поддeрживается - '0'..'9','A'..'Z'

  // получить строку статусов массива строк текстов бегущей строки
  //  0 - серый - пустая
  //  1 - черный - отключена
  //  2 - зеленый - активна - просто текст, без макросов
  //  3 - фиолетовый - активна, содержит макросы кроме даты
  //  4 - синий - активная, содержит макрос даты
  //  5 - красный - для строки 0 - это управляющая строка
  
  char buf[TEXTS_MAX_COUNT + 1];
  memset(buf, '\0', TEXTS_MAX_COUNT + 1);
  
  for (uint8_t i=0; i < TEXTS_MAX_COUNT; i++) {
    char charIndex = getAZIndex(i);
    String fileName(directoryName); fileName += '/'; fileName += charIndex;
    if (INITIALIZE_TEXTS == 1 && !LittleFS.exists(fileName)) {
      switch (i) {
        case 0:  saveTextLine('0', textLine_0); break;
        case 1:  saveTextLine('1', textLine_1); break;
        case 2:  saveTextLine('2', textLine_2); break;
        case 3:  saveTextLine('3', textLine_3); break;
        case 4:  saveTextLine('4', textLine_4); break;
        case 5:  saveTextLine('5', textLine_5); break;
        case 6:  saveTextLine('6', textLine_6); break;
        case 7:  saveTextLine('7', textLine_7); break;
        case 8:  saveTextLine('8', textLine_8); break;
        case 9:  saveTextLine('9', textLine_9); break;
        case 10: saveTextLine('A', textLine_A); break;
        case 11: saveTextLine('B', textLine_B); break;
        case 12: saveTextLine('C', textLine_C); break;
        case 13: saveTextLine('D', textLine_D); break;
        case 14: saveTextLine('E', textLine_E); break;
        case 15: saveTextLine('F', textLine_F); break;
        case 16: saveTextLine('G', textLine_G); break;
        case 17: saveTextLine('H', textLine_H); break;
        case 18: saveTextLine('I', textLine_I); break;
        case 19: saveTextLine('J', textLine_J); break;
        case 20: saveTextLine('K', textLine_K); break;
        case 21: saveTextLine('L', textLine_L); break;
        case 22: saveTextLine('M', textLine_M); break;
        case 23: saveTextLine('N', textLine_N); break;
        case 24: saveTextLine('O', textLine_O); break;
        case 25: saveTextLine('P', textLine_P); break;
        case 26: saveTextLine('Q', textLine_Q); break;
        case 27: saveTextLine('R', textLine_R); break;
        case 28: saveTextLine('S', textLine_S); break;
        case 29: saveTextLine('T', textLine_T); break;
        case 30: saveTextLine('U', textLine_U); break;
        case 31: saveTextLine('V', textLine_V); break;
        case 32: saveTextLine('W', textLine_W); break;
        case 33: saveTextLine('X', textLine_X); break;
        case 34: saveTextLine('Y', textLine_Y); break;
        case 35: saveTextLine('Z', textLine_Z); break; 
      }
    }

    String text(getTextByIndex(i));

    // Проверить строку на наличие макроса события {P}/ Если есть добавить ч строку индексов строк с отслеживаемыми событиями
    if (text.indexOf("{P") >= 0) {
      textWithEvents += charIndex;
    }
    
    // Для строки с индексом 0 проверить - является ли она управляющей
    if (i == 0) {     
      textIndecies = text;
      if (!isFirstLineControl()) textIndecies = "";
    }    
    buf[i] = getTextStateChar(i, text);

    // Если строка не пустая - добавить ее в строку индексов непустых строк
    // В дальнейшем для определения какую строку показывать это позволит избежать 
    // открытие файлов и чтение их содержимого чтобы понять пустая строка или нужно искать другую.
    if (text.length() > 0) {
      textsNotEmpty += charIndex;
    }
  }  
  
  textStates =  String(buf);
  sequenceIdx = isFirstLineControl() ? 1 : -1;
}

char getTextStateChar(uint8_t i, const String& text) {
  char c = '0';    // статус - пустая
  if (text.length() > 0) {
    if (i == 0 && text[0] == '#')
      c = '5';     // статус - строка 0 - управляющая последовательность
    else if (text[0] == '-' || text.indexOf("{-}") >=0)
      c = '1';     // статус - отключена
    else if (text.indexOf("{") < 0)
      c = '2';     // статус - текст без макросов
    else if (text.indexOf("{") >= 0 && text.indexOf("{D") < 0 && text.indexOf("{R") < 0 && text.indexOf("{P") < 0 && text.indexOf("{S") < 0)
      c = '3';     // статус - текст с макросами, но без дато-зависимых макросов
    else if (text.indexOf("{D") >= 0 || text.indexOf("{R") >= 0 || text.indexOf("{P") >= 0 || text.indexOf("{S") >= 0)
      c = '4';     // статус - текст с дато-зависимыми макросами
  }
  return c;  
}

void runningText() {
  String text;
  if (thisMode == MC_TEXT) {
    FastLED.clear();    
    // Включен режим (не оверлей!) бегущей строки
    if (wifi_print_version) 
      text = FIRMWARE_VER;
    else if (wifi_print_ip_text) 
      text = wifi_current_ip;
    else if (init_time)
      text = clockCurrentText();
    else
      text = FIRMWARE_VER;
  } else {
    // Обычно отображаемые буквы - UTF-8 состоят из двух байт. Исключение из поддерживаемых символов - символ евро '€', который состоит из 4 байт - [226, 98, 0, 140] - занимает 4 позиции в строке.
    // Два лишних байта отображаются как пробел - пустое место. Если их не отображать (пропускать) - отображение бегущей строки обрывается (заканчивается) до того как последний символ скроется с экрана
    // Временное решение - заменить символ евро на двухбайтовый UTF-8, но который отсутствует в шрифте и отображать вместо него знак евро.
    if (currentText.indexOf("€") >= 0) {
      currentText.replace("€", "¶");
    }
    // Вывод текста оверлея бегущей строки
    textHasDateTime = init_time && (currentText.indexOf("{D") >= 0 || currentText.indexOf("{R") >= 0 || currentText.indexOf("{P") >= 0 || currentText.indexOf("{S") >= 0);  
    if (textHasDateTime) {
      text = processDateMacrosInText(currentText);          // Обработать строку, превратив макросы даты в отображаемые значения
      if (text.length() == 0) {                             // Если дата еще не инициализирована - вернет другую строку, не требующую даты
         fullTextFlag = true;                               // Если нет строк, не зависящих от даты - вернется пустая строка - отображать нечего
         return;
      }      
    } else {
      text = currentText;                                   // Строка не содержит изменяемых во времени компонентов - отобразить ее как есть
    }
  }

  fillString(text);
}

void fillString(const String& text) {
  
  if (loadingTextFlag) {
    offset = pWIDTH;   // перемотка в правый край
    // modeCode = MC_TEXT;
    loadingTextFlag = false;  
  }

  uint32_t color;

  // Для многоцветной строки начальный цвет хранится в textColor[0]
  // Задан ли специальный цвет отображения строки?
  // Если режим цвета - монохром (0) или задан неверно (>2) - использовать глобальный или специальный цвет
  
  uint16_t i = 0, j = 0, pos = 0, modif = 0, restSec = -1, restTextOffset = 0, restTextWidth = 0;
  bool showRestSeconds = false;
  String restSecStr;

  if (momentIdx >= 0 && momentTextIdx >= 0 && pTextCount > 0 && isPTextCentered) {
    // Отображаемый текст - с макросом {P} и один раз он уже отобразился полностью, 
    // текст содержит флаг отображения остатка секнд по центру матрицы и остаток времени до события - менее минуты
    // Теперь просто отображаем остчет оставшегося времени по центру матрицы

    // Сколько секунд осталось до события?
    restSec = moments[momentIdx].moment - now();
    showRestSeconds = restSec > 0 && restSec < 60;
    
    // Остаток секунд в виде строки
    restSecStr = padNum(restSec,2);

    // Получить ширину строки в точках, чтобы высчитать центр отображения на матрице
    restTextWidth = 2 * LET_WIDTH + SPACE;    

    // Начальная позиция вывода остатка секунд по центру экрана
    restTextOffset = ((pWIDTH - restTextWidth) / 2);
  }
  
  while (true) {

    // Если строка - многоцветная (содержит несколько макросов определения цвета) - определить каким цветом выводится текущая буква строки
    if (textHasMultiColor) {
      if (pos < (MAX_COLORS - 1) && i >= textColorPos[pos+1]) {      
        pos++;
      }      
      color = pos < MAX_COLORS ? textColor[pos] : globalTextColor;
    } else {
      // 0 - монохром: 1 - радуга; 2 -  каждая буква свой цвет
      color = COLOR_TEXT_MODE;
      if (color == 0 || color > 2) {
        color = useSpecialTextColor ? specialTextColor : globalTextColor;  
      }
    }

    if (showRestSeconds) {

      // Определились с цветом - выводим очередную цифру  
      drawLetter(j, restSecStr[i], 0, restTextOffset + j * (LET_WIDTH + SPACE), color);
      j++;
      
      if (restSecStr[i] == '\0') break;
      i++; 
      
    } else {

      // Определились с цветом - выводим очередную букву  
      if ((uint16_t)text[i] > 191) {    // работаем с русскими буквами!
        modif = (uint16_t)text[i];
      } else {
        drawLetter(j, text[i], modif, offset + j * (LET_WIDTH + SPACE), color);
        j++;
        modif = 0;
      }
      if (text[i] == '\0') break;
      i++;
      
    }

  }
  fullTextFlag = false;

  // Строка убежала?
  if (offset < -j * (LET_WIDTH + SPACE)) {
    offset = pWIDTH + 3;
    fullTextFlag = true;
    if (momentIdx >= 0 && momentTextIdx >= 0) pTextCount++;
  }      
  
}

uint8_t getTextY() {
  int8_t LH = LET_HEIGHT;
  if (LH > pHEIGHT) LH = pHEIGHT;
  int8_t offset_y = (pHEIGHT - LH) / 2;     // по центру матрицы по высоте
  #if (BIG_FONT == 2)
    offset_y--;                             // на 1 строку ниже для большого шрифта, иначе шрифт 8x13 не влазит по вертикали диакритич. символы на матрице 16x16
  #endif
  return offset_y; 
}

// -----------------------------------------------------------------------
// Отрисовка букв алфавита на матрице
// -----------------------------------------------------------------------

bool drawLetter(uint8_t index, uint8_t letter, uint8_t modif, int16_t offset, uint32_t color) {
  
  int8_t LH = LET_HEIGHT;
  if (LH > pHEIGHT) LH = pHEIGHT;

  int8_t start_pos = 0, finish_pos = LET_WIDTH;
  int8_t offset_y = getTextY();
  
  CRGB letterColor;
  if (color == 1) letterColor = CHSV(uint8_t(offset * 10), 255, 255);
  else if (color == 2) letterColor = CHSV(uint8_t(index * 30), 255, 255);
  else letterColor = color;

  if (offset < -LET_WIDTH || offset > pWIDTH) return false;
  if (offset < 0) start_pos = -offset;
  if (offset > (pWIDTH - LET_WIDTH)) finish_pos = pWIDTH - offset;

  for (uint8_t i = start_pos; i < finish_pos; i++) {
    int32_t  thisByte; // байт колонки i отображаемого символа шрифта или -1 если такого символа нет в шрифте
    uint16_t diasByte; // байт колонки i отображаемого диакритического символа
    int8_t   diasOffs; // смещение по Y отображения диакритического символа: diasOffs > 0 - позиция над основной буквой; diasOffs < 0 - позиция ниже основной буквы
    int32_t pn;        // номер пикселя в массиве leds[]
    
    if (MIRR_V) {
      thisByte = getFont(letter, modif, LET_WIDTH - 1 - i);
      diasByte = getDiasByte(letter, modif, LET_WIDTH - 1 - i);
    } else {
      thisByte = getFont(letter, modif, i);
      diasByte = getDiasByte(letter, modif, i);
    }

    // Если такого символа нет в шрифте - ничего не рисовать
    if (thisByte < 0) return false;
    
    diasOffs = getDiasOffset(letter, modif);

    for (uint8_t j = 0; j < LH; j++) {
      bool thisBit;

      if (MIRR_H) thisBit = thisByte & (1 << j);
      else        thisBit = thisByte & (1 << (LH - 1 - j));

      // рисуем столбец буквы шрифта (i - горизонтальная позиция, j - вертикальная)      
      if (thisBit) { 
        int8_t y = offset_y + j;
        if (y >= 0 && y < pHEIGHT) {
          pn = getPixelNumber(offset + i, offset_y + j);
          if (pn < NUM_LEDS) {
            leds[pn] = letterColor;
          }
        }
      }

      if (MIRR_H) thisBit = diasByte & (1 << j);
      else        thisBit = diasByte & (1 << (LH - 1 - j));

      // рисуем столбец диакритического символа (i - горизонтальная позиция, j - вертикальная)      
      if (thisBit) { 
        int8_t y = offset_y + j + diasOffs;
        if (y >= 0 && y < pHEIGHT) {
          pn = getPixelNumber(offset + i, y);
          if (pn >= 0 && pn < NUM_LEDS) {
            leds[pn] = letterColor;
          }
        }
      }
    }
  }
  return true;
}

#if (BIG_FONT == 0)
 // Шрифт меньше/равно 8 точек - достаточно байта
 #define read_char pgm_read_byte
#else
 // Шрифт меньше/равно 16 точек - достаточно двух байт (word)
 #define read_char pgm_read_word
#endif


// интерпретатор кода символа в массиве fontHEX (для Arduino IDE 1.8.* и выше)
int32_t getFont(uint8_t font, uint8_t modif, uint8_t row) {
  font = font - '0' + 16;   // перевод код символа из таблицы ASCII в номер согласно нумерации массива    

  //DEBUGLN("modif=" + String(modif) + "; font=" + String(font));

  if (font <= 94) {
    return read_char(&(fontHEX[font][row]));       // для английских букв и символов
  } else if (modif == 209 && font == 116) {        // є
    return read_char(&(fontHEX[161][row])); 
  } else if (modif == 209 && font == 118) {        // і
      return read_char(&(fontHEX[73][row])); 
  } else if (modif == 209 && font == 119) {        // ї
      return read_char(&(fontHEX[162][row])); 
  } else if (modif == 209 && font == 113) {        // ё
      return read_char(&(fontHEX[132][row])); 
  } else if (modif == 208 && font == 100) {        // Є
      return read_char(&(fontHEX[160][row])); 
  } else if (modif == 208 && font == 102) {        // І
    return read_char(&(fontHEX[41][row])); 
  } else if (modif == 208 && font == 103) {        // Ї
    return read_char(&(fontHEX[41][row])); 
  } else if (modif == 208 && font == 97) {         // Ё
    return read_char(&(fontHEX[100][row]));     
  } else if ((modif == 208 || modif == 209) && font >= 112 && font <= 159) {         // и пизд*ц для русских
    return read_char(&(fontHEX[font - 17][row]));
  } else if ((modif == 208 || modif == 209) && font >= 96 && font <= 111) {
    return read_char(&(fontHEX[font + 47][row]));
  } else if (modif == 194) {                                          
    switch (font) {
      case 129: return read_char(&(fontHEX[166][row]));                              // Знак ¡
      case 144: return read_char(&(fontHEX[159][row]));                              // Знак градуса '°'
      case 150: return read_char(&(fontHEX[165][row]));                              // Знак евро '€' - реально '¶' - замена в runningText(), т.к. работает с оно-двух-байтовыми символами UTF-8, а евро - 4 байта, что ломает алгоритм
      case 159: return read_char(&(fontHEX[167 ][row]));                              // Знак ¿
    }    
  } else if (modif == 195) {
    switch (font) {
      case 127: return read_char(&(fontHEX[163][row])); // ß - 195 127 - 163         // Индекс строки в массиве fontHex - 163
      case 100: return read_char(&(fontHEX[33][row]));  // Ä - 195 100 - 33
      case 118: return read_char(&(fontHEX[47][row]));  // Ö - 195 118 - 47
      case 124: return read_char(&(fontHEX[53][row]));  // Ü - 195 124 - 53
      case 132: return read_char(&(fontHEX[65][row]));  // ä - 195 132 - 65
      case 150: return read_char(&(fontHEX[79][row]));  // ö - 195 150 - 79
      case 156: return read_char(&(fontHEX[85][row]));  // ü - 195 156 - 85    
      case 97:  return read_char(&(fontHEX[33][row]));  // Á - 195 97  - 33          
      case 105: return read_char(&(fontHEX[37][row]));  // É - 195 105 - 37          
      case 109: return read_char(&(fontHEX[41][row]));  // Í - 195 109 - 41          
      case 115: return read_char(&(fontHEX[47][row]));  // Ó - 195 115 - 47          
      case 122: return read_char(&(fontHEX[53][row]));  // Ú - 195 122 - 53          
      case 113: return read_char(&(fontHEX[46][row]));  // Ñ - 195 114 - 46          
      case 129: return read_char(&(fontHEX[65][row]));  // á - 195 129 - 65
      case 137: return read_char(&(fontHEX[69][row]));  // é - 195 137 - 69
      case 141: return read_char(&(fontHEX[164][row])); // í - 195 141 - 164
      case 147: return read_char(&(fontHEX[79][row]));  // ó - 195 147 - 79
      case 154: return read_char(&(fontHEX[85][row]));  // ú - 195 154 - 85    
      case 145: return read_char(&(fontHEX[78][row]));  // ñ - 195 145 - 78      
    }
  } else if (modif == 196 || modif == 197) {                                         // Буквы литовского алфавита  Ą Č Ę Ė Į Š Ų Ū Ž ą č ę ė į š ų ū ž
    switch (font) {
      case 100: return read_char(&(fontHEX[33][row])); //Ą 196   100  -     33
      case 108: return read_char(&(fontHEX[35][row])); //Č 196   108  -     35
      case 120: return read_char(&(fontHEX[37][row])); //Ę 196   120  -     37
      case 118: return read_char(&(fontHEX[37][row])); //Ė 196   118  -     37
      case 142: return read_char(&(fontHEX[41][row])); //Į 196   142  -     41
      case 128: return read_char(&(fontHEX[51][row])); //Š 197   128  -     51
      case 146: return read_char(&(fontHEX[53][row])); //Ų 197   146  -     53
      case 138: return read_char(&(fontHEX[53][row])); //Ū 197   138  -     53
      case 157: return read_char(&(fontHEX[58][row])); //Ž 197   157  -     58
      case 101: return read_char(&(fontHEX[65][row])); //ą 196   101  -     65
      case 109: return read_char(&(fontHEX[67][row])); //č 196   109  -     67  
      case 121: return read_char(&(fontHEX[69][row])); //ę 196   121  -     69
      case 119: return read_char(&(fontHEX[69][row])); //ė 196   119  -     69
      case 143: return read_char(&(fontHEX[73][row])); //į 196   143  -     73
      case 129: return read_char(&(fontHEX[83][row])); //š 197   129  -     83
      case 147: return read_char(&(fontHEX[85][row])); //ų 197   147  -     85
      case 139: return read_char(&(fontHEX[85][row])); //ū 197   139  -     85
      case 158: return read_char(&(fontHEX[90][row])); //ž 197   158  -     90
    }
  }
  return -1;
}

uint16_t getDiasByte(uint8_t font, uint8_t modif, uint8_t row) {
  font = font - '0' + 16;   // перевод код символа из таблицы ASCII в номер согласно нумерации массива
    // 0 - Ё - две точки над буквой Ё Ä Ö Ü ё ä,ö,ü
    // 1 - Č - перевернутая крышечка над заглавной буквой Č Ž č ž
    // 2 - Ė - точка над заглавной буквой Ė ė
    // 3 - Ū - надстрочная черта над заглавной буквой Ū ū
    // 4 - Ą - хвостик снизу букв Ą ą Ę ę ų - смещение к правому краю буквы
    // 5 - Į - хвостик снизу букв Į į Ų     - по центру буквы    
    // 6 - Á - ударение над буквами Á, É, Í, Ó, Ú, á, é, í, ó, ú - по центру буквы    
    // 7 - Ñ - тильда над буквой Ñ  - по центру буквы    
  if ((modif == 208 && font == 97) || (modif == 195 && (font == 100 || font == 118 || font == 124))) {         // Ё, немец. Ä,Ö,Ü
    return read_char(&(diasHEX[0][row])); 
  } else if ((modif == 209 && font == 113) || (modif == 195 && (font == 132 || font == 150 || font == 156))) { // ё, немец. ä,ö,ü
    return read_char(&(diasHEX[0][row])); 
  } else if ((modif == 208) && font == 103) {      // Ї
    return read_char(&(diasHEX[0][row])); 
  } else if (modif == 195) {                                                          // Буквы испанского алфавита  Á, É, Í, Ó, Ú, Ü, Ñ, á, é, í, ó, ú, ü, ñ
    switch (font) {
      case 97:  return read_char(&(diasHEX[6][row])); //Á 195    97  -     33          
      case 105: return read_char(&(diasHEX[6][row])); //É 195    105 -     37          
      case 109: return read_char(&(diasHEX[6][row])); //Í 195    109 -     41          
      case 115: return read_char(&(diasHEX[6][row])); //Ó 195    115 -     47          
      case 122: return read_char(&(diasHEX[6][row])); //Ú 195    122 -     53          
      case 113: return read_char(&(diasHEX[7][row])); //Ñ 195    114 -     46          
      case 129: return read_char(&(diasHEX[6][row])); //á 195    129 -     65
      case 137: return read_char(&(diasHEX[6][row])); //é 195    137 -     69
      case 147: return read_char(&(diasHEX[6][row])); //ó 195    147 -     79
      case 154: return read_char(&(diasHEX[6][row])); //ú 195    154 -     85    
      case 145: return read_char(&(diasHEX[7][row])); //ñ 195    145 -     78      
    }
  } else if (modif == 196 || modif == 197) {                                           // Буквы литовского алфавита  Ą Č Ę Ė Į Š Ų Ū Ž ą č ę ė į š ų ū ž
    switch (font) {
      case 100: return read_char(&(diasHEX[4][row])); //Ą 196   100  -     33 
      case 108: return read_char(&(diasHEX[1][row])); //Č 196   108  -     35
      case 120: return read_char(&(diasHEX[4][row])); //Ę 196   120  -     37
      case 118: return read_char(&(diasHEX[2][row])); //Ė 196   118  -     37
      case 142: return read_char(&(diasHEX[5][row])); //Į 196   142  -     41
      case 128: return read_char(&(diasHEX[1][row])); //Š 197   128  -     51
      case 146: return read_char(&(diasHEX[5][row])); //Ų 197   146  -     53
      case 138: return read_char(&(diasHEX[3][row])); //Ū 197   138  -     53
      case 157: return read_char(&(diasHEX[1][row])); //Ž 197   157  -     58
      case 101: return read_char(&(diasHEX[4][row])); //ą 196   101  -     65
      case 109: return read_char(&(diasHEX[1][row])); //č 196   109  -     67  
      case 121: return read_char(&(diasHEX[4][row])); //ę 196   121  -     69
      case 119: return read_char(&(diasHEX[2][row])); //ė 196   119  -     69
      case 143: return read_char(&(diasHEX[5][row])); //į 196   143  -     73
      case 129: return read_char(&(diasHEX[1][row])); //š 197   129  -     83
      case 147: return read_char(&(diasHEX[4][row])); //ų 197   147  -     85
      case 139: return read_char(&(diasHEX[3][row])); //ū 197   139  -     85
      case 158: return read_char(&(diasHEX[1][row])); //ž 197   158  -     90      
    }
  }
  return 0;
}

int8_t getDiasOffset(uint8_t font, uint8_t modif) {
  font = font - '0' + 16;   // перевод код символа из таблицы ASCII в номер согласно нумерации массива
  if ((modif == 208 && font == 97) || (modif == 195 && (font == 100 || font == 118 || font == 124))) {         // Ё, немец. Ä,Ö,Ü
    return 3; 
  } else if ((modif == 209 && font == 113) || (modif == 195 && (font == 132 || font == 150 || font == 156))) { // ё, немец. ä,ö,ü
    #if (BIG_FONT == 0)
      return 1; 
    #else
      return 0; 
    #endif  
  } else if ((modif == 208) && font == 103) {      // Ї
    return 3; 
  } else if (modif == 195) {                       // Буквы испанского алфавита  Á, É, Í, Ó, Ú, Ü, Ñ, á, é, í, ó, ú, ü, ñ
    #if (BIG_FONT == 0)
      switch (font) {
        case 97:  return 2; //Á 195    97  -     33          
        case 105: return 2; //É 195    105 -     37          
        case 109: return 2; //Í 195    109 -     41          
        case 115: return 2; //Ó 195    115 -     47          
        case 122: return 2; //Ú 195    122 -     53          
        case 113: return 3; //Ñ 195    114 -     46          
        case 129: return 1; //á 195    129 -     65
        case 137: return 1; //é 195    137 -     69
        case 147: return 1; //ó 195    147 -     79
        case 154: return 0; //ú 195    154 -     85    
        case 145: return 1; //ñ 195    145 -     78      
      }
    #else
      switch (font) {
        case 97:  return 3; //Á 195    97  -     33          
        case 105: return 3; //É 195    105 -     37          
        case 109: return 3; //Í 195    109 -     41          
        case 115: return 3; //Ó 195    115 -     47          
        case 122: return 3; //Ú 195    122 -     53          
        case 113: return 3; //Ñ 195    114 -     46          
        case 129: return 0; //á 195    129 -     65
        case 137: return 0; //é 195    137 -     69
        case 147: return 0; //ó 195    147 -     79
        case 154: return 0; //ú 195    154 -     85    
        case 145: return 0; //ñ 195    145 -     78      
      }
    #endif
  } else if (modif == 196 || modif == 197) {       // Буквы литовского алфавита  Ą Č Ę Ė Į Š Ų Ū Ž ą č ę ė į š ų ū ž
    // Смещение надстрочных заглавных - 3
    // Смещение надстрочных маленьких букв - 0 или 1
    // Смещение подстрочного символа -1
    #if (BIG_FONT == 0)
      switch (font) {
        case 100: return -1; //Ą 196   100
        case 108: return  2; //Č 196   108
        case 120: return -1; //Ę 196   120
        case 118: return  3; //Ė 196   118
        case 142: return -1; //Į 196   142
        case 128: return  2; //Š 197   128
        case 146: return -1; //Ų 197   146
        case 138: return  3; //Ū 197   138
        case 157: return  2; //Ž 197   157
        case 101: return -1; //ą 196   101
        case 109: return  0; //č 196   109
        case 121: return -1; //ę 196   121
        case 119: return  1; //ė 196   119
        case 143: return -1; //į 196   143
        case 129: return  0; //š 197   129
        case 147: return -1; //ų 197   147
        case 139: return  1; //ū 197   139
        case 158: return  0; //ž 197   158
      }
    #else
      switch (font) {
        case 100: return -1; //Ą 196   100
        case 108: return  3; //Č 196   108
        case 120: return -1; //Ę 196   120
        case 118: return  3; //Ė 196   118
        case 142: return -1; //Į 196   142
        case 128: return  3; //Š 197   128
        case 146: return -1; //Ų 197   146
        case 138: return  3; //Ū 197   138
        case 157: return  3; //Ž 197   157
        case 101: return -1; //ą 196   101
        case 109: return  0; //č 196   109
        case 121: return -1; //ę 196   121
        case 119: return  0; //ė 196   119
        case 143: return -1; //į 196   143
        case 129: return  0; //š 197   129
        case 147: return -1; //ų 197   147
        case 139: return  0; //ū 197   139
        case 158: return  0; //ž 197   158
      }
    #endif
  }
  return 0;
}

// ------------- СЛУЖЕБНЫЕ ФУНКЦИИ --------------

// Сдвинуть позицию отображения строки
void shiftTextPosition() {
  offset -= TEXT_SHIFT;
}

// Получить / установить настройки отображения очередного текста бегущей строки
// Если нет строк, готовых к отображению (например все строки отключены) - вернуть false - 'нет готовых строк'
bool prepareNextText(const String& text) {  
  // Если есть активная строка текущего момента - отображать ее 
  int8_t nextIdx = momentTextIdx >= 0 ? momentTextIdx : nextTextLineIdx;

  textShowTime = -1;              // Если больше нуля - сколько времени отображать бегущую строку в секундах; Если 0 - используется textShowCount; В самой строке спец-макросом может быть указано кол-во секунд
  textShowCount = 1;              // Сколько раз прокручивать текст бегущей строки поверх эффектов; По умолчанию - 1; В самой строке спец-макросом может быть указано число 
  useSpecialTextColor = false;    // В текущей бегущей строке был задан цвет, которым она должна отображаться
  specialTextColor = 0xffffff;    // Цвет отображения бегущей строки, может быть указан макросом в строке. Если не указан - используются глобальные настройки цвета бегущей строки
  specialTextEffect = -1;         // Эффект, который нужно включить при начале отображения строки текста, может быть указан макросом в строке.  
  specialTextEffectParam = -1;    // Параметр для эффекта (см. выше). Например эффект MC_SDCARD имеет более 40 подэффектов. Номер подэффекта хранится в этой переменной, извлекается из макроса {E}
  nextTextLineIdx = -1;           // Какую следующую строку показывать, может быть указан макросом в строке. Если указан - интервал отображения игнорируется, строка показывается сразу;
  textHasDateTime = false;        // Строка имеет макрос отображения текущего времени - ее нужно пересчитывать каждый раз при отрисовке; Если макроса времени нет - рассчитать текст строки один раз на этапе инициализации
  textHasMultiColor = false;      // Строк имеет множественное определение цвета - многоцветная строка

  #if (USE_MP3 == 1) 
  runTextSound = -1;              // Нет звука, сопровождающего строку
  runTextSoundRepeat = false;     // Нет повторения звука (однократное воспроизведение)
  #endif
  
  // Получить очередную строку из массива строк, обработав все макросы кроме дато-зависимых
  // Если макросы, зависимые от даты есть - установить флаг textHasDateTime, макросы оставить в строке
  // Результат положить в currentText
  // Далее если флаг наличия даты в строке установлен - каждый раз перед отрисовкой выполнять подстановку даты и
  // обработку наступления даты последнего показа и переинициализацию строки
  // Если зависимостей от даты нет - вычисленная строка просто отображается пока не будет затребована новая строка

  offset = pWIDTH;   // перемотка новой строки в правый край
  if (text.length() != 0) {
    syncText = text;
    if (needProcessMacros) {
      currentText = processMacrosInText(currentText);
      needProcessMacros = false;
    }
  } else {
  
    // Если nextIdx >= 0 - значит в предыдущей строке было указано какую строку показывать следующей - показываем ее
    currentTextLineIdx = nextIdx >= 0 ? nextIdx : getNextLine(currentTextLineIdx);
    if (currentTextLineIdx >= TEXTS_MAX_COUNT) currentTextLineIdx = -1;

    currentText = currentTextLineIdx < 0 ? "" : getTextByIndex(currentTextLineIdx);
    syncText = currentText;
    // Если выбрана строка для принудительного показа - игнорировать запрет по '-' в начале строки или по макросу {-}
    if (nextIdx >= 0) {
      if (currentText[0] == '-') currentText[0] = ' ';
      currentText.replace("{-}", "");
    }
    currentText = processMacrosInText(currentText);
    
  }

  if (text.length() == 0 && currentTextLineIdx == 0 && currentText[0] == '#') currentText = "";

  // После обработки макросов в строке может оказаться, что строка не должна отображаться (например, по времени показа макроса {S}),
  // но в строке присутствует также макрос {#N}, который был разобран и результат сохранен в nextTextLineIdx.
  // Если основную строку показывать не нужно (возвращена пустая строка), то и "прицеп" нужно "обнулить".
  if (currentText.length() == 0) {
    nextTextLineIdx = -1;
  }

  return currentText.length() > 0;
}

// Получить индекс строки для отображения
// -1 - если показывать нечего
int8_t getNextLine(int8_t currentIdx) {
  // Если не задана следующая строка - брать следующую из массива в соответствии с правилом
  // sequenceIdx < 0 - просто брать следующую строку
  // sequenceIdx > 0 - Строка textIndecies содержит последовательность отображения строк, например "#12345ZYX"
  //                   в этом случае sequenceIdx = 1..textIndecies.length() и показывает на символ в строке, содержащий индекс следующей строки к отображению
  //                   Если извлеченный символ - '#' - брать случайную строку из массива
  int8_t nextLineIdx = currentIdx;
  int16_t cnt = 0;
  bool found = false;
  if (sequenceIdx < 1) {
    while (cnt < TEXTS_MAX_COUNT) {
      nextLineIdx++; cnt++;
      if (nextLineIdx >= TEXTS_MAX_COUNT) nextLineIdx = 0;
      char c_idx = getAZIndex(nextLineIdx);
      if (textsNotEmpty.indexOf(c_idx) < 0) continue;
      String text(getTextByAZIndex(c_idx));
      bool disabled = (text.length() == 0) || (nextLineIdx == 0 && text[0] == '#') || (text[0] == '-') || (text.indexOf("{-}") >= 0) || (text.indexOf("{P") >= 0);
      bool wrong_date = (text.indexOf("{S") >= 0) && !forThisDate(text);
      if (disabled || wrong_date) continue;    
      break;
    }
  } else {
    if (sequenceIdx >= (int16_t)textIndecies.length()) {
      // перемотать на начало последовательности
      sequenceIdx = isFirstLineControl() ? 1 : -1;
    }
    int8_t att = 0;
    cnt = textsNotEmpty.length();
    if (cnt > 0) {
      char c = textIndecies.charAt(sequenceIdx);
      if (c == '#') {
        // textIndecies == "##", sequenceIdx всегда 1; textIndecies.charAt(1) == '#';
        while (!found && att < cnt) {
          att++;
          uint8_t idx = random8(0,cnt - 1);
          char c_idx = textsNotEmpty[idx];
          int8_t t_idx = getTextIndex(c_idx);
          if (t_idx < 0) continue;        
          // Проверить - доступен ли текст в указанной строке к отображению?
          String text(getTextByIndex(t_idx));
          // Строка должна быть не пустая,
          // Не отключена - в первом символе или наличие макроса {-}?
          // Строки с макросом события {P} не отображаются по интервалу показа - только сразу до/после события
          // Если строка содержит макрос {S} - текущая дата должна попадать в диапазон доступных для показа дат
          bool disabled = (text.length() == 0) || (t_idx == 0 && text[0] == '#') || (text[0] == '-') || (text.indexOf("{-}") >= 0) || (text.indexOf("{P") >= 0);
          bool wrong_date = (text.indexOf("{S") >= 0) && !forThisDate(text);
          // Если строка не отключена и доступна к отображению - брать ее
          if (!(disabled || wrong_date)) {
            found = true;
            nextLineIdx = t_idx;
            break;
          }        
        }      
      } else {
        // Последовательное отображение строк как указано в последовательности в textIndecies - '#12345'
        // здесь 'c' - char - индекс, выдернутый из указанной последовательности в очередной позиции
        nextLineIdx = getTextIndex(c); 
        uint8_t c_idx = sequenceIdx;
        while (!found) {
          // Проверить - доступен ли текст в указанной строке к отображению?
          String text(getTextByIndex(nextLineIdx));
          // Строка должна быть не пустая,
          // Не отключена - в первом символе или наличие макроса {-}?
          // Строки с макросом события {P} не отображаются по интервалу показа - только сразу до/после события
          // Если строка содержит макрос {S} - текущая дата должна попадать в диапазон доступных для показа дат
          bool disabled = (text.length() == 0) || (nextLineIdx == 0 && text[0] == '#') || (text[0] == '-') || (text.indexOf("{-}") >= 0) || (text.indexOf("{P") >= 0);
          bool wrong_date = (text.indexOf("{S") >= 0) && !forThisDate(text);
          // Если строка не отключена и доступна к отображению - брать ее
          if (!(disabled || wrong_date)) {
            found = true;
            break;
          }
          // Строка недоступна - брать следующий номер в последовательности
          sequenceIdx++;
          if (sequenceIdx >= (int16_t)textIndecies.length()) {
            // перемотать на начало последовательности
            sequenceIdx = isFirstLineControl() ? 1 : -1;
          }
          if (c_idx == sequenceIdx) break;
          // Если после перемотки вернулись в позицию с которой начали - строк доступных к показу нет
          c = textIndecies.charAt(sequenceIdx);
          nextLineIdx = getTextIndex(c); 
        }
        if (found) {
          sequenceIdx++;
        }
      }
    }
    if (!found) {
      nextLineIdx = -1;
      textLastTime = millis();
    }
  }
  return nextLineIdx;
}

// Выполнить разбор строки на наличие макросов, применить указанные настройки
String processMacrosInText(const String& text) {  

  String textLine(text);
  /*   
     Общие правила:
     - Макрос - последовательность управляющих конструкций, каждая из которых заключена в фигурные скобки, например "{Exx}" 
     - textIndecies - (строка с индексом '0')
     - Если строка textIndecies начинается с '#'  - строка содержит последовательность строк к отображению, например "#1234ZYX"
     - Если строка textIndecies начинается с '##' - отображать строки из массива в случайном порядке
     - Если строка textIndecies НЕ начинается с '#' или '##' - это обычная строка для отображения
     - Если строка начинается с '-' или пустая или содержит макрос '{-}' - строка отключена и не отображается 
     
     Строка, извлеченная из массива может содержать в себе следующие макросы:
      "{-}"     - в любом месте строки означает, что строка временно отключена, аналогично "-" в начале строки
      "{#N}"    - в любом месте строки означает,что после отображения строки, следующей строкой отображать строку с номером N, где N - 1..9,A..Z -> {#9}
                  Настройка из п.0 (через какое время показывать следующую строку) игнорируется, строка показывается сразу
                  после завершения отображения "родительской" строки
      "{An+}    - проиграть звук номер n из папки SD:/03; если есть '+' - проигрывать в цикле пока показывается строка, если нет суффикса '+' - проиграть один раз -> {A3+}
      "{Exx}"   - эту строку отображать на фоне эффекта xx - где xx - номер эффекта. Эффект не меняется пока отображается строка -> {E21}
      "{Exx-n}" - эту строку отображать на фоне эффекта xx, вариант n - где xx - номер эффекта, n - номер варианта -> {E24-8}
      "{Ts}"    - отображать строку указанное количество секунд s -> {T30}
      "{Nx}"    - отображать строку указанное количество раз x -> {N3} 
                  если S или N не указаны - строка прокручивается 1 раз;
                  Если указаны оба - работает модификатор показа по времени
      "{Cc}"    - отображать строку указанным цветом С; Цвет - в виде #AA77FE; Специальные значения - #000001 - радуга;  - #000002 - каждая буква свой цвет; -> {C#005500}
      "{Bc}"    - отображать строку на однотонном фоне указанного цвета С; Цвет - в виде #337700; -> {B#000000}
      "{WS}"    - отображать вместо {WS} состояние текущей погоды - "Пасмурно", "Ясно", "Дождь", "Гроза" и т.д. -> {WS}
      "{WT}"    - отображать вместо {WT} текущую температуру воздуха, например "+26", "-31" -> {WT}
      "{D:F}"   - где F - один из форматов даты / времени                         "С Новым {D:yyyy} годом!"
                  d    - день месяца, в диапазоне от 1 до 31.  (допускается D)
                  dd   - день месяца, в диапазоне от 01 до 31. (допускается DD)
                  ddd  - сокращенное название дня недели       (допускается DDD)
                  dddd - полное название дня недели            (допускается DDDD)
                  M    - месяц от 1 до 12
                  MM   - месяц от 01 до 12
                  MMM  - месяц прописью (янв..дек)
                  MMMМ - месяц прописью (января..декабря)
                  y    - год в диапазоне от 0 до 99            (допускается Y)
                  yy   - год в диапазоне от 00 до 99           (допускается YY)
                  yyyy - год в виде четырехзначного числа      (допускается YYYY)
                  h    - час в 12-часовом формате от 1 до 12
                  hh   - час в 12-часовом формате от 01 до 12
                  H    - час в 24-часовом формате от 0 до 23
                  HH   - час в 24-часовом формате от 00 до 23
                  m    - минуты в диапазоне от 0 до 59
                  mm   - минуты в диапазоне от 00 до 59
                  s    - секунды в диапазоне от 0 до 59        (допускается S)
                  ss   - секунды в диапазоне от 00 до 59       (допускается SS)
                  T    - Первый символ указателя AM/PM
                  TT   - Указатель AM/PM
                  t    - Первый символ указателя am/pm
                  tt   - Указатель am/pm
        
        Если формат не указан - используется формат H:mm        
        пример: "Красноярское время {D:HH:mm}" - бегущая строка "Красноярское время 07:15"  
                "Сегодня {D:DD MMMM YYYY} года" - бегущая строка "Сегодня 26 июля 2020 года"

     "{D}"      - просто часы вида "21:00" в виде бегущей строки

     "{R01.01.2021 0:00:00#N}" 
     "{R01.01.**** 0:00:00#N}" 
     "{R01.01.***+ 0:00:00#N}" 
       - где после R указана дата и опционально время до которой нужно отсчитывать оставшееся время, выводя строку остатка в формате:
           для R: X дней X часов X минут; 
           для P: X дней X часов X минут X секунд; 
         Если время не указано - считается 0:00
         Если дней не осталось - выводится только X часов X минут; 
         Если часов тоже уже не осталось - выводится X минут
         Если минут тоже уже не осталось - выводится X секунд
         Год в дате может быть указан '****', что означает "текущий год" или '***+', что означает "Следующий год"
         Время в макросе может быть указано с секундами: '23:59:59'
       
       - где после даты/времени может быть указан
         - #N - если осталось указанная дата уже наступила - вместо этой строки выводится строка с номером N
                если строка замены не указана - просроченная строка не выводится берется следующая строка
         - Внимание! При использовании макроса для вывода строки "До Нового года осталось" с годом, указанным звездочками '****'
           следует иметь в виду, что по достижении указанной даты год сменится на следующий, и событие "По достижении даты показовать строку #N"
           не сработает - вместо этого будет отображаться "До Нового года осталось 365 дней".
           Чтобы это избежать - и строку с макросом {R} и строку #N нужно использовать совместно с макросом {S}, который онграничивает
           перирод показа этой строки:
             getTextByIndex(2)   = "До {C#00D0FF}Нового года {C#FFFFFF}осталось {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59:59}";
             getTextByIndex(3)   = "С {C#00D0FF}Новым {C#0BFF00}{D:yyyy} {C#FFFFFF}годом!{S01.01.****#31.01.**** 23:59:59}";


     "{P01.01.2020#N#B#A}"
     "{P**.**.**** 7:00#N#B#A#F}"
     "{P01.01.2020#N#B#A}"
     "{P7:00#N#B#A#F}"   
       - где после P указана дата и опционально время до которой нужно отсчитывать оставшееся время, выводя строку остатка в формате:
          X дней X часов X минут X секунд; 
         Если дней не осталось - выводится только X часов X минут; 
         Если часов тоже уже не осталось - выводится X минут
         Если минут тоже уже не осталось - выводится X секунд

       Строка с режимом {P не выводится при периодическом переборе строк для отображения - только до/после непосредственно времени события:
       время наступления события мониторится и за #B секунд до события бегущая строка начинает отображаться на матрице и после события #A секунд отображается строка-заместитель 
       
       
       Для режима P дата может опускаться (означает "каждый день") или ее компоненты могут быть заменены звездочкой '*'
       **.10.2020 - весь октябрь 2020 года 
       01.**.**** - каждое первое число месяца
       **.**.2020 - каждый день 2020 года
       
       "{P7:00#N#120#30#12347}"  - каждый пн,вт,ср,чт,вс в 7 утра
       
       - где после даты может быть указан
         - #N - если осталось указанная дата уже наступила - вместо этой строки выводится строка с номером N
                если строка замены не указана - просроченная строка не выводится берется следующая строка
         - #B - начинать отображение за указанное количество секунд ДО наступления события (before). Если не указано - 60 секунд по умолчанию
         - #A - отображать строку-заместитель указанное количество секунд ПОСЛЕ наступления события (after). Если не указано - 60 секунд по умолчанию
         - #F - дни недели для которых работает эта строка - полезно когда дата не определена 1-пн..7-вс
                Если указана точная дата и указан день недели, который не соответствует дате - строка выведена не будет

     "{S01.01.2020#01.01.2020}"
     "{S01.01.2020 7:00#01.01.2020 19:00}"
     "{S01.01.**** 7:00#01.01.**** 19:00}"
       - где после S указаны даты начала и конца периода доступного для отображения строки.
       Для режима S элементы даты быть заменены звездочкой '*'
       **.10.2020 - весь октябрь 2020 года 
       01.**.**** - каждое первое число месяца
       **.**.2020 - каждый день 2020 года  
  */

  // Выполнять цикл поиска подходящей к отображению строки
  // Если ни одной строки не найдено - возвратить false

  bool    found = false;
  uint8_t attempt = 0;
  int16_t idx, idx2;

  while (!found && (attempt < TEXTS_MAX_COUNT)) {
    
    // -------------------------------------------------------------    
    // Строка начинается с '-' или пустая или содержит "{-}" - эта строка отключена, брать следующую
    // Строку показывать даже если отключена когда установлен флаг ignoreTextOverlaySettingforEffect
    // -------------------------------------------------------------    

    if (textLine.length() > 0 && ignoreTextOverlaySettingforEffect) {
      if (textLine[0] == '-') textLine[0] = ' ';
      textLine.replace("{-}", "");
    }

    if (textLine.length() == 0 || textLine.charAt(0) == '-' || textLine.indexOf("{-}") >= 0) {
      attempt++;  
      currentTextLineIdx = getNextLine(currentTextLineIdx);
      textLine = (currentTextLineIdx < 0 || currentTextLineIdx >= TEXTS_MAX_COUNT) ? "" : getTextByIndex(currentTextLineIdx);
      continue;
    }

    // Если в строке содержится макрос, связанный с погодой, но погода еще не получена с сервера - пропускать строку, брать следующую
    if (!init_weather && (textLine.indexOf("{WS}") >= 0 || textLine.indexOf("{WT}") >= 0)) {
      attempt++;  
      currentTextLineIdx = getNextLine(currentTextLineIdx);
      textLine = (currentTextLineIdx < 0 || currentTextLineIdx >= TEXTS_MAX_COUNT) ? "" : getTextByIndex(currentTextLineIdx);
      continue;
    }

    // -------------------------------------------------------------
    // Эти форматы содержат строку, зависящую от текущего времени.
    // Оставить эти форматы как есть в строке - они будут обрабатываться на каждом проходе, подставляя текущее время
    // Сейчас просто выставить флаг, что строка содержит макросы, зависимые от даты
    //    "{D:F}" - где F - один из форматов даты / времени если формата даты нет - аналогично {D}
    //    "{D}"  - просто часы вида "21:00" в виде бегущей строки
    //    "{R01.01.****#N}" 
    //    "{P01.**.2021 8:00#N#B#A#F}" 
    //    "{S01.01.****}" 
    // -------------------------------------------------------------

    textHasDateTime = textLine.indexOf("{D") >= 0 || textLine.indexOf("{R") >= 0 || textLine.indexOf("{P") >= 0 || textLine.indexOf("{S") >= 0;  

    // Если время еще не инициализировано и строка содержит макросы, зависимые от времени -
    // строку отображать нельзя - пропускаем, переходим к поиску следующей строки
    if (!init_time && textHasDateTime) {
      textHasDateTime = false;
      attempt++;  
      continue;
    }

    found = true;

    // -------------------------------------------------------------    
    // "{#N}" - в любом месте строки означает,что после отображения строки, следующей строкой отображать строку с номером N, где N - 1..9,A..Z или двухзначный индекс 1..19
    // -------------------------------------------------------------    
    
    nextTextLineIdx = -1;
    idx = textLine.indexOf("{#");
    while (idx >= 0) {

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь номер эффекта, он должен быть 0..MAX_EFFECT-1, может быть двузначным
      String tmp;
      if (idx2 - idx > 1) {
        tmp = textLine.substring(idx+2, idx2);
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);

      int16_t len = tmp.length();
      if (len == 0 || len > 2) break;

      char c = tmp.charAt(0);
      if (len == 1 || (c >= 'A' && c <= 'Z')) {        
        // Запоминаем номер следующей к показу строки, извлеченный из макроса
          nextTextLineIdx = getTextIndex(c);
      } else {
         nextTextLineIdx = tmp.toInt();
      }
      
      if (nextTextLineIdx >= TEXTS_MAX_COUNT) {
        nextTextLineIdx = -1;
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);
      
      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{#");  
    }

    // -------------------------------------------------------------
    //  "{An+} - при начале строки проигрывать звук n из папки "SD://03"? если есть суффикс '+' - воспроизводить в цикле пока показывается строка
    // -------------------------------------------------------------
    #if (USE_MP3 == 1)

    runTextSound = -1;
    runTextSoundRepeat = false;
    
    idx = textLine.indexOf("{A");
    while (idx >= 0) {
      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь номер звука, может быть двузначным
      String tmp;
      if (idx2 - idx > 1) {
        tmp = textLine.substring(idx+2, idx2);
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);

      // Последним символом в выделенной строке может быть '+' - признак повторения звука (цикл)
      if (tmp.endsWith("+")) {
        runTextSoundRepeat = true;
        tmp = tmp.substring(0, tmp.length()-1);
      }

      // Преобразовать строку в число
      idx = tmp.length() > 0 ? tmp.toInt() : -1;
      runTextSound = idx;

      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{A");  
    }
    #endif

    // -------------------------------------------------------------
    //  "{Exx} - эту строку отображать на фоне эффекта xx - где xx - номер эффекта. Эффект не меняется пока отображается строка      
    //  "{Exx-n} - эту строку отображать на фоне эффекта xx с вариантом n - где xx - номер эффекта? n - номер варианта эффекта.
    // -------------------------------------------------------------

    specialTextEffect = -1;
    specialTextEffectParam = -1;
    idx = textLine.indexOf("{E");
    while (idx >= 0) {

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь номер эффекта, он должен быть 0..MAX_EFFECT-1, может быть двузначным
      String tmp, tmp1;
      if (idx2 - idx > 1) {
        tmp = textLine.substring(idx+2, idx2);
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);

      // tmp может содержать кроме номера эффекта, номер варианта, отделенный знаком '-' (например из {E43} или {E34-12} будут строки '43' и '43-12'
      // если есть номер варианта - отделить его от номера эффекта
      int16_t p = tmp.indexOf('-');
      if (p > 0) {
        tmp1 = tmp.substring(p + 1);
        tmp  = tmp.substring(0, p);
      }
      
      // Преобразовать строку в число
      idx = tmp.length() > 0 ? tmp.toInt() : -1;
      specialTextEffect = idx >= 0 && idx < MAX_EFFECT ? idx : -1;

      idx = tmp1.length() > 0 ? tmp1.toInt() : -1;
      specialTextEffectParam = idx >= 0 ? idx : -1;
            
      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{E");  
    }

    // -------------------------------------------------------------
    //  "{TS}" - отображать строку указанное количество секунд S
    // -------------------------------------------------------------

    textShowTime = -1;
    idx = textLine.indexOf("{T");
    while (idx >= 0) {

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь количество секунд отображения этой бегущей строки
      String tmp;
      if (idx2 - idx > 1) {
        tmp = textLine.substring(idx+2, idx2);
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);

      // Преобразовать строку в число
      idx = tmp.length() > 0 ? tmp.toInt() : -1;
      textShowTime = idx > 0 ? idx : -1;
      
      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{T");
    }

    // -------------------------------------------------------------
    //  "{NX}" - отображать строку указанное количество раз X
    // -------------------------------------------------------------

    textShowCount = 1;
    idx = textLine.indexOf("{N");
    while (idx >= 0) {

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь количество раз отображения этой бегущей строки
      String tmp;
      if (idx2 - idx > 1) {
        tmp = textLine.substring(idx+2, idx2);
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);

      // Преобразовать строку в число
      idx = tmp.length() > 0 ? tmp.toInt() : -1;
      textShowCount = idx > 0 ? idx : 1;
      
      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{N");  
    }
      
    // -------------------------------------------------------------
    // "{BC}"- отображать строку  на однотонном фоне указанного цвета С; Цвет - в виде #007700
    // -------------------------------------------------------------

    useSpecialBackColor = false;
    idx = textLine.indexOf("{B");
    while (idx >= 0) {

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь цвет фона отображения этой бегущей строки
      String tmp;
      if (idx2 - idx > 1) {
        tmp = textLine.substring(idx+2, idx2);
      }
      
      // удаляем макрос
      textLine.remove(idx, idx2 - idx + 1);
           
      // Преобразовать строку в число
      useSpecialBackColor = true;
      specialBackColor = (uint32_t)HEXtoInt(tmp);
      
      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{B");  
    }

    #if (USE_WEATHER == 1)     
    
    // {WS} - отображать текущую погоду - "Ясно", "Облачно" и т.д
    idx = textLine.indexOf("{WS}");
    if (idx >= 0) {
      textLine.replace("{WS}", weather);
    }

    // {WT} - отображать текущую температуру в виде "+26" или "-26"
    // Если включено отображение температуры цветом - добавить макрос цвета перед температурой 
    idx = textLine.indexOf("{WT}");
    if (idx >= 0) {
      // Подготовить строку текущего времени HH:mm и заменить все вхождения {D} на эту строку
      String s_temperature((temperature == 0 ? "" : (temperature > 0 ? "+" : ""))); s_temperature += temperature;
      String s_color = "";

      if (useTemperatureColor) {
        s_color = "{C" + getTemperatureColor(temperature) + "}";
      }
      
      textLine.replace("{WT}", s_color + s_temperature);
    }

    #endif
     
    // -------------------------------------------------------------
    // "{CC}"- отображать строку указанным цветом С; Цвет - в виде #AA77FE
    // -------------------------------------------------------------
    // Если макрос стоит в самом начале или в самом конце строки и единственный - всю строку отображать указанным цветом
    // Если макрос стоит в середине строки или макросов цвета несколько - они указывают позицию в строке с которой меняется цвет отображения
    // В этом случае два варианта - строка также содержит макрос, зависящий от времени?
    // - не содержит дат - разбор можно сделать один раз, сохранить позиции и цвета в массив и дальше использовать его при отображении
    // - содержит даты - разбор позиций цветов определять ПОСЛЕ формирования строки с вычисленной датой
    
    textHasMultiColor = checkIsTextMultiColor(textLine);

    // Если строка не содержит даты или не содержит множественного определения цвета
    // обработать макрос цвета, цвет сохранить в specialTextColor, установить флаг useSpecialTextColor
    // В дальнейшем при отображении строки не нужно каждый раз вычислять цвет и позицию - просто использовать specialTextColor

    // Если строка содержит множественное определение цвета и не содержит даты - подготовить массив цветов,
    // который будет отображаться для отрисовки строки
    
    if (textHasMultiColor && !textHasDateTime) {
      textLine = processColorMacros(textLine);
    } else 
    
    if (!textHasMultiColor) { 
      
      useSpecialTextColor = false;
      idx = textLine.indexOf("{C");
      while (idx >= 0) {
  
        // Закрывающая скобка
        // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
        idx2 = textLine.indexOf("}", idx);        
        if (idx2 < 0) break;
  
        // Извлечь цвет текста отображения этой бегущей строки
        String tmp;
        if (idx2 - idx > 1) {
          tmp = textLine.substring(idx+2, idx2);
        }
        
        // удаляем макрос
        textLine.remove(idx, idx2 - idx + 1);
             
        // Преобразовать строку в число
        useSpecialTextColor = true;
        specialTextColor = (uint32_t)HEXtoInt(tmp);
        
        // Есть еще вхождения макроса?
        idx = textLine.indexOf("{C");  
      }

    }
    attempt++;
  }

  return found ? textLine : "";
}

// Обработать макросы даты в строке
// textLine - строка, содержащая макросы
String processDateMacrosInText(const String& text) {

  String textLine(text); 
  
  /* -------------------------------------------------------------
   Эти форматы содержат строку, зависящую от текущего времени.
   Оставить эти форматы как есть в строке - они будут обрабатываться на каждом проходе, подставляя текущее время
   Сейчас просто выставить флаг, что строка содержит макросы, зависимые от даты
      "{D}"  - просто часы вида "21:00" в виде бегущей строки
      "{D:F}" - где F - один из форматов даты / времени если формата даты нет - аналогично {D}
       Формат даты поддерживает следующие спецификаторы       
          d    - день месяца, в диапазоне от 1 до 31.  (допускается D)
          dd   - день месяца, в диапазоне от 01 до 31. (допускается DD)
          ddd  - сокращенное название дня недели       (допускается DDD)
          dddd - полное название дня недели            (допускается DDDD)
          M    - месяц от 1 до 12
          MM   - месяц от 01 до 12
          MMM  - месяц прописью (янв..дек)
          MMMМ - месяц прописью (января..декабря)
          y    - год в диапазоне от 0 до 99            (допускается Y)
          yy   - год в диапазоне от 00 до 99           (допускается YY)
          yyyy - год в виде четырехзначного числа      (допускается YYYY)
          h    - час в 12-часовом формате от 1 до 12
          hh   - час в 12-часовом формате от 01 до 12
          H    - час в 23-часовом формате от 0 до 23
          HH   - час в 23-часовом формате от 00 до 23
          m    - минуты в диапазоне от 0 до 59
          mm   - минуты в диапазоне от 00 до 59
          s    - секунды в диапазоне от 0 до 59        (допускается S)
          ss   - секунды в диапазоне от 00 до 59       (допускается SS)
          T    - Первый символ указателя AM/PM
          TT   - Указатель AM/PM
          t    - Первый символ указателя am/pm
          tt   - Указатель am/pm
  
      "{R01.01.2021#N}" 
      "{R01.01.****}" 
      "{R01.01.***+}" 
         - где после R указана дата до которой нужно отсчитывать оставшееся время, выводя строку остатка в формате:
             X дней X часов X минут; 
           Если дней не осталось - выводится только X часов X минут; 
           Если минут тоже уже не осталось - выводится X минут
           Год в дате может быть указан '****', что означает "текущий год" или '***+', что означает "Следующий год"
           Время в макросе может быть указано с секундами: '23:59:59'
       
         - где после даты может быть указан
           - #N - если осталось указанная дата уже наступила - вместо этой строки выводится строка с номером N
                  если строка замены не указана или отключена (символ "-" вначале строки) - просроченная строка не выводится
                  берется следующая строка
                  
         - Внимание! При использовании макроса для вывода строки "До Нового года осталось" с годом, указанным звездочками '****'
           следует иметь в виду, что по достижении указанной даты год сменится на следующий, и событие "По достижении даты показовать строку #N"
           не сработает - вместо этого будет отображаться "До Нового года осталось 365 дней".
           Чтобы это избежать - и строку с макросом {R} и строку #N нужно использовать совместно с макросом {S}, который онграничивает
           перирод показа этой строки:
             getTextByIndex(2)   = "До {C#00D0FF}Нового года {C#FFFFFF}осталось {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59}";
             getTextByIndex(3)   = "С {C#00D0FF}Новым {C#0BFF00}{D:yyyy} {C#FFFFFF}годом!{S01.01.****#31.01.**** 23:59}";

     "{PДД.ММ.ГГГГ#N#B#A#F}"
       - где после P указаны опционально дата и время до которой нужно отсчитывать оставшееся время, выводя строку остатка в формате:
         X дней X часов X минут X секунд; 
         Если дней не осталось - выводится только X часов X минут; 
         Если часов тоже уже не осталось - выводится X минут
         Если минут тоже уже не осталось - выводится X секунд

       Строка с режимом {P не выводится при периодическом переборе строк для отображения - только до/после непосредственно времени события:
       время наступления события мониторится и за #B секунд до события бегущая строка начинает отображаться на матрице и после события #A секунд отображается строка-заместитель 
       
       Для режима P дата может опускаться (означает "каждый день") или ее компоненты могут быть заменены звездочкой '*'
       **.10.2020 - весь октябрь 2020 года 
       01.**.**** - каждое первое число месяца любого года
       **.**.2020 - каждый день 2020 года
       
       "{P7:00#N#120#30#12347}"  - каждый пн,вт,ср,чт,вс в 7 утра (за 120 сек до наступления события и 30 секунд после наступления

       - где компоненты даты:
         ДД - число месяца
         MM - месяц
         ГГГГ - год
         Компоненты даты могут быть заменены * - означает "любой"
         
       - где после даты может быть указан
         - #N - если осталось указанная дата уже наступила - вместо этой строки выводится строка с номером N
                если строка замены не указана - просроченная строка не выводится берется следующая строка
         - #B - начинать отображение за указанное количество секунд ДО наступления события (before). Если не указано - 60 секунд по умолчанию
         - #A - отображать строку-заместитель указанное количество секунд ПОСЛЕ наступления события (after). Если не указано - 60 секунд по умолчанию
         - #F - дни недели для которых работает эта строка, когда дата не определена 1-пн..7-вс
                Если указана точная дата и указан день недели, который не соответствует дате - строка выведена не будет
          
     "{S01.01.2020#01.01.2020}"
     "{S01.01.2020 7:00#01.01.2020 19:00}"
     "{S01.01.**** 7:00#01.01.**** 19:00}"
       - где после S указаны даты начала и конца периода доступного для отображения строки.
       Для режима S элементы даты быть заменены звездочкой '*'
       **.10.2020 - весь октябрь 2020 года 
       01.**.**** - каждое первое число месяца
       **.**.2020 - каждый день 2020 года  
     ------------------------------------------------------------- 
  */

  uint8_t  aday = day();
  uint8_t  amnth = month();
  uint16_t ayear = year();
  uint8_t  hrs = hour(), hrs_t;
  uint8_t  mins = minute();
  uint8_t  secs = second();
  bool     am = isAM();
  bool     pm = isPM();
  int16_t  idx, idx2;

  int8_t   wd = weekday()-1;  // day of the week, Sunday is day 0   
  if (wd == 0) wd = 7;        // Sunday is day 7, Monday is day 1;

  while (true) {
    // {D} - отображать текущее время в формате 'HH:mm'
    idx = textLine.indexOf("{D}");
    if (idx >= 0) {
      // Подготовить строку текущего времени HH:mm и заменить все вхождения {D} на эту строку
      String s_time(padNum(hrs, 2)); s_time += ':'; s_time += padNum(mins, 2);
      textLine.replace("{D}", s_time);
    }

    // {D:F} (где F - форматная строка
    idx = textLine.indexOf("{D:");
    if (idx >= 0) {
  
      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;
  
      // Извлечь форматную строку
      String sFmtProcess('#'); 
      String sFormat('$');
  
      if (idx2 - idx > 1) {
        sFormat = textLine.substring(idx+3, idx2);
      }
      
      sFmtProcess = sFormat;
      
      //  YYY+ - СДЕДУЮЩИЙ год в виде четырехзначного числа
      String str(ayear + 1);
      sFmtProcess.replace("YYY+", str);
      sFmtProcess.replace("yyy+", str);

      //  YYYY - год в виде четырехзначного числа
      str.clear();
      str += ayear;
      sFmtProcess.replace("YYYY", str);
      sFmtProcess.replace("yyyy", str);
  
      //  YY   - год в диапазоне от 00 до 99
      str = str.substring(2);
      sFmtProcess.replace("YY", str);
      sFmtProcess.replace("yy", str);
  
      //  Y    - год в диапазоне от 0 до 99
      if (str[0] == '0') str = str.substring(1);
      sFmtProcess.replace("Y", str);
      sFmtProcess.replace("y", str);
      
      //  HH   - час в 23-часовом формате от 00 до 23
      str.clear();
      str += padNum(hrs, 2);
      sFmtProcess.replace("HH", str);
  
      //  H    - час в 23-часовом формате от 0 до 23
      str.clear();
      str += hrs;
      sFmtProcess.replace("H", str);
  
      //  hh   - час в 12-часовом формате от 01 до 12
      hrs_t = hrs;
      if (hrs_t > 12) hrs_t = hrs_t - 12;
      if (hrs_t == 0) hrs_t = 12;

      str.clear();
      str += padNum(hrs_t, 2);
      sFmtProcess.replace("hh", str);
  
      //  h    - час в 12-часовом формате от 1 до 12
      str.clear();
      str += hrs_t;
      sFmtProcess.replace("h", str);
      
      //  mm   - минуты в диапазоне от 00 до 59
      str.clear();
      str += padNum(mins, 2);
      sFmtProcess.replace("mm", str);
  
      //  m    - минуты в диапазоне от 0 до 59
      str.clear();
      str += mins;
      sFmtProcess.replace("m", str);
  
      //  ss   - секунды в диапазоне от 00 до 59
      str.clear();
      str += padNum(secs, 2);
      sFmtProcess.replace("SS", str);
      sFmtProcess.replace("ss", str);
  
      //  s    - секунды в диапазоне от 0 до 59
      str.clear();
      str += secs;
      sFmtProcess.replace("S", str);
      sFmtProcess.replace("s", str);
  
      //  tt   - Указатель AM/PM
      str.clear();
      str += am ? "AM" : (pm ? "PM" : "");
      sFmtProcess.replace("TT", str);
      str.toLowerCase();
      sFmtProcess.replace("tt", str);
  
      //  t    - Первый символ указателя AM/PM
      str.clear();
      str += am ? "A" : (pm ? "P" : "");
      sFmtProcess.replace("T", str);
      str.toLowerCase();
      sFmtProcess.replace("t", str);

      //  dddd - полное название дня недели            (допускается DDDD)
      str.clear();
      str += substitureDateMacros(getWeekdayString(wd));
      sFmtProcess.replace("DDDD", str);
      sFmtProcess.replace("dddd", str);
      
      //  ddd  - сокращенное название дня недели       (допускается DDD)
      str.clear(); // DDD
      str += substitureDateMacros(getWeekdayShortString(wd));
      sFmtProcess.replace("DDD", str);
      sFmtProcess.replace("ddd", str);
  
      //  dd   - день месяца, в диапазоне от 01 до 31. (допускается DD)
      str.clear();
      str += padNum(aday,2);  // DD
      sFmtProcess.replace("DD", str);
      sFmtProcess.replace("dd", str);
  
      //  d    - день месяца, в диапазоне от 1 до 31.  (допускается D)
      str.clear();
      str += aday;  // D
      sFmtProcess.replace("D", str);
      sFmtProcess.replace("d", str);

      //  MMMМ - месяц прописью (января..декабря)
      str.clear();
      str += substitureDateMacros(getMonthString(amnth));
      sFmtProcess.replace("MMMM", str);
      
      //  MMM  - месяц прописью (янв..дек)
      str.clear();
      str += substitureDateMacros(str);
      str = getMonthShortString(amnth);
      sFmtProcess.replace("MMM", str);
  
      //  MM   - месяц от 01 до 12
      str.clear();
      str += padNum(amnth,2);
      sFmtProcess.replace("MM", str);
  
      //  M    - месяц от 1 до 12
      str.clear();
      str += amnth;
      sFmtProcess.replace("M", str);
      sFmtProcess = unsubstitureDateMacros(sFmtProcess);

      // Заменяем в строке макрос с исходной форматной строкой на обработанную строку с готовой текущей датой
      textLine.replace("{D:" + sFormat + "}", sFmtProcess);
      
      // Есть еще вхождения макроса?
      idx = textLine.indexOf("{D:");
    }

    // "{R01.01.****}" 
    // "{R01.01.***+}" 
    // "{R01.01.2023#N}" 
    // "{R10.10.2023 7:00#N}" 
    idx = textLine.indexOf("{R");
    if (idx >= 0) {
            
      // Если время  события уже наступило и в строке указана строка-заместитель для отображения ПОСЛЕ наступления события - показывать эту строку
      // Если замены строки после наступления события нет - textLine будет пустой - отображать нечего
      // Строка замены снова может содержать метки времени - поэтому отправить проверку / замену на второй круг

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // Извлечь дату события, время и строку замены ПОСЛЕ события (если есть)
      String str;
      if (idx2 - idx > 1) {
        str += textLine.substring(idx+2, idx2);
        str.trim();
      }

      // удаляем макрос; точка вставки строки остатка будет будет в позицию idx
      textLine.remove(idx, idx2 - idx + 1);

      // Здесь str имеет вид '01.01.2020', или '01.01.****', или '01.01.***+' если строки замены ПОСЛЕ события нет или '01.01.2020#J' или '01.01.2020#19' если строка замены указана (J=19 - индекс в массиве)
      // так же после даты может быть указано время '01.01.2020 7:00:00'
      if (str.length() > 0) {

        time_t t_now = now();
        time_t t_event = now();
        time_t t_diff = 0;

        // Точка вставки строки остатка
        uint16_t insertPoint = idx;
        int8_t  afterEventIdx = -1;

        // Здесь - str - дата (и время) события, s_nn - номер строки замены или пустая строка, если строки замены нет
        // Получить дату наступления события из строки 'ДД.ММ.ГГГГ'

        // Есть индекс строки замены? Если есть - отделить ее от даты события
        idx = str.indexOf("#");
        String s_nn;
        if (idx >= 0) {
          s_nn = idx>=0 ? str.substring(idx+1) : "";
          str = str.substring(0,idx);
        }

        tmElements_t tm = ParseDateTime(str);
        t_event = makeTime(tm);

        /*
        DEBUGLN(DELIM_LINE);
        DEBUGLN("Исходная R-дата: '" + str + "'");
        DEBUGLN(String(F("Дата события: ")) + padNum(tm.Day,2) + "." + padNum(tm.Month,2) + "." + padNum(tmYearToCalendar(tm.Year),4) + " " + padNum(tm.Hour,2) + ":" + padNum(tm.Minute,2) + ":" + padNum(tm.Second,2));
        DEBUGLN(DELIM_LINE);
        */
        
        // Если t_now >= t_event - событие уже прошло, нужно заменять обрабатываемую строку на строку подстановки, указанную (или нет) в s_nn 
        if (t_now >= t_event) {

          uint8_t len = s_nn.length();
          if (len > 0) {
            // Преобразовать строку в индекс
            char c = s_nn.charAt(0);
            if (len == 1 || (c >= 'A' && c <= 'Z')) {        
              // Номер следующей к показу строки, извлеченный из макроса в формате 1..9,A..Z
              afterEventIdx = getTextIndex(c);
            } else {
              // Номер следующей к показу строки, извлеченный из макроса в формате десятичного числа
              afterEventIdx = s_nn.toInt();
            }
      
            if (afterEventIdx >= TEXTS_MAX_COUNT) {
              afterEventIdx = -1;
            }
          } else {
            // s_nn не содержит индекса строки замены
            afterEventIdx = -1; 
          }

          // Строка замены не указана - завершить показ строки
          if (afterEventIdx < 0) {
            return "";    
          }

          // Получить текст строки замены. Он может содержать макросы, в т.ч. и макросы даты. Их нужно обработать.
          // Для того, чтобы указанная строка замены не отображалась при обычном переборе - она должна быть помечена как отключенная - '-' в начале строки или '{-}' в любом месте
          // Когда же пришло время отобразить строку - перед обработкой макросов ее нужно "включить"
          textLine = getTextByIndex(afterEventIdx);

          // содержит ли строка замены ограничение на показ в периоде, указанном макросом {S}?
          bool wrong_date = (textLine.indexOf("{S") >= 0) && !forThisDate(textLine);
          if (wrong_date) {
            return "";    
          }

          if (textLine.length() == 0) return "";              
          if (textLine[0] == '-') textLine = textLine.substring(1);
          if (textLine.indexOf("{-}") >= 0) textLine.replace("{-}", "");
          if (textLine.length() == 0) return "";    

          textLine = processMacrosInText(textLine);
          
          // Строка замены содержит в себе макросы даты? Если да - вычислить всё снова для новой строки                   
          if (textLine.indexOf("{D}") >= 0 || textLine.indexOf("{D:") >= 0 ||textLine.indexOf("{R") >= 0 || textLine.indexOf("{P") >= 0 || textLine.indexOf("{S") >= 0) continue;

          // Вернуть текст строки замены, отображаемой после того, как событие прошло
          return textLine;
        }

        // Событие еще не наступило
        
        // Чтобы строка замены наступившего события не показывалась в обычном порядке - она должна быть отключена
        // Проверить, что это так и если не отключена - отключить
        uint16_t len = s_nn.length();        
        if (len > 0) {
          // Преобразовать строку в индекс
          char c = s_nn.charAt(0);
          int16_t nm = -1;
          if (len == 1 || (c >= 'A' && c <= 'Z')) {        
            // Номер следующей к показу строки, извлеченный из макроса в формате 1..9,A..Z
            nm = getTextIndex(c);
          } else {
            // Номер следующей к показу строки, извлеченный из макроса в формате десятичного числа
            nm = s_nn.toInt();
          }
    
          if (nm > 0 && nm < TEXTS_MAX_COUNT) {
            String s(getTextByIndex(nm));
            if (s.length() > 0 && s[0] != '-' && s.indexOf("{-}") < 0) {
              s = '-' + s;              
              saveTextLine(getAZIndex(nm), s);
            }
          }
        }
        
        // Вычислить сколько до его наступления осталось времени
        t_diff = t_event - t_now; // кол-во секунд до события

        // Пересчет секунд в дней до события / часов до события / минут до события
        uint16_t restDays = t_diff / SECS_PER_DAY;
        uint8_t restHours = (t_diff - (restDays * SECS_PER_DAY)) / SECS_PER_HOUR;
        uint8_t restMinutes = (t_diff - (restDays * SECS_PER_DAY) - (restHours * SECS_PER_HOUR)) / SECS_PER_MIN;
        uint8_t restSeconds = (t_diff - (restDays * SECS_PER_DAY) - (restHours * SECS_PER_HOUR) - (restMinutes * SECS_PER_MIN));

        // Если осталось меньше минуты - отображать секунды
        // Если осталось меньше часа - отображать только минуты
        // Если осталось несколько часов - отображать часы и минуты
        // Если осталось меньше-равно 7 дней - отображать дни и часы
        // Если осталось больше 7 дней - отображать дни

        // Когда, реально, скажем, осталось 22 дня и 2 часа - гугль на вопрос "Сколько осталось до ..." пишет не 22, а 23
        // Будем следовать методике расчета гугля - если осталось более 7 дней - пишем на 1 больше
        // Если осталось менее 7 целых дней - там уже начнут выводиться часы. При выводе часов не округляем количество дней в большую сторону
        if (restDays > 7) restDays++; 
        
        String tmp;
        if (restDays == 0 && restHours == 0 && restMinutes == 0)
          { tmp += restSeconds; tmp += WriteSeconds(restSeconds); }
        else if (restDays == 0 && restHours == 0 && restMinutes > 0)
          { tmp += restMinutes; tmp += WriteMinutes(restMinutes); }
        else if (restDays == 0 && restHours > 0 && restMinutes > 0)
          { tmp += restHours; tmp += WriteHours(restHours); tmp += ' '; tmp += restMinutes; tmp += WriteMinutes(restMinutes); }
        else if (restDays > 0 && restDays <= 7 && restHours > 0)
          { tmp += restDays; tmp += WriteDays(restDays); tmp += ' '; tmp += restHours; tmp += WriteHours(restHours); }
        else  
          { tmp += restDays; tmp += WriteDays(restDays); }

        textLine = textLine.substring(0, insertPoint) + tmp + textLine.substring(insertPoint);
      }
    }

    // "{P01.01.2021#N#B#A#F}" 
    // "{P7:00#N#B#A#F}" 
    // "{P7:00#N#B#A#F#T}" или  "{P7:00#N#B#A#F#t}"
    // "{P**.01.2021 7:00#N#B#A#F}" 
    idx = textLine.indexOf("{P");
    if (idx >= 0) {

      // Строка с событием непрерывной проверки - при старте программы и изменении текста строк и после завершения отображения очередной такой строки формируется массив ближайших событий
      // Массив содержит время наступления события, сколько до и сколько после отображать а так же - индексы отображаемых строк ДО и строки замены ПОСЛЕ события
      // Если данная строка вызвана для отображения - значит событие уже наступило (#B секунд перед событием - его нужно просто отобразить) 
      // Строка ПОСЛЕ события не может содержать макроса {P} и в этот блок просто не попадаем
      // Выкусываем макрос {P} без анализа и отображаем то что осталось с учетом замены макроса на остаток времени

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // удаляем макрос. Позиция idx - точка вставки текста остатка времени      
      textLine.remove(idx, idx2 - idx + 1);
      uint16_t insertPoint = idx;

      // Текущее активное событие - momentIdx, который указывает на элемент массива moments[]
      if (momentIdx >= 0 && !noCounterInText) {
        // Вычислить сколько до наступления события осталось времени. 
        // Время события - поле moment структуры Moment элемента массива
        // Данная строка, содержавшая макрос {P} - всегда ДО события, строка ПОСЛЕ события макроса {P} содержать не может и в этот блок не попадает
        time_t t_diff = moments[momentIdx].moment - now(); // кол-во секунд до события
  
        // Пересчет секунд в дней до события / часов до события / минут до события
        uint16_t restDays = t_diff / SECS_PER_DAY;
        uint8_t restHours = (t_diff - (restDays * SECS_PER_DAY)) / SECS_PER_HOUR;
        uint8_t restMinutes = (t_diff - (restDays * SECS_PER_DAY) - (restHours * SECS_PER_HOUR)) / SECS_PER_MIN;
        uint8_t restSeconds = (t_diff - (restDays * SECS_PER_DAY) - (restHours * SECS_PER_HOUR) - (restMinutes * SECS_PER_MIN));
  
        // Если осталось меньше минуты - отображать секунды
        // Если осталось меньше часа - отображать только минуты
        // Если осталось несколько часов - отображать часы и минуты
        // Если осталось меньше-равно 7 дней - отображать дни и часы
        // Если осталось больше 7 дней - отображать дни
        
        String tmp;
        if (restDays == 0 && restHours == 0 && restMinutes == 0)
          { tmp += restSeconds; tmp += WriteSeconds(restSeconds); }
        else if (restDays == 0 && restHours == 0 && restMinutes > 0)
          { tmp += restMinutes; tmp += WriteMinutes(restMinutes); }
        else if (restDays == 0 && restHours > 0 && restMinutes > 0)
          { tmp += restHours; tmp += WriteHours(restHours); tmp += ' '; tmp += restMinutes; tmp += WriteMinutes(restMinutes); }
        else if (restDays > 0 && restDays <= 7 && restHours > 0)
          { tmp += restDays; tmp += WriteDays(restDays); tmp += ' '; tmp += restHours; tmp += WriteHours(restHours); }
        else  
          { tmp += restDays; tmp += WriteDays(restDays); }
          
        String tl_str(textLine.substring(0, insertPoint)); tl_str += tmp; tl_str += textLine.substring(insertPoint);         
        textLine = tl_str;
      }
    }

    // "{S01.01.2020#01.01.2020}"
    // "{S01.01.2020 7:00#01.01.2020 19:00}"
    // "{S01.01.**** 7:00#01.01.**** 19:00}"
    idx = textLine.indexOf("{S");
    if (idx >= 0) {
      // Строка с событием проверки текущей даты - выводится только при совпадении текущей даты с указанной (вычисленной по маске)
      // Проверка строки производится раньше при решении какую строку показывать в getNextLine(). Если сюда попали - значит строка 
      // пригодна к отображению в текущую дату - просто выкусить макрос

      // Закрывающая скобка
      // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
      idx2 = textLine.indexOf("}", idx);        
      if (idx2 < 0) break;

      // удаляем макрос;
      textLine.remove(idx, idx2 - idx + 1);
    }

    // Если в строке еще остались макросы, связанные со временем - обработать их
    if (textLine.indexOf("{D}") >= 0 || textLine.indexOf("{D:") >= 0 || textLine.indexOf("{R") >= 0 || textLine.indexOf("{P") >= 0 || textLine.indexOf("{S") >= 0) continue;

    // Если при разборе строка помечена как многоцветная - обработать макросы цвета 
    if (textHasMultiColor) {                                 
      textLine = processColorMacros(textLine);
    }
    
    break;
  }

  return textLine;
}

String substitureDateMacros(const String& txt) {  
  String str(txt);
  str.replace("DD", "~1~");
  str.replace("dd", "~2~");
  str.replace("D",  "~3~");
  str.replace("d",  "~4~");
  str.replace("MM", "~5~");
  str.replace("mm", "~6~");
  str.replace("M",  "~7~");
  str.replace("m",  "~8~");
  return str;
}

String unsubstitureDateMacros(const String& txt) {  
  String str(txt);
  str.replace("~1~", "DD");
  str.replace("~2~", "dd");
  str.replace("~3~", "D");
  str.replace("~4~", "d");
  str.replace("~5~", "MM");
  str.replace("~6~", "mm");
  str.replace("~7~", "M");
  str.replace("~8~", "m");
  return str;
}

// Обработать макросы цвета в строке, в случае если строка многоцветная
String processColorMacros(const String& txt) {

  String text(txt);
  // Обнулить массивы позиций цвета и самого цвета
  for (uint8_t i = 0; i < MAX_COLORS; i++) {
    textColorPos[i] = 0;
    textColor[i] = 0xFFFFFF;
  }

  // Если макрос цвета указан не с начала строки - начало строки отображать цветом globalTextColor
  uint16_t cnt = 0;
  int16_t idx, idx2;  

  idx = text.indexOf("{C");
  if (idx > 0) {
    textColorPos[cnt] = 0;
    textColor[cnt] = globalTextColor;   
    cnt++;
  }

  // Обработать все макросы цвета, входящие в строку
  while (idx >= 0) {
    // Закрывающая скобка
    // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
    idx2 = text.indexOf("}", idx);        
    if (idx2 < 0) break;

    // Извлечь цвет текста отображения этой бегущей строки
    String tmp;
    if (idx2 - idx > 1) {
      tmp = text.substring(idx+2, idx2);
    }
    
    // удаляем макрос
    text.remove(idx, idx2 - idx + 1);

    // Преобразовать строку цвета в число, сохранить позицию и цвет
    if (cnt < MAX_COLORS) {
      textColorPos[cnt] = idx;
      textColor[cnt] = (uint32_t)HEXtoInt(tmp);
      cnt++;
    }

    // Последний элемент массива - длина строки, чтобы все буквы до конца строки показывались правильным цветом
    textColorPos[cnt] = text.length();;
    textColor[cnt] = globalTextColor;
    
    // Есть еще вхождения макроса?
    idx = text.indexOf("{C");  
  }
  
  return text;
}

// Проверка содержит ли эта строка множественное задание цвета
bool checkIsTextMultiColor(const String& text) {

  // Строка не содержит макроса цвета
  int16_t idx = text.indexOf("{C"), idx_first = idx;
  if (idx < 0) {
    return false;
  }
  
  // Строка отображается одним (указанным) цветом, если цвет указан только один раз в самом начале или в самом конце строки
  // Если цвет в середине строки - значит начало строки отображается цветом globalTextColor, а с позиции макроса и до конца - указанным в макросе цветом
  uint16_t cnt = 0;
  while (idx>=0 && cnt < 2) {
    cnt++;
    idx = text.indexOf("{C", idx + 1);  
  }


  if (cnt == 1 && (idx_first == 0 || idx_first == (int16_t)(text.length() - 10))) {  // text{C#0000FF} поз макр - 4, длина строки - 14, длина макроса - 10
    return false;
  }

  return true;  
}

// получить строку из массива строк текстов бегущей строки по индексу '0'..'9','A'..'Z'
int8_t getTextIndex(char c) {
  int8_t idx = -1;
  if (c >= '0' && c <= '9') 
    idx = (int8_t)(c - '0');
  else if (c >= 'A' && c <= 'Z') 
    idx = 10 + (int8_t)(c - 'A');        
  return (idx < 0 || idx >= TEXTS_MAX_COUNT) ? -1 : idx;
}

// получить строку из массива строк текстов бегущей строки по индексу '0'..'9','A'..'Z'
char getAZIndex(uint8_t idx) {
  char c = '-';
  if (idx <= 9)             
     c = char('0' + idx);
  else if (idx >= 10 && idx < TEXTS_MAX_COUNT)
     c = char('A' + idx - 10);
  return c;
}

// получить строку из массива строк текстов бегущей строки по индексу '0'..'9','A'..'Z'
String getTextByAZIndex(char c) {
  int8_t idx = getTextIndex(c);
  return (idx < 0 || idx >= TEXTS_MAX_COUNT) ? "" : getTextByIndex(idx);
}

// получить строку из массива строк текстов бегущей строки по индексу 0..35
String getTextByIndex(uint8_t idx) {
  return getTextByIndexFS(idx, false);
}

// получить строку из массива строк текстов бегущей строки по индексу 0..35
String getTextByIndexFS(uint8_t idx, bool backup) {
  if (idx >= TEXTS_MAX_COUNT) return "";

  // preparedTextIdx - индекс строки, которая находится в preparedText
  // Если запрошенная по индексу строка совпадает с загруженной - не читать с диска, а вернуть ранее загруженную
  if (preparedTextIdx == idx && preparedTextStorage == "FS") return preparedText;

  // Загрузить текст из файловой стистемы микроконтроллера
  char c = getAZIndex(idx);

  String directoryName(TEXT_STORAGE);
  if (backup) directoryName += ".bak";
  
  String fileName(directoryName); fileName += '/'; fileName += c;

  String text;
  
  if (LittleFS.exists(fileName)) {
    
    File file = LittleFS.open(fileName, "r");
    if (file) {
      // считываем содержимое файла ssid
      char* buf = (char*)malloc(1024);
      memset(buf, '\0', 1024); 
      size_t len = file.read((uint8_t*)buf, 1024);
      file.close();
      if (len > 0) {
        text = String(buf);
        preparedText = text;
        preparedTextIdx = idx;
        preparedTextStorage = "FS";
      }
      free(buf); 
    } else {
      DEBUG(F("Ошибка чтения строки с индексом '")); DEBUG(c); DEBUGLN('\'');    
    }    
  }
  return text;
}

// получить строку из массива строк текстов бегущей строки по индексу 0..35
String getTextByIndexSD(uint8_t idx, bool backup) {
  if (idx >= TEXTS_MAX_COUNT) return "";

  #if (USE_SD == 0 || USE_SD == 1 && FS_AS_SD == 1)

    return getTextByIndexFS(idx, backup);
  
  #else

    // preparedTextIdx - индекс строки, которая находится в preparedText
    // Если запрошенная по индексу строка совпадает с загруженной - не читать с диска, а вернуть ранее загруженную
    if (preparedTextIdx == idx && preparedTextStorage == "SD") return preparedText;
  
    // Загрузить текст из файловой стистемы микроконтроллера
    char c = getAZIndex(idx);
  
    String directoryName(TEXT_STORAGE);
    if (backup) directoryName += ".bak";

    String fileName(directoryName); fileName += '/'; fileName += c;

    String text;

    if (SD.exists(fileName)) {
      
      File file = SD.open(fileName, "r");
      if (file) {
        // считываем содержимое файла ssid
        char* buf = (char*)malloc(1024);
        memset(buf, '\0', 1024); 
        size_t len = file.read((uint8_t*)buf, 1024);
        file.close();
        if (len > 0) {
          text = String(buf);
          preparedText = text;
          preparedTextIdx = idx;
          preparedTextStorage = "SD";
        }
        free(buf);
      } else {
        DEBUG(F("Ошибка чтения строки с индексом '")); DEBUG(c); DEBUGLN('\'');    
      }    
    }
    return text;
  #endif
}

void saveTextLine(char index, const String& text) {
  saveTextLineFS(index, text, false);
}

void saveTextLineFS(char index, const String& text, bool backup) {
  String directoryName(TEXT_STORAGE);
  if (backup) directoryName += ".bak";

  bool ok = true;
  if (!LittleFS.exists(directoryName)) {
    ok = LittleFS.mkdir(directoryName);
    if (!ok) {
      DEBUG(String(MSG_FOLDER_CREATE_ERROR)); DEBUG(F(" '")); DEBUGLN('\'');
    }
  }
  
  String fileName(directoryName); fileName += '/'; fileName += index;

  File file;
  if (LittleFS.exists(fileName)) {
    ok = LittleFS.remove(fileName);
  }
  
  if (ok) {
    file = LittleFS.open(fileName, "w");
    if (file) {
      size_t len = text.length()+1, lenw = 0;
      if (len > 1024) len = 1024;
      char buf[1024];
      memset(buf, '\0', 1024);
      text.toCharArray(buf, len);
      lenw = file.write((uint8_t*)buf, len);
      ok = lenw == len;       
      file.close();
    } else {
      ok = false;
    }
  }
  
  if (!ok) {
    DEBUG(F("Ошибка сохранения строки с индексом '")); DEBUG(index); DEBUGLN('\'');
  } 
}

void saveTextLineSD(char index, const String& text, bool backup) {

  #if (USE_SD == 0 || USE_SD == 1 && FS_AS_SD == 1)
  
    saveTextLineFS(index, text, backup);
  
  #else  
  
    String directoryName(TEXT_STORAGE);
    if (backup) directoryName += ".bak";
  
    bool ok = true;
    if (!SD.exists(directoryName)) {
      ok = SD.mkdir(directoryName);
      if (!ok) {
        DEBUG(String(MSG_FOLDER_CREATE_ERROR)); DEBUG(F(" '")); DEBUG(directoryName); DEBUGLN('\'');      
      }
    }
      
    String fileName(directoryName); fileName += '/'; fileName += index;    
    
    File file;
    
    // Если файл с таким именем уже есть - удалить (перезапись файла новым)
    if (SD.exists(fileName)) {
      ok = SD.remove(fileName);
    }
  
    if (ok) {
      file = SD.open(fileName, "w");
      if (file) {
        size_t len = text.length()+1, lenw = 0;
        if (len > 1024) len = 1024;
        char buf[1024];
        memset(buf, '\0', 1024);
        text.toCharArray(buf, len);
        lenw = file.write((uint8_t*)buf, len);
        ok = lenw == len;       
        file.close();
      } else {
        ok = false;
      }
    }
    
    if (!ok) {
      DEBUG(F("Ошибка сохранения строки с индексом '")); DEBUG(index); DEBUGLN('\'');
    }
     
  #endif
}

// Сканировать массив текстовых строк на наличие событий постоянного отслеживания - макросов {P}
void rescanTextEvents() {
/*
 "{PДД.ММ.ГГГГ#N#B#A#F#T}"
   - где после P указаны опционально дата и время до которой нужно отсчитывать оставшееся время, выводя строку остатка в формате:
     X дней X часов X минут X секунд; 
     Если дней не осталось - выводится только X часов X минут; 
     Если часов тоже уже не осталось - выводится X минут
     Если минут тоже уже не осталось - выводится X секунд

   Строка с режимом {P не выводится при периодическом переборе строк для отображения - только до/после непосредственно времени события:
   время наступления события мониторится и за #B секунд до события бегущая строка начинает отображаться на матрице и после события #A секунд отображается строка-заместитель 
   
   Для режима P дата может опускаться (означает "каждый день") или ее компоненты могут быть заменены звездочкой '*'
   **.10.2020 - весь октябрь 2020 года 
   01.**.**** - каждое первое число месяца любого года
   **.**.2020 - каждый день 2020 года
   
   "{P7:00#N#120#30#12347}"  - каждый пн,вт,ср,чт,вс в 7 утра (за 120 сек до наступления события и 30 секунд после наступления
   - где компоненты даты:
     ДД - число месяца
     MM - месяц
     ГГГГ - год
     Компоненты даты могут быть заменены * - означает "любой"
     
   - где после даты может быть указан
     - #N - если осталось указанная дата уже наступила - вместо этой строки выводится строка с номером N
            если строка замены не указана - просроченная строка не выводится берется следующая строка
     - #B - начинать отображение за указанное количество секунд ДО наступления события (before). Если не указано - 60 секунд по умолчанию
     - #A - отображать строку-заместитель указанное количество секунд ПОСЛЕ наступления события (after). Если не указано - 60 секунд по умолчанию
     - #F - дни недели для которых работает эта строка, когда дата не определена 1-пн..7-вс
            Если указана точная дата и указан день недели, который не соответствует дате - строка выведена не будет
     - #T - если в конце строки есть флаг '#T' - вывести строку с обратным отсчетом один раз (или до тех пор пока до события не останется меньше минуты),
            затем просто по центру выводить обратный отсчет в секундах до события
     - #t - то же, что и #T, только остаток секунд на месте макроса {P} в строке не выводится
*/
  // Предварительная очистка массива постоянно отслеживаемых событий
  for (uint8_t i = 0; i < MOMENTS_NUM; i++) {
    moments[i].moment = 0;
  }

  bool     found  = false;
  bool     flagT  = false;
  bool     flagT2 = false;
  uint8_t  stage = 0;         // 0 - разбор даты (день); 1 - месяц; 2 - год; 3 - часы; 4- минуты; 5 - строка замены; 6 - секунд ДО; 7 - секунд ПОСЛЕ; 8 - дни недели
  uint8_t  iDay = 0, iMonth = 0, iHour = 0, iMinute = 0, iSecond = 0, star_cnt = 0;
  uint16_t iYear = 0;
  uint32_t iBefore = 60, iAfter = 60, num = 0;
  uint8_t  moment_idx = 0;    // индекс элемента в формируемом массиве
  int8_t   text_idx = -1;     // индекс строки заменителя  
  String   wdays = "1234567"; // Дни недели. Если не указано - все дни пн..вс

  // Строка textWithEvents содержит индексы строк, в которых присутствует макрос {P}.
  // Не требуется просматриватьь и обрабатывать все строки - достаточно только строки с наличием макроса ХЗЪ
  
  for (uint16_t i = 0; i < textWithEvents.length(); i++) {

    int16_t t_idx = getTextIndex(textWithEvents[i]);
    if (t_idx < 0) continue;
      
    String text(getTextByIndex(t_idx));
    int16_t idx = text.indexOf("{P");
    if (idx < 0) continue;

    int16_t idx2 = text.indexOf("}", idx);        
    if (idx2 < 0) continue;

    // Вычленяем содержимое макроса "{P}"
    String str(text.substring(idx+2, idx2));
    str.trim();

    if (!found) {
      DEBUGLN(DELIM_LINE);
      DEBUGLN(F("Строки с событием {P}"));
      found = true;
    }
    DEBUGLN(DELIM_LINE);
    DEBUG(F("Строка: '")); DEBUG(text); DEBUGLN("'");

    // Сбрасываем переменные перед разбором очередной строки
    stage  = 0; iDay = 0; iMonth = 0; iYear = 0; iHour = 0; iMinute = 0; iSecond = 0; iBefore = 60; iAfter = 60; star_cnt = 0; num = 0;
    wdays  = "1234567";
    flagT  = false;
    flagT2 = false;

    // Если в строке более чем 1 знак '#' и строка заканчивается на #T - это FlagT. Более 1 раза, т.к если один раз - это строка заместитель с индексом T
    idx = str.indexOf("#");
    if (idx >= 0) {
      idx = str.indexOf("#", idx + 1);
      if (idx > 0) {
        flagT  = str.endsWith("#T") || str.endsWith("#t");
        flagT2 = str.endsWith("#t");
        if (flagT) {
          str = str.substring(0, str.length() - 2);
        }
      }
    }
    
    // Побайтово разбираем строку макроса
    bool err = false;
    for (uint16_t ix = 0; ix < str.length(); ix++) {
      if (err) {
        DEBUGLN();
        DEBUG(F("Ошибка в макросе\n'{P")); DEBUG(str); DEBUG(F("}'\n  "));
        for(uint16_t n=0; n<ix; n++) DEBUG('-');
        DEBUGLN('^');
        break;
      }      
      char c = str[ix];
      switch (c) {
        // замена элемента даты "любой" - день месяц или год
        case '*':
          // только для дня/месяца/года и не более двух звезд для дня/ месяца или четырех для года и  нельзя звезду сочетать с цифрой
          err = (stage > 2) || (star_cnt == 1 && num != 0) || (stage <= 1 && star_cnt > 2) || (stage == 2 && star_cnt > 4);  
          if (!err) {
            star_cnt++;  // счетчик звезд
            num = 0;     // обнулить число          
          }
          break;  
        // Разделитель даты дня/месяца/года
        case '.':
          err = stage > 2;  // точка - разделитель элементов даты и в других стадиях недопустима
          if (!err) {
            switch (stage) {
              case 0: iDay = num;   stage = 1; break; // Следующая стадия - разбор месяца
              case 1: iMonth = num; stage = 2; break; // Следующая стадия - разбор года
              case 2: iYear = num; break;
            }
            star_cnt = 0; num = 0;
          }
          break;  
        // Разделитель часов и минут  
        case ':':
          err = stage != 0 && stage != 3;  // Дата может быть опущена (stage == 0) и текущая стадия stage == 3 - был разбор часов. Если это не так - ошибка
          if (!err) {
            iHour = num;
            stage = 4;    // следующая стадия - разбор минут
            num = 0;
          }
          break;  
        // Разделитель строки замены/времени ДО/времени ПОСЛЕ/дней недели  
        case '#':
          switch (stage) {
            case 2: iYear = num;    stage = 5; star_cnt = 0; num = 0;break;   // Сейчас разбор года - переходим в стадию номера строки заменителя
            case 4: iMinute = num;  stage = 5; star_cnt = 0; num = 0; break;  // Сейчас разбор минут времени - переходим в стадию номера строки заменителя
            case 5: text_idx = num; stage = 6; break;  // Закончен разбор номера строки замены; Следующая стадия - разбор секунд ДО
            case 6: iBefore = num;  stage = 7; break;  // Закончен разбор номера секунд ДО; Следующая стадия - разбор секунд ПОСЛЕ
            case 7: iAfter = num;   stage = 8; break;  // Закончен разбор номера секунд ПОСЛЕ; Следующая стадия - разбор дней недели
            default:
              err = true;    // В любой другой стадии № не на своем месте - ошибка
              break;
          }          
          num = 0;  
          break;  
        // Разделитель даты и времени  
        case ' ':
          err = stage != 2;  // Разделитель даты и времени. Если предыдущая фаза - не разбор года - это ошибка. Пробел в других местах недопустим
          if (!err) {
            iYear = num;
            stage = 3; // следующая стадия - разбор часов
            num = 0;
            star_cnt = 0;
          }
          break;  
        default:
          // Здесь могут быть цифры 0..9 для любой стадии (день/месяц/год/часы/минуты/сек ДО/сек ПОСЛЕ/дни недели/номер строки замены)
          if (c >= '0' && c <= '9') {
            err = star_cnt != 0;     // Если число звезд не равно 0 - цифра сочетается со звездой - нельзя
            if (!err) {
              num = num * 10 + (c - '0');
            }
            break;
          }
          // Здесь могут быть буквы A..Z - только для стадии строка замены и при этом буква должна быть только одна
          if (c >= 'A' && c <= 'Z' && stage == 5 && num == 0) {
            num = getTextIndex(c);
            break;
          }
          // Любой другой символ - ошибка разбора макроса
          err = true;
          break;  
      }

      // Если случилась стадия 8 - все от текущей позиции до конца строки - дни недели
      // Просто копируем остаток строки в дни недели и завершаем разбор
      if (!err && stage == 8) {
        wdays = str.substring(ix+1);
        if (wdays.length() == 0) wdays = "1234567";
        break;
      }  

      // Это последний символ в строке?
      if (ix == str.length() - 1) {
        // Если строка кончилась ДО полного разбора даты или времени - ошибка
        // Остальные параметры могут быть опущены - тогда принимают значения по умолчанию
        // 0 - разбор даты (день); 1 - месяц; 2 - год; 3 - часы; 4- минуты; 5 - строка замены; 6 - секунд ДО; 7 -секунд ПОСЛЕ; 8 - дни недели
        switch (stage) {
          case 2:  iYear    = num; break;
          case 4:  iMinute  = num; break;
          case 5:  text_idx = num; break;
          case 6:  iBefore  = num; break;
          case 7:  iAfter   = num; break;
          default: err      = true;  break;
        }                  
      }
    }

    // Разбор прошел без ошибки? Добавить элемент в массив отслеживаемых событий
    if (!err) {
      // Если день/месяц/год отсутствуют или указаны заменителями - брать текущую   

      bool star_day = false, star_month = false, star_year = false;
      if (iDay   == 0) { iDay   = day();   star_day   = true; }
      if (iMonth == 0) { iMonth = month(); star_month = true; }
      if (iYear  == 0) { iYear  = year();  star_year  = true; }

      // Если год меньше текущего - событие в прошлом - его добавлять в отслеживаемые не нужно 
      if (iYear < year()) continue;
      
      // Сформировать ближайшее время события из полученных компонент
      tmElements_t tm = {iSecond, iMinute, iHour, 0, iDay, iMonth, (uint8_t)CalendarYrToTm(iYear)}; 
      time_t t_event = makeTime(tm);            
      
      // Если событие уже прошло - это может быть, когда дата опущена или звездочками, а время указано меньше текущего - брать то же время следующего дня/месяца/года
      const uint8_t monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};

      while ((uint32_t)t_event < (uint32_t)now() && (star_day || star_month || star_year)) {
        if (star_day) {
          iDay++;
          uint8_t daysInMonth = monthDays[iMonth-1];
          if (daysInMonth == 28) daysInMonth += uint8_t(LEAP_YEAR(iYear - 1970));
          if (iDay>daysInMonth) {
            iDay = 1;
            if (star_month) {
              iMonth++;
              if (iMonth>12) {
                iMonth=1;
                if (star_year) iYear++;
                else break;
              }
            } else {
              break;
            }
          }
        }
        else if (star_month) {
          iMonth++;
          if (iMonth>12) {
            iMonth=1;
            if (star_year) iYear++;
            else break;
          }
        }
        else if (star_year) {
          iYear++;          
        }        
        tm = {0, iMinute, iHour, 0, iDay, iMonth, (uint8_t)CalendarYrToTm(iYear)};
        t_event = makeTime(tm);        
      }
      
      // Если звездочек нет или после перехода к следующему дню время всё равно меньше текущего - событие в прошлом - добавлять не нужно
      if ((uint32_t)t_event < (uint32_t)now()) continue;

      // Полученное время события попадает в разрешенные дни недели? Если нет - добавлять не нужно
      int8_t weekDay = weekday(t_event) - 1;    // day of the week, Sunday is day 0   
      if (weekDay == 0) weekDay = 7;            // Sunday is day 7, Monday is day 1;

      char cc = weekDay + '0';            
      if (wdays.indexOf(cc) < 0) continue;
      
      breakTime(t_event, tm);
      
      String str(F("Событие: ")); 
      str += padNum(tm.Day,2);
      str += ".";
      str += padNum(tm.Month,2);
      str += ".";
      str += padNum(tmYearToCalendar(tm.Year),4);
      str += " ";
      str += padNum(tm.Hour,2);
      str += ":";
      str += padNum(tm.Minute,2);
      str += F("; before=");
      str += iBefore;
      str += F("; after=");
      str += iAfter;
      str += F("; days='");
      str += wdays;
      str += F("'; text='");
      str += getAZIndex(t_idx);
      str += F("'; replace='");
      str += getAZIndex(text_idx);
      str += "'; center=";
      str += flagT ? "true" : "false";
      DEBUGLN(str);
      str.clear();
        
      // Заполнить текущий элемент массива полученными параметрами
      moments[moment_idx].moment  = t_event;
      moments[moment_idx].before  = iBefore;
      moments[moment_idx].after   = iAfter;
      moments[moment_idx].index_b = t_idx;
      moments[moment_idx].index_a = text_idx;
      moments[moment_idx].flagT   = flagT;
      moments[moment_idx].flagT2  = flagT2;

      // Строка-заместитель должна быть отключена, чтобы она не отображалась как регулярная строка
      if (text_idx >= 0) {
        String text(getTextByIndex(text_idx));
        if (text.length() > 0) {
          bool disabled = (text_idx == 0 && text[0] == '#') || text[0] == '-' || text.indexOf("{-}") >= 0; 
          if (!disabled) {
            text = "-" + text;
            saveTextLine(getAZIndex(text_idx), text);
          }
        }
      }
      
      // К следующему элементу массива
      moment_idx++;
    }
  }
  DEBUGLN(DELIM_LINE);
  textCheckTime = millis();
}

// Проверить есть ли в настоящий момент активное событие?
// Возврат - индекс строки текста в массиве текстовых строк или -1, если активного события нет
void checkMomentText() {
  momentIdx = -1;                   // Индекс строки в массиве moments для активного текущего непрерывно отслеживаемого события
  momentTextIdx = -1;               // Индекс строки для активного текущего непрерывно отслеживаемого события
  time_t this_moment = now();
  for (uint8_t i = 0; i < MOMENTS_NUM; i++) {
    // Не содержит события
    if (moments[i].moment == 0) break;
    // Время за #B секунд до наступления события? - отдать index_b
    if ((uint32_t)this_moment >= moments[i].moment - moments[i].before && this_moment < moments[i].moment) {
      momentIdx = i;
      momentTextIdx = moments[i].index_b; // before 
      isPTextCentered = moments[i].flagT;
      noCounterInText = moments[i].flagT2;
      break;
    }    
    // Время #А секунд после наступления события? - отдать index_a
    if ((time_t)this_moment >= moments[i].moment && (time_t)this_moment <= moments[i].moment + (time_t)moments[i].after) {
      momentIdx = i;
      momentTextIdx = moments[i].index_a; // after
      isPTextCentered = false;
      noCounterInText = false;
      break;
    }    
  }
}

// Проверить текст, содержащий макрос {S}
// Возвращает true - если дата в макросе после расшифровки совпадает с текущей датой - текст можно отображать
// Если дата не совпадает - текст отображать сегодня нельзя - еще не пришло (или уже прошло) время для этого текста
bool forThisDate(const String& pText) { 
  /*
     text - в общем случае - "{S01.01.**** 7:00:00#01.01.**** 19:00:00}" - содержит даты начала и конца, разделенные символом "#"
     Дата как правило имеет формат "ДД.ММ.ГГГГ ЧЧ:MM:СС"; В дате День может быть замене на "**" - текущий день, месяц - "**" - текущий месяц, год - "****" - текущий год или "***+" - следующий год
     Примеры:
       "{S01.01.2020#01.01.2020}"
       "{S01.01.2020 7:00#01.01.2020 19:00}"
       "{S01.01.**** 7:00#01.01.**** 19:00:00}"
         - где после S указаны даты начала и конца периода доступного для отображения строки.
         Для режима S элементы даты быть заменены звездочкой '*'
         **.10.2020 - весь октябрь 2020 года 
         01.**.**** - каждое первое число месяца
         **.**.2020 - каждый день 2020 года
         Если время начала периода отсутствует - считается 00:00:00
         Если время конца периода отсутствует  - считается 23:59:59
         Допускается указывать несколько макросов {S} в строке для определения нескольких разрешенных диапазонов
  */
  bool   ok = false;
  int16_t idx2;

  String text(pText);
  int16_t idx = text.indexOf("{S");
  
  while (idx >= 0) {
    // Строка с событием проверки текущей даты - выводится только при совпадении текущей даты с указанной (вычисленной по маске)
    // Проверка строки производится раньше при решении какую строку показывать в getNextLine(). Если сюда попали - значит строка 
    // пригодна к отображению в текущую дату - просто выкусить макрос

    // Закрывающая скобка
    // Если ее нет - ошибка, ничего со строкой не делаем, отдаем как есть
    idx2 = text.indexOf("}", idx);        
    if (idx2 < 0) break;

    // Извлечь дату события из макроса
    String str;
    if (idx2 - idx > 1) {
      str = text.substring(idx + 2, idx2);
      str.trim();
    }

    // Проверить дату
    if (str.length() > 0) {
      /*
      DEBUGLN(DELIM_LINE);
      DEBUG(F("Строка: '"));
      DEBUGLN(text + "'");
      */
      time_t now_moment = now();
      extractMacroSDates(str);
      ok = (now_moment >= (time_t)textAllowBegin) && (now_moment <= (time_t)textAllowEnd);
      /*
      DEBUGLN("now=" + String(now_moment) + "; start=" + String(textAllowBegin) + "; end=" + String(textAllowEnd));
      if (ok) DEBUGLN(F("вывод разрешен"));
      else    DEBUGLN(F("вывод запрещен"));
      DEBUGLN(DELIM_LINE); 
      */
    }

    // Дата проверена и совпадает с допустимым диапазоном - строку можно отображать
    if (ok) break;
    
    // удаляем макрос;
    text.remove(idx, idx2 - idx + 1);

    // Есть ли еще макрос {S} в строке?
    idx = text.indexOf("{S");
  }
  
//  DEBUGLN(DELIM_LINE);

  return ok;
}

void extractMacroSDates(const String& text) {

  // Text - в общем случае - "01.01.**** 7:00:00#01.01.**** 19:00:00" - содержит даты начала и конца, разделенные символом "#"
  // Дата как правило имеет формат "ДД.ММ.ГГГГ ЧЧ:MM:СС"; В дате День может быть замене на "**" - текущий день, месяц - "**" - текущий месяц, год - "****" - текущий год или "***+" - следующий год
  // Если не указана дата начала интервала - берется дата конца интервала и время ставится в 00:00:00
  // Если не указана дата конца интервала - берется дата начала интервала и время ставится в 23:59:59
  
  textAllowBegin = 0;        // время начала допустимого интервала отображения unixTime
  textAllowEnd = 0;          // время конца допустимого интервала отображения unixTime

  if (text.length() == 0) {
    DEBUG(F("Строка: '")); DEBUG(text); DEBUGLN("'");
    DEBUGLN(F("Ошибка: макрос {S} не содержит интервала дат"));                 
    return;
  }      

  
  int16_t idx = text.indexOf('#');

  bool hasDate1 = idx != 0;  // -1 или больше нуля означает,что '#' либо нет - тогда вся строка - data1, либо больше 0 - есть часть для data1; в строке '#07.01.****' есть data2, нет data1
  bool hasDate2 = idx > 0;   // Если в строке есть '#' - значит data2 присутствует
  
  String s_date1(idx < 0 ? text : ( idx == 0 ? "" : text.substring(0, idx)));
  String s_date2(idx >=0 ? text.substring(idx+1) : "");

  // Сформировать ближайшее время события из полученных компонент  
  tmElements_t tm1, tm2;
  if (hasDate1) tm1 = ParseDateTime(s_date1);
  if (hasDate2) tm2 = ParseDateTime(s_date2);

  // Если нет начала интервала - брать время 00:00:00 и дату конца интервала 
  if (!hasDate1) tm1 = {0, 0, 0, 0, tm2.Day, tm2.Month, tm2.Year };

  // Если нет конца интервала - брать время 23:59:59 и дату начала интервала 
  if (!hasDate2) tm2 = {59, 59, 23, 0, tm1.Day, tm1.Month, tm1.Year };

  // Если в строке даты окончания периода время не указано вообще (отсутствует пробел как разделитель даты и времени) - время окончания поставить 23:59:59
  if (hasDate2 && s_date2.indexOf(' ') < 0) {
    tm2 = {59, 59, 23, 0, tm2.Day, tm2.Month, tm2.Year };
  }
      
  time_t t_event1 = makeTime(tm1);
  time_t t_event2 = makeTime(tm2);

  /*
  DEBUGLN(DELIM_LINE); 
  DEBUGLN("date1='" + s_date1 + ";");
  DEBUGLN("date2='" + s_date2 + ";");
  DEBUGLN(String(F("Интервал показа: ")) + 
                 padNum(tm1.Day,2) + "." + padNum(tm1.Month,2) + "." + padNum(tmYearToCalendar(tm1.Year),4) + " " + padNum(tm1.Hour,2) + ":" + padNum(tm1.Minute,2) + ":" + padNum(tm1.Second,2) + " -- " +
                 padNum(tm2.Day,2) + "." + padNum(tm2.Month,2) + "." + padNum(tmYearToCalendar(tm2.Year),4) + " " + padNum(tm2.Hour,2) + ":" + padNum(tm2.Minute,2) + ":" + padNum(tm2.Second,2));
  DEBUGLN(DELIM_LINE); 
  */
  
  if (t_event2 < t_event1) {
    DEBUG(F("Строка: '")); DEBUG(text); DEBUGLN("'");
    String str(F("Интервал показа: "));
    str += padNum(tm1.Day,2);
    str += '.';
    str += padNum(tm1.Month,2);
    str += '.';
    str += padNum(tmYearToCalendar(tm1.Year),4);
    str += ' ';
    str += padNum(tm1.Hour,2);
    str += ':';
    str += padNum(tm1.Minute,2);
    str += ':';
    str += padNum(tm1.Second,2);
    str += " -- ";
    str += padNum(tm2.Day,2);
    str += '.';
    str += padNum(tm2.Month,2);
    str += '.';
    str += padNum(tmYearToCalendar(tm2.Year),4);
    str += ' ';
    str += padNum(tm2.Hour,2);
    str += ':';
    str += padNum(tm2.Minute,2);
    str += ':';
    str += padNum(tm2.Second,2);
    DEBUGLN(str);
    str.clear();
    DEBUGLN(F("Ошибка: дата начала больше даты окончания разрешенного интервала"));                 
    textAllowBegin = 0; // время начала допустимого интервала отображения unixTime
    textAllowEnd   = 0; // время конца допустимого интервала отображения unixTime
    return;
  }      
  
  textAllowBegin = t_event1; // время начала допустимого интервала отображения unixTime
  textAllowEnd   = t_event2; // время конца допустимого интервала отображения unixTime
}

bool isFirstLineControl() {

  // По умолчанию - строка 0 - обычная строка для отображения, а не управляющая
  // textIndecies - строка с индексом '0'
  // Если textIndecies начинается с '#'  - это строка содержит последовательность индексов строк в каком порядке их отображать. Индексы - 0..9,A..Z
  // Если textIndecies начинается с '##' - это управляющая строка, показывающая, что строки отображать в случайном порядке
  
  bool isControlLine = false;

  // Однако если строка не пуста и начинается с '#' - это управляющая строка
  if (textIndecies.length() > 0 && textIndecies[0] == '#') {
    isControlLine = true;
    sequenceIdx = 1;
    // Строка из одного символа, и это '#' - считаем что строка - "##" - управляющая - случайное отображение строк
    if (textIndecies.length() < 2) {
      textIndecies = "##";
      saveTextLine('0', textIndecies);
    }
    // Если второй символ в строке тоже '#' - остальная часть строки нам не нужна - отбрасываем
    if (textIndecies[1] == '#') {
      textIndecies = "##";
      saveTextLine('0', textIndecies);
    }
  }

  if (isControlLine) {
    // Допускаются только 1..9,A-Z в верхнем регистре, остальные удалить; 
    // textIndecies[1] == '#' - допускается - значит брать случайную последовательность
    // '0' - НЕ допускается - т.к. строка массива с индексом 0 - и есть управляющая
    textIndecies.toUpperCase();
    for (uint16_t i = 1; i < textIndecies.length(); i++) {
      char c = textIndecies.charAt(i);
      if ((i == 1 && c == '#') || (c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z')) continue;
      textIndecies[i] = ' ';
    }
    textIndecies.replace(" ", "");
    if (textIndecies.length() < 2) {
      textIndecies = "##";
      saveTextLine('0', textIndecies);
    }
  }
  
  return isControlLine;  
}

tmElements_t ParseDateTime(const String& str) {

  uint8_t  aday = day();
  uint8_t  amnth = month();
  uint16_t ayear = year();
//uint8_t  hrs = hour();
//uint8_t  mins = minute();
//uint8_t  secs = second();

  uint16_t iYear = 0;  
  uint8_t iMonth = 0, iDay = 0, iHours = 0, iMinutes = 0, iSeconds = 0;
  int8_t idx;
  
  tmElements_t tm;  

  String s_date;
  String s_time;

  // Корректная дата - 10 символов (ДД.ММ.ГГГГ), точки в позициях 2 и 5; ГГГГ может быть '****' - текущий год или '***+' - следующий год
  // Если есть время - оно отделено пробелом от даты, формат (ЧЧ:MM:СС), часы и минуты и секундв разделены двоеточием
  if (str.length() >= 10) {
    idx = str.indexOf(" ");
    if (idx < 0) idx = str.length();
    s_date = str.substring(0,idx);
    s_time = str.substring(idx+1);
    s_time.trim();
    s_date.trim();
  }

  /*
  DEBUGLN(DELIM_LINE);
  DEBUGLN("text = '" + str + "'");
  DEBUGLN("parse -> date = '" + s_date + "'; time = '" + s_time + "'");
  */
  
  if (s_date.length() == 10 && s_date.charAt(2) == '.' && s_date.charAt(5) == '.') {
    
    idx = CountTokens(s_date, '.');
    
    if (idx > 0) {
      String sDay(GetToken(s_date, 1, '.'));
      String sMonth(idx >= 2 ? GetToken(s_date, 2, '.') : "0");
      String sYear(idx >= 3 ? GetToken(s_date, 3, '.') : "0");
      
      iDay   = sDay == "**" ? aday : sDay.toInt();
      iMonth = sMonth == "**" ? amnth : sMonth.toInt();
      iYear  = sYear == "****" ? ayear : (sYear == "***+" ? (ayear + 1) : sYear.toInt());
    
      if (iDay   == 0) iDay   = aday;
      if (iMonth == 0) iMonth = amnth;
      if (iYear  == 0) iYear  = ayear;
    }
    
  }

  if (s_time.length() > 0) {
    idx = CountTokens(s_time, ':');              
    if (idx > 0) {
      iHours = GetToken(s_time, 1, ':').toInt();
      iMinutes = idx >= 2 ? GetToken(s_time, 2, ':').toInt() : 0;
      iSeconds = idx >= 3 ? GetToken(s_time, 3, ':').toInt() : 0;
    }
  }
  
  tm = {iSeconds, iMinutes, iHours, 0, iDay, iMonth, (uint8_t)CalendarYrToTm(iYear)};

  /*
  DEBUGLN(String(F("Parse out: ")) + padNum(tm.Day,2) + "." + padNum(tm.Month,2) + "." + padNum(tmYearToCalendar(tm.Year),4) + " " + padNum(tm.Hour,2) + ":" + padNum(tm.Minute,2) + ":" + padNum(tm.Second,2));
  DEBUGLN(DELIM_LINE);
  */
  
  return tm;
}
