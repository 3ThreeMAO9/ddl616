/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: user_key_test.c
 * Desc: user 模块 - 密码密钥(密码区)存储测试
 *       覆盖：写入 addKeyCode / 读回 getKeyPasswordCode / 校验 isValidKeyCode /
 *             计数 readKeyCnt / 容量上限 isFullKey(USER_CODE_CNT_ENABLE) /
 *             删除 delKeyCode(按密码) 与 delKeyInfo(按块) / 简单密码 isTooSimpleCode /
 *             用户档案联动：user_get_key_cnt / user_get_key_info / user_can_add_key /
 *             user_delete_normal(删用户连带删其名下钥匙) / user_delete_all_normal
 *       注意：addKeyCode 从 0 号槽起找空位；用例里先录管理密码占住 0 号槽（modifyMasterKeyCode 写），
 *             因此新增的普通密码都从 1 号开始，便于断言"总数 = 管理 1 + 普通 N"
 * Version: 1.0.0
 * Date: 2026-10-13
 */

#include "test.h"

#ifdef TEST_USER_KEY_ENABLE

#include <string.h>

#include "user.h"
#include "flash_data.h"
#include "flash_drive.h"        // user_flash_erase
#include "hal_rtc.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "user_key_test"

// ============================================================
// 工具
// ============================================================
#define KEY_TEST_PW_LEN         (6)     // 6 位密码（= USER_CODE_LEN_MIN）
#define KEY_TEST_ADMIN_PW       {9, 7, 5, 3, 1, 9}       // 管理密码样本（相邻差>1，不会被判"太简单"）
#define CODE_SN(key_id)         (BLOCK_INDEX_CODE_START + (key_id))     // key_id -> 内部块号(SN)

// 造一把普通密码：1,3,5,7,9,20+seq（每把唯一，且相邻差都 >1）
static void key_test_make_pw(uint8_t* pw, uint8_t seq)
{
    uint8_t i;

    for (i = 0; i < KEY_TEST_PW_LEN; i++)
    {
        pw[i] = (uint8_t)(1 + i * 2);
    }
    pw[KEY_TEST_PW_LEN - 1] = (uint8_t)(20 + seq);
}

static user_time_t key_test_para(uint8_t user_id)
{
    user_time_t para;

    para.user_id     = user_id;
    para.user_policy = USER_POLICY_PERMANENT;
    para.key_urgent  = KEY_URGENT_NORMAL;
    para.timestamp   = hal_get_rtc_time();

    return para;
}

// 清空密钥区（主 + 备份两页）并刷新计数
static void user_key_clear_all(void)
{
    user_flash_erase(USER_PAGE_BACKUP_ADDR, (USER_PAGE_CNT * FLASH_ERASE_SIZE));
    user_flash_erase(USER_PAGE_START_ADDR, (USER_PAGE_CNT * FLASH_ERASE_SIZE));
    updateKeyCnt();

    OB_LOGW(TAG, "cleared: code cnt = %u, empty = %u",
            readKeyCnt(USER_TYPE_PERMANENT_CODE), isEmptyKey(false));
}

// 每个用例的准备：清空 -> 录管理密码（写 0 号槽）
static void key_test_prepare(void)
{
    const uint8_t admin_pw[KEY_TEST_PW_LEN] = KEY_TEST_ADMIN_PW;

    user_key_clear_all();
    modifyMasterKeyCode((uint8_t*)admin_pw, KEY_TEST_PW_LEN);
    updateKeyCnt();

    OB_LOGW(TAG, "prepare done: code cnt = %u (expect 1，管理密码), isFull = %u",
            readKeyCnt(USER_TYPE_PERMANENT_CODE), isFullKey(USER_TYPE_PERMANENT_CODE));
}

// 清档案区（主 + 备份两页）：联动用例要把"用户"也一起清干净
static void key_test_clear_profile(void)
{
    user_flash_erase(PROFILE_PAGE_BACKUP_ADDR, (PROFILE_PAGE_CNT * FLASH_ERASE_SIZE));
    user_flash_erase(PROFILE_PAGE_START_ADDR, (PROFILE_PAGE_CNT * FLASH_ERASE_SIZE));

    OB_LOGW(TAG, "profile cleared: 档案总数 = %u (expect 0)", user_get_total_cnt());
}

