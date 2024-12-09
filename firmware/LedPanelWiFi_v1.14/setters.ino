// Для поддержки работы Web нужно знать какой параметр изменился, чтобы отправить изменения на сервер
// По этой причине нельзя напрямую присваивать новое значение переменной, нужно выполнить дополнительные действия,
// чтобы зафиксировать изменение значения. Для этой цели данная страница содержит функции - сеттеры, 
// устанавливающие значения переменных.

#include "timeProcessor.h"

// Добавление ключа в список изменившихся параметров, чьи новые значения необходимо отправить на сервер
void addKeyToChanged(const String& key) {
  String search('|'); search += key; search += '|';
  // Если ключа еще нет в строке измененных параметров - добавить 
  if (changed_keys.length() == 0) changed_keys = search; else 
  if (changed_keys.indexOf(search) < 0) {
    changed_keys += key;
    changed_keys += '|';
  }
}

// PS isTurnedOff
void set_isTurnedOff(bool value) {
  if (isTurnedOff == value) return;
  isTurnedOff = value;
  addKeyToChanged("PS");
  #if (USE_E131 == 1)
    commandTurnOnOff(value);
  #endif
}

// FK isAuxActive
void set_isAuxActive(bool value) {
  if (isAuxActive == value) return;
  isAuxActive = value;
  putAuxLineState(value);
  addKeyToChanged("FK");
  #if (USE_E131 == 1)
    commandAuxActive(value);
  #endif
  DEBUG(F("Питaние дополнительной линии "));
  if (isAuxActive) { DEBUGLN(F("включено"));  }
  else             { DEBUGLN(F("выключено")); }
}

// FG состояние режимов линии управления - битовая маска
void set_AuxLineModes(uint16_t value) {
  if (auxLineModes == value) return;
  auxLineModes = value;
  putAuxLineModes(value);
  addKeyToChanged("FG");
}

// BR isNightClock
void set_isNightClock(bool value) {
  if (isNightClock == value) return;
  isNightClock = value;
  addKeyToChanged("BR");
}

// BR deviceBrightness
void set_globalBrightness(uint8_t value) {
  #if (USE_E131 == 1)
    commandSetBrightness(value);
  #endif
  if (deviceBrightness == value) return;
  putMaxBrightness(value);
  deviceBrightness = getMaxBrightness();
  FastLEDsetBrightness(deviceBrightness);
  addKeyToChanged("BR");
}

// BR specialBrightness
void set_specialBrightness(uint8_t value) {
  #if (USE_E131 == 1)
    commandSetSpecialBrightness(value);
  #endif
  if (specialBrightness == value) return;;
  specialBrightness = value;
  FastLEDsetBrightness(specialBrightness);
  addKeyToChanged("BR");
}

// DM manualMode
void set_manualMode(bool value) {
  if (manualMode == value) return;
  putAutoplay(value);
  manualMode = getAutoplay();
  addKeyToChanged("DM");
}

// PD autoplayTime
void set_autoplayTime(uint32_t value) {
  if (autoplayTime == value) return;
  putAutoplayTime(value);
  autoplayTime = getAutoplayTime();
  addKeyToChanged("PD");
}

// IT idleTime
void set_idleTime(uint32_t value) {
  if (idleTime == value) return;;
  putIdleTime(value);
  idleTime = getIdleTime();
  addKeyToChanged("IT");
}

// AL isAlarming 
void set_isAlarming(bool value) {
  if (isAlarming == value) return;
  isAlarming = value;
  #if (USE_E131 == 1)
    commandAlarming(isAlarming || isPlayAlarmSound);
  #endif
  addKeyToChanged("AL");  
}

// AL isPlayAlarmSound
void set_isPlayAlarmSound(bool value) {
  if (isPlayAlarmSound == value) return;
  isPlayAlarmSound = value;
  #if (USE_E131 == 1)
    commandAlarming(isAlarming || isPlayAlarmSound);
  #endif
  addKeyToChanged("AL");
}

// AL isAlarmStopped
void set_isAlarmStopped(bool value) {
  if (isAlarmStopped == value) return;
  isAlarmStopped = value;
  addKeyToChanged("AL");
}

