/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: led.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */
#ifndef LED_HH
#define LED_HH

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
typedef void (*led_event_callback_t)(uint8_t, uint8_t, uint8_t, uint8_t);

#define LED_KEY_BOARD_TIME      (200)   //unit: ms

#define SET_LED_POWER(_level) (HAL_GPIO_Write(LED_POWER_GPIO, LED_POWER_PIN, _level))
#define SET_LED_OE(_level) (HAL_GPIO_Write(LED_OE_GPIO, LED_OE_PIN, _level))
#define SET_LED_LE(_level) (HAL_GPIO_Write(LED_LE_GPIO, LED_LE_PIN, _level))
#define SET_LED_SDI(_level) (HAL_GPIO_Write(LED_SDI_GPIO, LED_SDI_PIN, _level))
#define SET_LED_SCK(_level) (HAL_GPIO_Write(LED_SCK_GPIO, LED_SCK_PIN, _level))

#define LED_POWER_INIT(_level)                                                                     \
    do                                                                                             \
    {                                                                                              \
        HAL_GPIO_Write(LED_POWER_GPIO, LED_POWER_PIN, _level);                                     \
        HAL_GPIO_Init(LED_POWER_GPIO, LED_POWER_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define LED_OE_INIT(_level)                                                                  \
    do                                                                                       \
    {                                                                                        \
        HAL_GPIO_Write(LED_OE_GPIO, LED_OE_PIN, _level);                                     \
        HAL_GPIO_Init(LED_OE_GPIO, LED_OE_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define LED_LE_INIT(_level)                                                                  \
    do                                                                                       \
    {                                                                                        \
        HAL_GPIO_Write(LED_LE_GPIO, LED_LE_PIN, _level);                                     \
        HAL_GPIO_Init(LED_LE_GPIO, LED_LE_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define LED_SDI_INIT(_level)                                                                   \
    do                                                                                         \
    {                                                                                          \
        HAL_GPIO_Write(LED_SDI_GPIO, LED_SDI_PIN, _level);                                     \
        HAL_GPIO_Init(LED_SDI_GPIO, LED_SDI_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define LED_SCK_INIT(_level)                                                                   \
    do                                                                                         \
    {                                                                                          \
        HAL_GPIO_Write(LED_SCK_GPIO, LED_SCK_PIN, _level);                                     \
        HAL_GPIO_Init(LED_SCK_GPIO, LED_SCK_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define LED_VALUE_0                    (1 << 0)
#define LED_VALUE_1                    (1 << 1)
#define LED_VALUE_2                    (1 << 2)
#define LED_VALUE_3                    (1 << 3)
#define LED_VALUE_4                    (1 << 4)
#define LED_VALUE_5                    (1 << 5)
#define LED_VALUE_6                    (1 << 6)
#define LED_VALUE_7                    (1 << 7)
#define LED_VALUE_8                    (1 << 8)
#define LED_VALUE_9                    (1 << 9)
#define LED_VALUE_10                   (1 << 10)
#define LED_VALUE_11                   (1 << 11)
#define LED_VALUE_12                   (1 << 12)
#define LED_VALUE_13                   (1 << 13)
#define LED_VALUE_14                   (1 << 14)
#define LED_VALUE_15                   (1 << 15)

#define LED_CH_CNT                     (16)
#define LED_VALUE_TAB {LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, \
                       LED_7, LED_8, LED_9, LED_10, LED_11, LED_12, LED_13, LED_14, LED_15}

#define LED_VALUE_ALL_STANDARD         (0xFFF)      //按bit置位 bit0：LED0  bit1：LED1
#define LED_VALUE_ALL (LED_0 | LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | LED_6 | LED_7 | LED_8 | LED_9 | LED_10 | LED_11)

#define LED_GROUP_CNT                  (6)  // 总组数
#define LED_GROUP_INTERVAL             (30) // ms
#define LED_GROUP_TAB                                                                              \
    {                                                                                              \
        (LED_10),                                                                                  \
        (LED_10 | LED_7 | LED_0),                                                                  \
        (LED_10 | LED_7 | LED_0 | LED_4 | LED_8 | LED_11),                                         \
        (LED_10 | LED_7 | LED_0 | LED_4 | LED_8 | LED_11 | LED_1 | LED_5 | LED_9),                 \
        (LED_10 | LED_7 | LED_0 | LED_4 | LED_8 | LED_11 | LED_1 | LED_5 | LED_9 | LED_2 | LED_6), \
        (LED_10 | LED_7 | LED_0 | LED_4 | LED_8 | LED_11 | LED_1 | LED_5 | LED_9 | LED_2 | LED_6 | LED_3)}

#define LED_EFFECT_GROUP_CNT            (10)  // 灯效组合总数
#define UNLOCK_LED_INTERVAL             (100) // 开锁间隔
#define LOCK_LED_INTERVAL               (100) // 关锁间隔

#define LED_EFFECT_TAB                               \
    {                                                \
        (LED_VALUE_10 | LED_VALUE_7 | LED_VALUE_4),  \
        (LED_VALUE_7 | LED_VALUE_4 | LED_VALUE_1),   \
        (LED_VALUE_4 | LED_VALUE_1 | LED_VALUE_2),   \
        (LED_VALUE_1 | LED_VALUE_2 | LED_VALUE_3),   \
        (LED_VALUE_2 | LED_VALUE_3 | LED_VALUE_6),   \
        (LED_VALUE_3 | LED_VALUE_6 | LED_VALUE_9),   \
        (LED_VALUE_6 | LED_VALUE_9 | LED_VALUE_11),  \
        (LED_VALUE_9 | LED_VALUE_11 | LED_VALUE_0),  \
        (LED_VALUE_11 | LED_VALUE_0 | LED_VALUE_10), \
        (LED_VALUE_0 | LED_VALUE_10 | LED_VALUE_7),  \
    }


// ========== 失败灯效配置（宏定义） ==========
#define FAILED_LED_INTERVAL             (300)    // 失败灯效单帧间隔（ms）
#define FAILED_LED_LOOP_CNT             (3)      // 失败灯效循环次数
#define FAILED_LED_MASK                 (LED_VALUE_1 | LED_VALUE_3 | LED_VALUE_5 | LED_VALUE_7 | LED_VALUE_9) // LED1/3/5/7/9亮起

/***********Enum***********/
typedef enum {
    LED_TYPE_NONE       = 0,    // 无类型
    LED_TYPE_KEY_BOARD  = 1,    // 键盘LED
    LED_TYPE_ALL        = 0xFF  // 所有LED
} led_type_t;

typedef enum
{
    LED_EVENT_NULL = 0,          // 无事件
    LED_EVENT_ALL_ON,            // 所有LED点亮
    LED_EVENT_ALL_OFF,           // 所有LED熄灭
    LED_EVENT_SET_VALUE,         // 设置LED自定义值
    LED_EVENT_KEY_BOARD,         // 键盘事件
    LED_EVENT_WAKE_UP,           // 唤醒灯效
    LED_EVENT_UNLOCK,            // 开锁灯效
    LED_EVENT_LOCK,              // 关锁灯效
    LED_EVENT_FAILED,            // 失败灯效
    LED_EVENT_CNT,
} led_event_t;

typedef enum
{
    LED_STATE_KEY_BOARD_NONE = 0, // 无
    LED_STATE_KEY_BOARD_PRESS,    // 键盘LED灭灯状态
    LED_STATE_KEY_BOARD_LIGHTUP,  // 键盘LED亮起状态
} led_state_key_t;

// 灯效类型（用于通用循环函数判断方向）
typedef enum {
    EFFECT_TYPE_UNLOCK = 0, // 开锁：顺时针
    EFFECT_TYPE_LOCK        // 关锁：逆时针
} led_effect_t;

/***********Struct***********/
typedef struct
{
    uint16_t state;
    uint16_t value;
    uint8_t half_period_cnt;
    uint32_t half_period;
    uint32_t time_out;
} key_led_handle_t;

typedef struct
{
    uint8_t enable;
    uint8_t current_group;
    uint16_t led_value;
    uint16_t interval;
    uint32_t time_out;
} wake_light_handle_t;

typedef struct
{
    uint8_t enable;        // 使能标志（0=禁用，1=启用）
    uint8_t current_group; // 当前LED组合索引
    uint32_t time_out;     // 定时超时值
} unlock_lock_light_handle_t;

typedef struct {
    uint8_t enable;
    uint8_t loop_cnt;
    uint16_t led_value;
    uint32_t time_out;
} failed_light_handle_t;

typedef struct
{
    uint8_t busy;
    uint8_t state;
    key_led_handle_t key_board;
    wake_light_handle_t wake_light;     //唤醒灯效
    unlock_lock_light_handle_t unlock_light;
    unlock_lock_light_handle_t lock_light;
    failed_light_handle_t failed_light;
} led_hanlde_t;

/***********Variable***********/

/***********Function***********/
void ledEventRegister_callback(led_event_callback_t callback);
void led_init(void);
void led_sleep(void);
void led_set_value(uint16_t value);
uint16_t led_get_value(void);
bool led_bit_set_in_led(uint8_t led_value, uint16_t *value);
void led_key_borad_handle(u8 led_value);
void led_hanlde(uint8_t event_type, uint16_t event_code);
void led_loop(void);
/*****************************/

#endif // LED_HH
