/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_version.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-25
 */

#include "uart_protocol.h"
#include "task_system_time.h"
#include "event.h"
#include "fmc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "up_version"

HANDLER_DEFINE(UP_CMD_VERSION)
{
    frame_version_t *param = (frame_version_t *)(packet->payload);
    uint8_t status = STATUS_FAILED;

    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "status[%02X]", param->status);
    frame_version_ack_def_t version_param;
    memset(&version_param, 0, sizeof(frame_version_ack_def_t));

    uint32_t boot_verstion = 0;
    FMC_Read(0xF000, (uint8_t *)&boot_verstion, sizeof(boot_verstion));
    if (param->status == 0xff)
    {
        version_param.fw_funcode = CLIENT_ITEM_SN;
        version_param.boot_ver = boot_verstion;
        version_param.flash_ver = 0;
        version_param.fw_ver = PATCH_VERSION;
        status = STATUS_SUCCESS;
    }
    uart_msg_version(status, (uint8_t *)&version_param, sizeof(frame_version_ack_def_t));
    return 0;
}
