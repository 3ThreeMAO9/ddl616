/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: utils.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#ifndef false
#define false                               0
#endif

#ifndef true
#define true                                1
#endif

/***********Macro***********/
#define BCD_CONVERT_HEX(a, b)               (((a & 0x0F) << 4) + (b & 0x0F))

// 大小端转换
#define BIG_LITTLE_SWAP16(X)                ((uint16_t)((X & 0xFF00) >> 8) | (uint16_t)((X & 0x00FF) << 8))

#define BIG_LITTLE_SWAP32(X)                (((uint32_t)(X & 0xFF000000) >> 24) | ((uint32_t)(X & 0x00FF0000) >> 8) | \
                                            ((uint32_t)(X & 0x0000FF00) << 8) | ((uint32_t)(X & 0x000000FF) << 24))

#define SWAP_HIGH_LOW_4BITS(value)          (((value << 4) & 0xF0) | ((value >> 4) & 0x0F))

//
#define UINT8_SWAP_UINT16(X, Y)             (((uint16_t)X << 8) + (uint16_t)Y)

#define MAX(X, Y)                           (X) > (Y) ? (X) : (Y)

#define MIN(X, Y)                           (X) < (Y) ? (X) : (Y)

#define BYTES_TO_UINT32_BE(bytes)           (((uint32_t)(bytes)[0] << 24) |\
                                             ((uint32_t)(bytes)[1] << 16) |\
                                             ((uint32_t)(bytes)[2] << 8) |\
                                             ((uint32_t)(bytes)[3] << 0))

#define BYTES_TO_UINT32_LE(bytes)           (((uint32_t)(bytes)[3] << 24) |\
                                             ((uint32_t)(bytes)[2] << 16) |\
                                             ((uint32_t)(bytes)[1] << 8) |\
                                             ((uint32_t)(bytes)[0] << 0))

#define BYTES_TO_UINT16_BE(bytes)           (((uint16_t)*(bytes) << 8) | ((uint16_t)*(bytes + 1) << 0))

#define BYTES_TO_UINT16_LE(bytes)           (((uint16_t)*(bytes + 1) << 8) | ((uint16_t)*(bytes) << 0))

#define IS_NUMBER_IN_RANGE(X, min, max)             ((X >= min) && (X <= max))


/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/
uint16_t check_sum(uint8_t *buf, uint16_t len);
uint8_t compareArraysWithValue(const uint8_t *data, uint16_t size, const uint8_t value);
uint8_t compare_arrays(const uint8_t *arr1, const uint8_t *arr2, uint16_t len);
uint16_t crc16_ccitt(const uint8_t *data, uint16_t len);

/*****************************/

#endif /* UTILS_H */
