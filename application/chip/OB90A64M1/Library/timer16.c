#include "OB90A64M1.h"
#include "timer16.h"
#include "gpio.h"

volatile uint32_t g_T16B0Interrupt;
volatile uint32_t g_T16B1Interrupt;

extern void hal_callback(unsigned char ucChannel);
extern void SoftUART_IRQHandler(void);

void CT16B0_IRQHandler()
{
    OB_CT16B0->TMR16IR = 0xFF; //clear interrupt
    g_T16B0Interrupt++;
    hal_callback(0);
}

void CT16B1_IRQHandler()
{
    // OB_GPIO0->DATA_b.GPIOXDATA8 = ~OB_GPIO0->DATA_b.GPIOXDATA8;
    // OB_GPIO0->DATA |= GPIO_PIN8;
    // OB_GPIO0->DATA &= (~GPIO_PIN8);
    OB_GPIO2->DATA |= GPIO_PIN2;   //test irq time
    SoftUART_IRQHandler();
    OB_GPIO2->DATA &= (~GPIO_PIN2);   //test irq time
}

// void TIMER16_Enable(OB_CT16B_Type *pTimer16)
// {
//     pTimer16->TMR16TCR = pTimer16->TMR16TCR | TIMER16_RESET;
//     pTimer16->TMR16TCR = pTimer16->TMR16TCR | TIMER16_ENABLE; //Enable counter
// }

void TIMER16_Close(OB_CT16B_Type *pTimer16)
{
    pTimer16->TMR16TCR = TIMER16_RESET;
}

void TIMER16_Delay(OB_CT16B_Type *pTimer16, uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer16 == OB_CT16B0)
        nPerClock = GetPeripheralClock(APB_T16B0);
    else
        nPerClock = GetPeripheralClock(APB_T16B1);

    nClock = (( (nPerClock /100000 ) *  nUsec)/10);
    pTimer16->TMR16PR = 0x00;
    pTimer16->TMR16MCR = TIMER16_MATCHS_INT |  TIMER16_MATCHS_RESET;
    TIMER16_Enable(pTimer16);
    while(1)
    {
        if (nClock >= 0x8000)
        {
            pTimer16->TMR16MR0 = 0x8000;
            nClock = nClock - 0x8000;
        }
        else
        {
            pTimer16->TMR16MR0 = nClock;
            nClock = 0x00;
        }
        while(!(pTimer16->TMR16IR & 0x01));
        pTimer16->TMR16IR = pTimer16->TMR16IR | 0x01;
        if (nClock == 0x00)
            break;
    }

    TIMER16_Close(pTimer16);
}

void TIMER16_ToggleOut(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer16 == OB_CT16B0)
        nPerClock = GetPeripheralClock(APB_T16B0);
    else
        nPerClock = GetPeripheralClock(APB_T16B1);

    nClock = (((nPerClock/100000) * nUsec)/10);

    pTimer16->TMR16PR = nClock / 0x10000;
    nClock = nClock / (pTimer16->TMR16PR + 1);
    if (nMatchSel == TIMER16_EMATCH0)
    {
        pTimer16->TMR16MR0 = nClock;
        pTimer16->TMR16MCR = (0x02) << 0;
    }
    else if (nMatchSel == TIMER16_EMATCH1)
    {
        pTimer16->TMR16MR1 = nClock;
        pTimer16->TMR16MCR = (0x02) << 3;
    }
    else if (nMatchSel == TIMER16_EMATCH2)
    {
        pTimer16->TMR16MR2 = nClock;
        pTimer16->TMR16MCR = (0x02) << 6;
    }
}

void TIMER16_EnableCapture(OB_CT16B_Type *pTimer16, uint32_t nCapSource, uint32_t nEdge)
{
    if (nCapSource == TIMER16_CAPTURE0)
        pTimer16->TMR16CCR = (pTimer16->TMR16CCR & 0xFFFFFFFC) | nEdge;
    else
        pTimer16->TMR16CCR = (pTimer16->TMR16CCR & 0xFFFFFFE7) | (nEdge << 0x03);
}

void TIMER16_DisableCapture(OB_CT16B_Type *pTimer16)
{
    pTimer16->TMR16CCR = 0x00;
}

uint32_t TIMER16_GetModuleClock(OB_CT16B_Type *pTimer16)
{
    uint32_t nPerClock, nCounter;

    if (pTimer16 == OB_CT16B0)
        nPerClock = GetPeripheralClock(APB_T16B0);
    else
        nPerClock = GetPeripheralClock(APB_T16B1);

    if ((pTimer16->TMR16MCR & TIMER16_MATCHS_RESET) == 0x00)
        nCounter = 0xFFFF * (pTimer16->TMR16PR + 1);
    else
        nCounter = pTimer16->TMR16MR0 * (pTimer16->TMR16PR + 1);

    return (nPerClock/nCounter);
}

