/**
 * @file ota_helper.c
 * @brief OTA升级辅助功能实现
 * @details 包含APP完整性校验、FLASH擦写、OTA状态管理、系统复位等核心功能
 */
#include "ota_helper.h"
#include "fmc.h"
#include "chip_config.h"
#include "ob_log.h"

//=============================================================
// 全局变量定义
//=============================================================
/** OTA助手句柄：管理OTA写入地址、运行状态等 */
static ota_helper_handle_t ota_helper_handle;
/** OTA参数缓存：存储从FLASH读取的OTA配置（CRC、长度、状态） */
static ota_fmc_area_t ota_fmc_area;

//=============================================================
// 宏定义（OTA核心配置）
//=============================================================
/******** 前期固定参数（后期可通过脚本覆盖） **********/
#define OTA_CRC32_DEFAULT     (0x4432)    /* 默认APP CRC值 */
#define OTA_LENGTH_DEFAULT    (53932)     /* 默认APP长度 */
#define OTA_APP_VALID_DEFAULT (1)         /* 默认APP有效状态 */

#define FLASH_APP_BEGIN_ADDR  (0x00000000)/* APP存储起始地址 */
#define MIN_APP_SIZE          (256)       /* APP最小长度（防止空固件） */
#define MAX_APP_SIZE          (0x0000EA00)/* APP最大长度限制 */
#define READ_BUFFER_SIZE      (512)       /* CRC计算缓存大小（4的倍数） */
#define FLASH_PAGE_SIZE       (0x00000200)/* FLASH单页大小（512字节） */
#define OTA_SECTOR_START_ADDR (0x0000EE00)/* OTA参数存储起始地址 */
/********************************************************/

/**
 * @brief OTA参数默认配置（固化到FLASH指定地址）
 * @note 段属性：将该结构体存储到0x0000EE00地址
 */
const ota_fmc_area_t ota_core_param __attribute__((section(".ARM.__at_0x0000EE00"), used)) = {
    .code_crc  = OTA_CRC32_DEFAULT,
    .code_size = OTA_LENGTH_DEFAULT,
    .state     = OTA_APP_VALID_DEFAULT,
};

//=============================================================
// 私有工具函数（内部调用，对外隐藏）
//=============================================================

/**
 * @brief 软件复位函数
 * @details 向系统复位寄存器写入复位指令，触发芯片重启
 * @note 复位后程序从启动地址重新执行，无返回值
 */
void soft_reset(void)
{
    uint32_t *pSFR = (uint32_t *)0xE000ED0C; /* 系统复位寄存器地址 */
    *pSFR = 0x05FA0004;                      /* 复位指令（标准ARM Cortex-M复位值） */
}

/**
 * @brief 读取OTA参数到全局缓存
 * @details 从FLASH指定地址读取OTA参数（CRC、长度、状态）到ota_fmc_area
 * @return 0-读取成功，1-读取失败（当前默认返回0，可扩展FMC返回值校验）
 */
static uint8_t ota_helper_read_param(void)
{
    /* 计算参数结构体的字长度（FMC按字读取，1字=4字节） */
    uint32_t param_word_len = sizeof(ota_fmc_area_t) / 4;
    /* 从FLASH读取参数到全局变量 */
    FMC_Read_Boot(OTA_SECTOR_START_ADDR, &ota_fmc_area, param_word_len);
    return 0;
}

/**
 * @brief 批量计算APP区域的CRC16值
 * @details 分块读取APP区域FLASH数据，累计计算CRC16（CCITT标准）
 * @param app_start_addr APP起始地址（固定为FLASH_APP_BEGIN_ADDR）
 * @param app_total_len  APP总长度（从OTA参数中获取）
 * @return 计算完成的CRC16值（初始值0xFFFF）
 */
