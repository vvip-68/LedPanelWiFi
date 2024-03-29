// Файл создан программой 'Jinx Frame Viewer'
#if (USE_ANIMATION == 1)

const uint16_t heart_0[] PROGMEM = {
    0x0000, 0xd003, 0xf800, 0x0000, 0xd80c, 0xd80c, 0x0000, 0x0000, 
    0xd003, 0xe800, 0xe800, 0xf800, 0xf9c9, 0xf4b8, 0xd80c, 0x0000, 
    0xd003, 0xf019, 0xe800, 0xe800, 0xf9c9, 0xf9c9, 0xd003, 0x0000, 
    0x0000, 0xd003, 0xf019, 0xe800, 0xd003, 0xd003, 0x0000, 0x0000, 
    0x0000, 0x0000, 0xe800, 0xf019, 0xd003, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

const uint16_t heart_1[] PROGMEM = {
    0x0000, 0xd003, 0xd003, 0x0000, 0x0000, 0xd80c, 0xd80c, 0x0000, 
    0xd003, 0xe800, 0xe800, 0xd003, 0xd003, 0xf4b8, 0xf4b8, 0xd80c, 
    0xd003, 0xf019, 0xe800, 0xe800, 0xf9c9, 0xf9c9, 0xf4b8, 0xd80c, 
    0xd003, 0xd003, 0xf019, 0xe800, 0xe800, 0xf9c9, 0xd80c, 0xd80c, 
    0x0000, 0xd003, 0xc003, 0xf019, 0xe800, 0xc003, 0xd80c, 0x0000, 
    0x0000, 0x0000, 0xc003, 0xf000, 0xc003, 0xc003, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0xf800, 0xc003, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000
};

const animation_t animation_heart PROGMEM = {
   .frames  = { heart_0, heart_1 },
   .start_x = 0,                        // Позиция отображения X (начальная)
   .start_y = 0,                        // Позиция отображения Y (начальная)
   .options = 1+2+4+8+16,               // Битовый флаг дополнительных параметров картинки
                                        //   1 - центрировать по горизонтали  (позиция start_x игнорируется)
                                        //   2 - центрировать по вертикали  (позиция start_y игнорируется)
                                        //   4 - есть прозрачные пиксели, цвет прозрачности в transparent_color
                                        //   8 - перед отрисовкой ПЕРВОГО кадра закрашивать всю матрицу цветом background_first_color
                                        //  16 - перед отрисовкой СЛЕДУЮЩИХ кадров закрашивать всю матрицу цветом background_color
                                        //  64 - начальное отображение - зеркальное по оси х
                                        // 128 - начальное отображение - зеркальное по оси y
   .frame_width = 8,                    // Ширина картинки (фрейма)
   .frame_height = 8,                   // Высота картинки (фрейма)
   .row_draw_direction = 0,             // Направление построчного рисования (битовые флаги):
                                        // Биты 0 и 1 (маска 0x03)
                                        //   0 (0x00) - сверху вниз
                                        //   1 (0x01) - снизу вверх
                                        //   2 (0x10) - слева направо
                                        //   3 (0x11)- справа налево
   .draw_frame_interval = 500,          // Интервал отрисовки очередной порции картинки анимации (строка при построчной анимации или кадр при покадровой)
   .draw_row_interval = 0,              // Задержка мс между отрисовкой строк изображения (если 0 - рисуется кадр целиком, а не построчно)
   .move_x_interval = 250,              // Смещение по оси X каждые N мс
   .move_y_interval = 180,              // Смещение по оси Y каждые N мс
   .move_type = 1+8+16+32,              // Тип движения - битовый флаг (разрешено только при отрисовке кадра целиком, при построчном - не работает):
                                        //   0 - нет;
                                        //   1 - из начальной позиции вправо
                                        //   2 - из начальной позиции влево
                                        //   4 - из начальной позиции вверх
                                        //   8 - из начальной позиции вниз
                                        //  16 - отражение при достижении границы по горизонтали
                                        //  32 - отражение при достижении границы по вертикали
                                        //  64 - переворот картинки при отражении по горизонтали
                                        // 128 - переворот картинки при отражении по вертикали
                                        // 256 - 1 - выходить за боковые рамки; 0 - не выходить
                                        // 512 - 1 - выходить за верх/низ; 0 - не выходить
   .transparent_color = 0x000000,       // Этот цвет - прозрачный, пиксели этого цвета не рисуются
   .background_first_color = 0x000000,  // Цвет заливки ВСЕЙ матрицы перед тем, как рисовать самый первый фрейм при активации эффекта анимации
   .background_color = 0x000000         // Цвет заливки ВСЕЙ матрицы перед тем, как рисовать очередной фрейм
};

#endif
