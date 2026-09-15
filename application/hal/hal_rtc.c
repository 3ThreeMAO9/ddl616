#include "hal_rtc.h"
#include "timestamp.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_rtc"

/***********Variable***********/
static rtc_callback_t rtc_callback;
static volatile rtc_handlet_t rtc_handle;
static rtc_time_info_t rtc_time;
// ----------------------------

void rtc_IRQ_callback(void)
{
    rtc_handle.interrupt++;
    rtc_time.local_timestamp++;       // 本地时间戳累加
    // hal_get_rtc_time();
    if (NULL != rtc_callback)
        rtc_callback();
}

uint32_t hal_get_rtc_interrupt(void)
{
    return rtc_handle.interrupt;
}

void hal_set_rtc_interrupt(uint32_t data)
{
    // OB_LOGD(TAG, "Set rtc interrupt: %u", data);
    rtc_handle.interrupt = data;
}

void hal_set_rtc_local_timestamp(uint32_t timestamp)
{
    OB_LOGI(TAG, "Set rtc local timestamp: %u", timestamp);
    rtc_time.local_timestamp = timestamp;
}

time_t hal_get_rtc_local_timestamp(void)
{
    // OB_LOGW(TAG, "Get rtc local timestamp: %u", rtc_time.local_timestamp);
    return rtc_time.local_timestamp;
}

void hal_set_rtc_time_zone(int16_t time_zone)
{
    OB_LOGI(TAG, "Set rtc time zone: %d", time_zone);
    rtc_time.time_zone = time_zone;
}

int16_t hal_get_rtc_time_zone(void)
{
    OB_LOGD(TAG, "Get rtc time zone: %d", rtc_time.time_zone);
    return rtc_time.time_zone;
}

void hal_set_rtc_utc_timestamp(uint32_t timestamp)
{
    OB_LOGI(TAG, "Set rtc utc timestamp: %u", timestamp);
    rtc_time.utc_timestamp = timestamp;
}

time_t hal_get_rtc_utc_timestamp(void)
{
    OB_LOGI(TAG, "Get rtc utc timestamp: %u", rtc_time.utc_timestamp);
    return rtc_time.utc_timestamp;
}

void hal_set_rtc_time(time_t timestamp, int16_t time_zone)
{
    rtc_time.time_zone = time_zone;
    rtc_time.utc_timestamp = timestamp;
    rtc_time.local_timestamp = rtc_time.utc_timestamp + ((int32_t)rtc_time.time_zone * 15 * 60);
    
    OB_LOGI(TAG, "Set rtc full time - UTC:%u, TZ:%d, Local:%u", 
            rtc_time.utc_timestamp, rtc_time.time_zone, rtc_time.local_timestamp);
}

uint32_t hal_get_rtc_time(void)
{
    timestamp_to_data_time(rtc_time.local_timestamp);
    return rtc_time.local_timestamp;
    // RTC_TIME_INFO time_info;

    // RTC_GetFullTime(&time_info);
    // OB_LOGD(TAG, "20%d-%02d-%02d  %02d:%02d:%02d", time_info.nYear, time_info.nMonth, time_info.nDate,
    //         time_info.nHour, time_info.nMinute, time_info.nSecond);
}

uint32_t hal_get_rtc_utc_time(void)
{
    rtc_time.utc_timestamp = rtc_time.local_timestamp - ((int32_t)rtc_time.time_zone * 15 * 60);
    timestamp_to_data_time(rtc_time.utc_timestamp);
    
    OB_LOGI(TAG, "Get rtc utc time: %u", rtc_time.utc_timestamp);
    return rtc_time.utc_timestamp;
}

time_t hal_get_rtc_to_utc_time(time_t timestamp)
{
    time_t utc_time = timestamp + ((int16_t)rtc_time.time_zone * 15 * 60);
    OB_LOGD(TAG, "Local to UTC - In:%u, TZ:%d, Out:%u", timestamp, rtc_time.time_zone, utc_time);
    return utc_time;
}

time_t hal_set_default_time(void)
{
    // RTC_TIME_INFO tmp;

    // tmp.nSecond = 0;
    // tmp.nMinute = 0;
    // tmp.nHour = 0;
    // tmp.nDate = 1;
    // tmp.nDay = 0;
    // tmp.nMonth = 1;
    // tmp.nYear = (__DATE__[9] - '0') * 10 + (__DATE__[10] - '0');
    // RTC_SetFullTime(&tmp);

    struct tm tm_info = {0};
    time_t timestamp;
    tm_info.tm_year = 2000 + SYSTEM_TIME_YEAR_DEFAULT - 1900;
    tm_info.tm_mon = SYSTEM_TIME_MONTH_DEFAULT - 1;
    tm_info.tm_mday = SYSTEM_TIME_DAY_DEFAULT;
    tm_info.tm_hour = SYSTEM_TIME_HOUR_DEFAULT;
    tm_info.tm_min = SYSTEM_TIME_MINUTE_DEFAULT;
    tm_info.tm_sec= SYSTEM_TIME_SECOND_DEFAULT;
    timestamp = mktime(&tm_info);

    OB_LOGI(TAG, "Generate default timestamp: %u (Date: %04d-%02d-%02d %02d:%02d:%02d)", 
            timestamp, 
            2000 + SYSTEM_TIME_YEAR_DEFAULT, SYSTEM_TIME_MONTH_DEFAULT, SYSTEM_TIME_DAY_DEFAULT,
            SYSTEM_TIME_HOUR_DEFAULT, SYSTEM_TIME_MINUTE_DEFAULT, SYSTEM_TIME_SECOND_DEFAULT);
            
    return timestamp;
}

void hal_rtc_init(rtc_callback_t callback)
{
    RTC_Open();
    RTC_SetInterrupt(RTC_INT_SEC);
    rtc_callback = callback;
    
    OB_LOGI(TAG, "HAL RTC Init success. Intr:ENABLE, Callback registered.");
    OB_LOGD(TAG, "Current UTC: %u, Local: %u, TimeZone: %d", 
            rtc_time.utc_timestamp, rtc_time.local_timestamp, rtc_time.time_zone);
}
