/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_get_record_log.c
 * Desc: 获取开锁记录（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-08
 *
 * 说明：命令字 0x09 / 应答码 0x89 与 ob_lock.h 的
 *       CMD_SERVER_GET_RECORD_LOG / CMD_LOCK_GET_RECORD_LOG_RSP 一致。
 *
 * 下发 payload（WiFi 模块侧即 kiot_tm_action_in_a_get_records_data_stu_t，pack(1)，共 7 字节）：
 *   uint32_t p_timestamp               // 时间戳（查询基准时刻，单位 s）
 *   uint8_t  p_record_event_type       // 记录事件类型：0=全部 1=操作记录 2=报警记录 3=访客记录
 *   uint8_t  p_record_cursor_count     // 本次要获取的记录条数
 *   uint8_t  p_record_cursor_direction // 方向：0=基于该时刻取过去的数据 1=取未来的数据
 *
 * 应答 payload（0x89）：N 条历史记录 raw，单条 8 字节（结构见 lock_log_def.h），按时间正序：
 *   p_record_event_type(1) + 事件类型(1) + p_timestamp(4) + 参数1(1) + p_key_id(1)
 *   事件类型/参数1 是 union，随 p_record_event_type 取操作/报警/访客类型
 *   长度上限 200 字节（instance: p_records_data_raw maxLength=200 -> 25 条）
 *   没有记录时回空 payload 的 0x89。
 */

#include "uart_protocol.h"
#include "lock_log.h"       // lock_log_get_records_raw()、lock_log_record_t

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_get_rec_log"

/* 0x89 应答 raw 上限：单条 8 字节 × 20 条 = 160（instance 里 p_records_data_raw maxLength=200，
 * p_record_cursor_count 上限 20，所以 160 够用） */
#define RECORD_RAW_MAX_CNT      (20)
#define RECORD_RAW_MAX_LEN      (RECORD_RAW_MAX_CNT * 8)

// [13744] (uart_protocol) [uart rx] cmd=0x09, sn=2, len=7, addr=3, enc=0
// [13749] A5 03 38 02 02 00 09 00 91 01 07 00 08 43 C7 6A 00 14 01
// [13757] (up_get_rec_log) req: timestamp=1791443720, event_type=0, count=20, direction=1

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_GET_RECORD_LOG)
{
    const kiot_tm_action_in_a_get_records_data_stu_t *req;
    uint8_t buf[RECORD_RAW_MAX_LEN];
    uint16_t len;

    if (packet->length < sizeof(kiot_tm_action_in_a_get_records_data_stu_t))
    {
        OB_LOGW(TAG, "len=%u too short, cmd=0x%02X", packet->length, packet->cmd);
        uart_msg_empty_ack(UP_CMD_GET_RECORD_LOG_ACK);
        return 0;
    }

    req = (const kiot_tm_action_in_a_get_records_data_stu_t *)packet->payload;

    len = lock_log_get_records_raw(req->p_record_event_type, req->p_timestamp,
                                   req->p_record_cursor_count, req->p_record_cursor_direction,
                                   buf, sizeof(buf));

    OB_LOGI(TAG, "req: timestamp=%u, event_type=%u, count=%u, direction=%u -> %u record(s)",
            req->p_timestamp, req->p_record_event_type,
            req->p_record_cursor_count, req->p_record_cursor_direction,
            len / sizeof(lock_log_record_t));

    uart_msg_records_raw(buf, len);     // len==0 时回空 payload 的 0x89

    return 0;
}
