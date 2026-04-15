/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_sleep.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-11
 */

#include "uart_protocol.h"
#include "task_system_time.h"
#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_sleep"

static uint8_t sleep_radar_flag = Disabled;

uint8_t get_sleep_radar_en(void)
{
    return sleep_radar_flag;
}

void set_sleep_radar_en(uint8_t data)
{
    sleep_radar_flag = data;
}

HANDLER_DEFINE(UP_CMD_SLEEP)
{
    frame_sleep_t *param = (frame_sleep_t*)(packet->payload);
    uint8_t status = STATUS_FAILED;

    packet->length = BIG_LITTLE_SWAP16(packet->length); // 大小端转换

    if (packet->length % sizeof(frame_sleep_t) != 0) {
        OB_LOGE(TAG, "UP_CMD_SLEEP: len(%d) not match group size", packet->length);
        return 0xff;
    }
    OB_LOGD(TAG, "UP_CMD_SLEEP: len=%d", packet->length);

    uint8_t group_cnt = packet->length / sizeof(frame_sleep_t);

    // 循环解析每组参数
    for(uint8_t i = 0; i < group_cnt; i++) {
        OB_LOGD(TAG, "UP_CMD_SLEEP[%d]: event = 0x%02X, data = 0x%02X", i + 1, param[i].event, param[i].data);
        switch (param[i].event){
        case SLEEP_MOTION_DETECTION:
            if (param[i].data == 1)
                set_sleep_radar_en(Enabled);
            else if (param[i].data == 0)
                set_sleep_radar_en(Disabled);
            break;
        default:
            OB_LOGE(TAG, "[%s] not default", __func__);
            break;
        }
    }
    baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_SLEEP);
    status = STATUS_SUCCESS;
    uart_msg_ack_sleep(status);
    return 0;
}
