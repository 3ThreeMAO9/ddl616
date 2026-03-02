#ifndef __RTC_H
#define __RTC_H

typedef struct
{
    uint32_t nSecond;
    uint32_t nMinute;
    uint32_t nHour;
    uint32_t nDate;
    uint32_t nDay;
    uint32_t nMonth;
    uint32_t nYear;
}RTC_TIME_INFO;

void     RTC_SetTime(uint32_t nTimeType, uint32_t nTimeValue);
uint32_t RTC_GetTime(uint32_t nTimeType);
void     RTC_GetFullTime(RTC_TIME_INFO *ptime_fino);
void     RTC_SetFullTime(RTC_TIME_INFO *ptime_fino);
void     RTC_GetFullAlarmTime(RTC_TIME_INFO* ptime_info);
void     RTC_SetFullAlarmTime(RTC_TIME_INFO* ptime_info);
void     RTC_SetInterrupt(uint32_t nINTType);
void     RTC_Open(void);

#define RTC_TIME_TYPE_SECOND     0
#define RTC_TIME_TYPE_MINUTE     1
#define RTC_TIME_TYPE_HOUR       2
#define RTC_TIME_TYPE_DATE       3
#define RTC_TIME_TYPE_DAY        4
#define RTC_TIME_TYPE_MONTH      5
#define RTC_TIME_TYPE_YEAR       6

#define RTC_INT_SEC       (0x01 << 0)
#define RTC_INT_MIN       (0x01 << 1)
#define RTC_INT_HOUR      (0x01 << 2)
#define RTC_INT_ALARM     (0x01 << 8)

#endif
