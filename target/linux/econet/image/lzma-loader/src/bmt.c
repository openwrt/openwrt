// SPDX-License-Identifier: GPL-2.0-only
/*
 * BMT (Bad Block Management Table) driver for EcoNet EN751221 SoC
 *
 * Minimal read-only implementation for lzma-loader boot support.
 * Handles both BBT (factory bad blocks) and BMT (worn blocks).
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#include "bmt.h"
#include "spi_nand.h"
#include "printf.h"
#include <string.h>

/* Global BMT context */
static struct bmt_context bmt_ctx;

/* Working buffers for page reads - sized for maximum supported flash geometry */
static uint8_t page_buf[SPINAND_MAX_PAGE_SIZE];
static uint8_t oob_buf[SPINAND_MAX_OOB_SIZE];

/*
 * Calculate BBT checksum
 * Checksums version + size + all table entries
 */
static uint16_t bbt_calc_checksum(const struct bbt_table *bbt)
{
    const uint8_t *data = (const uint8_t *)bbt->table;
    uint16_t checksum = 0;
    uint32_t i;

    /* Add header fields */
    checksum += bbt->header.version;
    checksum += bbt->header.size;

    /* Add all table data */
    for (i = 0; i < sizeof(bbt->table); i++) {
        checksum += data[i];
    }

    return checksum;
}

/*
 * Calculate BMT checksum
 * Checksums version + size + table entries (up to size count)
 */
static uint8_t bmt_calc_checksum(const struct bmt_table *bmt)
{
    const uint8_t *data = (const uint8_t *)bmt->table;
    uint8_t checksum = 0;
    uint32_t i, table_bytes;

    /* Add header fields */
    checksum += bmt->header.version;
    checksum += bmt->header.size;

    /* Add table data (all reserve_block_count entries) */
    table_bytes = bmt_ctx.reserve_block_count * sizeof(struct bmt_entry);
    if (table_bytes > sizeof(bmt->table))
        table_bytes = sizeof(bmt->table);

    for (i = 0; i < table_bytes; i++) {
        checksum += data[i];
    }

    return checksum;
}

/*
 * Check if a block is bad by reading its OOB marker
 * Returns: 1 if bad, 0 if good, -1 on read error
 */
static int is_block_bad(uint32_t block)
{
    uint32_t page;
    uint8_t marker;

    /* Read first page of block */
    page = block * bmt_ctx.pages_per_block;

    if (spi_nand_read_page_with_oob(page, page_buf, oob_buf) < 0)
        return -1;  /* Read error - assume bad */

    /* Check bad block marker (first byte of OOB) */
    marker = oob_buf[0];

    return (marker != BAD_BLOCK_MARKER_GOOD) ? 1 : 0;
}

/*
 * Scan reserve area from end to find good blocks
 * Calculate reserve area start based on REQUIRED_GOOD_BLOCKS
 */
static int bmt_scan_reserve_area(void)
{
    uint32_t block;
    uint32_t good_blocks = 0;
    uint32_t required_good;

    /* Calculate how many good blocks we need (8% of total) */
    required_good = (bmt_ctx.total_blocks * RESERVE_PERCENT) / 100;

    printf("BMT: Scanning for reserve area (need %d good blocks)\n", required_good);

    /* Scan from end of flash backwards */
    for (block = bmt_ctx.total_blocks; block > 0; block--) {
        int bad = is_block_bad(block - 1);

        if (bad < 0) {
            /* Read error - skip this block */
            printf("BMT: Block %d read error, skipping\n", block - 1);
            continue;
        }

        if (!bad) {
            /* Good block found */
            good_blocks++;
        }

        /* Once we have enough good blocks, this is the reserve start */
        if (good_blocks >= required_good) {
            bmt_ctx.reserve_start_block = block - 1;
            bmt_ctx.reserve_block_count = bmt_ctx.total_blocks - bmt_ctx.reserve_start_block;
            printf("BMT: Reserve area: blocks %d-%d (%d blocks, %d good)\n",
                   bmt_ctx.reserve_start_block,
                   bmt_ctx.total_blocks - 1,
                   bmt_ctx.reserve_block_count,
                   good_blocks);
            return 0;
        }
    }

    printf("BMT: ERROR: Could not find enough good blocks for reserve area\n");
    return -1;
}

