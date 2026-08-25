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
#include "hal_uart.h"
#include "system_timer.h"

void ob_log_init(void)
{
#if (OB_LOG_ENABLE == 1)
    ob_log_set_base_level(OB_LOG_LEVEL_DEBUG);

#if (OB_LOG_UART_SEL == OB_LOG_UART_SIMU)
    simu_uart_init();
#elif (OB_LOG_UART_SEL == OB_LOG_UART_1)
    hal_uart_config_t uart_cfg = {
        .tx_port = LOG_UART_TX_GPIO,
        .tx_pin = LOG_UART_TX_PIN,
        .rx_port = LOG_UART_RX_GPIO,
        .rx_pin = LOG_UART_RX_PIN,
        .baudrate = UART_BAUDRATE_1000000,
        .uart_group = LOG_UART_SEL,
        .callback = NULL
    };
    hal_uart_Init(&uart_cfg);
#endif

#endif
}

#if (OB_LOG_WITH_TIMESTAMP != 0)
const char *ob_log_get_timestamp(void)
{
    static char buf[12];

    snprintf(buf, sizeof(buf), "%lu", system_ms_get());

    return buf;
}
#endif // OB_LOG_WITH_TIMESTAMP

#if (OB_LOG_LEVEL_DEFAULT >= OB_LOG_LEVEL_FATAL)
void ob_log_fatal_hook(void)
{
    while (0)
    {
        /* code */
    }
    
}
#endif // OB_LOG_LEVEL_DEFAULT
