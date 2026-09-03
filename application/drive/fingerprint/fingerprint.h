
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: fingerprint.h
 * Desc: 指纹/ 指静脉模块驱动接口
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-09
 * Note： 驱动层控制句柄fp_context_t继承自模块层，驱动层不涉及具体的硬件配置实现，通过fingerprint_GPIO_Ops_t抽象为函数指针；
 */
#ifndef __FINGERPRINT_H
#define __FINGERPRINT_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "system_timer.h"

typedef uint8_t (*fingerprint_callback_t)(uint8_t result, void* para, uint8_t lenth);

/*****************Macro****************/
// 根据实际需求启用/禁用功能以节省内存
#define FP_ENABLE_DELETE            1  // 启用删除功能
#define FP_ENABLE_VERIFY_DELETE     1  // 启用验证删除功能
#define FP_ENABLE_LED_CONTROL       0  // 禁用LED控制（如不需要）

// 缓冲区大小配置（根据实际需求调整）
#define FP_RX_BUFFER_SIZE        64  // 接收缓冲区
#define FP_TX_BUFFER_SIZE        32  // 发送缓冲区

#define FP_MAX_RETRIES           2   // 最大重试次数

#define FINGERPRINT_COUNT_MAX           (100)
#define FINGERPRINT_COUNT_DEFAULT       (FINGERPRINT_COUNT_MAX)

#define FINGER_VEIN_FLAG                (0x9B)

#define FINGERPRINT_REGISTER_COUNT_DEFAULT  (6)

#define FINGERPRINT_REPEAT_CNT_MAX          (5)

// ==================== 内部宏定义 ====================
#define FP_PACKET_HEADER         0xEF01
#define FP_DEVICE_ADDRESS        0xFFFFFFFF
#define FP_DEFAULT_TIMEOUT       2000    // 2秒超时
#define FP_RX_TIMEOUT            600      // 600ms接收超时
#define FP_RX_SCAN_TIMEOUT       100      // 100ms接收超时

/*****************Enum*****************/
// 指令集
typedef enum {
    FP_CMD_NONE = 0,
    FP_CMD_GET_IMAGE = 0x01,
    FP_CMD_GEN_CHAR = 0x02,
    FP_CMD_MATCH = 0x03,
    FP_CMD_SEARCH = 0x04,
    FP_CMD_REG_MODEL = 0x05,
    FP_CMD_STORE_CHAR = 0x06,
    FP_CMD_LOAD_CHAR = 0x07,
    FP_CMD_UP_CHAR = 0x08,
    FP_CMD_DOWN_CHAR = 0x09,
    FP_CMD_DELETE_CHAR = 0x0C,
    FP_CMD_EMPTY = 0x0D,
    FP_CMD_WRITE_REG = 0x0E,
    FP_CMD_READ_SYS_PARA = 0x0F,
    FP_CMD_READ_INDEX_TABLE = 0x1F,
    FP_CMD_CANCEL = 0x30,
    FP_CMD_SLEEP = 0x33,
    FP_CMD_GET_CHIP_SN = 0x34,
    FP_CMD_CONTROL_LED = 0x40,

}fp_cmd_e;

// 确认码
typedef enum {
    FP_ACK_OK = 0x00,
    FP_ACK_FAIL = 0x01,
    FP_ACK_NO_FINGER = 0x02,
    FP_ACK_IMAGE_FAIL = 0x03,
    FP_ACK_FEATURE_FAIL = 0x04,
    FP_ACK_NO_MATCH = 0x08,
    FP_ACK_NOT_FOUND = 0x09,
    FP_ACK_INVALID_PAGE = 0x0B,
    FP_ACK_FLASH_ERROR = 0x18,
    FP_ACK_TIMEOUT = 0x26,
    FP_ACK_DB_FULL = 0x1F,

}fp_ack_e;

// 包标识
typedef enum {
    PKG_COMMAND = 0x01,
    PKG_DATA    = 0x02,
    PKG_ACK     = 0x07,
    PKG_END     = 0x08,

}fp_pkt_flag_e;

// 操作模式
typedef enum {
    FP_MODE_INIT = 0,           // 初始化（设置无效，模块内部行为）
    FP_MODE_SLEEP,              // 休眠
    FP_MODE_IDLE,               // 空闲
    FP_MODE_VERIFY,             // 验证
    FP_MODE_REGISTER,           // 注册
    FP_MODE_DELETE,             // 按编号删除
    FP_MODE_VERIFY_DELETE,      // 验证删除
    FP_MODE_RESET_ALL,         // 恢复出厂
}fp_mode_e;

typedef enum {
    FINGER_TYPE_FINGERPRINT = 0,
    FINGER_TYPE_FINGER_VEIN,

}finger_module_type_e;

typedef enum{
    FP_EVENT_SUCCESS_HANDLE = 0, // 操作成功
    FP_EVENT_INVALID_FP = 1,     // 非法指纹(未录入)
    FP_EVENT_INVALID_MODULE = 2, // 非法模块(模块与录入的不一致)
    FP_EVENT_INVALID_REPEAT = 3, // 重复指纹
    FP_EVENT_SUCCESS_INIT = 4,   // 初始化成功
    FP_EVENT_FAIL_COMBINE = 5,   // 合成模板失败
    FP_EVENT_FAIL_STORE = 6,     // 保存模板失败
    FP_EVENT_FAIL_READ_SN = 7,   // 读取唯一码失败
    FP_EVENT_FAIL_FULL = 8,      // 列表已满
    FP_EVENT_FAIL_DELETE = 9,    // 删除失败
    FP_EVENT_PROCESSING = 10,    // 过程
    FP_EVENT_CHIP_SN = 11,       // 读取Chip SN
    FP_EVENT_DELETE_ALL = 12,    // 删除所有指纹
    FP_EVENT_POWER_ON = 13,      // 指纹上电

} fingerprint_event_e;

