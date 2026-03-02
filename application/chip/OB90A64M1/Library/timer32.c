#include "OB90A64M1.h"
#include "timer32.h"

volatile uint32_t g_T32B0Interrupt;
volatile uint32_t g_T32B1Interrupt;

void CT32B0_IRQHandler()
{
    g_T32B0Interrupt++;
    OB_CT32B0->TMR32IR = 0xFF; //clear interrupt
}

void CT32B1_IRQHandler()
{
    g_T32B1Interrupt++;
    OB_CT32B1->TMR32IR = 0xFF; //clear interrupt
}

void TIMER32_Enable(OB_CT32B_Type *pTimer32)
{
    pTimer32->TMR32TCR = pTimer32->TMR32TCR | TIMER32_RESET;
    pTimer32->TMR32TCR = pTimer32->TMR32TCR | TIMER32_ENABLE; //Enable counter
}

void TIMER32_Close(OB_CT32B_Type *pTimer32)
{
    pTimer32->TMR32TCR = TIMER32_RESET;
}

void TIMER32_Delay(OB_CT32B_Type *pTimer32, uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer32 == OB_CT32B0)
        nPerClock = GetPeripheralClock(APB_T32B0);
    else
        nPerClock = GetPeripheralClock(APB_T32B1);

    nClock = (((nPerClock/100000)*nUsec)/10);
    pTimer32->TMR32PC  = 0x00;
    pTimer32->TMR32MCR = TIMER32_MATCHS_INT | TIMER32_MATCHS_RESET;
    pTimer32->TMR32TCR = pTimer32->TMR32TCR | TIMER32_RESET;
    pTimer32->TMR32TCR = pTimer32->TMR32TCR | TIMER32_ENABLE; //Enable counter
    while(1)
    {
        if (nClock >= 0x80000000)
        {
            pTimer32->TMR32MR0 = 0x80000000;
            nClock = nClock - 0x80000000;
        }
        else
        {
            pTimer32->TMR32MR0 = nClock;
            nClock = 0x00;
        }
        while(!(pTimer32->TMR32IR & 0x01));
        pTimer32->TMR32IR = pTimer32->TMR32IR | 0x01;
        if (nClock == 0x00)
            break;
    }

    TIMER32_Close(pTimer32);
}

void TIMER32_EnableCapture(OB_CT32B_Type *pTimer32, uint32_t nCapSource, uint32_t nEdge)
{
    if (nCapSource == TIMER32_CAPTURE0)
        pTimer32->TMR32CCR = (pTimer32->TMR32CCR & 0xFFFFFFFC) | nEdge;
    else
        pTimer32->TMR32CCR = (pTimer32->TMR32CCR & 0xFFFFFFE7) | (nEdge << 0x03);
}

void TIMER32_DisableCapture(OB_CT32B_Type *pTimer32)
{
    pTimer32->TMR32CCR = 0x00;
}

uint32_t TIMER32_GetModuleClock(OB_CT32B_Type *pTimer32)
{
    uint32_t nPerClock, nCounter;

    if (pTimer32 == OB_CT32B0)
        nPerClock = GetPeripheralClock(APB_T32B0);
    else
        nPerClock = GetPeripheralClock(APB_T32B1);

    if ((pTimer32->TMR32MCR & TIMER32_MATCHS_RESET) == 0x00)
        nCounter = 0xFFFF * (pTimer32->TMR32PC + 1);
    else
        nCounter = pTimer32->TMR32MR0 * (pTimer32->TMR32PC + 1);

    return (nPerClock/nCounter);
}

void TIMER32_ToggleOut(OB_CT32B_Type *pTimer32, uint32_t nMatchSel, uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer32 == OB_CT32B0)
        nPerClock = GetPeripheralClock(APB_T32B0);
    else
        nPerClock = GetPeripheralClock(APB_T32B1);

    nClock = (((nPerClock /100000)*nUsec)/10);

    pTimer32->TMR32PR = nClock/0x10000;
    nClock = nClock/(pTimer32->TMR32PR + 1);
    if (nMatchSel == TIMER32_EMATCH0)
    {
        pTimer32->TMR32MR0 = nClock;
        pTimer32->TMR32MCR = (0x02) << 0;
    }
    else if (nMatchSel == TIMER32_EMATCH1)
    {
        pTimer32->TMR32MR1 = nClock;
        pTimer32->TMR32MCR = (0x02) << 3;
    }
    else if (nMatchSel == TIMER32_EMATCH2)
    {
        pTimer32->TMR32MR2 = nClock;
        pTimer32->TMR32MCR = (0x02) << 6;
    }
    else if (nMatchSel == TIMER32_EMATCH3)
    {
        pTimer32->TMR32MR3 = nClock;
        pTimer32->TMR32MCR = (0x02) << 9;
    }
}

