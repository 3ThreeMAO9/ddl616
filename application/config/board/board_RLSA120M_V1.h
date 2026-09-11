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
//player
#define PLAYER_EN_PORT                  (OB_GPIO2)
#define PLAYER_EN_PIN                   (GPIO_PIN9)

#define PLAYER_DATA_PORT                (OB_GPIO0)
#define PLAYER_DATA_PIN                 (GPIO_PIN12)
#define PLAYER_PWM_CHANNEL              (PWM_CH0B)
#define PLAYER_PWM_PERIOD               ((uint32_t)(64 * 1000))           //unit: Hz
#define PLAYER_PWM_DUTY                 ((uint8_t)(0x80))                 //unit: %
//end player

// motor
#define MOTOR_LA_GPIO                   (OB_GPIO2)
#define MOTOR_LA_PIN                    (GPIO_PIN13)

#define MOTOR_LB_GPIO                   (OB_GPIO2)
#define MOTOR_LB_PIN                    (GPIO_PIN4)

// tamper key
#define TAMPER_KEY_GPIO                 (OB_GPIO1)
#define TAMPER_KEY_PIN                  (GPIO_PIN1)

//NFC
#define NFC_NRST_GPIO                   (OB_GPIO0)
#define NFC_NRST_PIN                    (GPIO_PIN0)
#define NFC_IRQ_GPIO                    (OB_GPIO1)
#define NFC_IRQ_PIN                     (GPIO_PIN3)
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

///< SPI FLASH
#define FLASH_CS_PORT                   (HAL_GPIO_PORT0)
#define FLASH_CS_PIN                    (HAL_GPIO_PIN8)

#define FLASH_CLK_PORT                  (HAL_GPIO_PORT2)
#define FLASH_CLK_PIN                   (HAL_GPIO_PIN10)

#define FLASH_MOSI_PORT                 (HAL_GPIO_PORT0)
#define FLASH_MOSI_PIN                  (HAL_GPIO_PIN4)

#define FLASH_MISO_PORT                 (HAL_GPIO_PORT0)
#define FLASH_MISO_PIN                  (HAL_GPIO_PIN5)

#define FLASH_SPI_ID                    (HAL_SPI1)
#define FLASH_SPI_TYPE                  (HAL_SPI_TYPE_SW)

#define FLASH_CS_PIN_SET                (OB_GPIO0->DATA |= GPIO_PIN8)
#define FLASH_CS_PIN_CLR                (OB_GPIO0->DATA &= ~GPIO_PIN8)
#define FLASH_CLK_PIN_SET               (OB_GPIO2->DATA |= GPIO_PIN10)
#define FLASH_CLK_PIN_CLR               (OB_GPIO2->DATA &= ~GPIO_PIN10)
#define FLASH_MOSI_PIN_SET              (OB_GPIO0->DATA |= GPIO_PIN4)
#define FLASH_MOSI_PIN_CLR              (OB_GPIO0->DATA &= ~GPIO_PIN4)
#define FLASH_MISO_READ_PIN             (OB_GPIO0->PIN & GPIO_PIN5)

///< End SPI FLASH


// led
#define RED_LED_GPIO                    (OB_GPIO0)
#define RED_LED_PIN                     (GPIO_PIN14)
#define GREEN_LED_GPIO                  (OB_GPIO0)
#define GREEN_LED_PIN                   (GPIO_PIN1)
#define WHITE_LED_GPIO                  (OB_GPIO0)
#define WHITE_LED_PIN                   (GPIO_PIN13)
#define BLUE_LED_GPIO                   (OB_GPIO1)
#define BLUE_LED_PIN                    (GPIO_PIN7)

//back uart
#define BACK_UART_SEL                   (UART_0)


//uart
#define UART_INT_GPIO                   (OB_GPIO3)
#define UART_INT_PIN                    (GPIO_PIN0)

// soft uart    
#define SIMU_UART_GPIO                  (OB_GPIO0)
#define SIMU_UART_PIN                   (GPIO_PIN9)

#define LOG_UART_SEL                    (UART_0)

#define UART0_TX_GPIO                   (OB_GPIO3)
#define UART0_TX_PIN                    (GPIO_PIN1)
#define UART0_RX_GPIO                   (OB_GPIO3)
#define UART0_RX_PIN                    (GPIO_PIN0)
                      

// fingerprint  
#define UART1_TX_GPIO                   (OB_GPIO2)
#define UART1_TX_PIN                    (GPIO_PIN1)
#define UART1_RX_GPIO                   (OB_GPIO2)
#define UART1_RX_PIN                    (GPIO_PIN0)
#define FP_TX_GPIO                      (HAL_GPIO_PORT2)
#define FP_TX_PIN                       (HAL_GPIO_PIN1)
#define FP_RX_GPIO                      (HAL_GPIO_PORT2)
#define FP_RX_PIN                       (HAL_GPIO_PIN0)
#define FP_POWER_GPIO                   (OB_GPIO0)
#define FP_POWER_PIN                    (GPIO_PIN6)
#define FP_WAKE_GPIO                    (OB_GPIO0)
#define FP_WAKE_PIN                     (GPIO_PIN7)
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
#define TOUCH_WAKE_GPIO                 (OB_GPIO2)
#define TOUCH_WAKE_PIN                  (GPIO_PIN7)
#define TOUCH_RESET_GPIO                (OB_GPIO1)
#define TOUCH_RESET_PIN                 (GPIO_PIN2)
#define TOUCH_I2C_SCL_GPIO              (OB_GPIO1)
#define TOUCH_I2C_SCL_PIN               (GPIO_PIN6)
#define TOUCH_I2C_SDA_GPIO              (OB_GPIO1)
#define TOUCH_I2C_SDA_PIN               (GPIO_PIN5)

#define KEYBOAD_MAX 14

#define KEY_VALUE_TAB {KEY_NUM_13, KEY_NUM_3, KEY_NUM_2, KEY_NUM_5, KEY_NUM_6, KEY_NUM_9, KEY_NUM_8, \
                       KEY_OK, KEY_NUM_0, KEY_CAN, KEY_NUM_7, KEY_NUM_4, KEY_NUM_1, KEY_NULL}

/****************************/

/*****************************/


#endif /* BOARD_RLSA120M_V1_H_ */

