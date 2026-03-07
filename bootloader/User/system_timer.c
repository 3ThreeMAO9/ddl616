/**
 * @file system_timer.c
 * @author Xiangqian_Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-13
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "system_timer.h"
#include "timer16.h"

uint8_t ota_uart_tick;
uint8_t system_tick;

static void timer16_irq_callback(void) {
    if (ota_uart_tick) {
        ota_uart_tick--;
    }
    system_tick++;
}

void system_timer_init(void) {
    //---------------NVIC enable--------------------
    NVIC_EnableIRQ(CT16B0_IRQn);
    //--------------------------------------------
    //Delay 1m second
    TIMER16_Delay_Interrupt(OB_CT16B0, TIMER16_MR1, (TIMER16_MR_I | TIMER16_MR_R), 1000, timer16_irq_callback);//unit 1us
    system_tick = 0;
}

