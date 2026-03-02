#include "bsp_qp_fsm.h"
#include "qp_fsm.h"


/***************Variable***************/
const bsp_qp_fsm_drive_io_t bsp_qp_fsm = {
    .init = qpFsmInit,
    .loop = qpFsmLoop,

};

// ------------------------------------------

const bsp_qp_fsm_drive_io_t* bsp_qp_fsm_get_driver(void)
{
    return &bsp_qp_fsm;
}
