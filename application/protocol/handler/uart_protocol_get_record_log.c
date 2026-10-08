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
 * 应答 payload：历史记录原始数据（尚未确定，本地记录存储未实现，见下方 TODO）
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_get_rec_log"

/* 记录事件类型，对应 p_record_event_type */
#define RECORD_EVENT_TYPE_ALL       (0)     // 全部
#define RECORD_EVENT_TYPE_OPERATION (1)     // 操作记录
#define RECORD_EVENT_TYPE_ALARM     (2)     // 报警记录
#define RECORD_EVENT_TYPE_GUEST     (3)     // 访客记录

/* 查询方向，对应 p_record_cursor_direction */
#define RECORD_CURSOR_DIR_PAST      (0)     // 基于某时刻获取过去的数据
#define RECORD_CURSOR_DIR_FUTURE    (1)     // 基于某时刻获取未来的数据

// [13744] (uart_protocol) [uart rx] cmd=0x09, sn=2, len=7, addr=3, enc=0
// [13749] A5 03 38 02 02 00 09 00 91 01 07 00 08 43 C7 6A 00 14 01
// [13757] (up_get_rec_log) req: timestamp=1791443720, event_type=0, count=20, direction=1

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_GET_RECORD_LOG)
{
    kiot_tm_action_in_a_get_records_data_stu_t *req =
        (kiot_tm_action_in_a_get_records_data_stu_t *)packet->payload;

    OB_LOGI(TAG, "req: timestamp=%u, event_type=%u, count=%u, direction=%u",
            req->p_timestamp, req->p_record_event_type,
            req->p_record_cursor_count, req->p_record_cursor_direction);

    // TODO: 本地历史记录存储尚未接入，先回空应答占位（0x89）。
    //       待记录存储（操作/报警/访客记录）就绪后补：
    //       按 event_type + timestamp + count + direction 查询记录
    //       -> 用 UP_CMD_GET_RECORD_LOG_ACK（0x89）回送记录原始数据。
    uart_msg_empty_ack(UP_CMD_GET_RECORD_LOG_ACK);

    return 0;
}
