/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */
#ifndef UART_PROTOCOL_HH
#define UART_PROTOCOL_HH

#include <stdint.h>
#include <string.h>
#include "uart_packet.h"
#include "task_uart.h"
#include "msg_protocol.h"

#include "event.h"

/***********Macro***********/

#define HANDLER_NAME(_cmd) uart_handler##_cmd
#define HANDLER_DEFINE(cmd) uint32_t HANDLER_NAME(_##cmd)(uart_packet_t *packet)

#define HEART_TIME_OUT                      (3000)   // 心跳间隔 ms
#define OTA_TO_BOOT_TIME_OUT                (1000)   // OTA重启 ms
#define PARAM_DATA_TIME_OUT                 (1000)   // 参数数据同步 ms

#define UP_CMD_HEART                        (0x2A)   // 心跳命令
#define UP_CMD_ACK_HEART                    (0xAA)   // 心跳应答

/*************************/
#define STATUS_SUCCESS                      (0x00)  // 操作成功
#define STATUS_FAILED                       (0x01)  // 操作失败
#define STATUS_TSN_DUPLICATE                (0x90)  // TSN序列号重复

/* WiFi 模块下发指令：命令字与 ob_lock.h 的 CMD_SERVER_* 保持一致 */

#define UP_CMD_GET_USERDATA_RAW             (0x01)  // 获取原始用户数据（WiFi模块 -> 门锁）
#define UP_CMD_ADD_NUMBER_KEY               (0x02)  // 添加数字密码（WiFi模块 -> 门锁）
#define UP_CMD_CANCEL_NUMBER_KEY            (0x03)  // 取消添加数字钥匙（WiFi模块 -> 门锁）
#define UP_CMD_DELETE_NUMBER_KEY            (0x04)  // 删除数字密码（WiFi模块 -> 门锁）
#define UP_CMD_ADD_USER                     (0x05)  // 添加用户（WiFi模块 -> 门锁）
#define UP_CMD_DELETE_USER                  (0x06)  // 删除用户（WiFi模块 -> 门锁）
#define UP_CMD_SET_USER                     (0x07)  // 设置用户（WiFi模块 -> 门锁）
#define UP_CMD_GET_USERLIST_TIMESTAMP       (0x08)  // 获取用户列表时间戳（WiFi模块 -> 门锁）
#define UP_CMD_GET_RECORD_LOG               (0x09)  // 获取开锁记录（WiFi模块 -> 门锁）

/* WiFi 模块下发指令：属性读写（命令字对齐 ob_lock.h 的 PROP_CMD_*）
 * payload = [opt(1字节), value...]：opt 0=读取（value 是模组缓冲值，须忽略）/ 1=设置
 * 应答与其它命令统一：回 cmd|0x80 + 属性值（一包）*/

#define UP_CMD_PROP_MODEL_SAFE              (0x16)  // 双重验证模式（读写：0=通用 1=双重）
#define UP_CMD_PROP_MODE_DEFENSE            (0x17)  // 布防模式（读写：0=撤防 1=布防）
#define UP_CMD_PROP_LANGUAGE                (0x19)  // 门锁语言（读写：字符串 "zh"/"en"/"sp"/"fr"）
#define UP_CMD_PROP_LOCK_VOLUME             (0x22)  // 门锁音量（读写：0=静音 1=低 2=中 3=高）
#define UP_CMD_PROP_LOCK_STATUS             (0x23)  // 门锁状态（只读：1=关锁 2=开锁）
#define UP_CMD_PROP_LOCKED_INSIDE_STATUS    (0x24)  // 门内反锁状态（只读：0=未反锁 1=反锁）
#define UP_CMD_PROP_CHANNEL_MODE_ENABLED    (0x2b)  // 通道(常开)模式开关（读写：0=关闭 1=开启）
#define UP_CMD_PROP_CHANNEL_MODE_POLICY     (0x2c)  // 通道模式策略（暂不实现：固定回 0=永久）
#define UP_CMD_PROP_CHANNEL_MODE            (0x2d)  // 通道模式时段（暂不实现：固定回空）
#define UP_CMD_PROP_BATTERY_INFO            (0x31)  // 电池信息（只读）
#define UP_CMD_PROP_SOLAR_PANELS_CONN_STATUS (0x33) // 太阳能板连接状态（只读：1=已连接 2=已断开）

#define UP_CMD_PROP_OPT_READ                (0)     // 属性 opt：读取
#define UP_CMD_PROP_OPT_WRITE               (1)     // 属性 opt：设置

/* 门锁上报事件（门锁 -> WiFi模块 -> APP/服务器），命令字与 ob_lock.h 的 EVENT_CMD_* 保持一致 */
#define UP_EVENT_RESET_FACTORY              (0x45)  // 恢复出厂设置事件
#define UP_EVENT_FINGERPRINT_ADD_STATUS     (0x46)  // 指纹添加状态/进度（按压次数）
#define UP_EVENT_KEY_ADD_COMPLETE           (0x47)  // 数字钥匙添加完成



#define UP_CMD_KEY                          (0x60)  // 按键事件
#define UP_CMD_BELL                         (0x61)  // 门铃事件
#define UP_CMD_JOIN_NET                     (0x62)  // 入网命令
#define UP_CMD_BLUE_MAC                     (0x63)  // 获取蓝牙MAC
#define UP_CMD_BLUE_VERSION                 (0x64)  // 获取蓝牙版本