/*
 * Load BBT from reserve area
 * BBT is at the bottom (first good block) of reserve area
 */
static int bmt_load_bbt(void)
{
    uint32_t block;
    uint32_t page;
    const struct bbt_table *bbt;
    uint16_t checksum;

    printf("BMT: Searching for BBT in reserve area\n");

    /* Scan from start of reserve area upwards */
    for (block = bmt_ctx.reserve_start_block;
         block < bmt_ctx.total_blocks;
         block++) {

        /* Skip bad blocks */
        if (is_block_bad(block))
            continue;

        /* Read first page */
        page = block * bmt_ctx.pages_per_block;
        if (spi_nand_read_page(page, page_buf) < 0)
            continue;

        /* Check signature */
        bbt = (const struct bbt_table *)page_buf;
        if (memcmp(bbt->header.signature, BBT_SIGNATURE, BBT_SIGNATURE_SIZE) != 0)
            continue;

        printf("BMT: Found BBT signature at block %d\n", block);

        /* Verify checksum */
        checksum = bbt_calc_checksum(bbt);
        if ((bbt->header.checksum & 0xFFFF) != checksum) {
            printf("BMT: BBT checksum mismatch (expected 0x%04x, got 0x%04x)\n",
                   (bbt->header.checksum & 0xFFFF), checksum);
            continue;
        }

        /* Verify version */
        if (bbt->header.version != BBT_VERSION) {
            printf("BMT: BBT version mismatch (expected %d, got %d)\n",
                   BBT_VERSION, bbt->header.version);
            continue;
        }

        /* Valid BBT found - copy to context */
        memcpy(&bmt_ctx.bbt, bbt, sizeof(bmt_ctx.bbt));
        bmt_ctx.bbt_block = block;

        printf("BMT: Loaded BBT v%d, %d bad blocks\n",
               bbt->header.version, bbt->header.size);

        return 0;
    }

    printf("BMT: WARNING: No valid BBT found, assuming no factory bad blocks\n");
    /* Initialize empty BBT */
    memset(&bmt_ctx.bbt, 0xFF, sizeof(bmt_ctx.bbt));
    memcpy(bmt_ctx.bbt.header.signature, BBT_SIGNATURE, BBT_SIGNATURE_SIZE);
    bmt_ctx.bbt.header.version = BBT_VERSION;
    bmt_ctx.bbt.header.size = 0;
    bmt_ctx.bbt.header.checksum = bbt_calc_checksum(&bmt_ctx.bbt);
    bmt_ctx.bbt_block = 0;

    return 0;
}

/*
 * Load BMT from reserve area
 * BMT is at the top (last good block) of reserve area
 */
static int bmt_load_bmt(void)
{
    uint32_t block;
    uint32_t page;
    const struct bmt_table *bmt;
    uint8_t checksum;

    printf("BMT: Searching for BMT in reserve area\n");

    /* Scan from end of flash backwards */
    for (block = bmt_ctx.total_blocks; block > bmt_ctx.reserve_start_block; block--) {

        /* Skip bad blocks */
        if (is_block_bad(block - 1))
            continue;

        /* Read first page */
        page = (block - 1) * bmt_ctx.pages_per_block;
        if (spi_nand_read_page(page, page_buf) < 0)
            continue;

        /* Check signature */
        bmt = (const struct bmt_table *)page_buf;
        if (memcmp(bmt->header.signature, BMT_SIGNATURE, BMT_SIGNATURE_SIZE) != 0)
            continue;

        printf("BMT: Found BMT signature at block %d\n", block - 1);

        /* Verify checksum */
        checksum = bmt_calc_checksum(bmt);
        if (bmt->header.checksum != checksum) {
            printf("BMT: BMT checksum mismatch (expected 0x%02x, got 0x%02x)\n",
                   bmt->header.checksum, checksum);
            continue;
        }

        /* Verify version */
        if (bmt->header.version != BMT_VERSION) {
            printf("BMT: BMT version mismatch (expected %d, got %d)\n",
                   BMT_VERSION, bmt->header.version);
            continue;
        }

        /* Valid BMT found - copy to context */
        memcpy(&bmt_ctx.bmt, bmt, sizeof(bmt_ctx.bmt));
        bmt_ctx.bmt_block = block - 1;

        printf("BMT: Loaded BMT v%d, %d mappings\n",
               bmt->header.version, bmt->header.size);

        return 0;
    }

    printf("BMT: No valid BMT found, assuming no worn blocks\n");
    /* Initialize empty BMT */
    memset(&bmt_ctx.bmt, 0xFF, sizeof(bmt_ctx.bmt));
    memcpy(bmt_ctx.bmt.header.signature, BMT_SIGNATURE, BMT_SIGNATURE_SIZE);
    bmt_ctx.bmt.header.version = BMT_VERSION;
    bmt_ctx.bmt.header.size = 0;
    bmt_ctx.bmt.header.bad_count = 0;
    bmt_ctx.bmt.header.checksum = bmt_calc_checksum(&bmt_ctx.bmt);
    bmt_ctx.bmt_block = 0;

    return 0;
}

