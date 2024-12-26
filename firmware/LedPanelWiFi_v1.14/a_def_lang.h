// Файл поддержки языков интерфейса управления, например в Web-приложении
// В зависимости от выбранного языка интерфейса в Web-приложение передается значение
// идентификатора LANG, что указывает Web-приложению строки какого языка использовать
// в интерфейсе.
//
// Также в интерфейс передаются списки названий эффектов, параметров и некоторые другие строки из скетча
// для правильного соответствия выбранному языку.
// 
// Языковые особенности имеют также макросы бегущей строки, связанные с датами - названия месяцев, дней недели,
// обозначение времени - дни, часы, минуты. Все эти строки задаются в этом файле и используются варианты языка 
// в зависимости от выбранного значения LANG
//
// Для добавления своего языка интерфейса скопируйте все содержимое блока разметки языка
// #if (LANG == 'RUS')
// ...
// #endif
// Замените сигнатуру RUS на соответствующую вашему языку, выполните перевод строк внутри скопированного блока
//
// В папке firmware/data/lang найдите файл rus.json и скопироуйте его в файл, с именем, соответствующем вышему языку,
// с буквами той же сигнатуры, которую вы задали шагом выше, только в строчном регистре
//
// Файл языка Web-интерфейса - это JSON файл, содержащий пары строк ключ-значение. 
// Ключ - в левой части пары, отделен от значения двоеточием ':'
// Выполните перевод строк значений на ваш язык, оставляя ключ неизменным - на русском языка.
//
// После загрузки исправленного web-интерфейса в файловую систему микроконтроллера Web-интерфейс будет отображаться
// на вашем языке.
//
//
// Измените здесь при необходимости сигнатуру языка, используемого в интерфейсе
// В этом файле есть поддержка языков Русского (сигнатура 'RUS'), Английского (сигнатура 'ENG') и Испанского (сигнатура 'ESP')
// 
#ifndef LANG
#define LANG 'RUS'
#endif

// ============================================== RUS ============================================== 

