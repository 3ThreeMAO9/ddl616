/**
 * @file fmc_flash.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-14
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "fmc_flash.h"

#define FMC_ISP_READ        (0x01 << 0x04)
#define FMC_ISP_PROGRAM     (0x02 << 0x04)
#define FMC_ISP_PAGE_ERASE  (0x04 << 0x04)
#define FMC_ISP_CHIP_RELOAD (0x0F << 0x04)

#define FMC_FLASH_PAGE_SIZE (512)

uint32_t fmc_flash_wait(void)
{
    uint32_t wait;
    wait = 0xFFFFFF;
    while (wait--)
    {
        if (!OB_FMC->ISPTS_b.ISPGO)
            return 0;
    }

    return 1;
}

uint32_t fmc_read(uint32_t addr, uint32_t *buf, uint32_t size)
{
    OB_FMC->ISPCC = FMC_ISP_READ | 0xE8000001;
    OB_FMC->ISPADR = addr;

    while (size)
    {
        OB_FMC->ISPTS = 0xE8000001;
        if (fmc_flash_wait())
            return 1;

        *buf = OB_FMC->ISPDAT;
        buf++;
        size--;
    }

    return 0;
}

uint32_t fmc_flash_erase_pages(uint32_t begin_addr, uint32_t end_addr)
{
    OB_FMC->ISPCC  = FMC_ISP_PAGE_ERASE | 0xE8000001;
    OB_FMC->ISPADR = begin_addr; // auto increase in page size

    do
    {
        OB_FMC->ISPTS  = 0xE8000001;
        if (fmc_flash_wait())
            return 1;

        begin_addr += FMC_FLASH_PAGE_SIZE;
    } while (begin_addr < end_addr);

    return 0;
}

uint32_t fmc_flash_program(uint32_t addr, uint32_t *data, uint32_t size)
{
    OB_FMC->ISPCC  = FMC_ISP_PROGRAM | 0xE8000001;
    OB_FMC->ISPADR = addr; // // auto increase in word size

    do
    {
        OB_FMC->ISPDAT = *data;
        OB_FMC->ISPTS  = 0xE8000001;
        if (fmc_flash_wait())
            return 1;

        data++;
        size -= 4;
    } while (size);

    return 0;
}

uint32_t fmc_flash_reload(void)
{
    OB_FMC->ISPCC  = FMC_ISP_CHIP_RELOAD | 0xE8000001;
    OB_FMC->ISPADR = 0x00;
    OB_FMC->ISPTS  = 0xE8000001;
    return fmc_flash_wait();
}

uint32_t fmc_get_boot_to(void)
{
    uint32_t bit = OB_FMC->BSCSR_b.BS & 0x02;
    bit >>= 1;
    return bit;
}

void fmc_set_boot_to(uint32_t target)
{
    OB_FMC->BSCSR_b.BS = (target == FMC_BOOT_TO_OTA) ? 0x02 : 0x00;
}
