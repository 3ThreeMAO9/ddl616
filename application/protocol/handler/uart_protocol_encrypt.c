/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_encrypt.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-04-03
 */

#include "uart_protocol.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "uart_protocol_encrypt"

HANDLER_DEFINE(UP_CMD_ENCRYPT)
{
#if (ENCRYPT_EN == true)

    frame_encrypt_t *param = (frame_encrypt_t*)(packet->payload);
    uint8_t status = STATUS_FAILED;
    OB_LOGD(TAG,"cmd 0x%02X tsn 0x%02X",packet->cmd,packet->TSN);
    OB_LOGD_DUMP(param->random,sizeof(frame_encrypt_t));

    set_auth_random(param->random);
   
    status = STATUS_SUCCESS;

    uart_msg_ack_encrypt_req(status, (uint8_t *)&param, sizeof(frame_encrypt_t));

#endif
    return 0;
}

