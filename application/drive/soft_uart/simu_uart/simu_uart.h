#ifndef SIMU__HH
#define SIMU__HH

#include "config.h"
#include "hal_gpio.h"

/*****************Macro****************/
#define OUT_LOW_BIT 	        (SIMU_UART_GPIO->DATA &= ~SIMU_UART_PIN)
#define OUT_HIGH_BIT 	        (SIMU_UART_GPIO->DATA |= SIMU_UART_PIN)

#define OUT_STOP_BIT 	        OUT_HIGH_BIT
#define OUT_START_BIT 	        OUT_LOW_BIT

#define SIMU_UART_INIT()                                                                             \
    do                                                                                               \
    {                                                                                                \
        OUT_HIGH_BIT;                                                                                \
        GPIO_SetPinMFType(SIMU_UART_GPIO, SIMU_UART_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
    } while (0)

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
