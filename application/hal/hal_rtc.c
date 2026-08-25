#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_rtc"

/***********Variable***********/
static rtc_callback_t rtc_callback;

// ----------------------------

void rtc_IRQ_callback(void)
{
    if (NULL != rtc_callback)
        rtc_callback();
}

void hal_set_default_time(void)
{
    RTC_TIME_INFO tmp;

    tmp.nSecond = 0;
    tmp.nMinute = 0;
    tmp.nHour = 0;
    tmp.nDate = 1;
    tmp.nDay = 0;
    tmp.nMonth = 1;
    tmp.nYear = (__DATE__[9] - '0') * 10 + (__DATE__[10] - '0');
    RTC_SetFullTime(&tmp);
}

void hal_get_rtc_time(void)
{
    RTC_TIME_INFO time_info;

    RTC_GetFullTime(&time_info);
    OB_LOGD(TAG, "20%d-%02d-%02d  %02d:%02d:%02d", time_info.nYear, time_info.nMonth, time_info.nDate,
            time_info.nHour, time_info.nMinute, time_info.nSecond);
}

void hal_rtc_init(rtc_callback_t callback)
{
    RTC_Open();
    hal_set_default_time();
    RTC_SetInterrupt(RTC_INT_SEC);
    rtc_callback = callback;
}
