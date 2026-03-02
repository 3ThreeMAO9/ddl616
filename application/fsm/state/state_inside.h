#ifndef STATE_INSIDE__HH
#define STATE_INSIDE__HH

#include "config.h"

#include "event.h"
#include "qp_port.h"
#include "qp_fsm.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/
QState lock_fsm_sleep(LockFsm *me, QEvent const *e);
QState lock_fsm_idle(LockFsm *me, QEvent const *e);

/**************************************/


/*****************Macro****************/

#endif 
