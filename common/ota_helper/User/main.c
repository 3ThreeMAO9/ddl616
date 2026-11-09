/**
 * @file main.c
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-31
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#include "OB90A64M1.h"
#include "main.h"
#include "chip_config.h"
#include "ota_info.h"
#include "flash_util.h"
#include "boot_spi_flash.h"
#include "app_entry.h"

#include <stdio.h>
#include <string.h>

// #define FORCE_OTA_ENABLE

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#define APP_OTA_FILE ((uint32_t)&g_app_file)

extern void App_Go(void);

void boot_main(void);
uint32_t start_backup(void);
void start_ota(void);

struct ota_context_t_def
{
    ota_info_t *info;

    ota_file_t app;
    ota_file_t ota;
    ota_file_t backup;
};
typedef struct ota_context_t_def ota_context_t;

static ota_context_t gs_ota_context;

uint32_t start_backup(void)
{
    uint32_t flag_copied = 0;

    uint32_t ret = ota_file_validate(SFLASH_BACKUP_BEGIN_ADDR, 0,
        &gs_ota_context.backup);
    if (ret) // backup is invalid
    {
do_backup:
        flash_fmc_copy_to_spi(APP_OTA_FILE, SFLASH_BACKUP_BEGIN_ADDR,
            gs_ota_context.app.size);
        flag_copied = 1;

        ret = ota_file_validate(SFLASH_BACKUP_BEGIN_ADDR, 0,
            &gs_ota_context.backup);
    }

    if (!memcmp(&gs_ota_context.app, &gs_ota_context.backup,
        sizeof(gs_ota_context.app)))
    {
        return 0;
    }

    if (!flag_copied)
        goto do_backup;

    return 1;
}

void start_ota(void)
{
    uint32_t ret;
    ret = ota_file_validate(SFLASH_OTA_BEGIN_ADDR, 0,
        &gs_ota_context.ota);
    if (ret)
    {
        gs_ota_context.info->error = OTA_INFO_ERROR_CSUM;
        return;
    }

    ret = ota_file_validate(APP_OTA_FILE, 1,
        &gs_ota_context.app);
    if (!ret) // app is valid, go backup
    {
        gs_ota_context.info->state = OTA_INFO_STATE_BKUP;
        if (start_backup())
        {
            /* backup failed */
            gs_ota_context.info->error = OTA_INFO_ERROR_BKUP;
            return;
        }
    }

    gs_ota_context.info->state = OTA_INFO_STATE_OTA;
    flash_spi_copy_to_fmc(SFLASH_OTA_BEGIN_ADDR, APP_OTA_FILE,
        gs_ota_context.ota.size);

    gs_ota_context.info->state = OTA_INFO_STATE_DONE;
    gs_ota_context.info->error = OTA_INFO_ERROR_NONE;
}

//static void uart_init(void)
//{
//    // rx
//    OB_GPIO2->MF1_b.PORT_11 = 4;
//    OB_GPIO2->MODE_b.MODEPIN11 = 3;
//    // tx
//    OB_GPIO0->MF0_b.PORT_3 = 4;
//    OB_GPIO0->MODE_b.MODEPIN3 = 2;

//    // OB_UART1->UDL = 416; // fsclk = 48M
//    OB_UART1->UDL = 208; // fsclk = 24M
//    OB_UART1->ULCR = (3 << 0) | (0 << 3) | (0 << 4);
//}

__attribute__((section("BL2")))
void uart_write(char *str, uint32_t size)
{
    while (size)
    {
        OB_UART1->UTHR = (uint8_t)*str;
        str++;
        size--;

        while (!(OB_UART1->ULSR & 0x40))
            ;
    }
}

static uint8_t gs_main_buffer[4096] __attribute__((aligned (16)));
__NO_RETURN void boot_main(void)
{
    // spi_flash_soft_protect_enable();

    // uart_init();

    g_boot_buffer = gs_main_buffer;
    g_boot_buffer_size = sizeof(gs_main_buffer);

    gs_ota_context.info = ota_info_get(1);
#ifdef FORCE_OTA_ENABLE
    if (gs_ota_context.info == NULL)
    {
        gs_ota_context.info = ota_info_get(0);
        gs_ota_context.info->state = OTA_INFO_STATE_WAIT;
    }
#endif // FORCE_OTA_ENABLE
    if (gs_ota_context.info)
    {
        if (gs_ota_context.info->state == OTA_INFO_STATE_WAIT)
        {
            start_ota();
            ota_info_update(gs_ota_context.info);
        }
    }

    uint32_t ret;
    while (1)
    {
        ret = ota_file_validate(APP_OTA_FILE, 1, &gs_ota_context.app);
        if (!ret) // app is valid, go app
        {
            App_Go();
        }

        ret = ota_file_validate(SFLASH_BACKUP_BEGIN_ADDR, 0, &gs_ota_context.backup);
        if (!ret) // backup is valid, go restore
        {
            flash_spi_copy_to_fmc(SFLASH_BACKUP_BEGIN_ADDR, APP_OTA_FILE,
                gs_ota_context.backup.size);
            continue;
        }

        ret = ota_file_validate(SFLASH_OTA_BEGIN_ADDR, 0, &gs_ota_context.ota);
        if (!ret) // ota is valid, copy ota
        {
            flash_spi_copy_to_fmc(SFLASH_OTA_BEGIN_ADDR, APP_OTA_FILE,
                gs_ota_context.ota.size);
            continue;
        }

        // Oops, a brick was born!!!
    }
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif
