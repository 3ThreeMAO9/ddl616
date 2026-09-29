/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_sfud_dma.c
 * Desc: SPI Flash 读写 PDMA 加速实现（OB90A64M1）
 * Version: 0.1.0 (draft，需实测验证)
 * Date: 2026-09-29
 *
 * ─── 硬件事实（均已从 OB90A64M1.h / spi.c 核对） ───────────────────────
 * 1. SPI = ARM PrimeCell SSP(PL022)：
 *      DR 注释 "Writes fill the transmit FIFO, and reads empty the receive FIFO"
 *      → TX / RX 是各自独立的 8 帧 FIFO，不是共享 FIFO。
 * 2. SPI->CONDMA : bit31 DMAE / bit29 DMATREQ / bit28 DMARREQ。
 * 3. PDMA 通道 0~3 = OB_PDMA0(0x40100080) / 1(0x40100090) / 2(0x401000A0) / 3(0x401000B0)
 *    PDMA_CSR: bit0 ENB / bit1 FININTSTS(W1C) / bit2 FININTENB
 *              bit6  SRCADRSEL(0=APB,1=AHB) / bit7 DESADRSEL
 *              bit8-10  SRCADRINC / bit12-14 DESADRINC
 *              bit16-19 DESREQSEL / bit20-21 DATAWIDTH / bit24-27 SRCREQSEL
 *    事件号：DMA_EVENT_SPI_Tx = 1，DMA_EVENT_SPI_Rx = 2。
 *
 * ─── ⚠️ 关键约束（实测结论，别照手册改回去） ────────────────────────────
 * PDMA_CSR_b.DATAWIDTH 手册注明：
 *     "when the DMA source or the destination is APB device, the data width
 *      must be set to word. The APB device supports the word-width transfer only."
 * 但实测（PY25Q32HB + OB90A64M1）**必须用 BYTE**：
 *   WORD 宽度下，一次 32 位 DR 访问会吃掉 4 个 SPI 帧、却只有 1 个字节落进 buffer，
 *   结果是有效数据以步长 4 分布 —— 实测前 16 字节为
 *       03 00 00 00 07 00 00 00 0B 00 00 00 0F 00 00 00
 *   即 d[3] d[7] d[11] d[15]，readback verify [FAIL]。
 * 改成 FLASH_DMA_WIDTH = DMA_WIDTH_BYTE 后：1 cycle = 1 SPI 帧，readback verify [OK]。
 *
 * 通用教训：DMA 宽度要看"每次访问搬几个外设帧"，不是"总线位宽"。
 *            FIFO 型外设（SPI DR）上 word 访问可能等于多帧，务必用已知 pattern 验证。
 *
 * ─── 为什么只用两个独立通道（留作参考） ────────────────────────────────
 * 原方案是双通道（RX 通道收数据 + TX 哑元通道出时钟），但探针实测发现
 * TX 通道一使能就抢着灌满 8 帧 TX FIFO 并开始发帧，而 RX 通道此时才刚挂上，
 * 在"CONDMA 只有单个 DMA 请求口"的前提下双方全程互相抢占。
 * 目前 readback 是 [OK] 的，但若日后出现偶发错位，可换：
 *   bsp_sfud_dma_read_rx_only() —— RX 走 DMA、TX 哑元由 CPU 推，
 *   彻底避开 DMAC 仲裁竞争（代价：TX 侧仍有 CPU 开销）。
 *
 * 验收标准：flash_test.c 打印 readback verify [OK]，
 *           read 1024 B 从 ~4.35 ms 降到 ~2.15 ms。
 *           只快不 OK = 数据错位，不能用。
 */

#include "OB90A64M1.h"
#include "dma.h"
#include "spi.h"
#include "bsp_sfud_dma.h"

/* ============================================================
 * 配置
 * ============================================================ */

#ifndef FLASH_DMA_CH_TX
#define FLASH_DMA_CH_TX         OB_PDMA0
#endif
#ifndef FLASH_DMA_CH_RX
#define FLASH_DMA_CH_RX         OB_PDMA1
#endif

/* RAM 侧地址增量 */
#ifndef FLASH_DMA_RAM_INC
#define FLASH_DMA_RAM_INC       DMA_MODE_INC_1
#endif

