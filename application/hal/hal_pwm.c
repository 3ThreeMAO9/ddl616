#include "hal_pwm.h"
#include "hal_gpio.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "hal_pwm"

/***************Variable***************/
static volatile pwm_driver_t pwmDriver;

// ------------------------------------------
void pwm_IRQ_callback(void)
{
    OB_PWM->UPDATE |= PWM_EVENT_MAX0;		//Double Buff

    if (pwmDriver.counter < (pwmDriver.max - pwmDriver.baseValue))
    {
        pwmDriver.counter += pwmDriver.baseValue;
    }
    OB_PWM->CMP0 = pwmDriver.counter;
    
    //OB_PWM->CMP0 = pwmDriver.max * (uint32_t)(pwmDriver.duty) / 100;
}

static void pwmChannelConfig(uint32_t channel)
{
    if (PWM_CH0A&channel)
    {
        GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN13, GPIO_MF_PWM, GPIO_PINMODE_PUSH_PULL);
    }

    if(PWM_CH0B&channel)
    {
        GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN12, GPIO_MF_PWM, GPIO_PINMODE_PUSH_PULL);
    }

    if (PWM_CH1A&channel)
    {
        GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN11, GPIO_MF_PWM, GPIO_PINMODE_PUSH_PULL);
    }

    if(PWM_CH1B&channel)
    {
        GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN10, GPIO_MF_PWM, GPIO_PINMODE_PUSH_PULL);
    }

    if (PWM_CH2A&channel)
    {
        GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN9, GPIO_MF_PWM, GPIO_PINMODE_PUSH_PULL);
    }

    if(PWM_CH2B&channel)
    {
        GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN8, GPIO_MF_PWM, GPIO_PINMODE_PUSH_PULL);
    }
}

void hal_pwmChannelInit(pwm_handle_t* pwmHandle)
{
    uint32_t nCounterValue, nMax;

    pwmChannelConfig(pwmHandle->channel);
    nMax = (uint32_t)((24 * 1000 * 1000) / pwmHandle->period);
    nCounterValue =  nMax * pwmHandle->duty / 0xFF;

    switch(pwmHandle->comparator)
    {
        case PWM_COMPARATOR0:
            OB_PWM->CS_b.CMP0 = pwmHandle->counter;		// CMP0 select CU0/CU1
            OB_PWM->CMP0 = nCounterValue;
            break;
        case PWM_COMPARATOR1:
            OB_PWM->CS_b.CMP1 = pwmHandle->counter;		// CMP0 select CU0/CU1
            OB_PWM->CMP1 = nCounterValue;
            break;
        case PWM_COMPARATOR2:
            OB_PWM->CS_b.CMP2 = pwmHandle->counter;		// CMP0 select CU0/CU1
            OB_PWM->CMP2 = nCounterValue;
            break;
        case PWM_COMPARATOR3:
            OB_PWM->CS_b.CMP3 = pwmHandle->counter;		// CMP0 select CU0/CU1
            OB_PWM->CMP3 = nCounterValue;
            break;
    }

    if (PWM_COUNTER0 == pwmHandle->counter)
    {
        OB_PWM->MAX0 = nMax;
        OB_PWM->CYCMP0 = pwmHandle->update;
        PWM_SetOutputEvent(pwmHandle->channel, PWM_EVENT_MAX0 | PWM_EVENT_UP0);
    }
    else if (PWM_COUNTER1 == pwmHandle->counter)
    {
        OB_PWM->MAX1 = nMax;
        OB_PWM->CYCMP1 = pwmHandle->update;
        PWM_SetOutputEvent(pwmHandle->channel, PWM_EVENT_MAX1 | PWM_EVENT_UP1);
    }

    if (pwmHandle->inverted)
    {
        OB_PWM->OUTIVT |= pwmHandle->channel;				// output is't reverse
    }
    else
    {
        OB_PWM->OUTIVT &= (~pwmHandle->channel);
    }
    OB_PWM->OUTINI = 0x00;

    // if (pwmHandle->interrupt)
    // {
    //     OB_PWM->UPDATE_b.IMMED = 1;
    //     if (PWM_COUNTER0 == pwmHandle->counter)
    //     {
    //         OB_PWM->IEENS |= PWM_EVENT_MAX0;				//中断使能：CU0/CU1匹配MAX0

    //         pwmDriver.max = nMax;
    //         pwmDriver.duty = pwmHandle->duty;
    //         pwmDriver.baseValue = pwmDriver.max / 100;
    //         pwmDriver.counter = nCounterValue;
    //     }
    // }

    NVIC_SetPriority(PWM_IRQn, 0);
    NVIC_EnableIRQ(PWM_IRQn);
    PWM_Start(PWM_MODE_FREE, PWM_MODE_FREE);
}


void hal_pwmChannelDisable(uint16_t channel, uint8_t level)
{
    PWM_OUT_DISABLE(channel);
    if (level)
    {
        OB_PWM->OUTFIX |= channel;
    }
    else
    {
        OB_PWM->OUTFIX &= (~channel);
    }
}

void hal_pwmChannelEnable(uint16_t channel)
{
    PWM_OUT_ENABLE(channel);
}

void hal_pwmSleep(void)
{
	PWM_IRQ_DISABLE();
	NVIC_DisableIRQ(PWM_IRQn);
}

void hal_pwm0(uint8_t duty)
{
    pwm_handle_t pwmHandle;
    pwmHandle.channel = PLAYER_PWM_CHANNEL;
    pwmHandle.counter = PWM_COUNTER1;
    pwmHandle.comparator = PWM_COMPARATOR1;
    pwmHandle.period = PLAYER_PWM_PERIOD;
    pwmHandle.duty = duty;
    pwmHandle.inverted = true;
    pwmHandle.interrupt = false;
    pwmHandle.update = 1;
    
    hal_pwmChannelInit(&pwmHandle);
    hal_pwmChannelEnable(PLAYER_PWM_CHANNEL);
}

