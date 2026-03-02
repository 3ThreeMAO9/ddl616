#ifndef SIMU__HH
#define SIMU__HH

#include "config.h"
#include "hal_gpio.h"

/*****************Macro****************/
#define SET_SIMU_UART(_level) (HAL_GPIO_Write(SIMU_UART_GPIO, SIMU_UART_PIN, _level))

#define SIMU_UART_INIT(_level)                                                                     \
    do                                                                                             \
    {                                                                                              \
        HAL_GPIO_Write(SIMU_UART_GPIO, SIMU_UART_PIN, _level);                                     \
        HAL_GPIO_Init(SIMU_UART_GPIO, SIMU_UART_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define OUT_LOW_BIT 	        SET_SIMU_UART(0)
#define OUT_HIGH_BIT 	        SET_SIMU_UART(1)

#define OUT_STOP_BIT 	        OUT_HIGH_BIT
#define OUT_START_BIT 	        OUT_LOW_BIT

/*****************Enum*****************/


/****************Struct****************/
#pragma pack(1)

#pragma pack()


/***************Variable***************/


/***************Function***************/
uint8_t simu_putchar (uint8_t s_data);
void simu_uart_init(void);

/**************************************/

#endif 
