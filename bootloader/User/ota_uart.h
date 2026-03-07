/**
 * @file log_uart.h
 * @author Xiangqian Ren (Xiangqian_Ren@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-13
 * 
 * @copyright Copyright (c) 2024 广州昂宝电子有限公司
 * 
 */

#ifndef OTA_UART_H
#define OTA_UART_H

#include <stdint.h>

#define OTA_UART_TIME_OUT                   (5)         // unit: ms
#define OTA_UART_BUFFER_SIZE                (2 * 1024)

typedef struct {
    uint8_t time_out;
    uint8_t buffer[OTA_UART_BUFFER_SIZE];
    uint16_t lenth;
    
} ota_uart_handle_t;


void ota_uart_init(void);
void ota_uart_poll(void);
void ota_request_ack_handle(void);

#endif // UART_H
