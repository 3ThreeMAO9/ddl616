/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: encrypt.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-04-03
 */

#ifndef _ENCRYPT_HH
#define _ENCRYPT_HH

#include <stdint.h>

#define ENCRYPT_EN                          (false)     // 加密通信

/*************************Macro*************************/
#define AUTH_DATA_TIME_OUT                  (1000)   // 加密数据 ms

/*************************Enum*************************/

/*************************Struct*************************/

/*************************Variable*************************/

/*************************Function*************************/
void set_auth_data_flag(uint8_t data);
uint8_t get_auth_data_flag(void);
void uart_protocol_auth_poll(void);
void set_auth_random(uint8_t *data);
uint8_t *get_auth_random(void);
void data_encrypt(uint8_t tsn, uint8_t *data, uint16_t len);
void data_decrypt(uint8_t tsn, uint8_t *data, uint16_t len);
/**********************************************************/

#endif

