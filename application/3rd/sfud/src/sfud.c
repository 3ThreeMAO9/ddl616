/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: serial flash operate functions by SFUD lib.
 * Created on: 2016-04-23
 */

#include "sfud.h"
#include <string.h>

/* send dummy data for read data */
#define DUMMY_DATA                               0xFF

#ifndef SFUD_FLASH_DEVICE_TABLE
#error "Please configure the flash device information table in (in sfud_cfg.h)."
#endif

/* user configured flash device information table */
static sfud_flash flash_table[] = SFUD_FLASH_DEVICE_TABLE;
/* mf_table 厂商名表已删除（只给 SFUD_INFO 用） */

#ifdef SFUD_USING_FLASH_INFO_TABLE
/* supported flash chip information table */
static const sfud_flash_chip flash_chip_table[] = SFUD_FLASH_CHIP_TABLE;
#endif

#ifdef SFUD_USING_QSPI
/**
 * flash read data mode
 */
enum sfud_qspi_read_mode {
    NORMAL_SPI_READ = 1 << 0,               /**< mormal spi read mode */
    DUAL_OUTPUT = 1 << 1,                   /**< qspi fast read dual output */
    DUAL_IO = 1 << 2,                       /**< qspi fast read dual input/output */
    QUAD_OUTPUT = 1 << 3,                   /**< qspi fast read quad output */
    QUAD_IO = 1 << 4,                       /**< qspi fast read quad input/output */
};

/* QSPI flash chip's extended information table */
static const sfud_qspi_flash_ext_info qspi_flash_ext_info_table[] = SFUD_FLASH_EXT_INFO_TABLE;
#endif /* SFUD_USING_QSPI */

static sfud_err software_init(const sfud_flash *flash);
static sfud_err hardware_init(sfud_flash *flash);
static sfud_err page256_or_1_byte_write(const sfud_flash *flash, uint32_t addr, size_t size, uint16_t write_gran,
        const uint8_t *data);
static sfud_err wait_busy(const sfud_flash *flash);
static sfud_err reset(const sfud_flash *flash);
static sfud_err read_jedec_id(sfud_flash *flash);
static sfud_err set_write_enabled(const sfud_flash *flash, bool enabled);
static void make_address_byte_array(const sfud_flash *flash, uint32_t addr, uint8_t *array);

/* ../port/sfup_port.c */
extern void sfud_log_debug(const char *file, const long line, const char *format, ...);
extern void sfud_log_info(const char *format, ...);

/**
 * SFUD initialize by flash device
 *
 * @param flash flash device
 *
 * @return result
 */
sfud_err sfud_device_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    /* hardware initialize */
    result = hardware_init(flash);
    if (result == SFUD_SUCCESS) {
        result = software_init(flash);
    }
    if (result == SFUD_SUCCESS) {
        flash->init_ok = true;
        SFUD_INFO("%s flash device initialized successfully.", flash->name);
    } else {
        flash->init_ok = false;
        SFUD_INFO("Error: %s flash device initialization failed.", flash->name);
    }

    return result;
}

/**
 * SFUD library initialize.
 *
 * @return result
 */
sfud_err sfud_init(void) {
    sfud_err cur_flash_result = SFUD_SUCCESS, all_flash_result = SFUD_SUCCESS;
    size_t i;

    SFUD_DEBUG("Start initialize Serial Flash Universal Driver(SFUD) V%s.", SFUD_SW_VERSION);
    SFUD_DEBUG("You can get the latest version on https://github.com/armink/SFUD .");
    
    /* initialize all flash device in flash device table */
    for (i = 0; i < sizeof(flash_table) / sizeof(sfud_flash); i++) {
        /* initialize flash device index of flash device information table */
        flash_table[i].index = i;
        cur_flash_result = sfud_device_init(&flash_table[i]);

        if (cur_flash_result != SFUD_SUCCESS) {
            all_flash_result = cur_flash_result;
        }
    }

    return all_flash_result;
}

