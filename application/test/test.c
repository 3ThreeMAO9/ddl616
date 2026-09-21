/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: test.c
 * Desc: 测试总入口
 * Version: 1.0.0
 * Date: 2026-09-21
 */

#include "test.h"

#ifdef TEST_ENABLE

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEBUG
#include "ob_log.h"
#define TAG "test"

void test_run(void)
{
    OB_LOGW(TAG, "========== TEST RUN START ==========");

#ifdef TEST_ACTIVECODE_ENABLE
    OB_LOGW(TAG, "--- [activecode] ---");
    test_activecode_all();
#endif

#ifdef TEST_USER_PROFILE_ENABLE
    OB_LOGW(TAG, "--- [user_profile] ---");
    test_user_profile_all();
#endif


    OB_LOGW(TAG, "========== TEST RUN END ==========");
}

#endif // TEST_ENABLE
