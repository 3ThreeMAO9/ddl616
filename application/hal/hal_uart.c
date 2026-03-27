#include "hal_uart.h"
#include "hal_gpio.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "hal_uart"

/*************************Variable*************************/
static volatile uint8_t uart0_buf[UART0_BUF_LEN];
static volatile uint8_t uart1_buf[UART1_BUF_LEN];
static volatile uint8_t uart2_buf[UART2_BUF_LEN];

static volatile uint16_t uart_len[UART_GROUP_CNT];
static volatile uint32_t uart_timeOut[UART_GROUP_CNT];

static const uint16_t uart_len_max[UART_GROUP_CNT] = {UART0_BUF_LEN, UART1_BUF_LEN, UART2_BUF_LEN};
static volatile uint8_t *pt[UART_GROUP_CNT] = {uart0_buf, uart1_buf, uart2_buf};

/************************* UART 通用复用管理 *************************/
static uart_owner_t g_uart_owner = UART_OWNER_NONE;

// 切换 UART 为指定设备（自动配置 IO + 波特率 + 回调）
void hal_uart_switch(uart_owner_t owner, hal_uart_config_t *cfg)
{
    if (cfg == NULL) {
        g_uart_owner = UART_OWNER_NONE;
        return;
    }

    if (g_uart_owner == owner) {
        return;
    }

    Uart_Group_t uart_group = cfg->uart_group;
  
    if (owner == UART_OWNER_DEV1){
        OB_GPIO0->MODE_b.MODEPIN3 = GPIO_PINMODE_PULL_UP;
        OB_GPIO2->MODE_b.MODEPIN11 = GPIO_PINMODE_PULL_UP;
        OB_GPIO0->DATA |= (GPIO_PIN3);
        OB_GPIO2->DATA |= (GPIO_PIN11);
        OB_GPIO0->MF0_b.PORT_3 = GPIO_MF_TYPE_GPIO;
        OB_GPIO2->MF1_b.PORT_11 = GPIO_MF_TYPE_GPIO; 
    }
    else if (owner == UART_OWNER_DEV2){
        OB_GPIO2->MODE_b.MODEPIN1 = GPIO_PINMODE_PULL_UP;
        OB_GPIO2->MODE_b.MODEPIN0 = GPIO_PINMODE_PULL_UP;
        OB_GPIO2->DATA |= (GPIO_PIN1);
        OB_GPIO2->DATA |= (GPIO_PIN0);
        OB_GPIO2->MF0_b.PORT_1 = GPIO_MF_TYPE_GPIO;
        OB_GPIO2->MF0_b.PORT_0 = GPIO_MF_TYPE_GPIO; 
    }

    hal_uart_sotp(uart_group);
    hal_uart_Init(cfg);

    g_uart_owner = owner;
}

// 获取当前 UART 归属
uart_owner_t hal_uart_get_owner(void)
{
    return g_uart_owner;
}

void hal_uart_receive_buff_callback(Uart_Group_t uart_group, uint8_t data)
{
    uart_timeOut[uart_group] = system_inc_time_cnt(UART_TIME_OUT);
    if (uart_len[uart_group] < uart_len_max[uart_group])
    {
        pt[uart_group][uart_len[uart_group]] = data;
        uart_len[uart_group]++;
    }
}

static uint8_t hal_uart_read_receive_state(Uart_Group_t uart_group)
{
    if (uart_group < UART_GROUP_CNT)
    {
        if ((uart_len[uart_group]) && (true == system_out_time_cnt(uart_timeOut[uart_group])))
        {
            return true;
        }
    }
    return false;
}

uint8_t hal_uart_receive_deal(Uart_Group_t uart_group, uint8_t *buf)
{
    uint16_t len;
    if (true == hal_uart_read_receive_state(uart_group))
    {
        memcpy((uint8_t *)buf, (uint8_t *)pt[uart_group], uart_len[uart_group]);
        len = uart_len[uart_group];
        uart_len[uart_group] = 0;
    }
    else
    {
        len = 0;
    }
    return len;
}

void hal_uartSendBuff(Uart_Group_t uart_group, uint8_t *data, uint16_t len)
{
    uart_len[uart_group] = 0;
    if (UART_0 == uart_group)
    {
        UART_Write(OB_UART0, data, len);
    }
    else if (UART_1 == uart_group)
    {
        UART_Write(OB_UART1, data, len);
    }
    else if (UART_SOFT_0 == uart_group)
    {
        SoftUART_Write(NULL,data, len);
    }
    else
    {
        ;
    }
}

