/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_light_ctl.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "uart_protocol.h"
#include "task_led.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_light_ctl"

// 字段	字节数	说明
// TSN	1	传输序号（不能为 0，每次发送时加 1， 命令和确认的TSN 相同。
// Cmd	1	0x04
// Len	2	0x00 0x04
// Event Type	1	0x00 = 键盘灯
// 0x01 = 开锁灯效
// 0x02 = 关锁灯效
// 0x03 = 设置成功灯效
// 0x04 = 设置失败灯效
// 0x05 = 系统锁定灯效
// 0x06 = 唤醒灯效
// Event Source	1	0x00 = 立即打断、0x01 = 连续触发、0x02 = 定时触发
// Event Code	2	【键盘灯】
// bit0=LED0
// bit1=LED1…
// bit15=LED15（1 = 亮，0 = 灭）

HANDLER_DEFINE(UP_CMD_LIGHT_CTL)
{
    frame_light_ctl_t *param = (frame_light_ctl_t*)(packet->payload);

    param->event_code = BIG_LITTLE_SWAP16(param->event_code);
    uart_msg_ack_light_ctl(STATUS_SUCCESS);
    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "type[%02X] source[%02X] code[%02X] ", param->event_type, param->event_source, param->event_code);

    switch (param->event_type)
    {
    case EVENT_KEYBOARD_LED:
        ledTaskHandle(LED_EVENT_SET_VALUE, param->event_code);
        break;
    case EVENT_UNLOCK_EFFECT:
        ledTaskHandle(LED_EVENT_UNLOCK, param->event_code);
        break;
    case EVENT_LOCK_EFFECT:
        ledTaskHandle(LED_EVENT_LOCK, param->event_code);
        break;
    case EVENT_SET_SUCCESS:
        break;
    case EVENT_SET_FAILED:
        ledTaskHandle(LED_EVENT_FAILED, param->event_code);
        break;
    case EVENT_SYSTEM_LOCK:
        break;
    case EVENT_WAKEUP:
        ledTaskHandle(LED_EVENT_WAKE_UP, param->event_code);
        break;
    default:
        break;
    }

    return 0;
}