void TIMER16_SetExternalMatch(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t nMode)
{
    uint32_t nBuff;

    nBuff = pTimer16->TMR16EMR;
    if ((nMatchSel & TIMER16_EMATCH0) == TIMER16_EMATCH0)
    {
        nBuff = nBuff & (~(0x03 << 0x04));
        nBuff = nBuff | (nMode << 0x04) | 0x01;
    }

    if ((nMatchSel & TIMER16_EMATCH1) == TIMER16_EMATCH1)
    {
        nBuff = nBuff & (~(0x03 << 0x06));
        nBuff = nBuff | (nMode << 0x06) | 0x02;
    }

    if ((nMatchSel & TIMER16_EMATCH2) == TIMER16_EMATCH2)
    {
        nBuff = nBuff & (~(0x03 << 0x08));
        nBuff = nBuff | (nMode << 0x08) | 0x04;
    }

    if ((nMatchSel & TIMER16_EMATCH3) == TIMER16_EMATCH3)
    {
        nBuff = nBuff & (~(0x03 << 0x0A));
        nBuff = nBuff | (nMode << 0x0A) | 0x08;
    }
    pTimer16->TMR16EMR = nBuff;
}


void TIMER16_Delay_Interrupt(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode,  uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer16 == OB_CT16B0)
        nPerClock = GetPeripheralClock(APB_T16B0);
    else
        nPerClock = GetPeripheralClock(APB_T16B1);

    nClock = (((nPerClock / 100000) * nUsec) / 10);
    pTimer16->TMR16PC  = 0x00;

    if (nMRSel == TIMER16_MR0)
        pTimer16->TMR16MR0 = nClock;

    else if (nMRSel == TIMER16_MR1)
        pTimer16->TMR16MR1 = nClock;

    else if (nMRSel == TIMER16_MR2)
        pTimer16->TMR16MR2 = nClock;

    else if (nMRSel == TIMER16_MR3)
        pTimer16->TMR16MR3 = nClock;

    TIMER16_EnableInterrupt(pTimer16, nMRSel, nMode);
    pTimer16->TMR16TCR = pTimer16->TMR16TCR | TIMER16_RESET;
    pTimer16->TMR16TCR = pTimer16->TMR16TCR | TIMER16_ENABLE; //Enable counter
}

// void TIMER16_EnableInterrupt(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode)
// {
//     uint32_t nBuff, nShift;
//     nBuff  = 7;
//     nShift = 0x00;

//     if (nMRSel == TIMER16_MR0)
//         nShift = 0x00;
//     else if (nMRSel == TIMER16_MR1)
//         nShift = 0x03;
//     else if (nMRSel == TIMER16_MR2)
//         nShift = 0x06;
//     else if (nMRSel == TIMER16_MR3)
//         nShift = 0x09;

//     nMode = nMode << nShift;
//     nBuff = nBuff << nShift;

//     pTimer16-> TMR16MCR = (pTimer16-> TMR16MCR & (~nBuff)) | nMode;
// }



void TIMER16_PWMSel(OB_CT16B_Type *pTimer16, uint32_t nMatchSel, uint32_t ModeSel)
{

    if (nMatchSel == TIMER16_EMATCH0)
        pTimer16->TMR16PWMC = pTimer16->TMR16PWMC | (ModeSel << 0x00);
    else if (nMatchSel == TIMER16_EMATCH1)
        pTimer16->TMR16PWMC = pTimer16->TMR16PWMC |(ModeSel << 0x01);
    else if (nMatchSel == TIMER16_EMATCH2)
        pTimer16->TMR16PWMC = pTimer16->TMR16PWMC |(ModeSel << 0x02);
    else if (nMatchSel == TIMER16_EMATCH3)
        pTimer16->TMR16PWMC = pTimer16->TMR16PWMC |(ModeSel << 0x03);
}

void TIMER16_PWM_Length(OB_CT16B_Type *pTimer16, uint32_t nMatchSel ,uint32_t nUsec, uint32_t PlusMode)
{
    uint32_t nClock;
    nClock = (((SystemCoreClock /100000)*nUsec)/10);

    if (nMatchSel == TIMER16_EMATCH0)
    {
        pTimer16->TMR16MR0 = nClock;
        if (TIMER16_PWM_LENGTH_HL == PlusMode)
            pTimer16->TMR16MCR = (0x02) << 0;
    }
    else if (nMatchSel == TIMER16_EMATCH1)
    {
        pTimer16->TMR16MR1 = nClock;
        if (TIMER16_PWM_LENGTH_HL == PlusMode)
            pTimer16->TMR16MCR = (0x02) << 3;
    }
    else if (nMatchSel == TIMER16_EMATCH2)
    {
        pTimer16->TMR16MR2 = nClock;
        if (TIMER16_PWM_LENGTH_HL == PlusMode)
            pTimer16->TMR16MCR = (0x02) << 6;
    }
    else if (nMatchSel == TIMER16_EMATCH3)
    {
        pTimer16->TMR16MR3 = nClock;
        if (TIMER16_PWM_LENGTH_HL == PlusMode);
            pTimer16->TMR16MCR = (0x02) << 9;
    }
}

// // 新增：单个通道关闭函数
// void TIMER16_CloseChannel(OB_CT16B_Type *pTimer16, uint32_t nMRSel)
// {
//     TIMER16_EnableInterrupt(pTimer16, nMRSel, 0x00);
// }

// // 新增：单个通道使能函数
// void TIMER16_EnableChannel(OB_CT16B_Type *pTimer16, uint32_t nMRSel)
// {
//     TIMER16_EnableInterrupt(pTimer16, nMRSel, (TIMER16_MR_I | TIMER16_MR_R));
// }