/**
 * get flash device by its index which in the flash information table
 *
 * @param index the index which in the flash information table  @see flash_table
 *
 * @return flash device
 */
sfud_flash *sfud_get_device(size_t index) {
    if (index < sfud_get_device_num()) {
        return &flash_table[index];
    } else {
        return NULL;
    }
}

/**
 * get flash device total number on flash device information table  @see flash_table
 *
 * @return flash device total number
 */
size_t sfud_get_device_num(void) {
    return sizeof(flash_table) / sizeof(sfud_flash);
}

/**
 * get flash device information table  @see flash_table
 *
 * @return flash device table pointer
 */
const sfud_flash *sfud_get_device_table(void) {
    return flash_table;
}

#ifdef SFUD_USING_QSPI
static void qspi_set_read_cmd_format(sfud_flash *flash, uint8_t ins, uint8_t ins_lines, uint8_t addr_lines,
        uint8_t dummy_cycles, uint8_t data_lines) {
    /* if medium size greater than 16Mb, use 4-Byte address, instruction should be added one */
    if (flash->chip.capacity <= 0x1000000) {
        flash->read_cmd_format.instruction = ins;
        flash->read_cmd_format.address_size = 24;
    } else {
        if(ins == SFUD_CMD_READ_DATA){
            flash->read_cmd_format.instruction = ins + 0x10;
        }
        else{
            flash->read_cmd_format.instruction = ins + 1;
        }
        flash->read_cmd_format.address_size = 32;
    }

    flash->read_cmd_format.instruction_lines = ins_lines;
    flash->read_cmd_format.address_lines = addr_lines;
    flash->read_cmd_format.alternate_bytes_lines = 0;
    flash->read_cmd_format.dummy_cycles = dummy_cycles;
    flash->read_cmd_format.data_lines = data_lines;
}

/**
 * Enbale the fast read mode in QSPI flash mode. Default read mode is normal SPI mode.
 *
 * it will find the appropriate fast-read instruction to replace the read instruction(0x03)
 * fast-read instruction @see SFUD_FLASH_EXT_INFO_TABLE
 *
 * @note When Flash is in QSPI mode, the method must be called after sfud_device_init().
 *
 * @param flash flash device
 * @param data_line_width the data lines max width which QSPI bus supported, such as 1, 2, 4
 *
 * @return result
 */
sfud_err sfud_qspi_fast_read_enable(sfud_flash *flash, uint8_t data_line_width) {
    size_t i = 0;
    uint8_t read_mode = NORMAL_SPI_READ;
    sfud_err result = SFUD_SUCCESS;

    SFUD_ASSERT(flash);
    SFUD_ASSERT(data_line_width == 1 || data_line_width == 2 || data_line_width == 4);

    /* get read_mode, If don't found, the default is SFUD_QSPI_NORMAL_SPI_READ */
    for (i = 0; i < sizeof(qspi_flash_ext_info_table) / sizeof(sfud_qspi_flash_ext_info); i++) {
        if ((qspi_flash_ext_info_table[i].mf_id == flash->chip.mf_id)
                && (qspi_flash_ext_info_table[i].type_id == flash->chip.type_id)
                && (qspi_flash_ext_info_table[i].capacity_id == flash->chip.capacity_id)) {
            read_mode = qspi_flash_ext_info_table[i].read_mode;
        }
    }

    /* determine qspi supports which read mode and set read_cmd_format struct */
    switch (data_line_width) {
    case 1:
        qspi_set_read_cmd_format(flash, SFUD_CMD_READ_DATA, 1, 1, 0, 1);
        break;
    case 2:
        if (read_mode & DUAL_IO) {
            qspi_set_read_cmd_format(flash, SFUD_CMD_DUAL_IO_READ_DATA, 1, 2, 4, 2);
        } else if (read_mode & DUAL_OUTPUT) {
            qspi_set_read_cmd_format(flash, SFUD_CMD_DUAL_OUTPUT_READ_DATA, 1, 1, 8, 2);
        } else {
            qspi_set_read_cmd_format(flash, SFUD_CMD_READ_DATA, 1, 1, 0, 1);
        }
        break;
    case 4:
        if (read_mode & QUAD_IO) {
            qspi_set_read_cmd_format(flash, SFUD_CMD_QUAD_IO_READ_DATA, 1, 4, 6, 4);
        } else if (read_mode & QUAD_OUTPUT) {
            qspi_set_read_cmd_format(flash, SFUD_CMD_QUAD_OUTPUT_READ_DATA, 1, 1, 8, 4);
        } else {
            qspi_set_read_cmd_format(flash, SFUD_CMD_READ_DATA, 1, 1, 0, 1);
        }
        break;
    }

    return result;
}
#endif /* SFUD_USING_QSPI */

