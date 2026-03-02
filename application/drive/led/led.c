/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: led.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#include "led.h"
#include "system_timer.h"
#include <string.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "led"

static uint16_t g_led_value = 0;
static led_event_callback_t led_callback;
static const uint16_t led_value_tab[] = LED_VALUE_TAB;
static led_hanlde_t led_handle;
static const uint16_t led_group_tab[] = LED_GROUP_TAB;

static const uint16_t led_effect_standard_tab[] = LED_EFFECT_TAB;

static void ledEvent_callback(uint8_t ledType, uint8_t ledEvent, uint8_t ledState, uint8_t ledValue)
{
    if (NULL != led_callback)
    {
        led_callback(ledType, ledEvent, ledState, ledValue);
    }
    else
    {
        OB_LOGE(TAG, "Err:led callback is null");
    }
}

void ledEventRegister_callback(led_event_callback_t callback)
{
    led_callback = callback;
    OB_LOGD(TAG,"[%s] led_register_callback: %X", __func__, callback);
}

void led_set_value(uint16_t value)
{
    g_led_value = value;
}

uint16_t led_get_value(void)
{
    return g_led_value;
}

void led_init(void)
{
    LED_OE_INIT(1);
    LED_LE_INIT(0);
    LED_SDI_INIT(0);
    LED_SCK_INIT(0);

    LED_POWER_INIT(0);

    // 初始化各灯效状态
    memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
    memset(&led_handle.wake_light, 0, sizeof(led_handle.wake_light));
    memset(&led_handle.unlock_light, 0, sizeof(led_handle.unlock_light));
    memset(&led_handle.lock_light, 0, sizeof(led_handle.lock_light));
    memset(&led_handle.failed_light, 0, sizeof(led_handle.failed_light)); // 初始化失败灯效
}

void led_sleep(void)
{
    SET_LED_LE(0);
    SET_LED_OE(1);
    SET_LED_SCK(0);
    SET_LED_SDI(0);
    SET_LED_POWER(1);

    // 休眠时停止所有灯效
    memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
    memset(&led_handle.wake_light, 0, sizeof(led_handle.wake_light));
    memset(&led_handle.unlock_light, 0, sizeof(led_handle.unlock_light));
    memset(&led_handle.lock_light, 0, sizeof(led_handle.lock_light));
    memset(&led_handle.failed_light, 0, sizeof(led_handle.failed_light));
}

void led_delay(void)
{
    uint8_t i = 3;
    while (i--)
        ;
}

void led_drive(uint16_t value)
{
    uint16_t i;
    SET_LED_OE(0);
    for (i = 0; i < 16; i++)
    {
        SET_LED_SCK(0);
        if (value & 0x8000)
        {
            SET_LED_SDI(1);
        }
        else
        {
            SET_LED_SDI(0);
        }
        value <<= 1;
        led_delay();
        SET_LED_SCK(1);
    }
    SET_LED_LE(1);
    led_delay();
    SET_LED_LE(0);
    led_delay();
    SET_LED_OE(0);
    led_delay();
    SET_LED_SCK(0);
    SET_LED_SDI(0);
}

