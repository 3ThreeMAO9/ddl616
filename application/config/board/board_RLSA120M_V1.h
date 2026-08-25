#ifndef BOARD_RLSA120M_V1_H_
#define BOARD_RLSA120M_V1_H_

#include "hal_gpio.h"



/***********Enum***********/
typedef enum
{
    KEY_NUM_0 = 0,
    KEY_NUM_1,
    KEY_NUM_2,
    KEY_NUM_3,
    KEY_NUM_4,
    KEY_NUM_5,
    KEY_NUM_6,
    KEY_NUM_7,
    KEY_NUM_8,
    KEY_NUM_9,
    KEY_CAN,
    KEY_OK,
    KEY_NUM_13,
    KEY_CNT,

    KEY_LOCK,
    KEY_RESET,
    KEY_NULL = 0xFF,
} key_value_t;

typedef enum
{
    LED_NUM_0 = 0,
    LED_NUM_1,
    LED_NUM_2,
    LED_NUM_3,
    LED_NUM_4,
    LED_NUM_5,
    LED_NUM_6,
    LED_NUM_7,
    LED_NUM_8,
    LED_NUM_9,
    LED_CAN,
    LED_OK,
    LED_CNT,
    LED_NULL = 0xFF,
} led_value_t;

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/***********Macro***********/
    // tamper key
    #define TAMPER_KEY_GPIO                 (HAL_GPIO_PORT1)
    #define TAMPER_KEY_PIN                  (HAL_GPIO_PIN1)

    // voice en
    #define VOICE_EN_GPIO                   (HAL_GPIO_PORT1)
    #define VOICE_EN_PIN                    (HAL_GPIO_PIN4)

//NFC
#define NFC_NRST_GPIO                   (HAL_GPIO_PORT0)
#define NFC_NRST_PIN                    (HAL_GPIO_PIN10)
#define NFC_IRQ_GPIO                    (HAL_GPIO_PORT1)
#define NFC_IRQ_PIN                     (HAL_GPIO_PIN3)
#define NFC_NSS_GPIO                    (HAL_GPIO_PORT3)
#define NFC_NSS_PIN                     (HAL_GPIO_PIN4)
#define NFC_CLK_GPIO                    (HAL_GPIO_PORT3)
#define NFC_CLK_PIN                     (HAL_GPIO_PIN5)
#define NFC_MOSI_GPIO                   (HAL_GPIO_PORT3)
#define NFC_MOSI_PIN                    (HAL_GPIO_PIN6)
#define NFC_MISO_GPIO                   (HAL_GPIO_PORT3)
#define NFC_MISO_PIN                    (HAL_GPIO_PIN7)

#define NFC_SPI                         (HAL_SPI0)
#define NFC_SPI_TYPE                    (HAL_SPI_TYPE_HW)

// #define NFC_IRQ_ADC_CHANNEL           (ADC_CHANNEL_4)
// #define NFC_IRQ_ADC_MODE              (1)

///<endNFC

//led
#define LED_POWER_GPIO                  (HAL_GPIO_PORT0)
#define LED_POWER_PIN                   (HAL_GPIO_PIN13)

    #define LED_OE_GPIO                     (HAL_GPIO_PORT0)
    #define LED_OE_PIN                      (HAL_GPIO_PIN11)
    #define LED_LE_GPIO                     (HAL_GPIO_PORT1)
    #define LED_LE_PIN                      (HAL_GPIO_PIN7)
    #define LED_SDI_GPIO                    (HAL_GPIO_PORT0)
    #define LED_SDI_PIN                     (HAL_GPIO_PIN14)
    #define LED_SCK_GPIO                    (HAL_GPIO_PORT0)
    #define LED_SCK_PIN                     (HAL_GPIO_PIN1)

    #define LED_NULL                        (0)
    #define LED_0                           (1 << 6)
    #define LED_1                           (1 << 3)
    #define LED_2                           (1 << 13)
    #define LED_3                           (1 << 14)
    #define LED_4                           (1 << 2)
    #define LED_5                           (1 << 10)
    #define LED_6                           (1 << 11)
    #define LED_7                           (1 << 1)
    #define LED_8                           (1 << 8)
    #define LED_9                           (1 << 9)
    #define LED_10                          (1 << 0)
    #define LED_11                          (1 << 4)
    #define LED_12                          (LED_NULL)
    #define LED_13                          (LED_NULL)
    #define LED_14                          (LED_NULL)
    #define LED_15                          (LED_NULL)

