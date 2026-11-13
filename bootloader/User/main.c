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
        /* 同 OTA 拷贝：size 是数据长度，备份要连 16 字节文件头一起写 */
        flash_fmc_copy_to_spi(APP_OTA_FILE, SFLASH_BACKUP_BEGIN_ADDR,
            gs_ota_context.app.size + sizeof(ota_file_t));
        flag_copied = 1;

        ret = ota_file_validate(SFLASH_BACKUP_BEGIN_ADDR, 0,
            &gs_ota_context.backup);
    }

    /* 头结构只有 16 字节，按字比较即可，避免把 memcmp 拉进 1KB 的 BL1 */
    uint32_t *a = (uint32_t *)&gs_ota_context.app;
    uint32_t *b = (uint32_t *)&gs_ota_context.backup;
    uint32_t i;
    for (i = 0; i < sizeof(gs_ota_context.app) / 4; i++)
    {
        if (a[i] != b[i])
        {
            if (!flag_copied)
                goto do_backup;

            return 1;
        }
    }

    return 0;
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

    /* OTA 区是「16 字节文件头 + 数据」，size 只是数据长度，要把头一起搬 */
    if (flash_spi_copy_to_fmc(SFLASH_OTA_BEGIN_ADDR, APP_OTA_FILE,
        gs_ota_context.ota.size + sizeof(ota_file_t)))
    {
        /* 失败时 state 仍是 WAIT，只置 error：回退到旧固件的 app 会上报失败 */
        gs_ota_context.info->error = OTA_INFO_ERROR_CSUM;
        return;
    }

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

    /* SPI flash 引脚初始化：P3.4~P3.7 必须切成输出，否则 start_ota 读不到数据 */
    spi_flash_init();

    gs_ota_context.info = ota_info_get(1);
#ifdef FORCE_OTA_ENABLE
    if (gs_ota_context.info == NULL)
    {
        gs_ota_context.info = ota_info_get(0);
        gs_ota_context.info->state = OTA_INFO_STATE_WAIT;
        gs_ota_context.info->error = OTA_INFO_ERROR_NONE;
    }
#endif // FORCE_OTA_ENABLE
    if (gs_ota_context.info &&
        (gs_ota_context.info->state == OTA_INFO_STATE_WAIT))
    {
        start_ota();
        ota_info_update(gs_ota_context.info);
    }

    /* 固件区校验失败时，按「备份区 → OTA 区」的顺序找可恢复镜像 */
    static const uint32_t fallback_addr[] =
    {
        SFLASH_BACKUP_BEGIN_ADDR,
        SFLASH_OTA_BEGIN_ADDR,
    };

    uint32_t ret, i;
    while (1)
    {
        ret = ota_file_validate(APP_OTA_FILE, 1, &gs_ota_context.app);
        if (!ret) // app is valid, go app
        {
            App_Go();
        }

        for (i = 0; i < sizeof(fallback_addr) / sizeof(fallback_addr[0]); i++)
        {
            ret = ota_file_validate(fallback_addr[i], 0, &gs_ota_context.backup);
            if (ret)
                continue;

            /* 从备份回退说明新固件没跑起来，把状态改成失败，
             * 避免旧 app 启动后误报升级成功 */
            if ((i == 0) && gs_ota_context.info)
            {
                gs_ota_context.info->state = OTA_INFO_STATE_WAIT;
                gs_ota_context.info->error = OTA_INFO_ERROR_CSUM;
                ota_info_update(gs_ota_context.info);
            }

            flash_spi_copy_to_fmc(fallback_addr[i], APP_OTA_FILE,
                gs_ota_context.backup.size + sizeof(ota_file_t));
            break;
        }

        // Oops, a brick was born!!!
    }
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#endif