// 联动用例的准备：清档案区 + 清密钥区 -> 录管理密码（顺带建管理员档案 0）
static void key_test_prepare_profile(void)
{
    key_test_clear_profile();
    key_test_prepare();         // 内部：清密钥区 + 录管理密码（0 号槽）+ 刷新计数

    OB_LOGW(TAG, "prepare(profile): 档案总数 = %u (expect 1，管理员), code cnt = %u (expect 1)",
            user_get_total_cnt(), readKeyCnt(USER_TYPE_PERMANENT_CODE));
}

// ============================================================
// 1. 单把密码：写入 / 读回 / 校验
// ============================================================
static void test_user_key_basic(void)
{
    uint8_t pw[KEY_TEST_PW_LEN];
    uint8_t key_id = 0xFF;
    uint8_t kid = 0xFF;
    user_time_t para;
    user_info_t info;

    OB_LOGI(TAG, "========== Test: Key Code Basic ==========");
    key_test_prepare();

    key_test_make_pw(pw, 0);
    para = key_test_para(1);

    // 1. 写入：普通密码 key_id 应为 1（0 是管理密码）
    if (addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
        OB_LOGI(TAG, "1. add OK, key_id = %u (expect 1)", key_id);
    } else {
        OB_LOGE(TAG, "1. add FAIL");
    }

    // 2. 计数：管理 1 + 普通 1 = 2
    OB_LOGI(TAG, "2. code cnt = %u (expect 2)", readKeyCnt(USER_TYPE_PERMANENT_CODE));

    // 3. 读回存储内容（SN = 密码区起始块 + key_id）
    memset(&info, 0, sizeof(info));
    if (getKeyPasswordCode(CODE_SN(key_id), (uint8_t*)&info)) {
        OB_LOGI(TAG, "3. read back: flag=%u type=%u key_id=%u user_id=%u len=%u",
                info.flag, info.key_type, info.key_id, info.parameter.user_id, info.info.password.len);
        if (0 == memcmp(info.info.password.buffer, pw, KEY_TEST_PW_LEN)) {
            OB_LOGI(TAG, "3. password bytes MATCH ✅");
        } else {
            OB_LOGE(TAG, "3. password bytes MISMATCH ❌");
        }
    } else {
        OB_LOGE(TAG, "3. read back FAIL");
    }

    // 4. 校验：普通模式(mode=1) 应命中刚写的 key_id
    kid = 0xFF;
    if (isValidKeyCode(pw, KEY_TEST_PW_LEN, true, 0, 0, &kid)) {
        OB_LOGI(TAG, "4. verify OK, kid = %u (expect %u) %s", kid, key_id, (kid == key_id) ? "✅" : "❌");
    } else {
        OB_LOGE(TAG, "4. verify FAIL ❌");
    }

    // 5. 改一位后应校验失败
    pw[KEY_TEST_PW_LEN - 1]++;
    if (!isValidKeyCode(pw, KEY_TEST_PW_LEN, true, 0, 0, NULL)) {
        OB_LOGI(TAG, "5. wrong password rejected ✅");
    } else {
        OB_LOGE(TAG, "5. wrong password SHOULD fail ❌");
    }
}

