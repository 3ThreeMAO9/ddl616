/**
 * @file ota_uart.c
 * @author Xiangqian_Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-13
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "ota_uart.h"
#include "uart.h"
#include "gpio.h"
#include "ota_protocol.h"
#include "ota_helper.h"
#include "system_timer.h"
#include "ob_log.h"
#include "chip_config.h"
#include "utils.h"

static ota_uart_handle_t ota_uart_handle;

static void ota_uart_irq_callback(uint8_t value) {
    ota_uart_tick = OTA_UART_TIME_OUT;

    if (ota_uart_handle.lenth < OTA_UART_BUFFER_SIZE) {
        ota_uart_handle.buffer[ota_uart_handle.lenth++] = value;
    }
}

static uint8_t ota_uart_send_packet(uint8_t* packet, uint16_t lenth) {
    if (UART_Write(OB_UART0, packet, lenth)) {
        return 1;
    }

    return 0;
}

void ota_uart_init(void) {

    GPIO_SetPinMFType(OB_GPIO3, GPIO_PIN0, GPIO_MF_UART0_RX, GPIO_PINMODE_PULL_UP);
    GPIO_SetPinMFType(OB_GPIO3, GPIO_PIN1, GPIO_MF_UART0_TX, GPIO_PINMODE_PULL_UP);
    UART_Open(OB_UART0, 115200, ota_uart_irq_callback);

    ota_register_callback(ota_uart_send_packet);

    ota_uart_handle.lenth = 0;
    ota_uart_tick = 0;

    // ota_uart_test();
}

static void ota_request_handle(parsed_packet_t* parsed) {
    ota_response_ack_t ota_response_ack = {
        .status = OTA_RESPONSE_STATUS_READY,
        .scheme_code = {0, 0, 1},
        .version = {0, 0, 2},
        .flash_size = (FLASH_APP_MAX_SIZE),
        .pkt_size = 128,
        .ota_mode = OTA_MODE_SINGLE_BOOT,
    };

    if (ota_helper_prepare()) {
        ota_response_ack.status = OTA_RESPONSE_STATUS_READY;
    }
    else {
        ota_response_ack.status = OTA_RESPONSE_STATUS_SYSTEM_BUSY;
    }

    ota_request_packet_ack((void*)(&ota_response_ack));
}

static void ota_transfer_handle(parsed_packet_t* parsed) {
    ota_transfer_ack_t ota_transfer_ack;
    uint32_t addr;
    uint8_t* code_ptr = (uint8_t*)(parsed->data_content + sizeof(addr));

    memcpy((uint8_t*)(&addr), parsed->data_content, sizeof(addr));
    addr = BIG_LITTLE_SWAP32(addr);

    OB_LOGD("data_lenth: ");
    OB_LOGD_DUMP(&parsed->data_len, 2);
    OB_LOGD("data_content: ");
    OB_LOGD_DUMP(parsed->data_content, parsed->data_len);

    if (ota_helper_write(addr, code_ptr, (parsed->data_len - 4))) {
        ota_transfer_ack.status = OTA_TRANSFER_STATUS_SUCCESS;
    }
    else {
        ota_transfer_ack.status = OTA_TRANSFER_STATUS_ADDR_ERROR;
    }

    memcpy((uint8_t*)(&ota_transfer_ack.addr), addr, sizeof(ota_transfer_ack.addr));

    ota_transfer_packet_ack((void*)(&ota_transfer_ack));
}

void ota_uart_poll(void) {
    if (ota_uart_handle.lenth && (!ota_uart_tick)) {
        parsed_packet_t parsed = parse_data_packet(ota_uart_handle.buffer, ota_uart_handle.lenth);
        if (parsed.is_valid) {
            switch (parsed.command) {
                /*
                case PRIVATE_CMD_OTA_RESPONSE:
                    ota_request_handle(&parsed);
                    break;
                */

                case PRIVATE_CMD_OTA_TRANSFER:
                    ota_transfer_handle(&parsed);
                    break;

                case PRIVATE_CMD_OTA_CONTROL:
                    ota_control_packet_ack(OTA_CONTROL_STATUS_SUCCESS);
                    break;
                
                default:
                    break;
            }
        }

        ota_uart_handle.lenth = 0;
    }
}

void ota_uart_test(void) {
    
    ota_uart_handle.lenth = ota_request_pkt_test(ota_uart_handle.buffer, NULL, 0);
    ota_uart_poll();
    
    const uint8_t buffer1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    ota_uart_handle.lenth = ota_transfer_pkt_test(ota_uart_handle.buffer, buffer1, sizeof(buffer1));
    ota_uart_poll();
    const uint8_t buffer2[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x12, 0x13, 0x14};
    ota_uart_handle.lenth = ota_transfer_pkt_test(ota_uart_handle.buffer, buffer2, sizeof(buffer2));
    ota_uart_poll();
    const uint8_t buffer3[] = {0x00, 0x00, 0x00, 0x08, 0x11, 0x12, 0x13, 0x14};
    ota_uart_handle.lenth = ota_transfer_pkt_test(ota_uart_handle.buffer, buffer3, sizeof(buffer3));
    ota_uart_poll();
    
    // ota_uart_handle.lenth = ota_control_pkt_test(ota_uart_handle.buffer);
    // ota_uart_poll();
}
