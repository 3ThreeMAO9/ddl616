#include "flash_data.h"
#include "flash_drive.h"
#include "bsp_rom_config.h"
#include "parameter.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "flash_data"

/***************Variable***************/

static void flash_write_data_pages(uint32_t pageAddr, uint32_t backupAddr, uint8_t* pData, uint16_t size)
{
    const uint32_t flag = true;

    //first write backup page
    user_flash_erase(backupAddr, ((size + FLASH_ERASE_SIZE - 1) / FLASH_ERASE_SIZE));

    user_flash_write(backupAddr + sizeof(uint32_t), pData, size);
    user_flash_write(backupAddr, (uint8_t *)(&flag), sizeof(uint32_t));

    //first write backup page
    user_flash_erase(pageAddr, ((size + FLASH_ERASE_SIZE - 1) / FLASH_ERASE_SIZE));

    user_flash_write(pageAddr + sizeof(uint32_t), pData, size);
    user_flash_write(pageAddr, (uint8_t *)(&flag), sizeof(uint32_t));
}

static void flash_write_page_flag(uint32_t pageAddr, uint16_t blockSize)
{
    uint8_t dataBlock[DATA_BLOCK_SIZE];

    //write page flag
    memset(dataBlock, 0xFF, blockSize);
    dataBlock[0] = FLASH_DATA_VALID;
    user_flash_write(pageAddr, dataBlock, blockSize);
}

static void flash_data_block_renew(uint32_t pageAddr, uint32_t backupAddr, uint8_t pageCnt,
                                 uint16_t index, uint8_t* pData, uint16_t blockSize)
{
    // 安全检查：块大小有效且是其整数倍（确保对齐）
    if (blockSize == 0 || (BATCH_SIZE % blockSize) != 0) {
        return;
    }

    uint16_t i;
    uint16_t blockCnt;
    uint8_t buf[BATCH_SIZE];      // 缓冲区
    uint16_t currentBlocks = 0;      // 连续非目标块计数
    const uint16_t blocksPer = BATCH_SIZE / blockSize;  // 包含的块数

    // 计算总块数
    blockCnt = (FLASH_ERASE_SIZE / blockSize) * pageCnt;
    if (index >= blockCnt) {
        return;
    }

    // 1. 擦除目标区域
    user_flash_erase(pageAddr, (uint32_t)pageCnt * FLASH_ERASE_SIZE);
    // 初始化缓冲区（Flash擦除后默认值0xFF）
    memset(buf, 0xFF, BATCH_SIZE);

    // 2. 遍历所有块，按对齐批量处理
    for (i = 1; i < blockCnt; i++) {
        if (i != index) {
            // 累计连续非目标块
            currentBlocks++;

            // 当累计块数达到对应的块数时，执行批量读写
            if (currentBlocks == blocksPer) {
                const uint32_t startIndex = i - currentBlocks + 1;  // 批量起始块索引
                const uint32_t readAddr = backupAddr + (uint32_t)startIndex * blockSize;
                const uint32_t writeAddr = pageAddr + (uint32_t)startIndex * blockSize;
                OB_LOGD(TAG,"user_flash_read readAddr       [%08X] BATCH_SIZE[%08X]",readAddr,BATCH_SIZE);
                // 一次性读取数据（连续块）
                user_flash_read(readAddr, buf, BATCH_SIZE);
                OB_LOGD(TAG,"user_flash_write writeAddr     [%08X] BATCH_SIZE[%08X]",writeAddr,BATCH_SIZE);
                // 一次性写入数据
                user_flash_write(writeAddr, buf, BATCH_SIZE);

                // 重置计数器和缓冲区
                currentBlocks = 0;
                memset(buf, 0xFF, BATCH_SIZE);
            }
        } else {
            // 2.1 先处理已累计但未达的连续块（不足的部分）
            if (currentBlocks > 0) {
                const uint32_t startIndex = i - currentBlocks;
                const uint32_t totalSize = (uint32_t)currentBlocks * blockSize;
                const uint32_t readAddr = backupAddr + (uint32_t)startIndex * blockSize;
                const uint32_t writeAddr = pageAddr + (uint32_t)startIndex * blockSize;

                // 读取并写入不足的剩余部分
                OB_LOGD(TAG,"user_flash_read readAddr       [%08X]  totalSize[%08X]",readAddr,totalSize);
                user_flash_read(readAddr, buf, totalSize);
                OB_LOGD(TAG,"user_flash_write writeAddr     [%08X]  totalSize[%08X]",writeAddr,totalSize);
                user_flash_write(writeAddr, buf, totalSize);

                currentBlocks = 0;
                memset(buf, 0xFF, BATCH_SIZE);
            }

            // 2.2 处理目标块
            const uint32_t targetAddr = pageAddr + (uint32_t)i * blockSize;
            memcpy(buf, pData, blockSize);
            OB_LOGD(TAG,"user_flash_write targetAddr    [%08X]  blockSize[%08X]   +++",targetAddr,blockSize);
            user_flash_write(targetAddr, buf, blockSize);
        }
    }

    // 3. 处理循环结束后未达的剩余连续块
    if (currentBlocks > 0) {
        const uint32_t startIndex = i - currentBlocks;
        const uint32_t totalSize = (uint32_t)currentBlocks * blockSize;
        const uint32_t readAddr = backupAddr + (uint32_t)startIndex * blockSize;
        const uint32_t writeAddr = pageAddr + (uint32_t)startIndex * blockSize;
        OB_LOGD(TAG,"user_flash_read readAddr       [%08X]  totalSize[%08X]",readAddr,totalSize);
        user_flash_read(readAddr, buf, totalSize);
        OB_LOGD(TAG,"user_flash_write writeAddr     [%08X]  totalSize[%08X]",writeAddr,totalSize);
        user_flash_write(writeAddr, buf, totalSize);
    }

    // 4. 写入页标记
    flash_write_page_flag(pageAddr, blockSize);
}

