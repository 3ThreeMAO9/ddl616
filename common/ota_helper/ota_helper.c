/**
 * @file ota_helper.c
 * @brief OTA升级辅助功能实现
 * @details 包含APP完整性校验、FLASH擦写、OTA状态管理、系统复位等核心功能
 */
#include "ota_helper.h"
// #include "ota_protocol.h"
#include "fmc.h"
#include "chip_config.h"
#include "ob_log.h"
#include "system_timer.h"
#include <string.h>  // 补充memset所需头文件

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
#define CHECKSUM_U16_MASK     ((uint16_t)~0x01)
#define CHECKSUM_U32_MASK     ((uint32_t)~0x03)

// #define FLASH_APP_BEGIN_ADDR  (0x00000000)/* APP存储起始地址 */
#define MIN_APP_SIZE          (256)       /* APP最小长度（防止空固件） */
#define MAX_APP_SIZE          (0x0000EA00)/* APP最大长度限制 */
#define READ_BUFFER_SIZE      (512)       /* 校验计算缓存大小（4的倍数） */
#define FLASH_PAGE_SIZE       (0x00000200)/* FLASH单页大小（512字节） */
// #define OTA_SECTOR_START_ADDR (0x0000EE00)/* OTA参数存储起始地址 */
/********************************************************/

//=============================================================
// 私有工具函数（内部调用，对外隐藏）
//=============================================================
uint16_t ota_file_calc_checksum_u16(uint16_t *ptr, uint32_t size)
{
    uint16_t sum;
    for (sum = 0; size & CHECKSUM_U16_MASK; ptr++)
    {
        sum += *ptr;
        size -= sizeof(*ptr);
    }

    if (size)
        sum += *ptr & 0xFF;

    return sum;
}

uint32_t ota_file_calc_checksum_u32(uint32_t *ptr, uint32_t size)
{
    uint32_t sum;
    for (sum = 0; size & CHECKSUM_U32_MASK; ptr++)
    {
        sum += *ptr;
        size -= sizeof(*ptr);
    }

    switch (size)
    {
    case 3:
        sum += *ptr & 0xFFFFFF;
        break;
    case 2:
        sum += *ptr & 0xFFFF;
        break;
    case 1:
        sum += *ptr & 0xFF;
        break;
    }

    return sum;
}

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
 * @brief 保存OTA参数结构体到FLASH指定地址
 * @param ota_param 待保存的OTA参数结构体指针
 * @return 0-保存成功，1-入参为空失败
 */
uint8_t ota_helper_save_fmc_area(const ota_fmc_area_t *ota_param)
{
    // 仅做最基础的空指针校验
    if (ota_param == NULL) 
        return 1;

    // 擦除扇区 + 写入结构体（核心逻辑）
    FMC_PageErase(OTA_SECTOR_START_ADDR);
    FMC_Write(OTA_SECTOR_START_ADDR, (uint8_t *)ota_param, sizeof(ota_fmc_area_t));
    
    return 0;
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
    FMC_Read_Boot(OTA_SECTOR_START_ADDR, (uint32_t *)&ota_fmc_area, param_word_len);
    return 0;
}

/**
 * @brief 校验OTA参数区域的16位累加和（sum1）
 * @details 计算指定OTA参数结构体中magic到state前的16位累加和，与checksum1对比
 * @param ota_param 待校验的OTA参数结构体指针
 * @return 1-校验通过，0-校验失败
 */
uint8_t ota_helper_check_param_checksum1(const ota_fmc_area_t *ota_param)
{
    uint16_t sum1 = 0;
    // 计算magic到state前的区域的16位累加和
    sum1 = ota_file_calc_checksum_u16((uint16_t *)(&ota_param->magic),
        sizeof(ota_fmc_area_t) - sizeof(ota_param->checksum1) - sizeof(ota_param->state));
    
    // 对比校验值
    if (sum1 != ota_param->checksum1)
        return 0;
    
    return 1;
}

/**
 * @brief 批量计算APP区域的32位累加和校验值
 * @details 分块读取APP区域FLASH数据，调用ota_file_calc_checksum_u32计算累加和
 * @param app_start_addr APP起始地址（固定为FLASH_APP_BEGIN_ADDR）
 * @param app_total_len  APP总长度（从OTA参数中获取）
 * @return 计算完成的32位累加和校验值
 */
