#include <Arduino.h>
#include "timerMinim.h"

timerMinim::timerMinim(uint32_t interval) {
  _interval = interval;
  _timer = millis();
}

void timerMinim::stopTimer() {
  _interval = 4294967295;
  _timer = millis();
}

void timerMinim::setInterval(uint32_t interval) {
  _interval = interval;
}

uint32_t timerMinim::getInterval() {
  return _interval;
}

uint32_t timerMinim::getRestTime() {
  bool stopped = _interval == 4294967295;
  int32_t rest = stopped ? 4294967295 : _interval - (millis() - _timer);
  return rest < 0 ? 0 : rest;
}

bool timerMinim::isReady() {
  if (millis() - _timer >= _interval) {
    _timer = millis();
    return true;
  }
  return false;
}

bool timerMinim::isStopped() {
  bool stopped = _interval == 4294967295;
  if (stopped) {
    _timer = millis();
  }
  return stopped;
}

void timerMinim::reset() {
  _timer = millis();
}
