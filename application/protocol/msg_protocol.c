/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#include "msg_protocol.h"
#include <stdarg.h>  

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "msg_protocol"

// 定义数据体填充函数的类型（差异化逻辑的抽象）
typedef void (*fill_data_body_t)(frame_data_t *data, va_list args);

/**
 * @brief 通用串口消息封装发送函数
 * @param data_cmd   数据体的cmd值（差异化参数）
 * @param data_len   数据体（frame_data_t内具体字段）的长度
 * @param fill_func  数据体填充函数（处理差异化逻辑）
 * @param ...        填充函数需要的可变参数
 */
static void uart_msg_common_send(uint8_t data_cmd, 
                                 uint16_t data_body_len, fill_data_body_t fill_func, ...)
{
    uint8_t buf[32] = {0};
    frame_dl_t *frame = (frame_dl_t *)buf;

    // 1. 填充帧头（公共逻辑）
    frame->header.frame_start = 0xAA55;
    frame->header.control_sm4 = Disabled;
    frame->header.control_version = 0;
    frame->header.cmd = 0xFB;
    frame->header.length = BIG_LITTLE_SWAP16(sizeof(frame_data_t));

    // 2. 填充数据体（差异化逻辑，通过函数指针实现）
    frame->data.TSN = get_tsn();
    frame->data.cmd = data_cmd; // 抽离为入参，适配不同cmd
    frame->data.length = BIG_LITTLE_SWAP16(data_body_len);
    
    va_list args;
    va_start(args, fill_func);
    if (fill_func != NULL) {
        fill_func(&frame->data, args);
    }
    va_end(args);

    // 3. 计算CRC+入队发送（公共逻辑）
    uint16_t crc_data_len = FRAME_HEADER_LEN + sizeof(frame_data_t);
    frame->crc = BIG_LITTLE_SWAP16(crc16_ccitt(buf, crc_data_len));
    uint16_t send_len = sizeof(frame_dl_t);
    uartTaskQueuePut(buf, frame->data.TSN, frame->data.cmd, send_len);
}

// ------------------------------ 差异化数据体填充函数 ------------------------------
static void fill_key_board_data(frame_data_t *data, va_list args)
{
    uint8_t eventType = va_arg(args, int); // va_arg不支持uint8_t，需用int中转
    uint8_t keyValue = va_arg(args, int);
    
    data->key_broad.event_type = EVENT_TYPE_NULL;
    data->key_broad.event_source = EVENT_SOURCE_KEYPAD;
    data->key_broad.event_code = eventType;
    data->key_broad.key_value = keyValue;
}

static void fill_finger_data(frame_data_t *data, va_list args)
{
    uint8_t event_code = va_arg(args, int); // va_arg不支持uint8_t，需用int中转
    uint8_t finger_id = va_arg(args, int);
    data->finger.event_type = EVENT_TYPE_NULL;
    data->finger.event_source = EVENT_SOURCE_FINGER;
    data->finger.event_code = event_code;
    data->finger.finger_id = finger_id;
}

static void fill_stay_warn_data(frame_data_t *data, va_list args)
{
    uint8_t eventType = va_arg(args, int);
    uint8_t eventId = va_arg(args, int);
    
    data->radar.event_type = EVENT_TYPE_NULL;
    data->radar.event_source = EVENT_SOURCE_RADAR;
    data->radar.event_code = eventType;
    data->radar.event_id = eventId;
}

static void fill_nfc_verify_data(frame_data_t *data, va_list args)
{
    uint8_t event_code = va_arg(args, int);
    uint8_t *card_id = va_arg(args, uint8_t*);
    
    data->card.event_type = EVENT_TYPE_NULL;
    data->card.event_source = EVENT_SOURCE_CARD;
    data->card.event_code = event_code;
    memcpy(data->card.card_id, card_id, sizeof(data->card.card_id));
}

static void fill_tamper_key_warn_data(frame_data_t *data, va_list args)
{
    uint8_t eventType = va_arg(args, int);
    
    data->tamper.event_type = EVENT_TYPE_NULL;
    data->tamper.event_source = EVENT_SOURCE_TAMPER;
    data->tamper.event_code = eventType;
    data->tamper.event_id = 0;
}

static void fill_face_data(frame_data_t *data, va_list args)
{
    uint8_t eventType = va_arg(args, int);
    uint8_t *face_id = va_arg(args, uint8_t*);
    int face_id_size = va_arg(args, int);
    
    data->face.event_type = EVENT_TYPE_NULL;
    data->face.event_source = EVENT_SOURCE_FACE;
    data->face.event_code = eventType;
    if (face_id != NULL) {
        memcpy(data->face.face_id, face_id, sizeof(data->face.face_id));
    }
}

