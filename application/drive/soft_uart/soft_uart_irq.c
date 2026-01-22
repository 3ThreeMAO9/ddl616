/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: soft_uart_irq.c
 * Desc: 软件串口定时器中断处理模块
 * Version: 1.0.1
 * Revision: James_Zhang
 * Date: 2025-12-13
 */

#include "soft_uart_cfg.h"
#include "soft_uart.h"
#include "soft_uart_log.h" // 引入LOG_TX相关声明

/************************* 外部变量声明** **************************/
#if SOFT_UART_TX_ENABLE
extern SoftUART_TxStruct_t tx_struct;
extern void SoftUART_StartTxNextByte(void);
#endif

#if SOFT_UART_RX_ENABLE
extern SoftUART_RxStruct_t rx_struct;
extern bool SoftUART_PutRxBuf(uint8_t data);
extern void SoftUART_RxIdleTimeoutHandler(void);
#endif

#if SOFT_UART_LOG_TX_ENABLE
extern SoftUART_LogTxStruct_t log_tx_struct;
extern void SoftUART_StartLogTxNextByte(void);
#endif

/************************ 定时器中断服务函数 ************************/
void SoftUART_IRQHandler(void)
{
    uint32_t irq_flag = SOFT_UART_TIM->TMR16IR; // 读取所有中断标志
#if SOFT_UART_RX_ENABLE
    // 处理RX中断（MR1，BIT1）
    if (irq_flag & 0x02)
    {
        SOFT_UART_TIM->TMR16IR = 0x02; // 清除MR1中断标志
        uint8_t rx_bit = SoftUART_GetRxPin();
        switch (rx_struct.state)
        {
        case SOFT_UART_RX_STATE_IDLE:
            if (rx_bit == 0)
            {
                rx_struct.state = SOFT_UART_RX_STATE_TRANSFER;
                rx_struct.bit_cnt = 0;
                rx_struct.data = 0;
                rx_struct.tick = 0;
            }
            else
            {
                if (rx_struct.tick < 255)
                    rx_struct.tick++;
                SoftUART_RxIdleTimeoutHandler();
            }
            break;

        case SOFT_UART_RX_STATE_TRANSFER:
            // 采样8位数据位（LSB先行）
            rx_struct.bit_cnt++;
            if (rx_bit)
                rx_struct.data |= (1 << (rx_struct.bit_cnt - 1));
            else
                rx_struct.data &= ~(1 << (rx_struct.bit_cnt - 1));
            // 8位数据采样完成，进入停止位检测
            if (rx_struct.bit_cnt >= 8)
            {
                SoftUART_EnabledINT();
                rx_struct.state = SOFT_UART_RX_STATE_STOP;
                rx_struct.stop_timeout = 0;
            }
            break;

        case SOFT_UART_RX_STATE_STOP:
            rx_struct.stop_timeout++;
            // 检测到停止位（高电平）
            if (rx_bit)
            {
                // 写入接收缓冲区
                SoftUART_PutRxBuf(rx_struct.data);
                // 回到空闲态
                rx_struct.state = SOFT_UART_RX_STATE_IDLE;
                rx_struct.bit_cnt = 0;
                rx_struct.tick = 0;
            }
            // 停止位超时容错（调整为3个半周期，提高兼容性）
            else if (rx_struct.stop_timeout >= 3)
            {
                rx_struct.state = SOFT_UART_RX_STATE_IDLE;
                rx_struct.bit_cnt = 0;
                rx_struct.tick = 0;
            }
            break;

        default:
            rx_struct.state = SOFT_UART_RX_STATE_IDLE;
            break;
        }
    }
#endif

#if SOFT_UART_TX_ENABLE
    // 处理TX中断（MR0，BIT0）
    if (irq_flag & 0x01)
    {
        SOFT_UART_TIM->TMR16IR = 0x01; // 清除MR0中断标志
        if (tx_struct.state == SOFT_UART_TX_BUSY)
        {
            // 缓存局部变量，减少结构体访问
            uint8_t bit_cnt = tx_struct.bit_cnt + 1;
            uint8_t tx_data = tx_struct.data;

            // if替代switch，减少分支开销
            if (bit_cnt <= 8) // 数据位
            {
                SoftUART_SetTxPin((tx_data >> (bit_cnt - 1)) & 0x01);
                tx_struct.bit_cnt = bit_cnt;
            }
            else if (bit_cnt == 9) // 停止位
            {
                SoftUART_SetTxPin(1);
                tx_struct.bit_cnt = bit_cnt;
            }
            else if (bit_cnt == 10) // 字节发送完成
            {
                tx_struct.bit_cnt = 0;      // 复位计数
                SoftUART_StartTxNextByte(); // 启动下一字节
            }
            else
            {
                tx_struct.bit_cnt = 0;
                tx_struct.state = SOFT_UART_IDLE;
                TIMER16_CloseChannel(SOFT_UART_TIM, SOFT_UART_TX_TIM_CH);
            }
        }
    }
#endif

#if SOFT_UART_LOG_TX_ENABLE
    // 处理LOG_TX中断（MR2，BIT2）
    if (irq_flag & 0x04)
    {
        SOFT_UART_LOG_TIM->TMR16IR = 0x04; // 清除MR2中断标志
        if (log_tx_struct.state == SOFT_UART_TX_BUSY)
        {
            uint8_t bit_cnt = log_tx_struct.bit_cnt + 1;
            uint8_t tx_data = log_tx_struct.data;

            if (bit_cnt <= 8) // 发送数据位（LSB先行）
            {
                SoftUART_Log_SetTxPin((tx_data >> (bit_cnt - 1)) & 0x01);
                log_tx_struct.bit_cnt = bit_cnt;
            }
            else if (bit_cnt == 9) // 发送停止位（高电平）
            {
                SoftUART_Log_SetTxPin(1);
                log_tx_struct.bit_cnt = bit_cnt;
            }
            else if (bit_cnt == 10) // 字节发送完成，启动下一字节
            {
                log_tx_struct.bit_cnt = 0;
                SoftUART_StartLogTxNextByte();
            }
            else // 异常状态复位
            {
                log_tx_struct.bit_cnt = 0;
                log_tx_struct.state = SOFT_UART_IDLE;
                TIMER16_CloseChannel(SOFT_UART_LOG_TIM, SOFT_UART_LOG_TX_TIM_CH);
            }
        }
    }
#endif
}

