#include "MIFARE.h"
#include "READER_API.h"
#include "FM17622.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "MIFARE"

/*****************************************************************************************/
/*名称：Mifare_Clear_Crypto																															 */
/*功能：Mifare_Clear_Crypto清除认证标志																 									*/
/*输入：																																								 */
/*																																						 					*/
/*输出:																																									 */
/*																																											 */
/*																																											 */
/*****************************************************************************************/							 
void Mifare_Clear_Crypto(void)
{
	NfcModifyReg(JREG_STATUS2,BIT3,0);
}

/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0x60：key A认证，0x61：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		FM17622_SUCCESS    :认证成功																	 */
/*		FM17622_AUTH_ERR :认证失败																	 */
/*****************************************************************************************/
 unsigned char Mifare_Auth(unsigned char mode, unsigned char sector, const unsigned char *mifare_key, const unsigned char *card_uid)
{	
	unsigned char reg_data;

	OB_LOGD(TAG, "uid->");
	OB_LOGD_DUMP(card_uid, 4);
	OB_LOGD(TAG, "mifare key->");
	OB_LOGD_DUMP(mifare_key, 6);
	
	NfcSetReg(JREG_STATUS2,0);	//clear Crypto1On 只在首次认证时使用，多次认证时不要清除认证标志
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x80);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA, mode);
	NfcSetReg(JREG_FIFODATA, sector * 4);
	NfcSetReg(JREG_FIFODATA, mifare_key[0]);
	NfcSetReg(JREG_FIFODATA, mifare_key[1]);
	NfcSetReg(JREG_FIFODATA, mifare_key[2]);
	NfcSetReg(JREG_FIFODATA, mifare_key[3]);
	NfcSetReg(JREG_FIFODATA, mifare_key[4]);
	NfcSetReg(JREG_FIFODATA, mifare_key[5]);
	NfcSetReg(JREG_FIFODATA, PICC_A.UID[0]);
	NfcSetReg(JREG_FIFODATA, PICC_A.UID[1]);
	NfcSetReg(JREG_FIFODATA, PICC_A.UID[2]);
	NfcSetReg(JREG_FIFODATA, PICC_A.UID[3]);
	
	NfcSetReg(JREG_COMMAND,CMD_AUTHENT);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	uint8_t i = 100;
	do {
		delay_us(100);
		NfcGetReg(JREG_STATUS2, &reg_data);
		if(reg_data & 0x08) {
			OB_LOGD(TAG, "Success: Mifare_Auth");
			return FM17622_SUCCESS;
		}
	}while(i--);

	OB_LOGD(TAG, "Fail: Mifare_Auth");

	return FM17622_AUTH_ERR;	
}

