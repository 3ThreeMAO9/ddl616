/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_sfud_dma.h
 * Desc: SPI Flash 读 PDMA 加速
 * Date: 2026-09-29
 *
 * 调用前提：CS 已拉低，命令+地址已由 CPU 发出。
 * 成功返回 len，失败返回 0。
 */

#ifndef BSP_SFUD_DMA_H
#define BSP_SFUD_DMA_H

#include <stdint.h>

uint32_t bsp_sfud_dma_read(uint8_t *buf, uint32_t len);

#endif /* BSP_SFUD_DMA_H */