static void hw_uart_io_init(hal_uart_config_t *cfg)
{
    OB_GPIO_Type *ob_tx_port = hal_convert_ob_gpio_port(cfg->tx_port);
    OB_GPIO_Type *ob_rx_port = hal_convert_ob_gpio_port(cfg->rx_port);
    uint32_t ob_tx_pin = hal_convert_ob_gpio_pin(cfg->tx_pin);
    uint32_t ob_rx_pin = hal_convert_ob_gpio_pin(cfg->rx_pin);

    ob_tx_port->DATA |= ob_tx_pin;
    ob_rx_port->DATA |= ob_rx_pin;
    uint8_t tx_mf = (cfg->uart_group == UART_0) ? GPIO_MF_UART0_TX : GPIO_MF_UART1_TX;
    uint8_t rx_mf = (cfg->uart_group == UART_0) ? GPIO_MF_UART0_RX : GPIO_MF_UART1_RX;
    GPIO_SetPinMFType(ob_tx_port, ob_tx_pin, tx_mf, GPIO_PINMODE_PUSH_PULL);
    GPIO_SetPinMFType(ob_rx_port, ob_rx_pin, rx_mf, GPIO_PINMODE_PULL_UP);

    delay_ms(10);
    OB_UART_Type *uart_dev = (cfg->uart_group == UART_0) ? OB_UART0 : OB_UART1;
    IRQn_Type irq_num = (cfg->uart_group == UART_0) ? UART0_IRQn : UART1_IRQn;
    UART_Open(uart_dev, cfg->baudrate, cfg->callback);
    UART_EnableInt(uart_dev, UART_INT_RBR);
    NVIC_SetPriority(irq_num, 0);
    NVIC_EnableIRQ(irq_num);
}

void hal_uart_Init(hal_uart_config_t *uart_cfg)
{
    if (uart_cfg == NULL)
        return;

    Uart_Group_t uart_group = uart_cfg->uart_group;

    if (uart_group == UART_0 || uart_group == UART_1)
    {
        hw_uart_io_init(uart_cfg); // 硬件UART走通用逻辑
    }
    else if (uart_group == UART_SOFT_0)
    {
        soft_uart2_init(uart_cfg->baudrate, uart_cfg->callback); 
    }
    else
    {
        return;
    }
    uart_len[uart_group] = 0;
}

void hal_uart_sotp(Uart_Group_t uart_group)
{
    if (uart_group == UART_0)
    {
        UART_DisableInt(OB_UART0, UART_INT_RBR);
        NVIC_DisableIRQ(UART0_IRQn);
    }
    else if (uart_group == UART_1)
    {
        UART_DisableInt(OB_UART1, UART_INT_RBR);
        NVIC_DisableIRQ(UART1_IRQn);
    }
    else
    {
        return;
    }
}

void hal_uart_gpio_sleep(hal_uart_sleep_config_t *cfg)
{
    if (cfg == NULL)
        return;
    HAL_GPIO_Write(cfg->tx_port, cfg->tx_pin, cfg->level);
    HAL_GPIO_Init(cfg->tx_port, cfg->tx_pin, cfg->mode, HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(cfg->rx_port, cfg->rx_pin, cfg->level);
    HAL_GPIO_Init(cfg->rx_port, cfg->rx_pin, cfg->mode, HAL_GPIO_PULL_NONE); 
}

static void hw_uart_sleep(hal_uart_sleep_config_t *uart_cfg)
{
    hal_uart_gpio_sleep(uart_cfg);

    hal_uart_sotp(uart_cfg->uart_group);
}

void hal_uart_sleep(hal_uart_sleep_config_t *uart_cfg)
{
    if (uart_cfg == NULL)
        return;
    Uart_Group_t uart_group = uart_cfg->uart_group;

    if (uart_group == UART_0 || uart_group == UART_1)
    {
        hw_uart_sleep(uart_cfg); // 硬件UART走通用逻辑
    }
    else if (uart_group == UART_SOFT_0)
    {
        // 软件串口休眠：提取结构体中的mode/level参数
        soft_uart2_sleep(uart_cfg->mode, uart_cfg->level);
    }
    else
        return;
}

void hal_uart_isr(Uart_Group_t uart_group)
{
    if (UART_0 == uart_group)
    {
        ;
    }
    else if (UART_1 == uart_group)
    {
        ;
    }
    else if (UART_SOFT_0 == uart_group)
    {
        ;
    }
    else
    {
        ;
    }  
}