/*****************************************************************************************/
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block_num，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		FM17622_SUCCESS    :成功																		 */
/*		FM17622_COMM_ERR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block_num,unsigned char *data_buff)
{	
	uint8_t reg_data;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x80);//Enable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,MIFARE_CMD_READ);
	NfcSetReg(JREG_FIFODATA,block_num);
	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	//Wait 10ms
	uint8_t i = 100;
	do {
		delay_us(100);
		NfcGetReg(JREG_FIFOLEVEL,&reg_data);
		if(reg_data == 16)
		{
			Read_FIFO(&data_buff[0], 16);
			OB_LOGD(TAG, "Success: Mifare_Blockread");	
			return FM17622_SUCCESS;
		}
	}while(i--);

	OB_LOGD(TAG, "Fail: Mifare_Blockread");

	return FM17622_COMM_ERR;	
}

/*****************************************************************************************/
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block_num，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		FM17622_SUCCESS    :成功																		 */
/*		FM17622_COMM_ERR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block_num, const unsigned char *buff)
{
	unsigned char reg_data;

	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x00);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel

	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA, MIFARE_CMD_COMP_WRITE);
	NfcSetReg(JREG_FIFODATA,block_num);
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING, 0x80);//Start Send

	//Wait 10ms
	uint8_t i = 100;
	do {

		delay_us(100);
		if (i) {
			i--;
		}
		else {
			OB_LOGD(TAG, "Fail: Mifare_Blockwrite 1");
			return FM17622_COMM_ERR;
		}
		NfcGetReg(JREG_FIFOLEVEL, &reg_data);
	}while(0 == reg_data);

	NfcGetReg(JREG_FIFODATA,&reg_data);
	OB_LOGD(TAG, "reg data 1[%02X]", reg_data);
	if(reg_data == 0x0A)
	{
		NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
		Write_FIFO((uint8_t*)buff, 16);
		NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
		NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
		//Wait 10ms
		i = 100;
		do {

			delay_us(100);
			NfcGetReg(JREG_FIFOLEVEL,&reg_data);
			if (1 == reg_data) {

				NfcGetReg(JREG_FIFODATA, &reg_data);
				
				OB_LOGD(TAG, "reg data 2[%02X]", reg_data);
				if(reg_data == 0x0A) {
					OB_LOGD(TAG, "Success: Mifare_Blockwrite");
					return FM17622_SUCCESS;
				}
			}

		}while(i--);
}

	OB_LOGD(TAG, "Fail: Mifare_Blockwrite 3");
	return FM17622_COMM_ERR;
}

/*****************************************************************************************/
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block_num，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		FM17622_SUCCESS    :成功																		 */
/*		FM17622_COMM_ERR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block_num,unsigned char *data_buff)
{		
	unsigned char reg_data,i;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x00);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,MIFARE_CMD_INCREMENT);
	NfcSetReg(JREG_FIFODATA,block_num);	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		NfcGetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM17622_COMM_ERR;
	}
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<4;i++)
		NfcSetReg(JREG_FIFODATA,data_buff[i]);
	NfcSetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transmit
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	
	return FM17622_SUCCESS;	
}
/*****************************************************************************************/
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block_num，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		FM17622_SUCCESS    :成功																		 */
/*		FM17622_COMM_ERR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block_num,unsigned char *data_buff)
{	
	unsigned char reg_data,i;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x00);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,MIFARE_CMD_DECREMENT);
	NfcSetReg(JREG_FIFODATA,block_num);	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		NfcGetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM17622_COMM_ERR;
	}
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<4;i++)
		NfcSetReg(JREG_FIFODATA,data_buff[i]);
	NfcSetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transmit
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	
	return FM17622_SUCCESS;	
}
/*****************************************************************************************/
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		FM17622_SUCCESS    :成功																		 */
/*		FM17622_COMM_ERR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block_num)
{		
	unsigned char reg_data;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x00);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,MIFARE_CMD_TRANSFER);
	NfcSetReg(JREG_FIFODATA,block_num);	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		NfcGetReg(JREG_FIFODATA,&reg_data);
		if(reg_data == 0x0A)
			return FM17622_SUCCESS;
	}
	return FM17622_COMM_ERR;	
}
/*****************************************************************************************/
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block_num，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		FM17622_SUCCESS    :成功																		 */
/*		FM17622_COMM_ERR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block_num)
{	
	unsigned char reg_data,i;
	NfcSetReg(JREG_TXMODE,0x80);//Enable TxCRC
	NfcSetReg(JREG_RXMODE,0x00);//Disable RxCRC
	NfcSetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	NfcSetReg(JREG_FIFODATA,MIFARE_CMD_RESTORE);
	NfcSetReg(JREG_FIFODATA,block_num);	
	NfcSetReg(JREG_COMMAND,CMD_TRANSCEIVE);//command = Transceive
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data == 1)
	{
		NfcGetReg(JREG_FIFODATA,&reg_data);
		if(reg_data != 0x0A)
			return FM17622_COMM_ERR;
	}
	NfcSetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);//Clear FIFO
	for(i=0;i<4;i++)
		NfcSetReg(JREG_FIFODATA,0x00);
	NfcSetReg(JREG_COMMAND,CMD_TRANSMIT);//command = Transmit
	NfcSetReg(JREG_BITFRAMING,0x80);//Start Send
	mDelay(10);//Wait 10ms
	NfcGetReg(JREG_FIFOLEVEL,&reg_data);
	if(reg_data != 0)
			return FM17622_COMM_ERR;
	return FM17622_SUCCESS;
	
}

unsigned char Mifare_CardTypeCUID(void) {
	const unsigned char mifare_key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	unsigned char buffer[16];

	if (FM17622_SUCCESS == Mifare_Auth(MIFARE_CMD_AUTH_KEY_B, 0, mifare_key, PICC_A.UID)) {
		if (FM17622_SUCCESS == Mifare_Blockread(0, buffer)) {
			OB_LOGD(TAG, "block 0: ");
			OB_LOGD_DUMP(buffer, 16);

			if (FM17622_SUCCESS == Mifare_Blockwrite(0, buffer)) {
				return FM17622_SUCCESS;
			}
		}
	}

	return FM17622_COMM_ERR;
}

unsigned char Mifare_CardModifySecotrKey(unsigned char secotr_id, const unsigned char* mifare_key) {
	const unsigned char mifare_key_default[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	unsigned char buffer[16];

	OB_LOGD(TAG, "secotr[%u]", secotr_id);

	if (FM17622_SUCCESS == Mifare_Auth(MIFARE_CMD_AUTH_KEY_B, secotr_id, mifare_key_default, PICC_A.UID)) {
		if (FM17622_SUCCESS == Mifare_Blockread((secotr_id * 4 + 3), buffer)) {
			memset(buffer, 0xFF, 16);
			// memcpy(buffer, mifare_key, 6);
			memcpy((buffer + 10), mifare_key, 6);
			OB_LOGD(TAG, "block 3: ");
			OB_LOGD_DUMP(buffer, 16);

			if (FM17622_SUCCESS == Mifare_Blockwrite((secotr_id * 4 + 3), buffer)) {
				return FM17622_SUCCESS;
			}
		}
	}

	return FM17622_COMM_ERR;
}

unsigned char Mifare_CardReadBlock(unsigned char sector_id, \
								unsigned char block_bit, \
								const unsigned char *mifare_key, \
								const unsigned char *card_uid, \
								unsigned char* block_buffer) {
	if (!block_bit) {
		return FM17622_COMM_ERR;
	}

	if (FM17622_SUCCESS != Mifare_Auth(MIFARE_CMD_AUTH_KEY_B, sector_id, mifare_key, card_uid)) {
		return FM17622_COMM_ERR;
	}

	for (unsigned char i = 0; i < 4; i++) {
		if (block_bit & (0x01 << i)) {
			if (FM17622_SUCCESS != Mifare_Blockread((sector_id * 4 + i), (block_buffer + (16 * i)))) {
				return FM17622_COMM_ERR;
			}
		}
	}

	return FM17622_SUCCESS;
}
