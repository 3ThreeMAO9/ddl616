/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol.h
 * Desc: 门锁 ↔ WiFi 模块串口协议：命令字、应答规则、处理函数声明
 * Version 1.1.0
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

/*==============================================================================
 * 1. Handler 注册
 *    HANDLER_DEFINE(UP_CMD_X)   定义 uart_handler_UP_CMD_X()
 *    HANDLER_IMPORT(UP_CMD_X)   在 uart_protocol_try_handle() 的 switch 中注册
 *============================================================================*/
#define HANDLER_NAME(_cmd)      uart_handler##_cmd
#define HANDLER_DEFINE(cmd)     uint32_t HANDLER_NAME(_##cmd)(uart_packet_t *packet)

/*==============================================================================
 * 2. 时间常量（ms）
 *============================================================================*/
#define HEART_TIME_OUT                      (3000)      // 心跳间隔（兼时间同步周期）
#define OTA_TO_BOOT_TIME_OUT                (1000)      // OTA 重启延时
#define PARAM_DATA_TIME_OUT                 (1000)      // 参数数据同步

/*==============================================================================
 * 3. 状态码
 *============================================================================*/
#define STATUS_SUCCESS                      (0x00)      // 操作成功
#define STATUS_FAILED                       (0x01)      // 操作失败
#define STATUS_TSN_DUPLICATE                (0x90)      // TSN 序列号重复

/*==============================================================================
 * 4. 命令字
 *    ACK 命令 = 请求命令字 | 0x80（SET_UART_ACK_CMD，见 uart_packet.h）
 *    编号分布：0x01~0x0F 动作 | 0x10~0x3F 属性 | 0x40~0x5F 事件
 *              0x60~0x6F 自定义 | 0x70~0x7F 链路层 | 0x80~0xFF ACK
 *============================================================================*/

/* ---- 4.1 动作类（WiFi → 门锁，命令字对齐 ob_lock.h 的 CMD_SERVER_*）---- */
#define UP_CMD_GET_USERDATA_RAW             (0x01)      // 获取原始用户数据
#define UP_CMD_ADD_NUMBER_KEY               (0x02)      // 添加数字密码
#define UP_CMD_CANCEL_NUMBER_KEY            (0x03)      // 取消添加数字钥匙
#define UP_CMD_DELETE_NUMBER_KEY            (0x04)      // 删除数字密码
#define UP_CMD_ADD_USER                     (0x05)      // 添加用户
#define UP_CMD_DELETE_USER                  (0x06)      // 删除用户
#define UP_CMD_SET_USER                     (0x07)      // 设置用户
#define UP_CMD_GET_USERLIST_TIMESTAMP       (0x08)      // 获取用户列表时间戳
#define UP_CMD_GET_RECORD_LOG               (0x09)      // 获取开锁记录

/* ---- 4.2 属性读写（命令字对齐 ob_lock.h 的 PROP_CMD_*）
 *      payload = [opt][value...]：opt 0=读取（value 为模组缓冲值，忽略）/ 1=设置
 *      应答统一：cmd|0x80 + 属性值（一包）---- */
#define UP_CMD_PROP_MODEL_SAFE              (0x16)      // 双重验证模式（读写：0=通用 1=双重）
#define UP_CMD_PROP_MODE_DEFENSE            (0x17)      // 布防模式（读写：0=撤防 1=布防）
#define UP_CMD_PROP_LANGUAGE                (0x19)      // 门锁语言（读写："zh"/"en"/"sp"/"fr"）
#define UP_CMD_PROP_LOCK_VOLUME             (0x22)      // 门锁音量（读写：0=静音 1=低 2=中 3=高）
#define UP_CMD_PROP_LOCK_STATUS             (0x23)      // 门锁状态（只读：1=关锁 2=开锁）
#define UP_CMD_PROP_LOCKED_INSIDE_STATUS    (0x24)      // 门内反锁状态（只读：0=未反锁 1=反锁）
#define UP_CMD_PROP_CHANNEL_MODE_ENABLED    (0x2b)      // 通道(常开)模式开关（读写：0=关 1=开）
#define UP_CMD_PROP_CHANNEL_MODE_POLICY     (0x2c)      // 通道模式策略（0=永久 1=自定义）
#define UP_CMD_PROP_CHANNEL_MODE            (0x2d)      // 通道模式时段（最多 4 组）
#define UP_CMD_PROP_BATTERY_INFO            (0x31)      // 电池信息（只读）
#define UP_CMD_PROP_SOLAR_PANELS_CONN_STATUS (0x33)     // 太阳能板连接状态（只读：1=已连 2=断开）