// ============================================================
// 2. 多把密码 + 归属用户
// ============================================================
static void test_user_key_multi(void)
{
    uint8_t pw[KEY_TEST_PW_LEN];
    uint8_t key_id;
    user_time_t para;
    user_info_t info;
    uint8_t i;

    OB_LOGI(TAG, "========== Test: Key Code Multi + Owner ==========");
    key_test_prepare();

    // 3 把密码分属用户 1/2/3，key_id 应为 1/2/3
    for (i = 0; i < 3; i++) {
        key_test_make_pw(pw, i);
        para = key_test_para((uint8_t)(1 + i));
        key_id = 0xFF;
        if (addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
            OB_LOGI(TAG, "add #%u: key_id = %u (expect %u), user_id = %u", i + 1, key_id, i + 1, 1 + i);
            memset(&info, 0, sizeof(info));
            getKeyPasswordCode(CODE_SN(key_id), (uint8_t*)&info);
            if (info.parameter.user_id != (1 + i)) {
                OB_LOGE(TAG, "    owner mismatch: got %u", info.parameter.user_id);
            }
        } else {
            OB_LOGE(TAG, "add #%u FAIL", i + 1);
        }
    }

    OB_LOGI(TAG, "code cnt = %u (expect 4: 管理 + 3 把)", readKeyCnt(USER_TYPE_PERMANENT_CODE));

    for (i = 1; i <= 3; i++) {
        OB_LOGI(TAG, "user %u 名下钥匙数 = %u (expect 1)", i, user_get_key_cnt((uint8_t)i));
    }

    // 三把都还能校验通过，且 key_id 对得上
    for (i = 0; i < 3; i++) {
        uint8_t kid = 0xFF;
        key_test_make_pw(pw, i);
        if (isValidKeyCode(pw, KEY_TEST_PW_LEN, true, 0, 0, &kid) && (kid == (uint8_t)(1 + i))) {
            OB_LOGI(TAG, "verify pw#%u OK, kid = %u ✅", i + 1, kid);
        } else {
            OB_LOGE(TAG, "verify pw#%u FAIL, kid = %u ❌", i + 1, kid);
        }
    }
}

// ============================================================
// 3. 容量上限（本工程可用 20 把：管理 1 + 普通 19；flash 预留到 50 槽不参与）
// ============================================================
static void test_user_key_capacity(void)
{
    uint8_t pw[KEY_TEST_PW_LEN];
    uint8_t key_id;
    user_time_t para;
    uint8_t i;
    uint8_t added = 0;

    OB_LOGI(TAG, "========== Test: Key Capacity (USER_CODE_CNT_ENABLE = %u) ==========", USER_CODE_CNT_ENABLE);
    key_test_prepare();

    // 普通密码上限 = 可用槽位 - 1（0 号给管理密码）
    for (i = 0; i < (USER_CODE_CNT_ENABLE - 1); i++) {
        key_test_make_pw(pw, i);
        para = key_test_para((uint8_t)(1 + (i % 49)));
        key_id = 0xFF;
        if (addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
            added++;
        }
    }

    OB_LOGI(TAG, "added %u (expect %u), code cnt = %u, isFull = %u (expect 1)",
            added, USER_CODE_CNT_ENABLE - 1, readKeyCnt(USER_TYPE_PERMANENT_CODE),
            isFullKey(USER_TYPE_PERMANENT_CODE));

    // 再加一把应被拒：预留槽位（21~50）不允许使用
    key_test_make_pw(pw, 99);
    key_id = 0xFF;
    if (!addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
        OB_LOGI(TAG, "extra add rejected ✅ (预留槽位没被占用)");
    } else {
        OB_LOGE(TAG, "extra add SHOULD fail ❌ (key_id = %u)", key_id);
    }
    OB_LOGI(TAG, "final code cnt = %u (expect %u)", readKeyCnt(USER_TYPE_PERMANENT_CODE), USER_CODE_CNT_ENABLE);
}

