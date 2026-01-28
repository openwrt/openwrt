// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI-NAND Flash driver for EcoNet EN751221 SoC
 *
 * Minimal read-only implementation for lzma-loader boot support.
 * Supports Dosilicon, MXIC, Toshiba, and Winbond SPI-NAND chips.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#include "spi_nand.h"
#include "spi_controller.h"
#include "printf.h"

/* Global flash information */
static struct spinand_info flash_info;

/*
 * Supported SPI-NAND device table
 *
 * Note: dummy_bytes indicates number of dummy bytes for READ_FROM_CACHE (0x03):
 *   - 1 = append dummy byte (Dosilicon and most chips)
 *   - 0 = no dummy byte (uncommon, check datasheet)
 */
static const struct spinand_device supported_devices[] = {
    /* Dosilicon (used in Platinum 4410) */
    { 0xE5, 0x71, "Dosilicon DS35Q1GA", 2048, 64, 64, 1024, 1 },
    { 0xE5, 0x21, "Dosilicon DS35M1GA", 2048, 64, 64, 1024, 1 },

    /* MXIC (Macronix) */
    { 0xC2, 0x12, "MXIC MX35LF1GE4AB", 2048, 64, 64, 1024, 1 },

    /* Toshiba */
    { 0x98, 0xCB, "Toshiba TC58CVG1S3H", 2048, 64, 64, 2048, 1 },

    /* Winbond */
    { 0xEF, 0xAA, "Winbond W25N01GV", 2048, 64, 64, 1024, 1 },

    /* Terminator */
    { 0, 0, NULL, 0, 0, 0, 0, 0 }
};

/* Simple delay for wait loops */
static void udelay(uint32_t us)
{
    volatile uint32_t i;
    /* Rough delay - assumes ~200MHz CPU */
    for (i = 0; i < us * 50; i++)
        ;
}

/*
 * Read manufacturer and device ID
 */
int spi_nand_read_id(uint8_t *mfr_id, uint8_t *dev_id)
{
    uint8_t cmd[2] = { SPINAND_CMD_READ_ID, 0x00 };
    uint8_t id[2];

    /* CS low */
    spi_cs_low();

    /* Send READ_ID command + address byte */
    spi_write_nbyte(cmd, 2);

    /* Read manufacturer ID and device ID */
    spi_read_nbyte(id, 2);

    /* CS high */
    spi_cs_high();

    *mfr_id = id[0];
    *dev_id = id[1];

    return 0;
}

/*
 * Read feature register
 */
int spi_nand_get_feature(uint8_t reg, uint8_t *val)
{
    uint8_t cmd[2] = { SPINAND_CMD_GET_FEATURE, reg };

    /* CS low */
    spi_cs_low();

    /* Send GET_FEATURE command + register address */
    spi_write_nbyte(cmd, 2);

    /* Read register value */
    spi_read_nbyte(val, 1);

    /* CS high */
    spi_cs_high();

    return 0;
}

/*
 * Wait for flash to become ready (OIP bit clears)
 */
int spi_nand_wait_ready(uint32_t timeout_ms)
{
    uint8_t status;
    uint32_t timeout;

    /* Convert ms to loop iterations (rough estimate) */
    timeout = timeout_ms * 100;

    do {
        if (spi_nand_get_feature(SPINAND_REG_STATUS, &status) < 0)
            return -1;

        if (!(status & SPINAND_STATUS_OIP))
            return 0;  /* Ready */

        udelay(10);
    } while (--timeout > 0);

    return -1;  /* Timeout */
}

/*
 * Load page into flash chip's internal cache
 */
static int spi_nand_page_read_to_cache(uint32_t page)
{
    uint8_t cmd[4];

    /* Build PAGE_READ command with row address */
    cmd[0] = SPINAND_CMD_PAGE_READ;
    cmd[1] = (page >> 16) & 0xFF;  /* Row address byte 2 */
    cmd[2] = (page >> 8) & 0xFF;   /* Row address byte 1 */
    cmd[3] = page & 0xFF;          /* Row address byte 0 */

    /* CS low */
    spi_cs_low();

    /* Send command */
    spi_write_nbyte(cmd, 4);

    /* CS high */
    spi_cs_high();

    return 0;
}

/*
 * Read data from flash chip's internal cache
 */
