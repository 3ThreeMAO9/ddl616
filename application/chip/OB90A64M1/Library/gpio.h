#ifndef __GPIO_H
#define __GPIO_H

/*********************Macro*********************/
// Pin Mode
#define GPIO_PINMODE_PULL_UP        0
#define GPIO_PINMODE_OPEN_DRAIN     1
#define GPIO_PINMODE_PUSH_PULL      2
#define GPIO_PINMODE_INPUT          3

// Interrupt Mode
#define GPIO_INTMODE_DISABLE         0
#define GPIO_INTMODE_FALLING_EDGE    1
#define GPIO_INTMODE_RISING_EDGE     2
#define GPIO_INTMODE_BOTH_EDGE       3
#define GPIO_INTMODE_LOW_LEVEL       4
#define GPIO_INTMODE_HIGH_LEVEL      5

// PinSel
#define GPIO_PIN0      0x00000001
#define GPIO_PIN1      0x00000002
#define GPIO_PIN2      0x00000004
#define GPIO_PIN3      0x00000008
#define GPIO_PIN4      0x00000010
#define GPIO_PIN5      0x00000020
#define GPIO_PIN6      0x00000040
#define GPIO_PIN7      0x00000080
#define GPIO_PIN8      0x00000100
#define GPIO_PIN9      0x00000200
#define GPIO_PIN10     0x00000400
#define GPIO_PIN11     0x00000800
#define GPIO_PIN12     0x00001000
#define GPIO_PIN13     0x00002000
#define GPIO_PIN14     0x00004000
#define GPIO_PIN15     0x00008000
#define GPIO_ALL       0x0000FFFF


// Trigger ADC Type
#define GPIO_TADC_DISABLE       0
#define GPIO_TADC_FALLING_EDGE  1
#define GPIO_TADC_RISING_EDGE   2
#define GPIO_TADC_BOTH_EDGE     3

// Multiple Function Type
#define GPIO_MF_TYPE_GPIO        0

#define GPIO_MF_TIMER16B0_CAP    1
#define GPIO_MF_TIMER16B0_MAT    2
#define GPIO_MF_TIMER16B1_CAP    3
#define GPIO_MF_TIMER16B1_MAT    4

#define GPIO_MF_TIMER32B0_CAP    5
#define GPIO_MF_TIMER32B0_MAT    6

#define GPIO_MF_TIMER32B1_CAP    7
#define GPIO_MF_TIMER32B1_MAT    8

#define GPIO_MF_ACMP_O           9
#define GPIO_MF_ACMP_I           10

#define GPIO_MF_FAULT            11
#define GPIO_MF_UART0_RX         12
#define GPIO_MF_UART0_TX         13

#define GPIO_MF_UART1_RX         14
#define GPIO_MF_UART1_TX         15

#define GPIO_MF_SPI_MASTER_SS    16
#define GPIO_MF_SPI_MASTER_CLK   17
#define GPIO_MF_SPI_MASTER_MOSI  18
#define GPIO_MF_SPI_MASTER_MISO  19

#define GPIO_MF_SPI_SLAVE_SS     20
#define GPIO_MF_SPI_SLAVE_CLK    21
#define GPIO_MF_SPI_SLAVE_MOSI   22
#define GPIO_MF_SPI_SLAVE_MISO   23

#define GPIO_MF_QEI              24
#define GPIO_MF_I2C0_SCL         25
#define GPIO_MF_I2C0_SDA         26

#define GPIO_MF_I2C1_SCL         27
#define GPIO_MF_I2C1_SDA         28

#define GPIO_MF_OPA_I            29
#define GPIO_MF_OPA_O            30


#define GPIO_MF_PWM              31
#define GPIO_MF_PWMTM            32
#define GPIO_MF_ADC              33
#define GPIO_MF_RTC_INPUT        34
#define GPIO_MF_RTC_OUTPUT       35

#define GPIO_MF_XTAL             36
#define GPIO_MF_SWD              37
#define GPIO_MF_RESET            38
#define GPIO_MF_SYSTEM           39