// ============================================================
// 4. 删除（按密码 / 按块）+ 简单密码判定
// ============================================================
static void test_user_key_delete(void)
{
    uint8_t pw1[KEY_TEST_PW_LEN];
    uint8_t pw2[KEY_TEST_PW_LEN];
    uint8_t kid1 = 0xFF;
    uint8_t kid2 = 0xFF;
    uint8_t kid = 0xFF;
    user_time_t para;
    uint8_t simple[KEY_TEST_PW_LEN]      = {1, 2, 3, 4, 5, 6};      // 相邻差=1 -> 太简单
    uint8_t not_simple[KEY_TEST_PW_LEN]  = {1, 3, 6, 2, 8, 4};      // 相邻差>1

    OB_LOGI(TAG, "========== Test: Key Delete + TooSimple ==========");
    key_test_prepare();

    key_test_make_pw(pw1, 1);
    key_test_make_pw(pw2, 2);
    para = key_test_para(1);

    if (addKeyCode(pw1, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &kid1)
        && addKeyCode(pw2, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &kid2)) {
        OB_LOGI(TAG, "add 2 pw: kid1 = %u, kid2 = %u (expect 1, 2)", kid1, kid2);
    } else {
        OB_LOGE(TAG, "add 2 pw FAIL");
    }
    OB_LOGI(TAG, "cnt before delete = %u (expect 3)", readKeyCnt(USER_TYPE_PERMANENT_CODE));

    // 1. 按密码删第一把
    if (delKeyCode(pw1, KEY_TEST_PW_LEN)) {
        OB_LOGI(TAG, "1. delKeyCode(pw1) OK, cnt = %u (expect 2)", readKeyCnt(USER_TYPE_PERMANENT_CODE));
    } else {
        OB_LOGE(TAG, "1. delKeyCode(pw1) FAIL");
    }
    if (!isValidKeyCode(pw1, KEY_TEST_PW_LEN, true, 0, 0, NULL)
        && isValidKeyCode(pw2, KEY_TEST_PW_LEN, true, 0, 0, &kid)) {
        OB_LOGI(TAG, "1. pw1 rejected / pw2 still valid (kid = %u) ✅", kid);
    } else {
        OB_LOGE(TAG, "1. delete check FAIL ❌");
    }

    // 2. 按块号删第二把
    delKeyInfo(CODE_SN(kid2));
    OB_LOGI(TAG, "2. delKeyInfo(SN=%u) done, cnt = %u (expect 1，只剩管理密码)",
            CODE_SN(kid2), readKeyCnt(USER_TYPE_PERMANENT_CODE));
    if (!isValidKeyCode(pw2, KEY_TEST_PW_LEN, true, 0, 0, NULL)) {
        OB_LOGI(TAG, "2. pw2 rejected ✅");
    } else {
        OB_LOGE(TAG, "2. pw2 SHOULD be gone ❌");
    }

    // 3. 简单密码判定
    OB_LOGI(TAG, "3. isTooSimpleCode(123456) = %u (expect 1)", isTooSimpleCode(simple, KEY_TEST_PW_LEN));
    OB_LOGI(TAG, "3. isTooSimpleCode(136284) = %u (expect 0)", isTooSimpleCode(not_simple, KEY_TEST_PW_LEN));

    // 4. 删干净后应回到空锁
    delKeyInfo(CODE_SN(0));         // 管理密码在 0 号槽
    OB_LOGI(TAG, "4. after clear master: cnt = %u, empty = %u (expect 0, 1)",
            readKeyCnt(USER_TYPE_PERMANENT_CODE), isEmptyKey(false));
}

