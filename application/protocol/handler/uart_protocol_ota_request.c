/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_protocol_ota_request.c
 * Desc:
 * Version: 1.0.0
 * Revision: XQR
 * Date: 2025-12-02
 */

#include "uart_protocol.h"
#include "ota_helper.h"
#include "fmc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_ota_request"

HANDLER_DEFINE(UP_CMD_OTA_REQUEST) {
    ota_fmc_area_t *param = (ota_fmc_area_t*)(packet->payload);
    uint8_t status = STATUS_FAILED;

    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "checksum1[%08X]", param->checksum1);
    OB_LOGD(TAG, "magic[%08X]", param->magic);
    OB_LOGD(TAG, "file_type[%02X]", param->file_type);
    OB_LOGD(TAG, "size[%08X]", param->size);
    OB_LOGD(TAG, "checksum2[%08X]", param->checksum2);

    param->state = 1;
    ota_helper_save_fmc_area(param);

    ota_helper_set_boot(1);

    return 0;
}
