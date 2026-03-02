
#ifndef _SOFTWARE_SPI_H_
#define _SOFTWARE_SPI_H_

#include <stdint.h>
#include "string.h"

#define SOFTWARE_SPI_NUM 1


void software_spi_init(hal_spi_config_t const *p_config);
void software_spi_enable(uint16_t spi);
void software_spi_disable(uint16_t spi);
uint32_t software_spi_write(uint16_t spi,const uint8_t *p_data,uint32_t size);
uint32_t software_spi_read(uint16_t spi,uint8_t *p_data,uint32_t size);
#endif 