/**
 * hardware initialize
 */
static sfud_err hardware_init(sfud_flash *flash) {
    extern sfud_err sfud_spi_port_init(sfud_flash * flash);

    sfud_err result = SFUD_SUCCESS;

    SFUD_ASSERT(flash);

    result = sfud_spi_port_init(flash);
    if (result != SFUD_SUCCESS) {
        return result;
    }

#ifdef SFUD_USING_QSPI
    /* set default read instruction */
    flash->read_cmd_format.instruction = SFUD_CMD_READ_DATA;
#endif /* SFUD_USING_QSPI */

    /* SPI write read function must be initialize */
    SFUD_ASSERT(flash->spi.wr);
    /* if the user don't configure flash chip information then using SFDP parameter or static flash parameter table */
    if (flash->chip.capacity == 0 || flash->chip.write_mode == 0 || flash->chip.erase_gran == 0
            || flash->chip.erase_gran_cmd == 0) {
        /* read JEDEC ID include manufacturer ID, memory type ID and flash capacity ID */
        result = read_jedec_id(flash);
        if (result != SFUD_SUCCESS) {
            return result;
        }

#ifdef SFUD_USING_SFDP
        extern bool sfud_read_sfdp(sfud_flash *flash);
        /* read SFDP parameters */
        if (sfud_read_sfdp(flash)) {
            flash->chip.name = NULL;
            flash->chip.capacity = flash->sfdp.capacity;
            /* only 1 byte or 256 bytes write mode for SFDP */
            if (flash->sfdp.write_gran == 1) {
                flash->chip.write_mode = SFUD_WM_BYTE;
            } else {
                flash->chip.write_mode = SFUD_WM_PAGE_256B;
            }
            /* find the the smallest erase sector size for eraser. then will use this size for erase granularity */
            flash->chip.erase_gran = flash->sfdp.eraser[0].size;
            flash->chip.erase_gran_cmd = flash->sfdp.eraser[0].cmd;
            for (i = 1; i < SFUD_SFDP_ERASE_TYPE_MAX_NUM; i++) {
                if (flash->sfdp.eraser[i].size != 0 && flash->chip.erase_gran > flash->sfdp.eraser[i].size) {
                    flash->chip.erase_gran = flash->sfdp.eraser[i].size;
                    flash->chip.erase_gran_cmd = flash->sfdp.eraser[i].cmd;
                }
            }
        } else {
#endif

#ifdef SFUD_USING_FLASH_INFO_TABLE
            /* read SFDP parameters failed then using SFUD library provided static parameter */
            for (i = 0; i < sizeof(flash_chip_table) / sizeof(sfud_flash_chip); i++) {
                if ((flash_chip_table[i].mf_id == flash->chip.mf_id)
                        && (flash_chip_table[i].type_id == flash->chip.type_id)
                        && (flash_chip_table[i].capacity_id == flash->chip.capacity_id)) {
                    flash->chip.name = flash_chip_table[i].name;
                    flash->chip.capacity = flash_chip_table[i].capacity;
                    flash->chip.write_mode = flash_chip_table[i].write_mode;
                    flash->chip.erase_gran = flash_chip_table[i].erase_gran;
                    flash->chip.erase_gran_cmd = flash_chip_table[i].erase_gran_cmd;
                    break;
                }
            }
#endif

#ifdef SFUD_USING_SFDP
        }
#endif

    }

    if (flash->chip.capacity == 0 || flash->chip.write_mode == 0 || flash->chip.erase_gran == 0
            || flash->chip.erase_gran_cmd == 0) {
        return SFUD_ERR_NOT_FOUND;
    }

    /* reset flash device */
    result = reset(flash);
    if (result != SFUD_SUCCESS) {
        return result;
    }

    /* 本芯片固定 PAGE_256B / ≤16MB：AAI 写保护与 4 字节地址分支已删除 */
    flash->addr_in_4_byte = false;

    return result;
}

