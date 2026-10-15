/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_set_volume.c
 * Desc: 设置音量（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-05
 *
 * 说明：接口骨架，报文结构未定，故不定义 payload、不解析、不应答。
 *       命令字 0x22 / 应答码 0xA2（PROP_CMD_LOCK_VOLUME）。
 *       接入参考：OB_LOCK_VOLUME_* 0=静音 1=低 2=中 3=高；player_task_volume_set() 立即生效
 *       （内部做值域校验）。音量无 Flash 持久化，断电回默认值（HIGH）。
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_set_vol"

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_SET_VOLUME)
{
    // TODO: 服务器下发的 payload 结构尚未确定，暂不解析、不应答。
    //       待格式确认后补：解析音量值 -> player_task_volume_set()
    //       -> 用 UP_CMD_SET_VOLUME_ACK（0xA2）回送应答。

    return 0;
}
