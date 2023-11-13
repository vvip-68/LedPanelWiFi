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
#ifndef  LANG
#define LANG 'RUS'
#endif

// ============================================== RUS ============================================== 

#if (LANG == 'RUS')
  #define UI F("RUS")
  
  // Список и порядок эффектов, передаваемый в Web-интерфейс. 
  // Порядок имен эффектов в списке должен соответствовать списку эффектов, определенному в файле a_def_soft.h 
  // в строках, начиная с 119 
  
  #if (USE_SD == 1)
    static const char EFFECT_LIST[] PROGMEM = "Часы,Лампа,Снегопад,Кубик,Радуга,Пейнтбол,Огонь,The Matrix,Шарики,Звездопад,Конфетти,Цветной шум,Облака,Лава,Плазма,Радужные переливы,Павлин,Зебра,Шумящий лес,Морской прибой,Смена цвета,Светлячки,Водоворот,Циклон,Мерцание,Северное сияние,Тени,Лабиринт,Змейка,Тетрис,Арканоид,Палитра,Спектрум,Синусы,Вышиванка,Дождь,Камин,Стрелки,Анимация,Погода,Узоры,Рубик,Звёзды,Штора,Трафик,Слайды,Рассвет,SD-Карта";
    #define EFFECT_LIST_LANG  // затычка
  #else
    static const char EFFECT_LIST[] PROGMEM = "Часы,Лампа,Снегопад,Кубик,Радуга,Пейнтбол,Огонь,The Matrix,Шарики,Звездопад,Конфетти,Цветной шум,Облака,Лава,Плазма,Радужные переливы,Павлин,Зебра,Шумящий лес,Морской прибой,Смена цвета,Светлячки,Водоворот,Циклон,Мерцание,Северное сияние,Тени,Лабиринт,Змейка,Тетрис,Арканоид,Палитра,Спектрум,Синусы,Вышиванка,Дождь,Камин,Стрелки,Анимация,Погода,Узоры,Рубик,Звёзды,Штора,Трафик,Слайды,Рассвет";
    #define EFFECT_LIST_LANG  // затычка
  #endif

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
  #define ALARM_SOUND_LIST   F("One Step Over,In the Death Car,Труба зовет,Маяк,Mister Sandman,Шкатулка,Banana Phone,Carol of the Bells")
  
  // Список звуков для комбобокса "Звук рассвета" в приложении на смартфоне  
  #define DAWN_SOUND_LIST    F("Птицы,Гроза,Прибой,Дождь,Ручей,Мантра,La Petite Fille De La Mer")
  
  // Список звуков для макроса {A} бегущей строки
  #define NOTIFY_SOUND_LIST  F("Piece Of Сake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board," \
                               "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal," \
                               "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3")
  
  #endif

  // Список названия анимаций. Анимации определены в файле 'animation.ino'
  #define LANG_IMAGE_LIST F("Сердце,Марио,Погода")   

  // Список названий узоров
  #define LANG_PATTERNS_LIST F("Зигзаг,Ноты,Ромб,Сердце,Елка,Клетка,Смайлик,Зигзаг,Полосы,Волны,Чешуя,Портьера,Плетенка,Снежинка,Квадратики,Греция,Круги,Рулет," \
                               "Узор 1,Узор 2,Узор 3,Узор 4,Узор 5,Узор 6,Узор 7,Узор 8,Узор 9,Узор 10,Узор 11,Узор 12,Узор 13,Узор 14")

  // Погодные условия от Yandex
  #define Y_CODE_01 F("облачно с прояснениями, небольшой дождь")           // cloudy, light rain
  #define Y_CODE_02 F("облачно с прояснениями, небольшой снег")            // cloudy, light snow
  #define Y_CODE_03 F("облачно с прояснениями, небольшой снег с дождем")   // cloudy, wet snow
  #define Y_CODE_04 F("переменная облачность")                             // partly cloudy
  #define Y_CODE_05 F("переменная облачность, дождь")                      // partly cloudy, rain
  #define Y_CODE_06 F("переменная облачность, снег")                       // partly cloudy, show
  #define Y_CODE_07 F("переменная облачность, снег с дождем")              // partly cloudy, wet snow
  #define Y_CODE_08 F("метель")                                            // snowstorm
  #define Y_CODE_09 F("туман")                                             // fog
  #define Y_CODE_10 F("пасмурно")                                          // overcast
  #define Y_CODE_11 F("пасмурно, временами дождь")                         // overcast, light rain 
  #define Y_CODE_12 F("пасмурно, временами снег")                          // overcast, light snow
  #define Y_CODE_13 F("пасмурно, временами снег с дождем")                 // overcast, wet snow 
  #define Y_CODE_14 F("пасмурно, дождь")                                   // overcast, rain
  #define Y_CODE_15 F("пасмурно, снег с дождем")                           // overcast, wet snow
  #define Y_CODE_16 F("пасмурно, снег")                                    // overcast, show
  #define Y_CODE_17 F("пасмурно, дождь c грозой")                          // overcast, thunderstorm withrain
  #define Y_CODE_18 F("ясно")                                              // clear

  // Погодные условия от OpenWeatherMap
  #define W_CODE_200 F("Гроза, небольшой дождь")                           // thunderstorm with light rain
  #define W_CODE_201 F("Дождь с грозой")                                   // thunderstorm with rain
  #define W_CODE_202 F("Гроза, ливни")                                     // thunderstorm with heavy rain
  #define W_CODE_210 F("Небольшая гроза")                                  // light thunderstorm
  #define W_CODE_211 F("Гроза")                                            // thunderstorm
  #define W_CODE_212 F("Сильная гроза")                                    // heavy thunderstorm
  #define W_CODE_221 F("Прерывистые грозы")                                // ragged thunderstorm
  #define W_CODE_230 F("Гроза, небольшой дождь")                           // thunderstorm with light drizzle
  #define W_CODE_231 F("Гроза с дождем")                                   // thunderstorm with drizzle
  #define W_CODE_232 F("Гроза с проливным дождем")                         // thunderstorm with heavy drizzle
  #define W_CODE_300 F("Мелкий дождь")                                     // light intensity drizzle
  #define W_CODE_301 F("Моросящий дождь")                                  // drizzle
  #define W_CODE_302 F("Сильный дождь")                                    // heavy intensity drizzle
  #define W_CODE_310 F("Небольшой дождь")                                  // light intensity drizzle rain
  #define W_CODE_311 F("Моросящий дождь")                                  // drizzle rain
  #define W_CODE_312 F("Сильный дождь")                                    // heavy intensity drizzle rain
  #define W_CODE_313 F("Ливень, дождь и морось")                           // shower rain and drizzle
  #define W_CODE_314 F("Сильный ливень, дождь и морось")                   // heavy shower rain and drizzle
  #define W_CODE_321 F("Моросящий дождь")                                  // shower drizzle  
  #define W_CODE_500 F("Небольшой дождь")                                  // light rain
  #define W_CODE_501 F("Умеренный дождь")                                  // moderate rain
  #define W_CODE_502 F("Ливень")                                           // heavy intensity rain
  #define W_CODE_503 F("Проливной дождь")                                  // very heavy rain
  #define W_CODE_504 F("Проливной дождь")                                  // extreme rain
  #define W_CODE_511 F("Град")                                             // freezing rain
  #define W_CODE_520 F("Небольшой дождь")                                  // light intensity shower rain
  #define W_CODE_521 F("Моросящий дождь")                                  // shower rain
  #define W_CODE_522 F("Сильный дождь")                                    // heavy intensity shower rain
  #define W_CODE_531 F("Временами дождь")                                  // ragged shower rain
  #define W_CODE_600 F("Небольшой снег")                                   // light snow
  #define W_CODE_601 F("Снег")                                             // Snow
  #define W_CODE_602 F("Снегопад")                                         // Heavy snow
  #define W_CODE_611 F("Слякоть")                                          // Sleet
  #define W_CODE_612 F("Легкий снег")                                      // Light shower sleet
  #define W_CODE_613 F("Ливень, снег")                                     // Shower sleet
  #define W_CODE_615 F("Мокрый снег")                                      // Light rain and snow
  #define W_CODE_616 F("Дождь со снегом")                                  // Rain and snow
  #define W_CODE_620 F("Небольшой снегопад")                               // Light shower snow
  #define W_CODE_621 F("Снегопад, метель")                                 // Shower snow
  #define W_CODE_622 F("Сильный снегопад")                                 // Heavy shower snow
  #define W_CODE_701 F("Туман")                                            // mist
  #define W_CODE_711 F("Дымка")                                            // Smoke
  #define W_CODE_721 F("Легкий туман")                                     // Haze
  #define W_CODE_731 F("Пыльные вихри")                                    // sand/ dust whirls
  #define W_CODE_741 F("Туман")                                            // fog
  #define W_CODE_751 F("Песчаные вихри")                                   // sand
  #define W_CODE_761 F("Пыльные вихри")                                    // dust
  #define W_CODE_762 F("Вулканический пепел")                              // volcanic ash
  #define W_CODE_771 F("Шквалистый ветер")                                 // squalls
  #define W_CODE_781 F("Торнадо")                                          // tornado
  #define W_CODE_800 F("Ясно")                                             // clear sky
  #define W_CODE_801 F("Небольшая облачность")                             // few clouds: 11-25%
  #define W_CODE_802 F("Переменная облачность")                            // scattered clouds: 25-50%
  #define W_CODE_803 F("Облачно с прояснениями")                           // broken clouds: 51-84%
  #define W_CODE_804 F("Пасмурно")                                         // overcast clouds: 85-100%

  // Константы вывода даты в бегущей строке

  #define SMonth_01  F("января")
  #define SMonth_02  F("февраля")
  #define SMonth_03  F("марта")
  #define SMonth_04  F("апреля")
  #define SMonth_05  F("мая")
  #define SMonth_06  F("июня")
  #define SMonth_07  F("июля")
  #define SMonth_08  F("августа")
  #define SMonth_09  F("сентября")
  #define SMonth_10  F("октября")
  #define SMonth_11  F("ноября")
  #define SMonth_12  F("декабря")

  #define SMnth_01   F("янв")
  #define SMnth_02   F("фев")
  #define SMnth_03   F("мар")
  #define SMnth_04   F("апр")
  #define SMnth_05   F("май")
  #define SMnth_06   F("июн")
  #define SMnth_07   F("июл")
  #define SMnth_08   F("авг")
  #define SMnth_09   F("сен")
  #define SMnth_10   F("окт")
  #define SMnth_11   F("ноя")
  #define SMnth_12   F("дек")

  #define SDayFull_1 F("понедельник")
  #define SDayFull_2 F("вторник")
  #define SDayFull_3 F("среда")
  #define SDayFull_4 F("четверг")
  #define SDayFull_5 F("пятница")
  #define SDayFull_6 F("суббота")
  #define SDayFull_7 F("воскресенье")

  #define SDayShort_1 F("пон")
  #define SDayShort_2 F("втр")
  #define SDayShort_3 F("срд")
  #define SDayShort_4 F("чтв")
  #define SDayShort_5 F("птн")
  #define SDayShort_6 F("сбт")
  #define SDayShort_7 F("вск")
  
  #define SDayShrt_1 F("пн")
  #define SDayShrt_2 F("вт")
  #define SDayShrt_3 F("ср")
  #define SDayShrt_4 F("чт")
  #define SDayShrt_5 F("пт")
  #define SDayShrt_6 F("сб")
  #define SDayShrt_7 F("вс")

  #define SDayForm_1 F(" дней")        // Пробел в начале обязателен
  #define SDayForm_2 F(" день")
  #define SDayForm_3 F(" дня")
 
  #define SHourForm_1 F(" час")        // Пробел в начале обязателен
  #define SHourForm_2 F(" часа")
  #define SHourForm_3 F(" часов")

  #define SMinuteForm_1 F(" минут")    // Пробел в начале обязателен
  #define SMinuteForm_2 F(" минута")
  #define SMinuteForm_3 F(" минуты")

  #define SSecondForm_1 F(" секунд")   // Пробел в начале обязателен
  #define SSecondForm_2 F(" секунда")
  #define SSecondForm_3 F(" секунды")

  // Примеры строк для "Бегущей строки", которые будут заполнены этими значениями при установленном флаге INITIALIZE_TEXTS == 1 в a_def_hard.h (в строке 62)
  // на шаге инициализации при первом запуске прошивки на микроконтроллере.
  // Данные примеры содержат некоторые варианты использования макросов в бегущей строке.
  
  #define textLine_0  F("##")
  #define textLine_1  F("Всё будет хорошо!")
  #define textLine_2  F("До {C#00D0FF}Нового года {C#FFFFFF}осталось {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59:59}{E21}")
  #define textLine_3  F("До {C#0019FF}Нового года{C#FFFFFF} {P01.01.****#4}")
  #define textLine_4  F("С {C#00D0FF}Новым {C#0BFF00}{D:yyyy} {C#FFFFFF}годом!{S01.01.****#31.01.**** 23:59:59}{E21}")
  #define textLine_5  F("В {C#10FF00}Красноярске {C#FFFFFF}{WS} {WT}°C")
  #define textLine_6  F("Show must go on!{C#000002}")
  #define textLine_7  F("{C#FF000F}Крибле! {C#000001}Крабле!! {C#00FF00}Бумс!!!{E24}")
  #define textLine_8  F("Крепитесь, люди - скоро {C#FF0300}лето!{S01.01.****#10.04.****}")
  #define textLine_9  F("Это {C#0081FF}\"ж-ж-ж\"{C#FFFFFF} - неспроста!")
  #define textLine_A  F("{C#000001}Раз! Два!! Три!!! {C#33C309}Ёлочка,{B#000000}{C#000001} гори!!!")
  #define textLine_B  F("Дело было вечером, делать было нечего...")
  #define textLine_C  F("Ах, какая прелесть!")
  #define textLine_D  F("Это нужно обдумать...")
  #define textLine_E  F("В этом что-то есть...")
  #define textLine_F  F("В {C#10FF00}Красноярске {C#FFFFFF}{WS} {WT}°C")
  #define textLine_G  F("Вот оно что, {C#FF0000}Михалыч{C#FFFFFF}!..")
  #define textLine_H  F("Сегодня {D:dd MMMM yyyy} года, {D:dddd}")
  #define textLine_I  F("Лень - двигатель прогресса")
  #define textLine_J  F("Чем бы дитя не тешилось...")
  #define textLine_K  F("С Рождеством!{S07.01.****}")
  #define textLine_L  F("С днем Победы!{S09.05.**** 7:00#09.05.**** 21:30}")
  #define textLine_M  F("Сегодня {D:dddd dd MMMM}, на улице {WS}, {WT}°C{E25}")
  #define textLine_N  F("С праздником, соседи!")
  #define textLine_O  F("Не скучайте!")
  #define textLine_P  F("Счастья всем! И пусть никто не уйдет обиженным.")
  #define textLine_Q  F("В отсутствии солнца сияйте сами!")
  #define textLine_R  F("Почему? Потому!")
  #define textLine_S  F("Время принимать решения!")
  #define textLine_T  F("Время делать выводы!")
  #define textLine_U  F("Лето, ах лето!{S15.05.****#01.09.****}")
  #define textLine_V  F("Нет предела совершенству!")
  #define textLine_W  F("Чего изволите, ваша светлость?")
  #define textLine_X  F("Курочка по зёрнышку, копеечка к копеечке!")
  #define textLine_Y  F("Подъем через {P7:30#Z#60#60#12345}!")
  #define textLine_Z  F("-Доброе утро!")

  // Строки результатов выполнения операций и некоторых других сообщений, передаваемых
  // из прошивки в Web-приложение для отображения
  
  #define MSG_FILE_DELETED        F("Файл удален")
  #define MSG_FILE_DELETE_ERROR   F("Ошибка удаления файла")
  #define MSG_FILE_SAVED          F("Файл сохранен")
  #define MSG_FILE_SAVE_ERROR     F("Ошибка записи в файл")
  #define MSG_FILE_CREATE_ERROR   F("Ошибка создания файла")
  #define MSG_FOLDER_CREATE_ERROR F("Ошибка создания папки для хранения изображений")
  #define MSG_FILE_LOADED         F("Файл загружен")
  #define MSG_FILE_LOAD_ERROR     F("Ошибка чтения файла")
  #define MSG_FILE_NOT_FOUND      F("Файл не найден")
  #define MSG_FOLDER_NOT_FOUND    F("Папка для хранения изображений не найдена")
  #define MSG_BACKUP_SAVE_ERROR   F("Не удалось сохранить резервную копию настроек")
  #define MSG_BACKUP_SAVE_OK      F("Резервная копия настроек создана")
  #define MSG_BACKUP_LOAD_ERROR   F("Не удалось загрузить резервную копию настроек")
  #define MSG_BACKUP_LOAD_OK      F("Настройки из резервной копии восстановлены")
  #define MSG_OP_SUCCESS          F("Настройки сохранены")

  #define MODE_NIGHT_CLOCK        F("Ночные часы")
  #define MODE_CLOCK              F("Часы")
  #define MODE_RUNNING_TEXT       F("Бегущая строка")
  #define MODE_LOAD_PICTURE       F("Загрузка изображения")
  #define MODE_DRAW               F("Рисование")
  #define MODE_DAWN               F("Рассвет")

  #define WTR_LANG_YA             F("ru")      // Яндекс.Погода - кажется понимает только "ru" и "en"
  #define WTR_LANG_OWM            F("ru")      // OpenWeatherMap = 2-[ бкувенный код языка - ru,en,de,fr,it и так далее. Если язык не знает - возвращает как для en
  
