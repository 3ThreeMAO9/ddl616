#include "OB90A64M1.h"
#include "pwm.h"

void PWM_IRQHandler(void)
{

}

void PWM_Start(uint32_t nMode0, uint32_t nMode1)
{
    OB_PWM->CON = nMode0 | (nMode1 << 0x08);
    OB_PWM->CON = OB_PWM->CON | 0x3939;
}


void PWM_EnableADCTrigger(uint32_t nEvent)
{
    OB_PWM->TADC0 = nEvent;
}

void PWM_DisableADCTrigger(uint32_t nEvent)
{
    OB_PWM->TADC0 = OB_PWM->TADC0 & (~nEvent);
}

void PWM_EnableOutput(uint32_t nChannelSel)
{
    OB_PWM->OUTEN |= nChannelSel;
}

void PWM_EnableInt(uint32_t nEvent)
{
    OB_PWM->IEENS = nEvent;
}

void PWM_DisableInt (uint32_t nEvent)
{
    OB_PWM->IEENC = nEvent;
}

void PWM_ClearIntFlag(uint32_t nChannelNum)
{
    OB_PWM->EVSTC = nChannelNum;
}

uint32_t PWM_GetIntFlag()
{
    return OB_PWM->EVST;
}

void PWM_OutputDuty(PWM_OUT_CONFIG* ppwm_out)
{
    uint32_t nClock, nCounter, nMax;
    nClock = GetPeripheralClock(APB_PWM);
    nClock = (nClock /1000) ;
    nMax   = (nClock * ppwm_out->Period)/ 1000; //( 2 *1000);
    nCounter =  (nClock * ppwm_out->Duty)/1000;

    if (ppwm_out->CounterSource == PWM_COUNTER0)
    {
        if (ppwm_out->MODE0 == PWM_MODE_UP_DOWN)
        {
            nMax = nMax / 2;
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_ZERO0);
        }
        else
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_MAX0);

        OB_PWM->MAX0 = nMax;
    }
    else
    {
        if (ppwm_out->MODE1 == PWM_MODE_UP_DOWN)
        {
            nMax = nMax / 2;
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_ZERO1);
        }
        else
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_MAX1);

        OB_PWM->MAX1 = nMax;
    }

    if (nCounter > nMax) // down
    {
        nCounter = (nMax * 2) - nCounter;
        if (ppwm_out->Comparator == PWM_COMPARATOR0)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_DOWM0);
            PWM_SetComparator(PWM_COMPARATOR0, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_DOWM0);
        }
        else if (ppwm_out->Comparator == PWM_COMPARATOR1)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_DOWM1);
            PWM_SetComparator(PWM_COMPARATOR1, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_DOWM1);
        }
        else if (ppwm_out->Comparator == PWM_COMPARATOR2)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_DOWM2);
            PWM_SetComparator(PWM_COMPARATOR2, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_DOWM2);
        }
        else if (ppwm_out->Comparator == PWM_COMPARATOR3)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_DOWM3);
            PWM_SetComparator(PWM_COMPARATOR3, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_DOWM3);
        }
    }
    //------------------------------------------------------------------------------------
    else
    {
        if (ppwm_out->Comparator == PWM_COMPARATOR0)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_UP0);
            PWM_SetComparator(PWM_COMPARATOR0, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_UP0);
        }
        else if (ppwm_out->Comparator == PWM_COMPARATOR1)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_UP1);
            PWM_SetComparator(PWM_COMPARATOR1, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_UP1);
        }
        else if (ppwm_out->Comparator == PWM_COMPARATOR2)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_UP2);
            PWM_SetComparator(PWM_COMPARATOR2, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_UP2);
        }
        else if (ppwm_out->Comparator == PWM_COMPARATOR3)
        {
            PWM_SetOutputEvent(ppwm_out->Channel, PWM_EVENT_UP3);
            PWM_SetComparator(PWM_COMPARATOR3, nCounter);
            if (ppwm_out->Interrupt == 0x01)
                PWM_EnableInt(PWM_EVENT_UP3);
        }
    }
    //----------------------------------------------------------------------------------
    if (ppwm_out->DBEnable == 0x01)
    {
        if (((ppwm_out->Channel & PWM_CH0A) == PWM_CH0A) || ((ppwm_out->Channel & PWM_CH0B) == PWM_CH0B))
        {
            PWM_SetOutputInit(PWM_CH0A, 0x00);
            PWM_SetOutputInit(PWM_CH0B, 0x01);
        }
        if (((ppwm_out->Channel & PWM_CH1A) == PWM_CH1A) || ((ppwm_out->Channel & PWM_CH1B) == PWM_CH1B))
        {
            PWM_SetOutputInit(PWM_CH1A, 0x00);
            PWM_SetOutputInit(PWM_CH1B, 0x01);
        }
        if (((ppwm_out->Channel & PWM_CH2A) == PWM_CH2A) || ((ppwm_out->Channel & PWM_CH2B) == PWM_CH2B))
        {
            PWM_SetOutputInit(PWM_CH2A, 0x00);
            PWM_SetOutputInit(PWM_CH2B, 0x01);
        }

        PWM_SetDeadBand(ppwm_out->Channel, ppwm_out->DBDelay, ppwm_out->DBMode);
    }
    else
    {
        PWM_SetOutputInit(ppwm_out->Channel, 0x00);
        PWM_SetDeadBand(ppwm_out->Channel, 0x00, ppwm_out->DBMode);
    }
    //---------------------------------------------------------------------
    PWM_SetChannelSource(ppwm_out->Comparator, ppwm_out->CounterSource);
    PWM_SetOutputInvert(ppwm_out->Invert);
    PWM_EnableOutput(ppwm_out->Channel);
}

