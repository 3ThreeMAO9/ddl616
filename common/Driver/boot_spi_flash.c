/**
 * @file spi_flash.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-14
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "boot_spi_flash.h"

#define SPI_FLASH_CMD_PP   (0x02)
#define SPI_FLASH_CMD_READ (0x03)
#define SPI_FLASH_CMD_RDID (0x9F)
#define SPI_FLASH_CMD_BE64 (0xD8)
#define SPI_FLASH_CMD_WREN (0x06)
#define SPI_FLASH_CMD_WRSR (0x01)
#define SPI_FLASH_CMD_RDSR (0x05)
#define SPI_FLASH_CMD_SECTOR_ERASE (0x20)

#define SPI_FLASH_SR_BIT_WIP (1 << 0)

#define SPI_FLASH_WAIT_CYCLE (1000000)

#ifdef BUILD_BOOTLOADER
void spi_flash_init(void)
{
    OB_GPIO0->MF0 = 0;
    OB_GPIO0->MF1 = 0;
    OB_GPIO0->MODE &= (uint32_t)~((3 << 4) | (3 << 5) | (3 << 8));
    OB_GPIO0->MODE |= (0 << 4) | (2 << 5) | (2 << 8);

    OB_GPIO2->MF0 = 0;
    OB_GPIO2->MF1 = 0;
    OB_GPIO2->MODE &= (uint32_t)~(3 << 10);
    OB_GPIO2->MODE |= (2 << 10);

    OB_GPIO0->DATA |= (1 << 4) | (1 << 5) | (1 << 8);
    OB_GPIO2->DATA |= (1 << 10);
}

__attribute__((noinline))
void spi_flash_cs_enable(void)
{
    SPI_FLASH_IO_CS_PORT->DATA &= (uint32_t)~SPI_FLASH_IO_CS_PIN;
    spi_flash_clk_clr();
}

__attribute__((noinline))
void spi_flash_cs_disable(void)
{
    spi_flash_clk_set();
    SPI_FLASH_IO_CS_PORT->DATA |= SPI_FLASH_IO_CS_PIN;
}

uint8_t spi_flash_spi_rw(uint8_t tx)
{
    uint8_t i, rx;
    for (i = rx = 0; i < 8; i++)
    {
        spi_flash_clk_clr();

        if(tx & 0x80)
            spi_flash_mosi_set();
        else
            spi_flash_mosi_clr();

        spi_flash_clk_set();

        rx <<= 1;
        if (spi_flash_miso_read())
            rx |= 1;

        tx <<= 1;
    }

    return rx;
}

void spi_flash_tx_cmd_addr(uint8_t cmd, uint32_t addr)
{
    spi_flash_spi_rw(cmd);
    spi_flash_spi_rw((addr >> 16) & 0xFF);
    spi_flash_spi_rw((addr >> 8) & 0xFF);
    spi_flash_spi_rw(addr & 0xFF);
}

void spi_flash_write_enable(void)
{
    spi_flash_cs_enable();
    spi_flash_spi_rw(SPI_FLASH_CMD_WREN);
    spi_flash_cs_disable();

    spi_flash_read_status();
}

void spi_flash_write_status(uint16_t status)
{
    spi_flash_write_enable();

    spi_flash_cs_enable();
    spi_flash_spi_rw(SPI_FLASH_CMD_WRSR);
    spi_flash_spi_rw(status & 0xFF);
    spi_flash_spi_rw(status >> 8);
    spi_flash_cs_disable();

    spi_flash_wait_finish(SPI_FLASH_WAIT_CYCLE);
}

uint8_t spi_flash_read_status(void)
{
    uint8_t sr;
    
    spi_flash_cs_enable();
    spi_flash_spi_rw(SPI_FLASH_CMD_RDSR);
    sr = spi_flash_spi_rw(0xFF);
    spi_flash_cs_disable();

    return sr;
}

uint32_t spi_flash_wait_finish(uint32_t wait_cycle)
{
    uint8_t sr;

    while (wait_cycle--)
    {
        sr = spi_flash_read_status();
        if (!(sr & SPI_FLASH_SR_BIT_WIP))
            return 0;
    }

    return 1;
}

void spi_flash_read(uint32_t addr, uint8_t *buf, uint32_t size)
{
    spi_flash_cs_enable();
    spi_flash_tx_cmd_addr(SPI_FLASH_CMD_READ, addr);

    while (size--)
    {
        *buf = spi_flash_spi_rw(0xFF);
        buf++;
    }

    spi_flash_cs_disable();
}

void spi_flash_64k_erase(uint32_t addr)
{
    spi_flash_soft_protect_disable();
    spi_flash_write_enable();

    spi_flash_cs_enable();
    spi_flash_tx_cmd_addr(SPI_FLASH_CMD_BE64, addr);
    spi_flash_cs_disable();

    spi_flash_wait_finish(SPI_FLASH_WAIT_CYCLE);
    spi_flash_soft_protect_enable();
}
void spi_flash_sector_erase(uint32_t addr)
{
    spi_flash_soft_protect_disable();
    spi_flash_write_enable();

    spi_flash_cs_enable();
    spi_flash_tx_cmd_addr(SPI_FLASH_CMD_SECTOR_ERASE, addr);
    spi_flash_cs_disable();

    spi_flash_wait_finish(SPI_FLASH_WAIT_CYCLE);
    spi_flash_soft_protect_enable();
}

void spi_flash_write(uint32_t addr, uint8_t *buf, uint32_t size)
{
    spi_flash_soft_protect_disable();
    spi_flash_write_enable();

    spi_flash_cs_enable();
    spi_flash_tx_cmd_addr(SPI_FLASH_CMD_PP, addr);

    while (size--)
    {
        if (buf)
        {
            spi_flash_spi_rw(*buf);
            buf++;
        }
        else
        {
            spi_flash_spi_rw(0xFF);
        }
    }

    spi_flash_cs_disable();

    spi_flash_wait_finish(SPI_FLASH_WAIT_CYCLE);
    spi_flash_soft_protect_enable();
}

void spi_flash_soft_protect_enable()
{
    spi_flash_write_status(0x7C);
}

void spi_flash_soft_protect_disable()
{
    spi_flash_write_status(0x00);
}
#else // BUILD_BOOTLOADER
uint32_t spi_flash_read_id(void)
{
    uint8_t rx[3];

    spi_flash_cs_enable();
    
    spi_flash_spi_rw(SPI_FLASH_CMD_RDID);
    rx[0] = spi_flash_spi_rw(0xFF);
    rx[1] = spi_flash_spi_rw(0xFF);
    rx[2] = spi_flash_spi_rw(0xFF);

    spi_flash_cs_disable();

    return (uint32_t)((rx[0] << 16) | (rx[1] << 8) | rx[2]);
}
#endif // BUILD_BOOTLOADER
