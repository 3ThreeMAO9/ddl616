/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: flash_test.c
 * Desc: flash 模块 - SFUD 驱动下 SPI Flash 擦/写/读耗时测试
 * Version: 1.0.0
 * Date: 2026-09-29
 */

#include "test.h"

#ifdef TEST_FLASH_ENABLE

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sfud.h"
#include "user.h"
#include "flash_data.h"
#include "hal_rtc.h"
#include "hal_wdt.h"
#include "timestamp.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "flash_test"

// ============================================================
// 配置
// ============================================================

/* 测试地址：需 4KB（扇区）对齐，注意避开 user 档案区（PROFILE_PAGE_xxx） */
#ifndef FLASH_TEST_ADDR
#define FLASH_TEST_ADDR         0x00300000UL
#endif

/* 单次读写长度：1KB */
#define FLASH_TEST_LEN          1024

/* 循环次数：多次取平均，弥补 system_ms_get() 只有 ms 精度 */
#define FLASH_TEST_ERASE_LOOPS  4
#define FLASH_TEST_WRITE_LOOPS  4
#define FLASH_TEST_READ_LOOPS   20

/* 总线探针循环次数（区分"循环开销"与"MMIO 开销"，默认关闭，见文件末） */
#define FLASH_TEST_BUS_LOOPS    200000UL

// ============================================================
// 内部变量
// ============================================================

static sfud_flash *s_flash = NULL;
static uint32_t    s_sector = 4096;                 /* 扇区大小，运行时从芯片信息读取 */
static uint8_t     s_buf[FLASH_TEST_LEN];

/* 平均单次耗时(us) = 总毫秒 * 1000 / 次数 */
static uint32_t avg_us(uint32_t total_ms, uint32_t cnt)
{
    if (cnt == 0) {
        return 0;
    }
    return total_ms * 1000UL / cnt;
}

// ============================================================
// 子测试（static，仅本文件用）
// ============================================================

/* 1. 擦除 1 个扇区的耗时 */
static void test_flash_erase_time(void)
{
    uint32_t i;
    uint32_t t0, ms;

    OB_LOGI(TAG, "---------- erase sector (%u B) ----------", s_sector);

    t0 = system_ms_get();
    for (i = 0; i < FLASH_TEST_ERASE_LOOPS; i++) {
        clear_feed_dog_cnt();
        if (sfud_erase(s_flash, FLASH_TEST_ADDR, s_sector) != SFUD_SUCCESS) {
            OB_LOGE(TAG, "erase failed");
            return;
        }
    }
    ms = system_ms_get() - t0;

    OB_LOGI(TAG, "erase x%u : %u ms, avg %u us/op",
            FLASH_TEST_ERASE_LOOPS, ms, avg_us(ms, FLASH_TEST_ERASE_LOOPS));
}

/* 2. 写 1KB 的耗时 */
static void test_flash_write_time(void)
{
    uint32_t i;
    uint32_t t0, ms;
    uint32_t total = FLASH_TEST_LEN * FLASH_TEST_WRITE_LOOPS;

    OB_LOGI(TAG, "---------- write %u B ----------", FLASH_TEST_LEN);

    for (i = 0; i < FLASH_TEST_LEN; i++) {
        s_buf[i] = (uint8_t)i;
    }

    /* 先擦除（不计入写耗时，NOR 必须先擦后写） */
    clear_feed_dog_cnt();
    if (sfud_erase(s_flash, FLASH_TEST_ADDR, s_sector) != SFUD_SUCCESS) {
        OB_LOGE(TAG, "erase before write failed");
        return;
    }

    t0 = system_ms_get();
    for (i = 0; i < FLASH_TEST_WRITE_LOOPS; i++) {
        clear_feed_dog_cnt();
        if (sfud_write(s_flash, FLASH_TEST_ADDR + i * FLASH_TEST_LEN,
                       FLASH_TEST_LEN, s_buf) != SFUD_SUCCESS) {
            OB_LOGE(TAG, "write failed");
            return;
        }
    }
    ms = system_ms_get() - t0;

    OB_LOGI(TAG, "write %u B x%u : %u ms, avg %u us/op, %u KB/s",
            FLASH_TEST_LEN, FLASH_TEST_WRITE_LOOPS, ms,
            avg_us(ms, FLASH_TEST_WRITE_LOOPS), total / ((ms > 0) ? ms : 1));

    /* 读回校验：确认读到的是真实数据（共享 FIFO 踩坑时这里会立刻报错） */
    memset(s_buf, 0x00, FLASH_TEST_LEN);
    if (sfud_read(s_flash, FLASH_TEST_ADDR, FLASH_TEST_LEN, s_buf) != SFUD_SUCCESS) {
        OB_LOGE(TAG, "readback failed");
        return;
    }
    for (i = 0; i < FLASH_TEST_LEN; i++) {
        if (s_buf[i] != (uint8_t)i) {
            uint32_t k;
            char     dump[3 * 16 + 1];
            uint32_t pos = 0;

            OB_LOGE(TAG, "readback verify [FAIL] @%u: got 0x%02X, want 0x%02X",
                    i, s_buf[i], (uint8_t)i);

            /* 打印前 16 字节，用于分辨错位的性质：
             * - 出现 03 00 30 00  → 读到的是自己发出去的命令+地址（TX/RX 串了）
             * - 出现 03 04 05 06  → 整体偏移（丢了前 3 个字节）
             * - 全 FF / 全 00     → 根本没读到数据 */
            for (k = 0; k < 16; k++) {
                pos += (uint32_t)snprintf(&dump[pos], sizeof(dump) - pos,
                                          "%02X ", s_buf[k]);
            }
            OB_LOGE(TAG, "first 16 B: %s", dump);
            return;
        }
    }
    OB_LOGI(TAG, "readback verify [OK]");
}

