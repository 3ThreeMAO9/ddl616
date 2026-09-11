/**
 * @file    hal_gpio.c
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
#include "hal_gpio.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_gpio"

/* Private defines -----------------------------------------------------------*/
volatile GpioIntState_t gpio_int_state = {0};

static uint8_t hal_read_gpio_port(volatile OB_GPIO_Type *pGPIO)
{
    volatile OB_GPIO_Type *port_tab[OB_GPIO_CNT] = {OB_GPIO0, OB_GPIO1, OB_GPIO2, OB_GPIO3};
    uint8_t i;
    for (i = 0; i < OB_GPIO_CNT; i++)
    {
        if (pGPIO == port_tab[i])
            break;
    }
    return i;
}

uint8_t hal_read_gpio_int_flag(volatile OB_GPIO_Type *pGPIO, volatile uint32_t nPinSel)
{
    gpio_port_id_t gpio_port;
    gpio_port = (gpio_port_id_t)hal_read_gpio_port(pGPIO);
    if (gpio_port < OB_GPIO_CNT)
    {
        if (g_GPIO_Interrupt[gpio_port]&nPinSel)
            return true;
    }
    return false;
}

uint8_t hal_clear_gpio_int_flag(volatile OB_GPIO_Type *pGPIO, volatile uint32_t nPinSel)
{
    gpio_port_id_t gpio_port;
    gpio_port = (gpio_port_id_t)hal_read_gpio_port(pGPIO);
    if (gpio_port < OB_GPIO_CNT)
        g_GPIO_Interrupt[gpio_port] &= (~nPinSel);
    else
        return false;
    return true;
}

HalGpioPort hal_convert_gpio_port(uint8_t port)
{
    switch (port)
    {
        case HAL_GPIO_PORT0:
            return HAL_GPIO_PORT0;
        case HAL_GPIO_PORT1:
            return HAL_GPIO_PORT1;
        case HAL_GPIO_PORT2:
            return HAL_GPIO_PORT2;
        case HAL_GPIO_PORT3:
            return HAL_GPIO_PORT3;
        default:
            return HAL_GPIO_PORT_INVALID;
    }
}

HalGpioPin hal_convert_gpio_pin(uint8_t pin)
{
    switch (pin)
    {
        case HAL_GPIO_PIN0:
            return HAL_GPIO_PIN0;
        case HAL_GPIO_PIN1:
            return HAL_GPIO_PIN1;
        case HAL_GPIO_PIN2:
            return HAL_GPIO_PIN2;
        case HAL_GPIO_PIN3:
            return HAL_GPIO_PIN3;
        case HAL_GPIO_PIN4:
            return HAL_GPIO_PIN4;
        case HAL_GPIO_PIN5:
            return HAL_GPIO_PIN5;
        case HAL_GPIO_PIN6:
            return HAL_GPIO_PIN6;
        case HAL_GPIO_PIN7:
            return HAL_GPIO_PIN7;
        case HAL_GPIO_PIN8:
            return HAL_GPIO_PIN8;
        case HAL_GPIO_PIN9:
            return HAL_GPIO_PIN9;
        case HAL_GPIO_PIN10:
            return HAL_GPIO_PIN10;
        case HAL_GPIO_PIN11:
            return HAL_GPIO_PIN11;
        case HAL_GPIO_PIN12:
            return HAL_GPIO_PIN12;
        case HAL_GPIO_PIN13:
            return HAL_GPIO_PIN13;
        case HAL_GPIO_PIN14:
            return HAL_GPIO_PIN14;
        case HAL_GPIO_PIN15:
            return HAL_GPIO_PIN15;
        default:
            return HAL_GPIO_PIN_INVALID;
    }
}


OB_GPIO_Type* hal_convert_ob_gpio_port(HalGpioPort port)
{
    OB_GPIO_Type* ob_port = OB_GPIO0;
    switch (port)
    {
    case HAL_GPIO_PORT0:
        ob_port = OB_GPIO0;
        break;
    case HAL_GPIO_PORT1:
        ob_port = OB_GPIO1;
        break;
    case HAL_GPIO_PORT2:
        ob_port = OB_GPIO2;
        break;
    case HAL_GPIO_PORT3:
        ob_port = OB_GPIO3;
        break;
    default:
        break;
    }
    return ob_port;
}

