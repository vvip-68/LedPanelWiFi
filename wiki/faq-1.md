## FAQ
### Основные вопросы
#### **В**: **Как скачать с этого грёбаного сайта?**  
**О**: На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка  "**<> Code**", вот её жми, там будет "**Download ZIP**"


#### **В**: **Скачался какой то файл .zip, куда его теперь?**  
**О**: Это архив. Можно открыть стандартными средствами Windows, но думаю у всех на компьютере установлен WinRAR, архив нужно правой кнопкой и извлечь.


#### **В**: **Я совсем новичок! Что мне делать с Arduino, где взять все программы?**  
**О**: Читай и смотри видос http://alexgyver.ru/arduino-first/


#### **В**: **Вылетает ошибка загрузки / компиляции!**  
**О**: Читай тут: https://alexgyver.ru/arduino-first/#step-5

-----

### Вопросы по этому проекту

#### **В**: **Эй чувак! У тебя проект не компилится. Ты файл FastLed.h в проект забыл включить. Выложи!**  
**О**: Это стандартная библиотека для FastLED для управления адресными светодиодами. Идите в менеджер библиотек и установите ее. Версию библиотеки выбирайте 3.6.0 или 3.7.0; Или скачайте с [сайта производителя](https://github.com/FastLED/FastLED).  

-----

#### **В**: **Собрал, использую NodeMCU/Wemos. Ничего не работает! Мигает один или несколько светодиодов в начале матрицы. И всё.**  
**О**: Производители разных плат (NodeMCU, Wemos) могут использовать различные схемы соединения контактов микроконтроллера ESP8266 к выводам макетной платы. 
       Обычно используемый в проекте пин вывода на ленту приходится или на пин D2 или на пин D4. Для проверки не подключайте сигнальный провод матрицы к микроконтроллеру, 
       вместо этого через резистор коснитесь вывода D2 или D4 пина микроконтроллера. Большая вероятность что матрица заработает с тем или иным вариантом подключения.

-----

#### **В**: **Не компилируется. Выбрана плата "голая ESP8266-12E". Сообщение об ошибке: "D4 was not declared in this scope."**  
**О**: Очевидно производители библиотеки для "голой ESP8266-12E" не определили данную константу. Используйте вместо константы D4 числовое определение пина для вашей платы или 
       выполните компиляцию проекта для плат NodeMCU или WeMos d1 mini. Рекомендуется в качестве платы назначения указывать **ModeMCU v1.0 (ESP-12E Module)** для плат NodeMCU и **LOLIN(WEMOS) D1 mini (clone)** для плат Wemos d1 mini.

-----

#### **В**: **Не компилируется. В сообщении об ошибке содержатся сведения о дублирующихся библиотеках.**  
**О**: В вашей среде установлено две версии одной и той же библиотеки. Обычно это библиотека FastLED - одна версия находится в папке установки среды Arduino 
       (например в "C:\Program Files (x86)\Arduino\libraries\"), другая - в папке документов пользователя (например "C:\Users\vvip-68\Documents\Arduino\libraries\"). 
       Удалите одну из версий библиотек, убедитесь что оставшаяся библиотека имеет версию 3.6.0 или 3.7.0 и попробуйте скомпилировать скетч снова.

-----

#### **В**: **Не компилируется. В сообщении об ошибке что-то про несоответствие типов.**  
**О**: Обычно такая ситуация возникает в двух случаях:
- выбрана неверная плата. Используйте **NodeMCU 1.0 (ESP-12E Module)** или **LOLIN(WEMOS) D1 mini (clone)**. Под эти платы проект собирается, под другие, возможно, нужна модификация кода.
- установлена устаревшая версия библиотек поддержки плат - например для ESP8266 версия библиотеки 2.4.2. 
  Данный проект использует библиотеки для плат ESP8266 версии **3.1.2.**.  
  Для версии микроконтроллера ESP32 используйте версию ядра **2.0.14**.  
  Изучите инструкцию в комментариях в начале основного файла проекта. Обновите библиотеки поддержки плат (ядро) до этой версии.
  С другими версиями ядра, в том числе и более новыми, проект может работать нестабильно, вообще не работать или даже не компилироваться.

-----

#### **В**: **Собрал, использую NodeMCU (Wemos D1 mini). Эффекты работают, но нестабильно. Случайные вспышки на матрице. Буквы бегущей строки прыгают.**  
**О**: Для начала убедитесь, что вы используете рекомендованные версии ядра системы - ***3.1.2*** для **ESP8266**, ***2.0.14*** для **ESP32**.  
       Если версии ядра установлены правильные, но проблема всё еще наблюдается - попробуйте рекомендации изложенные 
       в статье [Три шага к успеху](https://github.com/vvip-68/LedPanelWiFi/wiki/Три-шага-к-успеху.-Некоторые-особенности-настроек...)

-----

#### **В**: **Подскажите, что не так... C подключением через точку доступа всё исправно работает, а при попытке подключиться к локальной сети не могу законнектиться через Web-интерфейс. В чем может быть проблема?**  
**О**: Проблема может быть в неправильно указанном статическом адресе / параметрах сети в прошивке. В скетче по умолчанию используется адрес в сети 192.168.**0**.xxx. 

Ваш WiFi роутер в зависимости от настроек может создавать сеть в другом диапазоне. Чаще всего это 192.168.**1**.xxx или 192.168.**100**.xxx; 
Проверьте какую сеть создает ваш роутер и укажите в скетче и при подключении приложения к сети именно эту сеть - назначьте устройству 
IP из **этой** сети. Допустим, роутер имеет IP-адрес 192.168.**1**.1; Укажите, что ваше устройство должно получить адрес 192.168.**1**.100  

Следующий вариант - ваш роутер имеет адрес, отличный от стандартного - 1-й адрес в сети, например вместо адреса 192.168.0.**1** 
роутер работает на адресе 192.168.0.**254**. Исправьте в скетче в разделе настроек сети параметр Gateway (GTW):  
параметр, указывающий какую подсеть использует роутер задается в настройках в файле **a_def_soft.h** в строке 31.

```
#ifndef GTW
#define GTW     1  // Последняя цифра в IP адресе роутера. Обычно IP роутера 192.168.0.*1*, но некоторые роутеры имеют адрес 192.168.0.100 или 192.168.0.254
#endif             // Тогда здесь вместо *1* должно быть 100 или 254 соответственно
```
-----

#### **В**: **Устройство создает точку доступа, телефон к ней подключается. В браузере ввожу IP адрес панели 192.168.4.1, но соединение не происходит. Что я делаю не так?**  
**О**: Некоторые телефоны не могут передавать данные через точку доступа, пока в них активен мобильный интернет. 
       Все передаваемые данные отправляются в интернет, вместо передачи их в точку доступа. В настройках телефона выключите мобильный интернет ("Мобильные данные"). 
       После этого телефон из браузера должен подключиться к устройству.  

-----

#### **В**: **Устройство подключается к сети, получает адрес - это видно в логах. Телефон к устройству подключается. В приложении ввожу IP адрес, полученный устройством - тот, что выводится в логе, но соединение не происходит. Что я делаю не так?**  
**О**: Некоторые роутеры  (например роутеры ASUS, в частности Asus RT-AC58U), имеют в своих настройках параметр "Изоляция точки доступа", он же - "Режим изоляции клиентов". 
       По умолчанию эта настройка включена. Отключите ее. После этого браузер должен подключиться к смартфону.  

-----

#### **В**: **В скетче есть настройки который задают имя и пароль к локальной сети. Указываю, но к сети даже не пытается подключиться В чем дело?**  
```
#define NETWORK_SSID ""  // Имя WiFi сети - пропишите здесь или задайте из приложения настроек Web-приложения в браузере   
#define NETWORK_PASS ""  // Пароль для подключения к WiFi сети - пропишите здесь или задайте из приложения настроек Web-приложения в браузере
```
**О**: Эти настройки определяют параметры доступа к сети по умолчанию, которые используются при ***ПЕРВОЙ*** загрузке прошивки в устройство. 
       В этот момент они сохраняются в памяти контроллера и при последующих запусках имя сети и пароль извлекаются из энергонезависимой памяти и 
       используются уже извлеченные значения, а не те, что прописаны в #define. 
       Если вы уже запускали устройство и ПОСЛЕ этого изменили в скетче имя и пароль сети, вам нужно также изменить значение флага, 
       указывающее было ли уже сохранение параметров в EEPROM или еще нет. Этот флаг находится в файле a_def_soft.h в строке 7.
       Измените на любое другое 16-ричное число в 1 байт.

```
#define EEPROM_OK 0x5A // Флаг, показывающий, что EEPROM инициализирована корректными данными 
```

Измените его на любое другое значение, например **0xA5**

Другой вариант сброса сохраненных настроек - загрузить прошивку с установленным режимом в меню **"Инструменты"** Arduino IDE
**"Erase Flash"** - **"All Flash Contents"** для **ESP8266** или **"Erase All Flash Before Sketch Upload"** - **"Enabled"** для **ESP32**.

-----

#### **В**: **Погода в Украине работать будет? Как узнать код региона для моего города?**  
**О**: Изучите настройку погоды для вашего региона [тут](https://github.com/vvip-68/LedPanelWiFi/wiki/Настройка-получения-информации-о-погоде)

-----

#### **В**: **Собрал, всё работает, но в начале матрицы постоянно ярко горит один светодиод. Как исправить?**  
**О**: Это проблема возникает при неправильно выбранной версии ядра ESP8266. Некоторые версии, отличные от рекомендованных, несовместимы с используемой версией библиотеки управления светодиодами FastLED.
Для исправления ситуации прочитайте инструкции по выбору версии ядра в комментариях в начале основного файла скетча. В Arduino IDE в менеджере плат найдите ESP8266 / ESP32 в зависимости от используемого микроконтроллера, посмотрите какая версия ядра установлена у вас и установите вместо нее правильную версию.  

-----

#### **В**: **Как мне добавить свою анимацию в скетч в дополнение к эффекту "Анимация"?**  
**О**: Изучите утилиту редактирования роликов и анимации JinxFramer - прочтите [инструкцию](https://github.com/vvip-68/LedPanelWiFi/wiki/Добавление-своей-анимации--к-эффекту-«Анимация»)  

-----

#### **В**: **Матрица после включения самопроизвольно меняет яркость. Или до максимума или до минимума.**  
**О**: ***Вариант 1***: У вас физически не подключена кнопка, но скетч скомпилирован с опцией использования функционала кнопки `#define USE_BUTTON 1`. В этом случае микроконтроллер свободный пин кнопки, притянутый внутренним резистором микроконтроллера к "земле" воспринимает как состояние кнопки "нажато". Длительное "нажатие" кнопки воспринимается скетчем как запрос на регулировку яркости матрицы.  
**Решение:** либо припаяйте кнопку, либо отключите в настройках скетча использование кнопки `#define USE_BUTTON 0`.   

***Вариант 2***: Используется версия ядра, отличная от рекомендованной **3.1.2** для **ESP8266**, **2.0.14** для **ESP32**.  

**Решение:** ***Вариант 1***:  
Отключить использование кнопки в настройках скетча - в файле a_def_hard.h указать
```
#define USE_BUTTON 0
```

**Решение:** ***Вариант 2***:  
Установить ядро рекомендованных версий **3.1.2** для **ESP8266** или **2.0.14** для **ESP32** - при этом кнопка матрицы будет работать корректно, самопроизвольного изменения яркости не будет происходить.

-----

#### **В**: **Проект собран, скетч загружен в плату. При попытке подключиться к устройству в браузере отображается "Сервер не активен"**  
![image](https://user-images.githubusercontent.com/39353139/236840474-0503c840-2dec-4e92-b981-056ef8ccb9d8.png)

**О**: Файлы Web-странички должны быть загружены в файловую систему микроконтроллера. Если вы этого не сделали - Web-сервер сообщает вам, что ему нечего отображать. То есть - сервер не активен.  

Файлы для загрузки находятся в проекте в папке **'data'** внутри парки скетча. Выполните [загрузку файлов](https://github.com/vvip-68/LedPanelWiFi/wiki/Пошаговая-инструкция-подготовки-среды-и-сборка-проекта#шаг-10) в [файловую систему](https://github.com/vvip-68/LedPanelWiFi/wiki/Загрузка-данных-в-файловую-систему-МК) микроконтроллера.   