static uint16_t convert_led_to_hw_led(uint16_t standard_led)
{
    uint16_t led_value = 0;
    if (standard_led & (1 << 0))  led_value |= LED_0;    // 标准bit0 → 硬件LED_0
    if (standard_led & (1 << 1))  led_value |= LED_1;    // 标准bit1 → 硬件LED_1
    if (standard_led & (1 << 2))  led_value |= LED_2;    // 标准bit2 → 硬件LED_2
    if (standard_led & (1 << 3))  led_value |= LED_3;    // 标准bit3 → 硬件LED_3
    if (standard_led & (1 << 4))  led_value |= LED_4;    // 标准bit4 → 硬件LED_4
    if (standard_led & (1 << 5))  led_value |= LED_5;    // 标准bit5 → 硬件LED_5
    if (standard_led & (1 << 6))  led_value |= LED_6;    // 标准bit6 → 硬件LED_6
    if (standard_led & (1 << 7))  led_value |= LED_7;    // 标准bit7 → 硬件LED_7
    if (standard_led & (1 << 8))  led_value |= LED_8;    // 标准bit8 → 硬件LED_8
    if (standard_led & (1 << 9))  led_value |= LED_9;    // 标准bit9 → 硬件LED_9
    if (standard_led & (1 << 10)) led_value |= LED_10;   // 标准bit10 → 硬件LED_10
    if (standard_led & (1 << 11)) led_value |= LED_11;   // 标准bit11 → 硬件LED_11
    if (standard_led & (1 << 12)) led_value |= LED_12;   // 标准bit12 → 硬件LED_12
    if (standard_led & (1 << 13)) led_value |= LED_13;   // 标准bit13 → 硬件LED_13
    if (standard_led & (1 << 14)) led_value |= LED_14;   // 标准bit14 → 硬件LED_14
    if (standard_led & (1 << 15)) led_value |= LED_15;   // 标准bit15 → 硬件LED_15
   return led_value;
}

// ========== 分组点灯处理函数 ==========
static void led_wake_light_loop(void)
{
    if (!led_handle.wake_light.enable)
    {
        return;
    }

    if (system_out_time_cnt(led_handle.wake_light.time_out))
    {
        led_handle.wake_light.time_out = system_inc_time_cnt(led_handle.wake_light.interval);
        g_led_value |= led_group_tab[led_handle.wake_light.current_group];
        led_drive(g_led_value);
        
        led_handle.wake_light.current_group++;
        if (led_handle.wake_light.current_group >= LED_GROUP_CNT)
        {
            led_handle.wake_light.enable = Disabled;
            led_handle.wake_light.current_group = 0;
            g_led_value = convert_led_to_hw_led(led_handle.wake_light.led_value);
            led_drive(g_led_value);
            ledEvent_callback(LED_TYPE_KEY_BOARD,LED_EVENT_WAKE_UP,NULL,NULL);
        }
    }
}

// ========== 开锁/关锁灯效 ==========
static void led_lock_unlock_loop(void)
{
    // 1. 处理开锁灯效：顺时针
    if (led_handle.unlock_light.enable)
    {
        if (system_out_time_cnt(led_handle.unlock_light.time_out))
        {
            led_handle.unlock_light.time_out = system_inc_time_cnt(UNLOCK_LED_INTERVAL);
            
            // 正向索引驱动
            uint16_t standard_led = led_effect_standard_tab[led_handle.unlock_light.current_group];
            g_led_value = convert_led_to_hw_led(standard_led);
            led_drive(g_led_value);
            
            // 索引递增重置
            led_handle.unlock_light.current_group = (led_handle.unlock_light.current_group + 1) % LED_EFFECT_GROUP_CNT;
        }
    }

    // 2. 处理关锁灯效：逆时针
    if (led_handle.lock_light.enable)
    {
        if (system_out_time_cnt(led_handle.lock_light.time_out))
        {
            led_handle.lock_light.time_out = system_inc_time_cnt(LOCK_LED_INTERVAL);
            
            // 反向索引驱动（逆时针）
            uint8_t reverse_idx = (LED_EFFECT_GROUP_CNT - 1) - led_handle.lock_light.current_group;
            uint16_t standard_led = led_effect_standard_tab[reverse_idx];
            g_led_value = convert_led_to_hw_led(standard_led);
            led_drive(g_led_value);
            
            // 索引递增重置
            led_handle.lock_light.current_group = (led_handle.lock_light.current_group + 1) % LED_EFFECT_GROUP_CNT;
        }
    }
}