// ============================================================
// 5. 用户档案 ↔ 钥匙 联动：建档 / 归属统计 / 删用户连带删钥匙
// ============================================================
static void test_user_key_profile_link(void)
{
    uint8_t pw[KEY_TEST_PW_LEN];
    uint8_t key_id = 0xFF;
    uint8_t kid = 0xFF;
    user_time_t para;
    user_key_info_t kinfo;
    user_profile_t profile;
    char name[PROFILE_NAME_LEN];
    uint8_t gone;
    uint8_t i;

    OB_LOGI(TAG, "========== Test: Profile <-> Key Link ==========");
    key_test_prepare_profile();

    // 1. 一个用户挂两把 + 另一个用户挂一把（addKeyCode 会给新归属用户自动建档）
    for (i = 0; i < 2; i++) {
        key_test_make_pw(pw, i);
        para = key_test_para(1);
        key_id = 0xFF;
        if (addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
            OB_LOGI(TAG, "1. user1 pw#%u -> key_id = %u (expect %u)", i + 1, key_id, i + 1);
        } else {
            OB_LOGE(TAG, "1. user1 pw#%u add FAIL", i + 1);
        }
    }
    key_test_make_pw(pw, 2);
    para = key_test_para(2);
    key_id = 0xFF;
    if (addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
        OB_LOGI(TAG, "1. user2 pw#1 -> key_id = %u (expect 3)", key_id);
    } else {
        OB_LOGE(TAG, "1. user2 pw#1 add FAIL");
    }

    // 2. 归属统计：钥匙数按 user_id 归户，档案数含管理员档案 0
    OB_LOGI(TAG, "2. user1 名下钥匙 = %u (expect 2), user2 = %u (expect 1)",
            user_get_key_cnt(1), user_get_key_cnt(2));
    OB_LOGI(TAG, "2. 档案总数 = %u (expect 3: 管理员 + user1 + user2)", user_get_total_cnt());

    // 3. 从档案侧看该用户的钥匙清单
    if (user_get_name(1, name, sizeof(name))) {
        OB_LOGI(TAG, "3. user1 name = %s (expect User1，自动建档的默认名)", name);
    } else {
        OB_LOGE(TAG, "3. user1 name read FAIL");
    }
    if (user_get_key_info(1, 0, &kinfo)) {
        OB_LOGI(TAG, "3. user1 key[0]: type = %u (expect 2=密码), key_id = %u (expect 1), urgent = %u",
                kinfo.key_type, kinfo.key_id, kinfo.key_urgent);
    } else {
        OB_LOGE(TAG, "3. user_get_key_info(1, 0) FAIL");
    }
    OB_LOGI(TAG, "3. user_get_key_info(1, 2) = %u (expect 0，只有两把)",
            user_get_key_info(1, 2, &kinfo));

    // 4. 按用户的钥匙上限（USER_CODE_CNT_NORMAL_MAX）
    OB_LOGI(TAG, "4. user_can_add_key(user1, 密码, 普通) = %u (上限 %u，已 2 把 -> expect 0)",
            user_can_add_key(1, USER_TYPE_PERMANENT_CODE, KEY_URGENT_NORMAL),
            USER_CODE_CNT_NORMAL_MAX);

    // 5. 删用户 1：档案 + 名下两把钥匙一并清掉，用户 2 不受影响
    OB_LOGI(TAG, "5. user_delete_normal(1) = %u (expect %u = OK)",
            (uint8_t)user_delete_normal(1), (uint8_t)USER_DEL_OK);
    OB_LOGI(TAG, "5. user1 名下钥匙 = %u (expect 0), 档案总数 = %u (expect 2)",
            user_get_key_cnt(1), user_get_total_cnt());
    OB_LOGI(TAG, "5. user_get_profile(1) = %u (expect 0), user_get_name(1) = %u (expect 0)",
            user_get_profile(1, &profile), user_get_name(1, name, sizeof(name)));

    gone = 1;
    for (i = 0; i < 2; i++) {
        key_test_make_pw(pw, i);
        if (isValidKeyCode(pw, KEY_TEST_PW_LEN, true, 0, 0, NULL)) {
            gone = 0;
        }
    }
    OB_LOGI(TAG, "5. user1 的两把密码都失效 = %s", gone ? "✅" : "❌");

    key_test_make_pw(pw, 2);
    if (isValidKeyCode(pw, KEY_TEST_PW_LEN, true, 0, 0, &kid)) {
        OB_LOGI(TAG, "5. user2 的密码仍有效 (kid = %u) ✅", kid);
    } else {
        OB_LOGE(TAG, "5. user2 的密码 SHOULD stay ❌");
    }

    // 6. 槽位回收：用户 1 空出来的 1/2 号槽应被复用
    key_test_make_pw(pw, 3);
    para = key_test_para(3);
    key_id = 0xFF;
    if (addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id)) {
        OB_LOGI(TAG, "6. user3 复用空槽 -> key_id = %u (expect 1) %s",
                key_id, (1 == key_id) ? "✅" : "❌");
    } else {
        OB_LOGE(TAG, "6. user3 add FAIL");
    }
    OB_LOGI(TAG, "6. user_get_key_cnt(3) = %u (expect 1), 档案总数 = %u (expect 3)",
            user_get_key_cnt(3), user_get_total_cnt());

    // 7. 边界：管理员不可删 / 不存在 / 重复删 / 只有档案没有钥匙
    OB_LOGI(TAG, "7. user_delete_normal(0) = %u (expect %u = ADMIN)",
            (uint8_t)user_delete_normal(0), (uint8_t)USER_DEL_FAIL_ADMIN);
    OB_LOGI(TAG, "7. user_delete_normal(9) = %u (expect %u = NOT_EXIST)",
            (uint8_t)user_delete_normal(9), (uint8_t)USER_DEL_FAIL_NOT_EXIST);
    OB_LOGI(TAG, "7. user_delete_normal(1) 再删一次 = %u (expect %u = NOT_EXIST)",
            (uint8_t)user_delete_normal(1), (uint8_t)USER_DEL_FAIL_NOT_EXIST);

    user_profile_add(5, "NoKey");
    OB_LOGI(TAG, "7. 只有档案没钥匙: user_delete_normal(5) = %u (expect %u = OK)",
            (uint8_t)user_delete_normal(5), (uint8_t)USER_DEL_OK);
    OB_LOGI(TAG, "7. user_get_profile(5) = %u (expect 0)", user_get_profile(5, &profile));
}

