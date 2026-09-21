/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: user_profile_test.c
 * Desc: user 模块 - 用户档案测试
 * Version: 1.0.0
 * Date: 2026-09-21
 */

#include "test.h"

#ifdef TEST_USER_PROFILE_ENABLE

#include "user.h"
#include "flash_data.h"
#include "hal_rtc.h"
#include "timestamp.h"
#include "system_timer.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "user_profile_test"

// ============================================================
// 子测试（static，仅本文件用）
// ============================================================

static void user_profile_clear_all(void)
{
    user_flash_erase(PROFILE_PAGE_BACKUP_ADDR, (PROFILE_PAGE_CNT * FLASH_ERASE_SIZE));
    user_flash_erase(PROFILE_PAGE_START_ADDR, (PROFILE_PAGE_CNT * FLASH_ERASE_SIZE));

#if (Enabled == PRINTF_USER)
    OB_LOGW(TAG, "user_profile_clear_all: all profiles cleared");
#endif
}

static void test_user_profile_basic(void)
{
    OB_LOGI(TAG, "========== Test: Profile Basic ==========");

    // 1. 清空所有档案
    user_profile_clear_all();
    OB_LOGI(TAG, "Cleared, total = %u", user_get_total_cnt());

    // 2. 添加 3 个用户
    user_profile_add(1, "Alice");
    user_profile_add(2, "Bob");
    user_profile_add(3, "Charlie");
    OB_LOGI(TAG, "Added 3 users, total = %u", user_get_total_cnt());

    // 3. 读取用户名
    char name[PROFILE_NAME_LEN];
    if (user_get_name(1, name, sizeof(name))) {
        OB_LOGI(TAG, "user 1 name = %s", name);
    }
    if (user_get_name(2, name, sizeof(name))) {
        OB_LOGI(TAG, "user 2 name = %s", name);
    }

    // 4. 修改 user 2 的名字
    user_profile_t profile;
    if (user_get_profile(2, &profile)) {
        strncpy(profile.user_name, "Bobby", PROFILE_NAME_LEN - 1);
        user_profile_update(2, &profile);
        OB_LOGI(TAG, "Updated user 2 name");
    }
    if (user_get_name(2, name, sizeof(name))) {
        OB_LOGI(TAG, "user 2 new name = %s", name);
    }

    // 5. 删除 user 3
    user_profile_delete(3);
    OB_LOGI(TAG, "Deleted user 3, total = %u", user_get_total_cnt());

    // 6. 验证删除后无法读取
    if (!user_get_name(3, name, sizeof(name))) {
        OB_LOGI(TAG, "user 3 correctly not found");
    }

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_user_profile_period(void)
{
    OB_LOGI(TAG, "========== Test: Profile Period ==========");

    user_profile_clear_all();

    uint32_t now = hal_get_rtc_time();

    // 1. 永久有效用户
    user_profile_add(1, "Permanent");
    OB_LOGI(TAG, "user 1 (permanent) valid = %u", user_is_valid_period(1));

    // 2. 自定义 - 有效期内
    user_profile_add(2, "Valid");
    user_profile_t profile;
    if (user_get_profile(2, &profile)) {
        profile.policy         = 1;
        profile.effective_date = now - 3600;
        profile.expire_date    = now + 3600;
        profile.valid_day      = 0x7F;
        profile.effective_time = 0;
        profile.expire_time    = 86400;
        user_profile_update(2, &profile);
    }
    OB_LOGI(TAG, "user 2 (valid period) valid = %u", user_is_valid_period(2));

    // 3. 自定义 - 已过期
    user_profile_add(3, "Expired");
    if (user_get_profile(3, &profile)) {
        profile.policy         = 1;
        profile.effective_date = now - 7200;
        profile.expire_date    = now - 3600;
        profile.valid_day      = 0x7F;
        profile.effective_time = 0;
        profile.expire_time    = 86400;
        user_profile_update(3, &profile);
    }
    OB_LOGI(TAG, "user 3 (expired) valid = %u", user_is_valid_period(3));

    // 4. 自定义 - 未生效
    user_profile_add(4, "Future");
    if (user_get_profile(4, &profile)) {
        profile.policy         = 1;
        profile.effective_date = now + 3600;
        profile.expire_date    = now + 7200;
        profile.valid_day      = 0x7F;
        profile.effective_time = 0;
        profile.expire_time    = 86400;
        user_profile_update(4, &profile);
    }
    OB_LOGI(TAG, "user 4 (future) valid = %u", user_is_valid_period(4));

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_user_profile_boundary(void)
{
    OB_LOGI(TAG, "========== Test: Profile Boundary ==========");

    user_profile_clear_all();

    // 1. 加满 50 个用户
    uint32_t start = system_ms_get();
    for (uint16_t i = 1; i <= PROFILE_COUNT; i++) {
        clear_feed_dog_cnt();
        if (!user_profile_add(i, NULL)) {
            OB_LOGE(TAG, "Add user %u failed", i);
        }
    }
    uint32_t elapsed = system_ms_get() - start;
    OB_LOGI(TAG, "Added %u users, elapsed = %u ms", PROFILE_COUNT, elapsed);
    OB_LOGI(TAG, "Total = %u (expected %u)", user_get_total_cnt(), PROFILE_COUNT);

    // 2. 再加一个（应该失败，表满）
    if (!user_profile_add(PROFILE_COUNT + 1, "Overflow")) {
        OB_LOGI(TAG, "Overflow correctly rejected");
    }

    // 3. 重复 user_id（应该失败）
    if (!user_profile_add(1, "Duplicate")) {
        OB_LOGI(TAG, "Duplicate correctly rejected");
    }

    // 4. 删除中间一个，再加一个（应该复用空槽）
    user_profile_delete(25);
    OB_LOGI(TAG, "Deleted user 25, total = %u", user_get_total_cnt());

    if (user_profile_add(100, "Reused")) {
        OB_LOGI(TAG, "Added user 100, total = %u", user_get_total_cnt());
    }

    // 5. 遍历打印所有
    user_profile_t profile;
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < PROFILE_COUNT; i++) {
        read_profile(i, &profile);
        if (profile.user_id != 0xFF) {
            cnt++;
        }
    }
    OB_LOGI(TAG, "Traverse count = %u", cnt);

    OB_LOGI(TAG, "========== Test Complete ==========");
}

static void test_user_profile_perf(void)
{
    OB_LOGI(TAG, "========== Test: Profile Performance ==========");

    user_profile_clear_all();

    // 测单条写入
    uint32_t start = system_ms_get();
    user_profile_add(1, "Perf");
    uint32_t elapsed = system_ms_get() - start;
    OB_LOGI(TAG, "Add single profile: %u ms", elapsed);

    // 测单条读取
    user_profile_t profile;
    start = system_ms_get();
    for (int i = 0; i < 100; i++) {
        user_get_profile(1, &profile);
    }
    elapsed = system_ms_get() - start;
    OB_LOGI(TAG, "Read 100 times: %u ms (avg %u us)", elapsed, elapsed * 10);

    // 测查找
    start = system_ms_get();
    for (int i = 0; i < 100; i++) {
        find_profile_idx_by_user_id(1);
    }
    elapsed = system_ms_get() - start;
    OB_LOGI(TAG, "Find 100 times: %u ms", elapsed);

    OB_LOGI(TAG, "========== Test Complete ==========");
}

// ============================================================
// 模块入口（非 static，供 test.c 调用）
// ============================================================

void test_user_profile_all(void)
{
    OB_LOGW(TAG, "============ PROFILE TEST START ============");

    clear_feed_dog_cnt();
    test_user_profile_basic();

    clear_feed_dog_cnt();
    test_user_profile_period();

    clear_feed_dog_cnt();
    test_user_profile_boundary();

    clear_feed_dog_cnt();
    test_user_profile_perf();

    clear_feed_dog_cnt();
    // 测试完清空，避免影响正常使用
    user_profile_clear_all();

    OB_LOGW(TAG, "============ PROFILE TEST END ============");
}

#endif // TEST_USER_PROFILE_ENABLE