// ========== 新增：失败灯效处理函数 ==========
static void led_failed_light_loop(void)
{
    if (!led_handle.failed_light.enable)
    {
        return;
    }

    if (system_out_time_cnt(led_handle.failed_light.time_out))
    {
        // 更新定时
        led_handle.failed_light.time_out = system_inc_time_cnt(FAILED_LED_INTERVAL);

        // 切换LED状态：亮→灭→亮 循环
        if (g_led_value == 0)
        {
            // 亮起指定LED（LED1/3/5/7/9）
            g_led_value = convert_led_to_hw_led(FAILED_LED_MASK);
            led_drive(g_led_value);
            
            // 计数+1（每亮一次算一次循环）
            led_handle.failed_light.loop_cnt++;
        }
        else
        {
            // 熄灭所有LED
            g_led_value = 0;
            led_drive(g_led_value);
            // 检查是否达到循环次数
            if (led_handle.failed_light.loop_cnt >= FAILED_LED_LOOP_CNT)
            {
                led_handle.failed_light.enable = Disabled; // 停止失败灯效
                led_handle.failed_light.loop_cnt = 0;                // 重置计数
                g_led_value = convert_led_to_hw_led(led_handle.failed_light.led_value);
                led_drive(g_led_value);
                ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_FAILED, NULL, 0); // 触发失败事件回调
            }
        }
    }
}

void led_hanlde(uint8_t event_type, uint16_t event_code)
{
    switch (event_type)
    {
    case LED_EVENT_ALL_ON:
        g_led_value = 0xFFFF;
        led_drive(g_led_value);
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_ALL_ON, NULL, g_led_value);
        break;
    case LED_EVENT_ALL_OFF:
        g_led_value = 0x0000;
        led_drive(g_led_value);
        // 停止所有灯效
        memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        led_handle.wake_light.enable = Disabled;
        led_handle.unlock_light.enable = Disabled;
        led_handle.lock_light.enable = Disabled;
        led_handle.failed_light.enable = Disabled;
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_ALL_OFF, NULL, g_led_value);
        break;
    case LED_EVENT_SET_VALUE:
        // 停止所有灯效
        memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        led_handle.wake_light.enable = Disabled;
        led_handle.unlock_light.enable = Disabled;
        led_handle.lock_light.enable = Disabled;
        led_handle.failed_light.enable = Disabled;

        g_led_value = convert_led_to_hw_led(event_code);
        led_drive(g_led_value);
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_SET_VALUE, NULL, g_led_value);
        break;
    case LED_EVENT_KEY_BOARD:
        led_key_borad_handle((uint8_t)event_code);
        break;
    case LED_EVENT_WAKE_UP:
        // 停止其他灯效
        memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        led_handle.unlock_light.enable = Disabled;
        led_handle.lock_light.enable = Disabled;
        led_handle.failed_light.enable = Disabled;
        
        g_led_value = 0x0000;
        led_drive(g_led_value);
        led_handle.wake_light.enable = Enabled;
        led_handle.wake_light.current_group = 0;
        led_handle.wake_light.led_value = event_code;
        led_handle.wake_light.interval = LED_GROUP_INTERVAL;
        led_handle.wake_light.time_out = system_inc_time_cnt(led_handle.wake_light.interval);
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_WAKE_UP, NULL, NULL);
        break;
    case LED_EVENT_UNLOCK:
        // 停止其他灯效
        memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        led_handle.wake_light.enable = Disabled;
        led_handle.lock_light.enable = Disabled;
        led_handle.failed_light.enable = Disabled;
        
        // 初始化开锁灯效
        led_handle.unlock_light.enable = Enabled;
        led_handle.unlock_light.current_group = 0;
        led_handle.unlock_light.time_out = system_inc_time_cnt(UNLOCK_LED_INTERVAL);
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_UNLOCK, NULL, NULL);
        break;
    case LED_EVENT_LOCK:
        // 停止其他灯效
        memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        led_handle.wake_light.enable = Disabled;
        led_handle.unlock_light.enable = Disabled;
        led_handle.failed_light.enable = Disabled;
        
        // 初始化关锁灯效
        led_handle.lock_light.enable = Enabled;
        led_handle.lock_light.current_group = 1;
        led_handle.lock_light.time_out = system_inc_time_cnt(LOCK_LED_INTERVAL);
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_LOCK, NULL, NULL);
        break;
    case LED_EVENT_FAILED:
        // 停止其他灯效
        memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        led_handle.wake_light.enable = Disabled;
        led_handle.unlock_light.enable = Disabled;
        led_handle.lock_light.enable = Disabled;
        
        // 初始化失败灯效
        led_handle.failed_light.enable = Enabled;
        led_handle.failed_light.loop_cnt = 0;
        led_handle.failed_light.led_value = event_code;
        led_handle.failed_light.time_out = system_inc_time_cnt(FAILED_LED_INTERVAL);
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_FAILED, NULL, 0);
        break;
    default:
        break;
    }
}

