#ifndef __ADC_H
#define __ADC_H

#define ADC_INT_SINGLE      1     // completion of a conversion on ADC
#define ADC_INT_SEQUENCE    2     // completion of a conversion on sequence

#define ADC_MODE_12BIT      0
#define ADC_MODE_10BIT      1

#define ADC_CHANNEL_0       0x00
#define ADC_CHANNEL_1       0x01
#define ADC_CHANNEL_2       0x02
#define ADC_CHANNEL_3       0x03
#define ADC_CHANNEL_4       0x04
#define ADC_CHANNEL_5       0x05
#define ADC_CHANNEL_6       0x06
#define ADC_CHANNEL_7       0x07
#define ADC_CHANNEL_8       0x08
#define ADC_CHANNEL_9       0x09
#define ADC_CHANNEL_10      0x0A
#define ADC_CHANNEL_11      0x0B
#define ADC_CHANNEL_12      0x0C
#define ADC_CHANNEL_13      0x0D
#define ADC_CHANNEL_14      0x0E
#define ADC_CHANNEL_15      0x0F
#define ADC_CHANNEL_16      0x10

#define ADC_INPUT_1ST       0x00
#define ADC_INPUT_2ND       0x01
#define ADC_INPUT_3RD       0x02
#define ADC_INPUT_4TH       0x03
#define ADC_INPUT_5TH       0x04
#define ADC_INPUT_6TH       0x05
#define ADC_INPUT_7TH       0x06
#define ADC_INPUT_8TH       0x07

void ADC_Open(OB_ADC_Type* pADC, uint32_t nADCMode);
void ADC_Close(OB_ADC_Type* pADC);

void ADC_EnableINT(OB_ADC_Type *pADC, uint32_t nINTSel);
void ADC_DisableINT(OB_ADC_Type *pADC, uint32_t nINTSel);

void ADC_SetInputSource(OB_ADC_Type *pADC,uint32_t nInputSel, uint32_t nChannelNO);

void ADC_StartConvert(OB_ADC_Type *pADC, uint32_t nLength);
void ADC_StopConvert(OB_ADC_Type *pADC);
void ADC_GetSampleResult(OB_ADC_Type *pADC, uint32_t *pResult, uint32_t nLength);

#endif