typedef enum {
    FP_LED_RED = 0,
    FP_LED_GREEN,
    FP_LED_BLUE,
    FP_LED_WHITE,
    FP_LED_CLOSE,

}fingerprint_led_e;

typedef enum {
    FP_STEP_0 = 0,
    FP_STEP_1,
    FP_STEP_2,
    FP_STEP_3,
    FP_STEP_4,
    FP_STEP_5,
    FP_STEP_6,
    FP_STEP_7,
    FP_STEP_8,
    FP_STEP_9,
    FP_STEP_10,

    FP_STEP_IDLE = 0x0D,
    FP_STEP_SLEEP = 0x0E,
    FP_STEP_POWER_DOWN = 0x0F,

    FP_STEP_CNT_MAX = 0x10,

}fingerprint_step_e;        //bit0~3

/****************Struct****************/
// GPIO操作函数结构体
typedef struct {
    void (*init)(void);                     // GPIO初始化
    void (*power)(uint8_t);                 // 供电使能： true，供电； false，断电
    void (*uart_init)(uint8_t);             // 串口初始化
    uint8_t (*send)(uint8_t*, uint8_t);     // 串口发送
    uint8_t (*receive)(uint8_t*);           // 串口接收
    uint8_t (*is_wake)(void);               // 唤醒检测

} fingerprint_GPIO_Ops_t;

// I2C配置结构体
typedef struct {
    fingerprint_GPIO_Ops_t ops;     // GPIO操作函数
    uint8_t params_size;

} fingerprint_Config_t;

// 模块状态（使用位域节省内存）
typedef struct {
    uint8_t init        : 1;    // 初始化
    uint8_t power       : 1;    // 供电标志

    uint8_t handshake   : 1;    // 上电握手
    uint8_t waiting     : 1;    // 等待响应
    uint8_t processing  : 1;    // 处理中
    uint8_t timeout     : 1;    // 超时
    uint8_t wait_lift   : 1;    // 等待抬手
    uint8_t led_state   : 1;    // led 状态

    uint8_t repeat_cnt;         // 重试次数

    // uint8_t reserved    : 1; // 保留
} fp_status_t;

typedef struct {
    uint8_t type;                                           // 模块类型
    uint8_t count_max;                                      // 用户容量
    uint8_t index_table[(FINGERPRINT_COUNT_MAX + 7) / 8];   // 指纹模板列表

}fp_module_attr_t;

typedef struct {
    uint8_t register_count : 3;                             // 注册次数
    uint8_t repeat : 1;                                     // 查重
    uint8_t auto_sleep : 1;                                 // 空闲状态进休眠
    uint8_t led_default : 2;                                // 空闲状态默认灯光
    uint8_t idle_irq :1;                                    // 空闲状态中断唤醒(auto_sleep失效)   
    uint8_t chip_sn_flag;                                   // chip_sn_flag
    uint8_t chip_sn[14];                                    // chip sn

}fp_function_attr_t;

typedef struct {
    uint8_t cmd;                            // 命令
    uint8_t code;                           // 确认码
    uint8_t buffer[FP_RX_BUFFER_SIZE - 12]; // 参数buffer
    uint8_t lenth;                          // 参数长度

}fp_ack_packet_t;

typedef struct {
    uint16_t page_id;
    uint8_t count;

}fp_reg_params_t;

typedef struct {
    uint16_t page_id;
    uint16_t count;

}fp_delete_params_t;

#if (FP_ENABLE_LED_CONTROL)
typedef struct {
    uint8_t wait : 1;                       // 待控制
    uint8_t processing  : 1;                // 处理中
    uint8_t color : 4;                      // 颜色

}fp_led_control_t;
#endif

typedef struct {
    uint8_t step : 4;                       // 步骤
    uint8_t mode : 4;                       // 模式

    fp_status_t status;                     // 状态标识位
    fp_ack_packet_t ack_packet;             // 应答包

    uint32_t tick;                          // 计时
    uint16_t timeout_ms;                    // 超时时间

    fp_module_attr_t mdl_attr;              // 模块属性（固定）
    fp_function_attr_t func_attr;           // 功能属性（软件功能决定）

    // 操作参数
    union {
        fp_reg_params_t reg;
        fp_delete_params_t del;

    } params;

#if (FP_ENABLE_LED_CONTROL)
    fp_led_control_t led;
#endif
    fingerprint_Config_t* config;           // 硬件控制句柄指针
    fingerprint_callback_t callback;        // 回调函数     

} fp_context_t;

/***************Variable***************/


/***************Function***************/
void fingerprintRegister_callback(fingerprint_callback_t callback);
void fingerprint_init(fp_context_t *ctx);
void fingerprint_process(fp_context_t *ctx);

uint8_t fp_is_ready(fp_context_t *ctx, uint8_t mode);

#if (FP_ENABLE_LED_CONTROL)
uint8_t fingerprint_control_led(fp_context_t *ctx, uint8_t color);
#endif

/**************************************/

#endif /* __FINGERPRINT_H */