// RM useRandomSequence
void set_useRandomSequence(bool value) {
  if (useRandomSequence == value) return;
  putRandomMode(value);
  useRandomSequence = getRandomMode();
  addKeyToChanged("RM");
}

// PW CURRENT_LIMIT
void set_CURRENT_LIMIT(uint16_t value) {
  if (CURRENT_LIMIT == value) return;
  putPowerLimit(value);
  CURRENT_LIMIT = getPowerLimit();
  addKeyToChanged("PW");
}

#if (USE_WEATHER == 1)
// WU useWeather
void set_useWeather(uint8_t value) {
  if (useWeather == value) return;
  putUseWeather(value);
  useWeather = getUseWeather();
  addKeyToChanged("WU");
}

// WT SYNC_WEATHER_PERIOD
void set_SYNC_WEATHER_PERIOD(uint16_t value) {
  if (SYNC_WEATHER_PERIOD == value) return;
  putWeatherInterval(value);
  SYNC_WEATHER_PERIOD = getWeatherInterval();
  addKeyToChanged("WT");
}

// WR regionID
void set_regionID(uint32_t value) {
  if (regionID == value) return;
  putWeatherRegion(value);
  regionID = getWeatherRegion();
  addKeyToChanged("WR");
}

// WS regionID2
void set_regionID2(uint32_t value) {
  if (regionID2 == value) return;
  putWeatherRegion2(value);
  regionID2 = getWeatherRegion2();
  addKeyToChanged("WS");
}

// WC useTemperatureColor
void set_useTemperatureColor(bool value) {
  if (useTemperatureColor == value) return;
  putUseTemperatureColor(value);
  useTemperatureColor = getUseTemperatureColor(); 
  addKeyToChanged("WC");
}

// WN useTemperatureColorNight
void set_useTemperatureColorNight(bool value) {
  if (useTemperatureColorNight == value) return;
  putUseTemperatureColorNight(value);
  useTemperatureColorNight = getUseTemperatureColorNight(); 
  addKeyToChanged("WN");
}

// W1 weather
void set_weather(const String& value) {
  if (weather == value) return;
  weather = value;
  addKeyToChanged("W1");
}

// W2 temperature
void set_temperature(int8_t value) {
  if (temperature == value) return;
  temperature = value;
  addKeyToChanged("W2");
}

// TF temperature - в Цельсиях / Фаренгейтах
void set_IsFarenheit(bool value) {
  if (isFarenheit == value) return;
  putIsFarenheit(value);
  isFarenheit = value;
  addKeyToChanged("TF");
}

// WV - нужно ли показывать знак градуса и C/F при отображении температуры в малых часах
void set_ShowTempProps(int8_t value) {
  int8_t old = (showTempDegree ? 0x02 : 0x00) | (showTempLetter ? 0x01 : 0x00);
  if (old == value) return;
  putShowTempProps(value);  
  value = getShowTempProps();
  showTempDegree = (value & 0x02) > 0;
  showTempLetter = (value & 0x01) > 0;
  addKeyToChanged("WV");
}

// WW - нужно ли показывать знак градуса и C/F при отображении температуры в макросе {WT} бегущей строки
void set_ShowTempTextProps(int8_t value) {
  int8_t old = (showTempTextDegree ? 0x02 : 0x00) | (showTempTextLetter ? 0x01 : 0x00);
  if (old == value) return;
  putShowTempTextProps(value);  
  value = getShowTempTextProps();
  showTempTextDegree = (value & 0x02) > 0;
  showTempTextLetter = (value & 0x01) > 0;
  addKeyToChanged("WW");
}

#endif

// EF thisMode
// EN thisMode
void set_thisMode(int8_t value) {
  if (thisMode == value) return;
  
  bool valid = (value == -1) || (value >= 0 && value < MAX_EFFECT) || (value >= SPECIAL_EFFECTS_START);
  if (!valid) return;

  valid = (value >= 0 && value < MAX_EFFECT);
  
  // Переключили на новый режим
  thisMode = value;
  effect_name.clear();

  // Ролучили имя нового режима
  if (valid && !isTurnedOff) {
    effect_name += getEffectName(value);
  } else if (isTurnedOff) {
    effect_name += F("Выключено");
  } else {
    switch (value) {
      case MC_DRAW:              effect_name += FPSTR(MODE_DRAW); break;
      case MC_LOADIMAGE:         effect_name += FPSTR(MODE_LOAD_PICTURE); break;
      case MC_TEXT:              effect_name += FPSTR(MODE_RUNNING_TEXT); break;
      case MC_DAWN_ALARM_SPIRAL: effect_name += FPSTR(MODE_DAWN); break;
      case MC_DAWN_ALARM_SQUARE: effect_name += FPSTR(MODE_DAWN); break;
    }
  }

  addKeyToChanged("EF");
  addKeyToChanged("EN");
     
  #if (USE_E131 == 1)
    commandSetMode(thisMode);
  #endif      
}

