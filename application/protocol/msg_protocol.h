/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#ifndef MSG_PROTOCOL_HH
#define MSG_PROTOCOL_HH

#include <stdint.h>
#include "uart_protocol.h"
#include "uart_packet.h"
#include "encrypt.h"
#include "config.h"
/***********Macro***********/

/***********Enum***********/
typedef enum{
    EVENT_KEY_ID_NONE    = 0x00,
    EVENT_KEY_RESET_ID   = 0x01,
    EVENT_KEY_SETTING_ID = 0x02,
}event_key_id_t;
typedef enum{
    EVENT_KEY_REPORT_NONE   = 0x00,   /* 无上报 */
    EVENT_KEY_REPORT_SINGLE = 0x01,   /* 单击 */
    EVENT_KEY_REPORT_DOUBLE = 0x02,   /* 双击 */
    EVENT_KEY_REPORT_LONG   = 0x03,   /* 长按 */
}event_key_report_t;


/***********Struct***********/
#pragma pack(1)
typedef struct
{
    uint8_t event_source;
    uint8_t event_code;
    uint16_t data;
} frame_work_mode_t;

typedef struct
{
    uint8_t event_param;
    uint8_t data;
} frame_report_param_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint16_t event_code;
    uint8_t event_finger;
} frame_light_ctl_t;

typedef struct
{
    uint8_t random[16]; // 随机数
} frame_encrypt_t;


typedef struct
{
    uint8_t status;
} frame_reset_t;

typedef struct
{
    uint8_t event;
    uint8_t data;
} frame_sleep_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t key_value;
} frame_key_broad_t;

typedef struct
{
    uint8_t data[12];
} frame_wake_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t finger_id;
} frame_finger_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t  card_id[4];
} frame_card_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t card_id[4];
    uint8_t read_sector[48];
} frame_read_card_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t face_id[2];
} frame_face_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t event_id;
} frame_radar_t;

typedef struct
{
    uint8_t event_type;
    uint8_t event_source;
    uint8_t event_code;
    uint8_t event_id;
} frame_tamper_t;

typedef struct
{
    uint8_t status;
} frame_version_t;

typedef struct
{
    uint8_t event;
    uint8_t data[16];
} frame_touch_sensitivity_t;

typedef struct
{
    uint8_t data;
} frame_pa_ctl_t;

typedef struct
{
    uint8_t status;
    uint8_t reserved1;
    uint8_t fw_funcode;
    uint8_t boot_ver : 4;
    uint8_t flash_ver : 4;
    uint8_t fw_ver;
} frame_version_ack_def_t;

typedef struct {
    uint8_t status;
} frame_heartbeat_t;

typedef struct {
    uint8_t status;
} frame_ack_def_t;

typedef struct {
    uint16_t checksum1;
    uint16_t magic;
    uint8_t file_type;
    uint8_t version[3];
    uint32_t size;
    uint32_t checksum2;
}frame_fmc_area_def_t;
typedef struct
{
    uint16_t frame_start;        // 帧起始标志
    uint8_t control_version : 7; // 版本号
    uint8_t encrypt : 1;         // 加密控制位
    uint8_t cmd;                 // 命令字
    uint16_t length;             // 数据长度
} frame_header_t;

typedef struct
{
    uint8_t TSN;   // 时间序列号
    uint8_t cmd;   // 命令字
    uint16_t length; // 数据长度
    union
    {
        frame_key_broad_t key_broad; // 按键帧结构体
        frame_finger_t finger;       // 指纹帧结构体
        frame_radar_t radar;         // 雷达帧结构体
        frame_card_t card;           // 卡片帧结构体
        frame_read_card_t read_card; // 读卡片扇区结构体
        frame_tamper_t tamper;       // 防撬帧结构体
        frame_face_t face;           // 人脸帧结构体
        frame_heartbeat_t heartbeat; // 心跳数据体
        frame_wake_t wake;           // 唤醒结构体

        frame_version_ack_def_t version;     // 版本号数据体
        frame_ack_def_t ack_def;
        // frame_encrypt_t encrypt;     // 加密回复结构体
        
        uint8_t status;
        uint8_t data[12];            // 数据域
    };
} frame_data_t;

#define FRAME_HEADER_LEN sizeof(frame_header_t)

typedef struct {
    frame_header_t header;
    uint8_t data[];
} frame_dl_t;

/**************************************************/
typedef struct
{
    uint8_t event_key_id;
    uint8_t event_key_report;
} frame_key_t;















/**************************************************/

#pragma pack()

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif // MSG_PROTOCOL_HH
