
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_fingerprint.h
 * Desc: 指纹/ 指静脉模块任务层声明及对外接口
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-12
 * Note: 除头文件中声明的对外接口，其他输出结果以回调函数中的事件输出。
 */

#ifndef __TASK_FINGERPRINT_H
#define __TASK_FINGERPRINT_H

#include "bsp_fingerprint.h"
/*
void main (void) {
    const fp_function_attr_t fp_attr = {
        .register_count = 5,
        .repeat = 1,
        .auto_sleep = 1,
        .led_default = FP_LED_BLUE,
    };

    // init
    fp_task_init();

    fp_task_set_attr((void*)(&attr), sizeof(fp_function_attr_t));

    // 切换模式
    fp_task_set_mode(FP_MODE_REGISTER);

    // loop
    while(1) {
        fp_task_loop();
    }
}
*/
/*****************Macro****************/
#define FP_TASK_MODE_DEFAULT                FP_MODE_SLEEP
#define FP_TASK_LED_COLOR_DEFAULT           FP_LED_BLUE

/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    uint8_t enable;
}fp_task_attribute_t;

typedef struct{

    const bsp_fp_drive_io_t *io;
    fp_task_attribute_t attr;

}fp_task_driver_t;


/***************Variable***************/


/***************Function***************/
/**
 * @brief : 指纹模块任务初始化
 * @param : none
 * @return : none
 * @note : 上电调用
 */
void fp_task_init(void);

/**
 * @brief : 指纹模块任务循环处理
 * @param : none
 * @return : none
 * @note : 主循环调用
 */
void fp_task_loop(void);

/**
 * @brief : 指纹模块任务模式设置
 * @param : mode, 枚举fp_mode_e;
 * @return : none
 * @note : 休眠、空闲、验证、注册、按Index删除、验证删除等模式
 */
void fp_task_set_mode(uint8_t mode);

/**
 * @brief : 指纹模块任务参数设置
 * @param : *attr, 结构体fp_function_attr_t;
 * @param : lenth， 参数长度
 * @return : none
 * @note : 设置注册次数、是否查重、无手指时是否休眠切电、默认LED颜色状态
 */
void fp_task_set_attr(void* attr, uint8_t lenth);

#if (FP_ENABLE_DELETE)
/**
 * @brief : 指纹模块任务按index删除
 * @param : params, 结构体fp_delete_params_t（起始index及数量）
 * @return : none
 * @note : 按照index删除单个或多个连续指纹/指静脉
 */
void fp_task_delete_fp(fp_delete_params_t params);      // 按Index删除

void fp_task_reset_all_fp(fp_delete_params_t params);
#endif

#if (FP_ENABLE_LED_CONTROL)
/**
 * @brief : 指纹模块任务控制指纹灯
 * @param : color, 枚举fingerprint_led_e
 * @return : result, 流程中间不允许切换LED状态
 * @note : 关闭或RGBW四色
 */
uint8_t fp_task_control_led(uint8_t color);
#endif
/**
 * @brief : 指纹模块任务唤醒检测
 * @param : none
 * @return : true, 指纹唤醒；
 * @note : 此处未扫描是否有图像
 */
uint8_t fp_task_is_wake(void);

/**
 * @brief : 指纹模块任务忙状态检测
 * @param : none
 * @return : true, 算法IC有供电，busy状态，不可以进休眠；
 * @note : 检测算法IC是否供电
 */
uint8_t fp_task_is_busy(void);

uint8_t fp_task_test_mode(uint8_t handle_id);

/**************************************/

#endif /* __TASK_FINGERPRINT_H */
