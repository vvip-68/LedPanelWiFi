## Всё собрал строго по инструкции, ничего не работает.

1. Разбираем всё обратно.
2. Берем платку, смотрим чтобы к ней НИЧЕГО не было подключено.
3. Плату в таком состоянии подключаем кабелем USB к компьютеру.
4. Берем последнюю версию [прошивки](https://github.com/vvip-68/LedPanelWiFi/) и загружаем ее в микроконтроллер.
5. Смотрим в сообщениях, что загрузка успешно выполнена и осуществлен перезапуск микроконтроллера, а в мониторе порта, что скетч стартовал, вывел версию прошивки, создал точку доступа PanelAP
6. Загружаем в файловую систему микроконтроллера данные из папки firmware/data - [инструкция](https://github.com/vvip-68/LedPanelWiFi/wiki/Загрузка-данных-в-файловую-систему-МК) 
7. Подключаемся телефоном к точке доступа, в браузере вводим в адресную строку адрес 192.168.4.1, ждем загрузку страницы. Пробуем тыкать на кнопки. Идем в раздел "Настройка", "Сеть", "Параметры сети", вводим имя сети, пароль для нее и IP адрес который должно получить устройство. 
8. Нажимаем кнопку "Применить", наблюдаем за перезагрузкой контроллера и подключением устройства к сети с указанным IP адресом.
9. Отключаем микроконтроллер от компьютера
10. Подключаем блок питания +5 вольт и минусовой провод к матрице. Минусовой провод подключаем также к пину GND микроконтроллера.
Контроллер подключаем USB кабелем к компьютеру, блок питания включаем в сеть.
11. Проверяем, что напряжение питания с блока не превышает +5.25 вольт. Если больше - регулируем его до уровня 4.8 вольта. (см. [заметки о питании](https://github.com/vvip-68/LedPanelWiFi/wiki/Заметки-о-питании))
12. Сигнальным проводом с матрицы с подключенным резистором 200 Ом тыкаемся поочередно в пины D4 и D2 микроконтроллера. В каком-то из этих двух вариантов на матрице должно сформироваться осмысленное отображение эффекта или бегущего текста. Что из этого конкретно должно в данный момент отображаться написано в мониторе порта. Запоминаем подключение к какому пину дало результат.
13. Отключаем микроконтроллер и блок питания. Провод +5V с блока питания подключаем к микроконтроллеру на пин его входного напряжения питания (у разных плат обозначен по разному - Vin, Vcc или +5). Сигнальный провод матрицы припаиваем к пину, который определили шагом выше. Очень желательно между пинами Vin и GND микроконтроллера припаять электролитический конденсатор номиналом 1000-4700 мкф, и параллельно ему керамический конденсатор на 33-100 нф.
14. Включаем блок питания в розетку. Панель должна заработать.
15. Собираем всю конструкцию в корпус. 
