/**
 * @file system_timer.c
 * @author James Zhang (James Zhang@on-bright.com)
 * @brief
 * @version 0.1
 * @date 2025-12-20
 *
 * @copyright Copyright (c) 2025 广州昂宝电子有限公司
 *
 */
#include "system_timer.h"
#include "hal_wdt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "sys_timer"

/*************************Variable*************************/
static system_time_callback_t system_timer_callback;
static volatile system_time_handle_t system_time_handle;

/*************************Function*************************/
extern void uartReceiveDeal_callback(void);
// -------------------------------------------------
static uint16_t system_timer_event_callback(uint8_t source,uint8_t event_id)
{
    if(NULL != system_timer_callback){
        return system_timer_callback(source, event_id);
    }
    else{
        return false;
    }
}

void system_timer_event_register_callback(system_time_callback_t callback)
{
    system_timer_callback = callback;
}

void system_ms_timer_irq_handler(void)
{
    system_time_handle.timer.tick++;
    uartReceiveDeal_callback();
}

static void wdt_irq_callback(void)
{
    system_timer_event_callback(DOWN_COUNT_SOURCE_WDT, 0);
}
void system_timer_init(uint8_t count_type)
{
    if(system_time_handle.init & (0x01 << count_type)){
        return;
    }

    switch (count_type)
    {
    case DOWN_COUNT_SOURCE_TIMER:
        {
            hal_timer_config_t config = {
                .event_handler = system_ms_timer_irq_handler,
                .period = 1000,
            };
            hal_timer_start(&config);
            memset((uint8_t *)(&system_time_handle.timer), 0, sizeof(timer_hanlde_t));
        }
        break;
    case DOWN_COUNT_SOURCE_WDT:
        hal_wdt_init(wdt_irq_callback);
        break;
    default:
        break;
    }
    OB_LOGD(TAG, "%s [%u]", __func__, count_type);
    system_time_handle.init |= (0x01 << count_type);
}

void system_timer_sleep(uint8_t event)
{
    if (ENTER_SLEEP == event)
    {

    }
    else{
        hal_timer_config_t config = {
            .event_handler = system_ms_timer_irq_handler,
            .period = 1000,
        };
        hal_timer_start(&config);
    }
}

void set_system_timer_out(uint8_t source, uint8_t id, uint32_t inc_cnt)
{
    // OB_LOGD(TAG, "Set time out:source[%u], id[%u], time_out[%lu]", source, id, inc_cnt);
    switch (source)
    {
    case DOWN_COUNT_SOURCE_TIMER:
        system_time_handle.timer.time_out[id].down_count = system_inc_time_cnt(inc_cnt);
        system_time_handle.timer.time_out[id].flag = inc_cnt ? true : false;
        break;
    default:
        break;
    }
}

uint16_t system_timer_loop(void)
{
    clear_feed_dog_cnt(); // 喂狗

    // timer 计时处理
    for (uint8_t i = 0; i < TIME_OUT_ID_CNT; i++)
    {
        if ((system_time_handle.timer.time_out[i].flag) && (system_out_time_cnt(system_time_handle.timer.time_out[i].down_count)))
        {
            system_time_handle.timer.time_out[i].flag = false;
            system_timer_event_callback(DOWN_COUNT_SOURCE_TIMER, i);
        }
    }

    return WAKE_SOURCE_NULL;
}

uint32_t system_ms_get(void)
{
    return system_time_handle.timer.tick;
}


void system_delayms(uint16_t ms)
{
    uint32_t delay_ms = system_time_handle.timer.tick;
    while (system_time_handle.timer.tick - delay_ms < ms)
    {
        /* code */
    }
}

uint8_t system_out_time_cnt(const uint32_t out_cnt)
{
    if ((system_time_handle.timer.tick - out_cnt) < (UINT32_MAX / 2))
    {
        return true;
    }
    return false;
}

uint32_t system_inc_time_cnt(const uint32_t inc_cnt)
{
    uint32_t ms;

    if (inc_cnt >= UINT32_MAX)
    {
        return system_time_handle.timer.tick;
    }
    ms = system_time_handle.timer.tick + inc_cnt;
    return ms;
}