// ============================================================
// 6. 删除全部普通用户（管理员档案与管理密码保留）
// ============================================================
static void test_user_key_delete_all(void)
{
    const uint8_t admin_pw[KEY_TEST_PW_LEN] = KEY_TEST_ADMIN_PW;
    uint8_t pw[KEY_TEST_PW_LEN];
    uint8_t key_id = 0xFF;
    user_time_t para;
    user_profile_t profile;
    user_info_t info;
    uint8_t gone;
    uint8_t i;

    OB_LOGI(TAG, "========== Test: Delete All Normal Users ==========");
    key_test_prepare_profile();

    // 2 个用户 3 把钥匙（前两把归 user1）+ 1 个只有档案的用户
    for (i = 0; i < 3; i++) {
        key_test_make_pw(pw, i);
        para = key_test_para((uint8_t)(1 + (i / 2)));
        key_id = 0xFF;
        addKeyCode(pw, KEY_TEST_PW_LEN, USER_TYPE_PERMANENT_CODE, &para, &key_id);
    }
    user_profile_add(3, "OnlyProfile");

    OB_LOGI(TAG, "before: code cnt = %u (expect 4: 管理 + 3 把), 档案总数 = %u (expect 4)",
            readKeyCnt(USER_TYPE_PERMANENT_CODE), user_get_total_cnt());
    OB_LOGI(TAG, "before: user1/2/3 名下钥匙 = %u/%u/%u (expect 2/1/0)",
            user_get_key_cnt(1), user_get_key_cnt(2), user_get_key_cnt(3));

    OB_LOGI(TAG, "user_delete_all_normal() = %u (expect %u = OK)",
            (uint8_t)user_delete_all_normal(), (uint8_t)USER_DEL_OK);

    OB_LOGI(TAG, "after: code cnt = %u (expect 1: 只剩管理密码)",
            readKeyCnt(USER_TYPE_PERMANENT_CODE));
    OB_LOGI(TAG, "after: 档案总数 = %u (expect 1: 只剩管理员)", user_get_total_cnt());
    OB_LOGI(TAG, "after: user1/2/3 名下钥匙 = %u/%u/%u (expect 0/0/0)",
            user_get_key_cnt(1), user_get_key_cnt(2), user_get_key_cnt(3));

    // 管理员档案 + 管理密码必须原样保留
    OB_LOGI(TAG, "after: user_get_profile(0) = %u (expect 1)", user_get_profile(0, &profile));
    memset(&info, 0, sizeof(info));
    if (getKeyPasswordCode(CODE_SN(0), (uint8_t*)&info) && info.flag
        && (0 == memcmp(info.info.password.buffer, admin_pw, KEY_TEST_PW_LEN))) {
        OB_LOGI(TAG, "after: 管理密码保留 ✅");
    } else {
        OB_LOGE(TAG, "after: 管理密码丢失 ❌");
    }

    gone = 1;
    for (i = 0; i < 3; i++) {
        key_test_make_pw(pw, i);
        if (isValidKeyCode(pw, KEY_TEST_PW_LEN, true, 0, 0, NULL)) {
            gone = 0;
        }
    }
    OB_LOGI(TAG, "after: 三把普通密码都失效 = %s", gone ? "✅" : "❌");

    // 已经没有普通用户了，再删一次应报 EMPTY
    OB_LOGI(TAG, "again: user_delete_all_normal() = %u (expect %u = EMPTY)",
            (uint8_t)user_delete_all_normal(), (uint8_t)USER_DEL_FAIL_EMPTY);
}

// ============================================================
// 入口
// ============================================================
void test_user_key_all(void)
{
    OB_LOGW(TAG, "========== User Key Test START ==========");

    test_user_key_basic();
    test_user_key_multi();
    test_user_key_capacity();
    test_user_key_delete();
    test_user_key_profile_link();
    test_user_key_delete_all();

    OB_LOGW(TAG, "========== User Key Test END ==========");
}

#endif // TEST_USER_KEY_ENABLE
