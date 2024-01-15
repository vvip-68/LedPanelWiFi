// This file derives from EmbUI project https://github.com/vortigont/EmbUI

#include "timeProcessor.h"

#ifdef ESP8266
 #include <coredecls.h>                 // settimeofday_cb()
 #include <TZ.h>                        // TZ declarations https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
 #include <sntp.h>

 #ifdef __NEWLIB__ 
  #if __NEWLIB__ >= 4
    extern "C" {
        #include <sys/_tz_structs.h>
    };
  #endif
 #endif
#endif

#ifdef ESP32
 #include <time.h>
 #include <esp_sntp.h>
#endif
#include "a_def_hard.h"                 // нам нужны макросы для дебага


#ifndef TZONE
#define TZONE PSTR("GMT0")              // default Time-Zone
#endif

extern bool vDEBUG_SERIAL;              // внешняя глобальная переменная включения дебага

// stub zone for a  <+-nn> names
static const char P_LOC[] PROGMEM = "LOC";

// static member must be defined outside the class
callback_function_t TimeProcessor::timecb = nullptr;

TimeProcessor::TimeProcessor()
{
// time set event handler
#ifdef ESP8266
    settimeofday_cb( [this]{ timeavailable();} );
#endif
   
#ifdef ESP32
    sntp_set_time_sync_notification_cb( [](struct timeval *tv){ timeavailable(tv);} );
#ifdef ESP_ARDUINO_VERSION
    sntp_servermode_dhcp(1);
#endif
#endif

    configTzTime(TZONE, ntp1, ntp2, userntp ? userntp->data() : NULL);
    sntp_stop();    // отключаем ntp пока нет подключения к AP

#ifdef ESP32
    // hook up WiFi events handler
    WiFi.onEvent(std::bind(&TimeProcessor::_onWiFiEvent, this, std::placeholders::_1, std::placeholders::_2));
#endif
}

String TimeProcessor::getFormattedShortTime()
{
    char buffer[6];
    sprintf_P(buffer,PSTR("%02u:%02u"), localtime(now())->tm_hour, localtime(now())->tm_min);
    return String(buffer);
}

/**
 * Set current system time from a string "YYYY-MM-DDThh:mm:ss"    [19]
 */
time_t TimeProcessor::setTime(const char *datetimestr){
    if (!datetimestr) return 0;
    //"YYYY-MM-DDThh:mm:ss"    [19]
    DEBUG(F("Время установлено: ")); DEBUGLN(datetimestr);

    struct tm tmStruct;
    memset(&tmStruct, 0, sizeof(tmStruct));
    strptime(datetimestr, strlen(datetimestr) < 19 ? "%Y-%m-%dT%H:%M" : "%Y-%m-%dT%H:%M:%S", &tmStruct);

    time_t time = mktime(&tmStruct);
    timeval tv = { time, 0 };
    settimeofday(&tv, NULL);
    return time;
}


/**
 * установки системной временной зоны/правил сезонного времени.
 * по сути дублирует системную функцию setTZ, но работает сразу
 * со строкой из памяти, а не из PROGMEM
 * Может использоваться для задания настроек зоны/правил налету из
 * браузера/апи вместо статического задания Зоны на этапе компиляции
 * @param tz - указатель на строку в формате TZSET(3)
 * набор отформатированных строк зон под прогмем лежит тут
 * https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
 */
void TimeProcessor::tzsetup(const char* tz){
    if (!tz || !*tz)
        return;

    /*
     * newlib has issues with TZ strings with quoted <+-nn> names 
     * this has been fixed in https://github.com/esp8266/Arduino/pull/7702 for esp8266 (still not in stable as of Nov 16 2020)
     * but it also affects ESP32 and who knows when to expect a fix there
     * So let's fix such zones in-place untill core support for both platforms available
     */
    if (tz[0] == 0x3C){     // check if first char is '<'
      String _tz(tz);
      String _tzfix(FPSTR(P_LOC));
      if (_tz.indexOf('<',1) > 0){  // there might be two <> quotes
    	//LOG(print, "2nd pos: "); LOG(println, _tz.indexOf('<',1)); 
        _tzfix += _tz.substring(_tz.indexOf('>')+1, _tz.indexOf('<',1));
        _tzfix += FPSTR(P_LOC);
      }
      _tzfix += _tz.substring(_tz.lastIndexOf('>')+1, _tz.length());
      setenv("TZ", _tzfix.c_str(), 1/*overwrite*/);
      //LOG(printf_P, PSTR("TIME: TZ fix applied: %s\n"), _tzfix.c_str());
    } else {
      setenv("TZ", tz, 1/*overwrite*/);
    }

    tzset();
    DEBUGLOG(printf_P, PSTR("TIME: Правило временной зоны: %s\n"), tz);
}


/**
 * обратный вызов при подключении к WiFi точке доступа
 * запускает синхронизацию времени
 */
#ifdef ESP8266
void TimeProcessor::onSTAGotIP([[maybe_unused]] const WiFiEventStationModeGotIP ipInfo)
{
    sntp_init();
}

void TimeProcessor::onSTADisconnected([[maybe_unused]] const WiFiEventStationModeDisconnected event_info)
{
  sntp_stop();
}
#endif  //ESP8266

#ifdef ESP32
void TimeProcessor::_onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info){
    switch (event){
    case SYSTEM_EVENT_STA_GOT_IP:
        sntp_setservername(1, (char*)ntp2);
        if (userntp)
            sntp_setservername(CUSTOM_NTP_INDEX, userntp->data());
        sntp_init();
        DEBUGLOG(println, F("\nTIME: Синхронизация с NTP"));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        sntp_stop();
        break;
    default:
        break;
    }
}
#endif  //ESP32


