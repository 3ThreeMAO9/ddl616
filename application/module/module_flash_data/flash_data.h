#ifndef FLASH_DATA__HH
#define FLASH_DATA__HH

#include "config.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
#pragma pack(1)


#pragma pack()




/***************Variable***************/


/***************Function***************/
void flash_data_init(void);
void flash_page_system_init(void);
void save_user_data(uint16_t user_sn, uint8_t* pData);
void save_parameter_data(uint8_t* pData, uint16_t size);
void save_wakeupstate_data(uint8_t* pData, uint16_t size);

/**************************************/

#endif 
