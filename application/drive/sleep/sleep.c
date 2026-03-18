/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: sleep.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-22
 */

#include "sleep.h"
#include "uart.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "hal_wdt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "sleep"

/***************Variable***************/
static sleep_event_callback_t sleep_callback;

// ------------------------------------------
static uint8_t sleep_event_callback(uint8_t event)
{
    if (NULL == sleep_callback)
    {
        OB_LOGE(TAG, "Error[%s]", "sleep_callback");
        return false;
    }

    return sleep_callback(event);
}

//  @brief 注册回调函数
//  @param callback 
void sleep_event_register_callback(sleep_event_callback_t callback)
{
   sleep_callback = callback;

	OB_LOGD(TAG, "Regsiter callback[%s]", "sleep_callback");
}

static void mcuSleep(void)
{
    volatile uint32_t *pAddr;

    OB_SYSCON->CCLKCFG = (OB_SYSCON->CCLKCFG & 0xFFFFFC00) | 0x01;  // 切换成HIRC 24M，再2分频
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00800000) != 0x00800000);
    //--------sleep mode ---------------------
    pAddr = (uint32_t *) SBR_ADDR;
    *pAddr = *pAddr | 0x04;
    __wfi();  //sleep

    OB_SYSCON->CCLKCFG = (OB_SYSCON->CCLKCFG & 0xFFFFFC00) | (0x01 << 8);// 切换回HIRC_DPLL 48M
    while((OB_SYSCON->CPUCHIPCTR0 & 0x00800000) != 0x00800000);
}

void sleep_loop(void)
{
    if (!sleep_event_callback(ENTER_SLEEP))
    {
        return;
    }

    hal_timer_stop(HAL_TIMER0);
    hal_timer_stop(HAL_TIMER1);
    
    hal_uart_sotp(UART_0);
    hal_uart_sotp(UART_1);

    do{
        clear_feed_dog_cnt();
        mcuSleep();

    }while(!sleep_event_callback(EXIT_SLEEP));
}

void unused_gpio_init(void)
{
    // <不使用IO

    HAL_GPIO_Write(HAL_GPIO_PORT1, HAL_GPIO_PIN1, 0); // 防撬
    HAL_GPIO_Init(HAL_GPIO_PORT1, HAL_GPIO_PIN1, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT0, HAL_GPIO_PIN9, 0); // LED BLUE
    HAL_GPIO_Init(HAL_GPIO_PORT0, HAL_GPIO_PIN9, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT3, HAL_GPIO_PIN2, 0); // LED RED
    HAL_GPIO_Init(HAL_GPIO_PORT3, HAL_GPIO_PIN2, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT3, HAL_GPIO_PIN3, 0); // LED GREEN
    HAL_GPIO_Init(HAL_GPIO_PORT3, HAL_GPIO_PIN3, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN14, 0); // LCD SDA
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN14, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT1, HAL_GPIO_PIN0, 0); // LCD SCL
    HAL_GPIO_Init(HAL_GPIO_PORT1, HAL_GPIO_PIN0, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN13, 0); // LCD RESET
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN13, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN4, 1); // LCD PWR
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN4, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT0, HAL_GPIO_PIN8, 1); // FLASH CS
    HAL_GPIO_Init(HAL_GPIO_PORT0, HAL_GPIO_PIN8, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN9, 0); // WEN
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN9, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN10, 1); // LOG
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN10, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN2, 1); // XTAL
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN2, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

    HAL_GPIO_Write(HAL_GPIO_PORT2, HAL_GPIO_PIN3, 1); // XTAL
    HAL_GPIO_Init(HAL_GPIO_PORT2, HAL_GPIO_PIN3, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);

}
