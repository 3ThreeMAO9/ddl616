#include "FM17622.h"
#include "READER_API.h"
#include "MIFARE.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "FM17622"

/***************Variable***************/


// ------------------------------------------

void Read_FIFO(uint8_t *fifo_data, uint8_t length) {
	GetFIFOData(JREG_FIFODATA, fifo_data, length);
}

void Write_FIFO(uint8_t *fifo_data, uint8_t length) {
	SetFIFOData(JREG_FIFODATA, fifo_data, length);
}

void Clear_FIFO(void) {
	uint8_t regdata;

	NfcGetReg(JREG_FIFOLEVEL, &regdata);
	if(regdata & 0x7F)			//FIFO如果不空，则FLUSH FIFO
	{
	    NfcSetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO);
	}
}

//***********************************************
//函数名称：GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//函数功能：读取扩展寄存器值
//入口参数：ExtRegAddr:扩展寄存器地址   ExtRegData:读取的值
//出口参数：unsigned char  TRUE：读取成功   FALSE:失败
//***********************************************
uint8_t GetReg_Ext(uint8_t ext_reg_address,uint8_t* ext_reg_data) {
	NfcSetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_RD_ADDR + ext_reg_address);
	NfcGetReg(JREG_EXT_REG_ENTRANCE,&(*ext_reg_data));
	return true;	
}
//***********************************************
//函数名称：SetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//函数功能：写扩展寄存器
//入口参数：ExtRegAddr:扩展寄存器地址   ExtRegData:要写入的值
//出口参数：unsigned char  TRUE：写成功   FALSE:写失败
//***********************************************
uint8_t SetReg_Ext(uint8_t ext_reg_address,uint8_t ext_reg_data) {
	NfcSetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_ADDR + ext_reg_address);
	NfcSetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_DATA + ext_reg_data);
	return true; 	
}
 
//*******************************************************
//函数名称：ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char* mask,unsigned char set)
//函数功能：寄存器位操作
//入口参数：ExtRegAddr:目标寄存器地址   mask:要改变的位  
//         set:  0:标志的位清零   其它:标志的位置起
//出口参数：unsigned char  TRUE：写成功   FALSE:写失败
//********************************************************
void ModifyReg_Ext(uint8_t ExtRegAddr, uint8_t mask, uint8_t set) {
    uint8_t regdata;

    GetReg_Ext(ExtRegAddr, &regdata);
    if(set) {
        regdata |= mask;
    }
    else {
        regdata &= ~(mask);
    }
    SetReg_Ext(ExtRegAddr, regdata);
}

//*************************************
//函数  名：SetCW 载波设置
//入口参数：
//出口参数：
//*************************************

uint8_t SetCW(uint8_t cw_mode) {
    switch (cw_mode) {
        case TX1_TX2_CW_DISABLE:
            NfcModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN | JBIT_TX2RFEN, 0);
            break;
        case TX1_CW_ENABLE:
            NfcModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN, 1);
            NfcModifyReg(JREG_TXCONTROL, JBIT_TX2RFEN, 0);
            break;
        case TX2_CW_ENABLE:
            NfcModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN, 0);
            NfcModifyReg(JREG_TXCONTROL, JBIT_TX2RFEN, 1);
            break;
        case TX1_TX2_CW_ENABLE:
            NfcModifyReg(JREG_TXCONTROL, JBIT_TX1RFEN | JBIT_TX2RFEN, 1);
            break;
        
        default:
            return false;
    }

    return true;
}

unsigned char FM17622_SoftReset(void) {
	NfcSetReg(JREG_COMMAND, CMD_SOFT_RESET);
    
	//Wait 2
	mDelay(1);
    if (0x20 != Read_Reg(JREG_COMMAND)) {
        return false;
    }

	return true;
}

void FM17622_HardReset(void) {
    CLR_NFC_NRST_LOW();
    mDelay(1);
    SET_NFC_NRST_HIGH();
    mDelay(1);
}

void FM17622_DeepSleep(void) {
    // 硬件复位后，拉低reset脚
    FM17622_HardReset();

    CLR_NFC_NRST_LOW();
}

uint8_t FM17622_Version(void) {
    uint8_t reg_data;

    if (NfcGetReg(JREG_VERSION, &reg_data)) {
        OB_LOGD(TAG, "NFC IC Version: %02X", reg_data);
        if ((0x00 != reg_data) && (0xFF != reg_data)) {
            return true;
        }
    }

    return false;
}

/** 
 * @brief 发送和接收7bit位的数据(用于后门指令)
 */
uint8_t FM17622_Transceive7Bits(uint8_t data_7bit,
                            uint8_t* pOutData,
                            uint8_t* pOutLenbit) {
    uint8_t status = false;
    uint8_t reg_data;
    uint8_t time_out;

    // Clear FIFO
    NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    NfcSetReg(JREG_FIFOLEVEL, JBIT_FLUSHFIFO);

    // 配置7位传输
    NfcSetReg(JREG_BITFRAMING, 0x07);

    // 写入7位数据到FIFO
    NfcSetReg(JREG_FIFODATA, data_7bit);

    // 执行传输
    NfcSetReg(JREG_COMMAND, CMD_TRANSCEIVE);//command = Transceive
    NfcSetReg(JREG_BITFRAMING, 0x87);//Start Send

    // 实测GEN卡应答小于1.4ms，因此缩短了该处的超时时间，如部分GEN卡无法正常识别，可尝试延迟
    if ((NULL != pOutData) && (NULL != pOutLenbit)) {
        reg_data = 0;
        time_out = 10;
        do{
            // 读取响应
            NfcGetReg(JREG_FIFOLEVEL, &reg_data);

            if (reg_data) {
                Read_FIFO(pOutData, reg_data);
                *pOutLenbit = ((Read_Reg(JREG_CONTROL) & 0x07) + ((reg_data - 1) / 8));

                OB_LOGD(TAG, "pOutLenbit[%u]", *pOutLenbit);
                OB_LOGD_DUMP(pOutData, reg_data);
                status = true;
                break;
            }
            delay_us(100);
        }while(time_out--);
    }

    // 清理
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    NfcSetReg(JREG_BITFRAMING, 0x80);

    // 配置7位传输
    reg_data = 0;
    NfcGetReg(JREG_BITFRAMING, &reg_data);
    reg_data = (reg_data & 0x80);   // 高5bit保持，设置低3bit
	NfcSetReg(JREG_BITFRAMING, reg_data);

    return status;
}