static int spi_nand_read_from_cache(uint16_t column, uint8_t *buf, uint32_t len)
{
    uint8_t cmd[4];

    /* Build READ_FROM_CACHE command */
    cmd[0] = SPINAND_CMD_READ_FROM_CACHE;
    cmd[1] = (column >> 8) & 0xFF;  /* Column address high */
    cmd[2] = column & 0xFF;         /* Column address low */
    cmd[3] = 0x00;                  /* Dummy byte (required by Dosilicon) */

    /* CS low */
    spi_cs_low();

    /* Send command + address + dummy */
    spi_write_nbyte(cmd, 4);

    /* Read data */
    spi_read_nbyte(buf, len);

    /* CS high */
    spi_cs_high();

    return 0;
}

/*
 * Read a complete page (data only)
 */
int spi_nand_read_page(uint32_t page, uint8_t *buf)
{
    /* Step 1: Load page into cache */
    if (spi_nand_page_read_to_cache(page) < 0)
        return -1;

    /* Step 2: Wait for page load to complete */
    if (spi_nand_wait_ready(100) < 0)  /* 100ms timeout */
        return -1;

    /* Step 3: Read from cache (data area only, column 0 to page_size) */
    if (spi_nand_read_from_cache(0, buf, flash_info.page_size) < 0)
        return -1;

    return 0;
}

/*
 * Read a complete page including OOB
 */
int spi_nand_read_page_with_oob(uint32_t page, uint8_t *buf, uint8_t *oob)
{
    /* Step 1: Load page into cache */
    if (spi_nand_page_read_to_cache(page) < 0)
        return -1;

    /* Step 2: Wait for page load to complete */
    if (spi_nand_wait_ready(100) < 0)
        return -1;

    /* Step 3: Read main data from cache */
    if (spi_nand_read_from_cache(0, buf, flash_info.page_size) < 0)
        return -1;

    /* Step 4: Read OOB data from cache */
    if (spi_nand_read_from_cache(flash_info.page_size, oob, flash_info.oob_size) < 0)
        return -1;

    return 0;
}

/*
 * Initialize SPI-NAND flash
 */
int spi_nand_init(void)
{
    uint8_t mfr_id, dev_id;
    const struct spinand_device *device = NULL;
    int i;

    /* Initialize SPI controller first */
    spi_controller_init();

    /* Read flash ID */
    if (spi_nand_read_id(&mfr_id, &dev_id) < 0) {
        printf("SPI-NAND: Failed to read ID\n");
        return -1;
    }

    printf("SPI-NAND: ID %02x:%02x\n", mfr_id, dev_id);

    /* Search device table */
    for (i = 0; supported_devices[i].name != NULL; i++) {
        if (supported_devices[i].mfr_id == mfr_id &&
            supported_devices[i].dev_id == dev_id) {
            device = &supported_devices[i];
            break;
        }
    }

    if (device == NULL) {
        printf("SPI-NAND: Unsupported chip %02x:%02x\n", mfr_id, dev_id);
        printf("SPI-NAND: Add entry to supported_devices[] table\n");
        return -1;
    }

    /* Initialize flash info structure from device table */
    flash_info.mfr_id = mfr_id;
    flash_info.dev_id = dev_id;
    flash_info.page_size = device->page_size;
    flash_info.oob_size = device->oob_size;
    flash_info.pages_per_block = device->pages_per_block;
    flash_info.total_blocks = device->total_blocks;

    printf("SPI-NAND: %s\n", device->name);
    printf("  Capacity: %d MB (%d blocks)\n",
           (device->page_size * device->pages_per_block * device->total_blocks) / (1024 * 1024),
           device->total_blocks);
    printf("  Page: %d bytes, OOB: %d bytes\n",
           device->page_size, device->oob_size);
    printf("  Block: %d KB (%d pages)\n",
           (device->page_size * device->pages_per_block) / 1024,
           device->pages_per_block);

    return 0;
}

/*
 * Get flash information
 */
const struct spinand_info *spi_nand_get_info(void)
{
    return &flash_info;
}

/*
 * Helper: Convert block number to first page number
 */
uint32_t spi_nand_block_to_page(uint32_t block)
{
    return block * flash_info.pages_per_block;
}

/*
 * Helper: Convert page number to block number
 */
uint32_t spi_nand_page_to_block(uint32_t page)
{
    return page / flash_info.pages_per_block;
}

/*
 * Helper: Convert byte address to page number
 */
uint32_t spi_nand_addr_to_page(uint32_t addr)
{
    return addr / flash_info.page_size;
}

/*
 * Helper: Get offset within page
 */
uint32_t spi_nand_addr_to_offset(uint32_t addr)
{
    return addr % flash_info.page_size;
}
