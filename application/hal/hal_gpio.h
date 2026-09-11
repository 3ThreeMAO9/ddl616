/**
 * @file    hal_gpio.h
 * @brief   Hardware Abstraction Layer (HAL) for General Purpose I/O (GPIO)
 * @version V1.0.0
 * @author  james_zhang
 * @date    2025-02-23
 * @details 该模块提供与硬件无关的GPIO操作接口，支持以下功能：
 *          - 初始化GPIO引脚（HAL_GPIO_Init）
 *          - 设置GPIO输出电平（HAL_GPIO_Write）
 *          - 读取GPIO输入电平（HAL_GPIO_Read）
 *          - 翻转GPIO输出电平（HAL_GPIO_Toggle）
 *          - 使能/禁用GPIO中断（HAL_GPIO_EnableIRQ/HAL_GPIO_DisableIRQ）
 *          - 设置中断回调函数（HAL_GPIO_SetIrqCallback）
 */
#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "config.h"
#include "gpio.h"
/* 公共类型定义 */
typedef void (*GPIOCallback)(void);

/**
 * @brief GPIO工作模式定义
 */
typedef enum {
    HAL_GPIO_MODE_INPUT,        ///< 输入模式（浮空）
    HAL_GPIO_MODE_INPUT_PULLUP, ///< 输入模式（上拉）
    HAL_GPIO_MODE_OUTPUT_PP,    ///< 推挽输出模式
    HAL_GPIO_MODE_OUTPUT_OD,    ///< 开漏输出模式
    HAL_GPIO_MODE_ANALOG,       ///< 模拟模式（用于ADC等）
} HalGpioMode;

/**
 * @brief GPIO上下拉配置
 */
typedef enum {
    HAL_GPIO_PULL_NONE,     ///< 无上下拉
    HAL_GPIO_PULL_HIGH,     ///< 高电平
    HAL_GPIO_PULL_LOW       ///< 低电平
} HalGpioPull;

/**
 * @brief GPIO中断触发类型
 */
typedef enum {
    HAL_GPIO_IRQ_RISING,        ///< 上升沿触发（电平由低变高）
    HAL_GPIO_IRQ_FALLING,       ///< 下降沿触发（电平由高变低）
    HAL_GPIO_IRQ_BOTH,          ///< 双边沿触发（任意电平变化）
    HAL_GPIO_IRQ_HIGH_LEVEL,    ///< 高电平触发（持续检测高电平）
    HAL_GPIO_IRQ_LOW_LEVEL,     ///< 低电平触发（持续检测低电平）
    HAL_GPIO_IRQ_NONE           ///< 无触发（默认/无效状态）
} HalGpioIrqType;

/**
 * @brief GPIO端口号
 */
typedef enum {
    HAL_GPIO_PORT0,
    HAL_GPIO_PORT1,
    HAL_GPIO_PORT2,
    HAL_GPIO_PORT3,
    HAL_GPIO_PORT_INVALID,
} HalGpioPort;

/**
 * @brief GPIO引脚号
 */
typedef enum {
    HAL_GPIO_PIN0,
    HAL_GPIO_PIN1,
    HAL_GPIO_PIN2,
    HAL_GPIO_PIN3,
    HAL_GPIO_PIN4,
    HAL_GPIO_PIN5,
    HAL_GPIO_PIN6,
    HAL_GPIO_PIN7,
    HAL_GPIO_PIN8,
    HAL_GPIO_PIN9,
    HAL_GPIO_PIN10,
    HAL_GPIO_PIN11,
    HAL_GPIO_PIN12,
    HAL_GPIO_PIN13,
    HAL_GPIO_PIN14,
    HAL_GPIO_PIN15,
    HAL_GPIO_PIN_INVALID,    ///< 无效引脚号
} HalGpioPin;

// 位段结构体：每个port对应16位（bit0-bit15对应pin0-pin15）
typedef struct {
    uint16_t port0 : 16; // port0的16个pin（bit0=pin0，bit15=pin15）
    uint16_t port1 : 16; // port1的16个pin
    uint16_t port2 : 16; // port2的16个pin
    uint16_t port3 : 16; // port3的16个pin
} GpioIntState_t;

/* 公共函数原型 -----------------------------------------------------------*/

/**
 * @brief 初始化GPIO引脚
 * @param port 端口号
 * @param pin  引脚号
 * @param mode 工作模式
 * @param pull 上下拉配置
 * @note 该函数会配置GPIO的以下属性：
 *       - 输入/输出模式
 *       - 输出类型（推挽/开漏）
 *       - 上下拉电阻
 *       - 复用功能（如果适用）
 */
void HAL_GPIO_Init(HalGpioPort port, HalGpioPin pin, HalGpioMode mode, HalGpioPull pull);

/**
 * @brief 设置GPIO输出电平
 * @param port  端口号
 * @param pin   引脚号
 * @param state 输出电平（true=高电平，false=低电平）
 * @note 该函数应在GPIO配置为输出模式后使用
 */
void HAL_GPIO_Write(HalGpioPort port, HalGpioPin pin, bool state);

/**
 * @brief 读取GPIO输入电平
 * @param port 端口号
 * @param pin  引脚号
 * @return 当前输入电平（true=高电平，false=低电平）
 * @note 该函数应在GPIO配置为输入模式后使用
 */
bool HAL_GPIO_Read(HalGpioPort port, HalGpioPin pin);

// 辅助/内部接口
HalGpioPort hal_convert_gpio_port(uint8_t port);
HalGpioPin hal_convert_gpio_pin(uint8_t pin);
uint32_t hal_convert_ob_gpio_pin(HalGpioPin pin);
OB_GPIO_Type* hal_convert_ob_gpio_port(HalGpioPort port);


uint8_t hal_read_gpio_int_flag(volatile OB_GPIO_Type *pGPIO, volatile uint32_t nPinSel);
uint8_t hal_clear_gpio_int_flag(volatile OB_GPIO_Type *pGPIO, volatile uint32_t nPinSel);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_GPIO_H */