#ifdef ESP8266
void TimeProcessor::timeavailable()
#endif
#ifdef ESP32
void TimeProcessor::timeavailable(struct timeval *t)
#endif
{
    DEBUGLOG(print, F("\nTIME: Получено время NTP: "));
    String s;
    getDateTimeString(s);
    DEBUGLOG(println, s.c_str());
    if(timecb)
        timecb();
}

/**
 * функция допечатывает в переданную строку передайнный таймстамп даты/времени в формате "YYYY-MM-DDThh:mm"
 * @param _tstamp - преобразовать заданный таймстамп, если не задан используется текущее локальное время
 */
void TimeProcessor::getDateTimeString(String &buf, const time_t _tstamp){
  char tmpBuf[DATETIME_STRLEN];
  const tm* tm = localtime(  _tstamp ? &_tstamp : now());
  sprintf_P(tmpBuf,PSTR("%04u-%02u-%02uT%02u:%02u"), tm->tm_year + TM_BASE_YEAR, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
  buf.concat(tmpBuf);
}

/**
 * установка текущего смещения от UTC в секундах
 */
void TimeProcessor::setOffset(const int val){
    DEBUGLOG(printf_P, PSTR("TIME: Смещение от UTC: %d секунд\n"), val);

    #ifdef ESP8266
        sntp_set_timezone_in_seconds(val);
    #elif defined ESP32
        //setTimeZone((long)val, 0);    // this breaks linker in some weird way
        configTime((long)val, 0, ntp1, ntp2);
    #endif
}

/**
 * Возвращает текущее смещение локального системного времени от UTC в секундах
 * с учетом часовой зоны и правил смены сезонного времени (если эти параметры были
 * корректно установленно ранее каким-либо методом)
 */
long int TimeProcessor::getOffset(){
    const tm* tm = localtime(now());
    auto tz = __gettzinfo();
    return *(tm->tm_isdst == 1 ? &tz->__tzrule[1].offset : &tz->__tzrule[0].offset);
}

void TimeProcessor::setcustomntp(const char* ntp){
    if (!ntp) return;
    if (!userntp)
        userntp = new std::string(ntp);
    else
        *userntp = ntp;
    sntp_setservername(CUSTOM_NTP_INDEX, userntp->data());
    DEBUGLOG(printf, "TIME: Сервер NTP: %s\n", userntp->data());
}

/**
 * @brief - retreive NTP server name or IP
 */
String TimeProcessor::getserver(uint8_t idx){
    if (sntp_getservername(idx)){
        return String(sntp_getservername(idx));
    } else {
#ifdef ESP8266
        IPAddress addr(sntp_getserver(idx));
#endif
#ifdef ESP32
        const ip_addr_t * _ip = sntp_getserver(idx);
        IPAddress addr(_ip->u_addr.ip4.addr);
#endif
        return addr.toString();
    }
};

/**
 * Attach user-defined call-back function that would be called on time-set event
 * 
 */
void TimeProcessor::attach_callback(callback_function_t callback){
    timecb = std::move(callback);
}

void TimeProcessor::ntpodhcp(bool enable){
    sntp_servermode_dhcp(enable);

    if (!enable){
        DEBUGLOG(println, F("TIME: NTP over DHCP - выключено"));
        sntp_setservername(0, (char*)ntp1);
        sntp_setservername(1, (char*)ntp2);
        if (userntp && userntp->length())
            sntp_setservername(CUSTOM_NTP_INDEX, userntp->data());
    }
};

void TimeProcessor::enable(){
    sntp_init();
}

void TimeProcessor::disable(){
    sntp_stop();
}

// the hour now (localtime)
int hour(){
    return localtime(TimeProcessor::getInstance().now())->tm_hour;
}

// the hour for the given time
int hour(time_t t){
    return localtime(&t)->tm_hour;
}

int minute(){
    return localtime(TimeProcessor::getInstance().now())->tm_min;
}

int minute(time_t t){
    return localtime(&t)->tm_min;
}

int second(){
    return localtime(TimeProcessor::getInstance().now())->tm_sec;
}

int second(time_t t){
    return localtime(&t)->tm_sec;
}

int day(){
    return localtime(TimeProcessor::getInstance().now())->tm_mday;
}

int day(time_t t){
    return localtime(&t)->tm_mday;
}

int month(){
    return localtime(TimeProcessor::getInstance().now())->tm_mon + 1;
}

int month(time_t t){
    return localtime(&t)->tm_mon + 1;
}

int year(){
    return localtime(TimeProcessor::getInstance().now())->tm_year + 1900;
}

int year(time_t t){
    return localtime(&t)->tm_year + 1900;
}

int weekday(){
    return localtime(TimeProcessor::getInstance().now())->tm_wday;
}

int weekday(time_t t){
    return localtime(&t)->tm_wday;
}

uint8_t isAM() { // returns true if time now is AM
    return !isPM(*TimeProcessor::getInstance().now()); 
}

uint8_t isAM(time_t t) { // returns true if given time is AM
    return !isPM(t);  
}

uint8_t isPM() { // returns true if PM
    return isPM(*TimeProcessor::getInstance().now()); 
}

uint8_t isPM(time_t t) { // returns true if PM
    return (hour(t) >= 12); 
}

const time_t* now() { 
    return TimeProcessor::now(); 
};

// leap year calulator expects year argument as years offset from 1970
bool isLeapYear(uint16_t Y) {
   return ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) );
}
