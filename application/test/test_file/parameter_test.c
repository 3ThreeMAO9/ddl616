/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: parameter_test.c
 * Desc: parameter 模块测试
 * Version: 1.0.0
 * Date: 2026-09-21
 */

#include "test.h"

#ifdef TEST_ACTIVECODE_ENABLE

#include "parameter.h"
#include "flash_data.h"
#include "base_attribute.h"
#include "utils.h"
#include "flash_drive.h"
#include "bsp_rom_config.h"
#include "hal_wdt.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "parameter_test"

// ============================================================
// 子测试（static，仅本文件用）
// ============================================================

static void test_activecode_states(void)
{
    OB_LOGI(TAG, "========== Test: Activecode States ==========");

    produce_info_t* info = (produce_info_t*)get_produce_info();
    uint8_t code[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 6};

    // 1. 初始：NONE → 不受限
    clear_activecode();
    OB_LOGI(TAG, "1. NONE: flag=%u, locked=%u (expect 0), activated=%u (expect 0)",
            info->activecode.flag, is_device_locked(), is_activated());

    // 2. 生产工具写哈希 → PENDING
    write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    OB_LOGI(TAG, "2. PENDING: flag=%u, locked=%u (expect 1), activated=%u (expect 0)",
            info->activecode.flag, is_device_locked(), is_activated());

    // 3. 用户输入正确激活码 → VALID
    if (verify_activecode(code, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "3. Verify OK");
    } else {
        OB_LOGE(TAG, "3. Verify FAIL");
    }
    OB_LOGI(TAG, "4. VALID: flag=%u, locked=%u (expect 0), activated=%u (expect 1)",
            info->activecode.flag, is_device_locked(), is_activated());

    // 4. 清除 → NONE
    clear_activecode();
    OB_LOGI(TAG, "5. After clear: flag=%u, locked=%u (expect 0)",
            info->activecode.flag, is_device_locked());

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_activecode_basic(void)
{
    OB_LOGI(TAG, "========== Test: Activecode Basic ==========");

    uint8_t code[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 6};

    // 1. 生产工具写哈希
    clear_activecode();
    if (write_activecode_hash(code, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "1. Write hash OK");
    } else {
        OB_LOGE(TAG, "1. Write hash FAIL");
        return;
    }

    // 2. 检查待激活状态
    OB_LOGI(TAG, "2. locked=%u (expect 1), activated=%u (expect 0)",
            is_device_locked(), is_activated());

    // 3. 用户输入正确激活码
    if (verify_activecode(code, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "3. Verify correct code OK");
    } else {
        OB_LOGE(TAG, "3. Verify correct code FAIL");
    }
    OB_LOGI(TAG, "4. locked=%u (expect 0), activated=%u (expect 1)",
            is_device_locked(), is_activated());

    // 5. 错误激活码（先清空重来，再验证错误码）
    clear_activecode();
    write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    uint8_t wrong[ACTIVECODE_LEN_MAX] = {9, 9, 9, 9, 9, 9};
    if (!verify_activecode(wrong, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "5. Verify wrong code correctly rejected");
    } else {
        OB_LOGE(TAG, "5. Verify wrong code SHOULD fail!");
    }

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_activecode_boundary(void)
{
    OB_LOGI(TAG, "========== Test: Activecode Boundary ==========");

    uint8_t code[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 6};

    // 1. NULL 指针
    if (!write_activecode_hash(NULL, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "1. NULL write correctly rejected");
    }
    if (!verify_activecode(NULL, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "2. NULL verify correctly rejected");
    }

    // 2. 长度错误
    if (!write_activecode_hash(code, 0)) {
        OB_LOGI(TAG, "3. len=0 correctly rejected");
    }
    if (!write_activecode_hash(code, ACTIVECODE_LEN_MAX - 1)) {
        OB_LOGI(TAG, "4. len too short correctly rejected");
    }
    if (!write_activecode_hash(code, ACTIVECODE_LEN_MAX + 1)) {
        OB_LOGI(TAG, "5. len too long correctly rejected");
    }

    // 3. NONE 状态下验证应该失败
    clear_activecode();
    if (!verify_activecode(code, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "6. verify when NONE correctly rejected");
    }

    // 4. VALID 状态下验证应该失败（不是 PENDING）
    write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    verify_activecode(code, ACTIVECODE_LEN_MAX);   // → VALID
    if (!verify_activecode(code, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "7. verify when VALID correctly rejected");
    }

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_activecode_clear(void)
{
    OB_LOGI(TAG, "========== Test: Activecode Clear ==========");

    uint8_t code[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 6};

    // 1. 写哈希 → 待激活
    write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    OB_LOGI(TAG, "1. After hash write: locked=%u (expect 1)", is_device_locked());

    // 2. 激活
    verify_activecode(code, ACTIVECODE_LEN_MAX);
    OB_LOGI(TAG, "2. After verify: activated=%u (expect 1)", is_activated());

    // 3. 清除 → NONE
    clear_activecode();
    OB_LOGI(TAG, "3. After clear: locked=%u (expect 0), activated=%u (expect 0)",
            is_device_locked(), is_activated());

    // 4. 重新写入再激活
    write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    verify_activecode(code, ACTIVECODE_LEN_MAX);
    OB_LOGI(TAG, "4. Re-write and activate: activated=%u (expect 1)", is_activated());

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_activecode_hash_unique(void)
{
    OB_LOGI(TAG, "========== Test: Activecode Hash Unique ==========");

    uint8_t code1[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 6};
    write_activecode_hash(code1, ACTIVECODE_LEN_MAX);

    uint8_t code2[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 7};
    uint8_t code3[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 6, 5};
    uint8_t code4[ACTIVECODE_LEN_MAX] = {2, 1, 3, 4, 5, 6};
    uint8_t code5[ACTIVECODE_LEN_MAX] = {0, 0, 0, 0, 0, 0};
    uint8_t code6[ACTIVECODE_LEN_MAX] = {9, 9, 9, 9, 9, 9};

    if (!verify_activecode(code2, ACTIVECODE_LEN_MAX)) OB_LOGI(TAG, "code2 rejected ✅");
    if (!verify_activecode(code3, ACTIVECODE_LEN_MAX)) OB_LOGI(TAG, "code3 rejected ✅");
    if (!verify_activecode(code4, ACTIVECODE_LEN_MAX)) OB_LOGI(TAG, "code4 rejected ✅");
    if (!verify_activecode(code5, ACTIVECODE_LEN_MAX)) OB_LOGI(TAG, "code5 rejected ✅");
    if (!verify_activecode(code6, ACTIVECODE_LEN_MAX)) OB_LOGI(TAG, "code6 rejected ✅");

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_activecode_persistence(void)
{
    OB_LOGI(TAG, "========== Test: Activecode Persistence ==========");

    produce_info_t* info = (produce_info_t*)get_produce_info();
    uint8_t code[ACTIVECODE_LEN_MAX] = {1, 2, 3, 4, 5, 6};

    // 1. 写哈希 → 待激活
    write_activecode_hash(code, ACTIVECODE_LEN_MAX);
    OB_LOGI(TAG, "1. Written, locked=%u (expect 1)", is_device_locked());

    // 2. 从 Flash 重新读取（模拟掉电重启）
    user_flash_read(PRODUCE_DATA_PAGE_START_ADDR,
                    (uint8_t*)info, sizeof(produce_info_t));

    OB_LOGI(TAG, "2. After read from Flash:");
    OB_LOGI(TAG, "   flag = %u (expect 1)", info->activecode.flag);
    OB_LOGI(TAG, "   hash = 0x%02X%02X%02X%02X",
            info->activecode.hash[3],
            info->activecode.hash[2],
            info->activecode.hash[1],
            info->activecode.hash[0]);
    OB_LOGI(TAG, "   locked=%u (expect 1)", is_device_locked());

    // 3. 验证应该能通过（并转为 VALID）
    if (verify_activecode(code, ACTIVECODE_LEN_MAX)) {
        OB_LOGI(TAG, "3. Verify after re-read OK, activated=%u (expect 1)", is_activated());
    } else {
        OB_LOGE(TAG, "3. Verify after re-read FAIL");
    }

    OB_LOGI(TAG, "========== Test Complete ==========");
}

// ============================================================
// 模块入口（非 static，供 test.c 调用）
// ============================================================

void test_activecode_all(void)
{
    OB_LOGW(TAG, "============ ACTIVECODE TEST START ============");

    clear_feed_dog_cnt();
    test_activecode_states();

    clear_feed_dog_cnt();
    test_activecode_basic();

    clear_feed_dog_cnt();
    test_activecode_boundary();

    clear_feed_dog_cnt();
    test_activecode_clear();

    clear_feed_dog_cnt();
    test_activecode_hash_unique();

    clear_feed_dog_cnt();
    test_activecode_persistence();

    clear_feed_dog_cnt();
    // 测试完清空，避免影响正常使用
    clear_activecode();

    OB_LOGW(TAG, "============ ACTIVECODE TEST END ============");
}

#endif // TEST_ACTIVECODE_ENABLE