/* DMA 数据宽度。
 * ⚠️ 实测（PY25Q32HB + 本芯片）：DATAWIDTH=WORD 时，一次 32 位 DR 访问会吃掉
 *    4 个 SPI 帧，但只有 1 个字节落进 buffer —— 结果是有效数据以步长 4 分布
 *    （实测前 16 字节为 03 00 00 00 07 00 00 00 ...，即 d[3] d[7] d[11] ...），
 *    TX 侧同样一个 cycle 发 4 帧，时钟也多发了 4 倍。
 *    虽然 PDMA 手册注明"APB 侧必须 word"，但实测必须用 BYTE 才是 1 cycle = 1 帧。
 *    若 BYTE 下 DMA 报错/超时，再改回 WORD 并配合 4 倍缓冲 + 二次搬运。 */
#ifndef FLASH_DMA_WIDTH
#define FLASH_DMA_WIDTH         DMA_WIDTH_BYTE
#endif

#define PDMA_ENB                (1UL << 0)
#define PDMA_FININTSTS          (1UL << 1)

#define DMA_WAIT_LOOPS          3000000UL

/* TX 哑元：必须 4 字节对齐且为 0（PDMA 要求源地址字对齐） */
static uint32_t s_dma_dummy __attribute__((aligned(4))) = 0UL;

/* ============================================================
 * 内部函数
 * ============================================================ */

/* 等通道完成：等 FININTSTS → 清标志 → 等 ENB 被硬件清零 */
static uint32_t pdma_wait_done(OB_PDMA_Type *pDMA)
{
    uint32_t timeout = DMA_WAIT_LOOPS;

    while ((pDMA->PDMA_CSR & PDMA_FININTSTS) == 0U) {
        if (--timeout == 0U) {
            return 1U;
        }
    }
    pDMA->PDMA_CSR |= PDMA_FININTSTS;           /* W1C */

    timeout = DMA_WAIT_LOOPS;
    while ((pDMA->PDMA_CSR & PDMA_ENB) != 0U) {
        if (--timeout == 0U) {
            return 1U;
        }
    }
    return 0U;
}

static void pdma_stop(OB_PDMA_Type *pDMA)
{
    pDMA->PDMA_CSR &= ~PDMA_ENB;
}

/* SPI->DR → RAM */
static void pdma_cfg_rx(OB_PDMA_Type *pDMA, uint8_t *buf, uint32_t len)
{
    pDMA->PDMA_CSR = 0U;
    pDMA->PDMA_CSR |= (DMA_BUS_APB << 6) | (DMA_BUS_AHB << 7);

    DMA_SourceAddr(pDMA, (uint32_t)&OB_SPI->DR);
    DMA_SourceMode(pDMA, DMA_MODE_INC_0);
    DMA_SourecEvent(pDMA, DMA_EVENT_SPI_Rx);

    DMA_DestinationAddr(pDMA, (uint32_t)buf);
    DMA_DestinationMode(pDMA, FLASH_DMA_RAM_INC);
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, FLASH_DMA_WIDTH);
    DMA_DataLength(pDMA, len);
}

/* RAM → SPI->DR（写 payload） */
static void pdma_cfg_tx(OB_PDMA_Type *pDMA, const uint8_t *buf, uint32_t len)
{
    pDMA->PDMA_CSR = 0U;
    pDMA->PDMA_CSR |= (DMA_BUS_AHB << 6) | (DMA_BUS_APB << 7);

    DMA_SourceAddr(pDMA, (uint32_t)buf);
    DMA_SourceMode(pDMA, FLASH_DMA_RAM_INC);
    DMA_SourecEvent(pDMA, DMA_EVENT_SPI_Tx);

    DMA_DestinationAddr(pDMA, (uint32_t)&OB_SPI->DR);
    DMA_DestinationMode(pDMA, DMA_MODE_INC_0);
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, FLASH_DMA_WIDTH);
    DMA_DataLength(pDMA, len);
}

/* 哑元 → SPI->DR：读 flash 时主模式必须自己出时钟 */
static void pdma_cfg_tx_dummy(OB_PDMA_Type *pDMA, uint32_t len)
{
    pDMA->PDMA_CSR = 0U;
    pDMA->PDMA_CSR |= (DMA_BUS_AHB << 6) | (DMA_BUS_APB << 7);

    DMA_SourceAddr(pDMA, (uint32_t)&s_dma_dummy);
    DMA_SourceMode(pDMA, DMA_MODE_INC_0);
    DMA_SourecEvent(pDMA, DMA_EVENT_SPI_Tx);

    DMA_DestinationAddr(pDMA, (uint32_t)&OB_SPI->DR);
    DMA_DestinationMode(pDMA, DMA_MODE_INC_0);
    DMA_DestinationEvent(pDMA, 0);

    DMA_DataWidth(pDMA, FLASH_DMA_WIDTH);
    DMA_DataLength(pDMA, len);
}

/* ============================================================
 * 对外接口
 * ============================================================ */

