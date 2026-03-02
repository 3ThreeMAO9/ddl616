#ifndef EVENT_QUEUE__HH
#define EVENT_QUEUE__HH

#include "config.h"
#include "fsm_state.h"


/*****************Macro****************/
#define EVENT_QUEUE_SIZE                    (5)

/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    uint8_t front_id;
    uint8_t rear_id;
    uint8_t cnt;

    QEvent queue[EVENT_QUEUE_SIZE];

}event_queue_t;

typedef struct{
    void (*init)();
    void (*push)(QEvent*);
    uint8_t (*pop)(QEvent*);

}event_queue_drive_t;

/***************Variable***************/


/***************Function***************/
void eventQueueInit(void);
void eventQueuePush(QEvent* event);
uint8_t eventQueuePop(QEvent* event);

/**************************************/

#endif 