static uint16_t ota_helper_calc_app_crc(uint32_t app_start_addr, uint32_t app_total_len)
{
    uint16_t calc_crc = 0xFFFF;    /* CRC16初始值（CCITT标准） */
    uint8_t read_buffer[READ_BUFFER_SIZE] = {0}; /* 数据缓存 */
    uint32_t read_word = 0;        /* 按字读取的临时变量 */
    uint32_t app_addr = app_start_addr; /* 当前读取地址 */
    uint32_t remain_bytes = app_total_len; /* 剩余未计算的字节数 */
    uint32_t buf_idx = 0;          /* 缓存写入索引 */

    while (remain_bytes > 0)
    {
        // 1. 读取4字节（按字读取，提升效率）
        if (remain_bytes >= 4)
        {
            FMC_Read_Boot(app_addr, &read_word, 1);
            /* 小端拆分32位数据为4个字节（与Python端字节序对齐） */
            read_buffer[buf_idx++] = (read_word >> 0)  & 0xFF;
            read_buffer[buf_idx++] = (read_word >> 8)  & 0xFF;
            read_buffer[buf_idx++] = (read_word >> 16) & 0xFF;
            read_buffer[buf_idx++] = (read_word >> 24) & 0xFF;

            app_addr += 4;          /* 地址偏移4字节 */
            remain_bytes -= 4;      /* 剩余字节数减4 */
        }
        // 2. 处理剩余不足4字节的情况
        else
        {
            uint8_t last_buf[4] = {0}; /* 不足4字节的临时缓存 */
            FMC_Read_Boot(app_addr, &read_word, 1);
            /* 仅提取剩余的有效字节 */
            for (uint32_t i = 0; i < remain_bytes; i++)
            {
                last_buf[i] = (read_word >> (i * 8)) & 0xFF;
            }
            /* 计算剩余字节的CRC */
            calc_crc = crc16_ccitt_accumulate(last_buf, (uint16_t)remain_bytes, calc_crc);
            remain_bytes = 0;       /* 结束循环 */
            continue;
        }

        // 3. 缓存满时计算CRC，避免内存溢出
        if (buf_idx >= READ_BUFFER_SIZE)
        {
            calc_crc = crc16_ccitt_accumulate(read_buffer, READ_BUFFER_SIZE, calc_crc);
            buf_idx = 0;            /* 重置缓存索引 */
        }
    }

    // 4. 处理缓存中未计算的最后一批数据（不足READ_BUFFER_SIZE的部分）
    if (buf_idx > 0)
    {
        calc_crc = crc16_ccitt_accumulate(read_buffer, (uint16_t)buf_idx, calc_crc);
    }

    return calc_crc;
}

//=============================================================
// APP完整性校验（核心业务功能）
//=============================================================

/**
 * @brief 检查APP固件是否完整（CRC校验）
 * @details 校验APP长度合法性，并计算CRC与OTA参数中的值对比
 * @return 1-APP完整（CRC匹配），0-APP不完整/校验失败
 * @note 需先调用ota_helper_init读取OTA参数到全局缓存
 */
uint8_t ota_helper_check_app_complete(void)
{
    uint16_t calc_crc = 0; /* 计算得到的APP CRC值 */

    // 1. 校验APP长度合法性（防止非法长度）
    if (ota_fmc_area.code_size < MIN_APP_SIZE || ota_fmc_area.code_size > MAX_APP_SIZE)
    {
        return 0;
    }

    // 2. 批量计算APP的CRC16值
    calc_crc = ota_helper_calc_app_crc(FLASH_APP_BEGIN_ADDR, ota_fmc_area.code_size);

    OB_LOGD("calc_crc: ");
    OB_LOGD_DUMP(&calc_crc, 2);
    OB_LOGD("ota_fmc_area.code_crc: ");
    OB_LOGD_DUMP(&ota_fmc_area.code_crc, 2);

    // 3. 对比CRC值（低16位匹配则认为完整）
    return (calc_crc == (uint16_t)ota_fmc_area.code_crc) ? 1 : 0;
}

//=============================================================
// FLASH擦写与OTA写入（OTA升级核心操作）
//=============================================================

/**
 * @brief OTA升级准备：擦除APP区域FLASH
 * @details 逐页擦除FLASH_APP_BEGIN_ADDR到MAX_APP_SIZE的所有页
 * @return 1-擦除成功，0-擦除失败（某一页擦除出错）
 */
