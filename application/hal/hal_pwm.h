#ifndef HAL_PWM__HH
#define HAL_PWM__HH

#include "config.h"
#include "pwm.h"


/*****************Macro****************/
#define PWM_OUT_DISABLE(PWM_CH_X)           OB_PWM->OUTEN &= (~PWM_CH_X)
#define PWM_OUT_ENABLE(PWM_CH_X)            OB_PWM->OUTEN |= (PWM_CH_X)

#define PWM_IRQ_DISABLE()                   {OB_PWM->IEENS = 0; OB_PWM->UPDATE = 0;}

/*****************Enum*****************/

/**
* @brief PWM时钟源
*/
typedef enum {
    HAL_PWM_COUNTER0,
    HAL_PWM_COUNTER1,
} HalPWMCounter;

/**
* @brief PWM比较器
*/
typedef enum {
    HAL_PWM_COMPARATOR0,
    HAL_PWM_COMPARATOR1,
    HAL_PWM_COMPARATOR2,
    HAL_PWM_COMPARATOR3,
} HalPWMComparator;

/**
* @brief PWM通道号
*/
typedef enum {
    HAL_PWM_CHANNEL0,
    HAL_PWM_CHANNEL1,
    HAL_PWM_CHANNEL2,
    HAL_PWM_CHANNEL3,
    HAL_PWM_CHANNEL4,
    HAL_PWM_CHANNEL5,
} HalPWMChannel;

/****************Struct****************/
#pragma pack(1)

typedef struct{
    uint16_t channel;
    uint8_t counter;
    uint8_t comparator;

    uint32_t period;
    uint8_t duty;
    uint8_t inverted;

    uint8_t interrupt;
    uint8_t update;

}pwm_handle_t;

typedef struct{
    uint32_t max;
    uint32_t counter;
    uint8_t duty;
    uint32_t baseValue;

}pwm_driver_t;


#pragma pack()


/***************Variable***************/


/***************Function***************/
void hal_pwmChannelInit(pwm_handle_t* pwmHandle);
void hal_pwmChannelDisable(uint16_t channel, uint8_t level);
void hal_pwmChannelEnable(uint16_t channel);

void hal_pwmSleep(void);

void hal_pwm0(uint8_t duty);
/**************************************/

#endif 
