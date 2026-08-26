#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_spi_software.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "hal_sw_spi"

static uint8_t spi_index = 0;
static hal_spi_config_t hal_spi_config[SOFTWARE_SPI_NUM];

void software_spi_init(hal_spi_config_t const *p_config)
{
    if(spi_index < SOFTWARE_SPI_NUM) {
        memcpy(&hal_spi_config[spi_index], p_config, sizeof(hal_spi_config_t));

        HAL_GPIO_Init(hal_convert_gpio_port(hal_spi_config[spi_index].ss_port),hal_convert_gpio_pin(hal_spi_config[spi_index].ss_pin),HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
        HAL_GPIO_Init(hal_convert_gpio_port(hal_spi_config[spi_index].mosi_port),hal_convert_gpio_pin(hal_spi_config[spi_index].mosi_pin),HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
        HAL_GPIO_Init(hal_convert_gpio_port(hal_spi_config[spi_index].sck_port),hal_convert_gpio_pin(hal_spi_config[spi_index].sck_pin),HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
        HAL_GPIO_Init(hal_convert_gpio_port(hal_spi_config[spi_index].miso_port),hal_convert_gpio_pin(hal_spi_config[spi_index].miso_pin),HAL_GPIO_MODE_INPUT,HAL_GPIO_PULL_NONE);

        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[spi_index].ss_port),hal_convert_gpio_pin(hal_spi_config[spi_index].ss_pin),1);
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[spi_index].sck_port),hal_convert_gpio_pin(hal_spi_config[spi_index].sck_pin),0);
        spi_index++;
    }
}

static inline void spi_sck_set(uint8_t index,uint8_t value)
{
    if(value){
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].sck_port),hal_convert_gpio_pin(hal_spi_config[index].sck_pin),1);
    }
    else{
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].sck_port),hal_convert_gpio_pin(hal_spi_config[index].sck_pin),0);
    }
}

static inline void spi_mosi_set(uint8_t index,uint8_t value)
{
    if(value){
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].mosi_port),hal_convert_gpio_pin(hal_spi_config[index].mosi_pin),1);
    }
    else{
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].mosi_port),hal_convert_gpio_pin(hal_spi_config[index].mosi_pin),0);
    }
}

static inline uint32_t spi_miso_read(uint8_t index)
{
    return HAL_GPIO_Read(hal_convert_gpio_port(hal_spi_config[index].miso_port),hal_convert_gpio_pin(hal_spi_config[index].miso_pin));
}

static inline uint32_t spi_index_get(uint16_t spi,uint8_t *index)
{
    uint32_t ret = false;
    for(uint8_t i=0;i<spi_index;i++){
        if(hal_spi_config[i].spi == spi){
            *index = i;
            ret = true;
            break;
        }
    }
    return ret;
}


void software_spi_enable(uint16_t spi)
{
    uint8_t index = 0;

    if(spi_index_get(spi,&index) == true){
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].ss_port),hal_convert_gpio_pin(hal_spi_config[index].ss_pin),0);
    }
}
void software_spi_disable(uint16_t spi)
{
    uint8_t index = 0;
    if(spi_index_get(spi,&index) == true){
        HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].ss_port),hal_convert_gpio_pin(hal_spi_config[index].ss_pin),1);
    }
}

static inline uint8_t spi_byte(uint8_t index,uint8_t data)
{
    uint8_t byte_rx=0;

    spi_sck_set(index,0);
    for(uint8_t i=0;i<8;i++){
        spi_mosi_set(index,data&(0x80>>i));
        spi_sck_set(index,1);
        if(spi_miso_read(index)){
            byte_rx |= (0x80>>i);
        }
        spi_sck_set(index,0);
    }
    return byte_rx;
}



uint32_t software_spi_write(uint16_t spi,const uint8_t *p_data,uint32_t size)
{
    uint8_t  index = 0;
    uint32_t ret;

    ret = spi_index_get(spi, &index);

    if(ret == true) {
        // HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].ss_port),hal_convert_gpio_pin(hal_spi_config[index].ss_pin),0);
        for(uint32_t i = 0; i < size; i++) {
            spi_byte(index, p_data[i]);
        }
        // HAL_GPIO_Write(hal_convert_gpio_port(hal_spi_config[index].ss_port),hal_convert_gpio_pin(hal_spi_config[index].ss_pin),1);
    }
    return ret;
}

uint32_t software_spi_read(uint16_t spi,uint8_t *p_data,uint32_t size)
{
    uint8_t  index = 0;
    uint32_t ret;

    ret = spi_index_get(spi, &index);
    if(ret == true) {
        for(uint32_t i = 0; i < size; i++) {
            p_data[i] = spi_byte(index,0x00);
        }
    }
    return ret;
}




