#include "DEVICE_CFG.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "DEVICE_CFG"

/***************Variable***************/


// ------------------------------------------
/************************** SPI drive config *************************/
// 1. 实现GPIO操作函数
static inline void NFC_SPI_Init(void) {

    HAL_GPIO_Init(NFC_NSS_GPIO,NFC_NSS_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(NFC_NSS_GPIO,NFC_NSS_PIN,1);
    HAL_GPIO_Init(NFC_CLK_GPIO,NFC_CLK_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(NFC_CLK_GPIO,NFC_CLK_PIN,1);
    HAL_GPIO_Init(NFC_MOSI_GPIO,NFC_MOSI_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(NFC_MOSI_GPIO,NFC_MOSI_PIN,1);
    HAL_GPIO_Init(NFC_MISO_GPIO,NFC_MISO_PIN,HAL_GPIO_MODE_INPUT_PULLUP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(NFC_MISO_GPIO,NFC_MISO_PIN,1);
}

// 1. 实现GPIO操作函数
static inline void NFC_SPI_SetCS(bool high) {
    HAL_GPIO_Write(NFC_NSS_GPIO,NFC_NSS_PIN, high);
}

static inline void NFC_SPI_SetCLK(bool high) {
    HAL_GPIO_Write(NFC_CLK_GPIO,NFC_CLK_PIN,high);
}


static inline void NFC_SPI_SetMOSI(bool high) {
    HAL_GPIO_Write(NFC_MOSI_GPIO,NFC_MOSI_PIN,high);
}

static inline bool NFC_SPI_GetMISO(void) {
	return (HAL_GPIO_Read(NFC_MISO_GPIO, NFC_MISO_PIN)) ? true : false;
}

static const SPI_Config nfc_spi_config = {
    .ops = {
        .init = NFC_SPI_Init,
        .set_cs = NFC_SPI_SetCS,
        .set_clk = NFC_SPI_SetCLK,
        .set_mosi = NFC_SPI_SetMOSI,
        .get_miso = NFC_SPI_GetMISO,
    },
};

uint8_t nfc_hardware_init(void) {
#if (NFC_SOPT_SPI_SEL)
	// SPI init
    if (SPI_Init(&nfc_spi_config)) {
        HAL_GPIO_Init(NFC_NRST_GPIO,NFC_NRST_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
        HAL_GPIO_Write(NFC_NRST_GPIO,NFC_NRST_PIN,1);
        HAL_GPIO_Init(NFC_IRQ_GPIO,NFC_IRQ_PIN,HAL_GPIO_MODE_INPUT,HAL_GPIO_PULL_HIGH);

        return true;
    }

    return false;

#else
    const hal_spi_config_t config = {
        .spi       = NFC_SPI,
		.type      = NFC_SPI_TYPE,
        .ss_port   = NFC_NSS_GPIO,
        .ss_pin    = NFC_NSS_PIN,
        .sck_port  = NFC_CLK_GPIO,
        .sck_pin   = NFC_CLK_PIN,
        .mosi_port = NFC_MOSI_GPIO,
        .mosi_pin  = NFC_MOSI_PIN,
        .miso_port = NFC_MISO_GPIO,
        .miso_pin  = NFC_MISO_PIN,
    };
    hal_spi_init(&config);

    HAL_GPIO_Init(NFC_NRST_GPIO,NFC_NRST_PIN,HAL_GPIO_MODE_OUTPUT_PP,HAL_GPIO_PULL_NONE);
    HAL_GPIO_Write(NFC_NRST_GPIO,NFC_NRST_PIN,1);
    HAL_GPIO_Init(NFC_IRQ_GPIO,NFC_IRQ_PIN,HAL_GPIO_MODE_INPUT,HAL_GPIO_PULL_HIGH);
    return true;
#endif
}

//***********************************************
//函数名称：NfcGetReg(uint8_t reg_addr, uint8_t reg_data)
//函数功能：写入寄存器值
//入口参数：reg_addr:目标寄存器地址   reg_data:读取的值
//出口参数：uint8_t  true：成功   false:失败
//***********************************************
uint8_t NfcSetReg(uint8_t reg_addr, uint8_t reg_data) {

    reg_addr = (reg_addr << 1) & 0x7F;
#if (NFC_SOPT_SPI_SEL)
    return SPI_WriteReg(&nfc_spi_config, reg_addr, reg_data);

#else
    hal_spi_enable(NFC_SPI);
    hal_spi_write(NFC_SPI, &reg_addr, sizeof(uint8_t));
    hal_spi_write(NFC_SPI, &reg_data, sizeof(uint8_t));
    hal_spi_disable(NFC_SPI);
    return true;

#endif
}

//***********************************************
//函数名称：NfcGetReg(uint8_t reg_addr, uint8_t *reg_data)
//函数功能：读取寄存器值
//入口参数：reg_addr:目标寄存器地址   reg_data:读取的值
//出口参数：uint8_t  true：成功   false:失败
//***********************************************
uint8_t NfcGetReg(uint8_t reg_addr, uint8_t *reg_data) {
    reg_addr = (reg_addr << 1) | 0x80;

#if (NFC_SOPT_SPI_SEL)
    return SPI_ReadReg(&nfc_spi_config, reg_addr, reg_data);

#else
    hal_spi_enable(NFC_SPI);
    hal_spi_write(NFC_SPI, &reg_addr, sizeof(uint8_t));
    hal_spi_read(NFC_SPI, reg_data, sizeof(uint8_t));
    hal_spi_disable(NFC_SPI);
    
    return true;

#endif
}

//*******************************************************
//函数名称：NfcModifyReg(unsigned char addr,unsigned char* mask,unsigned char set)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   mask:要改变的位  
//         set:  0:标志的位清零   其它:标志的位置起
//出口参数：
//********************************************************
uint8_t NfcModifyReg(uint8_t addr, uint8_t mask, uint8_t set) {
    uint8_t regdata;
    if (NfcGetReg(addr, &regdata)) {
        if(set) {
            regdata |= mask;
        }
        else {
            regdata &= (~mask);
        }
        
        return NfcSetReg(addr, regdata);
    }

    return false;
}

uint8_t Write_Reg(uint8_t reg_addr, uint8_t reg_value) {
	return NfcSetReg(reg_addr, reg_value);
}

uint8_t Read_Reg(uint8_t reg_addr) {
    uint8_t reg_value;
    if (NfcGetReg(reg_addr, &reg_value)) {
        return reg_value;
    }

    return 0;
}

uint8_t GetFIFOData(uint8_t addr, uint8_t *reg_data, uint8_t size) {	
    for(uint8_t i=0; i<size; i++){
        NfcGetReg(addr, reg_data+i);
    }
	return true;
}

uint8_t SetFIFOData(uint8_t addr, uint8_t *reg_data, uint8_t size) {
    for(uint8_t i=0; i<size; i++){
        NfcSetReg(addr, reg_data[i]);
    }

	return true;
}

/*************************************************************
函数名：	    Set_BitMask
功能：	    置位寄存器操作
输入参数：	reg_add，寄存器地址；mask，寄存器置位
返回值：	    OK
                ERROR
*************************************************************/
unsigned char Set_BitMask(unsigned char reg_add,unsigned char mask) {
    NfcModifyReg(reg_add,mask,1);
	return 0;
}

//实测1ms
void mDelay(uint16_t ms) {
	delay_ms(ms);
}
