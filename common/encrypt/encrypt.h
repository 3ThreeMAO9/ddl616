/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: encrypt.h
 * Desc: 串口异或加密（3 个 1 字节密钥：出厂/门锁/随帧）
 * Version: 2.0.0
 * Date: 2026-10-19
 */

#ifndef _ENCRYPT_HH
#define _ENCRYPT_HH

#include <stdint.h>

#define ENCRYPT_EN                          (true)      // 加密通信

/*************************Macro*************************/
#define LOCK_KEY_FACTORY                    (0xBA)      // 出厂密钥 K_F（双方固件必须一致）
#define LOCK_KEY_NONE                       (0x00)      // 门锁密钥未生成
#define LOCK_KEY_MAX                        (0xFF)

#define LOCK_KEY_FRAME_LEN                  (4)         // 密钥发送 payload 长度：随机 4 字节
#define LOCK_KEY_FRAME_INDEX                (1)         // 双方约定：第 2 个字节（下标 1）才是真正的门锁密钥

/*************************Enum*************************/

/*************************Function*************************/
// 上电生成门锁密钥 K_L（排除 0x00 / 0xFF）
void    lock_key_generate(void);
// 读门锁密钥 K_L（0 表示未生成）
uint8_t get_lock_key(void);
// 生成 4 字节密钥帧：4 个随机字节，下标 LOCK_KEY_FRAME_INDEX 写入当前 K_L
void    lock_key_build_frame(uint8_t *frame);
// 异或加解密（自反，收发共用）：实际密钥 = rand ^ key
void    data_xor_crypt(uint8_t rand, uint8_t key, uint8_t *data, uint16_t len);

/**********************************************************/

#endif
