#include "OB90A64M1.h"
#include "rtc.h"

extern void rtc_IRQ_callback(void);

void RTC_IRQHandler(void)
{
    OB_RTC->IESTC = OB_RTC->IEST;
    rtc_IRQ_callback();
}

void RTC_SetTime(uint32_t nTimeType, uint32_t nTimeValue)
{
    switch (nTimeType)
    {
    case RTC_TIME_TYPE_SECOND:
        OB_RTC->SEC   = (nTimeValue/10)<<4;
        OB_RTC->SEC  |= nTimeValue%10;
        break;

    case RTC_TIME_TYPE_MINUTE:
        OB_RTC->MIN   = (nTimeValue/10)<<4;
        OB_RTC->MIN  |= nTimeValue%10;
        break;

    case RTC_TIME_TYPE_HOUR:
        OB_RTC->HOUR  = (nTimeValue/10)<<4;
        OB_RTC->HOUR |= nTimeValue%10;
        break;

    case RTC_TIME_TYPE_DATE:
        OB_RTC->DATE  = (nTimeValue/10)<<4;
        OB_RTC->DATE |= nTimeValue%10;
        break;

    case RTC_TIME_TYPE_DAY:
        OB_RTC->DAY   = nTimeValue;
        break;

    case RTC_TIME_TYPE_MONTH:
        OB_RTC->MON   = (nTimeValue/10)<<4;
        OB_RTC->MON  |= nTimeValue%10;
        break;

    case RTC_TIME_TYPE_YEAR:
        OB_RTC->MON   = (nTimeValue/10)<<4;
        OB_RTC->MON  |= nTimeValue%10;
        break;

    default:
        break;
    }
    OB_RTC->WTKEY1 = 0x00;
    OB_RTC->WTKEY2 = 0x0B;
}

uint32_t RTC_GetTime(uint32_t nTimeType)
{
    volatile uint32_t nReadLatch = OB_RTC->LATCH; // Read once first for latch.

    uint32_t nTime = 0;

    switch (nTimeType)
    {
    case RTC_TIME_TYPE_SECOND:
        nTime = (OB_RTC->SEC>>4)*10 + (OB_RTC->SEC&0x0F);
        break;

    case RTC_TIME_TYPE_MINUTE:
        nTime = (OB_RTC->MIN>>4)*10 + (OB_RTC->MIN&0x0F);
        break;

    case RTC_TIME_TYPE_HOUR:
        nTime = (OB_RTC->HOUR>>4)*10 + (OB_RTC->HOUR&0x0F);
        break;

    case RTC_TIME_TYPE_DATE:
        nTime = (OB_RTC->DATE>>4)*10 + (OB_RTC->DATE&0x0F);
        break;

    case RTC_TIME_TYPE_DAY:
        nTime = OB_RTC->DAY;
        break;

    case RTC_TIME_TYPE_MONTH:
        nTime = (OB_RTC->MON>>4)*10 + (OB_RTC->MON&0x0F);
        break;

    case RTC_TIME_TYPE_YEAR:
        nTime = (OB_RTC->YEAR>>4)*10 + (OB_RTC->YEAR&0x0F);
        break;

    default:
        return (0);
    }

    return nTime;
}

void RTC_GetFullTime(RTC_TIME_INFO* ptime_info)
{
    volatile uint32_t nReadLatch = OB_RTC->LATCH; // Read once first for latch.

    ptime_info->nSecond = (OB_RTC->SEC  >>4)*10 + (OB_RTC->SEC &0x0F);
    ptime_info->nMinute = (OB_RTC->MIN  >>4)*10 + (OB_RTC->MIN &0x0F);
    ptime_info->nHour   = (OB_RTC->HOUR >>4)*10 + (OB_RTC->HOUR&0x0F);
    ptime_info->nDate   = (OB_RTC->DATE >>4)*10 + (OB_RTC->DATE&0x0F);
    ptime_info->nDay    = (OB_RTC->DAY);
    ptime_info->nMonth  = (OB_RTC->MON  >>4)*10 + (OB_RTC->MON &0x0F);
    ptime_info->nYear   = (OB_RTC->YEAR >>4)*10 + (OB_RTC->YEAR&0x0F);
}

void RTC_SetFullTime (RTC_TIME_INFO* ptime_info)
{

    OB_RTC->SEC    = ((ptime_info->nSecond/10)<<4) + (ptime_info->nSecond %10);
    OB_RTC->MIN    = ((ptime_info->nMinute/10)<<4) + (ptime_info->nMinute %10);
    OB_RTC->HOUR   = ((ptime_info->nHour  /10)<<4) + (ptime_info->nHour   %10);
    OB_RTC->DATE   = ((ptime_info->nDate  /10)<<4) + (ptime_info->nDate   %10);
    OB_RTC->DAY    = ((ptime_info->nDay));
    OB_RTC->MON    = ((ptime_info->nMonth /10)<<4) + (ptime_info->nMonth  %10);
    OB_RTC->YEAR   = ((ptime_info->nYear  /10)<<4) + (ptime_info->nYear   %10);

    OB_RTC->WTKEY1 = 0x00;
    OB_RTC->WTKEY2 = 0x0B;
}

void RTC_SetFullAlarmTime(RTC_TIME_INFO* ptime_info)
{
    OB_RTC->ALSEC  = ((ptime_info->nSecond/10)<<4) + (ptime_info->nSecond %10);
    OB_RTC->ALMIN  = ((ptime_info->nMinute/10)<<4) + (ptime_info->nMinute %10);
    OB_RTC->ALHOUR = ((ptime_info->nHour  /10)<<4) + (ptime_info->nHour   %10);
}

void RTC_GetFullAlarmTime(RTC_TIME_INFO* ptime_info)
{
    OB_RTC->SEC = OB_RTC->LATCH; // for latch

    ptime_info->nSecond = (OB_RTC->ALSEC  >>4)*10 + (OB_RTC->ALSEC  &0x0F);
    ptime_info->nMinute = (OB_RTC->ALMIN  >>4)*10 + (OB_RTC->ALMIN  &0x0F);
    ptime_info->nHour   = (OB_RTC->ALHOUR >>4)*10 + (OB_RTC->ALHOUR &0x0F);
}

void RTC_SetInterrupt (uint32_t nINTType)
{
    NVIC_EnableIRQ(RTC_IRQn);
    OB_RTC->IEENS = nINTType;
}

void RTC_Open(void)
{
    // OB_SYSCON->CPUCHIPCTR0 |= 0x04;
    // OB_RTC->CON  =0x00;
    uint32_t *pCPUCHIPCTR0;

    pCPUCHIPCTR0 = (uint32_t *)0x500100B0;
    *pCPUCHIPCTR0 = *pCPUCHIPCTR0 | 0x04;
    OB_RTC->CON = 0x00;
}