#if (LANG == 'RUS')
  #define UI F("RUS")
  
  // Список и порядок эффектов, передаваемый в Web-интерфейс. 
  // Порядок имен эффектов в списке должен соответствовать списку эффектов, определенному в файле a_def_soft.h 
  // в строках, начиная с 89. Данный список передается в Web-клиент, для чего требуется буфер. Буфер имеет размер MAX_BUFFER_SIZE
  // Если даннвя строка (UTF-8, два байта на символ) не влезет в буфер - эффекты не будут переданы в Web-интерфейс, плашки эффектов будут отсутствовать,
  // в логах браузера будет строчка
  //  {"e": "stt","d": "{\"LE\":null}"}
  //  json='{"LE":null}'
  // В этом случае нужно либо сокращать имена эффектов, либо увеличивать размер буфера. Но увеличение размера буфера может привести к нехватки памяти
  // и нестабильной работе прошивки на ESP8266. На ESP32 памяти больше - размер буфера можно увеличиватью

  static const char EFFECT_LIST[] PROGMEM =
    "Часы,Лампа,Снегопад,Кубик,Радуга,Пейнтбол,Огонь,The Matrix,Червячки,Звездопад,Конфетти," 
    "Цветной шум,Облака,Лава,Плазма,Бензин на воде,Павлин,Зебра,Шумящий лес,Морской прибой,Смена цвета," 
    "Светлячки,Водоворот,Циклон,Мерцание,Северное сияние,Тени,Лабиринт,Змейка,Тетрис,Арканоид," 
    "Палитра,Спектрум,Синусы,Вышиванка,Дождь,Камин,Стрелки,Узоры,Рубик,Звёзды,Штора,Трафик,Рассвет,Поток,Фейерверк,Полосы"

  // Эффекты Анимации, Погоды, Слайды и SD-карта могут быть отключены условиями USE_ANIMATION = 0 и USE_SD = 0
  // Список эффектов передается в WebUI позиционно: эффект "Часы" имеют ID=0, эффект "SD-карта" имеет ID=47 (см. определение в a_def_soft.h)
  // При включении на стороне WebUI в контроллер передается ID (точнее позиция эффекта в списке). 
  // Чтобы при отключении например "Анимациия","Погода","Слайды" не было смещения - нумерации эффектов вместо отсутствующих передается пустая строка
  // Тогда при получении списка WebUI пропустит пустые эффекты, но позиции останутся правильными и эффект "SD-карта" будет иметь ID=47, а не 44
  
  #if (USE_ANIMATION == 1)
    ",Анимация,Погода,Слайды"
  #else  
    ",,,"
  #endif
  
  #if (USE_SD == 1)   
    ",SD-Карта"
  #else  
    ","
  #endif
  ;                 // <-- эта точка с запятой закрывает оператор static const char EFFECT_LIST[] PROGMEM =

  // ****************** ОПРЕДЕЛЕНИЯ ПАРАМЕТРОВ БУДИЛЬНИКА ********************
  
  #if (USE_MP3 == 1)
  // SD карточка в MP3 плеере (DFPlayer) содержит в корне три папки - "01","02" и "03"
  // Папка "01" содержит MP3 файлы звуков, проигрываемых при наступлении события будильника
  // Папка "02" содержит MP3 файлы звуков, проигрываемых при наступлении события рассвета
  // Папка "03" содержит MP3 файлы звуков, проигрываемых в макросе {A} бегущей строки
  // DFPlayer не имеет возможности считывать имена файлов, только возможность получить количество файлов в папке.
  // Команда на воспроизведение звука означает - играть файл с индексом (номером) N из папки M
  // Номера файлов определяются таблицей размещения файлов SD-карты (FAT) и формируются в порядке очереди записи файлов на чистую флэшку
  // Так, файл записанный в папку первым получает номер 1, второй - 2 и так далее и никак не зависит от имен файлов
  // Данные массивы содержат отображаемые в приложении имена звуков в порядке, соответствующем нумерации записанных в папки файлов.
  //
  // Для формирования правильного порядка файлов звуков переименуйте их на компьютере во временной папке так, чтобы они 
  // формировали нужный порядок, например - задайте им просто числовые имена вроде 001.mp3, 002.mp3 и так далее или 
  // задайте числовой префикс существующему имени, например 01_birds.mpr, 02_thunder.mp3 и так далее
  // Файлы в папке должны быть отсортированы в порядке имен.
  // Далее создайте на чистой microSD-карте папку в которую будут помещены звуковые файлы и скопируйте их в 
  // упорядоченном списке.
  
  // Список звуков для комбобокса "Звук будильника" в приложении на смартфоне
  static const char ALARM_SOUND_LIST[] PROGMEM = 
    "One Step Over,In the Death Car,Труба зовет,Маяк,Mister Sandman,Шкатулка,Banana Phone,Carol of the Bells";
  
  // Список звуков для комбобокса "Звук рассвета" в приложении на смартфоне  
  static const char DAWN_SOUND_LIST[] PROGMEM =
    "Птицы,Гроза,Прибой,Дождь,Ручей,Мантра,La Petite Fille De La Mer";
  
  // Список звуков для макроса {A} бегущей строки
  static const char NOTIFY_SOUND_LIST[] PROGMEM = 
    "Piece Of Сake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board,"
    "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal,"
    "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3";
  #endif

  // Список названия анимаций. Анимации определены в файле 'animation.ino'
  #define LANG_IMAGE_LIST_DEF
  static const char LANG_IMAGE_LIST[] PROGMEM = 
    "Сердце,Марио,Погода";

  // Список названий узоров
  static const char LANG_PATTERNS_LIST[] PROGMEM = 
    "Зигзаг,Ноты,Ромб,Сердце,Елка,Клетка,Смайлик,Зигзаг,Полосы,Волны,Чешуя,Портьера,Плетенка,Снежинка,Квадратики,Греция,Круги,Рулет,"
    "Узор 1,Узор 2,Узор 3,Узор 4,Узор 5,Узор 6,Узор 7,Узор 8,Узор 9,Узор 10,Узор 11,Узор 12,Узор 13,Узор 14";

  // Погодные условия от Yandex
  static const char Y_CODE_01[] PROGMEM = "облачно с прояснениями, небольшой дождь";           // cloudy, light rain
  static const char Y_CODE_02[] PROGMEM = "облачно с прояснениями, небольшой снег";            // cloudy, light snow
  static const char Y_CODE_03[] PROGMEM = "облачно с прояснениями, небольшой снег с дождем";   // cloudy, wet snow
  static const char Y_CODE_04[] PROGMEM = "переменная облачность";                             // partly cloudy
  static const char Y_CODE_05[] PROGMEM = "переменная облачность, дождь";                      // partly cloudy, rain
  static const char Y_CODE_06[] PROGMEM = "переменная облачность, снег";                       // partly cloudy, show
  static const char Y_CODE_07[] PROGMEM = "переменная облачность, снег с дождем";              // partly cloudy, wet snow
  static const char Y_CODE_08[] PROGMEM = "метель";                                            // snowstorm
  static const char Y_CODE_09[] PROGMEM = "туман";                                             // fog
  static const char Y_CODE_10[] PROGMEM = "пасмурно";                                          // overcast
  static const char Y_CODE_11[] PROGMEM = "пасмурно, временами дождь";                         // overcast, light rain 
  static const char Y_CODE_12[] PROGMEM = "пасмурно, временами снег";                          // overcast, light snow
  static const char Y_CODE_13[] PROGMEM = "пасмурно, временами снег с дождем";                 // overcast, wet snow 
  static const char Y_CODE_14[] PROGMEM = "пасмурно, дождь";                                   // overcast, rain
  static const char Y_CODE_15[] PROGMEM = "пасмурно, снег с дождем";                           // overcast, wet snow
  static const char Y_CODE_16[] PROGMEM = "пасмурно, снег";                                    // overcast, show
  static const char Y_CODE_17[] PROGMEM = "пасмурно, дождь c грозой";                          // overcast, thunderstorm withrain
  static const char Y_CODE_18[] PROGMEM = "ясно";                                              // clear

  // Погодные условия от OpenWeatherMap
  static const char W_CODE_200[] PROGMEM = "Гроза, небольшой дождь";                           // thunderstorm with light rain
  static const char W_CODE_201[] PROGMEM = "Дождь с грозой";                                   // thunderstorm with rain
  static const char W_CODE_202[] PROGMEM = "Гроза, ливни";                                     // thunderstorm with heavy rain
  static const char W_CODE_210[] PROGMEM = "Небольшая гроза";                                  // light thunderstorm
  static const char W_CODE_211[] PROGMEM = "Гроза";                                            // thunderstorm
  static const char W_CODE_212[] PROGMEM = "Сильная гроза";                                    // heavy thunderstorm
  static const char W_CODE_221[] PROGMEM = "Прерывистые грозы";                                // ragged thunderstorm
  static const char W_CODE_230[] PROGMEM = "Гроза, небольшой дождь";                           // thunderstorm with light drizzle
  static const char W_CODE_231[] PROGMEM = "Гроза с дождем";                                   // thunderstorm with drizzle
  static const char W_CODE_232[] PROGMEM = "Гроза с проливным дождем";                         // thunderstorm with heavy drizzle
  static const char W_CODE_300[] PROGMEM = "Мелкий дождь";                                     // light intensity drizzle
  static const char W_CODE_301[] PROGMEM = "Моросящий дождь";                                  // drizzle
  static const char W_CODE_302[] PROGMEM = "Сильный дождь";                                    // heavy intensity drizzle
  static const char W_CODE_310[] PROGMEM = "Небольшой дождь";                                  // light intensity drizzle rain
  static const char W_CODE_311[] PROGMEM = "Моросящий дождь";                                  // drizzle rain
  static const char W_CODE_312[] PROGMEM = "Сильный дождь";                                    // heavy intensity drizzle rain
  static const char W_CODE_313[] PROGMEM = "Ливень, дождь и морось";                           // shower rain and drizzle
  static const char W_CODE_314[] PROGMEM = "Сильный ливень, дождь и морось";                   // heavy shower rain and drizzle
  static const char W_CODE_321[] PROGMEM = "Моросящий дождь";                                  // shower drizzle  
  static const char W_CODE_500[] PROGMEM = "Небольшой дождь";                                  // light rain
  static const char W_CODE_501[] PROGMEM = "Умеренный дождь";                                  // moderate rain
  static const char W_CODE_502[] PROGMEM = "Ливень";                                           // heavy intensity rain
  static const char W_CODE_503[] PROGMEM = "Проливной дождь";                                  // very heavy rain
  static const char W_CODE_504[] PROGMEM = "Проливной дождь";                                  // extreme rain
  static const char W_CODE_511[] PROGMEM = "Град";                                             // freezing rain
  static const char W_CODE_520[] PROGMEM = "Небольшой дождь";                                  // light intensity shower rain
  static const char W_CODE_521[] PROGMEM = "Моросящий дождь";                                  // shower rain
  static const char W_CODE_522[] PROGMEM = "Сильный дождь";                                    // heavy intensity shower rain
  static const char W_CODE_531[] PROGMEM = "Временами дождь";                                  // ragged shower rain
  static const char W_CODE_600[] PROGMEM = "Небольшой снег";                                   // light snow
  static const char W_CODE_601[] PROGMEM = "Снег";                                             // Snow
  static const char W_CODE_602[] PROGMEM = "Снегопад";                                         // Heavy snow
  static const char W_CODE_611[] PROGMEM = "Слякоть";                                          // Sleet
  static const char W_CODE_612[] PROGMEM = "Легкий снег";                                      // Light shower sleet
  static const char W_CODE_613[] PROGMEM = "Ливень, снег";                                     // Shower sleet
  static const char W_CODE_615[] PROGMEM = "Мокрый снег";                                      // Light rain and snow
  static const char W_CODE_616[] PROGMEM = "Дождь со снегом";                                  // Rain and snow
  static const char W_CODE_620[] PROGMEM = "Небольшой снегопад";                               // Light shower snow
  static const char W_CODE_621[] PROGMEM = "Снегопад, метель";                                 // Shower snow
  static const char W_CODE_622[] PROGMEM = "Сильный снегопад";                                 // Heavy shower snow
  static const char W_CODE_701[] PROGMEM = "Туман";                                            // mist
  static const char W_CODE_711[] PROGMEM = "Дымка";                                            // Smoke
  static const char W_CODE_721[] PROGMEM = "Легкий туман";                                     // Haze
  static const char W_CODE_731[] PROGMEM = "Пыльные вихри";                                    // sand/ dust whirls
  static const char W_CODE_741[] PROGMEM = "Туман";                                            // fog
  static const char W_CODE_751[] PROGMEM = "Песчаные вихри";                                   // sand
  static const char W_CODE_761[] PROGMEM = "Пыльные вихри";                                    // dust
  static const char W_CODE_762[] PROGMEM = "Вулканический пепел";                              // volcanic ash
  static const char W_CODE_771[] PROGMEM = "Шквалистый ветер";                                 // squalls
  static const char W_CODE_781[] PROGMEM = "Торнадо";                                          // tornado
  static const char W_CODE_800[] PROGMEM = "Ясно";                                             // clear sky
  static const char W_CODE_801[] PROGMEM = "Небольшая облачность";                             // few clouds: 11-25%
  static const char W_CODE_802[] PROGMEM = "Переменная облачность";                            // scattered clouds: 25-50%
  static const char W_CODE_803[] PROGMEM = "Облачно с прояснениями";                           // broken clouds: 51-84%
  static const char W_CODE_804[] PROGMEM = "Пасмурно";                                         // overcast clouds: 85-100%

  // Константы вывода даты в бегущей строке

  static const char SMonth_01[]  PROGMEM = "января";
  static const char SMonth_02[]  PROGMEM = "февраля";
  static const char SMonth_03[]  PROGMEM = "марта";
  static const char SMonth_04[]  PROGMEM = "апреля";
  static const char SMonth_05[]  PROGMEM = "мая";
  static const char SMonth_06[]  PROGMEM = "июня";
  static const char SMonth_07[]  PROGMEM = "июля";
  static const char SMonth_08[]  PROGMEM = "августа";
  static const char SMonth_09[]  PROGMEM = "сентября";
  static const char SMonth_10[]  PROGMEM = "октября";
  static const char SMonth_11[]  PROGMEM = "ноября";
  static const char SMonth_12[]  PROGMEM = "декабря";

  static const char SMnth_01[]   PROGMEM = "янв";
  static const char SMnth_02[]   PROGMEM = "фев";
  static const char SMnth_03[]   PROGMEM = "мар";
  static const char SMnth_04[]   PROGMEM = "апр";
  static const char SMnth_05[]   PROGMEM = "май";
  static const char SMnth_06[]   PROGMEM = "июн";
  static const char SMnth_07[]   PROGMEM = "июл";
  static const char SMnth_08[]   PROGMEM = "авг";
  static const char SMnth_09[]   PROGMEM = "сен";
  static const char SMnth_10[]   PROGMEM = "окт";
  static const char SMnth_11[]   PROGMEM = "ноя";
  static const char SMnth_12[]   PROGMEM = "дек";

  static const char SDayFull_1[] PROGMEM = "понедельник";
  static const char SDayFull_2[] PROGMEM = "вторник";
  static const char SDayFull_3[] PROGMEM = "среда";
  static const char SDayFull_4[] PROGMEM = "четверг";
  static const char SDayFull_5[] PROGMEM = "пятница";
  static const char SDayFull_6[] PROGMEM = "суббота";
  static const char SDayFull_7[] PROGMEM = "воскресенье";

  static const char SDayShort_1[] PROGMEM = "пон";
  static const char SDayShort_2[] PROGMEM = "втр";
  static const char SDayShort_3[] PROGMEM = "срд";
  static const char SDayShort_4[] PROGMEM = "чтв";
  static const char SDayShort_5[] PROGMEM = "птн";
  static const char SDayShort_6[] PROGMEM = "сбт";
  static const char SDayShort_7[] PROGMEM = "вск";
  
  static const char SDayShrt_1[] PROGMEM = "пн";
  static const char SDayShrt_2[] PROGMEM = "вт";
  static const char SDayShrt_3[] PROGMEM = "ср";
  static const char SDayShrt_4[] PROGMEM = "чт";
  static const char SDayShrt_5[] PROGMEM = "пт";
  static const char SDayShrt_6[] PROGMEM = "сб";
  static const char SDayShrt_7[] PROGMEM = "вс";

  static const char SDayForm_1[] PROGMEM = " дней";        // Пробел в начале обязателен
  static const char SDayForm_2[] PROGMEM = " день";
  static const char SDayForm_3[] PROGMEM = " дня";
 
  static const char SHourForm_1[] PROGMEM = " час";        // Пробел в начале обязателен
  static const char SHourForm_2[] PROGMEM = " часа";
  static const char SHourForm_3[] PROGMEM = " часов";

  static const char SMinuteForm_1[] PROGMEM = " минут";    // Пробел в начале обязателен
  static const char SMinuteForm_2[] PROGMEM = " минута";
  static const char SMinuteForm_3[] PROGMEM = " минуты";

  static const char SSecondForm_1[] PROGMEM = " секунд";   // Пробел в начале обязателен
  static const char SSecondForm_2[] PROGMEM = " секунда";
  static const char SSecondForm_3[] PROGMEM = " секунды";

  // Примеры строк для "Бегущей строки", которые будут заполнены этими значениями при установленном флаге INITIALIZE_TEXTS == 1 в a_def_hard.h (в строке 62)
  // на шаге инициализации при первом запуске прошивки на микроконтроллере.
  // Данные примеры содержат некоторые варианты использования макросов в бегущей строке.
  
  #if (INITIALIZE_TEXTS == 1)
  static const char textLine_0[] PROGMEM = "##";
  static const char textLine_1[] PROGMEM = "Пусть сбудутся ваши мечты!";
  static const char textLine_2[] PROGMEM = "До {C#00D0FF}Нового года {C#FFFFFF}осталось {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59:59}{E21}";
  static const char textLine_3[] PROGMEM = "До {C#0019FF}Нового года{C#FFFFFF} {P01.01.****#4}";
  static const char textLine_4[] PROGMEM = "С {C#00D0FF}Новым {C#0BFF00}{D:yyyy} {C#FFFFFF}годом!{S01.01.****#31.01.**** 23:59:59}{E21}";
  static const char textLine_5[] PROGMEM = "В {C#10FF00}Красноярске {C#FFFFFF}{WS} {WT}";
  static const char textLine_6[] PROGMEM = "Show must go on!{C#000002}";
  static const char textLine_7[] PROGMEM = "{C#FF000F}Крибле! {C#000001}Крабле!! {C#00FF00}Бумс!!!{E24}";
  static const char textLine_8[] PROGMEM = "Крепитесь, люди - скоро {C#FF0300}лето!{S01.01.****#10.04.****}";
  static const char textLine_9[] PROGMEM = "Это {C#0081FF}\"ж-ж-ж\"{C#FFFFFF} - неспроста!";
  static const char textLine_A[] PROGMEM = "{C#000001}Раз! Два!! Три!!! {C#33C309}Ёлочка,{B#000000}{C#000001} гори!!!{C#FFFFFF}";
  static const char textLine_B[] PROGMEM = "-Дело было вечером, делать было нечего...";
  static const char textLine_C[] PROGMEM = "С наступающим Новым {C#00C911}{D:yyy+}{C#FFFFFF} годом!{S01.12.****#31.12.****}";
  static const char textLine_D[] PROGMEM = "-Это нужно обдумать...";
  static const char textLine_E[] PROGMEM = "-В этом что-то есть...";
  static const char textLine_F[] PROGMEM = "В {C#10FF00}Красноярске {C#FFFFFF}{WS} {WT}";
  static const char textLine_G[] PROGMEM = "Вот оно что, {C#FF0000}Михалыч{C#FFFFFF}!..";
  static const char textLine_H[] PROGMEM = "Сегодня {D:d MMMM yyyy} года, {D:dddd}";
  static const char textLine_I[] PROGMEM = "-Лень - двигатель прогресса";
  static const char textLine_J[] PROGMEM = "-Чем бы дитя не тешилось...";
  static const char textLine_K[] PROGMEM = "Счастливого Рождества!{S07.01.****}";
  static const char textLine_L[] PROGMEM = "С днем Победы!{S09.05.**** 7:00#09.05.**** 21:30}";
  static const char textLine_M[] PROGMEM = "Сегодня {D:dddd dd MMMM}, на улице {WS}, {WT}";
  static const char textLine_N[] PROGMEM = "С праздником, соседи!";
  static const char textLine_O[] PROGMEM = "-Не скучайте!";
  static const char textLine_P[] PROGMEM = "Счастья всем! И пусть никто не уйдет обиженным.";
  static const char textLine_Q[] PROGMEM = "В отсутствии солнца сияйте сами!";
  static const char textLine_R[] PROGMEM = "Раз! {C#000001}Два!! {C#FFFFFF}Три!!! {C#018C0A}Ёлочка,{C#FFFFFF} гори!!!{S01.01.****#19.01.****}{S01.12.****#31.12.****}";
  static const char textLine_S[] PROGMEM = "-Время принимать решения!";
  static const char textLine_T[] PROGMEM = "-Время делать выводы!";
  static const char textLine_U[] PROGMEM = "Лето, ах лето!{S15.05.****#01.09.****}";
  static const char textLine_V[] PROGMEM = "-Нет предела совершенству!";
  static const char textLine_W[] PROGMEM = "До {C#0080B0}Нового года{C#FFFFFF} осталось... понять как жить дальше...{S01.12.****#31.12.**** 23:59:59}";
  static const char textLine_X[] PROGMEM = "-Курочка по зёрнышку, копеечка к копеечке!";
  static const char textLine_Y[] PROGMEM = "Подъем через {P7:30#Z#60#60#12345}!";
  static const char textLine_Z[] PROGMEM = "-Доброе утро!";
  #endif
  
  // Строки результатов выполнения операций и некоторых других сообщений, передаваемых
  // из прошивки в Web-приложение для отображения
  
  static const char MSG_FILE_DELETED[] PROGMEM       = "Файл удален";
  static const char MSG_FILE_DELETE_ERROR[] PROGMEM  = "Ошибка удаления файла";
  static const char MSG_FILE_SAVED[] PROGMEM         = "Файл сохранен";
  static const char MSG_FILE_SAVE_ERROR[] PROGMEM    = "Ошибка записи в файл";
  static const char MSG_FILE_CREATE_ERROR[] PROGMEM  = "Ошибка создания файла";
  static const char MSG_FOLDER_CREATE_ERROR[] PROGMEM = "Ошибка создания папки хранилища";
  static const char MSG_FILE_LOADED[] PROGMEM        = "Файл загружен";
  static const char MSG_FILE_LOAD_ERROR[] PROGMEM    = "Ошибка чтения файла";
  static const char MSG_FILE_NOT_FOUND[] PROGMEM     = "Файл не найден";
  static const char MSG_FOLDER_NOT_FOUND[] PROGMEM   = "Папка не найдена";
  static const char MSG_BACKUP_SAVE_ERROR[] PROGMEM  = "Не удалось сохранить резервную копию настроек";
  static const char MSG_BACKUP_SAVE_OK[] PROGMEM     = "Резервная копия настроек создана";
  static const char MSG_BACKUP_LOAD_ERROR[] PROGMEM  = "Не удалось загрузить резервную копию настроек";
  static const char MSG_BACKUP_LOAD_OK[] PROGMEM     = "Настройки из резервной копии восстановлены";
  static const char MSG_OP_SUCCESS[] PROGMEM         = "Настройки сохранены";
  static const char MSG_TEXT_EXPORT_ERROR[] PROGMEM  = "Не удалось экспортировать строки";
  static const char MSG_TEXT_EXPORT_OK[] PROGMEM     = "Экспорт строк выполнен";
  static const char MSG_TEXT_IMPORT_ERROR[] PROGMEM  = "Не удалось импортировать строки";
  static const char MSG_TEXT_IMPORT_OK[] PROGMEM     = "Импорт строк выполнен";

  static const char MODE_NIGHT_CLOCK[] PROGMEM       = "Ночные часы";
  static const char MODE_CLOCK[] PROGMEM             = "Часы";
  static const char MODE_RUNNING_TEXT[] PROGMEM      = "Бегущая строка";
  static const char MODE_LOAD_PICTURE[] PROGMEM      = "Загрузка изображения";
  static const char MODE_DRAW[] PROGMEM              = "Рисование";
  static const char MODE_DAWN[] PROGMEM              = "Рассвет";

  static const char WTR_LANG_YA[]                    = "ru";      // Яндекс.Погода - кажется понимает только "ru" и "en"
  static const char WTR_LANG_OWM[]                   = "ru";      // OpenWeatherMap = 2-[ бкувенный код языка - ru,en,de,fr,it и так далее. Если язык не знает - возвращает как для en
  
