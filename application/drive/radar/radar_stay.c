/**
 * @file radar_stay.c
 * @author James Zhang (James Zhang@on-bright.com)
 * @brief 雷达逗留检测逻辑实现
 * @version 0.1
 * @date 2025-12-20
 *
 * @copyright Copyright (c) 2025 广州昂宝电子有限公司
 *
 */
#include "radar_stay.h"
#include "radar_chip_config.h"
#include "module_radar.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "radar_stay"

// 逗留检测上下文（静态全局，仅本文件可见）
static struct {
    uint8_t curr_period_sec;          // 当前周期内已计时秒数
    uint8_t period_high_count;        // 当前周期高电平次数
    uint8_t continuous_valid_periods; // 连续有效周期数
    bool stay_triggered;              // 是否已触发逗留（避免重复触发）
    bool stay_detect_enable;          // 逗留检测使能标志（true-使能，false-禁用）
} radar_stay_ctx = {0};

/**
 * @brief 读取雷达中断引脚电平
 * @return bool true=检测到高电平（有目标），false=低电平（无目标）
 */
static bool radar_hw_get_level(void)
{
    return READ_RADAR_INT();
}

/**
 * @brief 设置雷达逗留检测使能/禁用
 * @param enable true-使能逗留检测，false-禁用逗留检测
 * @note 禁用时会自动重置逗留检测状态
 */
void radar_stay_set_enable(bool enable)
{
    if (radar_stay_ctx.stay_detect_enable != enable) {
        radar_stay_ctx.stay_detect_enable = enable;
        
        if (!enable) {
            radar_stay_reset(); // 禁用时重置所有状态，避免残留
            OB_LOGD(TAG, "Radar stay detection disabled, reset all state");
        } else {
            OB_LOGD(TAG, "Radar stay detection enabled");
        }
    }
}

/**
 * @brief 获取雷达逗留检测使能状态
 * @return bool true-已使能，false-已禁用
 */
bool radar_stay_get_enable_state(void)
{
    return radar_stay_ctx.stay_detect_enable;
}

/**
 * @brief 雷达逗留检测主函数（每秒调用一次）
 */
void radar_stay_scan(void)
{
    // 1. 若逗留检测未使能，直接退出，不执行任何检测逻辑
    if (!radar_stay_ctx.stay_detect_enable) {
        return;
    }

    // 2. 读取当前雷达电平状态
    bool radar_high = radar_hw_get_level();

    // 3. 统计当前周期内高电平次数
    if (radar_high)
        radar_stay_ctx.period_high_count++; // 高电平次数+1

    // 4. 当前周期计时+1（每秒+1）
    radar_stay_ctx.curr_period_sec++;

    if (radar_stay_ctx.continuous_valid_periods == STAY_TOTAL_PERIODS - 1) { // 处于最后一个周期
        if (radar_stay_ctx.period_high_count >= PERIOD_HIGH_THRESHOLD) {
            if (!radar_stay_ctx.stay_triggered) {
                radar_stay_ctx.stay_triggered = true; // 标记已触发，避免重复
                OB_LOGI(TAG, "stay warn!");
            }
        }
    }

    // 5. 判断是否完成一个5秒周期
    if (radar_stay_ctx.curr_period_sec >= PERIOD_DURATION_SEC) {
        // 判断当前周期是否有效（高电平次数≥2）
        bool period_valid = (radar_stay_ctx.period_high_count >= PERIOD_HIGH_THRESHOLD);
        OB_LOGD(TAG, "Period %d ended, high level count: %d | period valid: %s",
                radar_stay_ctx.continuous_valid_periods + 1,
                radar_stay_ctx.period_high_count,
                period_valid ? "Yes" : "No");

        // 6. 周期结束：判断当前周期是否有效
        if (period_valid) {
            // 有效周期：连续有效周期数+1（前3个周期正常累计，第四个周期已提前触发）
            radar_stay_ctx.continuous_valid_periods++;
        } else {
            // 无效周期：重置连续有效周期数、清空逗留标记
            radar_stay_ctx.continuous_valid_periods = 0;
            radar_stay_ctx.stay_triggered = false;
            OB_LOGD(TAG, "stay reset");
        }

        // 8. 重置当前周期计时和高电平计数，进入下一个周期
        radar_stay_ctx.curr_period_sec = 0;
        radar_stay_ctx.period_high_count = 0; // 重置高电平次数
    }
}

/**
 * @brief 重置雷达逗留检测状态
 */
void radar_stay_reset(void)
{
    memset(&radar_stay_ctx, 0, sizeof(radar_stay_ctx));
}

/**
 * @brief 获取当前雷达逗留检测状态
 * @return radar_stay_state_e 当前逗留检测状态
 */
radar_stay_state_e radar_stay_get_state(void)
{
    if (!radar_stay_ctx.stay_detect_enable) {
        return RADAR_STAY_STATE_IDLE;
    }

    if (radar_stay_ctx.stay_triggered) {
        return RADAR_STAY_STATE_TRIGGERED;
    } else if (radar_stay_ctx.continuous_valid_periods > 0) {
        return RADAR_STAY_STATE_DETECTING;
    } else {
        return RADAR_STAY_STATE_IDLE;
    }
}
