#ifndef HAL_FMC__HH
#define HAL_FMC__HH

#include "config.h"
#include "fmc.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
#pragma pack(1)


#pragma pack()


/***************Variable***************/


/***************Function***************/
void hal_fmc_write(uint32_t addr, uint8_t* pData, uint16_t len);
void hal_fmc_read(uint32_t addr, uint8_t* pData, uint16_t len);
void hal_fmc_erase(uint32_t addr);

/**************************************/

#endif 
