/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: encrypt.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-04-03
 */

#include "encrypt.h"
#include <string.h>
#include <stddef.h>
#include "system_timer.h"
#include "msg_protocol.h"

#if (ENCRYPT_EN == true)

/*************************Variable*************************/
typedef struct
{
    uint8_t flag;       // 加密
    uint32_t time_out;  // 超时时间
    uint8_t random[16]; // 随机数
} auth_hanlde_t;

static auth_hanlde_t auth_hanlde;
//--------------------------------------------------

void set_auth_data_flag(uint8_t data)
{
    auth_hanlde.flag = data;
}

uint8_t get_auth_data_flag(void)
{
    return auth_hanlde.flag;
}

void set_auth_random(uint8_t *data)
{
    if (data == NULL)
        return;

    memcpy(auth_hanlde.random, data, 16);
    set_auth_data_flag(1);
}

uint8_t *get_auth_random(void)
{
    return auth_hanlde.random;
}

void uart_protocol_auth_poll(void)
{
    if (get_auth_data_flag() == 0)
    {
        if (system_out_time_cnt(auth_hanlde.time_out))
        {
            auth_hanlde.time_out = system_inc_time_cnt(AUTH_DATA_TIME_OUT);
            uart_msg_auth();
        }
    }
}

/**
 * @brief 按你的规则生成 16字节 KEY3
 * @param tsn     帧序列号 TSN
 * @param key3    输出：16字节最终密钥
 */
void generate_key3(uint8_t tsn, uint8_t *key3)
{
    uint8_t i = 0;
    uint8_t key1, key2;
    uint8_t *random = get_auth_random(); // 获取16字节随机数

    if (key3 == NULL || random == NULL)
        return;

    // 1. TSN高8位、低8位取索引（0~15）
    uint8_t h = (tsn >> 4) & 0x0F; // 高4位 → 0~15
    uint8_t l = tsn & 0x0F;        // 低4位 → 0~15

    // 2. 取KEY1、KEY2
    key1 = random[h];
    key2 = random[l];

    // 3. 生成KEY3
    // 奇数位 ^ KEY1
    // 偶数位 ^ KEY2
    for (i = 0; i < 16; i++)
    {
        if (i % 2 == 1) // 奇数下标
        {
            key3[i] = random[i] ^ key1;
        }
        else // 偶数下标
        {
            key3[i] = random[i] ^ key2;
        }
    }
}

/**
 * @brief 数据加密（异或KEY3）
 */
void data_encrypt(uint8_t tsn, uint8_t *data, uint16_t len)
{
    uint8_t key3[16] = {0};
    uint16_t i;

    generate_key3(tsn, key3);

    for (i = 0; i < len; i++)
    {
        data[i] ^= key3[i % 16];
    }
}

/**
 * @brief 数据解密（和加密同一个函数）
 */
void data_decrypt(uint8_t tsn, uint8_t *data, uint16_t len)
{
    data_encrypt(tsn, data, len); // 异或运算 加密解密同一个函数
}

#else

void uart_protocol_auth_poll(void);

#endif
