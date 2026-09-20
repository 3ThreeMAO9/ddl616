/**
 * @file bsp_rom_config.h
 * @brief Flash地址配置 - 固定地址存储方案
 */

#ifndef _BSP_ROM_CONFIG_H
#define _BSP_ROM_CONFIG_H

#include "item_config.h"

//flash addr
#define FLASH_ERASE_SIZE                (0x1000)         //flash erase size (4KB)
#define FLASH_USER_DEFINE_START_ADDR    (0x1000)
#define FLASH_USER_DEFINE_END_ADDR      (0x17FFF)

//page cnt
#define SPI_FLASH_SECTOR_NUM            (1)
#define PARAMETER_PAGE_CNT              (1)
#define PRODUCE_DATA_PAGE_CNT           (1)
#define LOCK_LOG_SECTOR_NUM             (4)
#define GC_SECTOR_CNT                   (1)
#define WAKEUP_STATE_CNT                (1)
#define ONE_LINE_KEY_CNT                (1)
#define PROFILE_PAGE_CNT                (1) 

//audio index addr
#define SPI_FLASH_INFO_BEGIN_ADDR       (FLASH_USER_DEFINE_START_ADDR)

//user addr
#define USER_PAGE_START_ADDR            (SPI_FLASH_INFO_BEGIN_ADDR + FLASH_ERASE_SIZE * SPI_FLASH_SECTOR_NUM)
#define USER_PAGE_BACKUP_ADDR           (USER_PAGE_START_ADDR + FLASH_ERASE_SIZE * USER_PAGE_CNT)

//parameter addr
#define PARAMETER_PAGE_START_ADDR       (USER_PAGE_BACKUP_ADDR + FLASH_ERASE_SIZE * USER_PAGE_CNT)
#define PARAMETER_PAGE_BACKUP_ADDR      (PARAMETER_PAGE_START_ADDR + FLASH_ERASE_SIZE * PARAMETER_PAGE_CNT)

//produce data addr
#define PRODUCE_DATA_PAGE_START_ADDR    (PARAMETER_PAGE_BACKUP_ADDR + FLASH_ERASE_SIZE * PARAMETER_PAGE_CNT)

//log data addr
#define LOCK_LOG_FLASH_ADDR             (PRODUCE_DATA_PAGE_START_ADDR + FLASH_ERASE_SIZE * PRODUCE_DATA_PAGE_CNT)

//test data addr
#define GC_SECTOR_ADDR                  (LOCK_LOG_FLASH_ADDR + FLASH_ERASE_SIZE * LOCK_LOG_SECTOR_NUM)

//sleep wake state data addr
#define WAKEUP_STATE_ADDR               (GC_SECTOR_ADDR + FLASH_ERASE_SIZE * GC_SECTOR_CNT)

//blue one line key data addr
#define ONE_LINE_KEY_ADDR               (WAKEUP_STATE_ADDR + FLASH_ERASE_SIZE * WAKEUP_STATE_CNT)

//user profile addr
#define PROFILE_PAGE_START_ADDR         (ONE_LINE_KEY_ADDR + FLASH_ERASE_SIZE * ONE_LINE_KEY_CNT)
#define PROFILE_PAGE_BACKUP_ADDR        (PROFILE_PAGE_START_ADDR + FLASH_ERASE_SIZE * PROFILE_PAGE_CNT)

// ========== 用户数据固定地址布局 ==========
// 每个用户块 32 字节，块0为页标记
// 布局：页标记(1块) + 密码(20块) + 指纹(50块) + 卡片(100块) + 人脸(50块) = 221块

#define USER_BLOCK_SIZE                 (32)

// 各类型用户起始块索引（从块1开始，块0为页标记）
#define BLOCK_INDEX_CODE_START          (1)                                          // 1
#define BLOCK_INDEX_FINGER_START        (BLOCK_INDEX_CODE_START + PERMANENT_USER_CODE_CNT)   // 1+20=21
#define BLOCK_INDEX_CARD_START          (BLOCK_INDEX_FINGER_START + USER_FINGERPRINTS_CNT)   // 21+50=71
#define BLOCK_INDEX_FACE_START          (BLOCK_INDEX_CARD_START + USER_CARD_CNT)             // 71+100=171