#endif

// ============================================== ENG ============================================== 

#if (LANG == 'ENG')
  #define UI F("ENG")

  // List of effects names and order in sequence trensferring to Web-application. 
  // Order of Names MUST correspont to order of effect ID definitions declared in a_def_soft.h file
  // starting from line 89
  
  static const char EFFECT_LIST[] PROGMEM =
    "Clock,Lamp,Showfall,Cube,Rainbow,Paintball,Fire,The Matrix,Worms,Starfall,Confetti,"
    "Color noise,Clouds,Lava,Plasma,Iridescent,Peacock,Zebra,Noisy forest,Sea surf,Color change,"
    "Fireflies,Whirlpool,Cyclone,Flicker,Northern lights,Shadows,Maze,Snake,Tetris,Arkanoid,"
    "Palette,Spectrum,Sinuses,Embroidery,Rain,Fireplace,Arrows,Patterns,Rubic,Stars,"
    "Curtain,Traffic,Dawn,Stream,Fireworks,Stripes"
    
  #if (USE_ANIMATION == 1)
    ",Animation,Weather,Slides"
  #else  
    ",,,"
  #endif

  #if (USE_SD == 1)
    ",SD-card"
  #else  
    ","
  #endif
  ;         // <-- this ';' closes the operator static const char EFFECT_LIST[] PROGMEM =
  
  // ****************** SOUNDS OF ALARMS, DAWN and RUNNING TEXT MACRO {A} ********************
  
  #if (USE_MP3 == 1)
  // SD card in MP3 player (DFPlayer) contains under root folder three folders with names - "1","2" and "3"
  // Folder "1" contains MP3 sound files, playing on event of alarm
  // Folder "2" contains MP3 sound files, playing on event of dawn
  // Folder "3" contains MP3 sound files, playing on event ow shoe running text containing {A} macros
  // DFPlayer does not have an ability to retrieve names of files in folders, only can read the count of files in each folder.
  // Command for play sound means - play file with index (number) N from folder M
  // Indecies of files are defined by file allocation table (FAT) of SD-card, which defining at time of copy files into SD-card folder on clear (just formatted) SD-card
  // So, file was written into folder first of all gets index 1, next - 2 and so on and do not depends on names of files
  // These definitions contain names of sounds displayed in comboboxes inside application in order that must match to indecies of files were written into folders.
  
  // To form the correct order of sound files, rename them on your computer in a temporary folder so that they
  // formed the desired order, for example - just give them numeric names like 001.mp3, 002.mp3 and so on or
  // set a numeric prefix to an existing name, for example 01_birds.mpr, 02_thunder.mp3, and so on
  // The files in the folder should be sorted in the order of names.
  // Next, create a folder on a clean microSD card where the sound files will be placed and copy them to
  // an ordered list.

  // List of sounds for "Alarm sound" combobox in phone application or Web-interface
  static const char ALARM_SOUND_LIST[] PROGMEM =  
    "One Step Over,In the Death Car,Horn call,Lighthouse,Mister Sandman,Сasket,Banana Phone,Carol of the Bells";
  
  // List of sounds for "Dawn sound" combobox in phone application or Web-interface
  static const char DAWN_SOUND_LIST[] PROGMEM =   
    "Birds,Thunderstorm,Surf,Rain,Creek,The Mantra,La Petite Fille De La Mer";
  
  // List of sounds for {A} macros of running text in phone application or Web-interface
  static const char NOTIFY_SOUND_LIST[] PROGMEM = 
    "Piece Of Cake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board,"
    "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal,"
    "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3";
  
  #endif


  // List of animation names. Animations are defined in file 'animation.ino'
  #define LANG_IMAGE_LIST_DEF
  static const char LANG_IMAGE_LIST[] PROGMEM = 
    "Heart,Mario,Weather";
  
  // List names of Patterns effect
  static const char LANG_PATTERNS_LIST[] PROGMEM = 
    "Zigzag,Notes,Rhomb,Heart,Fir tree,Cells,Smile,Zigzag-2,Streaks,Waves,Scales,Curtain,Wicker,Snowflake,Squares,Greece,Circles,Roll,"
    "Pattern 1,Pattern 2,Pattern 3,Pattern 4,Pattern 5,Pattern 6,Pattern 7,Pattern 8,Pattern 9,Pattern 10,Pattern 11,Pattern 12,Pattern 13,Pattern 14";

  // Weather condition from Yandex
  static const char Y_CODE_01[] PROGMEM = "Cloudy, light rain";                     // cloudy, light rain
  static const char Y_CODE_02[] PROGMEM = "Cloudy, light snow";                     // cloudy, light snow
  static const char Y_CODE_03[] PROGMEM = "Cloudy, wet snow";                       // cloudy, wet snow
  static const char Y_CODE_04[] PROGMEM = "Partly cloudy";                          // partly cloudy
  static const char Y_CODE_05[] PROGMEM = "Partly cloudy, rain";                    // partly cloudy, rain
  static const char Y_CODE_06[] PROGMEM = "Partly cloudy, show";                    // partly cloudy, show
  static const char Y_CODE_07[] PROGMEM = "Partly cloudy, wet snow";                // partly cloudy, wet snow
  static const char Y_CODE_08[] PROGMEM = "Snowstorm";                              // snowstorm
  static const char Y_CODE_09[] PROGMEM = "Fog";                                    // fog
  static const char Y_CODE_10[] PROGMEM = "Overcast clouds";                        // overcast
  static const char Y_CODE_11[] PROGMEM = "Overcast clouds, light rain";            // overcast, light rain 
  static const char Y_CODE_12[] PROGMEM = "Overcast clouds, light snow";            // overcast, light snow
  static const char Y_CODE_13[] PROGMEM = "Overcast clouds, wet snow";              // overcast, wet snow 
  static const char Y_CODE_14[] PROGMEM = "Overcast clouds, rain";                  // overcast, rain
  static const char Y_CODE_15[] PROGMEM = "Overcast clouds, wet snow";              // overcast, wet snow
  static const char Y_CODE_16[] PROGMEM = "Overcast clouds, show";                  // overcast, show
  static const char Y_CODE_17[] PROGMEM = "Overcast clouds, thunderstorm withrain"; // overcast, thunderstorm withrain
  static const char Y_CODE_18[] PROGMEM = "Clear sky";                              // clear

  // Weather condition from OpenWeatherMap
  static const char W_CODE_200[] PROGMEM = "Thunderstorm with light rain";          // thunderstorm with light rain
  static const char W_CODE_201[] PROGMEM = "Thunderstorm with rain";                // thunderstorm with rain
  static const char W_CODE_202[] PROGMEM = "Thunderstorm with heavy rain";          // thunderstorm with heavy rain
  static const char W_CODE_210[] PROGMEM = "Light thunderstorm";                    // light thunderstorm
  static const char W_CODE_211[] PROGMEM = "Thunderstorm";                          // thunderstorm
  static const char W_CODE_212[] PROGMEM = "Heavy thunderstorm";                    // heavy thunderstorm
  static const char W_CODE_221[] PROGMEM = "Ragged thunderstorm";                   // ragged thunderstorm
  static const char W_CODE_230[] PROGMEM = "Thunderstorm with light drizzle";       // thunderstorm with light drizzle
  static const char W_CODE_231[] PROGMEM = "Thunderstorm with drizzle";             // thunderstorm with drizzle
  static const char W_CODE_232[] PROGMEM = "Thunderstorm with heavy drizzle";       // thunderstorm with heavy drizzle
  static const char W_CODE_300[] PROGMEM = "Light intensity drizzle";               // light intensity drizzle
  static const char W_CODE_301[] PROGMEM = "Drizzle";                               // drizzle
  static const char W_CODE_302[] PROGMEM = "Heavy intensity drizzle";               // heavy intensity drizzle
  static const char W_CODE_310[] PROGMEM = "Light intensity drizzle rain";          // light intensity drizzle rain
  static const char W_CODE_311[] PROGMEM = "Drizzle rain";                          // drizzle rain
  static const char W_CODE_312[] PROGMEM = "Heavy intensity drizzle rain";          // heavy intensity drizzle rain
  static const char W_CODE_313[] PROGMEM = "Shower rain and drizzle";               // shower rain and drizzle
  static const char W_CODE_314[] PROGMEM = "Heavy shower rain and drizzle";         // heavy shower rain and drizzle
  static const char W_CODE_321[] PROGMEM = "Shower drizzle";                        // shower drizzle  
  static const char W_CODE_500[] PROGMEM = "Light rain";                            // light rain
  static const char W_CODE_501[] PROGMEM = "Moderate rain";                         // moderate rain
  static const char W_CODE_502[] PROGMEM = "Heavy intensity rain";                  // heavy intensity rain
  static const char W_CODE_503[] PROGMEM = "Very heavy rain";                       // very heavy rain
  static const char W_CODE_504[] PROGMEM = "Extreme rain";                          // extreme rain
  static const char W_CODE_511[] PROGMEM = "Freezing rain";                         // freezing rain
  static const char W_CODE_520[] PROGMEM = "Light intensity shower rain";           // light intensity shower rain
  static const char W_CODE_521[] PROGMEM = "Shower rain";                           // shower rain
  static const char W_CODE_522[] PROGMEM = "Heavy intensity shower rain";           // heavy intensity shower rain
  static const char W_CODE_531[] PROGMEM = "Ragged shower rain";                    // ragged shower rain
  static const char W_CODE_600[] PROGMEM = "Light snow";                            // light snow
  static const char W_CODE_601[] PROGMEM = "Snow";                                  // Snow
  static const char W_CODE_602[] PROGMEM = "Heavy snow";                            // Heavy snow
  static const char W_CODE_611[] PROGMEM = "Sleet";                                 // Sleet
  static const char W_CODE_612[] PROGMEM = "Light shower sleet";                    // Light shower sleet
  static const char W_CODE_613[] PROGMEM = "Shower sleet";                          // Shower sleet
  static const char W_CODE_615[] PROGMEM = "Light rain and snow";                   // Light rain and snow
  static const char W_CODE_616[] PROGMEM = "Rain and snow";                         // Rain and snow
  static const char W_CODE_620[] PROGMEM = "Light shower snow";                     // Light shower snow
  static const char W_CODE_621[] PROGMEM = "Shower snow";                           // Shower snow
  static const char W_CODE_622[] PROGMEM = "Heavy shower snow";                     // Heavy shower snow
  static const char W_CODE_701[] PROGMEM = "mist";                                  // mist
  static const char W_CODE_711[] PROGMEM = "Smoke";                                 // Smoke
  static const char W_CODE_721[] PROGMEM = "Haze";                                  // Haze
  static const char W_CODE_731[] PROGMEM = "Sand / dust whirls";                    // sand/ dust whirls
  static const char W_CODE_741[] PROGMEM = "Fog";                                   // fog
  static const char W_CODE_751[] PROGMEM = "Sand";                                  // sand
  static const char W_CODE_761[] PROGMEM = "Dust";                                  // dust
  static const char W_CODE_762[] PROGMEM = "Volcanic ash";                          // volcanic ash
  static const char W_CODE_771[] PROGMEM = "Squalls";                               // squalls
  static const char W_CODE_781[] PROGMEM = "Tornado";                               // tornado
  static const char W_CODE_800[] PROGMEM = "Clear sky";                             // clear sky
  static const char W_CODE_801[] PROGMEM = "Few clouds: 11-25%";                    // few clouds: 11-25%
  static const char W_CODE_802[] PROGMEM = "Scattered clouds: 25-50%";              // scattered clouds: 25-50%
  static const char W_CODE_803[] PROGMEM = "Broken clouds: 51-84%";                 // broken clouds: 51-84%
  static const char W_CODE_804[] PROGMEM = "Overcast clouds: 85-100%";              // overcast clouds: 85-100%

  // Constants of running text data macroses

  static const char SMonth_01[] PROGMEM = "january";
  static const char SMonth_02[] PROGMEM = "february";
  static const char SMonth_03[] PROGMEM = "march";
  static const char SMonth_04[] PROGMEM = "april";
  static const char SMonth_05[] PROGMEM = "may";
  static const char SMonth_06[] PROGMEM = "june";
  static const char SMonth_07[] PROGMEM = "july";
  static const char SMonth_08[] PROGMEM = "august";
  static const char SMonth_09[] PROGMEM = "september";
  static const char SMonth_10[] PROGMEM = "october";
  static const char SMonth_11[] PROGMEM = "november";
  static const char SMonth_12[] PROGMEM = "december";

  static const char SMnth_01[] PROGMEM = "jan";
  static const char SMnth_02[] PROGMEM = "feb";
  static const char SMnth_03[] PROGMEM = "mar";
  static const char SMnth_04[] PROGMEM = "apr";
  static const char SMnth_05[] PROGMEM = "may";
  static const char SMnth_06[] PROGMEM = "jun";
  static const char SMnth_07[] PROGMEM = "jul";
  static const char SMnth_08[] PROGMEM = "aug";
  static const char SMnth_09[] PROGMEM = "sep";
  static const char SMnth_10[] PROGMEM = "oct";
  static const char SMnth_11[] PROGMEM = "nov";
  static const char SMnth_12[] PROGMEM = "dec";

  static const char SDayFull_1[] PROGMEM = "Monday";
  static const char SDayFull_2[] PROGMEM = "Tuesday";
  static const char SDayFull_3[] PROGMEM = "Wednesday";
  static const char SDayFull_4[] PROGMEM = "Thursday";
  static const char SDayFull_5[] PROGMEM = "Friday";
  static const char SDayFull_6[] PROGMEM = "Saturday";
  static const char SDayFull_7[] PROGMEM = "Sunday";

  static const char SDayShort_1[] PROGMEM = "Mon";
  static const char SDayShort_2[] PROGMEM = "Tue";
  static const char SDayShort_3[] PROGMEM = "Wed";
  static const char SDayShort_4[] PROGMEM = "Thu";
  static const char SDayShort_5[] PROGMEM = "Fri";
  static const char SDayShort_6[] PROGMEM = "Sat";
  static const char SDayShort_7[] PROGMEM = "Sun";
  
  static const char SDayShrt_1[] PROGMEM = "Mo";
  static const char SDayShrt_2[] PROGMEM = "Tu";
  static const char SDayShrt_3[] PROGMEM = "We";
  static const char SDayShrt_4[] PROGMEM = "Th";
  static const char SDayShrt_5[] PROGMEM = "Fr";
  static const char SDayShrt_6[] PROGMEM = "Sa";
  static const char SDayShrt_7[] PROGMEM = "Su";

  static const char SDayForm_1[] PROGMEM = " days";          // Start space is mandatory
  static const char SDayForm_2[] PROGMEM = " day";
  static const char SDayForm_3[] PROGMEM = " days";
 
  static const char SHourForm_1[] PROGMEM = " hour";         // Start space is mandatory
  static const char SHourForm_2[] PROGMEM = " hours";
  static const char SHourForm_3[] PROGMEM = " hours";

  static const char SMinuteForm_1[] PROGMEM = " minutes";    // Start space is mandatory
  static const char SMinuteForm_2[] PROGMEM = " minute";
  static const char SMinuteForm_3[] PROGMEM = " minutes";

  static const char SSecondForm_1[] PROGMEM = " seconds";    // Start space is mandatory
  static const char SSecondForm_2[] PROGMEM = " second";
  static const char SSecondForm_3[] PROGMEM = " seconds";

  // Examples of lines for the "Running Line" that will be filled with these values when the INITIALIZE_TEXTS == 1 flag is set in a_def_hard.h (in line 62)
  // at the initialization step when the firmware is first started on the microcontroller.
  // These examples contain some options for using macros in a running line.

  #if (INITIALIZE_TEXTS == 1)
  static const char textLine_0[] PROGMEM = "##";
  static const char textLine_1[]         = "";
  static const char textLine_2[] PROGMEM = "There are {C#10FF00}{R01.01.***+} {C#FFFFFF} left until the {C#00D0FF}New year!{S01.12.****#31.12.**** 23:59:59}{E21}";
  static const char textLine_3[] PROGMEM = "{C#0019FF}New year{C#FFFFFF} in {P01.01.****#4} {C#0019FF}New Year's Eve{C#FFFFFF} ";
  static const char textLine_4[] PROGMEM = "Happy {C#00D0FF}New {C#0BFF00}{D:yyyy} {C#FFFFFF}year!{S01.01.****#31.01.**** 23:59:59}{E21}";
  static const char textLine_5[]         = "";
  static const char textLine_6[] PROGMEM = "Show must go on!{C#000002}";
  static const char textLine_7[] PROGMEM = "{C#FF000F}Ready! {C#000001}Steady!! {C#00FF00}Go!!!{E24}";
  static const char textLine_8[] PROGMEM = "{C#0003FF}Winter {C#FFFFFF}is coming...!{S01.10.****#01.12.****}";
  static const char textLine_9[]         = "";
  static const char textLine_A[]         = "";
  static const char textLine_B[]         = "";
  static const char textLine_C[]         = "";
  static const char textLine_D[]         = "";
  static const char textLine_E[]         = "";
  static const char textLine_F[] PROGMEM = "Weather in {C#10FF00}Krasnoyarsk: {C#FFFFFF}{WS} {WT}";
  static const char textLine_G[]         = "";
  static const char textLine_H[] PROGMEM = "Today {D:dd MMMM yyyy} year, {D:dddd}";
  static const char textLine_I[]         = "";
  static const char textLine_J[]         = "";
  static const char textLine_K[] PROGMEM = "Happy Cristmas!{S25.12.****}";
  static const char textLine_L[] PROGMEM = "Victory day!{S09.05.**** 7:00#09.05.**** 21:30}";
  static const char textLine_M[] PROGMEM = "Today {D:dddd dd MMMM}, outdoor {WS}, {WT}{E25}";
  static const char textLine_N[]         = "";
  static const char textLine_O[]         = "";
  static const char textLine_P[]         = "";
  static const char textLine_Q[]         = "";
  static const char textLine_R[]         = "";
  static const char textLine_S[]         = "";
  static const char textLine_T[]         = "";
  static const char textLine_U[] PROGMEM = "Summer is the best time!{S15.05.****#01.09.****}";
  static const char textLine_V[]         = "";
  static const char textLine_W[]         = "";
  static const char textLine_X[]         = "";
  static const char textLine_Y[] PROGMEM = "Wake up in {P7:30#Z#60#60#12345}!";
  static const char textLine_Z[] PROGMEM = "-Good morning!";
  #endif
  
  // Lines of results of operations and some other messages transmitted
  // from the firmware to the Web application to display

  static const char MSG_FILE_DELETED[] PROGMEM       = "File deleted";
  static const char MSG_FILE_DELETE_ERROR[] PROGMEM  = "File delete error";
  static const char MSG_FILE_SAVED[] PROGMEM         = "File sucessfully saved";
  static const char MSG_FILE_SAVE_ERROR[] PROGMEM    = "File write error";
  static const char MSG_FILE_CREATE_ERROR[] PROGMEM  = "File create error";
  static const char MSG_FOLDER_CREATE_ERROR[] PROGMEM = "Unable to create folder for storage";
  static const char MSG_FILE_LOADED[] PROGMEM        = "File loaded";
  static const char MSG_FILE_LOAD_ERROR[] PROGMEM    = "File read error";
  static const char MSG_FILE_NOT_FOUND[] PROGMEM     = "File not found";
  static const char MSG_FOLDER_NOT_FOUND[] PROGMEM   = "Folder storage not found";
  static const char MSG_BACKUP_SAVE_ERROR[] PROGMEM  = "Error while create backup file";
  static const char MSG_BACKUP_SAVE_OK[] PROGMEM     = "Backup file sucessfully saved";
  static const char MSG_BACKUP_LOAD_ERROR[] PROGMEM  = "Unable to load backup file";
  static const char MSG_BACKUP_LOAD_OK[] PROGMEM     = "Settings were sucessfully loaded";
  static const char MSG_OP_SUCCESS[] PROGMEM         = "Sucessfully applied";
  static const char MSG_TEXT_EXPORT_ERROR[] PROGMEM  = "Texts export error";
  static const char MSG_TEXT_EXPORT_OK[] PROGMEM     = "Texts exported";
  static const char MSG_TEXT_IMPORT_ERROR[] PROGMEM  = "Texts import error";
  static const char MSG_TEXT_IMPORT_OK[] PROGMEM     = "Texts imported";

  static const char MODE_NIGHT_CLOCK[] PROGMEM       = "Night clock";
  static const char MODE_CLOCK[] PROGMEM             = "Clock";
  static const char MODE_RUNNING_TEXT[] PROGMEM      = "Running text";
  static const char MODE_LOAD_PICTURE[] PROGMEM      = "Load picture";
  static const char MODE_DRAW[] PROGMEM              = "Draw";
  static const char MODE_DAWN[] PROGMEM              = "Dawn";

  static const char WTR_LANG_YA[]                    = "en";      // Yandex.Weather seems know only "ru" and "en"
  static const char WTR_LANG_OWM[]                   = "en";      // OpenWeatherMap - 2-letter language code "ru", "en", "de", "fr", "it" and so on. For unknown code returns same as "en"

