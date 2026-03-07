/**
 * @file ota_protocol.c
 * @author Xiangqian_Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-14
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "ota_protocol.h"
#include "utils.h"
#include <string.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "ota_protocol"

////////////////////////////////////////////////////////
static ota_send_packet_callback_t ota_send_packet_callback;
static parsed_packet_t parsed_packet;

////////////////////////////////////////////////////////

void ota_register_callback(ota_send_packet_callback_t callback) {
    ota_send_packet_callback = callback;
    memset((uint8_t*)(&parsed_packet), 0, sizeof(parsed_packet_t));
}

/**
 * 检查帧头合法性
 */
static uint8_t check_frame_header(const uint8_t *frame) {
    return (frame[0] == FRAME_HEADER_1 && frame[1] == FRAME_HEADER_2);
}

/**
 * 检查命令字合法性
 */
static uint8_t check_command(const uint8_t *frame) {
    return (frame[3] == CMD_PASSTHROUGH);  // frame[2]是Control，frame[3]是Cmd
}

/**
 * 检查数据包长度合法性
 */
static uint8_t check_length(const uint8_t *frame, uint16_t frame_len) {
    // 获取数据长度字段（大端模式）
    uint16_t payload_len = ((uint16_t)frame[4] << 8) | frame[5];
    
    // 计算期望的总帧长度
    // 固定部分：帧头2 + Control1 + Cmd1 + Len2 + CRC2 = 8字节
    uint16_t expected_len = MIN_FRAME_SIZE + payload_len;
    
    // Payload长度必须是16的整数倍（根据你的描述是N*16）
    if (payload_len % 16 != 0) {
        return false;
    }
    
    // 检查实际接收到的长度是否匹配
    return (frame_len >= expected_len);
}

/**
 * 检查CRC校验码
 */
static uint8_t check_crc(const uint8_t *frame, uint16_t frame_len) {
    if (frame_len < MIN_FRAME_SIZE) {
        return false;
    }
    // 计算CRC的数据范围：从帧头到Payload结束
    uint16_t payload_len = ((uint16_t)frame[4] << 8) | frame[5];
    uint16_t crc_data_len = 6 + payload_len;  // Control+Cmd+Len+Payload
    // 计算CRC
    uint16_t calculated_crc = crc16_ccitt(&frame[0], crc_data_len);
    
    // 获取帧中的CRC值（大端模式）
    uint16_t frame_crc = ((uint16_t)frame[frame_len - 2] << 8) | frame[frame_len - 1];
    return (calculated_crc == frame_crc);
}

/**
 * 检查私有数据头合法性
 * @param payload: 指向Payload开始位置的指针
 * @param payload_len: Payload总长度
 */
static uint8_t check_private_header(const uint8_t *payload, uint16_t payload_len) {
    if (payload_len < PRIVATE_HEADER_SIZE) {
        return false;
    }
    
    // 检查TSN不为0
    if (payload[0] == 0) {  // TSN字段
        return false;
    }
    
    // 获取数据内容长度（大端模式）
    uint16_t data_content_len = ((uint16_t)payload[2] << 8) | payload[3];
    
    // 检查数据内容长度是否合理
    // 私有数据总长度 = TSN1 + 命令字1 + 长度2 + 数据内容N
    uint16_t expected_private_len = PRIVATE_HEADER_SIZE + data_content_len;
    
    // 检查是否超出Payload边界
    if (expected_private_len > payload_len) {
        return false;
    }
    
    return true;
}

/**
 * 检查协议版本（如果需要）
 */
static uint8_t get_protocol_version(const uint8_t *frame) {
    return frame[2] & VERSION_MASK;  // Control字段的低4位
}

/**
 * 检查是否启用加密
 */
static uint8_t is_encrypted(const uint8_t *frame) {
    return (frame[2] & ENCRYPT_FLAG) != 0;
}

/**
 * 主函数：检查整个数据包的合法性
 * @param data: 完整数据包指针
 * @param data_len: 数据包总长度
 * @return: true-数据包合法，false-数据包非法
 */