void PWM_SetComparator(uint32_t nComparatorNum, uint32_t nTimer)
{
    if (nComparatorNum == PWM_COMPARATOR0)
        OB_PWM->CMP0 = nTimer;
    else if (nComparatorNum == PWM_COMPARATOR1)
        OB_PWM->CMP1 = nTimer;
    else if (nComparatorNum == PWM_COMPARATOR2)
        OB_PWM->CMP2 = nTimer;
    else if (nComparatorNum == PWM_COMPARATOR3)
        OB_PWM->CMP3 = nTimer;
}

void PWM_SetOutputInit(uint32_t nChannelSel, uint32_t nInit)
{
    if ((nChannelSel & PWM_CH0A) == PWM_CH0A)
    {
        if (nInit == 0x00)
            OB_PWM->OUTINI = OB_PWM->OUTINI & 0xFFFFFFFE;
        else
            OB_PWM->OUTINI = OB_PWM->OUTINI | 0x01;
    }
    //--------------------------------------------------
    if ((nChannelSel & PWM_CH0B) == PWM_CH0B)
    {
        if (nInit == 0x00)
            OB_PWM->OUTINI = OB_PWM->OUTINI & 0xFFFFFFFD;
        else
            OB_PWM->OUTINI = OB_PWM->OUTINI | 0x02;
    }
    //--------------------------------------------------
    if ((nChannelSel & PWM_CH1A) == PWM_CH1A)
    {
        if (nInit == 0x00)
            OB_PWM->OUTINI = OB_PWM->OUTINI & 0xFFFFFFFB;
        else
            OB_PWM->OUTINI = OB_PWM->OUTINI | 0x04;
    }
    //--------------------------------------------------
    if ((nChannelSel & PWM_CH1B) == PWM_CH1B)
    {
        if (nInit == 0x00)
            OB_PWM->OUTINI = OB_PWM->OUTINI & 0xFFFFFFF7;
        else
            OB_PWM->OUTINI = OB_PWM->OUTINI | 0x08;
    }
    //--------------------------------------------------
    if ((nChannelSel & PWM_CH2A) == PWM_CH2A)
    {
        if (nInit == 0x00)
            OB_PWM->OUTINI = OB_PWM->OUTINI & 0xFFFFFFEF;
        else
            OB_PWM->OUTINI = OB_PWM->OUTINI | 0x10;
    }
    //--------------------------------------------------
    if ((nChannelSel & PWM_CH2B) == PWM_CH2B)
    {
        if (nInit == 0x00)
            OB_PWM->OUTINI = OB_PWM->OUTINI & 0xFFFFFFDF;
        else
            OB_PWM->OUTINI = OB_PWM->OUTINI | 0x20;
    }
    //--------------------------------------------------
}