// 各类型用户Flash地址宏
#define USER_ADDR_CODE(index)           (USER_PAGE_START_ADDR + (BLOCK_INDEX_CODE_START + (index)) * USER_BLOCK_SIZE)
#define USER_ADDR_FINGER(index)         (USER_PAGE_START_ADDR + (BLOCK_INDEX_FINGER_START + (index)) * USER_BLOCK_SIZE)
#define USER_ADDR_CARD(index)           (USER_PAGE_START_ADDR + (BLOCK_INDEX_CARD_START + (index)) * USER_BLOCK_SIZE)
#define USER_ADDR_FACE(index)           (USER_PAGE_START_ADDR + (BLOCK_INDEX_FACE_START + (index)) * USER_BLOCK_SIZE)

// 各类型用户备份地址宏
#define USER_ADDR_CODE_BACKUP(index)    (USER_PAGE_BACKUP_ADDR + (BLOCK_INDEX_CODE_START + (index)) * USER_BLOCK_SIZE)
#define USER_ADDR_FINGER_BACKUP(index)  (USER_PAGE_BACKUP_ADDR + (BLOCK_INDEX_FINGER_START + (index)) * USER_BLOCK_SIZE)
#define USER_ADDR_CARD_BACKUP(index)    (USER_PAGE_BACKUP_ADDR + (BLOCK_INDEX_CARD_START + (index)) * USER_BLOCK_SIZE)
#define USER_ADDR_FACE_BACKUP(index)    (USER_PAGE_BACKUP_ADDR + (BLOCK_INDEX_FACE_START + (index)) * USER_BLOCK_SIZE)

// 用户数据总块数（包含页标记）
#define USER_TOTAL_BLOCKS               (1 + PERMANENT_USER_CODE_CNT + USER_FINGERPRINTS_CNT + USER_CARD_CNT + USER_FACE_CNT)
#define USER_TOTAL_SIZE                 (USER_TOTAL_BLOCKS * USER_BLOCK_SIZE)
#define USER_PAGE_CNT                   ((USER_TOTAL_SIZE + FLASH_ERASE_SIZE - 1) / FLASH_ERASE_SIZE)

// 各类型在Flash中的偏移
#define USER_OFFSET_CODE_START          (BLOCK_INDEX_CODE_START * USER_BLOCK_SIZE)
#define USER_OFFSET_FINGER_START        (BLOCK_INDEX_FINGER_START * USER_BLOCK_SIZE)
#define USER_OFFSET_CARD_START          (BLOCK_INDEX_CARD_START * USER_BLOCK_SIZE)
#define USER_OFFSET_FACE_START          (BLOCK_INDEX_FACE_START * USER_BLOCK_SIZE)

// ========== 用户档案固定地址布局 ==========
// 块0为页标记，档案从块1开始
// 布局：页标记(1块) + 档案(50条 × 2块) = 101块
// 每条档案 64 字节（2 块 × 32 字节）

#define PROFILE_BLOCK_SIZE              (32)                                            // 32 字节/块
#define PROFILE_BLOCK_PER_USER          (2)                                             // 每条档案 2 块
#define PROFILE_USER_SIZE               (PROFILE_BLOCK_SIZE * PROFILE_BLOCK_PER_USER)   // 64 字节
#define PROFILE_NAME_LEN                (41)                                            // 用户名长度
#define PROFILE_COUNT                   (50)                                            // 用户档案数量

#define PROFILE_DATA_BLOCKS             (PROFILE_COUNT * PROFILE_BLOCK_PER_USER)       // 100 块
#define PROFILE_TOTAL_BLOCKS            (BLOCK_INDEX_CODE_START + PROFILE_DATA_BLOCKS) // 101 块
#define PROFILE_TOTAL_SIZE              (PROFILE_TOTAL_BLOCKS * PROFILE_BLOCK_SIZE)    // 3232 字节

// 单条档案Flash地址宏（跳块0页标记）
#define PROFILE_ADDR(index)             (PROFILE_PAGE_START_ADDR  + (BLOCK_INDEX_CODE_START + (index) * PROFILE_BLOCK_PER_USER) * PROFILE_BLOCK_SIZE)
#define PROFILE_ADDR_BACKUP(index)      (PROFILE_PAGE_BACKUP_ADDR + (BLOCK_INDEX_CODE_START + (index) * PROFILE_BLOCK_PER_USER) * PROFILE_BLOCK_SIZE)

///END SPI FLASH

#endif // _BSP_ROM_CONFIG_H
