
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
#define FP_TASK_MODE_DEFAULT                FP_MODE_VERIFY
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
// 单条按index删除的结果（异步：模块回调置终态），可用 fp_task_delete_result 查询
typedef enum {
    FP_DELETE_NONE = 0,     // 本条刚下发 / 还没删过
    FP_DELETE_BUSY,         // 命令在模块里执行中
    FP_DELETE_OK,           // 模板已删除
    FP_DELETE_FAIL,         // 删除失败（模块报找不到、超时等）
} fp_delete_result_e;

/**
 * @brief : 指纹模块任务按index删除
 * @param : params, 结构体fp_delete_params_t（起始index及数量）
 * @return : none
 * @note : 模块是异步状态机，同一时刻只认一条删除命令，且同模式重复下发会被丢弃
 *         （fp_set_mode_API 的同模式早退）。所以这里只把 index 记进待删表，由
 *         fp_task_loop() 在模块空闲时一条一条下发；调用方不必等待、不必重试
 */
void fp_task_delete_fp(fp_delete_params_t params);      // 按Index删除（入队）

/**
 * @brief : 提示本批删除是"删全部普通用户的指纹"
 * @return : none
 * @note : 普通指纹模板号从 1 开始（0 号固定留给管理指纹，见驱动 read_empty_fp_index），
 *         所以这一批先用一条范围删除（1 ~ 最末模板号）搞定；模块若因中间有空槽而失败，
 *         会自动退回逐条删除（fp_task_delete_fp 的记账不受影响，仍是回退用的工作清单）
 */
void fp_task_delete_all_except_master(void);

/**
 * @brief : 查询最近一条按index删除命令的结果
 * @return : fp_delete_result_e
 */
uint8_t fp_task_delete_result(void);

/**
 * @brief : 查询待删模板是否已全部删完
 * @return : 1, 没有待删模板；0, 还有待删模板（含正在删的那一条）
 * @note : 待删模板全部删完时，指纹任务层会推 EVENT_RESULT_DELETE_KEYS_DONE（Q_HANDLE_SIG）
 */
uint8_t fp_task_delete_is_done(void);

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