#endif

// ============================================== ESP ============================================== 

#if (LANG == 'ESP')
  #define UI F("ESP")
  
  // Lista y orden de los efectos, transmitido al interfaz web. 
  // El orden de los nombres de los efectos en la lista debe coincidir con la lista de los efectos determinada en el archivo a_def_soft.h 
  // en las lineas desde 89 
  
  static const char EFFECT_LIST[] PROGMEM =
    "Reloj,Lámpara,Nevada,Cubo,Arco iris,Paintball,Fuego,The Matrix,Gusanos,Lluvia de estrellas,Confeti,"
    "Ruido de color,Nubes,Lava,Plasma,Arco iris,Pavo real,Cebra,Bosque ruidoso,Surf,Cambio de color,"
    "Luciérnagas,Remolino,Ciclón,Parpadeo,Luces del Norte,Sombras,Laberinto,Serpiente,Tetris,Arkanoid,"
    "Paleta,Espectro,Senos,Vyshyvanka,Lluvia,Chimenea,Flechas,Patrones,Rubik,Estrellas,Cortina,Tráfico,"
    "Amanecer,Corriente,Fuegos art,Rayas"
  
  #if (USE_ANIMATION == 1)
    ",Animación,Clima,Diapositivas"
  #else
    ",,,"
  #endif

  #if (USE_SD == 1)
    ",Tarjeta SD"
  #else  
    ","
  #endif
  ;         // <-- esto ';' cierra el operador static const char EFFECT_LIST[] PROGMEM =

  // ****************** DEFINICION DE LOS PARAMETROS DEL DESPERTADOR ********************
  
  #if (USE_MP3 == 1)
  // Tarjeta SD en el reproductor de MP3 (DFPlayer) contiene en su raiz tres carpetas - "01","02" и "03"
  // Carpeta "01" contiene los archivos MP3 de los sonidos que estan sonando cuando ocurre el evento de despertador
  // Carpeta "02" contiene los archivos MP3 de los sonidos que estan sonando cuando ocurre el evento de ocaso
  // Carpeta "03" contiene los archivos MP3 de los sonidos que estan sonando en el macro {A} de letrero de desplazamiento
  // DFPlayer no tiene posibilidad de leer los nombres de los archivos, solo se puede recibir la cantidad de los archivos en la carpeta.
  // El comando de reproducir el sonido significa - reproducir el archivo con index (numero) N desde la caprpeta M
  // Los numeros de los archivos estan definidos por la tabla de colocacion de los archivos de tarjeta SD (FAT) y se forman en el orden de inscripcion de los archivos en la tarjeta vacia
  // Asi, el archivo colocado el primero recibe el numero 1, el segundo ' 2, etc. y eso de ningun manera depende de los nombres propios de los archivos.
  // Estos matrizes contienen los nombres de los sonidos que se muestran en la aplicacion en el orden que coincide con la numeracion de los archivos en la carpeta.
  //
  // Para formar el orden correcto de los archivos de los sonidos cambia sus nombres en el ordenador en la carpeta temporal asi que ellos
  // se mantengan en el orden necesario, por ejemplo - dales simples nombres numericos  001.mp3, 002.mp3 etc. o 
  // dales un prefijo numerico a su nombre actual, por ejemplo 01_birds.mpr, 02_thunder.mp3 etc.
  // Los archivos en la carpeta deben ser ordenados en el orden de sus nombres.
  // Ahora hay que crear en la tarjeta SD vacia una carpeta para meter alli los archivos de sonidos, y hay que copiarlos alli 
  // como una lista ordenada.
  
  // Lista de los sonidos para la caja combo "El Sonido del Despertador" en la aplicacion en el movil
  static const char ALARM_SOUND_LIST[] PROGMEM = 
    "One Step Over,In the Death Car,La Trompeta Esta Llamando,Farola,Mister Sandman,Ataud,Banana Phone,Carol of the Bells";
  
  // Lista de los sonidos para la caja combo "El Sonido del Ocaso" en la aplicacion en el movil
  static const char DAWN_SOUND_LIST[] PROGMEM = 
    "Aves,Tormenta,Surf,Lluvia,Arroyo,Mantra,La Petite Fille De La Mer";
  
  // Lista de los sonidos `para el macro {A} de letrero de desplazamiento
  static const char NOTIFY_SOUND_LIST[] PROGMEM = 
    "Piece Of Сake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board,"
    "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal,"
    "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3";
  
  #endif

  // Lista de los nombres de animaciones. Las animaciones estan definidos en el archivo 'animation.ino'
  #define LANG_IMAGE_LIST_DEF
  static const char LANG_IMAGE_LIST[] PROGMEM = 
    "Corazon,Mario,Tiempo";

  // Lista de nombres de los patrones
  static const char LANG_PATTERNS_LIS[] PROGMEM = 
    "Zigzag,Notas musicales,Rombo,Corazón,Arbol de Navidad,Celda,Carita sonriente,Zigzag,Rayas,Ondas,Escamas,Cortinas,Trenza,Copo de nieve,Cuadrados,Grecia,Círculos,Rollo," 
    "Patron 1,Patron 2,Patron 3,Patron 4,Patron 5,Patron 6,Patron 7,Patron 8,Patron 9,Patron 10,Patron 11,Patron 12,Patron 13,Patron 14";

  // Condiciones del tiempo por Yandex
  static const char Y_CODE_01[] PROGMEM = "parcialmente nublado, lluvia ligera";             // cloudy, light rain
  static const char Y_CODE_02[] PROGMEM = "parcialmente nublado, nieve ligera";              // cloudy, light snow
  static const char Y_CODE_03[] PROGMEM = "parcialmente nublado, nieve ligera con lluvia";   // cloudy, wet snow
  static const char Y_CODE_04[] PROGMEM = "parcialmente nublado";                            // partly cloudy
  static const char Y_CODE_05[] PROGMEM = "parcialmente nublado, lluvia";                    // partly cloudy, rain
  static const char Y_CODE_06[] PROGMEM = "parcialmente nublado, nieve";                     // partly cloudy, show
  static const char Y_CODE_07[] PROGMEM = "parcialmente nublado, chubascos de nieve";        // partly cloudy, wet snow
  static const char Y_CODE_08[] PROGMEM = "tormenta de nieve";                               // snowstorm
  static const char Y_CODE_09[] PROGMEM = "niebla";                                          // fog
  static const char Y_CODE_10[] PROGMEM = "Principalmente nublado";                          // overcast
  static const char Y_CODE_11[] PROGMEM = "nublado, a veces lluvia";                         // overcast, light rain 
  static const char Y_CODE_12[] PROGMEM = "nublado, a veces nieve";                          // overcast, light snow
  static const char Y_CODE_13[] PROGMEM = "nublado con nieve y lluvia ocasionales";          // overcast, wet snow 
  static const char Y_CODE_14[] PROGMEM = "nublado, lluvia";                                 // overcast, rain
  static const char Y_CODE_15[] PROGMEM = "nublado, nieve y lluvia";                         // overcast, wet snow
  static const char Y_CODE_16[] PROGMEM = "nublado, nieve";                                  // overcast, show
  static const char Y_CODE_17[] PROGMEM = "nublado, lluvia con truenos";                     // overcast, thunderstorm withrain
  static const char Y_CODE_18[] PROGMEM = "despejado";                                       // clear

  // Condiciones del tiempo por OpenWeatherMap
  static const char W_CODE_200[] PROGMEM = "Tormenta, lluvia ligera";                        // thunderstorm with light rain
  static const char W_CODE_201[] PROGMEM = "Lluvia con truenos";                             // thunderstorm with rain
  static const char W_CODE_202[] PROGMEM = "Tormenta, chubascos";                            // thunderstorm with heavy rain
  static const char W_CODE_210[] PROGMEM = "pequeña tormenta";                               // light thunderstorm
  static const char W_CODE_211[] PROGMEM = "Tormenta";                                       // thunderstorm
  static const char W_CODE_212[] PROGMEM = "Tormenta fuerte";                                // heavy thunderstorm
  static const char W_CODE_221[] PROGMEM = "tormentas eléctricas intermitentes";             // ragged thunderstorm
  static const char W_CODE_230[] PROGMEM = "Tormenta, lluvia ligera";                        // thunderstorm with light drizzle
  static const char W_CODE_231[] PROGMEM = "Tormenta con lluvia";                            // thunderstorm with drizzle
  static const char W_CODE_232[] PROGMEM = "Tormenta con fuertes lluvias";                   // thunderstorm with heavy drizzle
  static const char W_CODE_300[] PROGMEM = "lluvia fina";                                    // light intensity drizzle
  static const char W_CODE_301[] PROGMEM = "lluvia torrencial";                              // drizzle
  static const char W_CODE_302[] PROGMEM = "Lluvia Pesada";                                  // heavy intensity drizzle
  static const char W_CODE_310[] PROGMEM = "Lluvia pequeña";                                 // light intensity drizzle rain
  static const char W_CODE_311[] PROGMEM = "lluvia torrencial";                              // drizzle rain
  static const char W_CODE_312[] PROGMEM = "Lluvia Pesada";                                  // heavy intensity drizzle rain
  static const char W_CODE_313[] PROGMEM = "Aguacero, lluvia y llovizna";                    // shower rain and drizzle
  static const char W_CODE_314[] PROGMEM = "Fuerte aguacero, lluvia y llovizna";             // heavy shower rain and drizzle
  static const char W_CODE_321[] PROGMEM = "lluvia torrencial";                              // shower drizzle  
  static const char W_CODE_500[] PROGMEM = "Lluvia pequeña";                                 // light rain
  static const char W_CODE_501[] PROGMEM = "lluvia moderada";                                // moderate rain
  static const char W_CODE_502[] PROGMEM = "Ducha";                                          // heavy intensity rain
  static const char W_CODE_503[] PROGMEM = "Lluvia torrencial";                              // very heavy rain
  static const char W_CODE_504[] PROGMEM = "Lluvia torrencial";                              // extreme rain
  static const char W_CODE_511[] PROGMEM = "Granizo";                                        // freezing rain
  static const char W_CODE_520[] PROGMEM = "Lluvia pequeña";                                 // light intensity shower rain
  static const char W_CODE_521[] PROGMEM = "lluvia torrencial";                              // shower rain
  static const char W_CODE_522[] PROGMEM = "Lluvia Pesada";                                  // heavy intensity shower rain
  static const char W_CODE_531[] PROGMEM = "Lluvia a veces";                                 // ragged shower rain
  static const char W_CODE_600[] PROGMEM = "nieve pequeña";                                  // light snow
  static const char W_CODE_601[] PROGMEM = "Nieve";                                          // Snow
  static const char W_CODE_602[] PROGMEM = "Nevada";                                         // Heavy snow
  static const char W_CODE_611[] PROGMEM = "Fango";                                          // Sleet
  static const char W_CODE_612[] PROGMEM = "nieve ligera";                                   // Light shower sleet
  static const char W_CODE_613[] PROGMEM = "aguacero, nieve";                                // Shower sleet
  static const char W_CODE_615[] PROGMEM = "Nieve humeda";                                   // Light rain and snow
  static const char W_CODE_616[] PROGMEM = "lluvia con nieve";                               // Rain and snow
  static const char W_CODE_620[] PROGMEM = "nevada ligera";                                  // Light shower snow
  static const char W_CODE_621[] PROGMEM = "nevada, ventisca";                               // Shower snow
  static const char W_CODE_622[] PROGMEM = "fuerte nevada";                                  // Heavy shower snow
  static const char W_CODE_701[] PROGMEM = "Niebla";                                         // mist
  static const char W_CODE_711[] PROGMEM = "Bruma";                                          // Smoke
  static const char W_CODE_721[] PROGMEM = "Niebla ligera";                                  // Haze
  static const char W_CODE_731[] PROGMEM = "remolinos de polvo";                             // sand/ dust whirls
  static const char W_CODE_741[] PROGMEM = "Bruma";                                          // fog
  static const char W_CODE_751[] PROGMEM = "torbellinos de arena";                           // sand
  static const char W_CODE_761[] PROGMEM = "Remolinos de polvo";                             // dust
  static const char W_CODE_762[] PROGMEM = "ceniza volcánica";                               // volcanic ash
  static const char W_CODE_771[] PROGMEM = "viento chubasco";                                // squalls
  static const char W_CODE_781[] PROGMEM = "Tornado";                                        // tornado
  static const char W_CODE_800[] PROGMEM = "Despejado";                                      // clear sky
  static const char W_CODE_801[] PROGMEM = "Parcialmente nublado";                           // few clouds: 11-25%
  static const char W_CODE_802[] PROGMEM = "Parcialmente nublado";                           // scattered clouds: 25-50%
  static const char W_CODE_803[] PROGMEM = "Nublado con claros";                             // broken clouds: 51-84%
  static const char W_CODE_804[] PROGMEM = "Principalmente nublado";                         // overcast clouds: 85-100%

  // Константы вывода даты в бегущей строке

  static const char SMonth_01[] PROGMEM = "enero";
  static const char SMonth_02[] PROGMEM = "febrero";
  static const char SMonth_03[] PROGMEM = "marzo";
  static const char SMonth_04[] PROGMEM = "abril";
  static const char SMonth_05[] PROGMEM = "mayo";
  static const char SMonth_06[] PROGMEM = "junio";
  static const char SMonth_07[] PROGMEM = "julio";
  static const char SMonth_08[] PROGMEM = "agosto";
  static const char SMonth_09[] PROGMEM = "septiembre";
  static const char SMonth_10[] PROGMEM = "octubre";
  static const char SMonth_11[] PROGMEM = "noviembre";
  static const char SMonth_12[] PROGMEM = "diciembre";

  static const char SMnth_01[] PROGMEM = "ene";
  static const char SMnth_02[] PROGMEM = "feb";
  static const char SMnth_03[] PROGMEM = "mar";
  static const char SMnth_04[] PROGMEM = "abr";
  static const char SMnth_05[] PROGMEM = "may";
  static const char SMnth_06[] PROGMEM = "jun";
  static const char SMnth_07[] PROGMEM = "jul";
  static const char SMnth_08[] PROGMEM = "ago";
  static const char SMnth_09[] PROGMEM = "sep";
  static const char SMnth_10[] PROGMEM = "oct";
  static const char SMnth_11[] PROGMEM = "nov";
  static const char SMnth_12[] PROGMEM = "dic";

  static const char SDayFull_1[] PROGMEM = "lunes";
  static const char SDayFull_2[] PROGMEM = "martes";
  static const char SDayFull_3[] PROGMEM = "miercoles";
  static const char SDayFull_4[] PROGMEM = "jueves";
  static const char SDayFull_5[] PROGMEM = "viernes";
  static const char SDayFull_6[] PROGMEM = "sabado";
  static const char SDayFull_7[] PROGMEM = "domingo";

  static const char SDayShort_1[] PROGMEM = "lun";
  static const char SDayShort_2[] PROGMEM = "mart";
  static const char SDayShort_3[] PROGMEM = "mier";
  static const char SDayShort_4[] PROGMEM = "juev";
  static const char SDayShort_5[] PROGMEM = "vier";
  static const char SDayShort_6[] PROGMEM = "sab";
  static const char SDayShort_7[] PROGMEM = "dom";
  
  static const char SDayShrt_1[] PROGMEM = "L";
  static const char SDayShrt_2[] PROGMEM = "M";
  static const char SDayShrt_3[] PROGMEM = "X";
  static const char SDayShrt_4[] PROGMEM = "J";
  static const char SDayShrt_5[] PROGMEM = "V";
  static const char SDayShrt_6[] PROGMEM = "S";
  static const char SDayShrt_7[] PROGMEM = "D";


  static const char SDayForm_1[] PROGMEM = " dias";         // El espacio al principio es obligatorio
  static const char SDayForm_2[] PROGMEM = " dia";
  static const char SDayForm_3[] PROGMEM = " dia";
 
  static const char SHourForm_1[] PROGMEM = " hora";        // El espacio al principio es obligatorio
  static const char SHourForm_2[] PROGMEM = " hora";
  static const char SHourForm_3[] PROGMEM = " horas";

  static const char SMinuteForm_1[] PROGMEM = " minutos";   // El espacio al principio es obligatorio
  static const char SMinuteForm_2[] PROGMEM = " minuto";
  static const char SMinuteForm_3[] PROGMEM = " minuto";

  static const char SSecondForm_1[] PROGMEM = " segunda";   // El espacio al principio es obligatorio
  static const char SSecondForm_2[] PROGMEM = " segunda";
  static const char SSecondForm_3[] PROGMEM = " segunda";

  // Ejemplos de frases para "Letrero de desplazamiento", que van a ser rellenados con esos parametros si esta marcado INITIALIZE_TEXTS == 1 en a_def_hard.h (en linea 62)
  // en el paso de inicializacion al primer comienzo del firmware del microcontrolador.
  // Estos ejemplos contienes algunos variantes del uso de los macros en el letrero de desplazamiento. 
  
  #if (INITIALIZE_TEXTS == 1)
  static const char textLine_0[] PROGMEM = "##";
  static const char textLine_1[] PROGMEM = "¡Todo estará bien!";
  static const char textLine_2[] PROGMEM = "Hasta {C#00D0FF}La Navidad {C#FFFFFF}se queda {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59:59}{E21}";
  static const char textLine_3[] PROGMEM = "Hasta {C#0019FF}La Navidad{C#FFFFFF} {P01.01.****#4}";
  static const char textLine_4[] PROGMEM = "Feliz {C#00D0FF}Navidad {C#0BFF00}{D:yyyy} {C#FFFFFF}!!!{S01.01.****#31.01.**** 23:59:59}{E21}";
  static const char textLine_5[] PROGMEM = "En {C#10FF00}Madrid {C#FFFFFF}{WS} {WT}";
  static const char textLine_6[] PROGMEM = "¡Show must go on!{C#000002}";
  static const char textLine_7[] PROGMEM = "{C#FF000F}¡Habracadabra! {C#00FF00}¡¡¡Bum!!!{E24}";
  static const char textLine_8[] PROGMEM = "Prepárense, gente, se acerca {C#FF0300}el verano!{S01.01.****#10.04.****}";
  static const char textLine_9[] PROGMEM = "¿Que {C#0081FF}pasa{C#FFFFFF} Hombre?";
  static const char textLine_A[] PROGMEM = "{C#000001}¡Uno! ¡¡Dos!! Tres!!! {C#33C309}Tira,{B#000000}{C#000001} ¡¡¡Otra vez!!!{C#FFFFFF}";
  static const char textLine_B[] PROGMEM = "Esa linia no tiene nada...";
  static const char textLine_C[] PROGMEM = "¡Que bonito!";
  static const char textLine_D[] PROGMEM = "Hola, hola caracola...";
  static const char textLine_E[] PROGMEM = "Hay algo inteligente en esto....";
  static const char textLine_F[] PROGMEM = "En {C#10FF00}Madrid {C#FFFFFF}{WS} {WT}";
  static const char textLine_G[] PROGMEM = "Eso es todo, {C#FF0000}hombre{C#FFFFFF}!..";
  static const char textLine_H[] PROGMEM = "Hoy {D:dd MMMM yyyy}, dia {D:dddd}";
  static const char textLine_I[] PROGMEM = "Pareza - motor de progreso";
  static const char textLine_J[]         = "";
  static const char textLine_K[]         = "";
  static const char textLine_L[]         = "";
  static const char textLine_M[] PROGMEM = "Ahora {D:dddd dd MMMM}, temperatura {WS}, {WT}{E25}";
  static const char textLine_N[]         = "";
  static const char textLine_O[]         = "";
  static const char textLine_P[]         = "";
  static const char textLine_Q[]         = "";
  static const char textLine_R[] PROGMEM = "¿Porque? ¡Por que!";
  static const char textLine_S[]         = "";
  static const char textLine_T[]         = "";
  static const char textLine_U[]         = "";
  static const char textLine_V[]         = "";
  static const char textLine_W[]         = "";
  static const char textLine_X[]         = "";
  static const char textLine_Y[] PROGMEM = "Levantote desde{P7:30#Z#60#60#12345}!";
  static const char textLine_Z[] PROGMEM = "¡BUENOS DIAS!";
  #endif
  
  // Líneas de resultados de ejecución de operaciones y algunos otros mensajes transmitidos
  // del firmware a la aplicación web para mostrar
  
  static const char MSG_FILE_DELETED[] PROGMEM       = "Archivo eliminado";
  static const char MSG_FILE_DELETE_ERROR[] PROGMEM  = "Error de eliminación de archivos";
  static const char MSG_FILE_SAVED[] PROGMEM         = "Archivo guardado";
  static const char MSG_FILE_SAVE_ERROR[] PROGMEM    = "Error al escribir en el archivo";
  static const char MSG_FILE_CREATE_ERROR[] PROGMEM  = "Error de creación de archivo";
  static const char MSG_FOLDER_CREATE_ERROR[] PROGMEM = "Error al crear una carpeta de almacenamiento";
  static const char MSG_FILE_LOADED[] PROGMEM        = "Archivo descargado";
  static const char MSG_FILE_LOAD_ERROR[] PROGMEM    = "Error de lectura de archivo";
  static const char MSG_FILE_NOT_FOUND[] PROGMEM     = "Archivo no encontrado";
  static const char MSG_FOLDER_NOT_FOUND[] PROGMEM   = "Carpeta no encontrada";
  static const char MSG_BACKUP_SAVE_ERROR[] PROGMEM  = "No se pudo hacer una copia de seguridad de la configuración";
  static const char MSG_BACKUP_SAVE_OK[] PROGMEM     = "Configuraciones respaldadas creadas";
  static const char MSG_BACKUP_LOAD_ERROR[] PROGMEM  = "Error al cargar la copia de seguridad de la configuración";
  static const char MSG_BACKUP_LOAD_OK[] PROGMEM     = "Configuración de copia de seguridad restaurada";
  static const char MSG_OP_SUCCESS[] PROGMEM         = "Los ajustes se han guardado";
  static const char MSG_TEXT_EXPORT_ERROR[] PROGMEM  = "Error de exportación de textos";
  static const char MSG_TEXT_EXPORT_OK[] PROGMEM     = "Textos exportados";
  static const char MSG_TEXT_IMPORT_ERROR[] PROGMEM  = "Error de importación de textos";
  static const char MSG_TEXT_IMPORT_OK[] PROGMEM     = "Textos importados";

  static const char MODE_NIGHT_CLOCK[] PROGMEM       = "Reloj de la noche";
  static const char MODE_CLOCK[] PROGMEM             = "Reloj";
  static const char MODE_RUNNING_TEXT[] PROGMEM      = "Letras desplasamento";
  static const char MODE_LOAD_PICTURE[] PROGMEM      = "Carga de imagen";
  static const char MODE_DRAW[] PROGMEM              = "Dibujo";
  static const char MODE_DAWN[] PROGMEM              = "Amanecer";
  
  static const char WTR_LANG_YA[]                    = "es";      // Yandex.Weather: parece entender solo "ru" y "en"
  static const char WTR_LANG_OWM[]                   = "es";      // OpenWeatherMap = 2 - [código de idioma alfabético-ru,en,de,fr, it y así sucesivamente. Si el idioma no sabe - devuelve tanto para en
  
