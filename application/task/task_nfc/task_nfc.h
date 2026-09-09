/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_nfc.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-11-17
 *  */

#ifndef TASK_NFC__HH
#define TASK_NFC__HH

#include "config.h"
#include "bsp_nfc.h"

typedef bsp_nfc_callback_t task_nfc_callback_t;


/*****************Enum*****************/
typedef enum {
    NFC_STATE_IDLE = 0,         // 空闲状态（未开启天线）
    NFC_STATE_SLEEP,            // 休眠模式（LPCD及ADC检卡）
    NFC_STATE_VERIFY,           // 验证模式
    NFC_STATE_REGISTER,         // 注册模式
    NFC_STATE_CARRIER,          // 单载波模式
    NFC_STATE_FUNCTION,         // 功能卡

}nfc_task_state_e;

/*****************Macro****************/
#define NFC_ENCRYPT_DEFAULT             NFC_ENCRYPT_NONE

#define NFC_FUNC_CARD_SECTOR_ID         (2)
#define NFC_USER_CARD_SECTOR_ID         (15)


/****************Struct****************/
typedef struct{
    const bsp_nfc_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    nfc_attribute_t attr;

}nfc_task_driver_t;


/***************Variable***************/


/***************Function***************/
/**
 * @brief NFC初始化
 * 入口参数: none
 * 出口参数：none
 */
void nfc_task_init(void);
/**
 * @brief NFC循环处理
 * 入口参数: none
 * 出口参数：none
 */
void nfc_task_loop(void);

/**
 * @brief NFC唤醒检查
 * 入口参数: none
 * 出口参数：true, NFC唤醒; false, 无效唤醒
 */
uint16_t nfc_task_is_wake(void);
/**
 * @brief NFC设置任务状态
 * 入口参数: state， 见枚举nfc_task_state_e
 * 出口参数：none
 */
void nfc_task_set_state(uint8_t state);
/**
 * @brief NFC加密配置
 * 入口参数: flag->TRUN_ON,加密; TRUN_OFF,不加密；
 * 出口参数：none
 */
void nfc_task_set_encrypt(uint8_t flag);

uint8_t nfc_task_test_mode(uint8_t handle_id);

/**************************************/

#endif 
