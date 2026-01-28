// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI-NAND Flash driver for EcoNet EN751221 SoC
 *
 * Minimal read-only implementation for lzma-loader boot support.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#ifndef _SPI_NAND_H_
#define _SPI_NAND_H_

#include <stdint.h>

/* NULL definition for bootloader environment */
#ifndef NULL
#define NULL 0
#endif

/*
 * Maximum supported flash geometry
 *
 * These values define the maximum supported page and OOB sizes across all
 * SPI-NAND chips this driver may support. Used for static buffer allocation.
 */
#define SPINAND_MAX_PAGE_SIZE           4096
#define SPINAND_MAX_OOB_SIZE            256

/*
 * SPI-NAND Command Set
 */
#define SPINAND_CMD_READ_ID                 0x9F
#define SPINAND_CMD_GET_FEATURE             0x0F
#define SPINAND_CMD_SET_FEATURE             0x1F
#define SPINAND_CMD_PAGE_READ               0x13    /* Load page into cache */
#define SPINAND_CMD_READ_FROM_CACHE         0x03    /* Read from cache (single) */
#define SPINAND_CMD_READ_FROM_CACHE_X2      0x3B    /* Read from cache (dual) */
#define SPINAND_CMD_READ_FROM_CACHE_X4      0x6B    /* Read from cache (quad) */

/* Feature register addresses */
#define SPINAND_REG_PROTECTION              0xA0
#define SPINAND_REG_FEATURE                 0xB0
#define SPINAND_REG_STATUS                  0xC0

/* Status register bits */
#define SPINAND_STATUS_OIP                  0x01    /* Operation In Progress */
#define SPINAND_STATUS_WEL                  0x02    /* Write Enable Latch */
#define SPINAND_STATUS_E_FAIL               0x04    /* Erase failed */
#define SPINAND_STATUS_P_FAIL               0x08    /* Program failed */
#define SPINAND_STATUS_ECC_MASK             0x30    /* ECC status mask */
#define SPINAND_STATUS_ECC_SHIFT            4

/* ECC status values */
#define SPINAND_ECC_NO_ERROR                0x00
#define SPINAND_ECC_CORRECTED               0x01
#define SPINAND_ECC_UNCORRECTABLE           0x02

/*
 * Flash information structure
 */
struct spinand_info {
    uint8_t  mfr_id;                /* Manufacturer ID */
    uint8_t  dev_id;                /* Device ID */
    uint32_t page_size;             /* Page size in bytes */
    uint32_t oob_size;              /* OOB/spare size in bytes */
    uint32_t pages_per_block;       /* Pages per block */
    uint32_t total_blocks;          /* Total number of blocks */
};

/*
 * Device table entry for supported SPI-NAND chips
 */
struct spinand_device {
    uint8_t  mfr_id;                /* Manufacturer ID */
    uint8_t  dev_id;                /* Device ID */
    const char *name;               /* Human-readable chip name */
    uint32_t page_size;             /* Page size in bytes */
    uint32_t oob_size;              /* OOB/spare size in bytes */
    uint32_t pages_per_block;       /* Pages per block */
    uint32_t total_blocks;          /* Total number of blocks */
    uint8_t  dummy_bytes;           /* Dummy bytes for READ_FROM_CACHE command */
};

/*
 * Function prototypes
 */

/**
 * spi_nand_init - Initialize and probe SPI-NAND flash
 *
 * Reads flash ID and verifies it's a supported Dosilicon chip.
 * Stores flash geometry in global structure for later use.
 *
 * Returns: 0 on success, -1 on error
 */
int spi_nand_init(void);

/**
 * spi_nand_read_id - Read manufacturer and device ID
 * @mfr_id: Pointer to store manufacturer ID
 * @dev_id: Pointer to store device ID
 *
 * Returns: 0 on success, -1 on error
 */
int spi_nand_read_id(uint8_t *mfr_id, uint8_t *dev_id);

/**
 * spi_nand_get_feature - Read feature register
 * @reg: Register address (SPINAND_REG_*)
 * @val: Pointer to store register value
 *
 * Returns: 0 on success, -1 on error
 */
int spi_nand_get_feature(uint8_t reg, uint8_t *val);

/**
 * spi_nand_wait_ready - Wait for flash operation to complete
 * @timeout_ms: Maximum time to wait in milliseconds
 *
 * Polls the OIP (Operation In Progress) bit until it clears
 * or timeout occurs.
 *
 * Returns: 0 on success, -1 on timeout
 */
int spi_nand_wait_ready(uint32_t timeout_ms);

/**
 * spi_nand_read_page - Read a page from flash
 * @page: Page number to read
 * @buf: Buffer to store page data (must be SPINAND_PAGE_SIZE bytes)
 *
 * Loads the specified page into the flash chip's internal cache,
 * then reads it out. Does not include OOB data.
 *
 * Returns: 0 on success, -1 on error
 */
int spi_nand_read_page(uint32_t page, uint8_t *buf);

/**
 * spi_nand_read_page_with_oob - Read a page including OOB
 * @page: Page number to read
 * @buf: Buffer to store page data (must be SPINAND_PAGE_SIZE bytes)
 * @oob: Buffer to store OOB data (must be SPINAND_OOB_SIZE bytes)
 *
 * Reads both main data and OOB/spare area.
 *
 * Returns: 0 on success, -1 on error
 */
int spi_nand_read_page_with_oob(uint32_t page, uint8_t *buf, uint8_t *oob);

/**
 * spi_nand_block_to_page - Convert block number to first page number
 * @block: Block number
 *
 * Note: Requires spi_nand_init() to be called first
 * Returns: Page number of first page in block
 */
uint32_t spi_nand_block_to_page(uint32_t block);

/**
 * spi_nand_page_to_block - Convert page number to block number
 * @page: Page number
 *
 * Note: Requires spi_nand_init() to be called first
 * Returns: Block number containing the page
 */
uint32_t spi_nand_page_to_block(uint32_t page);

/**
 * spi_nand_addr_to_page - Convert byte address to page number
 * @addr: Byte address
 *
 * Note: Requires spi_nand_init() to be called first
 * Returns: Page number containing the address
 */
uint32_t spi_nand_addr_to_page(uint32_t addr);

/**
 * spi_nand_addr_to_offset - Get offset within page
 * @addr: Byte address
 *
 * Note: Requires spi_nand_init() to be called first
 * Returns: Offset within the page
 */
uint32_t spi_nand_addr_to_offset(uint32_t addr);

/**
 * spi_nand_get_info - Get flash information structure
 *
 * Returns: Pointer to flash info structure (valid after spi_nand_init)
 */
const struct spinand_info *spi_nand_get_info(void);

#endif /* _SPI_NAND_H_ */
