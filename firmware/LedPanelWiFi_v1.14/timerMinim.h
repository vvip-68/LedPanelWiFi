#pragma once

class timerMinim
{
  public:
    timerMinim(uint32_t interval);        // объявление таймера с указанием интервала
    void setInterval(uint32_t interval);  // установка интервала работы таймера
    uint32_t getInterval();               // получение установленного интервала работы таймера
    uint32_t getRestTime();               // получение остатка времени до срабатывания таймера
    bool isReady();                       // возвращает true, когда пришло время. Сбрасывается в false сам (AUTO) или вручную (MANUAL)
    bool isStopped();                     // возвращает true, если таймер остановлен
    void reset();                         // ручной сброс таймера на установленный интервал
    void stopTimer();                     // Остановить таймер

  private:
    uint32_t _timer = 0;
    uint32_t _interval = 0;
};
