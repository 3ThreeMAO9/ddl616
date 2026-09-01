/**
 * @file hal_timer.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief
 * @version 0.1
 * @date 2023-03-01
 *
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 *
 */
#ifndef _HAL_TIMER_H
#define _HAL_TIMER_H

#include "config.h"
#include <stdint.h>
#include "string.h"

typedef void (*hal_timer_callback_t)(void);

/**
 * @brief 定时器选择
 *
 */
typedef enum {
    HAL_TIMER0,
    HAL_TIMER1,
    HAL_TIMER2,
    HAL_TIMER3,
}HalTimerSel;

typedef struct
{
    uint32_t        timer_id;
    float           period; //us
    uint32_t        repeat;
    hal_timer_callback_t event_handler;
}hal_timer_config_t;

uint32_t hal_timer_start(hal_timer_config_t const *p_config);
uint32_t hal_timer_stop(HalTimerSel timer_id);
void hal_timer_register_callback(hal_timer_callback_t cb);

uint32_t hal_timer1_start(hal_timer_config_t const *p_config);
void hal_timer1_register_callback(hal_timer_callback_t cb);

void time0_irq_handler(void);
#endif // _HAL_TIMER_H

