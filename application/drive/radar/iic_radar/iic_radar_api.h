/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: iic_radar_api.h
 * Desc: I2C雷达模块API层头文件（对外暴露，应用层调用）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef __IIC_RADAR_API_H__
#define __IIC_RADAR_API_H__

#include "stdint.h"
#include "stdbool.h"

/***********Macro***********/

/***********Enum***********/

/***********Struct***********/

/***********Function***********/
void iic_radar_init(void);
void set_distance(unsigned char power_select, unsigned char ls_onoff, unsigned char gain, unsigned int delta);
/*****************************/

#endif /* __IIC_RADAR_API_H__ */
