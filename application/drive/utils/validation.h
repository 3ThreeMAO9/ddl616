/**
 * @file validation.h
 * @author Dandjinh Deng (Dandjinh_Deng@on-bright.com)
 * @brief 校验工具库，包括各种校验方法
 * @version 0.1
 * @date 2023-01-04
 * 
 * @copyright Copyright (c) 2023 广州昂宝电子有限公司
 * 
 */

#ifndef _VALIDATION_H
#define _VALIDATION_H

#include <stdint.h>

/**
 * @brief Calculate the checksum
 * 
 * @param[in] buf Pointer to buffer
 * @param[in] size Size of buffer
 * @return uint8_t Checksum result
 */
uint8_t validation_u8_sum_u8(const uint8_t *buf, uint32_t size);

uint16_t validation_u8_sum_u16(const uint8_t *buf, uint32_t size);

uint32_t validation_u8_sum_u32(const uint8_t *buf, uint32_t size);

uint8_t XOR_calc_u8(uint8_t *buff,uint32_t size);

uint8_t LRC_calc_u8(uint8_t *buff,uint32_t size);

uint16_t  bytes_to_uint16(const uint8_t *buff);

uint32_t uint32_to_bytes(uint32_t value,uint8_t bit,uint8_t *outdata);

void get_ble_openkey(uint8_t *input_key,uint8_t *mac, uint16_t userId,uint8_t *outdata);

uint32_t array_to_u32(uint8_t *buff);

void u32_to_array(uint32_t value,uint8_t *outbuff);

uint32_t array_to_u32_decimal_format(uint8_t *buff,uint8_t size);

void array_reverse(uint8_t* array, uint32_t size);

void hex_to_string(uint8_t *source,uint32_t size,uint8_t *target);

uint8_t BCD_to_uint8(uint8_t bcd);

uint8_t uint8_to_BCD(uint8_t value);

uint8_t pw_change_array(uint8_t* input, uint8_t* array, uint8_t size);

uint8_t pw_split_array(uint8_t *input_array, uint8_t *array);

#endif // _VALIDATION_H
