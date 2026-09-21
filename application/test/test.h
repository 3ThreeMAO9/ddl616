/**
 * Copyright (h) 2026 GZ-OB, All rights reserved.
 * File name: test.h
 * Desc: 测试入口
 * Version: 1.0.0
 * Date: 2026-09-21
 */

#ifndef TEST_H
#define TEST_H

/*****************Enum*****************/
// #define TEST_ENABLE                 // 正式版本必须注释掉


#ifdef TEST_ENABLE
    // #define TEST_ACTIVECODE_ENABLE   // 测试激活码功能
    // #define TEST_USER_PROFILE_ENABLE // 测试用户管理



#endif


/*****************Macro****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/
#ifdef TEST_ENABLE

void test_run(void);                    // 测试总入口

#ifdef TEST_ACTIVECODE_ENABLE
void test_activecode_all(void);
#endif

#ifdef TEST_USER_PROFILE_ENABLE
void test_user_profile_all(void);
#endif

#endif // TEST_ENABLE
/**************************************/

#endif // TEST_H
