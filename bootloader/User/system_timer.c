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
#include "utils.h"
#include "timer16.h"
#include "ob_log.h"

uint8_t ota_uart_tick;
static uint32_t system_tick;

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

uint32_t system_ms_get(void)
{
    return system_tick;
}

uint8_t system_out_time_cnt(const uint32_t out_cnt)
{
    return ((system_tick - out_cnt) < (UINT32_MAX / 2)) ? 1 : 0;
}

uint32_t system_inc_time_cnt(const uint32_t inc_cnt)
{
    return (inc_cnt >= UINT32_MAX) ? system_tick : (system_tick + inc_cnt);
}