#endif

// ============================================== LAT ============================================== 

#if (LANG == 'LAT')
  #define UI F("LAT")
  // Web saskarnei nodoto efektu saraksts un secība. 
  // Efektu nosaukumu secībai sarakstā jāsakrīt ar a_def_soft.h failā definēto efektu sarakstu
  // rindās, kas sākas ar 89. Šis saraksts tiek nodots tīmekļa klientam, kuram ir nepieciešams buferis. Bufera lielums ir MAX_BUFFER_SIZE
  // Ja dotā virkne (UTF-8, divi baiti uz katru rakstzīmi) neiekļaujas buferī - efekti netiks pārsūtīti uz tīmekļa saskarni, efektu joslas nebūs redzamas,
  // pārlūkprogrammas žurnālos parādīsies rinda
  // {"e": "stt","d": "{\"LE\":null}"}
  // json='{"LE":null}'
  // Šādā gadījumā ir vai nu jāsaīsina efektu nosaukumi, vai arī jāpalielina bufera lielums. Taču, palielinot bufera lielumu, var rasties atmiņas trūkums
  // un nestabilu programmaparatūras darbību ESP8266. ESP32 ir vairāk atmiņas - bufera lielumu var palielināt.

  static const char EFFECT_LIST[] PROGMEM =
  "Pulkstenis,Lampas,Sniegputenis,Kubs,Varavīksne,Peintbols,Ugunsgrēks,The Matrix,Tārpi,Zvaigžņu kritums,Confetti," 
  "Krāsu troksnis,Mākoņi,Lava,Plazma,Benzīns ūdenī,Pāvs,Zebra,Skaļš mežs,Jūras sērfošana,Krāsu maiņa," 
  "Ugunspuķes,Virpulis,Ciklons,Mirgošana,Ziemeļblāzma,Ēnas,Labirints,Čūska,Tetris,Arkanoids," 
  "Palete,Spektrs,Sinusas,Vyshhyvanka,Lietus,Kamīns,Strēlītes,Raksti,Rubiks,Zvaigznes,Aizkars,Satiksme,Rītausma,Plūsma,Uguņošana,Svītras"

  // Animācijas, laikapstākļu, diapozitīvu un SD kartes efektus var atspējot ar nosacījumiem USE_ANIMATION = 0 un USE_SD = 0
  // Efektu saraksts WebUI tiek nodots pozicionāli: efektam “Clock” ir ID=0, efektam “SD-card” ir ID=47 (skatīt definīciju a_def_soft.h)
  // Ja tas ir iespējots WebUI pusē, ID (precīzāk, efekta pozīcija sarakstā) tiek nodots kontrolierim. 
  // Lai, atslēdzot, piemēram, “Animācija”, “Laikapstākļi”, “Slaidi”, netiktu veikta nobīde - trūkstošo efektu numerācija tiek nodota kā tukša virkne
  // Tad, iegūstot sarakstu, WebUI izlaidīs tukšos efektus, bet pozīcijas paliks pareizas, un efektam “SD karte” būs ID=47, nevis 44.
  #if (USE_ANIMATION == 1)
  ",Animācija,Laikapstākļi,Slaidi"
  #else 
  ",,,"
  #endif
  #if (USE_SD == 1) 
  ",SD-karte"
  #else 
  ","
  #endif
  ; // <-- šis semikolslēdzis noslēdz operatoru static const char EFFECT_LIST[] PROGMEM =

  // ****************** ALARMU UZSTĀDĪŠANA ********************
  #if (USE_MP3 == 1)
  // SD kartē MP3 atskaņotājā (DFPlayer) saknē ir trīs mapes - “01”, “02” un “03”.
  // Mapē “01” ir MP3 faili ar skaņām, kas tiek atskaņotas, kad notiek modinātājs
  // Mapē “02” ir MP3 faili ar skaņām, kas tiek atskaņotas, kad notiek saullēkts
  // Mapē “03” ir MP3 faili ar skaņām, kas atskaņotas {A} makro skrejvirkne
  // DFPlayer nav iespējas nolasīt failu nosaukumus, bet tikai iegūt mapē esošo failu skaitu.
  // Komanda atskaņot audio nozīmē - atskaņot failu ar indeksu (numuru) N no mapes M
  // Failu numurus nosaka SD kartes failu piešķiršanas tabula (FAT), un tie tiek ģenerēti tādā secībā, kādā faili tiek ierakstīti tukšajā zibatmiņas diskā
  // Tādējādi mapē ierakstītais fails pirmais saņem numuru 1, otrais - 2 un tā tālāk, un tas nav atkarīgs no failu nosaukumiem.
  // Šajos masīvos ir programmā parādīto skaņu nosaukumi tādā secībā, kas atbilst mapēs ierakstīto failu numerācijai.
  //
  // Lai izveidotu pareizu skaņu failu secību, pārdēvējiet tos datorā pagaidu mapē, lai tie būtu šādi
  // piemēram, dodiet tiem tikai ciparu nosaukumus, piemēram, 001.mp3, 002.mp3 utt. vai 
  // piešķirt esošajam nosaukumam ciparu prefiksu, piemēram, 01_birds.mpr, 02_thunder.mp3 un tā tālāk.
  // Failiem mapē jābūt sakārtotiem nosaukumu secībā.
  // Pēc tam tīrā microSD kartē izveidojiet mapi, kurā tiks ievietoti skaņu faili, un nokopējiet tos uz
  // sakārtotu sarakstu.
  // Skaņu saraksts viedtālruņa lietojumprogrammas kombo lodziņā “Trauksmes signāla skaņa”
  static const char ALARM_SOUND_LIST[] PROGMEM = 
  "One Step Over,In the Death Car,Trompete sauс,Bāka,Mister Sandman,Zārks,Banana Phone,Carol of the Bells";
  // Skaņu saraksts izvēles rūtiņai “Rītausmas skaņa” viedtālruņa lietotnē. 
  static const char DAWN_SOUND_LIST[] PROGMEM =
  "Putni,Vētra,Sērfot,Lietus,Straume,Mantra,La Petite Fille De La Mer";
  // Skaņu saraksts {A} makro skrejvirkne
  static const char NOTIFY_SOUND_LIST[] PROGMEM = 
  "Piece Of Сake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board,"
  "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal,"
  "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3";
  #endif

  // Animācijas nosaukumu saraksts. Animācijas ir definētas failā 'animation.ino'.
  #define LANG_IMAGE_LIST_DEF
  static const char LANG_IMAGE_LIST[] PROGMEM = 
  "Sirds,Mario,Laikapstākļi";

  // Modeļu nosaukumu saraksts
  static const char LANG_PATTERNS_LIST[] PROGMEM = 
  "Zigzags,Notis,Rombs,Sirds,Ziemassvētku eglīte,Šūna,Smaidiņš,Zigzaga,Svītras,Viļņi,Svari,Aizkars,Tīkls,Sniegpārsla,Kvadrātiņi,Grieķija,Apļi,Rulete,"
  "Raksts 1,Raksts 2,Raksts 3,Raksts 4,Raksts 5,Raksts 6,Raksts 7,Raksts 8,Raksts 9,Raksts 10,Raksts 11,Raksts 12,Raksts 13,Raksts 14";

  // Laika apstākļi no Yandex
  static const char Y_CODE_01[] PROGMEM = "daļēji mākoņains, neliels lietus"; // cloudy, light rain
  static const char Y_CODE_02[] PROGMEM = "daļēji mākoņains, neliels sniegs"; // cloudy, light snow
  static const char Y_CODE_03[] PROGMEM = "daļēji mākoņains, neliels sniegs un lietus"; // cloudy, wet snow
  static const char Y_CODE_04[] PROGMEM = "daļēji mākoņains"; // partly cloudy
  static const char Y_CODE_05[] PROGMEM = "daļēji mākoņains, lietus"; // partly cloudy, rain
  static const char Y_CODE_06[] PROGMEM = "daļēji mākoņains, sniegs"; // partly cloudy, show
  static const char Y_CODE_07[] PROGMEM = "daļēji mākoņains, sniegs un lietus"; // partly cloudy, wet snow
  static const char Y_CODE_08[] PROGMEM = "putenis"; // snowstorm
  static const char Y_CODE_09[] PROGMEM = "migla"; // fog
  static const char Y_CODE_10[] PROGMEM = "apmācies"; // overcast
  static const char Y_CODE_11[] PROGMEM = "apmācies, brīžiem līst"; // overcast, light rain 
  static const char Y_CODE_12[] PROGMEM = "apmācies, brīžiem sniegs"; // overcast, light snow
  static const char Y_CODE_13[] PROGMEM = "apmācies, brīžiem sniegs un lietus"; // overcast, wet snow 
  static const char Y_CODE_14[] PROGMEM = "apmācies, lietus"; // overcast, rain
  static const char Y_CODE_15[] PROGMEM = "apmācies, sniegs un lietus"; // overcast, wet snow
  static const char Y_CODE_16[] PROGMEM = "apmācies, sniegs"; // overcast, show
  static const char Y_CODE_17[] PROGMEM = "apmācies, lietus un pērkona negaiss"; // overcast, thunderstorm withrain
  static const char Y_CODE_18[] PROGMEM = "skaidrs"; // clear

  // Laika apstākļi no OpenWeatherMap
  static const char W_CODE_200[] PROGMEM = "Pērkona negaiss, neliels lietus"; // thunderstorm with light rain
  static const char W_CODE_201[] PROGMEM = "Lietus ar pērkona negaisu"; // thunderstorm with rain
  static const char W_CODE_202[] PROGMEM = "Pērkona negaiss, spēcīgas lietusgāzes"; // thunderstorm with heavy rain
  static const char W_CODE_210[] PROGMEM = "Neliels pērkona negaiss"; // light thunderstorm
  static const char W_CODE_211[] PROGMEM = "Pērkona negaiss"; // thunderstorm
  static const char W_CODE_212[] PROGMEM = "Spēcīgs pērkona negaiss"; // heavy thunderstorm
  static const char W_CODE_221[] PROGMEM = "Periodisks pērkona negaiss"; // ragged thunderstorm
  static const char W_CODE_230[] PROGMEM = "Pērkona negaiss, neliels smidzinošs lietus"; // thunderstorm with light drizzle
  static const char W_CODE_231[] PROGMEM = "Pērkona negaiss un smidzinošs lietus"; // thunderstorm with drizzle
  static const char W_CODE_232[] PROGMEM = "Pērkona negaiss ar spēcīgu lietu"; // thunderstorm with heavy drizzle
  static const char W_CODE_300[] PROGMEM = "Neliels lietus"; // light intensity drizzle
  static const char W_CODE_301[] PROGMEM = "Smidzinošs lietus"; // drizzle
  static const char W_CODE_302[] PROGMEM = "Spēcīgs smidzinošs lietus"; // heavy intensity drizzle
  static const char W_CODE_310[] PROGMEM = "Neliels smidzinošs lietus"; // light intensity drizzle rain
  static const char W_CODE_311[] PROGMEM = "Smidzinošs lietus"; // drizzle rain
  static const char W_CODE_312[] PROGMEM = "Spēcīgs lietus"; // heavy intensity drizzle rain
  static const char W_CODE_313[] PROGMEM = "Spēcīgs lietus, lietus un smidzinošs lietus"; // shower rain and drizzle
  static const char W_CODE_314[] PROGMEM = "Spēcīga lietusgāze, lietus un smidzinošs lietus"; // heavy shower rain and drizzle
  static const char W_CODE_321[] PROGMEM = "Smidzinošs lietus"; // shower drizzle 
  static const char W_CODE_500[] PROGMEM = "Neliels lietus"; // light rain
  static const char W_CODE_501[] PROGMEM = "Mērens lietus"; // moderate rain
  static const char W_CODE_502[] PROGMEM = "Spēcīgs lietus"; // heavy intensity rain
  static const char W_CODE_503[] PROGMEM = "Spēcīgs lietus"; // very heavy rain
  static const char W_CODE_504[] PROGMEM = "Lietusgāze"; // extreme rain
  static const char W_CODE_511[] PROGMEM = "Krusa"; // freezing rain
  static const char W_CODE_520[] PROGMEM = "Neliels lietus"; // light intensity shower rain
  static const char W_CODE_521[] PROGMEM = "Smidzinošs lietus"; // shower rain
  static const char W_CODE_522[] PROGMEM = "Spēcīgs lietus"; // heavy intensity shower rain
  static const char W_CODE_531[] PROGMEM = "Brīžiem līst"; // ragged shower rain
  static const char W_CODE_600[] PROGMEM = "Neliels sniegs"; // light snow
  static const char W_CODE_601[] PROGMEM = "Sniegs"; // Snow
  static const char W_CODE_602[] PROGMEM = "Sniegputenis"; // Heavy snow
  static const char W_CODE_611[] PROGMEM = "Slapjš"; // Sleet
  static const char W_CODE_612[] PROGMEM = "Viegls sniegs"; // Light shower sleet
  static const char W_CODE_613[] PROGMEM = "Lietusgāze, sniegs"; // Shower sleet
  static const char W_CODE_615[] PROGMEM = "Slapjš sniegs"; // Light rain and snow
  static const char W_CODE_616[] PROGMEM = "Lietus ar sniegu"; // Rain and snow
  static const char W_CODE_620[] PROGMEM = "Neliels sniegputenis"; // Light shower snow
  static const char W_CODE_621[] PROGMEM = "Sniegputenis"; // Shower snow
  static const char W_CODE_622[] PROGMEM = "Spēcīgs sniegputenis"; // Heavy shower snow
  static const char W_CODE_701[] PROGMEM = "Migla"; // mist
  static const char W_CODE_711[] PROGMEM = "Dūmaka"; // Smoke
  static const char W_CODE_721[] PROGMEM = "Neliela migla"; // Haze
  static const char W_CODE_731[] PROGMEM = "Putekļu virpuļi"; // sand/ dust whirls
  static const char W_CODE_741[] PROGMEM = "Migla"; // fog
  static const char W_CODE_751[] PROGMEM = "Smilšu virpuļi"; // sand
  static const char W_CODE_761[] PROGMEM = "Putekļu virpuļi"; // dust
  static const char W_CODE_762[] PROGMEM = "Vulkāniskie pelni"; // volcanic ash
  static const char W_CODE_771[] PROGMEM = "Brāzmains vējš"; // squalls
  static const char W_CODE_781[] PROGMEM = "Tornado"; // tornado
  static const char W_CODE_800[] PROGMEM = "Skaidras debesis"; // clear sky
  static const char W_CODE_801[] PROGMEM = "Neliels mākoņainums"; // few clouds: 11-25%
  static const char W_CODE_802[] PROGMEM = "Daļēji mākoņains"; // scattered clouds: 25-50%
  static const char W_CODE_803[] PROGMEM = "Daļēji mākoņains"; // broken clouds: 51-84%
  static const char W_CODE_804[] PROGMEM = "Apmācies"; // overcast clouds: 85-100%

  // Datuma izvades konstantes atzīmētājā

  static const char SMonth_01[] PROGMEM = "janvāra";
  static const char SMonth_02[] PROGMEM = "februāra";
  static const char SMonth_03[] PROGMEM = "marta";
  static const char SMonth_04[] PROGMEM = "aprīļa";
  static const char SMonth_05[] PROGMEM = "maija";
  static const char SMonth_06[] PROGMEM = "jūnija";
  static const char SMonth_07[] PROGMEM = "jūlija";
  static const char SMonth_08[] PROGMEM = "augusta";
  static const char SMonth_09[] PROGMEM = "septembra";
  static const char SMonth_10[] PROGMEM = "oktobra";
  static const char SMonth_11[] PROGMEM = "novembra";
  static const char SMonth_12[] PROGMEM = "decembra";

  static const char SMnth_01[] PROGMEM = "jan";
  static const char SMnth_02[] PROGMEM = "feb";
  static const char SMnth_03[] PROGMEM = "mar";
  static const char SMnth_04[] PROGMEM = "apr";
  static const char SMnth_05[] PROGMEM = "maj";
  static const char SMnth_06[] PROGMEM = "jūn";
  static const char SMnth_07[] PROGMEM = "jūl";
  static const char SMnth_08[] PROGMEM = "aug";
  static const char SMnth_09[] PROGMEM = "sep";
  static const char SMnth_10[] PROGMEM = "okt";
  static const char SMnth_11[] PROGMEM = "nov";
  static const char SMnth_12[] PROGMEM = "dec";

  static const char SDayFull_1[] PROGMEM = "pirmdiena";
  static const char SDayFull_2[] PROGMEM = "otrdiena";
  static const char SDayFull_3[] PROGMEM = "trešdiena";
  static const char SDayFull_4[] PROGMEM = "ceturtdiena";
  static const char SDayFull_5[] PROGMEM = "piektdiena";
  static const char SDayFull_6[] PROGMEM = "sestdiena";
  static const char SDayFull_7[] PROGMEM = "svētdiena";

  static const char SDayShort_1[] PROGMEM = "pir";
  static const char SDayShort_2[] PROGMEM = "otr";
  static const char SDayShort_3[] PROGMEM = "tre";
  static const char SDayShort_4[] PROGMEM = "cet";
  static const char SDayShort_5[] PROGMEM = "pie";
  static const char SDayShort_6[] PROGMEM = "ses";
  static const char SDayShort_7[] PROGMEM = "sv";
  static const char SDayShrt_1[] PROGMEM = "pr";
  static const char SDayShrt_2[] PROGMEM = "ot";
  static const char SDayShrt_3[] PROGMEM = "tr";
  static const char SDayShrt_4[] PROGMEM = "ct";
  static const char SDayShrt_5[] PROGMEM = "pk";
  static const char SDayShrt_6[] PROGMEM = "ss";
  static const char SDayShrt_7[] PROGMEM = "sv";

  static const char SDayForm_1[] PROGMEM = " dienas"; // Atstarpe sākumā ir obligāta
  static const char SDayForm_2[] PROGMEM = " diena";
  static const char SDayForm_3[] PROGMEM = " dienas";

  static const char SHourForm_1[] PROGMEM = " stunda"; // Atstarpe sākumā ir obligāta
  static const char SHourForm_2[] PROGMEM = " stundas";
  static const char SHourForm_3[] PROGMEM = " stundas";

  static const char SMinuteForm_1[] PROGMEM = " minūtes"; // Atstarpe sākumā ir obligāta
  static const char SMinuteForm_2[] PROGMEM = " minūte";
  static const char SMinuteForm_3[] PROGMEM = " minūtes";

  static const char SSecondForm_1[] PROGMEM = " sekundes"; // Atstarpe sākumā ir obligāta
  static const char SSecondForm_2[] PROGMEM = " sekunde";
  static const char SSecondForm_3[] PROGMEM = " sekundes";

  // Veicamās rindas, kas tiks aizpildīta ar šīm vērtībām, kad a_def_hard.h (62. rindā) ir iestatīts karodziņš INITIALIZE_TEXTS == 1, piemēri
  // inicializācijas posmā, pirmo reizi palaižot programmaparatūru mikrokontrolierī.
  // Šajos piemēros ir sniegtas dažas iespējas, kā izmantot makroautomātiskos kodus ķeksīša programmā.
  #if (INITIALIZE_TEXTS == 1)
  static const char textLine_0[] PROGMEM = "##";
  static const char textLine_1[] PROGMEM = "Viss būs kārtībā!";
  static const char textLine_2[] PROGMEM = "Līdz {C#00D0FF}Jaunajam gadam {C#FFFFFF} atlicis {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59:59}{E21}";
  static const char textLine_3[] PROGMEM = "Līdz {C#0019FF}Jaunajam gadam{C#FFFFFF} {P01.01.****#4}";
  static const char textLine_4[] PROGMEM = "Laimīgu {C#00D0FF}Jauno {C#0BFF00}{D:yyyy} {C#FFFFFF}gadu!{S01.01.****#31.01.**** 23:59:59}{E21}";
  static const char textLine_5[] PROGMEM = "{C#10FF00}Rīgā {C#FFFFFF}{WS} {WT}";
  static const char textLine_6[] PROGMEM = "Show must go on!{C#000002}";
  static const char textLine_7[] PROGMEM = "{C#FF000F}Krible! {C#000001}Krable!! {C#00FF00}Bums!!!{E24}";
  static const char textLine_8[] PROGMEM = "Sagatavojieties, cilvēki {C#FF0300}tuvojas vasara!{S01.01.****#10.04.****}";
  static const char textLine_9[] PROGMEM = "";
  static const char textLine_A[] PROGMEM = "";
  static const char textLine_B[] PROGMEM = "";
  static const char textLine_C[] PROGMEM = "Laimīgu Jauno {C#00C911}{D:yyy+}{C#FFFFFF} gadu!{S01.12.****#31.12.****}";
  static const char textLine_D[] PROGMEM = "";
  static const char textLine_E[] PROGMEM = "";
  static const char textLine_F[] PROGMEM = "{C#10FF00}Rīgā {C#FFFFFF}{WS} {WT}";
  static const char textLine_G[] PROGMEM = "";
  static const char textLine_H[] PROGMEM = "Šodien {D:d MMMM yyyy} gadi, {D:dddd}";
  static const char textLine_I[] PROGMEM = "";
  static const char textLine_J[] PROGMEM = "";
  static const char textLine_K[] PROGMEM = "Priecīgus Ziemassvētkus!{S07.01.****}";
  static const char textLine_L[] PROGMEM = "";
  static const char textLine_M[] PROGMEM = "Šodien {D:dddd dd MMMM}, uz ielas {WS}, {WT}";
  static const char textLine_N[] PROGMEM = "Priecīgus svētkus, kaimiņi!";
  static const char textLine_O[] PROGMEM = "Neesiet garlaicīgi!";
  static const char textLine_P[] PROGMEM = "";
  static const char textLine_Q[] PROGMEM = "";
  static const char textLine_R[] PROGMEM = "";
  static const char textLine_S[] PROGMEM = "";
  static const char textLine_T[] PROGMEM = "";
  static const char textLine_U[] PROGMEM = "";
  static const char textLine_V[] PROGMEM = "";
  static const char textLine_W[] PROGMEM = "";
  static const char textLine_X[] PROGMEM = "";
  static const char textLine_Y[] PROGMEM = "Pacelieties {P7:30#Z#60#60#12345}!";
  static const char textLine_Z[] PROGMEM = "Labrīt!";
  #endif
  // Darbības rezultātu virknes un daži citi ziņojumi, ko pārraida
  // no programmaparatūras uz tīmekļa lietojumprogrammu, lai parādītu
  static const char MSG_FILE_DELETED[] PROGMEM = "Izdzēsts fails";
  static const char MSG_FILE_DELETE_ERROR[] PROGMEM = "Failu dzēšanas kļūda";
  static const char MSG_FILE_SAVED[] PROGMEM = "Faili ir saglabāti";
  static const char MSG_FILE_SAVE_ERROR[] PROGMEM = "Kļūda, rakstot failā";
  static const char MSG_FILE_CREATE_ERROR[] PROGMEM = "Faila izveides kļūda";
  static const char MSG_FOLDER_CREATE_ERROR[] PROGMEM = "Kļūda, izveidojot glabāšanas mapi";
  static const char MSG_FILE_LOADED[] PROGMEM = "Augšupielādētais fails";
  static const char MSG_FILE_LOAD_ERROR[] PROGMEM = "Failu nolasīšanas kļūda";
  static const char MSG_FILE_NOT_FOUND[] PROGMEM = "Faili nav atrasti";
  static const char MSG_FOLDER_NOT_FOUND[] PROGMEM = "Mapes nav atrastas";
  static const char MSG_BACKUP_SAVE_ERROR[] PROGMEM = "Neizdevās dublēt iestatījumu rezerves kopiju";
  static const char MSG_BACKUP_SAVE_OK[] PROGMEM = "Iestatījumu dublēšana";
  static const char MSG_BACKUP_LOAD_ERROR[] PROGMEM = "Neizdevās ielādēt iestatījumu rezerves kopiju";
  static const char MSG_BACKUP_LOAD_OK[] PROGMEM = "Iestatījumu atjaunošana no dublējuma";
  static const char MSG_OP_SUCCESS[] PROGMEM = "Saglabāti iestatījumi";
  static const char MSG_TEXT_EXPORT_ERROR[] PROGMEM = "Tekstu eksportēšanas kļūda";
  static const char MSG_TEXT_EXPORT_OK[] PROGMEM = "Eksportētie teksti";
  static const char MSG_TEXT_IMPORT_ERROR[] PROGMEM = "Tekstu importēšanas kļūda";
  static const char MSG_TEXT_IMPORT_OK[] PROGMEM = "Importētie teksti";

  static const char MODE_NIGHT_CLOCK[] PROGMEM = "Nakts pulksteņi";
  static const char MODE_CLOCK[] PROGMEM = "Pulkstenis";
  static const char MODE_RUNNING_TEXT[] PROGMEM = "Skrejošā līnija";
  static const char MODE_LOAD_PICTURE[] PROGMEM = "Attēlu augšupielāde";
  static const char MODE_DRAW[] PROGMEM = "Zīmēšana";
  static const char MODE_DAWN[] PROGMEM = "Rītausma";

  static const char WTR_LANG_YA[] = "lv"; // Яндекс.Погода - tikai šķiet, ka saprot. "ru" и "en"
  static const char WTR_LANG_OWM[] = "lv"; // OpenWeatherMap = 2-[ valodas kods - ru,en,de,fr,it n tā tālāk. Ja valoda nezina - atgriežas kā par en
#endif

// ================================================================================================= 

#ifndef UI
#error Не определен язык интерфейса в файле 'a_def_lang.h'
#endif