void TIMER32_SetExternalMatch(OB_CT32B_Type *pTimer32, uint32_t nMatchSel, uint32_t nMode)
{
    uint32_t nBuff;

    nBuff = pTimer32->TMR32EMR;
    if ((nMatchSel & TIMER32_EMATCH0) == TIMER32_EMATCH0)
    {
        nBuff = nBuff & (~(0x03 << 0x04));
        nBuff = nBuff | (nMode << 0x04) | 0x01;
    }

    if ((nMatchSel & TIMER32_EMATCH1) == TIMER32_EMATCH1)
    {
        nBuff = nBuff & (~(0x03 << 0x06));
        nBuff = nBuff | (nMode << 0x06) | 0x02;
    }

    if ((nMatchSel & TIMER32_EMATCH2) == TIMER32_EMATCH2)
    {
        nBuff = nBuff & (~(0x03 << 0x08));
        nBuff = nBuff | (nMode << 0x08) | 0x04;
    }

    if ((nMatchSel & TIMER32_EMATCH3) == TIMER32_EMATCH3)
    {
        nBuff = nBuff & (~(0x03 << 0x0A));
        nBuff = nBuff | (nMode << 0x0A) | 0x08;
    }

    pTimer32->TMR32EMR = nBuff;
}

void TIMER32_Delay_Interrupt(OB_CT32B_Type *pTimer32, uint32_t nMRSel,uint32_t nMode,  uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer32 == OB_CT32B0)
        nPerClock = GetPeripheralClock(APB_T32B0);
    else
        nPerClock = GetPeripheralClock(APB_T32B1);

    nClock = (((nPerClock/100000)*nUsec)/10);
    pTimer32->TMR32PC = 0x00;

    if (nMRSel == TIMER32_MR0)
        pTimer32->TMR32MR0 = nClock;
    else if (nMRSel == TIMER32_MR1)
        pTimer32->TMR32MR1 = nClock;
    else if (nMRSel == TIMER32_MR2)
        pTimer32->TMR32MR2 = nClock;
    else if (nMRSel == TIMER32_MR3)
        pTimer32->TMR32MR3 = nClock;

    TIMER32_EnableInterrupt(pTimer32, nMRSel, nMode);
    pTimer32->TMR32TCR = pTimer32->TMR32TCR | TIMER32_RESET;
    pTimer32->TMR32TCR = pTimer32->TMR32TCR | TIMER32_ENABLE; //Enable counter
}

void TIMER32_EnableInterrupt(OB_CT32B_Type *pTimer32, uint32_t nMRSel, uint32_t nMode)
{
    uint32_t nBuff, nShift;
    nBuff  = 7;
    nShift = 0x00;
    if (nMRSel == TIMER32_MR0)
        nShift = 0x00;
    else if (nMRSel == TIMER32_MR1)
        nShift = 0x03;
    else if (nMRSel == TIMER32_MR2)
        nShift = 0x06;
    else if (nMRSel == TIMER32_MR3)
        nShift = 0x09;

    nMode = nMode <<  nShift;
    nBuff = nBuff <<  nShift;

    pTimer32->TMR32MCR = (pTimer32-> TMR32MCR & (~nBuff)) | nMode;
}

void TIMER32_PWMSel(OB_CT32B_Type *pTimer32, uint32_t nMatchSel, uint32_t ModeSel)
{

    if (nMatchSel == TIMER32_EMATCH0)
        pTimer32->TMR32PWMC = pTimer32->TMR32PWMC | (ModeSel << 0x00);
    else if (nMatchSel == TIMER32_EMATCH1)
        pTimer32->TMR32PWMC = pTimer32->TMR32PWMC |(ModeSel << 0x01);
    else if (nMatchSel == TIMER32_EMATCH2)
        pTimer32->TMR32PWMC = pTimer32->TMR32PWMC |(ModeSel << 0x02);
    else if (nMatchSel == TIMER32_EMATCH3)
        pTimer32->TMR32PWMC = pTimer32->TMR32PWMC |(ModeSel << 0x03);
}

void TIMER32_PWM_Length(OB_CT32B_Type *pTimer32, uint32_t nMatchSel ,uint32_t nUsec, uint32_t PlusMode)
{
    uint32_t nClock;
    nClock = (((SystemCoreClock /100000)*nUsec)/10);

    if (nMatchSel == TIMER32_EMATCH0)
    {
        pTimer32->TMR32MR0 = nClock;
        if (TIMER32_PWM_LENGTH_HL == PlusMode)
            pTimer32->TMR32MCR = (0x02) << 0;
    }
    else if (nMatchSel == TIMER32_EMATCH1)
    {
        pTimer32->TMR32MR1 = nClock;
        if (TIMER32_PWM_LENGTH_HL == PlusMode)
            pTimer32->TMR32MCR = (0x02) << 3;
    }
    else if (nMatchSel == TIMER32_EMATCH2)
    {
        pTimer32->TMR32MR2 = nClock;
        if (TIMER32_PWM_LENGTH_HL == PlusMode)
            pTimer32->TMR32MCR = (0x02) << 6;
    }
    else if (nMatchSel == TIMER32_EMATCH3)
    {
        pTimer32->TMR32MR3 = nClock;
        if (TIMER32_PWM_LENGTH_HL == PlusMode);
            pTimer32->TMR32MCR = (0x02) << 9;
    }
}
