/**
 * @file ota_protocol.h
 * @author Xiangqian_Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-14
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#ifndef OTA_PROTOCOL_H
#define OTA_PROTOCOL_H

#include <stdint.h>

typedef uint8_t (*ota_send_packet_callback_t)(uint8_t*, uint16_t);

/*****************Macro****************/
// 帧结构常量定义
#define FRAME_HEADER_1                          0x55
#define FRAME_HEADER_2                          0xAA
#define CMD_PASSTHROUGH                         0xFB
#define MIN_FRAME_SIZE                          8  // 最小帧长度：帧头2 + Control1 + Cmd1 + Len2 + CRC2

// CRC-16/CCITT 参数
#define CRC_CCITT_POLY                          0x1021
#define CRC_CCITT_INIT                          0xFFFF

// 加密标志位
#define ENCRYPT_FLAG                            0x80
#define VERSION_MASK                            0x0F

// 私有数据头大小
#define PRIVATE_HEADER_SIZE                     4  // TSN1 + 命令字1 + 长度2

#define PRIVATE_CMD_ACK_FLAG                    0x80

#define PRIVATE_CMD_OTA_RESPONSE                0x70
#define PRIVATE_CMD_OTA_TRANSFER                0x71
#define PRIVATE_CMD_OTA_CONTROL                 0x72

#define OTA_FRAME_BUFFER_SIZE                   (256)

/*****************Enum*****************/
typedef enum {
    OTA_RESPONSE_STATUS_READY = 0,              // 准备就绪
    OTA_RESPONSE_STATUS_STORAGE_INSUFFICIENT,   // 存储空间不足
    OTA_RESPONSE_STATUS_HW_INCOMPATIBLE,        // 硬件不兼容
    OTA_RESPONSE_STATUS_SYSTEM_BUSY,            // 系统忙
    OTA_RESPONSE_STATUS_UPDATING,               // 升级中
    OTA_RESPONSE_STATUS_FIRMWARE_ERROR,         // APP固件异常， 请求恢复

} ota_response_status_e;

typedef enum {
    OTA_TRANSFER_STATUS_SUCCESS = 0,            // 接收成功
    OTA_TRANSFER_STATUS_CRC_ERROR,              // CRC校验错误
    OTA_TRANSFER_STATUS_PKT_LOSE,               // 数据包丢失
    OTA_TRANSFER_STATUS_ADDR_ERROR,             // 地址错误

} ota_transfer_status_e;

typedef enum {
    OTA_CONTROL_STATUS_SUCCESS = 0,             // 成功
    OTA_CONTROL_STATUS_FAIL,                    // 失败

} ota_control_status_e;

typedef enum {
    OTA_MODE_SINGLE_BOOT = 0,           // 单区升级+Boot(直接覆盖代码区)
    OTA_MODE_SINGLE_CACHE,              // 单区升级+Boot(有缓存Flash)
    OTA_MODE_DUAL,                      // 双区升级(A/B区切换)

} ota_mode_e;

/****************Struct****************/
#pragma pack(1)
typedef union {
    uint8_t value;

    struct {
        uint8_t version : 4;
        uint8_t reserverd : 3;
        uint8_t encrypted : 1;
    } fields;

}config_byte_t;

typedef struct {
    uint8_t is_valid : 1;
    uint8_t is_encrypted : 1;
    uint8_t version;
    
    uint8_t tsn;
    uint8_t command;
    uint16_t data_len;
    const uint8_t *data_content;
    
} parsed_packet_t;

typedef struct {
    uint8_t scheme_code[3];
    uint8_t version[3];
    uint32_t code_size;
    uint8_t code_crc;

} ota_request_t;

typedef struct {
    uint32_t addr;
    uint8_t* code_ptr;
    
} ota_transfer_t;

typedef struct {
    uint8_t status;
    uint8_t scheme_code[3];
    uint8_t version[3];

    uint32_t flash_size;
    uint32_t pkt_size;
    uint8_t ota_mode;

} ota_response_ack_t;

typedef struct {
    uint8_t status;
    uint32_t addr;

} ota_transfer_ack_t;

#pragma pack()

/***************Variable***************/


/***************Function***************/
void ota_register_callback(ota_send_packet_callback_t callback);
parsed_packet_t parse_data_packet(const uint8_t *data, uint16_t data_len);
uint8_t ota_packet_ack(uint8_t cmd, void* data_content, uint16_t lenth);

uint8_t ota_request_packet_ack(ota_response_ack_t* ota_response_ack);
uint8_t ota_transfer_packet_ack(ota_transfer_ack_t* ota_transfer_ack);
uint8_t ota_control_packet_ack(uint8_t status);

uint16_t ota_request_pkt_test(uint8_t* packet, uint8_t* data_content, uint16_t lenth);
uint16_t ota_transfer_pkt_test(uint8_t* packet, uint8_t* data_content, uint16_t lenth);
uint16_t ota_control_pkt_test(uint8_t* packet, uint8_t* data_content, uint16_t lenth);

/**************************************/

#endif // UART_H
