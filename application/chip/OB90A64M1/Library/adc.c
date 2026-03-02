#include "OB90A64M1.h"
#include "adc.h"

void ADC_IRQHandler(void)
{

}

void ADC_Open(OB_ADC_Type* pADC, uint32_t nADCMode)
{
    NVIC_EnableIRQ(ADC_IRQn);

    pADC->CON_b.EN0   = 1;
    pADC->CON_b.MODE0 = nADCMode;
    pADC->RSIN0 = 0x80000000;
    pADC->CD0   = 0;
    while(1)
    {
        if ((pADC->CALCON0& 0x101) == 0x01)
            break;
    }
}

void ADC_Close(OB_ADC_Type* pADC)
{
    pADC->CON_b.EN0 = 0;
    pADC->CON = 0x00;
    pADC->CALCON0 = 0x00;

    NVIC_DisableIRQ(ADC_IRQn);
}

void ADC_EnableINT(OB_ADC_Type *pADC, uint32_t nINTSel)
{

    if (nINTSel == ADC_INT_SINGLE)
        pADC->IEENS_b.SIN0 = 1;
    else
        pADC->IEENS_b.SEQ0 = 1;


}

void ADC_DisableINT(OB_ADC_Type *pADC, uint32_t nINTSel)
{
    if (nINTSel == ADC_INT_SINGLE)
        pADC->IEENC_b.SIN0 = 1;
    else
        pADC->IEENC_b.SEQ0 = 1;

}

void ADC_SetSHTime(OB_ADC_Type *pADC, uint32_t SHTime)
{
    // SHTime Unit = ADC Clock Count.

     pADC->SH0 = SHTime;
}

void ADC_SetInputSource(OB_ADC_Type *pADC, uint32_t nInputSel, uint32_t nChannelNO)
{
    switch(nInputSel)
    {
        case ADC_INPUT_1ST:
            pADC->A0SC0 = ( pADC->A0SC0 & 0xFFFFFF00 ) |  (nChannelNO << 0x00);
            break;

        case ADC_INPUT_2ND:
            pADC->A0SC0 = ( pADC->A0SC0 & 0xFFFF00FF ) |  (nChannelNO << 0x08);
            break;

        case ADC_INPUT_3RD:
            pADC->A0SC0 = ( pADC->A0SC0 & 0xFF00FFFF ) |  (nChannelNO << 0x10);
            break;

        case ADC_INPUT_4TH:
            pADC->A0SC0 = ( pADC->A0SC0 & 0x00FFFFFF ) |  (nChannelNO << 0x18);
            break;

        case ADC_INPUT_5TH:
            pADC->A0SC1 = ( pADC->A0SC1 & 0xFFFFFF00 ) |  (nChannelNO << 0x00);
            break;

        case ADC_INPUT_6TH:
            pADC->A0SC1 = ( pADC->A0SC1 & 0xFFFF00FF ) |  (nChannelNO << 0x08);
            break;

        case ADC_INPUT_7TH:
            pADC->A0SC1 = ( pADC->A0SC1 & 0xFF00FFFF ) |  (nChannelNO << 0x10);
            break;

        case ADC_INPUT_8TH:
            pADC->A0SC1 = ( pADC->A0SC1 & 0x00FFFFFF ) |  (nChannelNO << 0x18);
            break;

        default:
            break;
    }
}

void ADC_StartConvert(OB_ADC_Type *pADC, uint32_t nLength)
{
    uint32_t nINTType = ADC_INT_SEQUENCE;

     pADC->SL0 = nLength;
     pADC->CON_b.START0 = 1;
     while(1)
     {
        if ((pADC->IEST & nINTType) == nINTType)
        {
            pADC->IESTC = (pADC->IEST & 0x00FF); //claer status
            break;
        }
     }

}

void ADC_StopConvert(OB_ADC_Type *pADC)
{
    pADC->CON_b.START0 = 0;
}

void ADC_GetSampleResult(OB_ADC_Type *pADC, uint32_t *pResult, uint32_t nLength)
{
    uint32_t* pRS;
    if (nLength > 8)
        nLength = 8;

    pRS = (uint32_t*)(&pADC->A0RS0);
    while(1)
    {
        *pResult = *pRS;
         pResult++;
         pRS++;
         nLength--;
         if (nLength == 0x00)
            return;
    }
}