/*
 * Initialize BMT subsystem
 */
int bmt_init(uint32_t total_blocks, uint32_t pages_per_block, uint32_t page_size)
{
    /* Store geometry */
    bmt_ctx.total_blocks = total_blocks;
    bmt_ctx.pages_per_block = pages_per_block;
    bmt_ctx.page_size = page_size;

    printf("BMT: Initializing (total blocks: %d)\n", total_blocks);

    /* Scan for reserve area */
    if (bmt_scan_reserve_area() < 0)
        return -1;

    /* Load BBT */
    if (bmt_load_bbt() < 0)
        return -1;

    /* Load BMT */
    if (bmt_load_bmt() < 0)
        return -1;

    printf("BMT: Initialization complete\n");
    printf("  User area: blocks 0-%d\n", bmt_ctx.reserve_start_block - 1);
    printf("  Reserve area: blocks %d-%d\n",
           bmt_ctx.reserve_start_block, bmt_ctx.total_blocks - 1);
    printf("  Factory bad blocks: %d\n", bmt_ctx.bbt.header.size);
    printf("  Worn block mappings: %d\n", bmt_ctx.bmt.header.size);

    return 0;
}

/*
 * Translate logical block through BBT
 * Factory bad blocks cause all subsequent blocks to shift up by one
 */
static int bmt_translate_bbt(uint32_t block)
{
    uint32_t i;
    uint32_t physical = block;

    /* For each factory bad block <= logical block, increment physical block */
    for (i = 0; i < bmt_ctx.bbt.header.size; i++) {
        if (bmt_ctx.bbt.table[i] <= physical) {
            physical++;
        }
    }

    /* Ensure we didn't go into reserve area */
    if (physical >= bmt_ctx.reserve_start_block) {
        printf("BMT: ERROR: Block %d translates to reserve area (%d)\n",
               block, physical);
        return -1;
    }

    return physical;
}

/*
 * Translate logical block through BMT
 * Worn blocks are remapped to replacement blocks in reserve area
 */
static int bmt_translate_bmt(uint32_t block)
{
    uint32_t i;

    /* Check if block is mapped */
    for (i = 0; i < bmt_ctx.bmt.header.size; i++) {
        if (bmt_ctx.bmt.table[i].from == block) {
            printf("BMT: Block %d -> %d (mapped)\n",
                   block, bmt_ctx.bmt.table[i].to);
            return bmt_ctx.bmt.table[i].to;
        }
    }

    /* Not mapped, return original block */
    return block;
}

/*
 * Translate logical block to physical block
 * Applies both BBT and BMT translations
 */
int bmt_translate_block(uint32_t block)
{
    int physical;

    /* First translate through BBT (factory bad blocks) */
    physical = bmt_translate_bbt(block);
    if (physical < 0)
        return -1;

    /* Then translate through BMT (worn blocks) */
    physical = bmt_translate_bmt(physical);
    if (physical < 0)
        return -1;

    return physical;
}

/*
 * Translate logical page to physical page
 */
int bmt_translate_page(uint32_t page)
{
    uint32_t block, page_offset;
    int physical_block;

    /* Extract block and page offset */
    block = page / bmt_ctx.pages_per_block;
    page_offset = page % bmt_ctx.pages_per_block;

    /* Translate block */
    physical_block = bmt_translate_block(block);
    if (physical_block < 0)
        return -1;

    /* Reconstruct page number */
    return (physical_block * bmt_ctx.pages_per_block) + page_offset;
}

/*
 * Get BMT context
 */
const struct bmt_context *bmt_get_context(void)
{
    return &bmt_ctx;
}
