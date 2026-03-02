#ifndef _READERAPI_H_
#define _READERAPI_H_

#include "config.h"

/*****************Macro****************/
//发射参数设置
#define MODWIDTH_106 0x26  //106Kbps为0x26
#define MODWIDTH_212 0x13  //212kbps为0x13
#define MODWIDTH_424 0x09  //424kbps为0x09
#define MODWIDTH_848 0x04  //848kbps为0x04
//接收参数配置
//TYPE A
#define RXGAIN_A    4   //设置范围0~7
#define GSNON_A     15  //设置范围0~15
#define GSP_A       31  //设置范围0~63
#define COLLLEVEL_A 4   //设置范围0~7
#define MINLEVEL_A  8   //设置范围0~15
//TYPE B
#define RXGAIN_B   6   //设置范围0~7
#define GSNON_B    15  //设置范围0~15
#define MODGSNON_B 6   //设置范围0~15
#define GSP_B      31  //设置范围0~63
#define MODGSP_B   10  //设置范围0~63
#define MINLEVEL_B 4   //设置范围0~15

#define RXWAIT 		4		//设置范围0~63
#define UARTSEL 	2		//默认设置为2  设置范围0~3 0:固定低电平 1:TIN包络信号 2:内部接收信号 3:TIN调制信号

#define FM17622_SUCCESS				0x00
#define FM17622_READING      		0x01
#define FM17622_RATS_ERR 	        0xD1
#define FM17622_PPS_ERR 	        0xD2
#define FM17622_PCB_ERR 	        0xD3
#define FM17622_AUTH_ERR	        0xE1
#define FM17622_RESET_ERR			0xF1
#define FM17622_PARAM_ERR 		    0xF2	//输入参数错误
#define FM17622_TIMER_ERR			0xF3	//接收超时
#define FM17622_COMM_ERR			0xF4	//通信错误
#define FM17622_COLL_ERR			0xF5	//冲突错误
#define FM17622_FIFO_ERR			0xF6	//FIFO错误
#define FM17622_CRC_ERR				0xF7
#define FM17622_PARITY_ERR		    0xF8
#define FM17622_PROTOCOL_ERR	    0xF9

#define CARD_TYPE_A_ENABLE              Enabled
#define CARD_TYPE_B_ENABLE              Disabled

/*****************Enum*****************/


/****************Struct****************/
#if (Enabled == CARD_TYPE_A_ENABLE)
typedef struct {
    unsigned char ATQA[2];
    unsigned char UID[4];
    unsigned char BCC;
    unsigned char SAK;
    unsigned char VENDOR_ID[8];         //厂商ID

}picc_a_t;

typedef struct {
    unsigned char sequence[4];          // 激活(7bit) + 确认(8bit) + 预留(2bytes)
    char description[32];
    
}back_door_sequence_t;

#endif
#if (Enabled == CARD_TYPE_B_ENABLE)

typedef struct {
    unsigned char ATQB[12];
    unsigned char PUPI[4];
    unsigned char APPLICATION_DATA[4];
    unsigned char PROTOCOL_INF[3];
    unsigned char ATTRIB[10];
    unsigned char UID[8];

}picc_b_t;

#endif

/***************Variable***************/
#if (Enabled == CARD_TYPE_A_ENABLE)
extern picc_a_t PICC_A; 
#endif
#if (Enabled == CARD_TYPE_B_ENABLE)
extern picc_b_t PICC_B; 
#endif

/***************Function***************/
#if (Enabled == CARD_TYPE_A_ENABLE)
extern void          FM17622_Initial_ReaderA(void);
extern unsigned char ReaderA_Halt(void);
extern unsigned char ReaderA_Request(void);
extern unsigned char ReaderA_Wakeup(void);
extern unsigned char ReaderA_AntiColl(void);
extern unsigned char ReaderA_Select(void);
extern unsigned char ReaderA_CardActivate(void);
extern unsigned char ReaderA_OpenBackDoor(void);
#endif

#if (Enabled == CARD_TYPE_B_ENABLE)
extern void          FM17622_Initial_ReaderB(void);
extern unsigned char ReaderB_Request(void);
extern unsigned char ReaderB_Wakeup(void);
extern unsigned char ReaderB_Attrib(void);
extern unsigned char ReaderB_GetUID(void);
extern unsigned char FM17622_Polling(unsigned char *polling_card);
#endif

/**************************************/

#endif

