#ifndef __PWM_H
#define __PWM_H

typedef struct
{
    __IO uint32_t MODE0;
    __IO uint32_t MODE1;
    __IO uint32_t Channel;
    __IO uint32_t Duty;
    __IO uint32_t Event;
    __IO uint32_t CounterSource;
    __IO uint32_t Comparator;
    __IO uint32_t Period;
    __IO uint32_t Interrupt;
    __IO uint32_t Invert;

    // DeadBand
    __IO uint32_t DBEnable;
    __IO uint32_t DBMode;
    __IO uint32_t DBDelay;
}PWM_OUT_CONFIG;

void     PWM_Start(uint32_t nMode0, uint32_t nMode1);
void     PWM_OutputDuty(PWM_OUT_CONFIG* ppwm_out);

void     PWM_EnableOutput(uint32_t nChannelSel);
void     PWM_SetOutputInit(uint32_t nChannelSel, uint32_t nInit);
void     PWM_SetOutputEvent(uint32_t nChannelSel, uint32_t nEvent);
void     PWM_SetOutputInvert(uint32_t nChannelSel);
void     PWM_SetChannelSource(uint32_t nChannelSel, uint32_t nClockSource);
void     PWM_SetComparator(uint32_t nComparatorNum, uint32_t nTimer);
void     PWM_SetDeadBand(uint32_t nChannelSel, uint32_t nDelay, uint32_t nMode);

void     PWM_EnableADCTrigger(uint32_t nEvent);
void     PWM_DisableADCTrigger(uint32_t nEvent);

void     PWM_EnableInt(uint32_t nEvent);
void     PWM_DisableInt(uint32_t nEvent);

uint32_t PWM_GetIntFlag(void);
void     PWM_ClearIntFlag(uint32_t nChannelNum);

#define PWM_CH0A            0x0001           /* PWM channel 0A */
#define PWM_CH0B            0x0002           /* PWM channel 0B */
#define PWM_CH1A            0x0004           /* PWM channel 1A */
#define PWM_CH1B            0x0008           /* PWM channel 1B */
#define PWM_CH2A            0x0010           /* PWM channel 2A */
#define PWM_CH2B            0x0020           /* PWM channel 2B */

#define PWM_EVENT_ZERO0     0x0001
#define PWM_EVENT_ZERO1     0x0002
#define PWM_EVENT_MAX0      0x0004
#define PWM_EVENT_MAX1      0x0008
#define PWM_EVENT_DOWM0     0x0010
#define PWM_EVENT_UP0       0x0020
#define PWM_EVENT_DOWM1     0x0040
#define PWM_EVENT_UP1       0x0080
#define PWM_EVENT_DOWM2     0x0100
#define PWM_EVENT_UP2       0x0200
#define PWM_EVENT_DOWM3     0x0400
#define PWM_EVENT_UP3       0x0800

#define PWM_COMPARATOR0     0x00
#define PWM_COMPARATOR1     0x01
#define PWM_COMPARATOR2     0x02
#define PWM_COMPARATOR3     0x03

#define PWM_COUNTER0        0x00
#define PWM_COUNTER1        0x01

#define PWM_MODE_FREE       0x00
#define PWM_MODE_SINGLE     0x02
#define PWM_MODE_UP_DOWN    0x04

#define PWM_DEADBAND_MODE_LL   0x00
#define PWM_DEADBAND_MODE_LH   0x01
#define PWM_DEADBAND_MODE_HL   0x02
#define PWM_DEADBAND_MODE_HH   0x03

#endif
