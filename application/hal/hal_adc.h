#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>
#include <string.h>
#include "config.h"
#include "adc.h"

/**********************Macro**********************/
#define ADC_CLOSE()                       ADC_Close(OB_ADC)

#define SH_TIME_DEFAULT                   (200)

/**********************Enum**********************/
typedef enum{
    ADC_CHANNEL_0 = 0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
    ADC_CHANNEL_10,
    ADC_CHANNEL_11,
    ADC_CHANNEL_12,
    ADC_CHANNEL_13,
    ADC_CHANNEL_14,
    ADC_CHANNEL_15,
    ADC_CHANNEL_16,

}ADC_Channel_t;

/**********************Struct**********************/
#pragma pack(1)

#pragma pack()

/**********************Variable**********************/


/**********************Function**********************/
void hal_adc_start_up(ADC_Channel_t channel, uint32_t SHTime);
void hal_adc_convert_result(uint32_t *result, uint8_t len);
void hal_adc_end(void);

uint32_t hal_readChannelCurrentCalibrate(ADC_Channel_t channel, uint8_t sampleCnt);

/****************************************************/

#endif