uint8_t ota_helper_prepare(void)
{
    /* 初始化OTA句柄，重置写入地址 */
    memset((uint8_t *)(&ota_helper_handle), 0, sizeof(ota_helper_handle_t));
    ota_helper_handle.addr = FLASH_APP_BEGIN_ADDR;

    uint32_t current_erase_addr = FLASH_APP_BEGIN_ADDR; /* 当前擦除地址 */
    while (current_erase_addr <= MAX_APP_SIZE)
    {
        /* 擦除单页FLASH */
        if (FMC_PageErase(current_erase_addr) != 0)
        {
            return 0;
        }
        current_erase_addr += FLASH_PAGE_SIZE; /* 地址偏移一页 */
    }

    return 1;
}

/**
 * @brief 写入OTA数据到FLASH
 * @details 校验写入地址合法性后，将数据写入指定FLASH地址
 * @param addr    目标写入地址（需与当前句柄地址一致）
 * @param buffer  待写入的数据缓存
 * @param lenth   写入数据长度（字节）
 * @return 1-写入成功，0-写入失败（地址不匹配）
 */
uint8_t ota_helper_write(uint32_t addr, uint8_t *buffer, uint32_t lenth)
{
    /* 校验写入地址：必须与当前待写入地址一致（防止地址错乱） */
    if (ota_helper_handle.addr != addr)
    {
        return 0;
    }
    /* 写入数据到FLASH（按字写入） */
    FMC_Write(addr, (uint32_t *)buffer, lenth);
    ota_helper_handle.addr += lenth; /* 更新下一次写入地址 */

    return 1;
}

//=============================================================
// OTA状态控制（启动分区、运行状态）
//=============================================================

/**
 * @brief 设置启动分区并触发软件复位
 * @details 修改启动分区配置后，立即复位芯片使配置生效
 * @param target 目标启动地址（FLASH分区地址）
 * @return 1-执行成功（实际复位无返回）
 */
uint8_t ota_helper_set_boot(uint32_t target)
{
    FMC_SetBootTo(target); /* 设置启动分区 */
    soft_reset();          /* 软件复位 */
    return 1;              /* 理论上不会执行到此处 */
}

/**
 * @brief 检查OTA升级是否正在运行
 * @details 读取OTA句柄中的运行状态标识
 * @return 1-OTA运行中，0-OTA未运行
 */
uint32_t ota_helper_is_ota_running(void)
{
    return (ota_helper_handle.status.process);
}

/**
 * @brief 设置OTA升级进程状态
 * @details 标记OTA是否正在运行（预留接口，暂无具体实现）
 * @param mask 状态掩码：1-运行中，0-停止
 */
void ota_helper_set_ota_process(uint8_t mask)
{
    // *ota_process = (mask ? 1 : 0); /* 预留逻辑，暂未实现 */
}

//=============================================================
// OTA状态（state）操作接口（新增核心）
//=============================================================

/**
 * @brief 读取当前OTA状态
 * @details 获取全局缓存中ota_fmc_area.state的值
 * @return 当前OTA状态值（如OTA_APP_VALID_DEFAULT/OTA_STATE_READY）
 */
uint8_t ota_helper_get_state(void)
{
    return ota_fmc_area.state;
}

/**
 * @brief 修改OTA状态并同步到FLASH
 * @details 更新全局缓存中的state值，并写入FLASH指定地址
 * @param state 新的OTA状态值
 * @return 0-修改成功，1-修改失败（当前默认返回0）
 */
uint8_t ota_helper_set_state(uint8_t state)
{
    ota_fmc_area.state = state; /* 更新内存中的状态值 */

    FMC_PageErase(OTA_SECTOR_START_ADDR);
    FMC_Write(OTA_SECTOR_START_ADDR, (uint32_t *)&ota_fmc_area, sizeof(ota_fmc_area_t));
    return 0;
}

//=============================================================
// 初始化模块
//=============================================================

/**
 * @brief OTA助手初始化
 * @details 初始化时读取FLASH中的OTA参数到全局缓存
 * @note 需在系统启动后、OTA操作前调用
 */
void ota_helper_init(void)
{
    ota_helper_read_param(); /* 读取OTA参数到全局变量 */
}