#define UP_CMD_PROP_OPT_READ                (0)         // 属性 opt：读取
#define UP_CMD_PROP_OPT_WRITE               (1)         // 属性 opt：设置

/* ---- 4.3 事件上报（门锁 → WiFi → APP，命令字对齐 EVENT_CMD_*）---- */
#define UP_EVENT_RESET_FACTORY              (0x45)      // 恢复出厂设置
#define UP_EVENT_FINGERPRINT_ADD_STATUS     (0x46)      // 指纹添加状态/进度（按压次数）
#define UP_EVENT_KEY_ADD_COMPLETE           (0x47)      // 数字钥匙添加完成

/* ---- 4.4 自定义命令 ---- */
#define UP_CMD_KEY                          (0x60)      // 按键事件
#define UP_CMD_BELL                         (0x61)      // 门铃事件
#define UP_CMD_JOIN_NET                     (0x62)      // 入网命令
#define UP_CMD_BLUE_MAC                     (0x63)      // 获取蓝牙 MAC
#define UP_CMD_BLUE_VERSION                 (0x64)      // 获取蓝牙版本
#define UP_CMD_JOIN_NET_RESULT              (0x65)      // 入网(蓝牙绑定)结果：0=成功 1=失败，回执 0xE5

/* ---- 4.5 时间同步（见《串口时间同步对接说明 v1.0》）
 *      0x66 下发：payload 8 字节 [UTC 4B LE][tz_offset 4B LE(秒)]，门锁回 0xE6
 *      0x67 查询：payload 空，WiFi 回 0xE7 + 8 字节；空 0xE7 = 暂无有效时间 ---- */
#define UP_CMD_TIME_PUSH                    (0x66)      // 时间下发（WiFi → 门锁）
#define UP_CMD_TIME_QUERY                   (0x67)      // 时间查询（门锁 → WiFi）

/* ---- 4.6 链路层：密钥交换（出厂密钥 K_F 加密）
 *      0x70 发送：payload 4 字节密钥帧（下标 1 为 K_L），WiFi 回 0xF0
 *      0x71 请求：payload 空，门锁回 0xF1（同样带 4 字节密钥帧）---- */
#define UP_CMD_SEND_LOCK_KEY                (0x70)      // 密钥发送（门锁 → WiFi）
#define UP_CMD_REQUEST_KEY                  (0x71)      // 密钥请求（WiFi → 门锁）

/* ---- 4.7 需要单独写 handler 的 ACK（其余 ACK 由 uart_protocol.c 统一清重传）
 *      HANDLER_DEFINE 需要完整标识符做 token 粘贴，故在此命名 ---- */
#define UP_CMD_BLUE_MAC_ACK                 SET_UART_ACK_CMD(UP_CMD_BLUE_MAC)       // 0xE3
#define UP_CMD_BLUE_VERSION_ACK             SET_UART_ACK_CMD(UP_CMD_BLUE_VERSION)   // 0xE4
#define UP_CMD_TIME_QUERY_ACK               SET_UART_ACK_CMD(UP_CMD_TIME_QUERY)     // 0xE7

/*==============================================================================
 * 5. 接口
 *============================================================================*/
/* 收发与轮询（由 task_protocol 驱动） */
uint16_t get_tsn(void);
uint8_t  uart_protocol_receive_handle(uint8_t *data, uint16_t len);
void     uart_protocol_poll(void);

/* 心跳（时间查询复用该机制） */
void uart_protocol_heart_inc_time_out(void);            // 重置心跳计时
void uart_protocol_heart_expire(void);                  // 让心跳立刻到期（唤醒后补发）

/* 串口活动时间（收发都会刷新），供休眠判定"串口是否已安静" */
void     uart_protocol_comm_touch(void);                // 刷新最近一次收发时刻
uint32_t uart_protocol_idle_ms(void);                   // 距最近一次收发的毫秒数

/* OTA */
void uart_protocol_ota_to_boot_inc_time_out(void);
void uart_protocol_ota_poll(void);

/* 参数同步 */
void uart_protocol_set_param_data_flag(uint8_t data);
void uart_protocol_param_poll(void);

#endif // UART_PROTOCOL_HH