/**
 * software initialize
 *
 * @param flash flash device
 *
 * @return result
 */
static sfud_err software_init(const sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    SFUD_ASSERT(flash);

    return result;
}

/**
 * read flash data
 *
 * @param flash flash device
 * @param addr start address
 * @param size read size
 * @param data read data pointer
 *
 * @return result
 */
sfud_err sfud_read(const sfud_flash *flash, uint32_t addr, size_t size, uint8_t *data) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_spi *spi = &flash->spi;
    uint8_t cmd_data[5], cmd_size;

    SFUD_ASSERT(flash);
    SFUD_ASSERT(data);
    /* must be call this function after initialize OK */
    SFUD_ASSERT(flash->init_ok);
    /* check the flash address bound */
    if (addr + size > flash->chip.capacity) {
        SFUD_INFO("Error: Flash address is out of bound.");
        return SFUD_ERR_ADDR_OUT_OF_BOUND;
    }
    /* lock SPI */
    if (spi->lock) {
        spi->lock(spi);
    }

    result = wait_busy(flash);

    if (result == SFUD_SUCCESS) {
#ifdef SFUD_USING_QSPI
        if (flash->read_cmd_format.instruction != SFUD_CMD_READ_DATA) {
            result = spi->qspi_read(spi, addr, (sfud_qspi_read_cmd_format *)&flash->read_cmd_format, data, size);
        } else
#endif
        {
            cmd_data[0] = SFUD_CMD_READ_DATA;
            make_address_byte_array(flash, addr, &cmd_data[1]);
            cmd_size = 4;   /* 固定 1 字节命令 + 3 字节地址 */
            result = spi->wr(spi, cmd_data, cmd_size, data, size);
        }
    }
    /* unlock SPI */
    if (spi->unlock) {
        spi->unlock(spi);
    }

    return result;
}

/* sfud_chip_erase() 已删除：整片擦除改由下面的扇区循环完成 */

/**
 * erase flash data
 *
 * @note It will erase align by erase granularity.
 *
 * @param flash flash device
 * @param addr start address
 * @param size erase size
 *
 * @return result
 */