static uint8_t isLoseDataPage(uint32_t pageAddr)
{
    uint8_t flag;
    user_flash_read(pageAddr, &flag, 1);

    if (0xFF == flag) {
        user_flash_read(pageAddr, &flag, 1);
        if (FLASH_DATA_VALID == flag) {
            return true;
        }
    }
    return false;
}

static void user_flash_renew(void)
{
    const uint16_t index = 0;
	uint8_t pData[DATA_BLOCK_SIZE]; 
    memset(pData, 0xFF, DATA_BLOCK_SIZE);

    if (isLoseDataPage(USER_PAGE_START_ADDR)) {
        flash_data_block_renew(USER_PAGE_START_ADDR, USER_PAGE_BACKUP_ADDR, USER_PAGE_CNT,
                               index, pData, USER_BLOCK_SIZE);
    }

    if (isLoseDataPage(PARAMETER_PAGE_START_ADDR)) {
        flash_data_block_renew(PARAMETER_PAGE_START_ADDR, PARAMETER_PAGE_BACKUP_ADDR, PARAMETER_PAGE_CNT,
                               index, pData, DATA_BLOCK_SIZE);
    }
}

static void flash_data_block_modify(uint32_t pageAddr, uint32_t backupAddr, uint8_t pageCnt,
                                    uint16_t index, uint8_t* pData, uint16_t blockSize)
{
    OB_LOGW(TAG,"flash_data_block_modify start");
    //first write backup page
    flash_data_block_renew(backupAddr, pageAddr, pageCnt, index, pData, blockSize);
    //first write data page
    flash_data_block_renew(pageAddr, backupAddr, pageCnt, index, pData, blockSize);
    OB_LOGW(TAG,"flash_data_block_modify end");
}

// -------- 密码用户 (块1~20) --------
uint8_t read_code_user(uint8_t index, user_info_t* pData)
{
    if (index >= PERMANENT_USER_CODE_CNT || pData == NULL) {
        return 0;
    }
    uint32_t addr = USER_ADDR_CODE(index);
    user_flash_read(addr, (uint8_t*)pData, sizeof(user_info_t));

    uint16_t sum = check_sum((uint8_t*)(&pData->flag), sizeof(user_info_t) - sizeof(pData->sum));
    if (sum != pData->sum && pData->flag) {
        // OB_LOGE(TAG, "Code user[%d] checksum error", index);
        pData->flag = 0;
        return 0;
    }
    return pData->flag;
}

