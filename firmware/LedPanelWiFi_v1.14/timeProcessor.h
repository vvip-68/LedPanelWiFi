#pragma once
#ifdef ESP32
#include <WiFi.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif


/*
 * COUNTRY macro allows to select a specific country pool for ntp requests, like ru.pool.ntp.org, eu.pool.ntp.org, etc...
 * otherwise a general pool "pool.ntp.org" is used as a fallback and vniiftri.ru's ntp is used as a primary
 * 
 */
#if !defined NTP1ADDRESS && !defined NTP2ADDRESS
#ifdef COUNTRY
    #define NTP1ADDRESS        COUNTRY ".pool.ntp.org"      // пул серверов времени для NTP
    #define NTP2ADDRESS        "ntp3.vniiftri.ru"           // https://vniiftri.ru/catalog/services/sinkhronizatsiya-vremeni-cherez-ntp-servera/
#else
    #define NTP1ADDRESS        "ntp3.vniiftri.ru"
    #define NTP2ADDRESS        ("pool.ntp.org")
#endif
#endif

#define CUSTOM_NTP_INDEX    2

#define TM_BASE_YEAR        1900
#define DAYSECONDS          (86400U)
#define DATETIME_STRLEN     (20U)   // ISO data/time string "YYYY-MM-DDThh:mm:ss", seconds optional

using callback_function_t = std::function<void(void)>;

// TimeProcessor class is a Singleton
class TimeProcessor
{
private:
    TimeProcessor();

    const char* ntp1 = NTP1ADDRESS;
    const char* ntp2 = NTP2ADDRESS;
    String userntp;                   // user defined NTP server

#ifdef ESP32
    /**
     * обратный вызов при подключении к WiFi точке доступа
     * запускает синхронизацию времени
     */
    void _onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
#endif

protected:
    static callback_function_t timecb;

    /**
     * Timesync callback
     */
#ifdef ESP8266
    static void timeavailable();
#endif
#ifdef ESP32
    static void timeavailable(struct timeval *t);
#endif


public:
    // this is a singleton
    TimeProcessor(TimeProcessor const&) = delete;
    void operator=(TimeProcessor const&) = delete;


    /**
     * обратный вызов при подключении к WiFi точке доступа
     * запускает синхронизацию времени
     */
#ifdef ESP8266
    void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
    void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
#endif

    /**
     * obtain a pointer to singleton instance
     */
    static TimeProcessor& getInstance(){
        static TimeProcessor inst;
        return inst;
    }

    /**
     * Функция установки системного времени, принимает в качестве аргумента указатель на строку в формате
     * "YYYY-MM-DDThh:mm:ss"
     */
    static time_t setTime(const char* datetimestr);

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
    void tzsetup(const char* tz);

    /**
     * установка пользовательского ntp-сервера
     * сервер имеет низший приоритет
     * @param ntp - сервер в виде ip или hostname
     */
    void setcustomntp(const char* ntp);

    /**
     * @brief - retreive NTP server name or IP
     */
    String getserver(uint8_t idx);

    /**
     *  установка смещения текущего системного времени от UTC в секундах
     *  можно применять если не получается выставить нормально зону
     *  и правила перехода сезонного времени каким-либо другим способом.
     *  При уставке правила перехода сезонного времени (если были) сбрасываются!
     */
    void setOffset(const int val);

    /**
     * Attach user-defined call-back function that would be called on time-set event
     * 
     */
    void attach_callback(callback_function_t callback);

    void dettach_callback(){
        timecb = nullptr;
    }

    /**
     * Возвращает текущее смещение локального системного времени от UTC в секундах
     * с учетом часовой зоны и правил смены сезонного времени (если эти параметры были
     * корректно установленны ранее каким-либо методом)
     */
    static long int getOffset();

    /**
     *  возвращает текуший unixtime
     */
    static time_t getUnixTime() {return time(nullptr); }

    /**
     * возвращает строку с временем в формате "00:00"
     */
    String getFormattedShortTime();

    /** 
     * Количество часов, начиная с полуночи.  0 — 23 
     */
    int getHours()
    {
        return localtime(now())->tm_hour;
    }

    /** 
     * Количество минут в одном часе.  0 — 59 
     */
    int getMinutes()
    {
        return localtime(now())->tm_min;
    }
    
    /**
     * функция допечатывает в переданную строку заданный таймстамп в дату/время в формате "9999-99-99T99:99"
     * @param _tstamp - преобразовать заданный таймстамп, если не задан используется текущее локальное время
     */
    static void getDateTimeString(String &buf, const time_t tstamp = 0);

    /**
     * returns pointer to current unixtime
     * (удобно использовать для передачи в localtime())
     */
    static const time_t* now(){
        static time_t _now;
        time(&_now);
        return &_now;
    }

    /**
     * возвращает true если текущее время :00 секунд 
     */
    static bool seconds00(){
        if ((localtime(now())->tm_sec))
          return false;
        else
          return true;
    }

    /**
     * @brief enable/disable NTP over DHCP
     */
    void ntpodhcp(bool enable);

    /**
     * @brief enable ntp sync
     * 
     */
    void enable();


    /**
     * @brief disable ntp sync
     * 
     */
    void disable();

};


// a bunch of wrappers from a TimeLib to make vvip happy :)

/* Useful Constants */
#define SECS_PER_MIN  ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY  ((time_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_t)(7UL))
#define SECS_PER_WEEK ((time_t)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((time_t)(SECS_PER_DAY * 365UL)) // TODO: ought to handle leap years
#define SECS_YR_2000  ((time_t)(946684800UL)) // the time at the start of y2k

int hour();            // the hour now 
int hour(time_t t);    // the hour for the given time
int minute();
int minute(time_t t);
int second();
int second(time_t t);
int day();
int day(time_t t);
int month();
int month(time_t t);
int year();
int year(time_t t);
int weekday();
int weekday(time_t t);

uint8_t isAM();            // returns true if time now is AM
uint8_t isAM(time_t t);    // returns true the given time is AM
uint8_t isPM();            // returns true if time now is PM
uint8_t isPM(time_t t);    // returns true the given time is PM

// возвращает указатель на переменную time_t - текущее время в системе в UTC
const time_t* now();

// leap year calulator expects year argument as years offset from 1970
bool isLeapYear(uint16_t Y);
