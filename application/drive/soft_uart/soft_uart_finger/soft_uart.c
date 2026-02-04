/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: soft_uart.c
 * Desc:
 * Version: 1.0.1
 * Revision: James_Zhang
 * Date: 2025-12-13
 */

#include "soft_uart_cfg.h"
#include <stdarg.h>
#include <string.h>
#include "soft_uart.h"
#include "ringbuffer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "soft_uart"

#if SOFT_UART_ENABLE
/************************ 全局变量 ****************************/
#if SOFT_UART_TX_ENABLE
SoftUART_TxStruct_t tx_struct = {0};
volatile uint8_t softuart_txbuf[SOFT_UART_TX_BUF_LEN];   // 全局发送缓冲区
volatile uint16_t softuart_tx_wr = 0;                    // 写指针
volatile uint16_t softuart_tx_rd = 0;                    // 读指针

#endif

#if SOFT_UART_RX_ENABLE
SoftUART_RxStruct_t rx_struct = {0};
#endif

/************************ 私有函数声明 ************************/

static void SoftUART_Tim16_Init(uint32_t period_us);

/************************ 接收缓冲区操作 ************************/
#if SOFT_UART_RX_ENABLE
static uart_callback_t g_soft_uart2_callback = NULL;

// 写数据到接收缓冲区（非阻塞）
void SoftUART_PutRxBuf(uint8_t data)
{
    g_soft_uart2_callback(data); // 转发接收到的字节到上层
}

#endif

void SoftUart_Timer16(OB_CT16B_Type *pTimer16, uint32_t nMRSel, uint32_t nMode, uint32_t nUsec)
{
    uint32_t nPerClock, nClock;

    if (pTimer16 == OB_CT16B0)
        nPerClock = GetPeripheralClock(APB_T16B0);
    else
        nPerClock = GetPeripheralClock(APB_T16B1);

    nPerClock /= 10000;
    nClock = (nPerClock * nUsec + 5000) / 10000;

    pTimer16->TMR16PC = 0x00;

    if (nMRSel == TIMER16_MR0)
        pTimer16->TMR16MR0 = nClock;
    else if (nMRSel == TIMER16_MR1)
        pTimer16->TMR16MR1 = nClock;
    else if (nMRSel == TIMER16_MR2)
        pTimer16->TMR16MR2 = nClock;
    else if (nMRSel == TIMER16_MR3)
        pTimer16->TMR16MR3 = nClock;

    TIMER16_EnableInterrupt(pTimer16, nMRSel, nMode);
    pTimer16->TMR16TCR = pTimer16->TMR16TCR | TIMER16_RESET;
}

/************************ Time16初始化（适配TX/RX） ************************/
static void SoftUART_Tim16_Init(uint32_t period_us)
{
#if SOFT_UART_TX_ENABLE
    // TX使用MR0通道，配置中断+复位模式
    SoftUart_Timer16(SOFT_UART_TIM, SOFT_UART_TX_TIM_CH, (TIMER16_MR_I | TIMER16_MR_R), period_us);
    // 初始化时关闭TX通道（按需开启）
    TIMER16_CloseChannel(SOFT_UART_TIM, SOFT_UART_TX_TIM_CH);
#endif

#if SOFT_UART_RX_ENABLE
    // RX配置中断+复位模式（半周期采样）
    SoftUart_Timer16(SOFT_UART_TIM, SOFT_UART_RX_TIM_CH, (TIMER16_MR_I | TIMER16_MR_R), period_us);
    // 初始化时关闭RX通道（GPIO中断触发后开启）
    TIMER16_CloseChannel(SOFT_UART_TIM, SOFT_UART_RX_TIM_CH);
#endif

    // 使能定时器中断（NVIC层面）
    NVIC_SetPriority(SOFT_UART_TIM_IRQ, 0);
    NVIC_EnableIRQ(SOFT_UART_TIM_IRQ);
}