// BS
void set_GameButtonSpeed(uint8_t spd) {
  if (spd < 10) spd = 10;
  if (spd > 100) spd = 100;
  uint8_t old_value = getGameButtonSpeed();
  if (old_value != spd) {
    putGameButtonSpeed(spd);
    addKeyToChanged("BS");
  }
}
  
// SM
void set_CurrentSpecialMode(int8_t spc_mode) {
  bool valid = spc_mode < MAX_SPEC_EFFECT; 
  if (!valid) return;
  int8_t old_value = getCurrentSpecMode();
  if (old_value != spc_mode ) {
    putCurrentSpecMode(spc_mode);
  }
  specialModeId = spc_mode;
  specialMode = specialModeId != -1;    
  addKeyToChanged("SM");
}

// UT 
void set_EffectTextOverlayUsage(uint8_t effect, bool value) {
  bool valid = effect < MAX_EFFECT;
  if (!valid) return;
  bool old_value = getEffectTextOverlayUsage(effect);
  String old_s_value(getStateValue("UT", effect));
  if (old_value != value) {
    putEffectTextOverlayUsage(effect, value);
    if (effect == thisMode && old_s_value != getStateValue("UT", effect)) addKeyToChanged("UT");
  }
}

// UC
void set_EffectClockOverlayUsage(uint8_t effect, bool value) {
  bool valid = effect < MAX_EFFECT;
  if (!valid) return;
  bool old_value = getEffectClockOverlayUsage(effect);
  String old_s_value(getStateValue("UC", effect));
  if (old_value != value) {
    putEffectClockOverlayUsage(effect, value);
    if (effect == thisMode && old_s_value != getStateValue("UC", effect)) addKeyToChanged("UC");
  }
}

// SE
void set_EffectSpeed(uint8_t effect, uint8_t value) {
  bool valid = effect < MAX_EFFECT;
  if (!valid) return;
  uint8_t old_value = effectSpeed[effect];
  String old_s_value(getStateValue("SE", effect));
  if (old_value != value) {
    effectSpeed[effect] = value;
    putEffectSpeed(effect, value);
  }
  if (effect == thisMode && old_s_value != getStateValue("SE", effect)) addKeyToChanged("SE");
  #if (USE_E131 == 1)
    commandSetEffectSpeed(value);
    syncEffectSpeed = value;
  #endif
}

// BE
void set_EffectContrast(uint8_t effect, uint8_t value) {
  bool valid = effect < MAX_EFFECT;
  if (!valid) return;
  uint8_t old_value = effectContrast[effect];
  String old_s_value(getStateValue("BE", effect));
  if (old_value != value) {
    effectContrast[effect] = value;
    putEffectContrast(effect, value);
  }
  if (effect == thisMode && old_s_value != getStateValue("BE", effect)) addKeyToChanged("BE");
  #if (USE_E131 == 1)
    commandSetEffectContrast(value);
    syncEffectContrast = value;
  #endif
}

// SS
void set_EffectScaleParam(uint8_t effect, uint8_t value) {
  bool valid = effect < MAX_EFFECT;
  if (!valid) return;
  uint8_t old_value = effectScaleParam[effect];
  String old_s_value(getParamForMode(effect));
  if (old_value != value) {
    effectScaleParam[effect] = value;
    putScaleForEffect(effect, value);
    if (effect == thisMode && old_s_value != getParamForMode(effect)) addKeyToChanged("SS");
  }
  #if (USE_E131 == 1)
    commandSetEffectParam(value);
    syncEffectParam1 = value;
  #endif
}

