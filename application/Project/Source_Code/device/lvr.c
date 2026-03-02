#include "lvr.h"

/*********************Variable********************/

// ----------------------------------------------

void Init_ARM_LVR_LP(void)
{
    uint32_t *pt = (uint32_t*)0x500100C0;
    *pt = 0x8F;
    *pt &= 0xFFFFFFF9;
}
