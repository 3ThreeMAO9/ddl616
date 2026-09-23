/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: uart_protocol_blue_mac.h
 * Desc: 蓝牙 MAC 数据共享接口
 * Version: 1.0.0
 * Date: 2026-09-23
 */

#ifndef UART_PROTOCOL_BLUE_MAC_H
#define UART_PROTOCOL_BLUE_MAC_H

#include <stdint.h>

/***********Macro***********/
#define BLUE_MAC_LEN_MAX    (6)

/***********Function***********/
/**
 * @brief 获取蓝牙 MAC 地址指针
 * @return 6 字节 MAC 数组指针（只读）
 */
uint8_t* blue_mac_get(void);

/**
 * @brief 设置蓝牙 MAC 地址
 * @param mac 6 字节 MAC 数组
 * @return 1=成功，0=参数错误
 */
uint8_t blue_mac_set(const uint8_t* mac);

/**
 * @brief 判断 MAC 是否有效（非全 0xFF）
 * @return 1=有效，0=未收到
 */
uint8_t blue_mac_is_valid(void);

#endif /* UART_PROTOCOL_BLUE_MAC_H */