/* 3. 读 1KB 的耗时 */
static void test_flash_read_time(void)
{
    uint32_t i;
    uint32_t t0, ms;
    uint32_t total = FLASH_TEST_LEN * FLASH_TEST_READ_LOOPS;

    OB_LOGI(TAG, "---------- read %u B ----------", FLASH_TEST_LEN);

    t0 = system_ms_get();
    for (i = 0; i < FLASH_TEST_READ_LOOPS; i++) {
        if (sfud_read(s_flash, FLASH_TEST_ADDR, FLASH_TEST_LEN, s_buf) != SFUD_SUCCESS) {
            OB_LOGE(TAG, "read failed");
            return;
        }
    }
    ms = system_ms_get() - t0;

    OB_LOGI(TAG, "read %u B x%u : %u ms, avg %u us/op, %u KB/s",
            FLASH_TEST_LEN, FLASH_TEST_READ_LOOPS, ms,
            avg_us(ms, FLASH_TEST_READ_LOOPS), total / ((ms > 0) ? ms : 1));
}

//static void test_flash_bus_probe(void)
//{
//    volatile uint32_t sink = 0;
//    volatile uint32_t ram  = 0;
//    uint32_t i, t0, ms_ram, ms_io;

//    OB_LOGI(TAG, "---------- bus probe (x%u) ----------", FLASH_TEST_BUS_LOOPS);

//    /* A. RAM 基线：同样的循环结构，但只碰 RAM 变量 */
//    t0 = system_ms_get();
//    for (i = 0; i < FLASH_TEST_BUS_LOOPS; i++) {
//        sink += ram;
//    }
//    ms_ram = system_ms_get() - t0;

//    /* B. MMIO：同样的循环，每次多读一次 SPI 状态寄存器 */
//    t0 = system_ms_get();
//    for (i = 0; i < FLASH_TEST_BUS_LOOPS; i++) {
//        sink += OB_SPI->SR;
//    }
//    ms_io = system_ms_get() - t0;

//    OB_LOGI(TAG, "ram loop : %u ms -> %u ns/op",
//            ms_ram, ms_ram * 1000000UL / FLASH_TEST_BUS_LOOPS);
//    OB_LOGI(TAG, "SR  loop : %u ms -> %u ns/op",
//            ms_io, ms_io * 1000000UL / FLASH_TEST_BUS_LOOPS);
//    if (ms_io > ms_ram) {
//        OB_LOGI(TAG, "MMIO cost: %u ns/access",
//                (ms_io - ms_ram) * 1000000UL / FLASH_TEST_BUS_LOOPS);
//    } else {
//        OB_LOGI(TAG, "MMIO cost: ~0 ns/access (loop overhead dominates)");
//    }

//    (void)sink;
//    (void)ram;
//}

// ============================================================
// 模块入口（非 static，供 test.c 调用）
// ============================================================

void test_flash_all(void)
{
    OB_LOGW(TAG, "============ FLASH TEST START ============");

    clear_feed_dog_cnt();

    /* 1. 初始化 SFUD（系统若已初始化，重复调用无副作用） */
    if (sfud_init() != SFUD_SUCCESS) {
        OB_LOGE(TAG, "sfud_init failed");
        OB_LOGW(TAG, "============ FLASH TEST END ============");
        return;
    }

    /* 2. 取设备：默认设备表第 0 个，多片时改索引 */
    s_flash = sfud_get_device(0);
    if (s_flash == NULL) {
        OB_LOGE(TAG, "get sfud device failed");
        OB_LOGW(TAG, "============ FLASH TEST END ============");
        return;
    }

    OB_LOGI(TAG, "device: %s, cap %u KB", s_flash->name, s_flash->chip.capacity / 1024);
    OB_LOGI(TAG, "test addr: 0x%08X", FLASH_TEST_ADDR);

    /* 3. 三项计时 */
    test_flash_erase_time();
    clear_feed_dog_cnt();

    test_flash_write_time();
    clear_feed_dog_cnt();

    test_flash_read_time();

    // clear_feed_dog_cnt();
    // test_flash_bus_probe();

    /* 4. 清理：擦除测试扇区，避免影响正常使用 */
    clear_feed_dog_cnt();
    if (sfud_erase(s_flash, FLASH_TEST_ADDR, s_sector) != SFUD_SUCCESS) {
        OB_LOGE(TAG, "cleanup erase failed");
    }

    OB_LOGW(TAG, "============ FLASH TEST END ============");
}

#endif // TEST_FLASH_ENABLE