/* 链路层：串口加密密钥交换（出厂密钥 K_F 加密）
 * 0x70 密钥发送（门锁 -> WiFi）：payload = 4 字节密钥帧（下标 1 为 K_L），WiFi 回 ACK 0xF0
 * 0x71 密钥请求（WiFi -> 门锁）：payload 空，门锁回 0xF1（同样带 4 字节密钥帧） */
#define UP_CMD_SEND_LOCK_KEY                (0x70)  // 密钥发送（门锁 -> WiFi），回执 0xF0
#define UP_CMD_REQUEST_KEY                  (0x71)  // 密钥请求（WiFi -> 门锁），回执 0xF1
#define UP_CMD_REQUEST_KEY_ACK              SET_UART_ACK_CMD(UP_CMD_REQUEST_KEY)     // 0xF1（门锁回）




#define UP_CMD_KEY_ACK                      SET_UART_ACK_CMD(UP_CMD_KEY)
#define UP_CMD_BELL_ACK                     SET_UART_ACK_CMD(UP_CMD_BELL)
#define UP_CMD_JOIN_NET_ACK                 SET_UART_ACK_CMD(UP_CMD_JOIN_NET)    
#define UP_CMD_BLUE_MAC_ACK                 SET_UART_ACK_CMD(UP_CMD_BLUE_MAC)    
#define UP_CMD_BLUE_VERSION_ACK             SET_UART_ACK_CMD(UP_CMD_BLUE_VERSION)
#define UP_CMD_GET_USERDATA_RAW_ACK         SET_UART_ACK_CMD(UP_CMD_GET_USERDATA_RAW) // 0x81
#define UP_CMD_ADD_NUMBER_KEY_ACK           SET_UART_ACK_CMD(UP_CMD_ADD_NUMBER_KEY)   // 0x82
#define UP_CMD_CANCEL_NUMBER_KEY_ACK        SET_UART_ACK_CMD(UP_CMD_CANCEL_NUMBER_KEY) // 0x83
#define UP_CMD_DELETE_NUMBER_KEY_ACK        SET_UART_ACK_CMD(UP_CMD_DELETE_NUMBER_KEY) // 0x84
#define UP_CMD_ADD_USER_ACK                 SET_UART_ACK_CMD(UP_CMD_ADD_USER)      // 0x85
#define UP_CMD_DELETE_USER_ACK              SET_UART_ACK_CMD(UP_CMD_DELETE_USER)   // 0x86
#define UP_CMD_SET_USER_ACK                 SET_UART_ACK_CMD(UP_CMD_SET_USER)      // 0x87
#define UP_CMD_GET_USERLIST_TIMESTAMP_ACK   SET_UART_ACK_CMD(UP_CMD_GET_USERLIST_TIMESTAMP) // 0x88
#define UP_CMD_GET_RECORD_LOG_ACK           SET_UART_ACK_CMD(UP_CMD_GET_RECORD_LOG) // 0x89
#define UP_CMD_PROP_MODEL_SAFE_ACK          SET_UART_ACK_CMD(UP_CMD_PROP_MODEL_SAFE)               // 0x96
#define UP_CMD_PROP_MODE_DEFENSE_ACK        SET_UART_ACK_CMD(UP_CMD_PROP_MODE_DEFENSE)             // 0x97
#define UP_CMD_PROP_LANGUAGE_ACK            SET_UART_ACK_CMD(UP_CMD_PROP_LANGUAGE)                 // 0x99
#define UP_CMD_PROP_LOCK_VOLUME_ACK         SET_UART_ACK_CMD(UP_CMD_PROP_LOCK_VOLUME)              // 0xA2
#define UP_CMD_PROP_LOCK_STATUS_ACK         SET_UART_ACK_CMD(UP_CMD_PROP_LOCK_STATUS)              // 0xA3
#define UP_CMD_PROP_LOCKED_INSIDE_STATUS_ACK SET_UART_ACK_CMD(UP_CMD_PROP_LOCKED_INSIDE_STATUS)    // 0xA4
#define UP_CMD_PROP_CHANNEL_MODE_ENABLED_ACK SET_UART_ACK_CMD(UP_CMD_PROP_CHANNEL_MODE_ENABLED)      // 0xAB
#define UP_CMD_PROP_CHANNEL_MODE_POLICY_ACK  SET_UART_ACK_CMD(UP_CMD_PROP_CHANNEL_MODE_POLICY)       // 0xAC
#define UP_CMD_PROP_CHANNEL_MODE_ACK         SET_UART_ACK_CMD(UP_CMD_PROP_CHANNEL_MODE)              // 0xAD
#define UP_CMD_PROP_BATTERY_INFO_ACK        SET_UART_ACK_CMD(UP_CMD_PROP_BATTERY_INFO)             // 0xB1
#define UP_CMD_PROP_SOLAR_PANELS_CONN_STATUS_ACK SET_UART_ACK_CMD(UP_CMD_PROP_SOLAR_PANELS_CONN_STATUS) // 0xB3
/***********Enum***********/

/***********Struct***********/
#pragma pack(1)


#pragma pack()

/***********Variable***********/

/***********Function***********/
uint16_t get_tsn(void);
uint8_t uart_protocol_receive_handle(uint8_t *data, uint16_t len);
void uart_protocol_heart_inc_time_out(void);
void uart_protocol_poll(void);
void uart_protocol_ota_to_boot_inc_time_out(void);
void uart_protocol_ota_poll(void);
void uart_protocol_clean_heart_send_cnt(void);
void uart_protocol_set_param_data_flag(uint8_t data);
void uart_protocol_param_poll(void);
/*****************************/

#endif // UART_PROTOCOL_HH




