// SPDX-License-Identifier: GPL-2.0-only
/*
 * BMT (Bad Block Management Table) driver for EcoNet EN751221 SoC
 *
 * Minimal read-only implementation for lzma-loader boot support.
 * Handles both BBT (factory bad blocks) and BMT (worn blocks).
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#ifndef _BMT_H_
#define _BMT_H_

#include <stdint.h>

/*
 * BBT (Bad Block Table) - Factory Bad Blocks
 *
 * The BBT contains blocks that were marked bad at the factory.
 * When a block is in the BBT, all blocks after it are shifted up by one,
 * so block addresses must be translated through the BBT before using them.
 *
 * The BBT is stored at the bottom of the reserve area (first good block).
 * Signature: "RAWB", Version: 1
 */

#define BBT_SIGNATURE               "RAWB"
#define BBT_SIGNATURE_SIZE          4
#define BBT_VERSION                 1

/* Default 1000, Genexis devices use 250 */
#ifndef MAX_BBT_SIZE
#define MAX_BBT_SIZE                1000
#endif

struct bbt_header {
    char     signature[4];          /* "RAWB" */
    uint32_t checksum;              /* 16-bit checksum in lower 16 bits */
    uint8_t  version;               /* 1 */
    uint8_t  size;                  /* Number of bad blocks in table */
    uint8_t  reserved[2];           /* Unused (0xFFFF) */
} __attribute__((packed));

struct bbt_table {
    struct bbt_header header;
    uint16_t table[MAX_BBT_SIZE];   /* Bad block indices (sorted ascending) */
} __attribute__((packed));

/*
 * BMT (Block Mapping Table) - Worn Blocks
 *
 * The BMT contains mappings from bad blocks (that failed after factory)
 * to replacement blocks in the reserve area. This allows remapping of
 * blocks that wear out over time.
 *
 * The BMT is stored at the top of the reserve area (last good block).
 * Signature: "BMT", Version: 1
 */

#define BMT_SIGNATURE               "BMT"
#define BMT_SIGNATURE_SIZE          3
#define BMT_VERSION                 1
#define MAX_BMT_SIZE                256

struct bmt_header {
    char    signature[3];           /* "BMT" */
    uint8_t version;                /* 1 */
    uint8_t bad_count;              /* Unused */
    uint8_t size;                   /* Number of mappings in table */
    uint8_t checksum;               /* Checksum of header + table */
    uint8_t reserved[13];           /* Unused */
} __attribute__((packed));

struct bmt_entry {
    uint16_t from;                  /* Bad block index (logical) */
    uint16_t to;                    /* Replacement block index (physical, in reserve area) */
} __attribute__((packed));

struct bmt_table {
    struct bmt_header header;
    struct bmt_entry table[MAX_BMT_SIZE];
} __attribute__((packed));

/*
 * Reserve Area Configuration
 *
 * The reserve area is located at the end of the flash and contains:
 * - BBT block (at bottom/start of reserve area)
 * - Free blocks and mapped blocks (middle)
 * - BMT block (at top/end of reserve area)
 *
 * The reserve area size is calculated by counting down from the end
 * until we have REQUIRED_GOOD_BLOCKS good (non-bad) blocks.
 * This is typically 8% of the total flash size.
 */

/* Reserve area = 8% of total blocks */
#define RESERVE_PERCENT             8

/*
 * Bad block marker in OOB
 * - 0x00: Factory bad block
 * - 0x55: Worn block (marked by BMT system)
 * - 0xFF: Good block
 */
#define BAD_BLOCK_MARKER_FACTORY    0x00
#define BAD_BLOCK_MARKER_WORN       0x55
#define BAD_BLOCK_MARKER_GOOD       0xFF

/*
 * BMT Context
 *
 * This structure holds the runtime BMT state including:
 * - Loaded BBT and BMT tables
 * - Reserve area boundaries
 * - Flash geometry
 */
struct bmt_context {
    /* Flash geometry */
    uint32_t total_blocks;          /* Total blocks in flash */
    uint32_t pages_per_block;       /* Pages per block */
    uint32_t page_size;             /* Page size in bytes */

    /* Reserve area */
    uint32_t reserve_start_block;   /* First block of reserve area */
    uint32_t reserve_block_count;   /* Number of blocks in reserve */

    /* Loaded tables */
    struct bbt_table bbt;           /* Factory bad block table */
    struct bmt_table bmt;           /* Worn block mapping table */

    /* Table block locations */
    uint32_t bbt_block;             /* Block where BBT is stored */
    uint32_t bmt_block;             /* Block where BMT is stored */
};

/*
 * Function prototypes
 */

/**
 * bmt_init - Initialize BMT subsystem
 * @total_blocks: Total number of blocks in flash
 * @pages_per_block: Number of pages per block
 * @page_size: Size of each page in bytes
 *
 * Scans the reserve area to find and load BBT and BMT tables.
 * Calculates reserve area boundaries.
 *
 * Returns: 0 on success, -1 on error
 */
int bmt_init(uint32_t total_blocks, uint32_t pages_per_block, uint32_t page_size);

/**
 * bmt_translate_block - Translate logical block to physical block
 * @block: Logical block number
 *
 * Applies both BBT and BMT translations:
 * 1. First translates through BBT (factory bad blocks)
 * 2. Then translates through BMT (worn blocks)
 *
 * Returns: Physical block number, or -1 if block is in reserve area or invalid
 */
int bmt_translate_block(uint32_t block);

/**
 * bmt_translate_page - Translate logical page to physical page
 * @page: Logical page number
 *
 * Convenience function that translates page address by:
 * 1. Converting page to block number
 * 2. Translating block through BMT
 * 3. Converting back to page number with same page offset
 *
 * Returns: Physical page number, or -1 on error
 */
int bmt_translate_page(uint32_t page);

/**
 * bmt_get_context - Get BMT context
 *
 * Returns: Pointer to BMT context structure (valid after bmt_init)
 */
const struct bmt_context *bmt_get_context(void);

#endif /* _BMT_H_ */
