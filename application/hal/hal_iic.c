#include "hal_iic.h"
#include "i2c_software.h"

void hal_iic_delay_us(uint32_t us)
{
    volatile uint32_t i, j;
    for(i = 0; i < us; i++)
        j;
}

uint32_t hal_i2c_init(const hal_iic_config_t* config)
{
    // software_iic_init(config);
    return 0;
}

uint32_t hal_i2c_write(uint16_t i2c, uint8_t reg_addr, uint8_t* buf, uint16_t sz)
{
    // return software_iic_write(i2c, reg_addr, buf, sz);
    return 0;
}

uint32_t hal_i2c_read(uint16_t i2c, uint8_t reg_addr, uint8_t* buf, uint16_t sz)
{
    // return software_iic_read(i2c, reg_addr, buf, sz);
    return 0;
}


