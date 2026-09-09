
#include "READER_API.h"
#include "FM17622.h"
#include "DEVICE_CFG.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "reader_api"

/***************Variable***************/
static const unsigned char RF_CMD_REQA    = 0x26;
static const unsigned char RF_CMD_WUPA    = 0x52;
static const unsigned char RF_CMD_ANTICOL = 0x93;
static const unsigned char RF_CMD_SELECT  = 0x93;

#if (Enabled == CARD_TYPE_A_ENABLE)
picc_a_t PICC_A;
#endif

#if (Enabled == CARD_TYPE_B_ENABLE)
picc_b_t PICC_B;
#endif

// ------------------------------------------

#if (Enabled == CARD_TYPE_A_ENABLE)
//*************************************
//函数  名：FM17622_Initial_ReaderA
//入口参数：
//出口参数：
//*************************************

void FM17622_Initial_ReaderA(void) {
	NfcSetReg(JREG_MODWIDTH,MODWIDTH_106);	//MODWIDTH = 106kbps
	NfcModifyReg(JREG_TXAUTO,BIT6,1);//Force 100ASK = 1
	NfcSetReg(JREG_GSN,(GSNON_A<<4));//Config GSN; Config ModGSN 	
	NfcSetReg(JREG_CWGSP,NFC_READ_CARD_POWER);//Config GSP
	NfcSetReg(JREG_CONTROL,BIT4);//Initiator = 1
	NfcSetReg(JREG_RFCFG,(RXGAIN_A<<4));//Config RxGain
	NfcSetReg(JREG_RXTRESHOLD,(MINLEVEL_A<<4) | COLLLEVEL_A);//Config MinLevel; Config CollLevel	
}

//*************************************
//函数  名：ReaderA_Halt
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderA_Halt(void)
{	
	unsigned char reg_data;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x80);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,0x50);
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	//Wait 2ms
	mDelay(2);//Wait 2ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 0)
	{
		OB_LOGD(TAG, "Success: Halt");
		return FM17622_SUCCESS;
	}
	OB_LOGD(TAG, "Fail: Halt");
	return FM17622_COMM_ERR;
	
}
//*************************************
//函数  名：ReaderA_Wakeup
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderA_Wakeup(void)
{
	unsigned char reg_data;
	NfcSetReg(JREG_TXMODE,0);//Disable TxCRC
	NfcSetReg(JREG_RXMODE,0);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,RF_CMD_WUPA);
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x87);//Start Send

	mDelay(1);//Wait 1ms

	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 2)
	{
		NfcGetReg(JREG_FIFODATA,PICC_A.ATQA);
		NfcGetReg(JREG_FIFODATA,PICC_A.ATQA+1);
		OB_LOGD(TAG, "Success: Wakeup");
		return FM17622_SUCCESS;
	}
	return FM17622_COMM_ERR;
}

//*************************************
//函数  名：ReaderA_Request
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderA_Request(void)
{
	uint8_t reg_data;

	NfcSetReg(JREG_TXMODE,0);//Disable TxCRC
	NfcSetReg(JREG_RXMODE,0);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,RF_CMD_REQA);
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x87);//Start Send
	//Wait 2ms
	uint8_t i = 100;
	do {

		delay_us(20);
		if (i) {
			i--;
		}
		else {
			return FM17622_COMM_ERR;
		}
		NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	}while(2 != reg_data);

	Read_FIFO(PICC_A.ATQA,sizeof(PICC_A.ATQA));
	OB_LOGD(TAG, "Success: Request");
	return FM17622_SUCCESS;
}

//*************************************
//函数  名：ReaderA_AntiColl
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderA_AntiColl(void)
{
	unsigned char reg_data;
	NfcSetReg(JREG_TXMODE,0);//Disable TxCRC
	NfcSetReg(JREG_RXMODE,0);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,RF_CMD_ANTICOL);
	NfcSetReg(JREG_FIFODATA,0x20);
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	//Wait 2ms
	uint8_t i = 100;
	do {

		delay_us(20);
		if (i) {
			i--;
		}
		else {
			OB_LOGD(TAG, "Fail: Anticoll");
			return FM17622_COMM_ERR;
		}
		NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	}while(5 != reg_data);
	Read_FIFO(PICC_A.UID, reg_data);
	if( (PICC_A.UID[0] ^ PICC_A.UID[1] ^ PICC_A.UID[2] ^ PICC_A.UID[3]) == PICC_A.BCC) {
		OB_LOGD(TAG, "Success: Anticoll");
		return FM17622_SUCCESS;
	}

	return FM17622_CRC_ERR;
}

//*************************************
//函数  名：ReaderA_Select
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderA_Select(void)
{
	unsigned char reg_data;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x80);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,RF_CMD_SELECT);
	NfcSetReg(JREG_FIFODATA,0x70);
	NfcSetReg(JREG_FIFODATA,PICC_A.UID[0]);
	NfcSetReg(JREG_FIFODATA,PICC_A.UID[1]);
	NfcSetReg(JREG_FIFODATA,PICC_A.UID[2]);
	NfcSetReg(JREG_FIFODATA,PICC_A.UID[3]);
	NfcSetReg(JREG_FIFODATA,PICC_A.BCC);		
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	//Wait 2ms
	uint8_t i = 100;
	do {

		delay_us(20);
		if (i) {
			i--;
		}
		else {
			OB_LOGD(TAG, "Fail: Select");
			return FM17622_COMM_ERR;
		}
		NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	}while(1 != reg_data);

	NfcGetReg(JREG_FIFODATA,&PICC_A.SAK);
	OB_LOGD(TAG, "Success: Select");
	return FM17622_SUCCESS;
}

