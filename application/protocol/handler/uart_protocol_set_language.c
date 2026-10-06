/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_set_language.c
 * Desc: 设置语言（服务器下发 -> WiFi 模块 -> 门锁）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-10-05
 *
 * 说明：本文件目前只实现接口骨架。
 *       服务器下发的报文结构尚未确定，因此不定义 payload 结构体、不解析、不应答。
 *       命令字 0x19 / 应答码 0x99 与 ob_lock.h 的 PROP_CMD_LANGUAGE（当前语言）一致。
 *
 * 接入参考（门锁侧已有实现，见 fsm/menu/fsm_menu_language_settings.c:30-37）：
 *   取值 language_set_t：0=中文 1=英文 2=西班牙语 3=法语
 *   setUserParameter(USER_PARA_LANGUAGE_MODE_ID, <值>);   // 写 Flash 持久化
 *   player_task_language_set(OB_LOCK_LANGUAGE_XX);        // 切换语音包
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_set_lang"

/***************Function***************/

// ============================================================
// 协议 handler
// ============================================================
HANDLER_DEFINE(UP_CMD_SET_LANGUAGE)
{
    // TODO: 服务器下发的 payload 结构尚未确定，暂不解析、不应答。
    //       待格式确认后补：解析语言值 -> setUserParameter() + player_task_language_set()
    //       -> 用 UP_CMD_SET_LANGUAGE_ACK（0x99）回送应答。
    OB_LOGI(TAG, "recv set language, cmd=0x%02X, len=%u (interface only)",
            packet->cmd, packet->length);

    return 0;
}
