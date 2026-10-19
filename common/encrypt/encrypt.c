/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: encrypt.c
 * Desc: 串口异或加密实现（key = rand ^ K）
 * Version: 2.1.0
 * Date: 2026-10-19
 */

#include "encrypt.h"
#include <stddef.h>
#include "system_timer.h"

#if (ENCRYPT_EN == true)

/*************************Variable*************************/
static uint8_t  s_lock_key;         // 门锁密钥 K_L（0 = 未生成）
static uint32_t s_seed;

/*************************Function*************************/

// 取一个伪随机字节（非密码学强度，满足协议"随机生成"即可）
// 注意：上电时 tick 只有几百，若直接取高位会恒为 0（曾导致 K_L 固定为兜底值），
//       所以先乘大奇数把 tick 打散，再走 LCG。
static uint8_t lock_key_rand_byte(void)
{
    if (0 == s_seed)
    {
        s_seed = (system_inc_time_cnt(0) * 2654435761u) + 0x9E3779B9u;
    }

    s_seed = s_seed * 1664525u + 1013904223u;

    return (uint8_t)(s_seed >> 24);
}

// 上电生成门锁密钥：排除 0x00 / 0xFF
void lock_key_generate(void)
{
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        uint8_t key = lock_key_rand_byte();

        if ((0x00 != key) && (LOCK_KEY_MAX != key))
        {
            s_lock_key = key;
            return;
        }
    }

    s_lock_key = 0x5A;      // 4 次都命中非法值（概率约 1/2^31），兜底
}

// 确保 K_L 已生成：上电早期协议任务可能先于 FSM 收到帧
static void lock_key_ensure(void)
{
    if (LOCK_KEY_NONE == s_lock_key)
    {
        lock_key_generate();
    }
}

uint8_t get_lock_key(void)
{
    return s_lock_key;
}

// 生成 4 字节密钥帧：4 个随机字节，双方约定取下标 1（第 2 个字节）作为真正的门锁密钥
void lock_key_build_frame(uint8_t *frame)
{
    uint8_t i;

    if (NULL == frame)
    {
        return;
    }

    lock_key_ensure();

    for (i = 0; i < LOCK_KEY_FRAME_LEN; i++)
    {
        frame[i] = lock_key_rand_byte();
    }

    frame[LOCK_KEY_FRAME_INDEX] = s_lock_key;
}

// 异或加解密（自反）：实际密钥 = rand ^ key
void data_xor_crypt(uint8_t rand, uint8_t key, uint8_t *data, uint16_t len)
{
    uint8_t k = rand ^ key;
    uint16_t i;

    if (NULL == data)
    {
        return;
    }

    for (i = 0; i < len; i++)
    {
        data[i] ^= k;
    }
}

#endif