void PWM_SetOutputEvent(uint32_t nChannelSel, uint32_t nEvent)
{
    uint32_t nBuff;
    nBuff = 0x00;
    if (nEvent & PWM_EVENT_ZERO0)
        nBuff = nBuff | (0x03 << 0);

    if (nEvent & PWM_EVENT_ZERO1)
        nBuff = nBuff | (0x03 << 2);

    if (nEvent & PWM_EVENT_MAX0)
        nBuff = nBuff | (0x03 << 4);

    if (nEvent & PWM_EVENT_MAX1)
        nBuff = nBuff | (0x03 << 6);

    if (nEvent & PWM_EVENT_DOWM0)
        nBuff = nBuff | (0x03 << 8);

    if (nEvent & PWM_EVENT_UP0)
        nBuff = nBuff | (0x03 << 10);

    if (nEvent & PWM_EVENT_DOWM1)
        nBuff = nBuff | (0x03 << 12);

    if (nEvent & PWM_EVENT_UP1)
        nBuff = nBuff | (0x03 << 14);

    if (nEvent & PWM_EVENT_DOWM2)
        nBuff = nBuff | (0x03 << 16);

    if (nEvent & PWM_EVENT_UP2)
        nBuff = nBuff | (0x03 << 18);

    if (nEvent & PWM_EVENT_DOWM3)
        nBuff = nBuff | (0x03 << 20);

    if (nEvent & PWM_EVENT_UP3)
        nBuff = nBuff | (0x03 << 22);

    nEvent = nBuff;
    //------------------------------------
    if ((nChannelSel & PWM_CH0A)== PWM_CH0A)
        OB_PWM->OUTCON0A |=  nEvent;

    if ((nChannelSel & PWM_CH0B) == PWM_CH0B)
        OB_PWM->OUTCON0B |= nEvent;

    if ((nChannelSel & PWM_CH1A) == PWM_CH1A)
        OB_PWM->OUTCON1A |= nEvent;

    if ((nChannelSel & PWM_CH1B) == PWM_CH1B)
        OB_PWM->OUTCON1B |= nEvent;

    if ((nChannelSel & PWM_CH2A) == PWM_CH2A)
        OB_PWM->OUTCON2A |= nEvent;

    if ((nChannelSel & PWM_CH2B) == PWM_CH2B)
        OB_PWM->OUTCON2B |= nEvent;
}

void PWM_SetDeadBand(uint32_t nChannelSel, uint32_t nDelay, uint32_t nMode)
{
    if (((nChannelSel & PWM_CH0A) == PWM_CH0A) || ((nChannelSel & PWM_CH0B) == PWM_CH0B))
    {
        OB_PWM->DB0 = nDelay;
        OB_PWM->DB0 = OB_PWM->DB0 | (nMode << 16);
    }

    if (((nChannelSel & PWM_CH1A) == PWM_CH1A) || ((nChannelSel & PWM_CH1B) == PWM_CH1B))
    {
        OB_PWM->DB1 = nDelay;
        OB_PWM->DB1 = OB_PWM->DB1 | (nMode << 16);
    }

    if (((nChannelSel & PWM_CH2A) == PWM_CH2A) || ((nChannelSel & PWM_CH2B) == PWM_CH2B))
    {
        OB_PWM->DB2 = nDelay;
        OB_PWM->DB2 = OB_PWM->DB2 | (nMode << 16);
    }
}

void PWM_SetOutputInvert(uint32_t nChannelSel)
{
    OB_PWM->OUTIVT = nChannelSel;
}

void PWM_SetChannelSource(uint32_t nComparatorSel, uint32_t nClockSource)
{
    if (nComparatorSel == PWM_COMPARATOR0)
    {
        if (nClockSource == PWM_COUNTER0)
            OB_PWM->CS = OB_PWM->CS & 0xFFFFFFFE;
        else
            OB_PWM->CS |= 0x01;
    }
    //-----------------------------------------------------
    if (nComparatorSel == PWM_COMPARATOR1)
    {
        if (nClockSource == PWM_COUNTER0)
            OB_PWM->CS = OB_PWM->CS & 0xFFFFFFFD;
        else
            OB_PWM->CS |= 0x02;
    }
    //-----------------------------------------------------
    if (nComparatorSel == PWM_COMPARATOR2)
    {
        if (nClockSource == PWM_COUNTER0)
            OB_PWM->CS = OB_PWM->CS & 0xFFFFFFFB;
        else
            OB_PWM->CS |= 0x04;
    }
    //-----------------------------------------------------
    if (nComparatorSel == PWM_COMPARATOR3)
    {
        if (nClockSource == PWM_COUNTER0)
            OB_PWM->CS = OB_PWM->CS & 0xFFFFFFF7;
        else
            OB_PWM->CS |= 0x08;
    }
}


