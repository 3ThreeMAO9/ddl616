/**
 * @file ota_helper.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-15
 * 
 * @copyright Copyright (c) 2022 
 */

#if OTA_ENABLE

#include "ota_helper.h"
#include "OB90A64M1.h"
#include "chip_config.h"
#include "ota_file.h"
#include "ota_info.h"
#include "boot_spi_flash.h"
#include "wdt.h"
#include "config.h"
//#include "main.h"
//#include "W25q.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#define OTA_BUFFER_SIZE (256)

static uint8_t gs_ota_running_flag = 0;

static __align(4) uint8_t gs_ota_buffer[OTA_BUFFER_SIZE];

static __ASM void go_boot(void)
{
    IMPORT boot_main

    MOVS  R0, #0
    LDR   R1, [R0]
    MSR   MSP, R1
    LDR   R1, =boot_main
    BX    R1
}

void ota_helper_init(void)
{
    g_boot_buffer = gs_ota_buffer;
    g_boot_buffer_size = sizeof(gs_ota_buffer);

    ota_info_t *info = ota_info_get(1);
    if ((info == NULL) ||
        (info->state == OTA_INFO_STATE_IDLE))
    {
        return;
    }

    ota_helper_result_callback(info);

    info->state = OTA_INFO_STATE_IDLE;
    ota_info_update(info);

#if (Enabled == PRINTF_EN)
    ota_file_t header;
    ota_file_validate(FLASH_APP_BEGIN_ADDR, 1, &header);
    LOG("app version: v%u.%u.%u\r\n", header.version[0],
        header.version[1], header.version[2]);
#endif // PRINTF_EN
}

uint32_t ota_helper_prepare(uint32_t size)
{
    if (size > FLASH_APP_MAX_SIZE)
        return 1;

    gs_ota_running_flag = 1;
    WDT_ReloadCounter();
    //spi_flash_64k_erase(SFLASH_OTA_BEGIN_ADDR);///
		w25q80_sector_erase(SFLASH_OTA_BEGIN_ADDR,_64Kbytes_ERASE);
    WDT_ReloadCounter();

    return 0;
}

uint32_t ota_helper_write(uint32_t offset, uint8_t *buf, uint32_t size)
{

    if (!gs_ota_running_flag)
        return 1;
    else if ((buf == NULL) || !size || ((offset + size) > FLASH_APP_MAX_SIZE))
        return 2;

    WDT_ReloadCounter();
	w25q80_write(buf,SFLASH_OTA_BEGIN_ADDR + offset,size);
    WDT_ReloadCounter();
		
    return 0;
}

uint32_t ota_helper_check_data(uint8_t version[3])
{
			//u8 i;
			ota_file_t header;
			uint32_t sum;
			uint32_t  *ptr, size, rd, cs2_addr;
			//u8 buf[20]={0};
			uint8_t g_boot_buffer_LA[1024];
			uint32_t g_boot_buffer_size_LA=1024;
			WDT_ReloadCounter();
			wait_finsh();
			w25q80_read((u8 *)(&header),SFLASH_OTA_BEGIN_ADDR,sizeof(header));


			if ((header.magic != OTA_FILE_MAGIC)
					|| (header.size < 4))
			{
			LOG("magic false\r\n");
					return 1;
			}


			sum=ota_file_calc_checksum_u16((uint16_t *)(&header.magic),
			offsetof(ota_file_t, data) - sizeof(header.checksum1));

			LOG("sum1=%08x\r\n",sum);
			LOG("sum1=%08x\r\n",sum);
			LOG("header.checksum1=%08x\r\n",header.checksum1);

			if(sum!=header.checksum1)
			{
				LOG("checksum1 false\r\n");
				return 1;		
			}
			sum = 0;
			cs2_addr = SFLASH_OTA_BEGIN_ADDR + sizeof(header);
			size = header.size;
			while (size)
			{
					/* rd shouble be 4 aligned unless the last bytes */
					rd = (size > g_boot_buffer_size_LA) ? g_boot_buffer_size_LA : size;
					w25q80_read(g_boot_buffer_LA,cs2_addr,rd);
					WDT_ReloadCounter();
					ptr = (uint32_t *)(g_boot_buffer_LA);
					sum += ota_file_calc_checksum_u32(ptr, rd);
					size -= rd;
					cs2_addr += rd;
			}
			//LOG("sum2=%08x\r\n",sum);
			//LOG("header.checksum2=%08x\r\n",header.checksum2);
			if (sum != header.checksum2)
			{
					LOG("checksum2 false\r\n");
					return 3;
			}
					LOG("succ\r\n");	
			return 0; 
}

__attribute((noreturn))
void ota_helper_upgrade(void)
{
    while (1)
    {
        ota_info_t *info = ota_info_get(0);
        info->error = OTA_INFO_ERROR_NONE;
        info->state = OTA_INFO_STATE_WAIT;

        ota_info_update(info);

        WDT_Close();

        NVIC_DisableIRQ(SysTick_IRQn);
        IRQn_Type irq = I2C1_IRQn;
        for (irq = I2C1_IRQn; irq <= PWMTM_IRQn; irq++)
            NVIC_DisableIRQ(irq);

        LOG("go_boot\r\n");
        go_boot();
    }
}

uint32_t ota_helper_is_ota_running(void)
{
    return gs_ota_running_flag;
}

void ota_helper_stop_ota(void)
{
    gs_ota_running_flag = 0;
}

#else
void ota_helper_init(void)
{
}

#endif // OTA_ENABLE