/**
 * @brief 检查目标LED对应的bit是否在g_led_value中置位（亮）
 * @param led_value LED编号（0~LED_CH_CNT-1）
 * @param value 输出参数：返回该LED对应的硬件bit掩码（可传NULL表示不需要）
 * @return true - 置位（亮），false - 未置位（灭）/参数非法
 */
bool led_bit_set_in_led(uint8_t led_value, uint16_t *value)
{
    if (led_value >= LED_CH_CNT) {
        OB_LOGE(TAG, "[%s] Err: led_value[%u] out of range", __func__, led_value);
        if (value != NULL) {
            *value = 0;
        }
        return false;
    }

    uint16_t led_bit = led_value_tab[led_value];
    if (value != NULL) {
        *value = led_bit;
    }

    if ((g_led_value & led_bit) == 0) {
        OB_LOGD(TAG, "[%s] LED(%d) bit(0x%04X) is off in g_led_value(0x%04X), skip", 
                __func__, led_value, led_bit, g_led_value);
        return false;
    }

    return true;
}

void led_key_borad_handle(uint8_t led_value)
{
    uint16_t value = 0;

    OB_LOGD(TAG, "[%s] led_value=%d",__func__, led_value);
    if (led_value >= LED_CH_CNT)
    {
        OB_LOGE(TAG, "Err:led_value[%u] out of range", led_value);
        return;
    }
    value = led_value_tab[led_value];

    if (!led_bit_set_in_led(led_value, &value)) {
        return;
    }

    if (led_handle.key_board.half_period_cnt)
    {
        if(led_handle.key_board.state == (g_led_value | value))
        {
            led_drive(led_handle.key_board.state);
            ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_KEY_BOARD,LED_STATE_KEY_BOARD_LIGHTUP,led_handle.key_board.value);    
            memset(&led_handle.key_board, 0, sizeof(led_handle.key_board));
        }
    }
    
    led_handle.key_board.state = (g_led_value | value);
    led_handle.key_board.value = led_value;
    led_handle.key_board.half_period_cnt = 1;
    led_handle.key_board.time_out = system_inc_time_cnt(LED_KEY_BOARD_TIME);
    if(led_handle.key_board.half_period_cnt)
    {
        led_handle.key_board.half_period = LED_KEY_BOARD_TIME;
    }

    led_drive(g_led_value & (~value));
    ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_KEY_BOARD, LED_STATE_KEY_BOARD_PRESS, led_handle.key_board.value);
}

static void led_key_board_loop(void)
{
    if(led_handle.key_board.half_period && system_out_time_cnt(led_handle.key_board.time_out))
    {
        led_handle.key_board.time_out = system_inc_time_cnt(led_handle.key_board.half_period);
        if(led_handle.key_board.half_period_cnt)
        {
            led_handle.key_board.half_period_cnt--;
            if(0 == led_handle.key_board.half_period_cnt)
            {
                led_handle.key_board.half_period = 0;
                // led_handle.key_board.enable = Disabled; // 关闭灯效
                ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_KEY_BOARD,LED_STATE_KEY_BOARD_LIGHTUP,led_handle.key_board.value);
            }
        }
        led_drive(led_handle.key_board.state);
    }
}

void led_loop(void)
{
    // 键盘灯处理
    led_key_board_loop();
    // 各灯效循环处理
    led_wake_light_loop();
    led_lock_unlock_loop();
    led_failed_light_loop();
}

