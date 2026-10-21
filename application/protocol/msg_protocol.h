/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#ifndef MSG_PROTOCOL_HH
#define MSG_PROTOCOL_HH

#include <stdint.h>
#include "uart_protocol.h"
#include "uart_packet.h"
#include "config.h"
#include "tm_data.h"
/***********Macro***********/

/***********Enum***********/
typedef enum{
    EVENT_KEY_ID_NONE    = 0x00,
    EVENT_KEY_RESET_ID   = 0x01,
    EVENT_KEY_SETTING_ID = 0x02,
}event_key_id_t;
typedef enum{
    EVENT_KEY_REPORT_NONE   = 0x00,   /* 无上报 */
    EVENT_KEY_REPORT_SINGLE = 0x01,   /* 单击 */
    EVENT_KEY_REPORT_DOUBLE = 0x02,   /* 双击 */
    EVENT_KEY_REPORT_LONG   = 0x03,   /* 长按 */
}event_key_report_t;


/***********Struct***********/
#pragma pack(1)


/*********************接收处理的结构体********************/
typedef struct
{
    uint8_t event_key_id;
    uint8_t event_key_report;
} frame_key_t;

typedef struct
{
    uint8_t status;
    uint8_t MAC[6];
} frame_ble_mac_t;

/*********************发送处理的结构体********************/
typedef struct {
    uint8_t status;
} frame_ack_def_t;

typedef struct {
    uint8_t cnt;            // 响几次
    uint32_t timeout;       // 间隔多久  单位：1ms
} frame_bell_def_t;

/**************************************************/

#pragma pack()

/***********Variable***********/

/***********Function***********/
void uart_msg_bell(uint8_t cnt, uint32_t timeout);
void uart_msg_blue_mac(void);
void uart_msg_join_net(void);

// 一次性密码校验（0x69）
void uart_msg_totp_verify(const uint8_t *pwd6);

// 带 1 字节 status 的应答：**只给门锁<->WiFi 模块自定义指令（0x60~0x64）用**；
// kiot 的 action 应答（0x8x）是透传，一律用 uart_msg_empty_ack 或直接回 out 内容，不带 status
void uart_msg_ack(uint8_t cmd, uint8_t status);

// 原始用户数据应答（0x81）：直接回一段 raw 数据
void uart_msg_userdata_raw(uint8_t *data, uint16_t len);

// 添加用户应答（0x85）：payload = 1 字节用户ID（= a_add_user 的 out）
void uart_msg_add_user_ack(uint8_t user_id);

// 无 payload 应答：传入"请求命令字"即可，内部自动置最高位（0x07 → 0x87）
void uart_msg_empty_ack(uint8_t cmd);

// 用户列表时间戳应答（0x88）：payload = uint32 数组，count = 项数
void uart_msg_userlist_timestamp(uint32_t *timestamp, uint16_t count);

// 历史记录应答（0x89）：payload = N 条记录 raw（单条 8 字节，见 lock_log_def.h）
void uart_msg_records_raw(uint8_t *data, uint16_t len);

// 属性应答（cmd 传属性请求命令字，内部自动置位）：payload = 属性值
void uart_msg_property_ack(uint8_t cmd, uint8_t *data, uint16_t len);

// 属性主动上报（cmd 传属性原命令字）：payload = 属性值；对端需回 cmd|0x80
void uart_msg_property_report(uint8_t cmd, uint8_t *data, uint16_t len);

// 语音包版本主动上报（0x30）：payload = "103\0"（3 位数字 + 结尾 0，共 4 字节），WiFi 回 0xB0
void uart_msg_report_voice_version(void);

// 前板版本串 "V39.140.003"，返回长度（不含 '\0'）
uint16_t uart_msg_front_version_str(char *buf, uint16_t size);

// 上报前板版本（0x20）
void uart_msg_report_front_panel_version(void);

// 后板(WiFi)版本
void     uart_msg_get_back_version(void);                       // 0x68 请求
uint8_t  uart_msg_back_version_valid(void);                     // 1 = 已取到
void     uart_msg_set_back_version(const uint8_t *ver4);        // 保存 0xE8 的 4 字节
uint16_t uart_msg_back_version_str(char *buf, uint16_t size);   // "V1.2.3.4"，返回长度（不含 '\0'）
void     uart_msg_report_back_panel_version(void);              // 上报 0x21

// 密钥发送（0x70）：payload = 4 字节随机数（下标 1 为门锁密钥），出厂密钥加密，回执为通用 ACK 0xF0
void uart_msg_send_lock_key(void);

// 时间查询（0x67）：payload 空，WiFi 回 0xE7 + 8 字节（UTC 4B + 时区偏移 4B，小端）
void uart_msg_time_query(void);

// ---- 事件上报（门锁 -> WiFi 模块 -> APP/服务器）----
// 恢复出厂设置（0x45）：payload = 4 字节时间戳（kiot_tm_event_e_reset_factory_stu_t）
void uart_msg_event_reset_factory(uint32_t timestamp);
// 数字钥匙添加完成（0x47）：result 取 kiot_tm_p_add_key_result_enum_t；type 用本工程 user_type_t（内部转物模型编号）
void uart_msg_event_key_add_complete(uint8_t result, uint8_t type, uint8_t key_id, uint32_t timestamp);
// 指纹添加状态/进度（0x46）：press_cnt = 已成功按压次数
void uart_msg_event_fingerprint_add_status(uint8_t press_cnt);

/*****************************/

#endif // MSG_PROTOCOL_HH
