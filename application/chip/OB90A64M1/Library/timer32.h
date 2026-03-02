#ifndef __TIMER32_H
#define __TIMER32_H

void     TIMER32_Enable(OB_CT32B_Type *pTimer32);
void     TIMER32_Close(OB_CT32B_Type *pTimer32);
void     TIMER32_Delay(OB_CT32B_Type *pTimer32, uint32_t nUsec);
void     TIMER32_EnableCapture(OB_CT32B_Type *pTimer32, uint32_t nCapSource, uint32_t nEdge);
void     TIMER32_DisableCapture(OB_CT32B_Type *pTimer32);
uint32_t TIMER32_GetModuleClock(OB_CT32B_Type *pTimer32);
void     TIMER32_SetExternalMatch(OB_CT32B_Type *pTimer32, uint32_t nMatchSel, uint32_t nMode);
void     TIMER32_ToggleOut(OB_CT32B_Type *pTimer32, uint32_t nMatchSel, uint32_t nUsec);
void     TIMER32_EnableInterrupt(OB_CT32B_Type *pTimer32, uint32_t nMRSel, uint32_t nMode);
void     TIMER32_Delay_Interrupt(OB_CT32B_Type *pTimer32, uint32_t nMRSel,uint32_t nMode,  uint32_t nUsec);
void     TIMER32_PWMSel(OB_CT32B_Type *pTimer32, uint32_t nMatchSel, uint32_t ModeSel);
void     TIMER32_PWM_Length(OB_CT32B_Type *pTimer32, uint32_t nMatchSel ,uint32_t nUsec, uint32_t PlusMode);

#define TIMER32_ENABLE          0x01
#define TIMER32_RESET           0x02

#define TIMER32_CAPTURE0        0x00
#define TIMER32_CAPTURE1        0x01

#define TIMER32_MATCHS_INT      (1<< 0x00)
#define TIMER32_MATCHS_RESET    (1<< 0x01)
#define TIMER32_MATCHS_STOP     (1<< 0x02)


#define TIMER32_EMATCH0         0x01
#define TIMER32_EMATCH1         0x02
#define TIMER32_EMATCH2         0x04
#define TIMER32_EMATCH3         0x08

#define TIMER32_EMATCH_NOTHING  0x00
#define TIMER32_EMATCH_LOW      0x01
#define TIMER32_EMATCH_HIGH     0x02
#define TIMER32_EMATCH_TOGGLE   0x03

#define TIMER32_CAPTURE_FALLING_EDGE    0x01    /* Falling edge trigger timer capture */
#define TIMER32_CAPTURE_RISING_EDGE     0x02    /* Rising edge trigger timer capture */

#define TIMER32_MR0         0x00
#define TIMER32_MR1         0x01
#define TIMER32_MR2         0x02
#define TIMER32_MR3         0x03

#define TIMER32_MR_I        ( 0x01 << 0x00 )
#define TIMER32_MR_R        ( 0x01 << 0x01 )
#define TIMER32_MR_S        ( 0x01 << 0x02 )

#define TIMER32_PWM_ENABLE 1

#define TIMER32_PWM_LENGTH_HL 0x00
#define TIMER32_PWM_LENGTH_L 0x01


#endif
