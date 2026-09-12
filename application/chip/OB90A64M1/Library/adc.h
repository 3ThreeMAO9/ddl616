#ifndef __ADC_H
#define __ADC_H

#define ADC_INT_SINGLE      1     // completion of a conversion on ADC
#define ADC_INT_SEQUENCE    2     // completion of a conversion on sequence

#define ADC_MODE_12BIT      0
#define ADC_MODE_10BIT      1

#define ADC_INPUT_1ST       0x00
#define ADC_INPUT_2ND       0x01
#define ADC_INPUT_3RD       0x02
#define ADC_INPUT_4TH       0x03
#define ADC_INPUT_5TH       0x04
#define ADC_INPUT_6TH       0x05
#define ADC_INPUT_7TH       0x06
#define ADC_INPUT_8TH       0x07
#define ADC_INPUT_8TH        0x07

#define ADC_SetInputSource(pADC, nInputSel, nChannelNO)\
    do{\
        switch (nInputSel)\
        {\
            case ADC_INPUT_1ST:\
                pADC->A0SC0 = ( pADC->A0SC0 & 0xFFFFFF00 ) | (nChannelNO << 0x00);\
                break;\
            case ADC_INPUT_2ND:\
                pADC->A0SC0 = ( pADC->A0SC0 & 0xFFFF00FF ) | (nChannelNO << 0x08);\
                break;\
            case ADC_INPUT_3RD:\
                pADC->A0SC0 = ( pADC->A0SC0 & 0xFF00FFFF ) | (nChannelNO << 0x10);\
                break;\
            case ADC_INPUT_4TH:\
                pADC->A0SC0 = ( pADC->A0SC0 & 0x00FFFFFF ) | (nChannelNO << 0x18);\
                break;\
            case ADC_INPUT_5TH:\
                pADC->A0SC1 = ( pADC->A0SC1 & 0xFFFFFF00 ) | (nChannelNO << 0x00);\
                break;\
            case ADC_INPUT_6TH:\
                pADC->A0SC1 = ( pADC->A0SC1 & 0xFFFF00FF ) | (nChannelNO << 0x08);\
                break;\
            case ADC_INPUT_7TH:\
                pADC->A0SC1 = ( pADC->A0SC1 & 0xFF00FFFF ) | (nChannelNO << 0x10);\
                break;\
            case ADC_INPUT_8TH:\
                pADC->A0SC1 = ( pADC->A0SC1 & 0x00FFFFFF ) | (nChannelNO << 0x18);\
                break;\
            default:\
                break;\
        }\
    }while(0)

void ADC_Open(OB_ADC_Type* pADC, uint32_t nADCMode);
void ADC_Close(OB_ADC_Type* pADC);

void ADC_EnableINT(OB_ADC_Type *pADC, uint32_t nINTSel);
void ADC_DisableINT(OB_ADC_Type *pADC, uint32_t nINTSel);
void ADC_SetSHTime(OB_ADC_Type *pADC, uint32_t SHTime);

void ADC_StartConvert(OB_ADC_Type *pADC, uint32_t nLength);
void ADC_StopConvert(OB_ADC_Type *pADC);
void ADC_GetSampleResult(OB_ADC_Type *pADC, uint32_t *pResult, uint32_t nLength);

#endif