// SQ
void set_EffectScaleParam2(uint8_t effect, uint8_t value) {
  bool valid = effect < MAX_EFFECT;
  if (!valid) return;
  uint8_t old_value = effectScaleParam2[effect];
  String old_s_value(getParam2ForMode(effect));
  if (old_value != value) {
    effectScaleParam2[effect] = value;
    putScaleForEffect2(effect, value);
    if (effect == thisMode && old_s_value != getParam2ForMode(effect)) addKeyToChanged("SQ");
  }
  #if (USE_E131 == 1)
    commandSetEffectParam2(value);
    syncEffectParam2 = value;
  #endif
}

// TE textOverlayEnabled
void set_textOverlayEnabled(bool value) {
  if (textOverlayEnabled == value) return;
  putTextOverlayEnabled(value);
  textOverlayEnabled = getTextOverlayEnabled();
  addKeyToChanged("TE");
}

// TI TEXT_INTERVAL
void set_TEXT_INTERVAL(uint16_t value) {
  if (TEXT_INTERVAL == value) return;
  putTextInterval(value);
  TEXT_INTERVAL = getTextInterval();
  addKeyToChanged("TI");
}

// CT COLOR_TEXT_MODE
void set_COLOR_TEXT_MODE(uint8_t value) {
  if (COLOR_TEXT_MODE == value) return;
  putTextColor(value);
  COLOR_TEXT_MODE = getTextColor();
  addKeyToChanged("CT");
}

// ST textScrollSpeed
void set_textScrollSpeed(uint8_t value) {
  if (textScrollSpeed == value) return;
  putTextScrollSpeed(value);
  textScrollSpeed = getTextScrollSpeed();
  addKeyToChanged("ST");
  #if (USE_E131 == 1)
    commandSetTextSpeed(value);
  #endif
}

// ?? globalColor
void set_globalColor(uint32_t value) {
  if (globalColor == value) return;
  putGlobalColor(value);
  globalColor = getGlobalColor();
  #if (USE_E131 == 1)
    commandSetGlobalColor(globalColor);
  #endif
}

// C1 globalClockColor
void set_globalClockColor(uint32_t value) {
  if (globalClockColor == value) return;
  putGlobalClockColor(value);
  globalClockColor = getGlobalClockColor();
  addKeyToChanged("C1");
}

// C2 globalTextColor
void set_globalTextColor(uint32_t value) {
  if (globalTextColor == value) return;
  putGlobalTextColor(value);
  globalTextColor = getGlobalTextColor();
  addKeyToChanged("C2");
}

// C12 12/24-часовой фоормат времени
void set_Time12(bool value) {
  if (time_h12 == value) return;
  putTime12(value);
  time_h12 = value;
  addKeyToChanged("C12");
}

// C35 - тип шрифта 3x5 - 0 - квадратный; 1 - скругленный
void set_SmallFontType(int8_t value) {
  int8_t old = use_round_3x5 ? 1 : 0;
  if (old == value) return;
  putSmallFontType(value);
  use_round_3x5 = value == 1;
  addKeyToChanged("C35");
}

// CE clockOverlayEnabled
void set_clockOverlayEnabled(bool value) {
  bool old_value = clockOverlayEnabled;
  String old_s_value(getStateValue("CE", thisMode));
  if (old_value != value) {
    putClockOverlayEnabled(value);
    clockOverlayEnabled = getClockOverlayEnabled();
  }
  if (old_s_value != getStateValue("CE", thisMode)) addKeyToChanged("CE");
}

// CC COLOR_MODE
void set_COLOR_MODE(uint8_t value) {
  if (COLOR_MODE == value) return;
  putClockColor(value);
  COLOR_MODE = getClockColor();
  addKeyToChanged("CC");
}

// CL drawColor
void set_drawColor(uint32_t value) {
  if (drawColor == value) return;
  drawColor = value;
  addKeyToChanged("CL");
}

// CO CLOCK_ORIENT
void set_CLOCK_ORIENT(uint8_t value) {
  uint8_t old_value = CLOCK_ORIENT;
  String old_s_value(getStateValue("CO", thisMode));
  if (old_value != value) {
    putClockOrientation(value);
    CLOCK_ORIENT = value;
    if (value == 0 && !allowHorizontal) value = 1;
    if (value == 1 && !allowVertical) value = 0;
    if (value != CLOCK_ORIENT) {
      CLOCK_ORIENT = value;
      putClockOrientation(value);
    }
  }
  if (old_s_value != getStateValue("CO", thisMode)) addKeyToChanged("CO");
}