#endif

// ============================================== ENG ============================================== 

#if (LANG == 'ENG')
  #define UI F("ENG")

  // List of effects names and order in sequence trensferring to Web-application. 
  // Order of Names MUST correspont to order of effect ID definitions declared in a_def_soft.h file
  // starting from line 119
  
  #if (USE_SD == 1)                      
    #define EFFECT_LIST F("Clock,Lamp,Showfall,Cube,Rainbow,Paintball,Fire,The Matrix,Balls,Starfall,Confetti," \
                          "Color noise,Clouds,Lava,Plasma,Iridescent,Peacock,Zebra,Noisy forest,Sea surf,Color change," \
                          "Fireflies,Whirlpool,Cyclone,Flicker,Northern lights,Shadows,Maze,Snake,Tetris,Arkanoid," \
                          "Palette,Spectrum,Sinuses,Embroidery,Rain,Fireplace,Arrows,Animation,Weather,Patterns,Rubic,Stars," \
                          "Curtain,Traffic,Slides,Dawn,SD-card")
  #else
    #define EFFECT_LIST F("Clock,Lamp,Showfall,Cube,Rainbow,Paintball,Fire,The Matrix,Balls,Starfall,Confetti," \
                          "Color noise,Clouds,Lava,Plasma,Iridescent,Peacock,Zebra,Noisy forest,Sea surf,Color change," \
                          "Fireflies,Whirlpool,Cyclone,Flicker,Northern lights,Shadows,Maze,Snake,Tetris,Arkanoid," \
                          "Palette,Spectrum,Sinuses,Embroidery,Rain,Fireplace,Arrows,Animation,Weather,Patterns,Rubic,Stars," \
                          "Curtain,Traffic,Slides,Dawn")                          
  #endif
  
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
  #define ALARM_SOUND_LIST   F("One Step Over,In the Death Car,Horn call,Lighthouse,Mister Sandman,Сasket,Banana Phone,Carol of the Bells")
  
  // List of sounds for "Dawn sound" combobox in phone application or Web-interface
  #define DAWN_SOUND_LIST    F("Birds,Thunderstorm,Surf,Rain,Creek,The Mantra,La Petite Fille De La Mer")
  
  // List of sounds for {A} macros of running text in phone application or Web-interface
  #define NOTIFY_SOUND_LIST  F("Piece Of Cake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board," \
                               "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal," \
                               "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3")
  
  #endif


  // List of animation names. Animations are defined in file 'animation.ino'
  #define LANG_IMAGE_LIST F("Heart,Mario,Weather")   
  
  // List names of Patterns effect
  #define LANG_PATTERNS_LIST F("Zigzag,Notes,Rhomb,Heart,Fir tree,Cells,Smile,Zigzag-2,Streaks,Waves,Scales,Curtain,Wicker,Snowflake,Squares,Greece,Circles,Roll," \
                               "Pattern 1,Pattern 2,Pattern 3,Pattern 4,Pattern 5,Pattern 6,Pattern 7,Pattern 8,Pattern 9,Pattern 10,Pattern 11,Pattern 12,Pattern 13,Pattern 14")

  // Weather condition from Yandex
  #define Y_CODE_01 F("Cloudy, light rain")                     // cloudy, light rain
  #define Y_CODE_02 F("Cloudy, light snow")                     // cloudy, light snow
  #define Y_CODE_03 F("Cloudy, wet snow")                       // cloudy, wet snow
  #define Y_CODE_04 F("Partly cloudy")                          // partly cloudy
  #define Y_CODE_05 F("Partly cloudy, rain")                    // partly cloudy, rain
  #define Y_CODE_06 F("Partly cloudy, show")                    // partly cloudy, show
  #define Y_CODE_07 F("Partly cloudy, wet snow")                // partly cloudy, wet snow
  #define Y_CODE_08 F("Snowstorm")                              // snowstorm
  #define Y_CODE_09 F("Fog")                                    // fog
  #define Y_CODE_10 F("Overcast clouds")                        // overcast
  #define Y_CODE_11 F("Overcast clouds, light rain")            // overcast, light rain 
  #define Y_CODE_12 F("Overcast clouds, light snow")            // overcast, light snow
  #define Y_CODE_13 F("Overcast clouds, wet snow")              // overcast, wet snow 
  #define Y_CODE_14 F("Overcast clouds, rain")                  // overcast, rain
  #define Y_CODE_15 F("Overcast clouds, wet snow")              // overcast, wet snow
  #define Y_CODE_16 F("Overcast clouds, show")                  // overcast, show
  #define Y_CODE_17 F("Overcast clouds, thunderstorm withrain") // overcast, thunderstorm withrain
  #define Y_CODE_18 F("Clear sky")                              // clear

  // Weather condition from OpenWeatherMap
  #define W_CODE_200 F("Thunderstorm with light rain")          // thunderstorm with light rain
  #define W_CODE_201 F("Thunderstorm with rain")                // thunderstorm with rain
  #define W_CODE_202 F("Thunderstorm with heavy rain")          // thunderstorm with heavy rain
  #define W_CODE_210 F("Light thunderstorm")                    // light thunderstorm
  #define W_CODE_211 F("Thunderstorm")                          // thunderstorm
  #define W_CODE_212 F("Heavy thunderstorm")                    // heavy thunderstorm
  #define W_CODE_221 F("Ragged thunderstorm")                   // ragged thunderstorm
  #define W_CODE_230 F("Thunderstorm with light drizzle")       // thunderstorm with light drizzle
  #define W_CODE_231 F("Thunderstorm with drizzle")             // thunderstorm with drizzle
  #define W_CODE_232 F("Thunderstorm with heavy drizzle")       // thunderstorm with heavy drizzle
  #define W_CODE_300 F("Light intensity drizzle")               // light intensity drizzle
  #define W_CODE_301 F("Drizzle")                               // drizzle
  #define W_CODE_302 F("Heavy intensity drizzle")               // heavy intensity drizzle
  #define W_CODE_310 F("Light intensity drizzle rain")          // light intensity drizzle rain
  #define W_CODE_311 F("Drizzle rain")                          // drizzle rain
  #define W_CODE_312 F("Heavy intensity drizzle rain")          // heavy intensity drizzle rain
  #define W_CODE_313 F("Shower rain and drizzle")               // shower rain and drizzle
  #define W_CODE_314 F("Heavy shower rain and drizzle")         // heavy shower rain and drizzle
  #define W_CODE_321 F("Shower drizzle")                        // shower drizzle  
  #define W_CODE_500 F("Light rain")                            // light rain
  #define W_CODE_501 F("Moderate rain")                         // moderate rain
  #define W_CODE_502 F("Heavy intensity rain")                  // heavy intensity rain
  #define W_CODE_503 F("Very heavy rain")                       // very heavy rain
  #define W_CODE_504 F("Extreme rain")                          // extreme rain
  #define W_CODE_511 F("Freezing rain")                         // freezing rain
  #define W_CODE_520 F("Light intensity shower rain")           // light intensity shower rain
  #define W_CODE_521 F("Shower rain")                           // shower rain
  #define W_CODE_522 F("Heavy intensity shower rain")           // heavy intensity shower rain
  #define W_CODE_531 F("Ragged shower rain")                    // ragged shower rain
  #define W_CODE_600 F("Light snow")                            // light snow
  #define W_CODE_601 F("Snow")                                  // Snow
  #define W_CODE_602 F("Heavy snow")                            // Heavy snow
  #define W_CODE_611 F("Sleet")                                 // Sleet
  #define W_CODE_612 F("Light shower sleet")                    // Light shower sleet
  #define W_CODE_613 F("Shower sleet")                          // Shower sleet
  #define W_CODE_615 F("Light rain and snow")                   // Light rain and snow
  #define W_CODE_616 F("Rain and snow")                         // Rain and snow
  #define W_CODE_620 F("Light shower snow")                     // Light shower snow
  #define W_CODE_621 F("Shower snow")                           // Shower snow
  #define W_CODE_622 F("Heavy shower snow")                     // Heavy shower snow
  #define W_CODE_701 F("mist")                                  // mist
  #define W_CODE_711 F("Smoke")                                 // Smoke
  #define W_CODE_721 F("Haze")                                  // Haze
  #define W_CODE_731 F("Sand / dust whirls")                    // sand/ dust whirls
  #define W_CODE_741 F("Fog")                                   // fog
  #define W_CODE_751 F("Sand")                                  // sand
  #define W_CODE_761 F("Dust")                                  // dust
  #define W_CODE_762 F("Volcanic ash")                          // volcanic ash
  #define W_CODE_771 F("Squalls")                               // squalls
  #define W_CODE_781 F("Tornado")                               // tornado
  #define W_CODE_800 F("Clear sky")                             // clear sky
  #define W_CODE_801 F("Few clouds: 11-25%")                    // few clouds: 11-25%
  #define W_CODE_802 F("Scattered clouds: 25-50%")              // scattered clouds: 25-50%
  #define W_CODE_803 F("Broken clouds: 51-84%")                 // broken clouds: 51-84%
  #define W_CODE_804 F("Overcast clouds: 85-100%")              // overcast clouds: 85-100%

  // Constants of running text data macroses

  #define SMonth_01  F("january")
  #define SMonth_02  F("february")
  #define SMonth_03  F("march")
  #define SMonth_04  F("april")
  #define SMonth_05  F("may")
  #define SMonth_06  F("june")
  #define SMonth_07  F("july")
  #define SMonth_08  F("august")
  #define SMonth_09  F("september")
  #define SMonth_10  F("october")
  #define SMonth_11  F("november")
  #define SMonth_12  F("december")

  #define SMnth_01   F("jan")
  #define SMnth_02   F("feb")
  #define SMnth_03   F("mar")
  #define SMnth_04   F("apr")
  #define SMnth_05   F("may")
  #define SMnth_06   F("jun")
  #define SMnth_07   F("jul")
  #define SMnth_08   F("aug")
  #define SMnth_09   F("sep")
  #define SMnth_10   F("oct")
  #define SMnth_11   F("nov")
  #define SMnth_12   F("dec")

  #define SDayFull_1 F("Monday")
  #define SDayFull_2 F("Tuesday")
  #define SDayFull_3 F("Wednesday")
  #define SDayFull_4 F("Thursday")
  #define SDayFull_5 F("Friday")
  #define SDayFull_6 F("Saturday")
  #define SDayFull_7 F("Sunday")

  #define SDayShort_1 F("Mon")
  #define SDayShort_2 F("Tue")
  #define SDayShort_3 F("Wed")
  #define SDayShort_4 F("Thu")
  #define SDayShort_5 F("Fri")
  #define SDayShort_6 F("Sat")
  #define SDayShort_7 F("Sun")
  
  #define SDayShrt_1 F("Mo")
  #define SDayShrt_2 F("Tu")
  #define SDayShrt_3 F("We")
  #define SDayShrt_4 F("Th")
  #define SDayShrt_5 F("Fr")
  #define SDayShrt_6 F("Sa")
  #define SDayShrt_7 F("Su")

  #define SDayForm_1 F(" days")          // Start space is mandatory
  #define SDayForm_2 F(" day")
  #define SDayForm_3 F(" days")
 
  #define SHourForm_1 F(" hour")         // Start space is mandatory
  #define SHourForm_2 F(" hours")
  #define SHourForm_3 F(" hours")

  #define SMinuteForm_1 F(" minutes")    // Start space is mandatory
  #define SMinuteForm_2 F(" minute")
  #define SMinuteForm_3 F(" minutes")

  #define SSecondForm_1 F(" seconds")    // Start space is mandatory
  #define SSecondForm_2 F(" second")
  #define SSecondForm_3 F(" seconds")

  // Examples of lines for the "Running Line" that will be filled with these values when the INITIALIZE_TEXTS == 1 flag is set in a_def_hard.h (in line 62)
  // at the initialization step when the firmware is first started on the microcontroller.
  // These examples contain some options for using macros in a running line.

  #define textLine_0  F("##")
  #define textLine_1  F("")
  #define textLine_2  F("There are {C#10FF00}{R01.01.***+} {C#FFFFFF} left until the {C#00D0FF}New year!{S01.12.****#31.12.**** 23:59:59}{E21}")
  #define textLine_3  F("{C#0019FF}New year{C#FFFFFF} in {P01.01.****#4} {C#0019FF}New Year's Eve{C#FFFFFF} ")
  #define textLine_4  F("Happy {C#00D0FF}New {C#0BFF00}{D:yyyy} {C#FFFFFF}year!{S01.01.****#31.01.**** 23:59:59}{E21}")
  #define textLine_5  F("")
  #define textLine_6  F("Show must go on!{C#000002}")
  #define textLine_7  F("{C#FF000F}Ready! {C#000001}Steady!! {C#00FF00}Go!!!{E24}")
  #define textLine_8  F("{C#0003FF}Winter {C#FFFFFF}is coming...!{S01.10.****#01.12.****}")
  #define textLine_9  F("")
  #define textLine_A  F("")
  #define textLine_B  F("")
  #define textLine_C  F("")
  #define textLine_D  F("")
  #define textLine_E  F("")
  #define textLine_F  F("Weather in {C#10FF00}Krasnoyarsk: {C#FFFFFF}{WS} {WT}°C")
  #define textLine_G  F("")
  #define textLine_H  F("Today {D:dd MMMM yyyy} year, {D:dddd}")
  #define textLine_I  F("")
  #define textLine_J  F("")
  #define textLine_K  F("Happy Cristmas!{S25.12.****}")
  #define textLine_L  F("Victory day!{S09.05.**** 7:00#09.05.**** 21:30}")
  #define textLine_M  F("Today {D:dddd dd MMMM}, outdoor {WS}, {WT}°C{E25}")
  #define textLine_N  F("")
  #define textLine_O  F("")
  #define textLine_P  F("")
  #define textLine_Q  F("")
  #define textLine_R  F("")
  #define textLine_S  F("")
  #define textLine_T  F("")
  #define textLine_U  F("Summer is the best time!{S15.05.****#01.09.****}")
  #define textLine_V  F("")
  #define textLine_W  F("")
  #define textLine_X  F("")
  #define textLine_Y  F("Wake up in {P7:30#Z#60#60#12345}!")
  #define textLine_Z  F("-Good morning!")

  // Lines of results of operations and some other messages transmitted
  // from the firmware to the Web application to display

  #define MSG_FILE_DELETED        F("File deleted")
  #define MSG_FILE_DELETE_ERROR   F("File delete error")
  #define MSG_FILE_SAVED          F("File sucessfully saved")
  #define MSG_FILE_SAVE_ERROR     F("File write error")
  #define MSG_FILE_CREATE_ERROR   F("File create error")
  #define MSG_FOLDER_CREATE_ERROR F("Unable to create folder for images storage")
  #define MSG_FILE_LOADED         F("File loaded")
  #define MSG_FILE_LOAD_ERROR     F("File read error")
  #define MSG_FILE_NOT_FOUND      F("File not found")
  #define MSG_FOLDER_NOT_FOUND    F("Folder with images storage not found")
  #define MSG_BACKUP_SAVE_ERROR   F("Error while create backup file")
  #define MSG_BACKUP_SAVE_OK      F("Backup file sucessfully saved")
  #define MSG_BACKUP_LOAD_ERROR   F("Unable to load backup file")
  #define MSG_BACKUP_LOAD_OK      F("Settings were sucessfully loaded")
  #define MSG_OP_SUCCESS          F("Sucessfully applied")

  #define MODE_NIGHT_CLOCK        F("Night clock")
  #define MODE_CLOCK              F("Clock")
  #define MODE_RUNNING_TEXT       F("Running text")
  #define MODE_LOAD_PICTURE       F("Load picture")
  #define MODE_DRAW               F("Draw")
  #define MODE_DAWN               F("Dawn")

  #define WTR_LANG_YA             F("en")      // Yandex.Weather seems know only "ru" and "en"
  #define WTR_LANG_OWM            F("en")      // OpenWeatherMap - 2-letter language code "ru", "en", "de", "fr", "it" and so on. For unknown code returns same as "en"