uint32_t hal_convert_ob_gpio_pin(HalGpioPin pin)
{
    uint32_t ob_pin = GPIO_PIN0;
    switch (pin)
    {
    case HAL_GPIO_PIN0:
        ob_pin = GPIO_PIN0;
        break;
    case HAL_GPIO_PIN1:
        ob_pin = GPIO_PIN1;
        break;
    case HAL_GPIO_PIN2:
        ob_pin = GPIO_PIN2;
        break;
    case HAL_GPIO_PIN3:
        ob_pin = GPIO_PIN3;
        break;
    case HAL_GPIO_PIN4:
        ob_pin = GPIO_PIN4;
        break;
    case HAL_GPIO_PIN5:
        ob_pin = GPIO_PIN5;
        break;
    case HAL_GPIO_PIN6:
        ob_pin = GPIO_PIN6;
        break;
    case HAL_GPIO_PIN7:
        ob_pin = GPIO_PIN7;
        break;
    case HAL_GPIO_PIN8:
        ob_pin = GPIO_PIN8;
        break;
    case HAL_GPIO_PIN9:
        ob_pin = GPIO_PIN9;
        break;
    case HAL_GPIO_PIN10:
        ob_pin = GPIO_PIN10;
        break;
    case HAL_GPIO_PIN11:
        ob_pin = GPIO_PIN11;
        break;
    case HAL_GPIO_PIN12:
        ob_pin = GPIO_PIN12;
        break;
    case HAL_GPIO_PIN13:
        ob_pin = GPIO_PIN13;
        break;
    case HAL_GPIO_PIN14:
        ob_pin = GPIO_PIN14;
        break;
    case HAL_GPIO_PIN15:
        ob_pin = GPIO_PIN15;
        break;
    default:
        break;
    }
    return ob_pin;
}


// 内部函数，用于配置GPIO模式和上拉/下拉
static void configure_gpio(HalGpioPort port, HalGpioPin pin, HalGpioMode mode, HalGpioPull pull)
{
    uint8_t pin_type = GPIO_MF_TYPE_GPIO;
    uint8_t pin_mode = GPIO_PINMODE_INPUT;
    OB_GPIO_Type* ob_port = OB_GPIO0;
    uint32_t ob_pin = GPIO_PIN0;
    // 配置GPIO模式
    switch (mode)
    {
        case HAL_GPIO_MODE_INPUT:
            pin_mode = GPIO_PINMODE_INPUT;
            // 设置GPIO为输入模式
            break;
        case HAL_GPIO_MODE_INPUT_PULLUP:
            pin_mode = GPIO_PINMODE_PULL_UP;
            // 设置GPIO为上拉输入模式
            break;
        case HAL_GPIO_MODE_OUTPUT_PP:
            pin_mode = GPIO_PINMODE_PUSH_PULL;
            // 设置GPIO为推挽输出
            break;
        case HAL_GPIO_MODE_OUTPUT_OD:
            pin_mode = GPIO_PINMODE_OPEN_DRAIN;
            // 设置GPIO为开漏输出
            break;
        case HAL_GPIO_MODE_ANALOG:
            if ((port == HAL_GPIO_PORT3) && (pin == HAL_GPIO_PIN1))
            {
                pin_type = GPIO_MF_UART0_TX;
                pin_mode = GPIO_PINMODE_PULL_UP;
            }
            else if ((port == HAL_GPIO_PORT3) && (pin == HAL_GPIO_PIN0))
            {
                pin_type = GPIO_MF_UART0_RX;
                pin_mode = GPIO_PINMODE_PULL_UP;
            }
            else if ((port == HAL_GPIO_PORT0) && (pin == HAL_GPIO_PIN3))
            {
                pin_type = GPIO_MF_UART1_TX;
                pin_mode = GPIO_PINMODE_PULL_UP;            
            }
            else if ((port == HAL_GPIO_PORT2) && (pin == HAL_GPIO_PIN11))
            {
                pin_type = GPIO_MF_UART1_RX;
                pin_mode = GPIO_PINMODE_PULL_UP;            
            }
            // 设置GPIO为模拟模式
            break;
        default:
            break;
    }
    // // 配置上拉/下拉电阻
    // switch (pull)
    // {
    //     case HAL_GPIO_PULL_NONE:
    //         break;
    //     case HAL_GPIO_PULL_HIGH:
    //         break;
    //     case HAL_GPIO_PULL_LOW:
    //         break;
    //     default:
    //         break;
    // }
    ob_port = hal_convert_ob_gpio_port(port);
    ob_pin = hal_convert_ob_gpio_pin(pin);

    GPIO_SetPinMFType(ob_port, ob_pin, pin_type, pin_mode);
}

// 初始化GPIO
void HAL_GPIO_Init(HalGpioPort port, HalGpioPin pin, HalGpioMode mode, HalGpioPull pull)
{
    // uart_printf("gpio->port %d  gpio->pin %d\r\n", port, pin);
    configure_gpio(port, pin, mode, pull);
}

// 写GPIO状态
void HAL_GPIO_Write(HalGpioPort port, HalGpioPin pin, bool state)
{
    OB_GPIO_Type *ob_port = hal_convert_ob_gpio_port(port);
    uint32_t ob_pin = hal_convert_ob_gpio_pin(pin);
    if (state)
        ob_port->DATA |= ob_pin;
    else
        ob_port->DATA &= (~ob_pin);
}

// 读GPIO状态
bool HAL_GPIO_Read(HalGpioPort port, HalGpioPin pin)
{
    OB_GPIO_Type *ob_port = hal_convert_ob_gpio_port(port);
    uint32_t ob_pin = hal_convert_ob_gpio_pin(pin);
    
    return (ob_port->PIN & ob_pin);
}
