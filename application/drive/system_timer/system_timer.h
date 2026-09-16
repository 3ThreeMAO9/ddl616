/**
 * @file system_timer.h
 * @author James Zhang (James Zhang@on-bright.com)
 * @brief
 * @version 0.1
 * @date 2025-12-20
 *
 * @copyright Copyright (h) 2025 广州昂宝电子有限公司
 *
 */
#ifndef _SYSTEM_TIMER_H
#define _SYSTEM_TIMER_H

#include <stdint.h>
#include "hal_timer.h"
#include "hal_rtc.h"
// #include "uart.h"

typedef uint16_t (*system_time_callback_t)(uint8_t, uint8_t);

/***********Macro***********/

/***********Enum***********/
typedef enum{
    SYSTEM_TIME_OUT_ID = 0,
    MODULE_TIME_OUT_ID,
    TIME_OUT_ID_CNT,
}timeOut_id_e;

typedef enum{
    DOWN_COUNT_SOURCE_TIMER = 0,
    DOWN_COUNT_SOURCE_WDT,
    DOWN_COUNT_SOURCE_RTC,
    DOWN_COUNT_SOURCE_CNT,
}down_count_source_e;



/***********Struct***********/
typedef struct{
    uint8_t flag;
    uint32_t down_count;
} timeOut_t;

typedef struct{
    uint32_t tick;
    timeOut_t time_out[TIME_OUT_ID_CNT];
}timer_hanlde_t;


typedef struct{
    uint8_t init;
    timer_hanlde_t timer;

}system_time_handle_t;

/***********Variable***********/

/***********Function***********/
void system_timer_event_register_callback(system_time_callback_t callback);
void system_timer_init(uint8_t count_type);
void system_ms_timer_irq_handler(void);
uint32_t system_ms_get(void);
uint16_t system_timer_loop(void);
void set_system_timer_out(uint8_t source, uint8_t id, uint32_t inc_cnt);
void system_timer_sleep(uint8_t event);
void system_delayms(uint16_t ms);
uint8_t system_out_time_cnt(const uint32_t out_cnt);
uint32_t system_inc_time_cnt(const uint32_t inc_cnt);

/*****************************/


#endif // _SYSTEM_TIMER_H