// CK CLOCK_SIZE
void set_CLOCK_SIZE(uint8_t value) {
  if (CLOCK_SIZE == value) return;
  putClockSize(value);
  CLOCK_SIZE = getClockSize();
  addKeyToChanged("CK");
}

// CX смещение часов по оси X
void set_ClockOffsetX(int8_t value) {
  int8_t oldValue = CLOCK_SIZE == 0 ? getClockOffsetXsmall() : getClockOffsetXbig();
  if (oldValue != value) {
    if (CLOCK_SIZE == 0)
      putClockOffsetXsmall(value);
    else 
      putClockOffsetXbig(value);      
  }
  clockOffsetX = CLOCK_SIZE == 0 ? getClockOffsetXsmall() : getClockOffsetXbig();
  addKeyToChanged("CX");
}

// CY смещение часов по оси Y
void set_ClockOffsetY(int8_t value) {
  int8_t oldValue = CLOCK_SIZE == 0 ? getClockOffsetYsmall() : getClockOffsetYbig();
  if (oldValue != value) {
    if (CLOCK_SIZE == 0)
      putClockOffsetYsmall(value);
    else 
      putClockOffsetYbig(value);      
  }
  clockOffsetY = CLOCK_SIZE == 0 ? getClockOffsetYsmall() : getClockOffsetYbig();
  addKeyToChanged("CY");
}

// NB nightClockBrightness
void set_nightClockBrightness(uint8_t value) {
  if (nightClockBrightness == value) return;
  putNightClockBrightness(value);
  nightClockBrightness = getNightClockBrightness();
  addKeyToChanged("NB");
  addKeyToChanged("BR");
}

// NC nightClockColor
void set_nightClockColor(uint8_t value) {
  if (nightClockColor == value) return;
  putNightClockColor(value);
  nightClockColor = getNightClockColor();
  addKeyToChanged("NC");
}

// CD - ширина точки в больших часах
void set_clockDotWidth(uint8_t value) {
  if (clockDotWidth == value) return;
  putClockDotWidth(value);
  clockDotWidth = getClockDotWidth();
  addKeyToChanged("CD");
}

// CS - ширина точки в больших часах
void set_clockDotSpace(bool value) {
  if (clockDotSpace == value) return;
  putClockDotSpace(value);
  clockDotSpace = getClockDotSpace();
  addKeyToChanged("CS");
}

// SC clockScrollSpeed
void set_clockScrollSpeed(uint8_t value) {
  if (clockScrollSpeed == value) return;
  putClockScrollSpeed(value);
  clockScrollSpeed = getClockScrollSpeed();
  addKeyToChanged("SC");
  #if (USE_E131 == 1)
    commandSetClockSpeed(value);
  #endif
}

// DC showDateInClock
void set_showDateInClock(bool value) {
  if (showDateInClock == value) return;
  putShowDateInClock(value);
  showDateInClock = getShowDateInClock();
  addKeyToChanged("DC");
}

// DD showDateDuration
void set_showDateDuration(uint8_t value) {
  if (showDateDuration == value) return;
  putShowDateDuration(value);
  showDateDuration = getShowDateDuration();
  addKeyToChanged("DD");
}

// DI showDateInterval
void set_showDateInterval(uint8_t value) {
  if (showDateInterval == value) return;
  putShowDateInterval(value);
  showDateInterval = getShowDateInterval();
  addKeyToChanged("DI");
}

// NP useNtp
void set_useNtp(bool value) {
  if (useNtp == value) return;
  putUseNtp(value);
  useNtp = getUseNtp();
  addKeyToChanged("NP");
  useNtp ? TimeProcessor::getInstance().enable() : TimeProcessor::getInstance().disable();
}

// TZ ntpTimeZone
void set_ntpTimeZone(const String& value) {
  if (getTimeZone() == value) return;
  putTimeZone(value);
  applyTimeZone(value);
  addKeyToChanged("TZ");
}

