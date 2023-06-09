# Как изменить размер букв бегущей строки

## Поддерживаемые размеры 

В настоящее время прошивка поддерживает бегущую строку трех размеров: шрифт 5x8, шрифт 8x13 и шрифт 10x16.  
Обратите внимание, что размеры вашей матрицы по высоте должны соответствовать размерам шрифта.
То есть - минимальный размер для которого ***Бегущая строка*** будет работать корректно - 8 диодов в высоту.

Если бегущая строка использует символы с диакритическими знаками - литовский, чешский, польский и т.п. алфавиты, например Č, Ė, Ū, Ą, Į,
высота шрифта увеличивается на три точки - выше и ниже начертания самого символа.

Настройка размера шрифта бегущей строки задается в файле **a_def_hard.h** в строке 198

```
#define BIG_FONT 0              // 0 - шрифт 5x8, 1 - шрифт  10x16; 2 - шрифт 8x13
```

Если ни один из предложенных вариантов вас не устраивает - рисуем собственный шрифт. Об этом читайте ниже.

## Как устроен шрифт

Размер шрифта по-умолчанию - 5x8. Это означает, что каждая буква занимает на матрице область, равную 5 колонок на 8 строк.
Типичное изображение буквы занимает 7 строк, и только некоторые буквы - Д,Ц,Щ имеют "хвостики", которые размещаются на самой нижней строке,
которую остальные буквы не используют.

Изображения букв шрифта определены в файле fonts.h

```
// шрифты для вывода текста 5x8
const uint8_t fontHEX[][5] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, //   0x20 32   0
  {0x00, 0x00, 0x6f, 0x00, 0x00}, // ! 0x21 33   1
  {0x00, 0x07, 0x00, 0x07, 0x00}, // " 0x22 34   2
  {0x14, 0x7f, 0x14, 0x7f, 0x14}, // # 0x23 35   3
  {0x00, 0x07, 0x04, 0x1e, 0x00}, // $ 0x24 36   4
  {0x23, 0x13, 0x08, 0x64, 0x62}, // % 0x25 37   5
  ...
  {0x7F, 0x49, 0x49, 0x49, 0x71}, // Б          96
  ...
  {0x06, 0x09, 0x06, 0x00, 0x00}, //°          159
  {0x3E, 0x49, 0x49, 0x41, 0x22}, // Є         160
  {0x38, 0x54, 0x54, 0x44, 0x28}, // є         162
  {0x00, 0x41, 0x7c, 0x45, 0x00}, // ї  
};
```
В крайней правой колонке комментария - индекс буквы в массиве fontHEX.  
Перед индексом - ASCII код буквы в шестнадцатеричном и десятичном виде.  
Самая первая колонка комментария - сам символ - буква, цифра, спецсимвол.  

Для изображения буквы нам достаточно 8 точек по вертикали (колонки матрицы) и 5 точек по горизонтали (строки матрицы).  
Шрифт рисуется на матрице по колонкам слева направо. То есть - для отрисовки 5 колонок нам нужно 5 элементов.  
Поэтому каждый символ задан пятью элементами массива и определение шрифта - `fontHEX[]`**`[5]`**  
Каждая колонка знакоместа символа - 8 точек, поэтому для этих точек достаточно 8-ми бит.  
8 бит - это тип **uint8_t**  
Поэтому тип элементов массива - **`uint8_t`**` fontHEX[][5]`  

Рассмотрим как нарисован образ буквы **Б** в массиве. Это элемент с индексом **96** в массиве **fontHEX**: 
```
  {0x7F, 0x49, 0x49, 0x49, 0x71}, // Б          96
```

На картинке показано как каждый байт массива изображения буквы **Б** разворачивается в точки:

![LETTER 5x8](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/Font/Б-5x8.png)

В каждой строке справа - 16-ричное представление байта колонки, как он определен в массиве `0x7F, 0x49, 0x49, 0x49, 0x71`.

## Рисуем свой шрифт

Если нас не устраивает размер букв, определенный в прошивке - придется нарисовать свои буквы нужного размера.  
Принцип тот же - рисуем по колонкам, изображения букв хранятся в массиве. Допустим, нас интересует шрифт размером 10x16;

Число точек в колонке - 16. Для хранения такого числа требуется тип, позволяющий хранить 16 бит. 
Ближайший стандартный тип, вмещающий 16 разрядов - uint16_t. 
Количество колонок - 10. Следовательно массив под шрифт размером 10x16 должен быть объявлен как:

```
// шрифты для вывода текста 10x16
const uint16_t fontHEX[][10] PROGMEM = {
}
```

Далее берем листок бумаги в клетку и рисуем изображение буквы в заданной сетке 10x16  

![LETTER 10x16](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/Font/Б-10x16.png)

Переводим закрашенные клетки (битовое представление) в 16-ричный вид:  

```
  {0x3FFF, 0x3FFF, 0x30C3, 0x30C3, 0x30C3, 0x30C3, 0x30C3, 0x30C3, 0x3FC3, 0x1F80},  // Б      96
```

Прорисовываем и прописываем все символы для нового шрифта, формируем массив.  

Переопределяем в скетче размер букв - ширину и высоту символа, промежуток между соседними буквами при отображении:  

```
  #define LET_WIDTH 10                      // ширина буквы шрифта
  #define LET_HEIGHT 16                     // высота буквы шрифта
  #define SPACE 2                           // пробел между буквами
```

Теперь, поскольку измененный нами шрифт содержит точки колонки шрифта в двух байтах (тип **uint16_t**),
в **runningText.ino** нужно все вызовы **pgm_read_byte** заменить на **pgm_read_word**. Данный вызов возвращает 16-битное слово (два байта)
при чтении из переопределенного нами массива изображения букв шрифта. На этом переделка скетча под большие буквы в бегущей строке закончена.

Пересобираем скетч, чтобы наши изменения вступили в силу.

## Редактор символа шрифта

Для облегчения создания своего шрифта с требуемым размером символов в папке проекта ***tools/FontMaker*** 
находится утилита редактирования символов шрифта. Запустите файл **FontMaker.exe**  

![FontMaker](https://github.com/vvip-68/LedPanelWiFi/blob/main/wiki/Font/FontMaker.png)

Порядок использования утилиты следующий:  

1. В поле, помеченном цифрой **1** введите требуемые размеры символа, например - 10x16. Программа создаст сетку редактирования указанного размера.  
2. Для очистки поля используйте кнопку **"Очистить"**, отмеченную маркером **2**  
3. Нажатие мышкой на клетках поля редактирования **3** переключает их в белый/черный цвет. Сформируйте образ символа.  
   В процессе формирования изображения в области ниже поля клеток формируется шестнадцатеричное представление чисел массива, 
   соответствующее сформированному образу (область **4**)  
4. В поле **5** автоматически создается элемент массива, соответствующий редактируемому символу.  
   Скопируйте текст из этого поля и вставьте в соответствующий элемент массива ***fontHEX*** в файле ***font.h***
5. Для редактирования существующего элемента массива ***fontHEX*** из файла ***font.h*** - скопируйте его в буфер обмена (*Clrl-C*) и
   вставьте (*Ctrl-V*) в поле **5** в редакторе шрифта. Образ символа, задаваемый текстом элемента массива будет воссоздан в
   области редактирования.


