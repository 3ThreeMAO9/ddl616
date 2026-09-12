#include "hal_adc.h"
#include "delay.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_adc"

/***********Variable***********/

void hal_adc_start_up(ADC_Channel_t channel, uint32_t SHTime)
{
    ADC_Open(OB_ADC, ADC_MODE_12BIT);
    ADC_SetInputSource(OB_ADC, ADC_INPUT_1ST, channel);
    ADC_SetSHTime(OB_ADC, SHTime);
}

void hal_adc_convert_result(uint32_t *result, uint8_t len)
{
    ADC_StartConvert(OB_ADC, len);
    ADC_GetSampleResult(OB_ADC, result, len);
}

void hal_adc_end(void)
{
    ADC_CLOSE();
}

uint32_t hal_readChannelCurrentCalibrate(ADC_Channel_t channel, uint8_t sampleCnt)
{
    uint8_t i;
    uint32_t result;
    uint32_t sample_value[2];

    ADC_Open(OB_ADC, ADC_MODE_12BIT);

    // 1st Input Source = channel
    // 2nd Input Source = ADC_CHANNEL_16
    ADC_SetInputSource(OB_ADC, ADC_INPUT_1ST, channel);
    // ADC_SetInputSource(OB_ADC, ADC_INPUT_2ND, ADC_CHANNEL_16);  // Standards: 0.6V

    ADC_SetSHTime(OB_ADC, SH_TIME_DEFAULT);

    result = 0;
    for (i = 0; i < sampleCnt; i++)
    {
        delay_ms(1);

        ADC_StartConvert(OB_ADC, 1);
        ADC_GetSampleResult(OB_ADC, sample_value, 1);
        result += sample_value[0];
    }

    result /= sampleCnt;
    result = ((600 * result) / 740);    // 0.6V 理论上是745，采样电容推荐用1uf，但实际电容采用的是1nf，值会偏小

    ADC_CLOSE();

    return result;
}
