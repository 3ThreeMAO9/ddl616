/**
 * @file system_timer.h
 * @author Xiangqian Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-13
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <stdint.h>

extern uint8_t ota_uart_tick;


void system_timer_init(void);
uint32_t system_ms_get(void);
uint8_t system_out_time_cnt(const uint32_t out_cnt);
uint32_t system_inc_time_cnt(const uint32_t inc_cnt);

#endif // UART_H
