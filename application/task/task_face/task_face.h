/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_face.h
 * Desc: 人脸识别模块任务层声明及对外接口
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 * Note: 除头文件中声明的对外接口，其他输出结果以回调函数中的事件输出。
 */

#ifndef __TASK_FACE_H
#define __TASK_FACE_H

#include "bsp_face.h"

/*****************Macro****************/
#define FACE_TASK_MODE_DEFAULT            FACE_MODE_SLEEP

/*****************Enum*****************/


/****************Struct****************/
// 人脸任务属性结构体
typedef struct{
    uint8_t enable;
}face_task_attribute_t;

// 人脸任务驱动句柄结构体
typedef struct{
    const bsp_face_drive_io_t *io;
    face_task_attribute_t attr;
}face_task_driver_t;

/***************Variable***************/


/***************Function***************/
/**
 * @brief : 人脸模块任务初始化
 * @param : none
 * @return : none
 * @note : 上电调用
 */
void face_task_init(void);

/**
 * @brief : 人脸模块任务循环处理
 * @param : none
 * @return : none
 * @note : 主循环调用
 */
void face_task_loop(void);

/**
 * @brief 读取人脸模块当前工作模式
 * @return 当前模式值（成功）/ 0xFF（失败，如指针为空）
 */
uint8_t face_task_read_mode(void);

/**
 * @brief : 人脸模块任务模式设置
 * @param : mode, 枚举face_mode_e;
 * @return : none
 * @note : 休眠、空闲、验证、注册、按Index删除、验证删除等模式
 */
void face_task_set_mode(uint8_t mode);

/**
 * @brief : 人脸模块任务参数设置
 * @param : *attr, 结构体face_function_attr_t;
 * @param : lenth， 参数长度
 * @return : none
 * @note : 设置注册次数、是否查重、无人脸时是否休眠切电
 */
void face_task_set_attr(void* attr, uint8_t lenth);

/**
 * @brief : 人脸模块任务按index删除
 * @param : params, 结构体face_delete_params_t（起始index及数量）
 * @return : none
 * @note : 按照index删除单个或多个连续人脸模板
 */
void face_task_delete_face(face_delete_params_t params);      // 按Index删除

void face_task_reset_all_face(face_delete_params_t params);

/**************************************/

#endif /* __TASK_FACE_H */
