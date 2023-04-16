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
// В этом файле есть поддержка языков Русского (сигнатура 'RUS') и Английского (сигнатура 'ENG')
// 
#ifndef LANG
#define LANG 'RUS'
#endif

// ============================================== RUS ============================================== 

#if (LANG == 'RUS')
  #define UI F("RUS")
  
  // Список и порядок эффектов, передаваемый в Web-интерфейс. 
  // Порядок имен эффектов в списке должен соответствовать списку эффектов, определенному в файле a_def_soft.h 
  // в строках, начиная с 119 
  
  #if (USE_SD == 1)                      
    #define EFFECT_LIST F("Часы,Лампа,Снегопад,Кубик,Радуга,Пейнтбол,Огонь,The Matrix,Шарики,Звездопад,Конфетти," \
                          "Цветной шум,Облака,Лава,Плазма,Радужные переливы,Павлин,Зебра,Шумящий лес,Морской прибой,Смена цвета," \
                          "Светлячки,Водоворот,Циклон,Мерцание,Северное сияние,Тени,Лабиринт,Змейка,Тетрис,Арканоид," \
                          "Палитра,Спектрум,Синусы,Вышиванка,Дождь,Камин,Стрелки,Анимация,Погода,Жизнь,Узоры,Рубик,Звёзды,Штора,Трафик," \
                          "Слайды,Рассвет,SD-Карта")
  #else
    #define EFFECT_LIST F("Часы,Лампа,Снегопад,Кубик,Радуга,Пейнтбол,Огонь,The Matrix,Шарики,Звездопад,Конфетти," \
                          "Цветной шум,Облака,Лава,Плазма,Радужные переливы,Павлин,Зебра,Шумящий лес,Морской прибой,Смена цвета," \
                          "Светлячки,Водоворот,Циклон,Мерцание,Северное сияние,Тени,Лабиринт,Змейка,Тетрис,Арканоид," \
                          "Палитра,Спектрум,Синусы,Вышиванка,Дождь,Камин,Стрелки,Анимация,Погода,Жизнь,Узоры,Рубик,Звёзды,Штора,Трафик," \
                          "Слайды,Рассвет")
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
  
#endif

// ============================================== ENG ============================================== 

#if (LANG == 'ENG')
  #define UI F("ENG")

  // List of effects names and order in sequence trensferring to Web-application. 
  // Order of Names MUST correspont to order of effect ID definitions declared in a_def_soft.h file
  // starting from line 119
  
  #if (USE_SD == 1)                      
    #define EFFECT_LIST F("Clock,Lamp,Showfall,Cube,Rainbow,Paintball,Fire,The Matrix,Balls,Startfall,Confetti," \
                          "Color noise,Clouds,Lava,Plasma,Iridescent,Peacock,Zebra,Noisy forest,Sea surf,Color change," \
                          "Fireflies,Whirlpool,Cyclone,Flicker,Northern lights,Shadows,Maze,Snake,Tetris,Arkanoid," \
                          "Palette,Spectrum,Sinuses,Embroidery,Rain,Fireplace,Arrows,Animation,Weather,Life,Patterns,Rubic,Stars," \
                          "Curtain,Traffic,Slides,Dawn,SD-card")
  #else
    #define EFFECT_LIST F("Clock,Lamp,Showfall,Cube,Rainbow,Paintball,Fire,The Matrix,Balls,Startfall,Confetti," \
                          "Color noise,Clouds,Lava,Plasma,Iridescent,Peacock,Zebra,Noisy forest,Sea surf,Color change," \
                          "Fireflies,Whirlpool,Cyclone,Flicker,Northern lights,Shadows,Maze,Snake,Tetris,Arkanoid," \
                          "Palette,Spectrum,Sinuses,Embroidery,Rain,Fireplace,Arrows,Animation,Weather,Life,Patterns,Rubic,Stars," \
                          "Curtain,Traffic,Slides,Dawn")                          
  #endif
  
  // ****************** SOUNDS OF ALARMS, DAWN and RUNNING TEXT MACRO {A} ********************
  
  #if (USE_MP3 == 1)
  // SD card in MP3 player (DFPlayer) contains under root folder three folders with names - "1","2" и "3"
  // Folder "1" contains MP3 sound files, playing on event of alarm
  // Folder "2" contains MP3 sound files, playing on event of dawn
  // Folder "3" contains MP3 sound files, playing on event ow shoe running text containing {A} macros
  // DFPlayer does not have an ability to retrieve names of files in folders, only can read the count of files in each folder.
  // Command for play sound means - play file with index (number) N from folder M
  // Indecies of files are defined by file allocation table (FAT) of SD-card, which defining at time of copy files into SD-card folder on clear (just formatted) SD-card
  // So, file was written into folder first of all gets index 1, next - 2 and so on and do not depends on names of files
  // These definitions contain names of sounds displayed in comboboxes inside application in order that must match to indecies of files were written into folders.
  
  // List of sounds for "Alarm sound" combobox in phone application or Web-interface
  #define ALARM_SOUND_LIST   F("One Step Over,In the Death Car,Horn call,Lighthouse,Mister Sandman,Сasket,Banana Phone,Carol of the Bells")
  
  // List of sounds for "Dawn sound" combobox in phone application or Web-interface
  #define DAWN_SOUND_LIST    F("Birds,Thunderstorm,Surf,Rain,Creek,The Mantra,La Petite Fille De La Mer")
  
  // List of sounds for {A} macros of running text in phone application or Web-interface
  #define NOTIFY_SOUND_LIST  F("Piece Of Сake,Swiftly,Pristine,Goes Without Saying,Inflicted,Eventually,Point Blank,Spring Board," \
                               "To The Point,Serious Strike,Jingle Bells,Happy New Year,Christmas Bells,Door Knock,Alarm Signal," \
                               "Viber Message,Viber Call,Text Message,Old Clock 1,Old Clock 2,Old Clock 3")
  
  #endif


  // List of animation names. Animations are defined in file 'animation.ino'
  #define LANG_IMAGE_LIST F("Heart,Mario,Weather")   
  
  // List names of Patterns effect
  #define LANG_PATTERNS_LIST F("Zigzag,Noteы,Rhomb,Heart,Fir tree,Cells,Smile,Zigzag-2,Streaks,Waves,Scales,Curtain,Wicker,Snowflake,Squares,Greece,Circles,Roll," \
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

  #define textLine_0  F("##")
  #define textLine_1  F("")
  #define textLine_2  F("There are {C#10FF00}{R01.01.***+} {C#FFFFFF} left until the {C#00D0FF}New year!{S01.12.****#31.12.**** 23:59:59}{E21}")
  #define textLine_3  F("{C#0019FF}New year{C#FFFFFF} in {P01.01.****#4} {C#0019FF}Нового года{C#FFFFFF} ")
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

#endif

// ================================================================================================= 

#ifndef EFFECT_LIST
#error Не определен язык интерфейса в файле 'a_def_lang.h'
#endif
