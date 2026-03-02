/**
 * @file ob_log_port.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#include "ob_log.h"
#include "ob_log_config.h"
#include "OB90A64M1.h"
#include "uart.h"
#include "gpio.h"
#include <string.h>

void ob_log_init(void)
{
    GPIO_SetPinMFType(OB_GPIO2, GPIO_PIN11, GPIO_MF_UART1_RX, GPIO_PINMODE_PULL_UP);
    GPIO_SetPinMFType(OB_GPIO0, GPIO_PIN3, GPIO_MF_UART1_TX, GPIO_PINMODE_PULL_UP);
    UART_Open(OB_UART1, 115200, NULL);
}

void ob_log_write(const char *str, uint32_t lenth)
{
    UART_Write(OB_UART1, (uint8_t*)str, lenth);
}

void ob_log_write_hex(const uint8_t* input, uint16_t lenth) {
    uint8_t buffer[3];
    for (uint16_t i = 0; i < lenth; i++) {

        buffer[0] = ((input[i] >> 4) & 0x0F);
        buffer[0] += ((buffer[0] >= 0x0A) ? 0x37 : 0x30);
        buffer[1] = (input[i] & 0x0F);
        buffer[1] += ((buffer[1] >= 0x0A) ? 0x37 : 0x30);
        buffer[2] = ' ';

        UART_Write(OB_UART1, (uint8_t*)buffer, 3);
    }
}
