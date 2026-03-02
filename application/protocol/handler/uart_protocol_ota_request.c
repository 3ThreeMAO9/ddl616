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
    OB_LOGI(TAG, "[%s]  UP_CMD_OTA_REQUEST received",__func__);
    ota_helper_set_ota_process(1);
    ota_helper_set_boot(FMC_BOOT_TO_OTA);

    return 0;
}