#define GPIO_EnableINT(pGPIO, nPinSel, nINTMode)     \
    do                                               \
    {                                                \
        if (pGPIO == OB_GPIO0)                       \
            NVIC_EnableIRQ(GPIO0_IRQn);              \
        else if (pGPIO == OB_GPIO1)                  \
            NVIC_EnableIRQ(GPIO1_IRQn);              \
        else if (pGPIO == OB_GPIO2)                  \
            NVIC_EnableIRQ(GPIO2_IRQn);              \
        else if (pGPIO == OB_GPIO3)                  \
            NVIC_EnableIRQ(GPIO3_IRQn);              \
        if (nPinSel & GPIO_PIN0)                     \
            pGPIO->IEENS0_b.GPIOXIEENS0 = nINTMode;  \
        else if (nPinSel & GPIO_PIN1)                \
            pGPIO->IEENS0_b.GPIOXIEENS1 = nINTMode;  \
        else if (nPinSel & GPIO_PIN2)                \
            pGPIO->IEENS0_b.GPIOXIEENS2 = nINTMode;  \
        else if (nPinSel & GPIO_PIN3)                \
            pGPIO->IEENS0_b.GPIOXIEENS3 = nINTMode;  \
        else if (nPinSel & GPIO_PIN4)                \
            pGPIO->IEENS0_b.GPIOXIEENS4 = nINTMode;  \
        else if (nPinSel & GPIO_PIN5)                \
            pGPIO->IEENS0_b.GPIOXIEENS5 = nINTMode;  \
        else if (nPinSel & GPIO_PIN6)                \
            pGPIO->IEENS0_b.GPIOXIEENS6 = nINTMode;  \
        else if (nPinSel & GPIO_PIN7)                \
            pGPIO->IEENS0_b.GPIOXIEENS7 = nINTMode;  \
        else if (nPinSel & GPIO_PIN8)                \
            pGPIO->IEENS1_b.GPIOXIEENS8 = nINTMode;  \
        else if (nPinSel & GPIO_PIN9)                \
            pGPIO->IEENS1_b.GPIOXIEENS9 = nINTMode;  \
        else if (nPinSel & GPIO_PIN10)               \
            pGPIO->IEENS1_b.GPIOXIEENS10 = nINTMode; \
        else if (nPinSel & GPIO_PIN11)               \
            pGPIO->IEENS1_b.GPIOXIEENS11 = nINTMode; \
        else if (nPinSel & GPIO_PIN12)               \
            pGPIO->IEENS1_b.GPIOXIEENS12 = nINTMode; \
        else if (nPinSel & GPIO_PIN13)               \
            pGPIO->IEENS1_b.GPIOXIEENS13 = nINTMode; \
        else if (nPinSel & GPIO_PIN14)               \
            pGPIO->IEENS1_b.GPIOXIEENS14 = nINTMode; \
        else if (nPinSel & GPIO_PIN15)               \
            pGPIO->IEENS1_b.GPIOXIEENS15 = nINTMode; \
    } while (0)

#define GPIO_SetPinMode(pGPIO, nPinSel, nPinMode) \
    do                                            \
    {                                             \
        if (nPinSel & GPIO_PIN0)                  \
            pGPIO->MODE_b.MODEPIN0 = nPinMode;    \
        else if (nPinSel & GPIO_PIN1)             \
            pGPIO->MODE_b.MODEPIN1 = nPinMode;    \
        else if (nPinSel & GPIO_PIN2)             \
            pGPIO->MODE_b.MODEPIN2 = nPinMode;    \
        else if (nPinSel & GPIO_PIN3)             \
            pGPIO->MODE_b.MODEPIN3 = nPinMode;    \
        else if (nPinSel & GPIO_PIN4)             \
            pGPIO->MODE_b.MODEPIN4 = nPinMode;    \
        else if (nPinSel & GPIO_PIN5)             \
            pGPIO->MODE_b.MODEPIN5 = nPinMode;    \
        else if (nPinSel & GPIO_PIN6)             \
            pGPIO->MODE_b.MODEPIN6 = nPinMode;    \
        else if (nPinSel & GPIO_PIN7)             \
            pGPIO->MODE_b.MODEPIN7 = nPinMode;    \
        else if (nPinSel & GPIO_PIN8)             \
            pGPIO->MODE_b.MODEPIN8 = nPinMode;    \
        else if (nPinSel & GPIO_PIN9)             \
            pGPIO->MODE_b.MODEPIN9 = nPinMode;    \
        else if (nPinSel & GPIO_PIN10)            \
            pGPIO->MODE_b.MODEPIN10 = nPinMode;   \
        else if (nPinSel & GPIO_PIN11)            \
            pGPIO->MODE_b.MODEPIN11 = nPinMode;   \
        else if (nPinSel & GPIO_PIN12)            \
            pGPIO->MODE_b.MODEPIN12 = nPinMode;   \
        else if (nPinSel & GPIO_PIN13)            \
            pGPIO->MODE_b.MODEPIN13 = nPinMode;   \
        else if (nPinSel & GPIO_PIN14)            \
            pGPIO->MODE_b.MODEPIN14 = nPinMode;   \
        else if (nPinSel & GPIO_PIN15)            \
            pGPIO->MODE_b.MODEPIN15 = nPinMode;   \
    } while (0)