void save_code_user(uint8_t index, user_info_t* pData)
{
    if (index >= PERMANENT_USER_CODE_CNT || pData == NULL) {
        return;
    }
    uint8_t temp[USER_BLOCK_SIZE];
    memset(temp, 0xFF, USER_BLOCK_SIZE);
    memcpy(temp, pData, sizeof(user_info_t));
    
    uint32_t addr = USER_ADDR_CODE(index);
    uint32_t backupAddr = USER_ADDR_CODE_BACKUP(index);

    uint32_t pageStartAddr = (addr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t backupPageStartAddr = (backupAddr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t blockInPage = (addr - pageStartAddr) / USER_BLOCK_SIZE;
    
    flash_data_block_modify(pageStartAddr, backupPageStartAddr, 1, blockInPage, temp, USER_BLOCK_SIZE);
}

// -------- 指纹用户 (块21~70) --------
uint8_t read_finger_user(uint8_t index, user_info_t* pData)
{
    if (index >= USER_FINGERPRINTS_CNT || pData == NULL) {
        return 0;
    }
    uint32_t addr = USER_ADDR_FINGER(index);
    user_flash_read(addr, (uint8_t*)pData, sizeof(user_info_t));
    
    uint16_t sum = check_sum((uint8_t*)(&pData->flag), sizeof(user_info_t) - sizeof(pData->sum));
    if (sum != pData->sum && pData->flag) {
        // OB_LOGE(TAG, "Finger user[%d] checksum error", index);
        pData->flag = 0;
        return 0;
    }
    return pData->flag;
}

void save_finger_user(uint8_t index, user_info_t* pData)
{
    if (index >= USER_FINGERPRINTS_CNT || pData == NULL) {
        return;
    }
    uint8_t temp[USER_BLOCK_SIZE];
    memset(temp, 0xFF, USER_BLOCK_SIZE);
    memcpy(temp, pData, sizeof(user_info_t));
    
    uint32_t addr = USER_ADDR_FINGER(index);
    uint32_t backupAddr = USER_ADDR_FINGER_BACKUP(index);
    
    uint32_t pageStartAddr = (addr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t backupPageStartAddr = (backupAddr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t blockInPage = (addr - pageStartAddr) / USER_BLOCK_SIZE;
    
    flash_data_block_modify(pageStartAddr, backupPageStartAddr, 1, 
                            blockInPage, temp, USER_BLOCK_SIZE);
}
// -------- 卡片用户 (块71~170) --------
uint8_t read_card_user(uint8_t index, user_info_t* pData)
{
    if (index >= USER_CARD_CNT || pData == NULL) {
        return 0;
    }
    uint32_t addr = USER_ADDR_CARD(index);
    user_flash_read(addr, (uint8_t*)pData, sizeof(user_info_t));
    
    uint16_t sum = check_sum((uint8_t*)(&pData->flag), sizeof(user_info_t) - sizeof(pData->sum));
    if (sum != pData->sum && pData->flag) {
        // OB_LOGE(TAG, "Card user[%d] checksum error", index);
        pData->flag = 0;
        return 0;
    }
    return pData->flag;
}

void save_card_user(uint8_t index, user_info_t* pData)
{
    if (index >= USER_CARD_CNT || pData == NULL) {
        return;
    }
    uint8_t temp[USER_BLOCK_SIZE];
    memset(temp, 0xFF, USER_BLOCK_SIZE);
    memcpy(temp, pData, sizeof(user_info_t));
    
    uint32_t addr = USER_ADDR_CARD(index);
    uint32_t backupAddr = USER_ADDR_CARD_BACKUP(index);
    
    uint32_t pageStartAddr = (addr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t backupPageStartAddr = (backupAddr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t blockInPage = (addr - pageStartAddr) / USER_BLOCK_SIZE;
    
    flash_data_block_modify(pageStartAddr, backupPageStartAddr, 1, 
                            blockInPage, temp, USER_BLOCK_SIZE);
}
// -------- 人脸用户 (块171~220) --------
uint8_t read_face_user(uint8_t index, user_info_t* pData)
{
    if (index >= USER_FACE_CNT || pData == NULL) {
        return 0;
    }
    uint32_t addr = USER_ADDR_FACE(index);
    user_flash_read(addr, (uint8_t*)pData, sizeof(user_info_t));
    
    uint16_t sum = check_sum((uint8_t*)(&pData->flag), sizeof(user_info_t) - sizeof(pData->sum));
    if (sum != pData->sum && pData->flag) {
        // OB_LOGE(TAG, "Face user[%d] checksum error", index);
        pData->flag = 0;
        return 0;
    }
    return pData->flag;
}

void save_face_user(uint8_t index, user_info_t* pData)
{
    if (index >= USER_FACE_CNT || pData == NULL) {
        return;
    }
    uint8_t temp[USER_BLOCK_SIZE];
    memset(temp, 0xFF, USER_BLOCK_SIZE);
    memcpy(temp, pData, sizeof(user_info_t));
    
    uint32_t addr = USER_ADDR_FACE(index);
    uint32_t backupAddr = USER_ADDR_FACE_BACKUP(index);
    
    uint32_t pageStartAddr = (addr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t backupPageStartAddr = (backupAddr / FLASH_ERASE_SIZE) * FLASH_ERASE_SIZE;
    uint32_t blockInPage = (addr - pageStartAddr) / USER_BLOCK_SIZE;
    
    flash_data_block_modify(pageStartAddr, backupPageStartAddr, 1, 
                            blockInPage, temp, USER_BLOCK_SIZE);
}

uint8_t read_user_data_with_check(uint16_t user_sn, user_info_t* pData)
{
    if (user_sn == 0 || user_sn > USER_CNT || pData == NULL) {
        return 0;
    }
    
    uint32_t addr = USER_PAGE_START_ADDR + user_sn * USER_BLOCK_SIZE;
    user_flash_read(addr, (uint8_t*)pData, sizeof(user_info_t));
    
    if (!pData->flag) {
        return 0;
    }
    
    uint16_t sum = check_sum((uint8_t*)(&pData->flag), sizeof(user_info_t) - sizeof(pData->sum));
    if (sum != pData->sum) {
        // OB_LOGE(TAG, "User[%u] checksum error: %04X != %04X", user_sn, sum, pData->sum);
        pData->flag = 0;
        return 0;
    }
    return 1;
}

void flash_update_user_cnt(user_key_cnt_t* cnt)
{
    uint16_t i;
    user_info_t temp_user;
    
    if (cnt == NULL) {
        return;
    }
    
    memset(cnt, 0, sizeof(user_key_cnt_t));

    for (i = 0; i < PERMANENT_USER_CODE_CNT; i++) {
        if (read_code_user(i, &temp_user) && temp_user.flag) {
            cnt->permanentCode++;
        }
    }
    
    for (i = 0; i < USER_FINGERPRINTS_CNT; i++) {
        if (read_finger_user(i, &temp_user) && temp_user.flag) {
            cnt->permanentFingers++;
        }
    }
    
    for (i = 0; i < USER_CARD_CNT; i++) {
        if (read_card_user(i, &temp_user) && temp_user.flag) {
            cnt->permanentCard++;
        }
    }
    
    for (i = 0; i < USER_FACE_CNT; i++) {
        if (read_face_user(i, &temp_user) && temp_user.flag) {
            cnt->permanentFace++;
        }
    }
    
    cnt->permanentKey = cnt->permanentCode + cnt->permanentFingers + 
                        cnt->permanentCard + cnt->permanentFace;
}

static void flash_user_data_init(void)
{
#if (Enabled == PRINTF_FLASH)
    OB_LOGD(TAG, "flash_user_data_init: fixed address layout");
    OB_LOGD(TAG, "  Code:   %d users, start block %d", PERMANENT_USER_CODE_CNT, BLOCK_INDEX_CODE_START);
    OB_LOGD(TAG, "  Finger: %d users, start block %d", USER_FINGERPRINTS_CNT, BLOCK_INDEX_FINGER_START);
    OB_LOGD(TAG, "  Card:   %d users, start block %d", USER_CARD_CNT, BLOCK_INDEX_CARD_START);
    OB_LOGD(TAG, "  Face:   %d users, start block %d", USER_FACE_CNT, BLOCK_INDEX_FACE_START);
    OB_LOGD(TAG, "  Total:  %d users, %d blocks, %d pages", 
            USER_CNT, USER_TOTAL_BLOCKS, USER_PAGE_CNT);
#endif
#if (Enabled == PRINTF_USER)
   uint8_t dataBlock[DATA_BLOCK_SIZE];
    for (uint16_t i = 0; i < (USER_CNT); i++)
    {
        user_flash_read(USER_PAGE_START_ADDR + ((i + 1) * USER_BLOCK_SIZE), dataBlock, USER_BLOCK_SIZE);
        // OB_LOGW(TAG,"Adrr %08X",USER_PAGE_START_ADDR + ((i + 1) * USER_BLOCK_SIZE));
        // OB_LOGI_DUMP(dataBlock, USER_BLOCK_SIZE);
        updateUserTable(i, (user_info_t *)(dataBlock));
    }
#endif
    updateUserCnt();
}

static void flash_parameter_data_init(void)
{
    uint8_t* pt = readUserParameterAddr();
    user_flash_read(PARAMETER_PAGE_START_ADDR + sizeof(uint32_t), pt, sizeof(parameter_t));
    userParameterInit();
}

void flash_data_init(void)
{
#if (Enabled == PRINTF_FLASH)
    OB_LOGD(TAG, "************************************************************/");
    OB_LOGD(TAG, "flash_data_init");
    OB_LOGD(TAG, "user addr[0x%08X], backup[0x%08X], cnt[%u]", 
            USER_PAGE_START_ADDR, USER_PAGE_BACKUP_ADDR, USER_PAGE_CNT);
#endif

    user_flash_renew();      // renew data page

    //user data init
    flash_user_data_init();

    //parameter data init
    flash_parameter_data_init();
#if (Enabled == PRINTF_FLASH)
    user_info_num();
    OB_LOGD(TAG, "/************************************************************/");
#endif
}

void save_user_data(uint16_t user_sn, uint8_t* pData)
{
    if (user_sn == 0 || user_sn > USER_CNT || pData == NULL) {
        return;
    }
    uint8_t temp[USER_BLOCK_SIZE];
    memset(temp, 0xFF, USER_BLOCK_SIZE);
    memcpy(temp, pData, sizeof(user_info_t));

    uint32_t addr = USER_PAGE_START_ADDR + user_sn * USER_BLOCK_SIZE;
    uint32_t backupAddr = USER_PAGE_BACKUP_ADDR + (addr - USER_PAGE_START_ADDR);
    uint32_t blockInPage = ((addr - USER_PAGE_START_ADDR) % FLASH_ERASE_SIZE) / USER_BLOCK_SIZE;
    
    flash_data_block_modify(addr, backupAddr, 1, blockInPage, temp, USER_BLOCK_SIZE);
}

void save_parameter_data(uint8_t* pData, uint16_t size)
{
    flash_write_data_pages(PARAMETER_PAGE_START_ADDR, PARAMETER_PAGE_BACKUP_ADDR, pData, size);
}

void save_wakeupstate_data(uint8_t* pData, uint16_t size)
{
    flash_write_data_pages(WAKEUP_STATE_ADDR, WAKEUP_STATE_ADDR, pData, size);
}

void flash_page_system_init(void)
{
    // user page
    user_flash_erase(USER_PAGE_BACKUP_ADDR, (USER_PAGE_CNT * FLASH_ERASE_SIZE));
    user_flash_erase(USER_PAGE_START_ADDR, (USER_PAGE_CNT * FLASH_ERASE_SIZE));

    // parameter page
    user_flash_erase(PARAMETER_PAGE_BACKUP_ADDR, (PARAMETER_PAGE_CNT * FLASH_ERASE_SIZE));
    user_flash_erase(PARAMETER_PAGE_START_ADDR, (PARAMETER_PAGE_CNT * FLASH_ERASE_SIZE));

    //wakeup state
    user_flash_erase(WAKEUP_STATE_ADDR, (PARAMETER_PAGE_CNT * FLASH_ERASE_SIZE));
}

