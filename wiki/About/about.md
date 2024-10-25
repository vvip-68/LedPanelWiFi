## Общие понятия

На AliExpress нет готового решения с подобным функционалом, чтобы вот так - купил, дождался посылки, открыл коробку, повесил на окно.
Все-таки это конструктор. Однако он состоит практически всего из трех частей, покупаемых отдельно - собственно "лампочки" (адресные светодиоды),
блок питания и микроконтроллер, который, собственно, управляет тем,  как зажигаются светодиоды. Нужна еще кое-какая мелочевка,
но она обычно заказывается оптом, и, как правило, есть у меня в наличии. Могу поделиться.

## Что такое гирлянда

Гирлянда - это матрица (сетка) в которой распологаются светодиоды. Имеет свою ширину и высоту. Например - 16 строк, 32 колонки.  
Соединяются светодиоды примерно так:

![Matrix](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/matrix.jpg)   

Конкретный размер - зависит от места, куда вы собираетесь повешать гирлянду, выбранной светодиодной ленты и количества денег, которые вы готовы потратить.
Естественно, чем больше размер "матрицы" - тем красочнее смотрятся на ней эффекты.
Кроме различного вида мерцания огоньков гирлянда может отображать часы (текущее время) и бегущую строку - короткие сообщения,
которые вы можете запрограммировать для периодического отображения.

Для часов размер гирлянды должен быть не менее (ширина x высота)
- 15x5 -  для мелких часов (размер цифры 3x5 точек)
- 25x7 -  для крупных часов и бегущей строки, шрифт 5x7

Я бы рекомендовал для начала ориентироваться на матрицу не менее 25x10, в крайнем случае - 25x8 для которой вам понадобится
250 или 200 диодов соответственно. Эффекты на матрицах меньшего размера будут смотреться не так красиво. Готовы потратиться на матрицу бОльшего размера - замечательно.


## Какие бывают светодиодные ленты

### Обычная лента для комнатных гирлянд 

Простейший тип - обычная лента с плотностью 30 диодов на метр, продается в катушках.
Расстояние между "огоньками" - примерно 3 см. То есть ячейка сетки будет 3x3 см. 
Может использоваться для небольших комнатных гирлянд. Лента собирается в матрицу как указано выше и крепится, например, к стене.
При размере матрицы 25 колонок, 10 строк размер гирлянды будет, соответственно, 75x30 см

