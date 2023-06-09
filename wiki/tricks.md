# Три шага к успеху. Некоторые особенности настроек...

Если на вашей матрице наблюдаются случайные цветные всполохи, срывы синхронизации, когда часы или бегущая строка "рассыпается",
выполните следующие шаги, которые, вероятно, смогут устранить проблему

## Шаг1

   Описанные эффекты возникают при превышении выходного напряжения блока питания выше 4.8-5.2 вольта. 
   Обычно китайские блоки питания имеют заводскую настройку выходного напряжение 5.4-5.6 вольт. 
   Для проверки - не подключайте +5 вольт от блока питания к микроконтроллеру совсем, питание подавайте на матрицу непосредственно. 
   "Землю" микроконтроллера и ленты соедините. Подключите сигнальный пин микроконтроллера ко входу DIN ленты. 
   Подключите микроконтроллер к компьютеру через USB (питание будет поступать отсюда). 
   Если после проделанных манипуляции проблема исчезла, регулируем напряжение на блоке питания:

   1. Для блоков питания в жестяном корпусе - возле гребенки выходных контактов есть "крутилка" - переменный резистор.  
      Подключаем к выходным разъемам вольтметр и маленькой отверткой крутим резистор влево, установив выходное напряжение в 4.8 вольта  
   2. Если ваш блок питания не имеет переменного резистора - возможно на его место впаян постоянный резистор - найдите его и земените переменным резистором.
      Далее выполните настройку из п.1
   3. Если вы не можете найти резистор или его регулировка не позволяет установить 4.8 вольта - переходим к **Шаг 2**

## Шаг 2
  
   Если ваш блок питания в пластиковой коробке и у вас нет возможности/желания разбирать его и выполнять рекомендации из **Шаг 1**
   или если вы используете схему с MOSFET для отключения питающего напряжения матрицы, попробуйте применить схему с
   включением кремниевого диода между минусом БП и пином GND микроконтроллера. Это позволит "приподнять" "землю" микроконтроллера
   на 0.6-0.8 вольт относительно уровня "земли" матрицы, что автоматически поднимает уровень логического сигнала на управляющем пине микроконтроллера / ленты.
   Лучшее согласование логических уровней может устранить всполохи на матрице.  
   Такое включение позволяет панели стабильно работать при напряжении с блока питания практически от 4.2 до 6.5 вольт без необходимости 
   регулировки выходного напряжения блока питания.  
   Если проделанные манипуляции не устранили проблему - пробуем рекомендации из **Шаг 3**

## Шаг 3

   Возможно устранить проблему всполохов и "рассыпания" текста поможет программная корректировка управляющего сигнала, подающегося на ленту / матрицу.  
   В библиотеке FastLED найдите файл **clockless_esp8266.h**. Он может быть расположен в одной из следующих папок:  

```
C:\Program Files (x86)\Arduino\libraries\FastLED\platforms\esp\8266\clockless_esp8266.h
C:\Users\<user>\Documents\Arduino\libraries\FastLED\platforms\esp\8266\clockless_esp8266.h
```

   и в строке 19 файла **clockless_esp8266.h** заменить 

```
template <int DATA_PIN, int T1, int T2, int T3, EOrder RGB_ORDER = RGB, int XTRA0 = 0, bool FLIP = false, int WAIT_TIME = 50>
```

   на

```
template <int DATA_PIN, int T1, int T2, int T3, EOrder RGB_ORDER = RGB, int XTRA0 = 0, bool FLIP = false, int WAIT_TIME = 10>
                                                                                                              ------------^
```

  Во всех файлах скетча ищем:
  Строку `#include <FastLED.h>` (это в одном файле только) и перед ней, выше пишем: 
```
  #define FASTLED_ALLOW_INTERRUPTS 0 
```
  Далее ищем строку **"FastLED.show()"** и после каждого вхождения с новой строки пишем: `yield();`

  После выполнения этих исправлений пересоберите проект и загрузите прошивку. С большой долей вероятности это решит проблему "мерцающих" пикселей на матрице.

   Однако этот метод с большой долей вероятности может привести к "отваливанию" WiFi соединения микроконтроллера с сетью и периодическим перезагрузкам, 
   обычно с сообщением о причине "Soft WDT" или "wdt reset". Обычно перезагрузка происходит через некоторое время после пропадания соединения с сетью.  

   Пробуйте подобрать значение ```WAIT_TIME``` от 5 до 50, соблюдая баланс между стабильностью работы микроконтроллера и стабильностью отображения эффектов.

   Вам решать - критично это для вас или нет. Что важнее - постоянное соединение с программой на телефоне и с сетью вообще или более стабильное отображение
   текста, часов и эффектов на матрице. Если у вас настроен выход в интернет через локальную сеть, с которой соединяется микроконтроллер - он после перезагрузки
   получит время, погоду и все что ему необходимо и продолжит работу. Внешне это выглядит как периодическое кратковременное пропадание эффектов с матрицы и 
   секунд через 10 всё восстанавливается.

   Как обычно - решать вам.