// NS ntpServerName
void set_ntpServerName(const String& value) {
  if (getNtpServer() == value) return;
  putNtpServer(value);  
  addKeyToChanged("NS");
  TimeProcessor::getInstance().setcustomntp(value.c_str());
}

// NW ssid
void set_Ssid(const String& value) {
  putSsid(value);
  addKeyToChanged("NW");
}

// HN - system/host name
void set_SystemName(const String& value) {
  putSystemName(value);
  addKeyToChanged("HN");
}

// NA|NX pass
void set_pass(const String& value) {
  putPass(value);
  addKeyToChanged("NA");
  addKeyToChanged("NX");
}
              
// AN apName
void set_SoftAPName(const String& value) {
  if (getSoftAPName() == value) return;
  putSoftAPName(value);
  addKeyToChanged("AN");
}              

// AA|AB apPass
void set_SoftAPPass(const String& value) {
  if (getSoftAPPass() == value) return;
  putSoftAPPass(value);
  addKeyToChanged("AA");
  addKeyToChanged("AB");
}              

// DH - ширина точки в больших часах
void set_useDHCP(bool value) {
  if (getUseDHCP() == value) return;
  putUseDHCP(value);
  addKeyToChanged("DH");
}

// IP wifi_connected
void set_wifi_connected(bool value) {
  if (wifi_connected == value) return;
  wifi_connected = value;
  addKeyToChanged("IP");
}              

// IP IP_STA[]
void set_StaticIP(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4) {
  IP_STA[0] = p1; 
  IP_STA[1] = p2; 
  IP_STA[2] = p3; 
  IP_STA[3] = p4; 
  putStaticIP(p1, p2, p3, p4);
  addKeyToChanged("IP");
}              

// DW showWeatherInClock
void set_showWeatherInClock(bool value) {
  if (showWeatherInClock == value) return;
  putShowWeatherInClock(value);
  showWeatherInClock = getShowWeatherInClock();
  addKeyToChanged("DW");
}

// OF needTurnOffClock
void set_needTurnOffClock(bool value) {
  if (needTurnOffClock == value) return;
  putTurnOffClockOnLampOff(value);
  needTurnOffClock = getTurnOffClockOnLampOff();
  addKeyToChanged("OF");
}

// AD dawnDuration
void set_dawnDuration(uint8_t value) {
  if (dawnDuration == value) return;
  uint8_t alarmWeekDay = getAlarmWeekDay();
  uint8_t alarmEffect = getAlarmEffect();
  uint8_t alarmDuration = getAlarmDuration();  
  putAlarmParams(alarmWeekDay,value,alarmEffect,alarmDuration);
  dawnDuration = getDawnDuration();
  addKeyToChanged("AD");
}

// AW alarmWeekDay
void set_alarmWeekDay(uint8_t value) {
  if (alarmWeekDay == value) return;
  uint8_t dawnDuration = getDawnDuration();
  uint8_t alarmEffect = getAlarmEffect();
  uint8_t alarmDuration = getAlarmDuration();  
  putAlarmParams(value,dawnDuration,alarmEffect,alarmDuration);
  alarmWeekDay = getAlarmWeekDay();
  addKeyToChanged("AW");
}

// AE alarmEffect
void set_alarmEffect(uint8_t value) {
  if (alarmEffect == value) return;
  uint8_t alarmWeekDay = getAlarmWeekDay();
  uint8_t dawnDuration = getDawnDuration();
  uint8_t alarmDuration = getAlarmDuration();  
  putAlarmParams(alarmWeekDay,dawnDuration,value,alarmDuration);
  alarmEffect = getAlarmEffect();
  addKeyToChanged("AE");
}

// MD alarmDuration
void set_alarmDuration(uint8_t value) {
  if (alarmDuration == value) return;
  uint8_t alarmWeekDay = getAlarmWeekDay();
  uint8_t dawnDuration = getDawnDuration();
  uint8_t alarmEffect = getAlarmEffect();
  putAlarmParams(alarmWeekDay,dawnDuration,alarmEffect,value);
  alarmDuration = getAlarmDuration();  
  addKeyToChanged("MD");
}

