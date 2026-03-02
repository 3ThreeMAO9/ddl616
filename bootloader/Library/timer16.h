#ifndef __TIMER16_H
#define __TIMER16_H

typedef void(*timer16_callback_t)(void);

void     TIMER16_Enable(OB_CT16B_Type *pTimer16);
void     TIMER16_Close(OB_CT16B_Type *pTimer16);
void     TIMER16_Delay(OB_CT16B_Type *pTimer16, uint32_t nUsec);
void     TIMER16_EnableCapture(OB_CT16B_Type *pTimer16, uint32_t nCapSource, uint32_t nEdge);
void     TIMER16_DisableCapture(OB_CT16B_Type *pTimer16);
uint32_t TIMER16_GetModuleClock(OB_CT16B_Type *pTimer16);
void     TIMER16_SetExternalMatch(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t nMode);
void     TIMER16_ToggleOut(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t nUsec);
void     TIMER16_EnableInterrupt(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode);
void     TIMER16_Delay_Interrupt(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode,  uint32_t nUsec, timer16_callback_t callback);
void TIMER16_PWMSel(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t ModeSel);
void TIMER16_PWM_Length(OB_CT16B_Type *pTimer16, uint32_t nMatchSel ,uint32_t nUsec, uint32_t PlusMode);

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
