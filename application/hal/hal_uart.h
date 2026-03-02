#ifndef HAL_UART_HH
#define HAL_UART_HH

#include <stdint.h>
#include "string.h"
#include "uart.h"
#include "simu_uart.h"
#include "soft_uart.h"
#include "soft_uart_log.h"

/*************************Macro*************************/
// uart baudrate
#define UART_BAUDRATE_9600      (9600)
#define UART_BAUDRATE_14400     (14400)
#define UART_BAUDRATE_19200     (19200)
#define UART_BAUDRATE_38400     (38400)
#define UART_BAUDRATE_57600     (57600)
#define UART_BAUDRATE_115200    (115200)
#define UART_BAUDRATE_512000    (512000)
#define UART_BAUDRATE_921600    (921600)
#define UART_BAUDRATE_1000000   (1000000)
#define UART_BAUDRATE_2000000   (2000000)

// Buf len
#define UART0_BUF_LEN   (512)
#define UART1_BUF_LEN   (64)
#define UART2_BUF_LEN   (64)

// Time out
#define UART_TIME_OUT   (8)     // unit:ms

/*************************Enum*************************/
typedef enum {
    UART_0 = 0,
    UART_1,
    UART_SOFT_0,
    UART_GROUP_CNT,
}Uart_Group_t;

/*************************Struct*************************/
#pragma pack(1)
typedef struct
{
    HalGpioPort tx_port; 
    HalGpioPin tx_pin;
    HalGpioPort rx_port;
    HalGpioPin rx_pin;
    uint32_t baudrate;
    Uart_Group_t uart_group;
    uart_callback_t callback;
}hal_uart_config_t;

typedef struct
{
    HalGpioPort tx_port;
    HalGpioPin tx_pin;
    HalGpioPort rx_port;
    HalGpioPin rx_pin;
    Uart_Group_t uart_group;
    HalGpioMode mode;
    uint8_t level;
} hal_uart_sleep_config_t;

#pragma pack()

/*************************Variable*************************/

/*************************Function*************************/
void hal_uart_Init(hal_uart_config_t *uart_cfg);
void hal_uart_sotp(Uart_Group_t uart_group);
void hal_uart_sleep(hal_uart_sleep_config_t *uart_cfg);
void hal_uartSendBuff(Uart_Group_t uart_group, uint8_t *data, uint16_t len);
uint8_t hal_uart_receive_deal(Uart_Group_t uart_group, uint8_t *buf);
void hal_uart_isr(Uart_Group_t uart_group);
/**********************************************************/
#endif

