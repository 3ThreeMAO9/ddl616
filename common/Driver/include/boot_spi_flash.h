/**
 * @file spi_flash.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-14
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#include <stdint.h>
#include "OB90A64M1.h"

#define SPI_FLASH_PAGE_SIZE (256)

/* change io should also change related setting in user option */
#define SPI_FLASH_IO_CS_PORT   OB_GPIO0
#define SPI_FLASH_IO_CS_PIN    (1 << 8)
#define SPI_FLASH_IO_CLK_PORT  OB_GPIO2
#define SPI_FLASH_IO_CLK_PIN   (1 << 10)
#define SPI_FLASH_IO_MISO_PORT OB_GPIO0
#define SPI_FLASH_IO_MISO_PIN  (1 << 5)
#define SPI_FLASH_IO_MOSI_PORT OB_GPIO0
#define SPI_FLASH_IO_MOSI_PIN  (1 << 4)

__STATIC_INLINE void spi_flash_clk_set(void)
{
    SPI_FLASH_IO_CLK_PORT->DATA |= SPI_FLASH_IO_CLK_PIN;
}

__STATIC_INLINE void spi_flash_clk_clr(void)
{
    SPI_FLASH_IO_CLK_PORT->DATA &= (uint32_t)~SPI_FLASH_IO_CLK_PIN;
}

__STATIC_INLINE void spi_flash_mosi_set(void)
{
    SPI_FLASH_IO_MOSI_PORT->DATA |= SPI_FLASH_IO_MOSI_PIN;
}

__STATIC_INLINE void spi_flash_mosi_clr(void)
{
    SPI_FLASH_IO_MOSI_PORT->DATA &= (uint32_t)~SPI_FLASH_IO_MOSI_PIN;
}

__STATIC_INLINE uint32_t spi_flash_miso_read(void)
{
    return SPI_FLASH_IO_MISO_PORT->PIN & SPI_FLASH_IO_MISO_PIN;
}

__STATIC_INLINE void soft_reset(void)
{
    __IO uint32_t *pSFR = (__IO uint32_t *)0xE000ED0C;
    *pSFR = 0x05FA0004;
}

void spi_flash_init(void);

void spi_flash_cs_enable(void);
void spi_flash_cs_disable(void);

uint8_t spi_flash_spi_rw(uint8_t tx);
void spi_flash_tx_cmd_addr(uint8_t cmd, uint32_t addr);

void spi_flash_write_status(uint16_t status);
uint8_t spi_flash_read_status(void);
uint32_t spi_flash_wait_finish(uint32_t wait_cycle);
uint32_t spi_flash_read_id(void);
void spi_flash_write_enable(void);
void spi_flash_64k_erase(uint32_t addr);
void spi_flash_sector_erase(uint32_t addr);
void spi_flash_write(uint32_t addr, uint8_t *buf, uint32_t size);
void spi_flash_read(uint32_t addr, uint8_t *buf, uint32_t size);
void spi_flash_soft_protect_enable(void);
void spi_flash_soft_protect_disable(void);

#endif // SPI_FLASH_H