#if SOFT_UART_RX_ENABLE
/************************ RX GPIO中断处理函数 ************************/
void SoftUART_RxGPIO_IRQHandler(void)
{
    // 检测RX引脚中断标志
    if (HAL_GPIO_ReadIntState(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN))
    {
        // 关闭RX GPIO中断（避免重复触发）
        SoftUART_DisabledINT();
        // 清除中断标志
        HAL_GPIO_ClearIntState(SOFT_UART_RX_GPIO, SOFT_UART_RX_PIN);
        // 复位计数到半周期，首次采样落在比特中心
        SOFT_UART_TIM->TMR16TC = SOFT_UART_TIM->TMR16MR1 >> 1; // 等价/2，移位更快

        // 复位并开启RX定时器通道（MR1）
        SOFT_UART_TIM->TMR16TCR = TIMER16_RESET | TIMER16_ENABLE;
        // 关键修复：仅检查RX通道（MR1）的MCR配置
        // 1. 计算RX通道（MR1）的移位值和掩码（和底层函数完全一致）
        uint32_t rx_shift = 0x03; // MR1对应移位3位（和TIMER16_EnableInterrupt中一致）
        uint32_t rx_mcr_mask = 7 << rx_shift; // 7=0b111，对应MR1的3位掩码（bit3-5）
        // 2. 检查RX通道的MCR是否未配置（值为0）
        if ((SOFT_UART_TIM->TMR16MCR & rx_mcr_mask) == 0)
            TIMER16_EnableInterrupt(SOFT_UART_TIM, SOFT_UART_RX_TIM_CH, (TIMER16_MR_I | TIMER16_MR_R));
        SOFT_UART_TIM->TMR16IR = 0x02;

        // 初始化接收状态
        rx_struct.state = SOFT_UART_RX_STATE_TRANSFER;
        rx_struct.bit_cnt = 0;
        rx_struct.data = 0;
        rx_struct.stop_timeout = 0;
        rx_struct.tick = 0;
    }
}
#else
void SoftUART_RxGPIO_IRQHandler(void) {}
#endif

