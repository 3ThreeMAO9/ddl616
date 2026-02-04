#ifndef __TIMER16_H
#define __TIMER16_H

// void     TIMER16_Enable(OB_CT16B_Type *pTimer16);
void     TIMER16_Close(OB_CT16B_Type *pTimer16);
void     TIMER16_Delay(OB_CT16B_Type *pTimer16, uint32_t nUsec);
void     TIMER16_EnableCapture(OB_CT16B_Type *pTimer16, uint32_t nCapSource, uint32_t nEdge);
void     TIMER16_DisableCapture(OB_CT16B_Type *pTimer16);
uint32_t TIMER16_GetModuleClock(OB_CT16B_Type *pTimer16);
void     TIMER16_SetExternalMatch(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t nMode);
void     TIMER16_ToggleOut(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t nUsec);
// void     TIMER16_EnableInterrupt(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode);
void     TIMER16_Delay_Interrupt(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode,  uint32_t nUsec);
void TIMER16_PWMSel(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t ModeSel);
void TIMER16_PWM_Length(OB_CT16B_Type *pTimer16, uint32_t nMatchSel ,uint32_t nUsec, uint32_t PlusMode);
// void TIMER16_CloseChannel(OB_CT16B_Type *pTimer16, uint32_t nMRSel);
// void TIMER16_EnableChannel(OB_CT16B_Type *pTimer16, uint32_t nMRSel);

// 底层：定时器中断使能配置宏（操作TMR16MCR寄存器，3位一组配置通道）
#define TIMER16_EnableInterrupt(pTimer16, nMRSel, nMode)                                        \
    do                                                                                          \
    {                                                                                           \
        uint32_t nShift = (nMRSel) * 3;                                                         \
        (pTimer16)->TMR16MCR = ((pTimer16)->TMR16MCR & (~(7 << nShift))) | ((nMode) << nShift); \
    } while (0)

// 上层：使能单个定时器通道（中断+匹配复位，等价原TIMER16_EnableChannel函数）
#define TIMER16_EnableChannel(pTimer16, nMRSel)                                  \
    do                                                                           \
    {                                                                            \
        TIMER16_EnableInterrupt(pTimer16, nMRSel, (TIMER16_MR_I | TIMER16_MR_R)); \
    } while (0)

// 上层：关闭单个定时器通道（清除所有配置，等价原TIMER16_CloseChannel函数）
#define TIMER16_CloseChannel(pTimer16, nMRSel)          \
    do                                                  \
    {                                                   \
        TIMER16_EnableInterrupt(pTimer16, nMRSel, 0x00); \
    } while (0)

#define TIMER16_Enable(pTimer16)                \
    do                                          \
    {                                           \
        (pTimer16)->TMR16TCR |= TIMER16_RESET;  \
        (pTimer16)->TMR16TCR |= TIMER16_ENABLE; \
    } while (0)

#define TIMER16_IS_CHANNEL_ENABLED(pTimer16, nMRSel)  \
    ( ( (pTimer16)->TMR16MCR >> ( (nMRSel) * 3 ) ) & 0x01 )

#define TIMER16_ENABLE          0x01
#define TIMER16_RESET           0x02

#define TIMER16_EMATCH0         0x01
#define TIMER16_EMATCH1         0x02
#define TIMER16_EMATCH2         0x04
#define TIMER16_EMATCH3         0x08

#define TIMER16_EMATCH_NOTHING  0x00
#define TIMER16_EMATCH_LOW      0x01
#define TIMER16_EMATCH_HIGH     0x02
#define TIMER16_EMATCH_TOGGLE   0x03

#define TIMER16_CAPTURE0        0x00
#define TIMER16_CAPTURE1        0x01

#define TIMER16_MATCHS_INT      (1<< 0x00)
#define TIMER16_MATCHS_RESET    (1<< 0x01)
#define TIMER16_MATCHS_STOP     (1<< 0x02)

#define TIMER16_CAPTURE_FALLING_EDGE    0x02    /* Falling edge trigger timer capture */
#define TIMER16_CAPTURE_RISING_EDGE     0x01    /* Rising edge trigger timer capture */
#define TIMER16_CAPTURE_INTERRUPT       0x04

#define TIMER16_MR0         0x00
#define TIMER16_MR1         0x01
#define TIMER16_MR2         0x02
#define TIMER16_MR3         0x03

#define TIMER16_MR_I        ( 0x01 << 0x00 )
#define TIMER16_MR_R        ( 0x01 << 0x01 )
#define TIMER16_MR_S        ( 0x01 << 0x02 )

#define TIMER16_EMATCH0         0x01
#define TIMER16_EMATCH1         0x02
#define TIMER16_EMATCH2         0x04
#define TIMER16_EMATCH3         0x08

#define TIMER16_EMATCH_NOTHING  0x00
#define TIMER16_EMATCH_LOW      0x01
#define TIMER16_EMATCH_HIGH     0x02
#define TIMER16_EMATCH_TOGGLE   0x03

#define TIMER16_PWM_ENABLE 1

#define TIMER16_PWM_LENGTH_HL 0x00
#define TIMER16_PWM_LENGTH_L 0x01
#endif