uint8_t validate_data_packet(const uint8_t *data, uint16_t data_len) {
    // 1. 检查最小长度
    if (data_len < MIN_FRAME_SIZE) {
        OB_LOGD("fail: data len");

        return false;
    }
    
    // 2. 检查帧头
    if (!check_frame_header(data)) {

        return false;
    }
    
    // 3. 检查命令字
    if (!check_command(data)) {

        return false;
    }
    
    // 4. 检查数据长度
    if (!check_length(data, data_len)) {
        return false;
    }
    
    // 5. 检查CRC校验
    if (!check_crc(data, data_len)) {
        return false;
    }
    
    // 6. 检查私有数据头（如果Payload有数据）
    uint16_t payload_len = ((uint16_t)data[4] << 8) | data[5];

    if (payload_len > 0) {
        const uint8_t *payload = &data[6];  // Payload起始位置
        
        // 注意：如果加密使能，这里不能直接解析Payload内容
        // 需要先解密才能检查私有数据头
        if (is_encrypted(data)) {
            // 加密模式下，这里可以添加解密后的校验
            // 需要实现SM4解密函数
        }

        if (!check_private_header(payload, payload_len)) {

            return false;
        }
    }
    
    return true;
}

parsed_packet_t parse_data_packet(const uint8_t *data, uint16_t data_len) {
    
    // 先验证数据包
    if (!validate_data_packet(data, data_len)) {
        parsed_packet.is_valid = false;
        return parsed_packet;
    }
    
    parsed_packet.is_valid = true;
    parsed_packet.is_encrypted = is_encrypted(data);
    parsed_packet.version = get_protocol_version(data);
    
    // 获取Payload信息（如果是加密的，这里需要先解密）
    if (parsed_packet.is_encrypted) {
        
    }

    uint16_t payload_len = ((uint16_t)data[4] << 8) | data[5];
    
    if (payload_len >= PRIVATE_HEADER_SIZE) {
        const uint8_t *payload = &data[6];
        
        // OB_LOGD("payload is valid");
    
        parsed_packet.tsn = payload[0];
        parsed_packet.command = payload[1];
        parsed_packet.data_len = ((uint16_t)payload[2] << 8) | payload[3];
        
        if ((PRIVATE_HEADER_SIZE + parsed_packet.data_len) <= payload_len) {
            parsed_packet.data_content = &payload[PRIVATE_HEADER_SIZE];
        }
    }
    
    return (parsed_packet);
}

static uint8_t ota_protocol_packet_ack(void* buffer, uint16_t lenth) {
    uint16_t index = 0;
    uint8_t send_buffer[OTA_FRAME_BUFFER_SIZE];

    memset(send_buffer, 0 , OTA_FRAME_BUFFER_SIZE);
    send_buffer[index++] = FRAME_HEADER_1;
    send_buffer[index++] = FRAME_HEADER_2;
    
    config_byte_t config_byte;

    config_byte.fields.encrypted = parsed_packet.is_encrypted;
    config_byte.fields.reserverd = 0;
    config_byte.fields.version = parsed_packet.version;
    send_buffer[index++] = config_byte.value;

    send_buffer[index++] = CMD_PASSTHROUGH;

    uint16_t payload_lenth = (lenth / 16 * 16);
    payload_lenth += (lenth % 16) ? 16 : 0;
    send_buffer[index++] = ((payload_lenth >> 8) & 0xFF);
    send_buffer[index++] = (payload_lenth & 0xFF);

    memcpy(&send_buffer[index], buffer, lenth);
    index += payload_lenth;

    uint16_t crc_data_len = 6 + payload_lenth;  // Control+Cmd+Len+Payload
    uint16_t crc = crc16_ccitt(&send_buffer[0], crc_data_len);
    send_buffer[index++] = (crc >> 8) & 0xFF;
    send_buffer[index++] = crc & 0xFF;

    if (ota_send_packet_callback) {
        // OB_LOGD("send packet: ");
        // OB_LOGD_DUMP(send_buffer, index);

        return ota_send_packet_callback(send_buffer, index);
    }

    return false;
}

