/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_sfud_dma.c
 * Desc: SPI Flash 读 PDMA 加速（OB90A64M1）
 * Date: 2026-09-29
 *
 * 只做读加速（写接 DMA 无收益：页编程等待占了大头）。
 * 实测：读 1KB 4350us -> 2150us（235 -> 476 KB/s），SCK 6MHz。
 *
 * ⚠️ DR 是 32 位寄存器，但 SPI 每次收发只有 8 位有效：
 *    DATAWIDTH = WORD  时，1 次 32 位 DR 访问 = 消耗 4 个 SPI 帧、只产出 1 个有效字节
 *                      → 有效字节按步长 4 散落，需二次压缩（见 FLASH_DMA_WORD）
 *    DATAWIDTH = BYTE  时，1 次 8 位 DR 访问  = 消耗 1 个 SPI 帧、产出 1 个字节（最优）
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

/* ---- 数据宽度选择：0 = BYTE（推荐，直接写 buf）；1 = WORD（照手册，需二次压缩） ---- */
#define FLASH_DMA_WORD      0

/* WORD 模式：一次只搬这么多"有效字节"，收完立刻压缩回 buf。
 * 静态占用固定 = FLASH_DMA_CHUNK * 4 B，与 len 无关。
 * 调小可省 RAM（16*4 = 64 B），但分块变多、DMA 重启开销增加。 */
#ifndef FLASH_DMA_CHUNK
#define FLASH_DMA_CHUNK     64U
#endif

/* TX 哑元源：4 字节对齐，PDMA 要求 */
static uint32_t s_dummy __attribute__((aligned(4))) = 0UL;

#if FLASH_DMA_WORD
/* WORD 模式散写区：DMA 把有效字节按 +4 步长落在这里，再压缩回 buf */
static uint8_t s_raw[FLASH_DMA_CHUNK * 4U + 4U] __attribute__((aligned(4)));
#endif

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
#if FLASH_DMA_WORD
    DMA_DataWidth(pDMA, DMA_WIDTH_WORD);        /* 1 cycle = 4 个 SPI 帧 */
#else
    DMA_DataWidth(pDMA, DMA_WIDTH_BYTE);        /* 1 cycle = 1 个 SPI 帧 */
#endif
    DMA_DataLength(pDMA, len);
}

/* 跑一次双通道 DMA：RX 收 n 字节到 dst（步长 dst_inc），TX 推 n 个 cycle 哑元。
 * 成功返回 0；失败返回非 0。 */
static uint32_t dma_run(uint32_t dst, uint32_t dst_inc, uint32_t cyc)
{
    uint32_t err;

    DMA_CH_RX->PDMA_CSR &= ~PDMA_ENB;
    DMA_CH_TX->PDMA_CSR &= ~PDMA_ENB;

    /* RX: SPI->DR(APB,不增) -> dst(AHB,按 dst_inc 递增) */
    dma_cfg(DMA_CH_RX, DMA_BUS_APB, (uint32_t)&OB_SPI->DR, DMA_MODE_INC_0,
            DMA_BUS_AHB, dst, dst_inc,
            DMA_EVENT_SPI_Rx, cyc);

    /* TX: s_dummy(AHB,不增) -> SPI->DR(APB,不增) */
    dma_cfg(DMA_CH_TX, DMA_BUS_AHB, (uint32_t)&s_dummy, DMA_MODE_INC_0,
            DMA_BUS_APB, (uint32_t)&OB_SPI->DR, DMA_MODE_INC_0,
            DMA_EVENT_SPI_Tx, cyc);

    OB_SPI->CONDMA_b.DMAE = 1;

    /* RX 先就位再放 TX，否则 TX 抢跑发帧、首帧数据丢失 */
    DMA_Enable(DMA_CH_RX);
    DMA_Enable(DMA_CH_TX);

    err  = dma_wait(DMA_CH_RX);
    err |= dma_wait(DMA_CH_TX);

    OB_SPI->CONDMA_b.DMAE = 0;
    DMA_CH_RX->PDMA_CSR &= ~PDMA_ENB;
    DMA_CH_TX->PDMA_CSR &= ~PDMA_ENB;

    return err;
}

/* 读 len 字节到 buf：RX 通道收数据，TX 通道推哑元出时钟。
 * 成功返回 len，失败返回 0（buf 内容不可信）。 */
uint32_t bsp_sfud_dma_read(uint8_t *buf, uint32_t len)
{
    /* 先等命令/地址移位完成再清 RX FIFO，否则残留字节会成为数据前缀 */
    spi_wait_idle(OB_SPI);
    SPI_ClearRxFIFO(OB_SPI);

#if FLASH_DMA_WORD
    /* WORD 模式：1 cycle 耗 4 个 SPI 帧、只产出 1 个有效字节，且必须落在步长 4 的位置。
     * 故按 FLASH_DMA_CHUNK 分块：每块先散写到 s_raw[]，再压缩回 buf。 */
    for (uint32_t off = 0U; off < len; off += FLASH_DMA_CHUNK) {
        uint32_t n = len - off;
        uint32_t i;

        if (n > FLASH_DMA_CHUNK) n = FLASH_DMA_CHUNK;

        if (dma_run((uint32_t)s_raw, DMA_MODE_INC_4, n) != 0U)
            return 0U;

        /* 压缩：s_raw[4i] -> buf[off+i]（4 字节里只有第 1 个是真数据） */
        for (i = 0U; i < n; i++)
            buf[off + i] = s_raw[i * 4U];
    }
#else
    if (dma_run((uint32_t)buf, DMA_MODE_INC_1, len) != 0U)
        return 0U;
#endif

    return len;
}