#define GPIO_SetPinMFType(pGPIO, nPinSel, nMFType, nPINMODE)                                          \
    do                                                                                                \
    {                                                                                                 \
        if (pGPIO == OB_GPIO0)                                                                        \
        {                                                                                             \
            if (nPinSel & GPIO_PIN0)                                                                  \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_0 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_0 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_0 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_0 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN1)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_1 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_1 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_1 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_1 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN2)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_CAP)                                                 \
                    pGPIO->MF0_b.PORT_2 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_2 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_2 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_2 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN3)                                                             \
            {                                                                                         \
                if ((nMFType == GPIO_MF_SPI_MASTER_SS) || (nMFType == GPIO_MF_SPI_SLAVE_SS))          \
                    pGPIO->MF0_b.PORT_3 = 1;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_3 = 3;                                                          \
                else if (nMFType == GPIO_MF_UART1_TX)                                                 \
                    pGPIO->MF0_b.PORT_3 = 4;                                                          \
                else if (nMFType == GPIO_MF_I2C1_SCL)                                                 \
                    pGPIO->MF0_b.PORT_3 = 5;                                                          \
                else if (nMFType == GPIO_MF_PWMTM)                                                    \
                    pGPIO->MF0_b.PORT_3 = 6;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_3 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN4)                                                             \
            {                                                                                         \
                if ((nMFType == GPIO_MF_SPI_MASTER_MOSI) || (nMFType == GPIO_MF_SPI_SLAVE_MOSI))      \
                    pGPIO->MF0_b.PORT_4 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_4 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_4 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_4 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN5)                                                             \
            {                                                                                         \
                if ((nMFType == GPIO_MF_SPI_MASTER_MISO) || (nMFType == GPIO_MF_SPI_SLAVE_MISO))      \
                    pGPIO->MF0_b.PORT_5 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_5 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_5 = 3;                                                          \
                else if (nMFType == GPIO_MF_TIMER32B0_CAP)                                            \
                    pGPIO->MF0_b.PORT_5 = 4;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_5 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN6)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B0_CAP)                                                 \
                    pGPIO->MF0_b.PORT_6 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_6 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_O)                                                   \
                    pGPIO->MF0_b.PORT_6 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_6 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN7)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B0_CAP)                                                 \
                    pGPIO->MF0_b.PORT_7 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_7 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_O)                                                   \
                    pGPIO->MF0_b.PORT_7 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_7 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN8)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_MAT)                                                 \
                    pGPIO->MF1_b.PORT_8 = 1;                                                          \
                else if (nMFType == GPIO_MF_PWM)                                                      \
                    pGPIO->MF1_b.PORT_8 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_8 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN9)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_MAT)                                                 \
                    pGPIO->MF1_b.PORT_9 = 1;                                                          \
                else if (nMFType == GPIO_MF_PWM)                                                      \
                    pGPIO->MF1_b.PORT_9 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_9 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN10)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_MAT)                                                 \
                    pGPIO->MF1_b.PORT_10 = 1;                                                         \
                else if (nMFType == GPIO_MF_PWM)                                                      \
                    pGPIO->MF1_b.PORT_10 = 3;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_10 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN11)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_MAT)                                                 \
                    pGPIO->MF1_b.PORT_11 = 1;                                                         \
                else if (nMFType == GPIO_MF_PWM)                                                      \
                    pGPIO->MF1_b.PORT_11 = 3;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_11 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN12)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_CAP)                                                 \
                    pGPIO->MF1_b.PORT_12 = 1;                                                         \
                else if (nMFType == GPIO_MF_PWM)                                                      \
                    pGPIO->MF1_b.PORT_12 = 3;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_12 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN13)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_CAP)                                                 \
                    pGPIO->MF1_b.PORT_13 = 1;                                                         \
                else if (nMFType == GPIO_MF_PWM)                                                      \
                    pGPIO->MF1_b.PORT_13 = 3;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_13 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN14)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B1_CAP)                                                 \
                    pGPIO->MF1_b.PORT_14 = 1;                                                         \
                else if (nMFType == GPIO_MF_FAULT)                                                    \
                    pGPIO->MF1_b.PORT_14 = 3;                                                         \
                else if ((nMFType == GPIO_MF_SPI_MASTER_MISO) || (nMFType == GPIO_MF_SPI_SLAVE_MISO)) \
                    pGPIO->MF1_b.PORT_14 = 4;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_14 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN15)                                                            \
            {                                                                                         \
                pGPIO->MF1_b.PORT_15 = 0;                                                             \
            }                                                                                         \
        }                                                                                             \
        else if (pGPIO == OB_GPIO1)                                                                   \
        {                                                                                             \
            if (nPinSel & GPIO_PIN0)                                                                  \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_0 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_0 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_0 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_0 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN1)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER32B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_1 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_1 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_1 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_1 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN2)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B1_CAP)                                                 \
                    pGPIO->MF0_b.PORT_2 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_2 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_2 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_2 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN3)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B1_CAP)                                                 \
                    pGPIO->MF0_b.PORT_3 = 1;                                                          \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF0_b.PORT_3 = 2;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_3 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_3 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN4)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B1_MAT)                                                 \
                    pGPIO->MF0_b.PORT_4 = 1;                                                          \
                else if (nMFType == GPIO_MF_ACMP_O)                                                   \
                    pGPIO->MF0_b.PORT_4 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_4 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN5)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B1_MAT)                                                 \
                    pGPIO->MF0_b.PORT_5 = 1;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_5 = 3;                                                          \
                else if (nMFType == GPIO_MF_RTC_INPUT)                                                \
                    pGPIO->MF0_b.PORT_5 = 4;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_5 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN6)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B1_MAT)                                                 \
                    pGPIO->MF0_b.PORT_6 = 1;                                                          \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF0_b.PORT_6 = 3;                                                          \
                else if (nMFType == GPIO_MF_RTC_OUTPUT)                                               \
                    pGPIO->MF0_b.PORT_6 = 4;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_6 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN7)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_CAP)                                                 \
                    pGPIO->MF0_b.PORT_7 = 1;                                                          \
                else if (nMFType == GPIO_MF_OPA_I)                                                    \
                    pGPIO->MF0_b.PORT_7 = 2;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_7 = 0;                                                          \
            }                                                                                         \
        }                                                                                             \
        else if (pGPIO == OB_GPIO2)                                                                   \
        {                                                                                             \
            if (nPinSel & GPIO_PIN0)                                                                  \
            {                                                                                         \
                if (nMFType == GPIO_MF_OPA_I)                                                         \
                    pGPIO->MF0_b.PORT_0 = 2;                                                          \
                else if (nMFType == GPIO_MF_UART1_RX)                                                 \
                    pGPIO->MF0_b.PORT_0 = 4;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_0 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN1)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_OPA_O)                                                         \
                    pGPIO->MF0_b.PORT_1 = 2;                                                          \
                else if (nMFType == GPIO_MF_UART1_TX)                                                 \
                    pGPIO->MF0_b.PORT_1 = 4;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_1 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN2)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_XTAL)                                                          \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 | 0x04;                           \
                    pGPIO->MF0_b.PORT_2 = 0;                                                          \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 & 0xFFFFFFFB;                     \
                    if ((nMFType == GPIO_MF_SPI_MASTER_SS) || (nMFType == GPIO_MF_SPI_SLAVE_SS))      \
                        pGPIO->MF0_b.PORT_2 = 2;                                                      \
                    else if (nMFType == GPIO_MF_PWMTM)                                                \
                        pGPIO->MF0_b.PORT_2 = 3;                                                      \
                    else if (nMFType == GPIO_MF_UART0_RX)                                             \
                        pGPIO->MF0_b.PORT_2 = 4;                                                      \
                    else if (nMFType == GPIO_MF_I2C0_SDA)                                             \
                        pGPIO->MF0_b.PORT_2 = 5;                                                      \
                    else                                                                              \
                        pGPIO->MF0_b.PORT_2 = 0;                                                      \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN3)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_XTAL)                                                          \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 | 0x04;                           \
                    pGPIO->MF0_b.PORT_3 = 0;                                                          \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 & 0xFFFFFFFB;                     \
                    if ((nMFType == GPIO_MF_SPI_MASTER_CLK) || (nMFType == GPIO_MF_SPI_SLAVE_CLK))    \
                        pGPIO->MF0_b.PORT_3 = 2;                                                      \
                    else if (nMFType == GPIO_MF_PWMTM)                                                \
                        pGPIO->MF0_b.PORT_3 = 3;                                                      \
                    else if (nMFType == GPIO_MF_UART0_TX)                                             \
                        pGPIO->MF0_b.PORT_3 = 4;                                                      \
                    else if (nMFType == GPIO_MF_I2C0_SCL)                                             \
                        pGPIO->MF0_b.PORT_3 = 5;                                                      \
                    else if (nMFType == GPIO_MF_QEI)                                                  \
                        pGPIO->MF0_b.PORT_3 = 6;                                                      \
                    else                                                                              \
                        pGPIO->MF0_b.PORT_3 = 0;                                                      \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN4)                                                             \
            {                                                                                         \
                if ((nMFType == GPIO_MF_SPI_MASTER_MOSI) || (nMFType == GPIO_MF_SPI_SLAVE_MOSI))      \
                    pGPIO->MF0_b.PORT_4 = 2;                                                          \
                else if (nMFType == GPIO_MF_FAULT)                                                    \
                    pGPIO->MF0_b.PORT_4 = 3;                                                          \
                else if (nMFType == GPIO_MF_TIMER16B0_CAP)                                            \
                    pGPIO->MF0_b.PORT_4 = 5;                                                          \
                else if (nMFType == GPIO_MF_QEI)                                                      \
                    pGPIO->MF0_b.PORT_4 = 6;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_4 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN5)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_SWD)                                                           \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 | 0x02;                           \
                    pGPIO->MF0_b.PORT_5 = 0;                                                          \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 & 0xFFFFFFFD;                     \
                    pGPIO->MF0_b.PORT_5 = 0;                                                          \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN6)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_SWD)                                                           \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 | 0x02;                           \
                    pGPIO->MF0_b.PORT_6 = 0;                                                          \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 & 0xFFFFFFFD;                     \
                    pGPIO->MF0_b.PORT_6 = 0;                                                          \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN7)                                                             \
            {                                                                                         \
                if ((nMFType == GPIO_MF_SPI_MASTER_MISO) || (nMFType == GPIO_MF_SPI_SLAVE_MISO))      \
                    pGPIO->MF0_b.PORT_7 = 1;                                                          \
                else if (nMFType == GPIO_MF_ACMP_O)                                                   \
                    pGPIO->MF0_b.PORT_7 = 3;                                                          \
                else if (nMFType == GPIO_MF_FAULT)                                                    \
                    pGPIO->MF0_b.PORT_7 = 4;                                                          \
                else if (nMFType == GPIO_MF_TIMER16B0_CAP)                                            \
                    pGPIO->MF0_b.PORT_7 = 5;                                                          \
                else if (nMFType == GPIO_MF_QEI)                                                      \
                    pGPIO->MF0_b.PORT_7 = 6;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_7 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN8)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_RESET)                                                         \
                {                                                                                     \
                    ;                                                                                 \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    if (nMFType == GPIO_MF_TIMER16B0_CAP)                                             \
                        pGPIO->MF1_b.PORT_8 = 1;                                                      \
                    else if (nMFType == GPIO_MF_QEI)                                                  \
                        pGPIO->MF1_b.PORT_8 = 3;                                                      \
                    else                                                                              \
                        pGPIO->MF1_b.PORT_8 = 0;                                                      \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN9)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_XTAL)                                                          \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 | 0x08;                           \
                    pGPIO->MF1_b.PORT_9 = 0;                                                          \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 & 0xFFFFFFF7;                     \
                    if (nMFType == GPIO_MF_TIMER16B0_CAP)                                             \
                        pGPIO->MF1_b.PORT_9 = 1;                                                      \
                    else if (nMFType == GPIO_MF_QEI)                                                  \
                        pGPIO->MF1_b.PORT_9 = 3;                                                      \
                    else                                                                              \
                        pGPIO->MF1_b.PORT_9 = 0;                                                      \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN10)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_XTAL)                                                          \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 | 0x08;                           \
                    pGPIO->MF1_b.PORT_10 = 0;                                                         \
                }                                                                                     \
                else                                                                                  \
                {                                                                                     \
                    OB_SYSCON->CPUCHIPCTR0 = OB_SYSCON->CPUCHIPCTR0 & 0xFFFFFFF7;                     \
                    if (nMFType == GPIO_MF_TIMER16B0_CAP)                                             \
                        pGPIO->MF1_b.PORT_10 = 1;                                                     \
                    else if (nMFType == GPIO_MF_QEI)                                                  \
                        pGPIO->MF1_b.PORT_10 = 3;                                                     \
                    else                                                                              \
                        pGPIO->MF1_b.PORT_10 = 0;                                                     \
                }                                                                                     \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN11)                                                            \
            {                                                                                         \
                if ((nMFType == GPIO_MF_SPI_MASTER_CLK) || (nMFType == GPIO_MF_SPI_SLAVE_CLK))        \
                    pGPIO->MF1_b.PORT_11 = 1;                                                         \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF1_b.PORT_11 = 3;                                                         \
                else if (nMFType == GPIO_MF_UART1_RX)                                                 \
                    pGPIO->MF1_b.PORT_11 = 4;                                                         \
                else if (nMFType == GPIO_MF_I2C1_SDA)                                                 \
                    pGPIO->MF1_b.PORT_11 = 5;                                                         \
                else if (nMFType == GPIO_MF_PWMTM)                                                    \
                    pGPIO->MF1_b.PORT_11 = 6;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_11 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN12)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B1_CAP)                                                 \
                    pGPIO->MF1_b.PORT_12 = 1;                                                         \
                else if (nMFType == GPIO_MF_ADC)                                                      \
                    pGPIO->MF1_b.PORT_12 = 2;                                                         \
                else if (nMFType == GPIO_MF_ACMP_I)                                                   \
                    pGPIO->MF1_b.PORT_12 = 3;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_12 = 0;                                                         \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN13)                                                            \
            {                                                                                         \
                if (nMFType == GPIO_MF_OPA_I)                                                         \
                    pGPIO->MF1_b.PORT_13 = 2;                                                         \
                else                                                                                  \
                    pGPIO->MF1_b.PORT_13 = 0;                                                         \
            }                                                                                         \
        }                                                                                             \
        else if (pGPIO == OB_GPIO3)                                                                   \
        {                                                                                             \
            if (nPinSel & GPIO_PIN0)                                                                  \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_CAP)                                                 \
                    pGPIO->MF0_b.PORT_0 = 1;                                                          \
                else if (nMFType == GPIO_MF_OPA_I)                                                    \
                    pGPIO->MF0_b.PORT_0 = 2;                                                          \
                else if (nMFType == GPIO_MF_UART0_RX)                                                 \
                    pGPIO->MF0_b.PORT_0 = 4;                                                          \
                else if (nMFType == GPIO_MF_SYSTEM)                                                   \
                    pGPIO->MF0_b.PORT_0 = 0x0F;                                                       \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_0 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN1)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_CAP)                                                 \
                    pGPIO->MF0_b.PORT_1 = 1;                                                          \
                else if (nMFType == GPIO_MF_OPA_O)                                                    \
                    pGPIO->MF0_b.PORT_1 = 2;                                                          \
                else if (nMFType == GPIO_MF_UART0_TX)                                                 \
                    pGPIO->MF0_b.PORT_1 = 4;                                                          \
                else if (nMFType == GPIO_MF_SYSTEM)                                                   \
                    pGPIO->MF0_b.PORT_1 = 0x0F;                                                       \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_1 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN2)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_2 = 1;                                                          \
                else if (nMFType == GPIO_MF_OPA_I)                                                    \
                    pGPIO->MF0_b.PORT_2 = 0x02;                                                       \
                else if (nMFType == GPIO_MF_I2C0_SCL)                                                 \
                    pGPIO->MF0_b.PORT_2 = 0x05;                                                       \
                else if (nMFType == GPIO_MF_SYSTEM)                                                   \
                    pGPIO->MF0_b.PORT_2 = 0x0F;                                                       \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_2 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN3)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_3 = 1;                                                          \
                else if (nMFType == GPIO_MF_OPA_I)                                                    \
                    pGPIO->MF0_b.PORT_3 = 0x02;                                                       \
                else if (nMFType == GPIO_MF_I2C0_SDA)                                                 \
                    pGPIO->MF0_b.PORT_3 = 0x05;                                                       \
                else if (nMFType == GPIO_MF_SYSTEM)                                                   \
                    pGPIO->MF0_b.PORT_3 = 0x0F;                                                       \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_3 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN4)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_OPA_O)                                                         \
                    pGPIO->MF0_b.PORT_4 = 2;                                                          \
                else if ((nMFType == GPIO_MF_SPI_MASTER_SS) || (nMFType == GPIO_MF_SPI_SLAVE_SS))     \
                    pGPIO->MF0_b.PORT_4 = 3;                                                          \
                else if (nMFType == GPIO_MF_SYSTEM)                                                   \
                    pGPIO->MF0_b.PORT_4 = 0x0F;                                                       \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_4 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN5)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_OPA_I)                                                         \
                    pGPIO->MF0_b.PORT_5 = 2;                                                          \
                else if ((nMFType == GPIO_MF_SPI_MASTER_CLK) || (nMFType == GPIO_MF_SPI_SLAVE_CLK))   \
                    pGPIO->MF0_b.PORT_5 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_5 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN6)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_TIMER16B0_MAT)                                                 \
                    pGPIO->MF0_b.PORT_6 = 1;                                                          \
                else if (nMFType == GPIO_MF_OPA_I)                                                    \
                    pGPIO->MF0_b.PORT_6 = 2;                                                          \
                else if ((nMFType == GPIO_MF_SPI_MASTER_MOSI) || (nMFType == GPIO_MF_SPI_SLAVE_MOSI)) \
                    pGPIO->MF0_b.PORT_6 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_6 = 0;                                                          \
            }                                                                                         \
            else if (nPinSel & GPIO_PIN7)                                                             \
            {                                                                                         \
                if (nMFType == GPIO_MF_OPA_O)                                                         \
                    pGPIO->MF0_b.PORT_7 = 2;                                                          \
                else if ((nMFType == GPIO_MF_SPI_MASTER_MISO) || (nMFType == GPIO_MF_SPI_SLAVE_MISO)) \
                    pGPIO->MF0_b.PORT_7 = 3;                                                          \
                else                                                                                  \
                    pGPIO->MF0_b.PORT_7 = 0;                                                          \
            }                                                                                         \
        }                                                                                             \
        GPIO_SetPinMode(pGPIO, nPinSel, nPINMODE);                                                    \
    } while (0)

/*********************Enum**********************/
typedef enum{
    OB_GPIO0_ID = 0,
    OB_GPIO1_ID,
    OB_GPIO2_ID,
    OB_GPIO3_ID,

    OB_GPIO_CNT,
}gpio_port_id_t;


typedef enum{
    GPIO_PIN0_ID = 0,
    GPIO_PIN1_ID,
    GPIO_PIN2_ID,
    GPIO_PIN3_ID,
    GPIO_PIN4_ID,
    GPIO_PIN5_ID,
    GPIO_PIN6_ID,
    GPIO_PIN7_ID,
    GPIO_PIN8_ID,
    GPIO_PIN9_ID,
    GPIO_PIN10_ID,
    GPIO_PIN11_ID,
    GPIO_PIN12_ID,
    GPIO_PIN13_ID,
    GPIO_PIN14_ID,
    GPIO_PIN15_ID,

    GPIO_PIN_CNT,
}gpio_pin_id_t;

/*********************Struct********************/


/*********************Variable******************/
extern volatile uint32_t g_GPIO_Interrupt[OB_GPIO_CNT];

/*********************Function******************/


/***********************************************/
#endif
