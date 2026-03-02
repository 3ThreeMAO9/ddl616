/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: soft_uart_log.c
 * Desc: 
 * Version: 1.0.1
 * Revision: James_Zhang
 * Date: 2025-12-13
 */

#include "soft_uart_cfg.h"
#include <stdarg.h>
#include <string.h>
#include "soft_uart_log.h"
#include "ringbuffer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "soft_uart_log"

#if SOFT_UART_ENABLE && SOFT_UART_LOG_TX_ENABLE
/************************ 全局变量 ****************************/
SoftUART_LogTxStruct_t log_tx_struct = {0};

/************************ 静态全局变量 ************************/
static struct rt_ringbuffer log_tx_rb;

/************************ 私有函数声明 ************************/

/************************ 私有函数实现 ************************/
// 启动LOG_TX缓冲区下一字节发送
void SoftUART_StartLogTxNextByte(void)
{
    __disable_irq();
    if (rt_ringbuffer_data_len(&log_tx_rb) == 0)
    {
        __enable_irq();
        log_tx_struct.state = SOFT_UART_IDLE;
        log_tx_struct.bit_cnt = 0;
        TIMER16_CloseChannel(SOFT_UART_LOG_TIM, SOFT_UART_LOG_TX_TIM_CH);
        return;
    }

    rt_ringbuffer_get(&log_tx_rb, &log_tx_struct.data, 1);
    __enable_irq();

    log_tx_struct.state = SOFT_UART_TX_BUSY;
    log_tx_struct.bit_cnt = 0;
    SoftUART_Log_SetTxPin(0);

    TIMER16_EnableChannel(SOFT_UART_LOG_TIM, SOFT_UART_LOG_TX_TIM_CH);
    SOFT_UART_LOG_TIM->TMR16IR = 0x04;
    TIMER16_Enable(SOFT_UART_LOG_TIM);
}

/************************ 对外接口实现 ************************/
// LOG_TX初始化
void SoftUART_LogTx_Init(void)
{
    // 1. 初始化LOG_TX引脚（推挽输出，空闲高电平）
    HAL_GPIO_Init(SOFT_UART_LOG_TX_GPIO, SOFT_UART_LOG_TX_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
    SoftUART_Log_SetTxPin(true);

    // 2. 初始化LOG_TX发送缓冲区和状态
    memset(log_tx_struct.buf, 0, sizeof(log_tx_struct.buf));
    log_tx_struct.state = SOFT_UART_IDLE;
    log_tx_struct.bit_cnt = 0;

    rt_ringbuffer_init(&log_tx_rb, log_tx_struct.buf, SOFT_UART_LOG_TX_BUF_LEN);

    // 3. 初始化LOG_TX定时器（MR2通道，中断+复位模式）
    SoftUart_Timer16(SOFT_UART_LOG_TIM, SOFT_UART_LOG_TX_TIM_CH, (TIMER16_MR_I | TIMER16_MR_R), 
                     SoftUART_CalcTimPeriod(SOFT_UART_LOG_TX_BAUDRATE));
    TIMER16_CloseChannel(SOFT_UART_LOG_TIM, SOFT_UART_LOG_TX_TIM_CH);

    // 4. 使能定时器中断（NVIC层面）
    NVIC_SetPriority(SOFT_UART_LOG_TIM_IRQ, 1);
    NVIC_EnableIRQ(SOFT_UART_LOG_TIM_IRQ);

    OB_LOGI(TAG, "soft uart log tx init success, baudrate=%d", SOFT_UART_LOG_TX_BAUDRATE);
}

// LOG_TX字节发送接口（非阻塞）
uint32_t SoftUART_LogWrite(OB_UART_Type *pUart, uint8_t *pTxBuf, uint32_t nWriteBytes)
{
    (void)pUart; // 兼容接口，无实际意义
    // 参数合法性校验
    if (pTxBuf == NULL || nWriteBytes == 0)
    {
        OB_LOGW(TAG, "invalid param: pTxBuf=%p, nWriteBytes=%d", pTxBuf, nWriteBytes);
        return 0;
    }

    uint32_t send_cnt = 0;
    for (uint32_t i = 0; i < nWriteBytes; i++)
    {
        __disable_irq(); // 关中断保护缓冲区操作
        uint8_t byte = pTxBuf[i];
        bool ret = (rt_ringbuffer_put(&log_tx_rb, &byte, 1) == 1);
        if (ret)
            send_cnt++;
        __enable_irq();

        if (!ret)
        {
            OB_LOGW(TAG, "log tx buffer full, send_cnt=%d", send_cnt);
            break; // 缓冲区满则停止
        }
    }

    // 若当前空闲，启动发送
    if (log_tx_struct.state == SOFT_UART_IDLE && send_cnt > 0)
    {
        SoftUART_StartLogTxNextByte();
    }

    return send_cnt;
}

#else
// 总开关/LOG_TX关闭时，空实现兼容接口
void SoftUART_LogTx_Init(void) {}
uint32_t SoftUART_LogWrite(OB_UART_Type *pUart, uint8_t *pTxBuf, uint32_t nWriteBytes)
{
    (void)pUart;
    (void)pTxBuf;
    (void)nWriteBytes;
    return 0;
}
#endif
