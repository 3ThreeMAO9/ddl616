/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_set_volume.c
 * Desc: 设置音量（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-05
 *
 * 说明：本文件目前只实现接口骨架。
 *       服务器下发的报文结构尚未确定，因此不定义 payload 结构体、不解析、不应答。
 *       命令字 0x22 / 应答码 0xA2 与 ob_lock.h 的 PROP_CMD_LOCK_VOLUME（门锁音量）一致。
 *
 * 接入参考（门锁侧已有实现，见 task/task_player/task_player.c:89-94）：
 *   取值 OB_LOCK_VOLUME_*：0=静音 1=低 2=中 3=高（bsp_system_def.h:61-66）
 *   player_task_volume_set(<值>);   // 立即生效，内部已做值域校验
 *   注意：音量当前无 Flash 持久化，断电后回到默认值（player_task_init 中固定设为 HIGH）。
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
    OB_LOGI(TAG, "recv set volume, cmd=0x%02X, len=%u (interface only)",
            packet->cmd, packet->length);

    return 0;
}
