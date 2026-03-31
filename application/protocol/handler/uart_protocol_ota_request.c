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
#define TAG "ota_request"

HANDLER_DEFINE(UP_CMD_OTA_REQUEST) {
    // 2. 只解析payload中存在的字段（checksum1~checksum2）
    ota_fmc_area_t param = {0}; // 定义结构体变量，而非直接强转指针
    memcpy(&param, packet->payload, sizeof(ota_fmc_area_t) - sizeof(uint32_t)); // 仅拷贝16字节有效数据

    uint8_t status = STATUS_FAILED;

    // 3. 打印有效字段（避免访问不存在的state）
    OB_LOGD(TAG, "cmd[%02X] tsn[%02X]", packet->cmd, packet->TSN);
    OB_LOGD(TAG, "checksum1[%04X]", param.checksum1);
    OB_LOGD(TAG, "magic[%04X]", param.magic);
    OB_LOGD(TAG, "file_type[%02X]", param.file_type);
    OB_LOGD(TAG, "version[%02X%02X%02X]", param.version[0], param.version[1], param.version[2]);
    OB_LOGD(TAG, "size[%08X]", param.size);
    OB_LOGD(TAG, "checksum2[%08X]", param.checksum2);

    // 4. 给state赋值（操作本地变量，而非payload越界地址）
    param.state = OTA_STATE_READY; 

    // 5. 保存完整结构体到FMC/Flash（包含state）
    ota_helper_save_fmc_area(&param);
    status = STATUS_SUCCESS;
    uart_msg_ack_ota_request(status);

    uart_protocol_ota_to_boot_inc_time_out();
    return 0;
}