sfud_err sfud_erase(const sfud_flash *flash, uint32_t addr, size_t size) {
    extern size_t sfud_sfdp_get_suitable_eraser(const sfud_flash *flash, uint32_t addr, size_t erase_size);

    sfud_err result = SFUD_SUCCESS;
    const sfud_spi *spi = &flash->spi;
    uint8_t cmd_data[5], cmd_size, cur_erase_cmd;
    size_t cur_erase_size;

    SFUD_ASSERT(flash);
    /* must be call this function after initialize OK */
    SFUD_ASSERT(flash->init_ok);
    /* check the flash address bound */
    if (addr + size > flash->chip.capacity) {
        SFUD_INFO("Error: Flash address is out of bound.");
        return SFUD_ERR_ADDR_OUT_OF_BOUND;
    }

    /* 整片擦除分支已删除，走下面的扇区循环 */

    /* lock SPI */
    if (spi->lock) {
        spi->lock(spi);
    }

    /* loop erase operate. erase unit is erase granularity */
    while (size) {
        /* if this flash is support SFDP parameter, then used SFDP parameter supplies eraser */
#ifdef SFUD_USING_SFDP
        size_t eraser_index;
        if (flash->sfdp.available) {
            /* get the suitable eraser for erase process from SFDP parameter */
            eraser_index = sfud_sfdp_get_suitable_eraser(flash, addr, size);
            cur_erase_cmd = flash->sfdp.eraser[eraser_index].cmd;
            cur_erase_size = flash->sfdp.eraser[eraser_index].size;
        } else {
#else
        {
#endif
            cur_erase_cmd = flash->chip.erase_gran_cmd;
            cur_erase_size = flash->chip.erase_gran;
        }
        /* set the flash write enable */
        result = set_write_enabled(flash, true);
        if (result != SFUD_SUCCESS) {
            goto __exit;
        }

        cmd_data[0] = cur_erase_cmd;
        make_address_byte_array(flash, addr, &cmd_data[1]);
        cmd_size = 4;   /* 固定 1 字节命令 + 3 字节地址 */
        result = spi->wr(spi, cmd_data, cmd_size, NULL, 0);
        if (result != SFUD_SUCCESS) {
            SFUD_INFO("Error: Flash erase SPI communicate error.");
            goto __exit;
        }
        result = wait_busy(flash);
        if (result != SFUD_SUCCESS) {
            goto __exit;
        }
        /* make erase align and calculate next erase address */
        if (addr % cur_erase_size != 0) {
            if (size > cur_erase_size - (addr % cur_erase_size)) {
                size -= cur_erase_size - (addr % cur_erase_size);
                addr += cur_erase_size - (addr % cur_erase_size);
            } else {
                goto __exit;
            }
        } else {
            if (size > cur_erase_size) {
                size -= cur_erase_size;
                addr += cur_erase_size;
            } else {
                goto __exit;
            }
        }
    }

__exit:
    /* set the flash write disable */
    set_write_enabled(flash, false);
    /* unlock SPI */
    if (spi->unlock) {
        spi->unlock(spi);
    }

    return result;
}

/**
 * write flash data (no erase operate) for write 1 to 256 bytes per page mode or byte write mode
 *
 * @param flash flash device
 * @param addr start address
 * @param size write size
 * @param write_gran write granularity bytes, only support 1 or 256
 * @param data write data
 *
 * @return result
 */
static sfud_err page256_or_1_byte_write(const sfud_flash *flash, uint32_t addr, size_t size, uint16_t write_gran,
        const uint8_t *data) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_spi *spi = &flash->spi;
    static uint8_t cmd_data[5 + SFUD_WRITE_MAX_PAGE_SIZE];
    uint8_t cmd_size;
    size_t data_size;

    SFUD_ASSERT(flash);
    /* only support 1 or 256 */
    SFUD_ASSERT(write_gran == 1 || write_gran == 256);
    /* must be call this function after initialize OK */
    SFUD_ASSERT(flash->init_ok);
    /* check the flash address bound */
    if (addr + size > flash->chip.capacity) {
        SFUD_INFO("Error: Flash address is out of bound.");
        return SFUD_ERR_ADDR_OUT_OF_BOUND;
    }
    /* lock SPI */
    if (spi->lock) {
        spi->lock(spi);
    }

    /* loop write operate. write unit is write granularity */
    while (size) {
        /* set the flash write enable */
        result = set_write_enabled(flash, true);
        if (result != SFUD_SUCCESS) {
            goto __exit;
        }
        cmd_data[0] = SFUD_CMD_PAGE_PROGRAM;
        make_address_byte_array(flash, addr, &cmd_data[1]);
        cmd_size = 4;   /* 固定 1 字节命令 + 3 字节地址 */

        /* make write align and calculate next write address */
        if (addr % write_gran != 0) {
            if (size > write_gran - (addr % write_gran)) {
                data_size = write_gran - (addr % write_gran);
            } else {
                data_size = size;
            }
        } else {
            if (size > write_gran) {
                data_size = write_gran;
            } else {
                data_size = size;
            }
        }
        size -= data_size;
        addr += data_size;

        memcpy(&cmd_data[cmd_size], data, data_size);

        result = spi->wr(spi, cmd_data, cmd_size + data_size, NULL, 0);
        if (result != SFUD_SUCCESS) {
            SFUD_INFO("Error: Flash write SPI communicate error.");
            goto __exit;
        }
        result = wait_busy(flash);
        if (result != SFUD_SUCCESS) {
            goto __exit;
        }
        data += data_size;
    }

__exit:
    /* set the flash write disable */
    set_write_enabled(flash, false);
    /* unlock SPI */
    if (spi->unlock) {
        spi->unlock(spi);
    }

    return result;
}

