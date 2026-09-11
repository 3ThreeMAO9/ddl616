#include "protocol_func_card.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "protocol func card"

/***************Variable***************/


// ------------------------------------------

uint8_t read_func_card_pkt_cmd(const func_card_pkt_t* packet) {
    if (FUNC_CARD_HEADER != packet->header) {
        return FUNC_CARD_CMD_NONE;
    }

    if (packet->cmd >= FUNC_CARD_CMD_CNT) {
        return FUNC_CARD_CMD_NONE;
    }

    if (packet->lenth > FUNC_CARD_BUFFER_SIZE) {
        return FUNC_CARD_CMD_NONE;
    }

    uint16_t sum = check_sum((uint8_t*)(&(packet->cmd)), (packet->lenth + 3));

    if (sum != packet->sum) {
        return FUNC_CARD_CMD_NONE;
    }

    if (packet->cmd >= FUNC_CARD_CMD_CNT) {
        return FUNC_CARD_CMD_NONE;
    }

    return packet->cmd;
}