Купить такую ленту можно, например [тут](https://aliexpress.ru/item/32967769057.html): 
Лента должна быть на 5 вольт - тип WS2812 (WS2812B). В катушке 5 метров вмещается 150 диодов.
Можно купить две катушки по 5 метров - будет 300 диодов и собрать гирлянду, например 30x10 или 20x15
Выбор - white - сама лента на которой размещены диоды - белого цвета, black - черного  
- IP30 - влагозащищенность - для сухих помещений (она дешевле)
- IP65 - водонепроницаемое силиконовое покрытие
- IP67 - водонепроницаемая силиконовая трубка

Для размещения гирлянды в комнате можно выбирать IP30. Для размещения на отпотевающем окне - лучше IP65  

![led-type-1](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/led-type-1.jpg)

### Лента для крупных гирлянд 

Такие гирлянды можно размещать в рамах окна или на балконном остеклении.
Светодиоды этих лент спаяны проводами обычно по 50 штук в ленте. Расстояние между отдельными диодами 10 или 12 см.
Таким образом ячейка сетки будет 10x10 или 12х12 сантиметров. Матрица размером 25x10 будет занимать соответственно 2.5 x 1 метр для ячеек 10 см 
или 3 метра на 1.20 для ячеек 12 см. 

Одна "нитка" содержит 50 диодов. Соответственно - для гирлянд 25x10 (250 диодов) нужно будет 5 "ниток", для 25x8 (200 диодов) - 4 нитки. 
Одна 50-диодная нитка складывается пополам - получаем две строки по 25 диодов. Сколько покупать для нужного вам размера гирлянды - считайте сами.

#### Лента с расстоянием 12 см

Ленту с расстоянием между диодами 12 см можно купить на Aliexpress примерно такую:

![led-type-2](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/led-type-2.jpg)

Благодаря широкому шлейфу ленту удобно крепить на окно, приклеивая шлейф к стеклу на прозрачный скотч.
Недостатком можно считать то, что диоды обращены излучателем в одну сторону. Если вы разместите ленту диодами на улицу - 
из комнаты будет виден только отраженный от стекла свет. Однако, эта лента более надежная чем та, что описана ниже.  

#### Лента с расстоянием 10 см

Ленту с расстоянием между диодами 10 см можно купить на Aliexpress примерно такую:

![led-type-3](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/led-type-3.jpg)

Эта лента позиционируется китайцами как более продвинутая, но мне она не понравилась.
Диоды в ленте соединены тонкими проволочками, изоляция которых вызывает некоторые сомнения. Похоже, что провода могут замыкать
между собой, из за чего диоды начинают светиться произвольными цветами, а вовсе не теми, которыми должны.
Может понадобиться колдовство с выявлением места где что замыкает, чтобы это устранить. Впрочем, без гарантии успеха.
Кроме того передача сигнала не очень надежна по какой-то причине, что периодически вызывает "срыв" и дергание картинки.
Зато форма светодиодов такова, что свет видно как снаружи так и снутри достаточно хорошо. В общем - с ней - как повезет.

#### Крепление гирлянды

Подумайте как вы ее будете "развешивать". В простейшем случае - для гирлянды на окно ее можно на зиму прилепить скотчем к окну,
по весне снять. Такой способ крепления ограничивает возможность открыть окно на проветривание - будут мешать провода.
В моем случае - я пришивал линии светодиодов на тюлевую шторку. Долго и муторно, на зато позволяет проветривать и открывать окна,
сдвинув штору с гирляндой в сторону.

## Источник питания

Для питания гирлянды требуется блок питания на 5 вольт, обеспечивающий достаточный ток.
Для гирлянды из 100 диодов достаточно ток 3 ампера, для 250-400 диодов - 6 ампер.
Лучше брать блок питания на 8 или 10 ампер - если вы вдруг в следующем году захотите увеличить размер гирлянды, докупив еще диодов.

Тут следует помнить, что светодиоды, купленные у разных продавцов или в разное время могут оказаться из разных партий и
несколько отличаться друг от друга по яркости / оттенку свечения. Если есть возможность - покупайте сразу в одном месте
нужное количество. Ну или, каждый год - дополнительно по паре линий чтобы расширять гирлянду до нужного размера, если есть желание.
Но тут как повезет с цветом.

Купить подходящий блок питания можно [тут](https://aliexpress.ru/item/1005002185053328.html?sku_id=12000018990942596):

![power-supply](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/power-supply.jpg)

Обратите внимание, что некоторые китайские магазины торгуют как светодиодами, так и источниками питания. Например магазин - BTF-LIGHTING-Store. Заказ из одного магазина предпочтительней - получите всё в одной посылке.

## Микроконтроллер

Это собственно то, что управляет диодами - говорит каждому из них каким цветом светиться в данный момент.  
Для гирлянды используется микроконтроллер Wemos D1 Mini. Можно купить, например, [тут](https://aliexpress.ru/item/32831353752.html) или [тут](https://aliexpress.ru/item/1005004344044423.html):
Выбирать "Wemos D1 mini". Это годится для гирлянд с числом светодиодов 500-700. Для гирлянд с большим количеством диодов
нужно выбирать другой микроконтроллер, например [такой](https://aliexpress.ru/item/32836372640.html?sku_id=12000029299984823) или [такой](https://aliexpress.ru/item/1005004605399313.html). 
Выбирать "ESP32".  

![wemos](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/wemos.jpg)

Необязательный элемент - модуль реле включения / выключения питания, подаваемого на светодиоды [тут](https://aliexpress.ru/item/32773890862.html) - "D1 Relay":

![relay](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/relay.jpg)

Матрица, даже когда не светится ("выключена") все равно потребляет некоторое количество энергии от источника питания.
Не много, но все же. Свести потребление к минимуму можно используя данное реле, которое разрывает цепь питания светодиодов, когда она выключена.
Гирлянда может включаться и выключаться автоматически по расписанию. Если же вы собираетесь делать это вручную выключая ее из розетки - это реле вам точно не нужно.
Вообще же потребление энергии - не больше чем у телевизора, оставленного включенным в розетку, но "выключенного" в дежурный режим.


## О закупках на AliExpress и не только

1. Лучше по возможности заказывать товары из одного магазина, тогда все придет вам в одной посылке
2. Вы можете поискать необходимые элементы у других продавцов - периодически они устраивают распродажи и скидки, где-то может получиться подешевле.
   Не стоит забывать о надежности продавцов (проверяйте их рейтинг). Если выбрали товар, но не уверены то ли это - спросите у меня. 
   Иногда товары **выглядят** похоже, но могут быть совсем не тем, что требуется. Лучше проконсультироваться.
3. Обычно доставка из магазинов AliExpress - 20-40 дней. Можете еще успеть к Новому году. Выбирайте товары с желтым значком "plus" - ускоренная доставка.
   Такие товары как правило приходят за три недели.

![plus](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/plus.jpg)

4. Иногда из российских магазинов товары приходят быстрее чем с AliExpress. Если не успеваете - заказывайте оттуда. 
   Правда стоят те же самые товары в российских магазинах в 2-3-4 раза дороже чем привезти с Китая. Но это ваш выбор.
   Можете посмотреть в [этом](https://giant4.ru/catalog/) магазине. Или на Wildberries.  
5. Как правило, если выбрать "Бесплатная доставка" - посылка отправляется с не отслеживаемым треком. 
   То есть она где-то перемещается, но где вам никто не скажет, пока не придет на почту. А может и вовсе не прийти. И такое бывает. 
   Я бы рекомендовал выбирать доставку с оплатой 50-70 рублей, в этом случае продавец как правило отправляет посылку, 
   которую можно отследить на сайте почты России или транспортной компании, которая везет вам посылку.

## Сборка

После того как посылка получена, можно приступить к сборке всего этого в единое устройство.
1. К ленте скорее всего придется припаять дополнительные провода на питание. 
2. Развесить гирлянду в месте назначения - приклеить скотчем к стеклу, повесить в виде "штор" и так далее - как ваша фантазия подскажет.
   Важно соблюдать направление включения в цепочку как на самом первом рисунке.
3. Запрограммировать микроконтроллер на ваш размер гирлянды.
4. Протянуть провод питания от блока питания к ленте, подключить контроллер, включить все это.
5. Установить на телефон программу управления гирляндой и настроить ее.

Для рукастых, умеющих припаять три провода - труда не составит сделать все самостоятельно.  
Для тех, кто не умеет паять - всю подготовительную работу (кроме развешивания гирлянды) могу сделать я. По соседски.
За небольшую дополнительную плату - сколько вам совесть подскажет :)

Так у меня выглядит конечное устройство - микроконтроллер, помещенный в коробочку с Леруа Мерлен.  
Слева и справа - разъемы для подключения блока питания и кабеля, идущего на питание гирлянды (выше - о необходимости припаять дополнительные провода),
справа - разъем для подключения управляющего сигнала к гирлянде. 

![Device](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/device.jpg)  

Taк выглядит на окне гирлянда из двух линий по 50 диодов - матрица 10x10  

![Device](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/About/window.jpg)  