// AT alarmHour[], alarmMinute[]
void set_alarmTime(uint8_t wd, uint8_t hour_value, uint8_t minute_value) {
  uint8_t old_hour   = getAlarmHour(wd);
  uint8_t old_minute = getAlarmMinute(wd);
  if (old_hour == hour_value && old_minute == minute_value) return;
  putAlarmTime(wd, hour_value, minute_value);
  alarmHour[wd-1] = getAlarmHour(wd);
  alarmMinute[wd-1] = getAlarmMinute(wd);
  addKeyToChanged("AT");
}

// MX isDfPlayerOk
void set_isDfPlayerOk(bool value) {
  if (isDfPlayerOk == value) return;
  isDfPlayerOk = value;
  addKeyToChanged("MX");
}

#if (USE_MP3 == 1)
// MU useAlarmSound
void set_useAlarmSound(bool value) {
  if (useAlarmSound == value) return;  
  uint8_t alarmSound = getAlarmSound();
  uint8_t dawnSound = getDawnSound();
  uint8_t maxAlarmVolume = getMaxAlarmVolume();
  putAlarmSounds(value, maxAlarmVolume, alarmSound, dawnSound);
  useAlarmSound = getUseAlarmSound();
  addKeyToChanged("MU");
}

// MV maxAlarmVolume
void set_maxAlarmVolume(uint8_t value) {
  if (maxAlarmVolume == value) return;  
  bool    useAlarmSound = getUseAlarmSound();
  uint8_t alarmSound = getAlarmSound();
  uint8_t dawnSound = getDawnSound();
  putAlarmSounds(useAlarmSound, value, alarmSound, dawnSound);
  maxAlarmVolume = getMaxAlarmVolume();
  addKeyToChanged("MV");
}

// MA alarmSound
void set_alarmSound(int8_t value) {
  if (alarmSound == value) return;  
  bool useAlarmSound = getUseAlarmSound();
  uint8_t dawnSound = getDawnSound();
  uint8_t maxAlarmVolume = getMaxAlarmVolume();
  putAlarmSounds(useAlarmSound, maxAlarmVolume, value, dawnSound);
  alarmSound = getAlarmSound();
  addKeyToChanged("MA");
}

// MB dawnSound
void set_dawnSound(int8_t value) {
  if (dawnSound == value) return;  
  bool    useAlarmSound = getUseAlarmSound();
  uint8_t alarmSound = getAlarmSound();
  uint8_t maxAlarmVolume = getMaxAlarmVolume();
  putAlarmSounds(useAlarmSound, maxAlarmVolume, alarmSound, value);
  dawnSound = getDawnSound();
  addKeyToChanged("MB");
}

// MP soundFolder
void set_soundFolder(uint8_t value) {
  if (soundFolder == value) return;  
  soundFolder = value;
  addKeyToChanged("MP");
}

// MP soundFile
void set_soundFile(uint8_t value) {
  if (soundFile == value) return;  
  soundFile = value;
  addKeyToChanged("MP");
}
#endif

// AU useSoftAP
void set_useSoftAP(bool value) {
  if (useSoftAP == value) return;
  putUseSoftAP(value);
  useSoftAP = getUseSoftAP();
  addKeyToChanged("AU");
}

// AM1T AM1_hour
void set_AM1_hour(uint8_t value) {
  if (AM1_hour == value) return;
  putAM1hour(value);
  AM1_hour = getAM1hour();
  addKeyToChanged("AM1T");
}

// AM1T AM1_minute
void set_AM1_minute(uint8_t value) {
  if (AM1_minute == value) return;
  putAM1minute(value);
  AM1_minute = getAM1minute();
  addKeyToChanged("AM1T");
}

// AM1A AM1_effect_id
void set_AM1_effect_id(int8_t value) {
  if (AM1_effect_id == value) return;
  putAM1effect(value);
  AM1_effect_id = getAM1effect();  
  addKeyToChanged("AM1A");
}

// AM2T AM2_hour
void set_AM2_hour(uint8_t value) {
  if (AM2_hour == value) return;
  putAM2hour(value);
  AM2_hour = getAM2hour();
  addKeyToChanged("AM2T");
}

// AM2T AM2_minute
void set_AM2_minute(uint8_t value) {
  if (AM2_minute == value) return;
  putAM2minute(value);
  AM2_minute = getAM2minute();
  addKeyToChanged("AM2T");
}

