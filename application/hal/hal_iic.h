#ifndef __HAL_IIC_H
#define __HAL_IIC_H
#include <stdint.h>

#define IIC_OK      0
#define IIC_ACK     1
#define IIC_NACK    0

#define HAL_IIC0    0
#define HAL_IIC1    1
#define HAL_IIC2    2
#define HAL_IIC3    3

#define IIC_MODE0   0   //标准模式
#define IIC_MODE1   1   //时钟延展模式

typedef struct
{
    uint16_t iic;
    uint32_t sck_port;
    uint32_t sck_pin;   ///<SCK pin number.
    uint32_t sda_port;
    uint32_t sda_pin;   ///<SDA pin number.
    uint32_t frequency; ///<IIC frequency.
    uint16_t wait_scl_ticks; //模式1，时钟延展使用
    uint8_t  enable;
    uint8_t  mode;      ///<IIC mode.
    uint16_t addr;
} hal_iic_config_t;

uint32_t hal_i2c_init(const hal_iic_config_t *config);
uint32_t hal_i2c_write(uint16_t i2c, uint8_t reg_addr, uint8_t *buf, uint16_t sz);
uint32_t hal_i2c_read(uint16_t i2c, uint8_t reg_addr, uint8_t *buf, uint16_t sz);

#endif //__HAL_IIC_H
