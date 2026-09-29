/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_sfud_dma.c
 * Desc: SPI Flash 读 PDMA 加速（OB90A64M1）
 * Date: 2026-09-29
 *
 * 只做读加速（写接 DMA 无收益：页编程等待占了大头）。
 * 实测：读 1KB 4350us -> 2150us（235 -> 476 KB/s），SCK 6MHz。
 *
 * ⚠️ DATAWIDTH 必须用 BYTE，不能照手册用 WORD：
 *    WORD 时一次 32 位 DR 访问吃掉 4 个 SPI 帧、只落 1 字节到 buffer，
 *    数据变成 03 00 00 00 07 00 00 00 ...（步长 4 错位）。
 * 通用教训：DMA 宽度看"每次访问搬几个外设帧"，不是总线位宽。
 *
 * 双通道（RX 收 + TX 哑元出时钟）需先挂 RX 再启 TX，否则首帧丢失导致错位。
 */

#include "OB90A64M1.h"
#include "dma.h"
#include "spi.h"
#include "bsp_sfud_dma.h"

#define DMA_CH_RX           OB_PDMA1
#define DMA_CH_TX           OB_PDMA0
#define DMA_WAIT_LOOPS      3000000UL

#define PDMA_ENB            (1UL << 0)
#define PDMA_FININTSTS      (1UL << 1)

/* TX 哑元源：4 字节对齐，PDMA 要求 */
static uint32_t s_dummy __attribute__((aligned(4))) = 0UL;

/* 等通道完成（带超时，防死等） */
static uint32_t dma_wait(OB_PDMA_Type *pDMA)
{
    uint32_t t = DMA_WAIT_LOOPS;

    while ((pDMA->PDMA_CSR & PDMA_FININTSTS) == 0U) {
        if (--t == 0U) return 1U;
    }
    pDMA->PDMA_CSR |= PDMA_FININTSTS;           /* W1C */

    t = DMA_WAIT_LOOPS;
    while ((pDMA->PDMA_CSR & PDMA_ENB) != 0U) {
        if (--t == 0U) return 1U;
    }
    return 0U;
}

/* 配一条通道：src/dst 总线类型、地址、增量、事件、长度 */
static void dma_cfg(OB_PDMA_Type *pDMA, uint32_t src_bus, uint32_t src,
                    uint32_t src_inc, uint32_t dst_bus, uint32_t dst,
                    uint32_t dst_inc, uint32_t event, uint32_t len)
{
    pDMA->PDMA_CSR = (src_bus << 6) | (dst_bus << 7);
    DMA_SourceAddr(pDMA, src);
    DMA_SourceMode(pDMA, src_inc);
    DMA_SourecEvent(pDMA, event);
    DMA_DestinationAddr(pDMA, dst);
    DMA_DestinationMode(pDMA, dst_inc);
    DMA_DestinationEvent(pDMA, 0);
    DMA_DataWidth(pDMA, DMA_WIDTH_BYTE);
    DMA_DataLength(pDMA, len);
}

/* 读 len 字节到 buf：RX 通道收数据，TX 通道推哑元出时钟。
 * 成功返回 len，失败返回 0（buf 内容不可信）。 */
uint32_t bsp_sfud_dma_read(uint8_t *buf, uint32_t len)
{
    uint32_t err;

    /* 先等命令/地址移位完成再清 RX FIFO，否则残留字节会成为数据前缀 */
    spi_wait_idle(OB_SPI);
    SPI_ClearRxFIFO(OB_SPI);

    DMA_CH_RX->PDMA_CSR &= ~PDMA_ENB;
    DMA_CH_TX->PDMA_CSR &= ~PDMA_ENB;

    /* RX: SPI->DR(APB,不增) -> buf(AHB,每字节+1) */
    dma_cfg(DMA_CH_RX, DMA_BUS_APB, (uint32_t)&OB_SPI->DR, DMA_MODE_INC_0,
            DMA_BUS_AHB, (uint32_t)buf, DMA_MODE_INC_1,
            DMA_EVENT_SPI_Rx, len);

    /* TX: s_dummy(AHB,不增) -> SPI->DR(APB,不增) */
    dma_cfg(DMA_CH_TX, DMA_BUS_AHB, (uint32_t)&s_dummy, DMA_MODE_INC_0,
            DMA_BUS_APB, (uint32_t)&OB_SPI->DR, DMA_MODE_INC_0,
            DMA_EVENT_SPI_Tx, len);

    OB_SPI->CONDMA_b.DMAE = 1;

    /* RX 先就位再放 TX，否则 TX 抢跑发帧、首帧数据丢失 */
    DMA_Enable(DMA_CH_RX);
    DMA_Enable(DMA_CH_TX);

    err  = dma_wait(DMA_CH_RX);
    err |= dma_wait(DMA_CH_TX);

    OB_SPI->CONDMA_b.DMAE = 0;
    DMA_CH_RX->PDMA_CSR &= ~PDMA_ENB;
    DMA_CH_TX->PDMA_CSR &= ~PDMA_ENB;

    return (err == 0U) ? len : 0U;
}