uint32_t bsp_sfud_dma_read(uint8_t *buf, uint32_t len)
{
    uint32_t err;

#ifdef SFUD_DMA_RX_ONLY
    /* 调试模式：只开 RX 通道，TX 哑元仍由 CPU 推。
     * 若这个模式数据正确 → 问题出在 TX 哑元通道（两通道抢同一 APB 外设）。 */
    return bsp_sfud_dma_read_rx_only(buf, len);
#endif

    /* ⚠️ 必须先等命令/地址字节全部移位完成，再清 RX FIFO。
     * 否则那些字节还没回灌到 RX FIFO（RNE=0），ClearRxFIFO 空转而过，
     * DMA 启动后它们就成了数据的前几个字节 —— 表现就是整体错位。
     * CPU 轮询时代靠 SPI_WriteFIFO 慢"顺手"等过去了，DMA 一快必然暴露。 */
    spi_wait_idle(OB_SPI);
    SPI_ClearRxFIFO(OB_SPI);

    pdma_stop(FLASH_DMA_CH_RX);
    pdma_stop(FLASH_DMA_CH_TX);
    pdma_cfg_rx(FLASH_DMA_CH_RX, buf, len);
    pdma_cfg_tx_dummy(FLASH_DMA_CH_TX, len);

    OB_SPI->CONDMA_b.DMAE = 1;                  /* 打开 SPI DMA 握手 */

    /* ⚠️ 顺序关键：RX 通道必须先就位，再放 TX 去拉时钟。
     * 反过来（TX 先使能）会立刻把 8 帧 TX FIFO 灌满、开始发帧，
     * 此时 RX 通道还没挂上 DMARREQ，回来的数据无处安放 —— 前几帧直接丢，
     * 表现就是整体错位（readback verify [FAIL] @0: 0x03）。
     * 探针实测：DMATREQ 在通道使能后立刻变 1，而 DMARREQ 只在帧真正回来时才出现。 */
    DMA_Enable(FLASH_DMA_CH_RX);                /* ① RX 先挂上，进入 pending */
    DMA_Enable(FLASH_DMA_CH_TX);                /* ② TX 后启动，开始产帧 */

    err  = pdma_wait_done(FLASH_DMA_CH_RX);
    err |= pdma_wait_done(FLASH_DMA_CH_TX);

    OB_SPI->CONDMA_b.DMAE = 0;
    pdma_stop(FLASH_DMA_CH_RX);
    pdma_stop(FLASH_DMA_CH_TX);

    return (err == 0U) ? len : 0U;
}

/* 调试用：只开 RX 通道，TX 哑元仍由 CPU 推（验证 RX 通道是否打通） */
uint32_t bsp_sfud_dma_read_rx_only(uint8_t *buf, uint32_t len)
{
    uint32_t err;
    uint32_t i;

    spi_wait_idle(OB_SPI);                      /* 同上：先等发完再清 */
    SPI_ClearRxFIFO(OB_SPI);

    pdma_stop(FLASH_DMA_CH_RX);
    pdma_cfg_rx(FLASH_DMA_CH_RX, buf, len);
    OB_SPI->CONDMA_b.DMAE = 1;
    DMA_Enable(FLASH_DMA_CH_RX);

    for (i = 0; i < len; i++) {
        uint32_t t = 100000UL;
        while (!(OB_SPI->SR & SPI_SR_TNF)) {
            if (--t == 0U) break;
        }
        OB_SPI->DR = 0x00U;                     /* CPU 推哑元出时钟 */
    }

    err = pdma_wait_done(FLASH_DMA_CH_RX);

    OB_SPI->CONDMA_b.DMAE = 0;
    pdma_stop(FLASH_DMA_CH_RX);

    return (err == 0U) ? len : 0U;
}

uint32_t bsp_sfud_dma_write(const uint8_t *buf, uint32_t len)
{
    uint32_t err;

    pdma_stop(FLASH_DMA_CH_TX);
    pdma_cfg_tx(FLASH_DMA_CH_TX, buf, len);

    OB_SPI->CONDMA_b.DMAE = 1;
    DMA_Enable(FLASH_DMA_CH_TX);

    err = pdma_wait_done(FLASH_DMA_CH_TX);

    OB_SPI->CONDMA_b.DMAE = 0;
    pdma_stop(FLASH_DMA_CH_TX);

    /* 写期间 RX FIFO 会堆满垃圾字节，清掉并清 overrun 标志 */
    SPI_ClearRxFIFO(OB_SPI);
    OB_SPI->ICR = SPI_INT_RORIM;

    return (err == 0U) ? len : 0U;
}
