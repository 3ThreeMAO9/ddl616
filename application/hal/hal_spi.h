
#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

#include <stdint.h>
#include "string.h"

#define HAL_SPI0    0
#define HAL_SPI1    1
#define HAL_SPI2    2
#define HAL_SPI3    3
#define HAL_SPI4    4
#define HAL_SPI5    5
#define HAL_SPI6    6
#define HAL_SPI7    7

#define HAL_SPI_TYPE_HW 0
#define HAL_SPI_TYPE_SW 1






typedef struct
{
    uint32_t sck_port; 
    uint32_t sck_pin;      ///< SCK pin number.
    uint32_t mosi_port;
    uint32_t mosi_pin;     ///< MOSI pin number (optional).
                          /**< Set to @ref NRF_DRV_SPI_PIN_NOT_USED
                           *   if this signal is not needed. */
    uint32_t miso_port;
    uint32_t miso_pin;     ///< MISO pin number (optional).
                          /**< Set to @ref NRF_DRV_SPI_PIN_NOT_USED
                           *   if this signal is not needed. */
    uint32_t ss_port;
    uint32_t ss_pin;       ///< Slave Select pin number (optional).
                          /**< Set to @ref NRF_DRV_SPI_PIN_NOT_USED
                           *   if this signal is not needed. The driver
                           *   supports only active low for this signal.
                           *   If the signal should be active high,
                           *   it must be controlled externally. */

    uint32_t frequency;     ///< SPI frequency.
    uint8_t  mode;      ///< SPI mode.
    uint8_t  type;    //HW or SW
    uint16_t  spi;
}hal_spi_config_t;

void hal_spi_init(hal_spi_config_t const *p_config);

void hal_spi_enable(uint16_t spi);
void hal_spi_disable(uint16_t spi);
uint32_t hal_spi_write(uint16_t spi,const uint8_t *p_data,uint32_t size);

uint32_t hal_spi_read(uint16_t spi,uint8_t *p_data,uint32_t size);


#endif 