/* aai_write() 已删除（本芯片只用 PAGE_256B） */

/**
 * write flash data (no erase operate)
 *
 * @param flash flash device
 * @param addr start address
 * @param size write size
 * @param data write data
 *
 * @return result
 */
sfud_err sfud_write(const sfud_flash *flash, uint32_t addr, size_t size, const uint8_t *data) {
    sfud_err result = SFUD_SUCCESS;

    /* AAI / 双缓冲写入分支已删除 */
    if (flash->chip.write_mode & SFUD_WM_PAGE_256B) {
        result = page256_or_1_byte_write(flash, addr, size, 256, data);
    }

    return result;
}

/**
 * erase and write flash data
 *
 * @param flash flash device
 * @param addr start address
 * @param size write size
 * @param data write data
 *
 * @return result
 */
sfud_err sfud_erase_write(const sfud_flash *flash, uint32_t addr, size_t size, const uint8_t *data) {
    sfud_err result = SFUD_SUCCESS;

    result = sfud_erase(flash, addr, size);

    if (result == SFUD_SUCCESS) {
        result = sfud_write(flash, addr, size, data);
    }

    return result;
}

static sfud_err reset(const sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_spi *spi = &flash->spi;
    uint8_t cmd_data[2];

    SFUD_ASSERT(flash);
    if (result == SFUD_SUCCESS) {
        result = wait_busy(flash);
    } else {
        SFUD_INFO("Error: Flash device reset failed.");
        return result;
    }
    cmd_data[0] = SFUD_CMD_ENABLE_RESET;
    result = spi->wr(spi, cmd_data, 1, NULL, 0);

    flash->retry.delay();
    cmd_data[1] = SFUD_CMD_RESET;
    result = spi->wr(spi, &cmd_data[1], 1, NULL, 0);
    if (result == SFUD_SUCCESS) {
        result = wait_busy(flash);
    }

    if (result == SFUD_SUCCESS) {
        SFUD_DEBUG("Flash device reset success.");
    } else {
        SFUD_INFO("Error: Flash device reset failed.");
    }

    return result;
}

static sfud_err read_jedec_id(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_spi *spi = &flash->spi;
    uint8_t cmd_data[1], recv_data[3];

    SFUD_ASSERT(flash);

    cmd_data[0] = SFUD_CMD_JEDEC_ID;
    result = spi->wr(spi, cmd_data, sizeof(cmd_data), recv_data, sizeof(recv_data));
    if (result == SFUD_SUCCESS) {
        flash->chip.mf_id = recv_data[0];
        flash->chip.type_id = recv_data[1];
        flash->chip.capacity_id = recv_data[2];
        SFUD_DEBUG("The flash device manufacturer ID is 0x%02X, memory type ID is 0x%02X, capacity ID is 0x%02X.",
                flash->chip.mf_id, flash->chip.type_id, flash->chip.capacity_id);
    } else {
        SFUD_INFO("Error: Read flash device JEDEC ID error.");
    }

    return result;
}

/**
 * set the flash write enable or write disable
 *
 * @param flash flash device
 * @param enabled true: enable  false: disable
 *
 * @return result
 */