static void fill_version_data(frame_data_t *data, va_list args)
{
    uint8_t eventType = va_arg(args, int);
    uint8_t *version_data = va_arg(args, uint8_t*);
    int size = va_arg(args, int);
    
    // data->version.fw_funcode = 0;
    // data->version.fw_ver = 0;
    // data->version.flash_ver = 0;
    if (version_data != NULL) {
        memcpy(&data->version, version_data, size);
    }
}

static void fill_ota_request_data(frame_data_t *data, va_list args)
{
    uint8_t *ptr = va_arg(args, uint8_t*);
    
    if (ptr != NULL) {
        memcpy(data->data, ptr, sizeof(frame_ota_request_ack_t));
    }
}

static void fill_status_data(frame_data_t *data, va_list args)
{
    data->status = 0xFF;
}

static void fill_ack_def_data(frame_data_t *data, va_list args)
{
    uint8_t status = (uint8_t)va_arg(args, int);
    data->ack_def.status = status; // 赋值到帧数据体
}

// ------------------------------ 业务层发送函数 ------------------------------
void uart_msg_key_board(uint8_t eventType, uint8_t keyValue)
{
    uart_msg_common_send(UP_CMD_REPORT_ORDER, 
                         sizeof(frame_key_broad_t), fill_key_board_data, 
                         eventType, keyValue);
}

void uart_msg_finger(uint8_t event_code, uint8_t finger_id)
{
    uart_msg_common_send(UP_CMD_REPORT_ORDER, 
                         sizeof(frame_finger_t), fill_finger_data, 
                         event_code, finger_id);
}

void uart_msg_stay_warn(uint8_t eventType, uint8_t eventId)
{
    uart_msg_common_send(UP_CMD_REPORT_ORDER, 
                         sizeof(frame_radar_t), fill_stay_warn_data, 
                         eventType, eventId);
}

void uart_msg_nfc_verify(event_code_card_t event_code, uint8_t *card_id)
{
    uart_msg_common_send(UP_CMD_REPORT_ORDER, 
                         sizeof(frame_card_t), fill_nfc_verify_data, 
                         event_code, card_id);
}

void uart_msg_tamper_key_warn(uint8_t eventType)
{
    uart_msg_common_send(UP_CMD_REPORT_ORDER, 
                         sizeof(frame_tamper_t), fill_tamper_key_warn_data, 
                         eventType);
}

void uart_msg_face(uint8_t eventType, uint8_t *face_id, uint8_t face_id_size)
{
    uart_msg_common_send(UP_CMD_REPORT_ORDER,
                         sizeof(frame_face_t), fill_face_data,
                         eventType, face_id, face_id_size);
}

void uart_msg_heartbeat(void)
{
    uart_msg_common_send(UP_CMD_HEART,
                         sizeof(frame_heartbeat_t), fill_status_data);
}

void uart_msg_param_req(void)
{
    uart_msg_common_send(UP_CMD_PARAM_REQ,
                         sizeof(frame_heartbeat_t), fill_status_data);
}

void uart_msg_ack_word_mode(uint8_t status)
{
    uart_msg_common_send(UP_CMD_ACK_WORK_MODE,
                         sizeof(frame_ack_def_t), fill_ack_def_data, status);
}

void uart_msg_ack_light_ctl(uint8_t status)
{
    uart_msg_common_send(UP_CMD_ACK_LIGHT_CTL,
                         sizeof(frame_ack_def_t), fill_ack_def_data, status);
}

void uart_msg_ack_reset(uint8_t status)
{
    uart_msg_common_send(UP_CMD_ACK_RESET,
                         sizeof(frame_ack_def_t), fill_ack_def_data, status);
}

void uart_msg_ack_sleep(uint8_t status)
{
    uart_msg_common_send(UP_CMD_ACK_SLEEP,
                         sizeof(frame_ack_def_t), fill_ack_def_data, status);
}

void uart_msg_ack_ota_request(uint8_t status)
{
    uart_msg_common_send(UP_CMD_ACK_OTA_REQUEST,
                         sizeof(frame_ack_def_t), fill_ack_def_data, status);
}

void uart_msg_ack_param_report(uint8_t status)
{
    uart_msg_common_send(UP_CMD_ACK_REPORT_PARAM,
                         sizeof(frame_heartbeat_t), fill_ack_def_data, status);
}

void uart_msg_version(uint8_t status, uint8_t *data, uint8_t size)
{
    if (data == NULL || size != sizeof(frame_version_ack_def_t)) {
        OB_LOGE(TAG, "uart_msg_version param error");
        return;
    }
    uart_msg_common_send(UP_CMD_ACK_VERSION,
                         sizeof(frame_version_ack_def_t), fill_version_data,
                         status, data, size);
}
