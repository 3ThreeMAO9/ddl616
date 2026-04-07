/**
 * @file radar_stay.h
 * @author James Zhang (James Zhang@on-bright.com)
 * @brief 雷达逗留检测模块头文件
 * @version 0.1
 * @date 2025-12-20
 *
 * @copyright Copyright (c) 2025 广州昂宝电子有限公司
 *
 */
#ifndef _RADAR_STAY_H_  // 规范的防重复包含宏（加下划线避免冲突）
#define _RADAR_STAY_H_

#include <stdint.h>
#include <stdbool.h>

/***********Macro***********/
// 逗留检测核心配置（对外导出，便于上层调整参数）
#define STAY_TOTAL_PERIODS    4       /* 逗留检测总周期数*/
#define PERIOD_DURATION_SEC   5       /* 单个检测周期时长（秒） */
#define PERIOD_HIGH_THRESHOLD 1       /* 单个周期内判定有效所需的高电平次数阈值 */

/***********Enum***********/
/**
 * @brief 雷达逗留检测状态枚举
 * @note 供上层业务判断当前逗留状态
 */
typedef enum {
    RADAR_STAY_STATE_IDLE = 0,        /* 空闲（未检测到逗留） */
    RADAR_STAY_STATE_DETECTING,       /* 检测中（正在累计有效周期） */
    RADAR_STAY_STATE_TRIGGERED        /* 已触发逗留 */
} radar_stay_state_e;

/***********Struct***********/
/**
 * @brief 雷达逗留检测上下文结构体（可选导出，便于上层调试）
 * @note 如需上层读取状态，可保留；仅内部使用则可隐藏
 */
typedef struct {
    uint8_t curr_period_sec;          /* 当前周期内已计时秒数（0~4） */
    bool period_has_high;             /* 当前周期内是否检测到雷达高电平 */
    uint8_t continuous_valid_periods; /* 连续有效周期数（0~4） */
    bool stay_triggered;              /* 是否已触发逗留（避免重复触发） */
} radar_stay_ctx_t;

/***********Function***********/
/**
 * @brief 雷达逗留检测主函数（每秒调用一次）
 * @param 无
 * @return 无
 */
void radar_stay_scan(void);

/**
 * @brief 重置雷达逗留检测状态
 * @note 手动复位所有检测上下文，恢复到初始状态
 * @param 无
 * @return 无
 */
void radar_stay_reset(void);

/**
 * @brief 获取当前雷达逗留检测状态
 * @note 供上层业务查询检测状态（新增实用接口）
 * @param 无
 * @return radar_stay_state_e 当前逗留检测状态
 */
radar_stay_state_e radar_stay_get_state(void);

/**
 * @brief 设置逗留触发回调函数（可选，解耦业务逻辑）
 * @note 允许上层自定义逗留触发后的行为，替代内部的硬编码回调
 * @param cb 回调函数指针（void (*)(void) 类型）
 * @return 无
 */
void radar_stay_set_trigger_callback(void (*cb)(void));

/**
 * @brief 设置雷达逗留检测使能/禁用
 * @param enable true-使能，false-禁用
 */
void radar_stay_set_enable(bool enable);

/**
 * @brief 获取雷达逗留检测使能状态
 * @return bool true-已使能，false-已禁用
 */
bool radar_stay_get_enable_state(void);

/*****************************/

#endif // _RADAR_STAY_H_
