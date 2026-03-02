#ifndef PARAMETER_MANAGE__HH
#define PARAMETER_MANAGE__HH

#include "config.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/



/***************Variable***************/


/***************Function***************/
void parameter_manage_init(void);
uint8_t write_parameter_info(const uint8_t* pData, uint16_t size);
uint8_t read_parameter_info(void* pData, uint16_t size);

/**************************************/

#endif 
