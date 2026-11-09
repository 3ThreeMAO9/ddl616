/**
 * @file test_point.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-17
 * 
 * @copyright Copyright (c) 2022 广州昂宝电子有限公司
 * 
 */

#ifndef TEST_POINT_H
#define TEST_POINT_H

#include "OB90A64M1.h"

#define TEST_POINT_T3_PORT OB_GPIO2
#define TEST_POINT_T3_PIN  (1 << 14)
#define TEST_POINT_T5_PORT OB_GPIO1
#define TEST_POINT_T5_PIN  (1 << 0)
#define TEST_POINT_T6_PORT OB_GPIO0
#define TEST_POINT_T6_PIN  (1 << 9)

__STATIC_INLINE void test_point_t3_toggle(void)
{
    TEST_POINT_T3_PORT->DATA ^= TEST_POINT_T3_PIN;
}

__STATIC_INLINE void test_point_t5_toggle(void)
{
    TEST_POINT_T5_PORT->DATA ^= TEST_POINT_T5_PIN;
}

__STATIC_INLINE void test_point_t6_toggle(void)
{
    TEST_POINT_T6_PORT->DATA ^= TEST_POINT_T6_PIN;
}

#endif // TEST_POINT_H