#endif

// ============================================== ESP ============================================== 

#if (LANG == 'ESP')
  #define UI F("ESP")
  
  // Lista y orden de los efectos, transmitido al interfaz web. 
  // El orden de los nombres de los efectos en la lista debe coincidir con la lista de los efectos determinada en el archivo a_def_soft.h 
  // en las lineas desde 119 
  
#if (USE_SD == 1)                      
    #define EFFECT_LIST F("Reloj,Lampara,Nevada,Cubo,Arco iris,Paintball,Fuego,The Matrix,Bolas,Lluvia de estrellas,Confeti," \
                          "Ruido de color,Nubes,Lava,Plasma,Arco iris,Pavo real,Cebra,Bosque ruidoso,Surf,Cambio de color," \
                          "Luciérnagas,Remolino,Ciclón,Parpadeo,Luces del Norte,Sombras,Laberinto,Serpiente,Tetris,Arkanoid," \
                          "Paleta,Espectro,Senos,Vyshyvanka,Lluvia,Chimenea,Flechas,Animación,Clima,Patrones,Rubik,Estrellas,Cortina,Tráfico," \
                          "Diapositivas,Amanecer,Tarjeta SD")
  #else
    #define EFFECT_LIST F("Reloj,Lámpara,Nevada,Cubo,Arco iris,Paintball,Fuego,The Matrix,Bolas,Lluvia de estrellas,Confeti," \
                          "Ruido de color,Nubes,Lava,Plasma,Arco iris,Pavo real,Cebra,Bosque ruidoso,Surf,Cambio de color," \
                          "Luciérnagas,Remolino,Ciclón,Parpadeo,Luces del Norte,Sombras,Laberinto,Serpiente,Tetris,Arkanoid," \
                          "Paleta,Espectro,Senos,Vyshyvanka,Lluvia,Chimenea,Flechas,Animación,Clima,Patrones,Rubik,Estrellas,Cortina,Tráfico," \
                          "Diapositivas,Amanecer")
  #endif

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
  #define ALARM_SOUND_LIST   F("One Step Over,In the Death Car,La Trompeta Esta Llamando,Farola,Mister Sandman,Ataud,Banana Phone,Carol of the Bells")
  
  // Lista de los sonidos para la caja combo "El Sonido del Ocaso" en la aplicacion en el movil
  #define DAWN_SOUND_LIST    F("Aves,Tormenta,Surf,Lluvia,Arroyo,Mantra,La Petite Fille De La Mer")
  
  // Lista de los sonidos `para el macro {A} de letrero de desplazamiento
  #define NOTIFY_SOUND_LIST  F("Piece Of Сake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board," \
                               "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal," \
                               "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3")
  
  #endif

  // Lista de los nombres de animaciones. Las animaciones estan definidos en el archivo 'animation.ino'
  #define LANG_IMAGE_LIST F("Corazon,Mario,Tiempo")   

  // Lista de nombres de los patrones
  #define LANG_PATTERNS_LIST F("Zigzag,Notas musicales,Rombo,Corazón,Arbol de Navidad,Celda,Carita sonriente,Zigzag,Rayas,Ondas,Escamas,Cortinas,Trenza,Copo de nieve,Cuadrados,Grecia,Círculos,Rollo," \
                               "Patron 1,Patron 2,Patron 3,Patron 4,Patron 5,Patron 6,Patron 7,Patron 8,Patron 9,Patron 10,Patron 11,Patron 12,Patron 13,Patron 14")

  // Condiciones del tiempo por Yandex
  #define Y_CODE_01 F("parcialmente nublado, lluvia ligera")             // cloudy, light rain
  #define Y_CODE_02 F("parcialmente nublado, nieve ligera")              // cloudy, light snow
  #define Y_CODE_03 F("parcialmente nublado, nieve ligera con lluvia")   // cloudy, wet snow
  #define Y_CODE_04 F("parcialmente nublado")                            // partly cloudy
  #define Y_CODE_05 F("parcialmente nublado, lluvia")                    // partly cloudy, rain
  #define Y_CODE_06 F("parcialmente nublado, nieve")                     // partly cloudy, show
  #define Y_CODE_07 F("parcialmente nublado, chubascos de nieve")        // partly cloudy, wet snow
  #define Y_CODE_08 F("tormenta de nieve")                               // snowstorm
  #define Y_CODE_09 F("niebla")                                          // fog
  #define Y_CODE_10 F("Principalmente nublado")                          // overcast
  #define Y_CODE_11 F("nublado, a veces lluvia")                         // overcast, light rain 
  #define Y_CODE_12 F("nublado, a veces nieve")                          // overcast, light snow
  #define Y_CODE_13 F("nublado con nieve y lluvia ocasionales")          // overcast, wet snow 
  #define Y_CODE_14 F("nublado, lluvia")                                 // overcast, rain
  #define Y_CODE_15 F("nublado, nieve y lluvia")                         // overcast, wet snow
  #define Y_CODE_16 F("nublado, nieve")                                  // overcast, show
  #define Y_CODE_17 F("nublado, lluvia con truenos")                     // overcast, thunderstorm withrain
  #define Y_CODE_18 F("despejado")                                       // clear

  // Condiciones del tiempo por OpenWeatherMap
  #define W_CODE_200 F("Tormenta, lluvia ligera")                        // thunderstorm with light rain
  #define W_CODE_201 F("Lluvia con truenos")                             // thunderstorm with rain
  #define W_CODE_202 F("Tormenta, chubascos")                            // thunderstorm with heavy rain
  #define W_CODE_210 F("pequeña tormenta")                               // light thunderstorm
  #define W_CODE_211 F("Tormenta")                                       // thunderstorm
  #define W_CODE_212 F("Tormenta fuerte")                                // heavy thunderstorm
  #define W_CODE_221 F("tormentas eléctricas intermitentes")             // ragged thunderstorm
  #define W_CODE_230 F("Tormenta, lluvia ligera")                        // thunderstorm with light drizzle
  #define W_CODE_231 F("Tormenta con lluvia")                            // thunderstorm with drizzle
  #define W_CODE_232 F("Tormenta con fuertes lluvias")                   // thunderstorm with heavy drizzle
  #define W_CODE_300 F("lluvia fina")                                    // light intensity drizzle
  #define W_CODE_301 F("lluvia torrencial")                              // drizzle
  #define W_CODE_302 F("Lluvia Pesada")                                  // heavy intensity drizzle
  #define W_CODE_310 F("Lluvia pequeña")                                 // light intensity drizzle rain
  #define W_CODE_311 F("lluvia torrencial")                              // drizzle rain
  #define W_CODE_312 F("Lluvia Pesada")                                  // heavy intensity drizzle rain
  #define W_CODE_313 F("Aguacero, lluvia y llovizna")                    // shower rain and drizzle
  #define W_CODE_314 F("Fuerte aguacero, lluvia y llovizna")             // heavy shower rain and drizzle
  #define W_CODE_321 F("lluvia torrencial")                              // shower drizzle  
  #define W_CODE_500 F("Lluvia pequeña")                                 // light rain
  #define W_CODE_501 F("lluvia moderada")                                // moderate rain
  #define W_CODE_502 F("Ducha")                                          // heavy intensity rain
  #define W_CODE_503 F("Lluvia torrencial")                              // very heavy rain
  #define W_CODE_504 F("Lluvia torrencial")                              // extreme rain
  #define W_CODE_511 F("Granizo")                                        // freezing rain
  #define W_CODE_520 F("Lluvia pequeña")                                 // light intensity shower rain
  #define W_CODE_521 F("lluvia torrencial")                              // shower rain
  #define W_CODE_522 F("Lluvia Pesada")                                  // heavy intensity shower rain
  #define W_CODE_531 F("Lluvia a veces")                                 // ragged shower rain
  #define W_CODE_600 F("nieve pequeña")                                  // light snow
  #define W_CODE_601 F("Nieve")                                          // Snow
  #define W_CODE_602 F("Nevada")                                         // Heavy snow
  #define W_CODE_611 F("Fango")                                          // Sleet
  #define W_CODE_612 F("nieve ligera")                                   // Light shower sleet
  #define W_CODE_613 F("aguacero, nieve")                                // Shower sleet
  #define W_CODE_615 F("Nieve humeda")                                   // Light rain and snow
  #define W_CODE_616 F("lluvia con nieve")                               // Rain and snow
  #define W_CODE_620 F("nevada ligera")                                  // Light shower snow
  #define W_CODE_621 F("nevada, ventisca")                               // Shower snow
  #define W_CODE_622 F("fuerte nevada")                                  // Heavy shower snow
  #define W_CODE_701 F("Niebla")                                         // mist
  #define W_CODE_711 F("Bruma")                                          // Smoke
  #define W_CODE_721 F("Niebla ligera")                                  // Haze
  #define W_CODE_731 F("remolinos de polvo")                             // sand/ dust whirls
  #define W_CODE_741 F("Bruma")                                          // fog
  #define W_CODE_751 F("torbellinos de arena")                           // sand
  #define W_CODE_761 F("Remolinos de polvo")                             // dust
  #define W_CODE_762 F("ceniza volcánica")                               // volcanic ash
  #define W_CODE_771 F("viento chubasco")                                // squalls
  #define W_CODE_781 F("Tornado")                                        // tornado
  #define W_CODE_800 F("Despejado")                                      // clear sky
  #define W_CODE_801 F("Parcialmente nublado")                           // few clouds: 11-25%
  #define W_CODE_802 F("Parcialmente nublado")                           // scattered clouds: 25-50%
  #define W_CODE_803 F("Nublado con claros")                             // broken clouds: 51-84%
  #define W_CODE_804 F("Principalmente nublado")                         // overcast clouds: 85-100%

  // Константы вывода даты в бегущей строке

  #define SMonth_01  F("enero")
  #define SMonth_02  F("febrero")
  #define SMonth_03  F("marzo")
  #define SMonth_04  F("abril")
  #define SMonth_05  F("mayo")
  #define SMonth_06  F("junio")
  #define SMonth_07  F("julio")
  #define SMonth_08  F("agosto")
  #define SMonth_09  F("septiembre")
  #define SMonth_10  F("octubre")
  #define SMonth_11  F("noviembre")
  #define SMonth_12  F("diciembre")

  #define SMnth_01   F("ene")
  #define SMnth_02   F("feb")
  #define SMnth_03   F("mar")
  #define SMnth_04   F("abr")
  #define SMnth_05   F("may")
  #define SMnth_06   F("jun")
  #define SMnth_07   F("jul")
  #define SMnth_08   F("ago")
  #define SMnth_09   F("sep")
  #define SMnth_10   F("oct")
  #define SMnth_11   F("nov")
  #define SMnth_12   F("dic")

  #define SDayFull_1 F("lunes")
  #define SDayFull_2 F("martes")
  #define SDayFull_3 F("miercoles")
  #define SDayFull_4 F("jueves")
  #define SDayFull_5 F("viernes")
  #define SDayFull_6 F("sabado")
  #define SDayFull_7 F("domingo")

  #define SDayShort_1 F("lun")
  #define SDayShort_2 F("mart")
  #define SDayShort_3 F("mier")
  #define SDayShort_4 F("juev")
  #define SDayShort_5 F("vier")
  #define SDayShort_6 F("sab")
  #define SDayShort_7 F("dom")
  
  #define SDayShrt_1 F("L")
  #define SDayShrt_2 F("M")
  #define SDayShrt_3 F("X")
  #define SDayShrt_4 F("J")
  #define SDayShrt_5 F("V")
  #define SDayShrt_6 F("S")
  #define SDayShrt_7 F("D")


  #define SDayForm_1 F(" dias")         // El espacio al principio es obligatorio
  #define SDayForm_2 F(" dia")
  #define SDayForm_3 F(" dia")
 
  #define SHourForm_1 F(" hora")        // El espacio al principio es obligatorio
  #define SHourForm_2 F(" hora")
  #define SHourForm_3 F(" horas")

  #define SMinuteForm_1 F(" minutos")   // El espacio al principio es obligatorio
  #define SMinuteForm_2 F(" minuto")
  #define SMinuteForm_3 F(" minuto")

  #define SSecondForm_1 F(" segunda")   // El espacio al principio es obligatorio
  #define SSecondForm_2 F(" segunda")
  #define SSecondForm_3 F(" segunda")

  // Ejemplos de frases para "Letrero de desplazamiento", que van a ser rellenados con esos parametros si esta marcado INITIALIZE_TEXTS == 1 en a_def_hard.h (en linea 62)
  // en el paso de inicializacion al primer comienzo del firmware del microcontrolador.
  // Estos ejemplos contienes algunos variantes del uso de los macros en el letrero de desplazamiento. 
  
  #define textLine_0  F("##")
  #define textLine_1  F("¡Todo estará bien!")
  #define textLine_2  F("Hasta {C#00D0FF}La Navidad {C#FFFFFF}se queda {C#10FF00}{R01.01.***+}{S01.12.****#31.12.**** 23:59:59}{E21}")
  #define textLine_3  F("Hasta {C#0019FF}La Navidad{C#FFFFFF} {P01.01.****#4}")
  #define textLine_4  F("Feliz {C#00D0FF}Navidad {C#0BFF00}{D:yyyy} {C#FFFFFF}!!!{S01.01.****#31.01.**** 23:59:59}{E21}")
  #define textLine_5  F("En {C#10FF00}Madrid {C#FFFFFF}{WS} {WT}°C")
  #define textLine_6  F("¡Show must go on!{C#000002}")
  #define textLine_7  F("{C#FF000F}¡Habracadabra! {C#00FF00}¡¡¡Bum!!!{E24}")
  #define textLine_8  F("Prepárense, gente, se acerca {C#FF0300}el verano!{S01.01.****#10.04.****}")
  #define textLine_9  F("¿Que {C#0081FF}pasa{C#FFFFFF} Hombre?")
  #define textLine_A  F("{C#000001}¡Uno! ¡¡Dos!! Tres!!! {C#33C309}Tira,{B#000000}{C#000001} ¡¡¡Otra vez!!!")
  #define textLine_B  F("Esa linia no tiene nada...")
  #define textLine_C  F("¡Que bonito!")
  #define textLine_D  F("Hola, hola caracola...")
  #define textLine_E  F("Hay algo inteligente en esto....")
  #define textLine_F  F("En {C#10FF00}Madrid {C#FFFFFF}{WS} {WT}°C")
  #define textLine_G  F("Eso es todo, {C#FF0000}hombre{C#FFFFFF}!..")
  #define textLine_H  F("Hoy {D:dd MMMM yyyy}, dia {D:dddd}")
  #define textLine_I  F("Pareza - motor de progreso")
  #define textLine_J  F("")
  #define textLine_K  F("")
  #define textLine_L  F("")
  #define textLine_M  F("Ahora {D:dddd dd MMMM}, temperatura {WS}, {WT}°C{E25}")
  #define textLine_N  F("")
  #define textLine_O  F("")
  #define textLine_P  F("")
  #define textLine_Q  F("")
  #define textLine_R  F("¿Porque? ¡Por que!")
  #define textLine_S  F("")
  #define textLine_T  F("")
  #define textLine_U  F("")
  #define textLine_V  F("")
  #define textLine_W  F("")
  #define textLine_X  F("")
  #define textLine_Y  F("Levantote desde{P7:30#Z#60#60#12345}!")
  #define textLine_Z  F("¡BUENOS DIAS!")

  // Líneas de resultados de ejecución de operaciones y algunos otros mensajes transmitidos
  // del firmware a la aplicación web para mostrar
  
  #define MSG_FILE_DELETED        F("Archivo eliminado")
  #define MSG_FILE_DELETE_ERROR   F("Error de eliminación de archivos")
  #define MSG_FILE_SAVED          F("Archivo guardado")
  #define MSG_FILE_SAVE_ERROR     F("Error al escribir en el archivo")
  #define MSG_FILE_CREATE_ERROR   F("Error de creación de archivo")
  #define MSG_FOLDER_CREATE_ERROR F("Error al crear una carpeta para almacenar imágenes")
  #define MSG_FILE_LOADED         F("Archivo descargado")
  #define MSG_FILE_LOAD_ERROR     F("Error de lectura de archivo")
  #define MSG_FILE_NOT_FOUND      F("Archivo no encontrado")
  #define MSG_FOLDER_NOT_FOUND    F("Carpeta de imágenes no encontrada")
  #define MSG_BACKUP_SAVE_ERROR   F("No se pudo hacer una copia de seguridad de la configuración")
  #define MSG_BACKUP_SAVE_OK      F("Configuraciones respaldadas creadas")
  #define MSG_BACKUP_LOAD_ERROR   F("Error al cargar la copia de seguridad de la configuración")
  #define MSG_BACKUP_LOAD_OK      F("Configuración de copia de seguridad restaurada")
  #define MSG_OP_SUCCESS          F("Los ajustes se han guardado")

  #define MODE_NIGHT_CLOCK        F("Reloj de la noche")
  #define MODE_CLOCK              F("Reloj")
  #define MODE_RUNNING_TEXT       F("Letras desplasamento")
  #define MODE_LOAD_PICTURE       F("Carga de imagen")
  #define MODE_DRAW               F("Dibujo")
  #define MODE_DAWN               F("Amanecer")

  #define WTR_LANG_YA             F("es")      // Yandex.Weather: parece entender solo "ru" y "en"
  #define WTR_LANG_OWM            F("es")      // OpenWeatherMap = 2 - [código de idioma alfabético-ru,en,de,fr, it y así sucesivamente. Si el idioma no sabe - devuelve tanto para en
  
#endif
// ================================================================================================= 

#if !defined(EFFECT_LIST) && !defined(EFFECT_LIST_LANG)
#error Не определен язык интерфейса в файле 'a_def_lang.h'
#endif
