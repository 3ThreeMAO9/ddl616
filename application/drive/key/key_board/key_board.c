#include "key_board.h"
#include "gtx314l.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "key_board"

/************************Variable************************/
static key_value_t key_board_value;

static const keyboard_handle_t keyboard_handle = {
    .init = touch_init,
    .scan = touch_scan,
    .is_wake = is_touch_wake,
    .sleep = touch_sleep_config,
    .wake = touch_wake_config,
    .parameter = touch_set_sensity,
};

// 获取对应id的按键是否为按下状态
uint8_t read_keyboard_level(uint8_t id) {
    return ((id == key_board_value) ? true : false);
}

// 读取当前按下的按键键值
uint8_t read_keyboard_value(void) {
    return key_board_value;
}

// 数字键盘的休眠配置（GTX314L配置进入休眠模式）
uint8_t keyboard_sleep_config(void) {
    if (NULL == keyboard_handle.sleep) {
        return true;
    }
    return keyboard_handle.sleep();
}

// 数字键盘的唤醒配置（GTX314L配置退出休眠模式/非触摸唤醒会复位触摸IC）
uint8_t keyboard_wake_config(uint8_t keyboard_wake) {
    if (NULL == keyboard_handle.wake) {
        return true;
    }
    return keyboard_handle.wake(keyboard_wake);
}

// 数字键盘扫描（GTX314L扫描是否异常、扫描并获取键值），上层需定时调用，以适配不同的频率需求
uint8_t keyboard_scan(void) {
    if (NULL == keyboard_handle.scan) {
        return false;
    }

    if (keyboard_handle.scan(&key_board_value)) {
        return true;
    }
    return false;
}

// 数字键盘初始化（上电或主控复位调用）
uint8_t keyboard_init(void* parameter) {
    if (NULL == keyboard_handle.init) {
        return false;
    }

    key_board_value = KEY_NULL;
    return keyboard_handle.init(((uint8_t*)(parameter)));
}

// 判断是否为有效的数字键盘唤醒（中断+引脚电平双重判断/ESH中决定了如果非有效键值，则用户无感）
uint8_t is_keyboard_wake(void) {
    if (NULL == keyboard_handle.is_wake) {
        return false;
    }
    return keyboard_handle.is_wake();
}

// 设置数字键盘参数（触摸灵敏度）
uint8_t keyboard_set_parameter(void* packet) {
    if (NULL == keyboard_handle.parameter) {
        return false;
    }
    return keyboard_handle.parameter(packet);
}