/************************ 软件串口初始化 ************************/
void SoftUART_Init(void)
{
#if SOFT_UART_TX_ENABLE
    // 1. 初始化TX引脚（推挽输出，空闲高电平）
    HAL_GPIO_Init(SOFT_UART_TX_GPIO, SOFT_UART_TX_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
    SoftUART_SetTxPin(true);
    memset(tx_struct.buf, 0, sizeof(tx_struct.buf));
    tx_struct.state = SOFT_UART_IDLE;
    tx_struct.bit_cnt = 0;
#endif

#if SOFT_UART_RX_ENABLE
    // 2. 初始化RX引脚（上拉输入，下降沿中断）
    HAL_GPIO_Init(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN, true);
    HAL_GPIO_EnableIRQ(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN, HAL_GPIO_IRQ_FALLING);
    // 配置RX GPIO中断优先级
    NVIC_SetPriority(SOFT_UART_RX_IRQ, 1);
    NVIC_EnableIRQ(SOFT_UART_RX_IRQ);

    rx_struct.state = SOFT_UART_RX_STATE_IDLE;
#endif
}

/************************ 新增：适配hal_uart的soft_uart2_init ************************/
void soft_uart2_init(uint32_t baudrate, uart_callback_t callback)
{
#if SOFT_UART_RX_ENABLE
    // 1. 保存上层回调函数
    g_soft_uart2_callback = callback;
#endif

    // 2. 初始化软件串口核心
    SoftUART_Init();

    // 3. 如果传入波特率非0，重新配置定时器周期
    if (baudrate > 0)
    {
        uint32_t period_us = SoftUART_CalcTimPeriod(baudrate);
        SoftUART_Tim16_Init(period_us);
    }

    OB_LOGI(TAG, "soft_uart2 init: baudrate=%d", baudrate);
}

/************************ 新增：适配hal_uart的soft_uart2_sleep ************************/
void soft_uart2_sleep(uint8_t mode, uint8_t level)
{
    // 1. 停止发送（关闭TX定时器通道）
#if SOFT_UART_TX_ENABLE
    TIMER16_CloseChannel(SOFT_UART_TIM, SOFT_UART_TX_TIM_CH);
    tx_struct.state = SOFT_UART_IDLE;
    // 设置TX引脚电平
    SoftUART_SetTxPin(level ? true : false);
    // 切换TX引脚为普通GPIO模式
    HAL_GPIO_Init(SOFT_UART_TX_GPIO, SOFT_UART_TX_PIN, (HalGpioMode)mode, HAL_GPIO_PULL_NONE);
#endif

    // 2. 停止接收（关闭RX中断和定时器）
#if SOFT_UART_RX_ENABLE
    // 禁用RX GPIO中断
    HAL_GPIO_DisableIRQ(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN);
    // 关闭RX定时器通道
    TIMER16_CloseChannel(SOFT_UART_TIM, SOFT_UART_RX_TIM_CH);
    // 设置RX引脚电平
    HAL_GPIO_Write(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN, (level ? true : false));
    // 切换RX引脚为普通GPIO模式
    HAL_GPIO_Init(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN, (HalGpioMode)mode, HAL_GPIO_PULL_NONE);
#endif

    OB_LOGI(TAG, "soft_uart2 sleep: mode=%d, level=%d", mode, level);
}

/************************ 发送功能 ************************/
#if SOFT_UART_TX_ENABLE
/**
 * @brief 软串口裸字节发送（完全替代UART_Write，非阻塞）
 * @param pUart: 兼容原函数接口的占位参数（软串口无硬件UART，传NULL即可）
 * @param pTxBuf: 待发送的字节数组（不可为NULL）
 * @param nWriteBytes: 要发送的字节数
 * @retval 成功写入发送缓冲区的字节数（缓冲区满则停止，返回已写入长度；参数非法返回0）
 */
uint32_t SoftUART_Write(OB_UART_Type *pUart, uint8_t *pTxBuf, uint32_t nWriteBytes)
{
    // 参数合法性校验（和原UART_Write对齐）
    if (pTxBuf == NULL || nWriteBytes == 0)
    {
        return 0;
    }

    uint32_t send_cnt = 0; // 已成功写入缓冲区的字节数
    // 逐字节写入软串口发送缓冲区
    for (uint32_t i = 0; i < nWriteBytes; i++)
    {
        if (((softuart_tx_wr + 1U) & SOFTUART_TX_BUF_MASK) == softuart_tx_rd)
        {
            break;
        }
        softuart_txbuf[softuart_tx_wr] = pTxBuf[i]; // 直接填数，无中间拷贝
        softuart_tx_wr = (softuart_tx_wr + 1U) & SOFTUART_TX_BUF_MASK; // 写指针自增+环形取模
        send_cnt++;
    }
    if (tx_struct.state == SOFT_UART_IDLE && send_cnt > 0)
    {
        // 从缓冲区取一字节（环形缓冲区）
        tx_struct.data = softuart_txbuf[softuart_tx_rd];
        softuart_tx_rd = (softuart_tx_rd + 1U) & SOFTUART_TX_BUF_MASK;
        // 标记发送忙，发起始位（低电平）
        tx_struct.state = SOFT_UART_TX_BUSY;
        tx_struct.bit_cnt = 0;
        SOFT_UART_TX_PIN_CLR;
        TIMER16_EnableChannel(SOFT_UART_TIM, SOFT_UART_TX_TIM_CH);
        SOFT_UART_TIM->TMR16IR = 0x01; // 清除MR0中断标志
        TIMER16_Enable(SOFT_UART_TIM);
    }
    return send_cnt;
}
#else
// TX关闭时，空实现兼容接口
uint32_t SoftUART_Write(OB_UART_Type *pUart, uint8_t *pTxBuf, uint32_t nWriteBytes)
{
    (void)pUart;
    (void)pTxBuf;
    (void)nWriteBytes;
    return 0;
}
#endif

#else
// 总开关关闭时，所有接口空实现
void SoftUART_Init(void) {}
uint32_t SoftUART_Write(OB_UART_Type *pUart, uint8_t *pTxBuf, uint32_t nWriteBytes)
{
    (void)pUart;
    (void)pTxBuf;
    (void)nWriteBytes;
    return 0;
}
uint32_t SoftUART_Read(OB_UART_Type *pUart, uint8_t *pRxBuf, uint32_t nReadBytes)
{
    (void)pUart;
    (void)pRxBuf;
    (void)nReadBytes;
    return 0;
}
uint8_t SoftUART_RxHandler(uint8_t *buf, uint8_t *len, uint8_t buf_len)
{
    (void)buf;
    (void)len;
    (void)buf_len;
    return 0;
}
void SoftUART_IRQHandler(void) {}
void soft_uart2_init(uint32_t baudrate, uart_callback_t callback)
{
    (void)baudrate;
    (void)callback;
}
void soft_uart2_sleep(uint8_t mode, uint8_t level)
{
    (void)mode;
    (void)level;
}
#endif
