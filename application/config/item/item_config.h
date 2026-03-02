/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: item_config.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef _ITEM_CONFIG_H_
#define _ITEM_CONFIG_H_

#include "type_def.h"
#include "base_attribute.h"
/***********Macro***********/
#define NFC_ENABLE                              Enabled

/***********Enum***********/
typedef enum{
    ENTER_SLEEP = 0,
    EXIT_SLEEP,
}sleep_state_e;


/***********Variable***********/

/*************************Struct*************************/
typedef struct
{
    uint16_t client_project;
    uint16_t outside_version;
} device_version_info_t;

/*************************Function*************************/
uint16_t readDeviceClientProjectSn(void);
uint16_t readDeviceVersion(void);
void read_device_version_info(void);

/*************************Project Adaptation*************************/
#if (CLIENT_ITEM_SN_12345 == CLIENT_ITEM_SN)
#include "item_12345.h"
#endif

#define DEVICE_TEST_KEY_BOARD_TAB {KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_NUM_4, KEY_NUM_5, KEY_NUM_6, \
                                   KEY_NUM_7, KEY_NUM_8, KEY_CAN, KEY_NUM_9, KEY_NUM_0, KEY_OK}

#endif /* _ITEM_CONFIG_H_ */