uint8_t ota_packet(uint8_t cmd, void* data_content, uint16_t length, uint8_t with_ack_flag) {
    uint8_t buffer[OTA_FRAME_BUFFER_SIZE - 8];
    uint16_t index = 0;

    buffer[index++] = parsed_packet.tsn;
    // 根据参数决定是否拼接ACK标识
    buffer[index++] = with_ack_flag ? (cmd | PRIVATE_CMD_ACK_FLAG) : cmd;
    // 数据长度大端存储
    buffer[index++] = ((length >> 8) & 0xFF);
    buffer[index++] = (length & 0xFF);
    // 拷贝数据内容（增加空指针校验，避免崩溃）
    if (data_content != NULL && length > 0) {
        memcpy(&buffer[index], (uint8_t*)data_content, length);
    }
    index += length;

    return ota_protocol_packet_ack(buffer, (length + 4));
}

uint8_t ota_packet_send(uint8_t cmd, void* data_content, uint16_t length) {
    return ota_packet(cmd, data_content, length, 0);
}

uint8_t ota_packet_ack(uint8_t cmd, void* data_content, uint16_t length) {
    return ota_packet(cmd, data_content, length, 1);
}

uint8_t ota_request_packet_ack(ota_response_ack_t* ota_response_ack) {

    ota_response_ack->flash_size = BIG_LITTLE_SWAP32(ota_response_ack->flash_size);
    ota_response_ack->pkt_size = BIG_LITTLE_SWAP32(ota_response_ack->pkt_size);

    ota_packet_ack(PRIVATE_CMD_OTA_RESPONSE, (void*)(ota_response_ack), sizeof(ota_response_ack_t));
}

uint8_t ota_transfer_packet_ack(ota_transfer_ack_t* ota_transfer_ack) {
    ota_packet_ack(PRIVATE_CMD_OTA_TRANSFER, (void*)(ota_transfer_ack), sizeof(ota_transfer_ack_t));
}

uint8_t ota_control_packet_ack(uint8_t status) {
    
    ota_packet_ack(PRIVATE_CMD_OTA_CONTROL, (void*)(&status), sizeof(status));
}

uint8_t ota_back2front_packet_ack(uint8_t status) {
    
    ota_packet_ack(PRIVATE_CMD_ACK_OTA_BACK2FRONT, (void*)(&status), sizeof(status));
}

void ota_control_request(void)
{
    uint8_t status = 0xff;
    ota_packet_send(PRIVATE_CMD_OTA_FRONT2BACK, (void *)(&status), sizeof(status));
}

uint16_t ota_request_pkt_test(uint8_t* packet, uint8_t* data_content, uint16_t lenth) {
    // 示例：创建一个测试数据包
    uint8_t test_packet[] = {
        // 帧头
        0x55, 0xAA,
        // Control: 版本1，未加密
        0x01,
        // Cmd: 透传指令
        0xFB,
        // Len: Payload长度6字节（大端）
        0x00, 0x20,
        // Payload: 私有数据
        0x01,       // TSN: 1
        0x70,       // 命令字: 0x70
        0x00, (6 + 8), // 数据长度: 2字节
        0x00, 0x00, 0x01, 0x01, 0x02, 0x03,
        0x00, 0x00, 0x00, 0x00, // 固件总字节数（这里需要计算正确文件的长度）
        0x00, 0x00, 0x00, 0x00, // 固件整体CRC32校验（这里需要计算正确文件的CRC）
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00  // CRC（这里需要计算正确的CRC值）
    };
    
    test_packet[6] = (++parsed_packet.tsn);
    test_packet[9] = lenth;
    memcpy(&test_packet[10], data_content, lenth);

    // 计算并设置正确的CRC
    uint16_t payload_len = (uint16_t)test_packet[5] + ((uint16_t)test_packet[4] << 8);
    uint16_t crc_data_len = 4 + payload_len;  // Control+Cmd+Len+Payload
    uint16_t crc = crc16_ccitt(&test_packet[0], crc_data_len);
    test_packet[sizeof(test_packet) - 2] = (crc >> 8) & 0xFF;
    test_packet[sizeof(test_packet) - 1] = crc & 0xFF;

    OB_LOGD("test pkt lenth: ");
    OB_LOGD_DUMP(&payload_len, 2);
    OB_LOGD("test pkt: ");
    OB_LOGD_DUMP(test_packet, sizeof(test_packet));
    
    memcpy(packet, test_packet, sizeof(test_packet));
    return sizeof(test_packet);
}