static uint32_t ota_helper_calc_app_checksum_u32(uint32_t app_start_addr, uint32_t app_total_len)
{
    uint32_t sum2 = 0;                      /* 32位累加和结果 */
    uint32_t read_buffer[READ_BUFFER_SIZE / 4] = {0}; /* 按字缓存（4字节/字） */
    uint32_t read_word = 0;                 /* 按字读取的临时变量 */
    uint32_t app_addr = app_start_addr;     /* 当前读取地址 */
    uint32_t remain_bytes = app_total_len;  /* 剩余未计算的字节数 */
    uint32_t buf_idx = 0;                   /* 缓存写入索引（按字计数） */
    uint32_t buf_words = READ_BUFFER_SIZE / 4; /* 缓存总字数 */

    while (remain_bytes > 0)
    {
        // 1. 读取4字节（按字读取，提升效率）
        if (remain_bytes >= 4)
        {
            FMC_Read_Boot(app_addr, &read_word, 1);  // 读取1个字（4字节）
            read_buffer[buf_idx++] = read_word;      // 存入缓存

            app_addr += 4;          /* 地址偏移4字节 */
            remain_bytes -= 4;      /* 剩余字节数减4 */
        }
        // 2. 处理剩余不足4字节的情况
        else
        {
            FMC_Read_Boot(app_addr, &read_word, 1);  // 读取最后1个字
            read_buffer[buf_idx++] = read_word;      // 存入缓存
            
            // 调用ota_file_calc_checksum_u32计算剩余字节的累加和
            sum2 += ota_file_calc_checksum_u32(&read_buffer[0], remain_bytes);
            remain_bytes = 0;       /* 结束循环 */
            continue;
        }

        // 3. 缓存满时调用ota_file_calc_checksum_u32计算
        if (buf_idx >= buf_words)
        {
            // 缓存满，计算整段缓存的累加和（4字节对齐）
            sum2 += ota_file_calc_checksum_u32(read_buffer, READ_BUFFER_SIZE);
            buf_idx = 0;            /* 重置缓存索引 */
        }
    }

    // 4. 处理缓存中未计算的最后一批数据（不足buf_words的部分）
    if (buf_idx > 0)
    {
        // 计算剩余缓存数据的累加和（按实际字数转换为字节数）
        sum2 += ota_file_calc_checksum_u32(read_buffer, buf_idx * 4);
    }

    return sum2;
}

//=============================================================
// APP完整性校验（核心业务功能）
//=============================================================

/**
 * @brief 检查APP固件是否完整
 * @details 校验APP长度合法性 + 32位累加和校验
 * @return 1-APP完整，0-APP不完整/校验失败
 * @note 需先调用ota_helper_init读取OTA参数到全局缓存
 */
uint8_t ota_helper_check_app_complete(void)
{
    // 1. 先校验OTA参数的16位累加和（sum1）
    if (!ota_helper_check_param_checksum1(&ota_fmc_area))
        return 0;

    // // 2. 校验APP长度合法性（防止非法长度）
    // if ((ota_fmc_area.size < MIN_APP_SIZE) || (ota_fmc_area.size > MAX_APP_SIZE))
    //     return 0;

    // 3. 批量计算APP区域32位累加和
    uint32_t sum2 = 0;
    sum2 = ota_helper_calc_app_checksum_u32(FLASH_APP_BEGIN_ADDR, ota_fmc_area.size);

    if (sum2 != ota_fmc_area.checksum2)
        return 0;

    return 1;
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
 * @param length  写入数据长度（字节）
 * @return 1-写入成功，0-写入失败（地址不匹配）
 */
uint8_t ota_helper_write(uint32_t addr, uint8_t *buffer, uint32_t length)
{
    /* 校验写入地址：必须与当前待写入地址一致（防止地址错乱） */
    // OB_LOGD_DUMP(&ota_helper_handle.addr, 2);
    // OB_LOGD_DUMP(&addr, 2);
    // OB_LOGD_DUMP(&length, 2);
    if (ota_helper_handle.addr != addr)
    {
        // OB_LOGD("FAIL");
        return 0;
    }
    /* 写入数据到FLASH（按字写入） */
    FMC_Write(addr, (uint8_t *)buffer, length);
    ota_helper_handle.addr += length; /* 更新下一次写入地址 */

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


//=============================================================
// OTA状态（state）操作接口（新增核心）
//=============================================================

/**
 * @brief 读取当前OTA状态
 * @details 获取全局缓存中ota_fmc_area.state的值
 * @return 当前OTA状态值（如OTA_APP_VALID_DEFAULT/OTA_STATE_READY）
 */
uint32_t ota_helper_get_state(void)
{
    return ota_fmc_area.state;
}

/**
 * @brief 修改OTA状态并同步到FLASH
 * @details 更新全局缓存中的state值，并写入FLASH指定地址
 * @param state 新的OTA状态值
 * @return 0-修改成功，1-修改失败（当前默认返回0）
 */
uint32_t ota_helper_set_state(uint32_t state)
{
    ota_helper_read_param();
    ota_fmc_area.state = state; /* 更新内存中的状态值 */

    ota_helper_save_fmc_area(&ota_fmc_area);
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