// AM2A AM2_effect_id
void set_AM2_effect_id(int8_t value) {
  if (AM2_effect_id == value) return;
  putAM2effect(value);
  AM2_effect_id = getAM2effect();
  addKeyToChanged("AM2A");
}

// AM3T AM3_hour
void set_AM3_hour(uint8_t value) {
  if (AM3_hour == value) return;
  putAM3hour(value);
  AM3_hour = getAM3hour();
  addKeyToChanged("AM3T");
}

// AM3T AM3_minute
void set_AM3_minute(uint8_t value) {
  if (AM3_minute == value) return;
  putAM3minute(value);
  AM3_minute = getAM3minute();
  addKeyToChanged("AM3T");
}

// AM3A AM3_effect_id
void set_AM3_effect_id(int8_t value) {
  if (AM3_effect_id == value) return;
  putAM3effect(value);
  AM3_effect_id = getAM3effect();
  addKeyToChanged("AM3A");
}

// AM4T AM4_hour
void set_AM4_hour(uint8_t value) {
  if (AM4_hour == value) return;
  putAM4hour(value);
  AM4_hour = getAM4hour();
  addKeyToChanged("AM4T");
}

// AM4T AM4_minute
void set_AM4_minute(uint8_t value) {
  if (AM4_minute == value) return;
  putAM4minute(value);
  AM4_minute = getAM4minute();
  addKeyToChanged("AM4T");
}

// AM4A AM4_effect_id
void set_AM4_effect_id(int8_t value) {
  if (AM4_effect_id == value) return;
  putAM4effect(value);
  AM4_effect_id = getAM4effect();
  addKeyToChanged("AM4A");
}

// AM5A dawn_effect_id
void set_dawn_effect_id(int8_t value) {
  if (dawn_effect_id == value) return;
  putAM5effect(value);
  dawn_effect_id = getAM5effect();
  addKeyToChanged("AM5A");
}

// AM6A dusk_effect_id
void set_dusk_effect_id(int8_t value) {
  if (dusk_effect_id == value) return;
  putAM6effect(value);
  dusk_effect_id = getAM6effect();
  addKeyToChanged("AM6A");
}

#if (USE_SD == 1)
// SX isSdCardExist -- SD карта подключена к системе
void set_isSdCardExist(bool value) {
  if (isSdCardExist == value) return;  
  isSdCardExist = value;
  addKeyToChanged("SX");
}
// SD isSdCardReady -- на SD-карте есть файлы с эффектами
void set_isSdCardReady(bool value) {
  if (isSdCardReady == value) return;  
  isSdCardReady = value;
  addKeyToChanged("SD");
}
#endif

#if (USE_E131 == 1)

// E1 - workMode
void set_SyncWorkMode(eWorkModes value) {
  if (workMode == value) return;
  putSyncWorkMode(value);
  workMode = getSyncWorkMode();
  addKeyToChanged("E1");  
}

// E2 - syncMode
void set_SyncDataMode(eSyncModes value) {
  if (syncMode == value) return;
  putSyncDataMode(value);
  syncMode = getSyncDataMode();
  addKeyToChanged("E2");  
}

// E3 - syncGroup
void set_SyncGroup(uint8_t value) {
  if (syncGroup == value) return;
  putSyncGroup(value);
  syncGroup = getSyncGroup();
  addKeyToChanged("E3");  
}

void set_SyncViewport(int8_t masterX, int8_t masterY, int8_t localX, int8_t localY, int8_t localW, int8_t localH) {
  putSyncViewport(masterX, masterY, localX, localY, localW, localH);
  loadSyncViewport();
  addKeyToChanged("EMX");  
  addKeyToChanged("EMY");  
  addKeyToChanged("ELX");  
  addKeyToChanged("ELY");  
  addKeyToChanged("ELW");  
  addKeyToChanged("ELH");  
}

#endif

void applyTimeZone(const String& value) {
  // value - правило времннОй зоны. Может впереди содержать трехзначный индекс строки в списке: 'MSK-3' или '349_MSK-3'
  // если индекс есть - нужно удалить его
  String rule = value.indexOf('_') > 0 ? value.substring(4) : value; 
  TimeProcessor::getInstance().tzsetup(rule.c_str());
}
  
