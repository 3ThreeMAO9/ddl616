#include "OB90A64M1.h"
#include "spi.h"
#include "math.h"
#include "stddef.h"
#include <string.h>

volatile uint32_t g_SPIRxCounter = 0x00;
volatile uint8_t g_SPIRxData[SPI_RX_SiZE];

void SPI_IRQHandler(void)
{
    if ((OB_SPI->SR & SPI_SR_RNE) == SPI_SR_RNE)
    {
        if (g_SPIRxCounter < SPI_RX_SiZE) // 防止缓冲区溢出
            g_SPIRxData[g_SPIRxCounter] = OB_SPI->DR;
        g_SPIRxCounter++;

        OB_SPI->ICR = SPI_INT_RTIM;
    }
}

uint32_t SPI_MasterOpen(OB_SPI_Type *pSPI, uint32_t nSPICapability, uint32_t nBusClock)
{
    NVIC_EnableIRQ(SPI_IRQn);

    pSPI->CR0 = nSPICapability;

    pSPI->CR1_b.MS   = 0; // Master Mode
    pSPI->CR1_b.SSNP = 0; // Slave Hardware Control
    // pSPI->CR1_b.SSNP = 1; // Slave Hardware Control     //CS 引脚控制是软件：0   硬件：1
    pSPI->CR1_b.SSE  = 1; // SPI Enable

    return (SPI_SetBusClock(pSPI, nBusClock) );
}

void SPI_Close(OB_SPI_Type *pSPI)
{
    pSPI->CR1_b.SSE =  0;  // SPI Disable
}

void SPI_EnableINT(OB_SPI_Type *pSPI, uint32_t nINTSel)
{
    pSPI->IMSC |= nINTSel;
}

void SPI_DisableINT(OB_SPI_Type *pSPI, uint32_t nINTSel)
{
    pSPI->IMSC &= (~nINTSel);

}

void SPI_ClearRxFIFO(OB_SPI_Type *pSPI)
{
    if (pSPI == NULL)  // 增加空指针校验，避免崩溃
        return;

    volatile uint32_t dummy;  // 用于读取并丢弃数据
    // 循环读取，直到接收FIFO为空（RNE=0）
    while ((pSPI->SR & SPI_SR_RNE) != 0)
    {
        dummy = pSPI->DR;  // 读取数据，清空FIFO
    }
}

uint8_t SPI_ClearTxFIFO(OB_SPI_Type *pSPI)
{
    uint32_t timeout = 100000;
    while (timeout-- > 0) {
        if ((pSPI->SR & SPI_SR_TFE) == SPI_SR_TFE)
            return 0; // 成功
    }
    return 1; // 超时错误
}

uint32_t SPI_SetBusClock(OB_SPI_Type *pSPI, uint32_t nBusClock)
{
    uint32_t nClock;

    nClock = GetPeripheralClock(APB_SPI);
    nClock = nClock / nBusClock;
    nClock = sqrt(nClock);
    pSPI->CR0 = (pSPI->CR0 & 0xFFFF00FF) | (nClock << 8);
    pSPI->CPSR = nClock;

    return SPI_GetBusClock(pSPI);
}

uint32_t SPI_GetBusClock(OB_SPI_Type *pSPI)
{
    uint32_t nClock, nSCR;

    nClock = GetPeripheralClock(APB_SPI);
    nSCR   = (pSPI->CR0 >> 0x08) & 0xFF;
    nClock = nClock /(pSPI->CPSR * (nSCR + 1));
    return nClock;
}

// 向SPI发送FIFO写入数据
uint8_t SPI_WriteFIFO(OB_SPI_Type *pSPI, const uint8_t *pBuf, uint32_t len)
{
    // 增加SPI外设指针的空指针校验，避免非法访问
    if (pSPI == NULL || pBuf == NULL || len == 0)
    {
        return 0;
    }
    
    uint32_t write_cnt = 0;
    volatile uint32_t sr_reg;  // 缓存状态寄存器值，volatile防止编译器优化
    
    // 循环写入FIFO，直到数据写完或FIFO满
    while (write_cnt < len)
    {
        // 读取状态寄存器，检查发送FIFO是否未满
        sr_reg = pSPI->SR;
        if ((sr_reg & SPI_SR_TNF) == 0)
        {
            break;  // 发送FIFO已满，退出写入
        }
        
        // 写入数据寄存器，显式转换数据类型（若DR位宽大于8位，确保类型匹配）
        pSPI->DR = (uint32_t)pBuf[write_cnt];
        write_cnt++;
    }
    return write_cnt;
}

uint8_t SPI_ReadWithClock(OB_SPI_Type *pSPI, uint8_t *pBuf, uint32_t len)
{
    if (pSPI == NULL || pBuf == NULL || len == 0)
        return 0;

    SPI_ClearRxFIFO(pSPI);
    uint32_t total_read = 0;
    const uint8_t dummy = 0x00;

    while (total_read < len)
    {
        pSPI->DR = (uint32_t)dummy;

        uint32_t timeout = 10000;
        while (--timeout && (pSPI->SR & SPI_SR_BSY));
        if (timeout == 0)
            break;

        if (pSPI->SR & SPI_SR_RNE)
        {
            pBuf[total_read++] = (uint8_t)pSPI->DR;
        }
    }

    return total_read;
}