uint16_t ota_transfer_pkt_test(uint8_t* packet, uint8_t* data_content, uint16_t lenth) {
    // 示例：创建一个测试数据包
    uint8_t test_packet[] = {
        // 帧头
        0x55, 0xAA,
        // Control: 版本1，未加密
        0x01,
        // Cmd: 透传指令
        0xFB,
        // Len: Payload长度6字节（大端）
        0x00, 0x10,
        // Payload: 私有数据
        0x01,       // TSN: 1
        0x71,       // 命令字: 0x71
        0x00, (4 + 8), // 数据长度: 2字节
        0x00, 0x00, 0x00, 0x00,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        // CRC（这里需要计算正确的CRC值）
        0x00, 0x00
    };
    
    test_packet[6] = (++parsed_packet.tsn);
    test_packet[9] = lenth;
    memcpy(&test_packet[10], data_content, lenth);

    // 计算并设置正确的CRC
    uint16_t payload_len = (uint16_t)test_packet[5] + ((uint16_t)test_packet[4] << 8);
    uint16_t crc_data_len = 4 + payload_len;  // Control+Cmd+Len+Payload
    uint16_t crc = crc16_ccitt(&test_packet[0], crc_data_len);
    test_packet[sizeof(test_packet) - 2] = (crc >> 8) & 0xFF;
    test_packet[sizeof(test_packet) - 1] = crc & 0xFF;

    OB_LOGD("test pkt: ");
    OB_LOGD_DUMP(test_packet, sizeof(test_packet));

    memcpy(packet, test_packet, sizeof(test_packet));
    return sizeof(test_packet);
}

uint16_t ota_control_pkt_test(uint8_t* packet, uint8_t* data_content, uint16_t lenth) {
    // 示例：创建一个测试数据包
    uint8_t test_packet[] = {
        // 帧头
        0x55, 0xAA,
        // Control: 版本1，未加密
        0x01,
        // Cmd: 透传指令
        0xFB,
        // Len: Payload长度6字节（大端）
        0x00, 0x10,
        // Payload: 私有数据
        0x01,       // TSN: 1
        0x72,       // 命令字: 0x71
        0x00, (1), // 数据长度: 2字节
        0x03,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // CRC（这里需要计算正确的CRC值）
        0x00, 0x00
    };
    
    test_packet[6] = (++parsed_packet.tsn);
    test_packet[9] = lenth;
    memcpy(&test_packet[10], data_content, lenth);

    // 计算并设置正确的CRC
    uint16_t payload_len = (uint16_t)test_packet[5] + ((uint16_t)test_packet[4] << 8);
    uint16_t crc_data_len = 4 + payload_len;  // Control+Cmd+Len+Payload
    uint16_t crc = crc16_ccitt(&test_packet[0], crc_data_len);
    test_packet[sizeof(test_packet) - 2] = (crc >> 8) & 0xFF;
    test_packet[sizeof(test_packet) - 1] = crc & 0xFF;

    OB_LOGD("test pkt lenth: ");
    OB_LOGD_DUMP(&payload_len, 2);

    OB_LOGD("test pkt: ");
    OB_LOGD_DUMP(test_packet, sizeof(test_packet));

    memcpy(packet, test_packet, sizeof(test_packet));
    return sizeof(test_packet);
}
