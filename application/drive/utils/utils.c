/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: utils.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "utils.h"

uint16_t check_sum(uint8_t* buf, uint16_t len)
{
    uint16_t i;
    uint16_t sum = 0;

    for (i = 0; i < len; i++)
    {
        sum += buf[i];
    }

    return sum;
}

uint8_t compareArraysWithValue(const uint8_t *data, uint16_t size, const uint8_t value)
{
    uint16_t i;

    for (i = 0; i < size; i++)
    {
        if (value != data[i])
        {
            return false;
        }
    }
    return true;
}

uint8_t compare_arrays(const uint8_t* arr1, const uint8_t* arr2, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }
    return true;
}

uint16_t crc16_ccitt(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF; // 初始值
    uint8_t i;
    while (len--)
    {
        crc ^= (*data++) << 8; // 高字节异或
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021; // 多项式0x1021
            else
                crc <<= 1;
        }
    }
    return crc; // 最终CRC值（可根据主控需求选择是否反转字节序）
}

uint32_t arraysConvertNumber(uint8_t* data, uint8_t len)
{
	uint8_t i;
	uint16_t number;

#if (Enabled==PRINTF_FUN)
	if (len > 9)
	{
		OB_LOGE(TAG,"Fail: arrays len[%u] is out", len);
	}
#endif

	number = 0;
	for (i = 0; i < len; i++)
	{
		number *= 10;
		number += data[i];
	}

	return number;
}
