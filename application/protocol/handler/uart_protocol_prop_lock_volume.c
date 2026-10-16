/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_prop_lock_volume.c
 * Desc: 门锁音量（WiFi 模块 -> 门锁），命令字 0x22 / 应答码 0xA2（PROP_CMD_LOCK_VOLUME）
 * Version 1.0.0
 * Date: 2026-10-16
 *
 * payload = [opt(1字节), value(1字节)]：opt 0=读取 / 1=设置；值 0=静音 1=低 2=中 3=高。
 * 音量不做 Flash 持久化，断电回默认高。
 */

#include "uart_protocol.h"
#include "task_player.h"

HANDLER_DEFINE(UP_CMD_PROP_LOCK_VOLUME)
{
    uint8_t opt = (packet->length >= 1) ? packet->payload[0] : UP_CMD_PROP_OPT_READ;
    uint8_t vol = (uint8_t)sys_volume_get();

    if ((UP_CMD_PROP_OPT_WRITE == opt) && (packet->length >= 2))
    {
        player_task_volume_set((int8_t)packet->payload[1]);     // 内部做值域校验
        vol = (uint8_t)sys_volume_get();
    }

    uart_msg_property_send(UP_CMD_PROP_LOCK_VOLUME, &vol, sizeof(vol));

    return 0;
}
