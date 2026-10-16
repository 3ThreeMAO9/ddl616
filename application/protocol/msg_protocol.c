/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "msg_protocol.h"
#include "utils.h"
#include "user.h"               // 事件上报要按本工程 user_type_t 转物模型编号
#include <stdarg.h>  

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "msg_protocol"

/**
 * @brief 通用串口消息封装发送
 * @param cmd        命令字
 * @param payload    数据体
 * @param payload_len 数据体长度
 */
static void uart_msg_common_send(uint8_t cmd, uint8_t* payload, uint16_t payload_len)
{
    static uint16_t s_sn = 0;
    uint8_t buf[UART0_BUF_LEN];
    uart_packet_t* pkt = (uart_packet_t*)buf;

    // 1. 填充头部
    pkt->mark   = LOCK_PACKET_MARK;                                                                 // 0xA5
    pkt->attr   = lock_packet_build_attr(LOCK_PACKET_ADDR_FRONT, LOCK_PACKET_ENCRYPT_TYPE_NONE);    // 无加密
    pkt->sn     = s_sn++;                                                                           // 序列号
    pkt->cmd    = cmd;
    pkt->random = 0;                                                                                // 无加密时填 0
    pkt->length = payload_len;

    // 2. 填 payload
    if (payload != NULL && payload_len > 0) {
        memcpy(pkt->payload, payload, payload_len);
    }

    // 3. 算 checksum2（明文 payload）
    pkt->checksum2 = check_sum(pkt->payload, payload_len);

    // 4. 加密（如果启用）
    // if (enc != NONE) data_encrypt(pkt->random, pkt->payload, payload_len);

    // 5. 算 checksum1（attr + sn~payload，跳过 checksum1）
    pkt->checksum1 = check_sum(buf + 1, 1)                // attr (1 字节)
                   + check_sum(buf + 4, 8 + payload_len); // sn~payload

    // 6. 打印日志
    // OB_LOGD(TAG, "tx cmd=0x%02X, sn=%u, len=%u", cmd, pkt->sn, payload_len);
    // OB_LOGD_DUMP(buf, LOCK_PACKET_HEAD_SIZE + payload_len);

    // 7. 入队发送
    uint16_t total_len = LOCK_PACKET_HEAD_SIZE + payload_len;
    uartTaskQueuePut(buf, pkt->sn, pkt->cmd, total_len);
}

void uart_msg_bell(uint8_t cnt, uint32_t timeout)
{
    frame_bell_def_t data = {
        .cnt = cnt,
        .timeout = timeout,
    };
    uart_msg_common_send(UP_CMD_BELL, (uint8_t *)&data, sizeof(frame_bell_def_t));
}

void uart_msg_blue_mac(void)
{
    uart_msg_common_send(UP_CMD_BLUE_MAC, NULL, 0);
}

void uart_msg_blue_version(void)
{
    uart_msg_common_send(UP_CMD_BLUE_VERSION, NULL, 0);
}

void uart_msg_join_net(void)
{
    uart_msg_common_send(UP_CMD_JOIN_NET, NULL, 0);
}

void uart_msg_ack(uint8_t cmd, uint8_t status)
{
    frame_ack_def_t ack = {.status = status};
    uart_msg_common_send(cmd, (uint8_t *)&ack, sizeof(frame_ack_def_t));
}

void uart_msg_empty_ack(uint8_t cmd)
{
    uart_msg_common_send(cmd, NULL, 0);
}

// 原始用户数据应答（0x81）：直接回一段 raw 数据
void uart_msg_userdata_raw(uint8_t *data, uint16_t len)
{
    uart_msg_common_send(UP_CMD_GET_USERDATA_RAW_ACK, data, len);
}

// 添加用户应答（0x85）：payload = 1 字节用户ID（kiot_tm_action_out_a_add_user_raw_stu_t）
void uart_msg_add_user_ack(uint8_t user_id)
{
    uart_msg_common_send(UP_CMD_ADD_USER_ACK, &user_id, sizeof(user_id));
}

void uart_msg_userlist_timestamp(uint32_t *timestamp, uint16_t count)
{
    uart_msg_common_send(UP_CMD_GET_USERLIST_TIMESTAMP_ACK,
                         (uint8_t *)timestamp, count * sizeof(uint32_t));
}

// 历史记录应答（0x89）：直接回一段 N 条记录 raw
void uart_msg_records_raw(uint8_t *data, uint16_t len)
{
    uart_msg_common_send(UP_CMD_GET_RECORD_LOG_ACK, data, len);
}

// ---- 事件上报（门锁 -> WiFi 模块）----

// 本工程 user_type_t -> 物模型 kiot_tm_p_key_type_enum_t（两套编号不同，别混用）
static uint8_t event_key_type_export(uint8_t type)
{
    static const uint8_t tbl[USER_TYPE_PERMANENT_MAX] =
    {
        KIOT_TM_P_KEY_TYPE_MI_MA,       // 密码 -> 0
        KIOT_TM_P_KEY_TYPE_ZHI_WEN,     // 指纹 -> 4
        KIOT_TM_P_KEY_TYPE_KA_PIAN,     // 卡片 -> 3
        KIOT_TM_P_KEY_TYPE_REN_LIAN,    // 人脸 -> 7
    };

    return (type < USER_TYPE_PERMANENT_MAX) ? tbl[type] : 0xFF;
}

// 数字钥匙添加完成（0x47）：添加结果 + 钥匙类型 + 编号 + 添加时间
void uart_msg_event_key_add_complete(uint8_t result, uint8_t type, uint8_t key_id, uint32_t timestamp)
{
    kiot_tm_event_e_key_add_complete_stu_t ev;

    ev.p_add_key_result = result;
    ev.p_key_type       = event_key_type_export(type);
    ev.p_key_id         = key_id;
    ev.p_timestamp      = timestamp;

    uart_msg_common_send(UP_EVENT_KEY_ADD_COMPLETE, (uint8_t *)&ev, sizeof(ev));
}

// 恢复出厂设置（0x45）：时间戳（kiot_tm_event_e_reset_factory_stu_t）
void uart_msg_event_reset_factory(uint32_t timestamp)
{
    kiot_tm_event_e_reset_factory_stu_t ev;

    ev.p_timestamp = timestamp;

    uart_msg_common_send(UP_EVENT_RESET_FACTORY, (uint8_t *)&ev, sizeof(ev));
}

// 指纹添加状态/进度（0x46）：已成功按压次数
void uart_msg_event_fingerprint_add_status(uint8_t press_cnt)
{
    kiot_tm_event_e_fingerprint_add_status_stu_t ev;

    ev.p_fingerprint_add_status = press_cnt;

    uart_msg_common_send(UP_EVENT_FINGERPRINT_ADD_STATUS, (uint8_t *)&ev, sizeof(ev));
}