static sfud_err set_write_enabled(const sfud_flash *flash, bool enabled) {
    sfud_err result = SFUD_SUCCESS;
    uint8_t cmd, register_status;

    SFUD_ASSERT(flash);

    if (enabled) {
        cmd = SFUD_CMD_WRITE_ENABLE;
    } else {
        cmd = SFUD_CMD_WRITE_DISABLE;
    }

    result = flash->spi.wr(&flash->spi, &cmd, 1, NULL, 0);

    if (result == SFUD_SUCCESS) {
        result = sfud_read_status(flash, &register_status);
    }

    if (result == SFUD_SUCCESS) {
        if (enabled && (register_status & SFUD_STATUS_REGISTER_WEL) == 0) {
            SFUD_INFO("Error: Can't enable write status.");
            return SFUD_ERR_WRITE;
        } else if (!enabled && (register_status & SFUD_STATUS_REGISTER_WEL) != 0) {
            SFUD_INFO("Error: Can't disable write status.");
            return SFUD_ERR_WRITE;
        }
    }

    return result;
}

/* set_4_byte_address_mode() 已删除（≤16MB 用不到） */

/**
 * read flash register status
 *
 * @param flash flash device
 * @param status register status
 *
 * @return result
 */
sfud_err sfud_read_status(const sfud_flash *flash, uint8_t *status) {
    uint8_t cmd = SFUD_CMD_READ_STATUS_REGISTER;

    SFUD_ASSERT(flash);
    SFUD_ASSERT(status);

    return flash->spi.wr(&flash->spi, &cmd, 1, status, 1);
}

static sfud_err wait_busy(const sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;
    uint8_t status;
    size_t retry_times = flash->retry.times;

    SFUD_ASSERT(flash);

    while (true) {
        result = sfud_read_status(flash, &status);
        if (result == SFUD_SUCCESS && ((status & SFUD_STATUS_REGISTER_BUSY)) == 0) {
            break;
        }
        /* retry counts */
        SFUD_RETRY_PROCESS(flash->retry.delay, retry_times, result);
    }

    if (result != SFUD_SUCCESS || ((status & SFUD_STATUS_REGISTER_BUSY)) != 0) {
        SFUD_INFO("Error: Flash wait busy has an error.");
    }

    return result;
}

static void make_address_byte_array(const sfud_flash *flash, uint32_t addr, uint8_t *array) {
    uint8_t len, i;

    SFUD_ASSERT(flash);
    SFUD_ASSERT(array);

    len = 3;    /* 固定 3 字节地址 */

    for (i = 0; i < len; i++) {
        array[i] = (addr >> ((len - (i + 1)) * 8)) & 0xFF;
    }
}

/**
 * write status register
 *
 * @param flash flash device
 * @param is_volatile true: volatile mode, false: non-volatile mode
 * @param status register status
 *
 * @return result
 */
sfud_err sfud_write_status(const sfud_flash *flash, bool is_volatile, uint8_t status) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_spi *spi = &flash->spi;
    uint8_t cmd_data[2];

    SFUD_ASSERT(flash);

    if (is_volatile) {
        cmd_data[0] = SFUD_VOLATILE_SR_WRITE_ENABLE;
        result = spi->wr(spi, cmd_data, 1, NULL, 0);
    } else {
        result = set_write_enabled(flash, true);
    }

    if (result == SFUD_SUCCESS) {
        cmd_data[0] = SFUD_CMD_WRITE_STATUS_REGISTER;
        cmd_data[1] = status;
        result = spi->wr(spi, cmd_data, 2, NULL, 0);
    }

    if (result != SFUD_SUCCESS) {
        SFUD_INFO("Error: Write_status register failed.");
    }

    return result;
}

sfud_err sfud_reset(const sfud_flash *flash)
{
    sfud_err result = SFUD_SUCCESS;
    result = reset(flash);
    if(SFUD_SUCCESS != result){
        result = reset(flash);
    }

    return result;
}