//back uart
#define BACK_UART_SEL                   (UART_0)


    //uart
    #define UART0_TX_GPIO                   (HAL_GPIO_PORT3)
    #define UART0_TX_PIN                    (HAL_GPIO_PIN1)
    #define UART0_RX_GPIO                   (HAL_GPIO_PORT3)
    #define UART0_RX_PIN                    (HAL_GPIO_PIN0)

    #define UART_INT_GPIO                   (UART0_RX_GPIO)
    #define UART_INT_PIN                    (UART0_RX_PIN)


    #define UART1_TX_GPIO                   (HAL_GPIO_PORT0)
    #define UART1_TX_PIN                    (HAL_GPIO_PIN3)
    #define UART1_RX_GPIO                   (HAL_GPIO_PORT2)
    #define UART1_RX_PIN                    (HAL_GPIO_PIN11)

    // soft uart    
#define SOFT_UART_LOG_TX_GPIO               (HAL_GPIO_PORT0)
#define SOFT_UART_LOG_TX_PIN                (HAL_GPIO_PIN9)
#define SIMU_UART_GPIO                      (SOFT_UART_LOG_TX_GPIO)
#define SIMU_UART_PIN                       (SOFT_UART_LOG_TX_PIN)

#define LOG_UART_TX_GPIO                    (HAL_GPIO_PORT3)
#define LOG_UART_TX_PIN                     (HAL_GPIO_PIN1)
#define LOG_UART_RX_GPIO                    (HAL_GPIO_PORT3)
#define LOG_UART_RX_PIN                     (HAL_GPIO_PIN0)
#define LOG_UART_SEL                        (UART_0)

// fingerprint  
#define FP_TX_GPIO                      (HAL_GPIO_PORT2)
#define FP_TX_PIN                       (HAL_GPIO_PIN1)
#define FP_RX_GPIO                      (HAL_GPIO_PORT2)
#define FP_RX_PIN                       (HAL_GPIO_PIN0)
#define FP_POWER_GPIO                   (HAL_GPIO_PORT0)
#define FP_POWER_PIN                    (HAL_GPIO_PIN6)
#define FP_WAKE_GPIO                    (HAL_GPIO_PORT0)
#define FP_WAKE_PIN                     (HAL_GPIO_PIN7)
#define FINGER_UART_SEL                 (UART_1)

    // face
    #define FACE_TX_GPIO                    (HAL_GPIO_PORT0)
    #define FACE_TX_PIN                     (HAL_GPIO_PIN3)
    #define FACE_RX_GPIO                    (HAL_GPIO_PORT2)
    #define FACE_RX_PIN                     (HAL_GPIO_PIN11)
    #define FACE_POWER_GPIO                 (HAL_GPIO_PORT2)
    #define FACE_POWER_PIN                  (HAL_GPIO_PIN12)
    #define FACE_UART_SEL                   (UART_1)            

//touch key
#define TOUCH_CH_CNT                    (14)
#define TOUCH_WAKE_GPIO                 (HAL_GPIO_PORT2)
#define TOUCH_WAKE_PIN                  (HAL_GPIO_PIN7)
#define TOUCH_RESET_GPIO                (HAL_GPIO_PORT1)
#define TOUCH_RESET_PIN                 (HAL_GPIO_PIN2)
#define TOUCH_I2C_SCL_GPIO              (HAL_GPIO_PORT1)
#define TOUCH_I2C_SCL_PIN               (HAL_GPIO_PIN6)
#define TOUCH_I2C_SDA_GPIO              (HAL_GPIO_PORT1)
#define TOUCH_I2C_SDA_PIN               (HAL_GPIO_PIN5)

#define KEYBOAD_MAX 14

#define KEY_VALUE_TAB {KEY_NUM_13, KEY_NUM_3, KEY_NUM_2, KEY_NUM_5, KEY_NUM_6, KEY_NUM_9, KEY_NUM_8, \
                       KEY_OK, KEY_NUM_0, KEY_CAN, KEY_NUM_7, KEY_NUM_4, KEY_NUM_1, KEY_NULL}

/****************************/

/*****************************/


#endif /* BOARD_RLSA120M_V1_H_ */

