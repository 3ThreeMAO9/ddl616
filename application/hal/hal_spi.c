
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_spi_software.h"
#include "spi.h"
#include "gpio.h"

#define HAL_SPI_MAX 2

static hal_spi_config_t spi_config[HAL_SPI_MAX] = {0};

void hal_spi_init(hal_spi_config_t const *p_config)
{
    if (p_config->spi > HAL_SPI_MAX)
    {
        return;
    }
    memcpy(&spi_config[p_config->spi], p_config, sizeof(hal_spi_config_t));
    if (p_config->type == HAL_SPI_TYPE_HW)
    {
        HAL_GPIO_Init(hal_convert_gpio_port(p_config->ss_port), hal_convert_gpio_pin(p_config->ss_pin), HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
        HAL_GPIO_Write(hal_convert_gpio_port(p_config->ss_port), hal_convert_gpio_pin(p_config->ss_pin), 1);

        // GPIO_SetPinMFType(OB_GPIO3, GPIO_PIN4, GPIO_MF_SPI_MASTER_SS, GPIO_PINMODE_PULL_UP);   // SSP_SS
        GPIO_SetPinMFType(OB_GPIO3, GPIO_PIN5, GPIO_MF_SPI_MASTER_CLK, GPIO_PINMODE_PULL_UP);  // SSP_CLK
        GPIO_SetPinMFType(OB_GPIO3, GPIO_PIN6, GPIO_MF_SPI_MASTER_MOSI, GPIO_PINMODE_PULL_UP); // SSP_MOSI
        GPIO_SetPinMFType(OB_GPIO3, GPIO_PIN7, GPIO_MF_SPI_MASTER_MISO, GPIO_PINMODE_PULL_UP); // SSP_MISO

        SPI_MasterOpen(OB_SPI, (SPI_OUT_PHASE_FIRST | SPI_OUT_POLARITY_HI | SPI_SIZE_8BIT ), 6000000);
        // SPI_EnableINT(OB_SPI, SPI_INT_RTIM);
    }
    else
    {
        software_spi_init(p_config);
    }
}

void hal_spi_enable(uint16_t spi)
{
    if (spi > HAL_SPI_MAX)
    {
        return;
    }

    if (spi_config[spi].type == HAL_SPI_TYPE_HW)
    {
        // HAL_GPIO_Init(hal_convert_gpio_port(spi_config[spi].ss_port), hal_convert_gpio_pin(spi_config[spi].ss_pin), HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
        HAL_GPIO_Write(hal_convert_gpio_port(spi_config[spi].ss_port), hal_convert_gpio_pin(spi_config[spi].ss_pin), 0);
    }
    else
    {
        software_spi_enable(spi);
    }
}
void hal_spi_disable(uint16_t spi)
{
    if (spi_config[spi].type == HAL_SPI_TYPE_HW)
    {
        // HAL_GPIO_Init(hal_convert_gpio_port(spi_config[spi].ss_port), hal_convert_gpio_pin(spi_config[spi].ss_pin), HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);
        HAL_GPIO_Write(hal_convert_gpio_port(spi_config[spi].ss_port), hal_convert_gpio_pin(spi_config[spi].ss_pin), 1);
    }
    else
    {
        software_spi_disable(spi);
    }
}

uint32_t hal_spi_write(uint16_t spi, const uint8_t *p_data, uint32_t size)
{
    if (p_data == NULL || size == 0)
        return 0;  // 参数错误

    if (spi_config[spi].type == HAL_SPI_TYPE_HW)
    {
        return SPI_WriteFIFO(OB_SPI, p_data, size);  // 返回实际写入的字节数
    }
    else
    {
        software_spi_write(spi, p_data, size);
    }
    return 0;
}

uint32_t hal_spi_read(uint16_t spi, uint8_t *p_data, uint32_t size)
{
    if (p_data == NULL || size == 0)
        return 0;  // 参数错误

    if (spi_config[spi].type == HAL_SPI_TYPE_HW)
    {
        return SPI_ReadWithClock(OB_SPI, p_data, size);  // 返回实际读取的字节数
    }
    else
    {
        software_spi_read(spi, p_data, size);
    }
    return 0;
}
