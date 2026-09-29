/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_sfud_dma.h
 * Desc: SPI Flash 读写用 PDMA 加速（OB90A64M1，SPI = ARM PrimeCell SSP / PL022）
 * Version: 0.1.0 (draft，需实测验证)
 * Date: 2026-09-29
 */

#ifndef BSP_SFUD_DMA_H
#define BSP_SFUD_DMA_H

#include <stdint.h>

/* 在 CS 已拉低、命令/地址已由 CPU 发出的前提下，用 PDMA 搬 payload。
 * 成功返回 len，失败返回 0。
 * 注意：buf 尾部需预留 >=4 字节余量（见 .c 文件头说明）。 */
uint32_t bsp_sfud_dma_read(uint8_t *buf, uint32_t len);
uint32_t bsp_sfud_dma_write(const uint8_t *buf, uint32_t len);

/* 仅供调试：单独验证 RX / TX 通道是否打通 */
uint32_t bsp_sfud_dma_read_rx_only(uint8_t *buf, uint32_t len);

#endif /* BSP_SFUD_DMA_H */
