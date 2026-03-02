#ifndef _MIFARE_H_
#define _MIFARE_H_

#include "config.h"



/*****************Macro****************/
#define MIFARE_CMD_READ                     0x30
#define MIFARE_CMD_HALT                     0x50
#define MIFARE_CMD_AUTH_KEY_A               0x60
#define MIFARE_CMD_AUTH_KEY_B               0x61
#define MIFARE_CMD_COMP_WRITE               0xA0
#define MIFARE_CMD_WRITE                    0xA2
#define MIFARE_CMD_TRANSFER                 0xB0
#define MIFARE_CMD_DECREMENT                0xC0
#define MIFARE_CMD_INCREMENT                0xC1
#define MIFARE_CMD_RESTORE                  0xC2

#define MIFARE_ACK                          0x0A
#define MIFARE_NACK                         0x00

/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/
extern void Mifare_Clear_Crypto(void);
extern unsigned char Mifare_Transfer(unsigned char block_num);
extern unsigned char Mifare_Restore(unsigned char block_num);
extern unsigned char Mifare_Blockset(unsigned char block_num,unsigned char *data_buff);
extern unsigned char Mifare_Blockinc(unsigned char block_num,unsigned char *data_buff);
extern unsigned char Mifare_Blockdec(unsigned char block_num,unsigned char *data_buff);
extern unsigned char Mifare_Blockwrite(unsigned char block_num, const unsigned char *buff);
extern unsigned char Mifare_Blockread(unsigned char block_num,unsigned char *data_buff);
extern unsigned char Mifare_Auth(unsigned char mode, unsigned char sector, const unsigned char *mifare_key, const unsigned char *card_uid);
extern unsigned char Mifare_Blockwrite_WaitAck(void);
extern unsigned char Mifare_CardModifySecotrKey(unsigned char secotr_id, const unsigned char* mifare_key);
extern unsigned char Mifare_CardTypeCUID(void);
extern unsigned char Mifare_CardReadBlock(unsigned char sector_id, \
								unsigned char block_bit, \
								const unsigned char *mifare_key, \
								const unsigned char *card_uid, \
								unsigned char* block_buffer);

/**************************************/

#endif