//*************************************
//函数  名：ReaderA_CardActivate
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderA_CardActivate(void) {
	if (FM17622_SUCCESS == ReaderA_Request()) {		// 实测< 1.3ms	
		if (FM17622_SUCCESS == ReaderA_AntiColl()) {		// 实测< 2.0ms
			if (FM17622_SUCCESS == ReaderA_Select()) {		// 实测< 1.9ms

				return FM17622_SUCCESS;
			}
		}
	}

	return FM17622_COMM_ERR;
}

uint8_t ReaderA_CardInfo(picc_a_t* info) {
	memcpy((uint8_t*)(info), (uint8_t*)(&PICC_A), sizeof(picc_a_t));
	return true;
}
#endif

#if (Enabled == CARD_TYPE_B_ENABLE)
//*************************************
//函数  名：FM17622_Initial_ReaderB
//入口参数：
//出口参数：
//*************************************

void FM17622_Initial_ReaderB(void)
{
	NfcModifyReg(JREG_STATUS2,BIT3,0);

	NfcSetReg(JREG_MODWIDTH,MODWIDTH_106);//MODWIDTH = 106kbps	
	NfcSetReg(JREG_TXAUTO,0);//Force 100ASK = 0		
	NfcSetReg(JREG_GSN,(GSNON_B<<4)|MODGSNON_B);//Config GSN; Config ModGSN   
	NfcSetReg(JREG_CWGSP,GSP_B);//Config GSP
	NfcSetReg(JREG_MODGSP,MODGSP_B);//Config ModGSP
	NfcSetReg(JREG_CONTROL,BIT4);//Initiator = 1
	NfcSetReg(JREG_RFCFG,RXGAIN_B<<4);//Config RxGain
	NfcSetReg(JREG_RXTRESHOLD,MINLEVEL_B<<4);//Config MinLevel;
	return;
}
//*************************************
//函数  名：ReaderB_Wakeup
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderB_Wakeup(void)
{
	uint8_t reg_data;
	NfcSetReg(JREG_TXMODE,0x83);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x83);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,0x05);
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_FIFODATA,0x08);
	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 12)
	{
		//for(i = 0;i < 12;i++)
		Read_FIFO(&PICC_B.ATQB[0],sizeof(PICC_B.ATQB));
		memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
		memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
		memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
		return FM17622_SUCCESS;
	}
	return FM17622_COMM_ERR;	
}
//*************************************
//函数  名：ReaderB_Request
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderB_Request(void)
{	
	uint8_t reg_data;
	NfcSetReg(JREG_TXMODE,0x83);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x83);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,0x05);
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_FIFODATA,0x00);
	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 12)
	{
		//for(i = 0;i < 12;i++)
		Read_FIFO(&PICC_B.ATQB[0],sizeof(PICC_B.ATQB));
		
		memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
		memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
		memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
			return FM17622_SUCCESS;
	}
	return FM17622_COMM_ERR;	
}
//*************************************
//函数  名：ReaderB_Attrib
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderB_Attrib(void)
{
	unsigned char reg_data;
	NfcSetReg(JREG_TXMODE,0x83);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x83);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,0x1D);
	NfcSetReg(JREG_FIFODATA,PICC_B.PUPI[0]);
	NfcSetReg(JREG_FIFODATA,PICC_B.PUPI[1]);
	NfcSetReg(JREG_FIFODATA,PICC_B.PUPI[2]);
	NfcSetReg(JREG_FIFODATA,PICC_B.PUPI[3]);
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_FIFODATA,0x08);
	NfcSetReg(JREG_FIFODATA,0x01);
	NfcSetReg(JREG_FIFODATA,0x01);
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{		
		NfcGetReg(JREG_FIFODATA,PICC_B.ATTRIB);
			return FM17622_SUCCESS;
	}
	return FM17622_COMM_ERR;	
}
//*************************************
//函数  名：ReaderB_GetUID
//入口参数：
//出口参数：FM17622_SUCCESS, FM17622_COMM_ERR
//*************************************
unsigned char ReaderB_GetUID(void)
{
	uint8_t reg_data;
	NfcSetReg(JREG_TXMODE,0x83);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x83);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_FIFODATA,0x36);
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_FIFODATA,0x08);
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 10)
	{		
		//for(i=0;i<8;i++)
		Read_FIFO(&PICC_B.UID[0],sizeof(PICC_B.UID));
			return FM17622_SUCCESS;
	}
	return FM17622_COMM_ERR;	
}

uint8_t ReaderB_CardInfo(picc_b_t* info) {
	memcpy((uint8_t*)(info), (uint8_t*)(&PICC_B), sizeof(picc_b_t));  
	return true;
}

#endif

