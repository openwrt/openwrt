// SPDX-License-Identifier: GPL-2.0-only
/*
 * NAND boot support for EcoNet EN751221 SoC
 *
 * Implements dual-image boot from SPI-NAND flash with BMT translation.
 * Selects OS1 or OS2 partition based on boot flag and loads kernel.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#include "nand_boot.h"
#include "spi_nand.h"
#include "bmt.h"
#include "printf.h"
#include <string.h>

/* External symbols from loader.c */
extern unsigned long kernel_la;
extern unsigned char *lzma_data;
extern unsigned long lzma_datasize;
extern void halt(void);

/* Kernel data buffer - will hold compressed kernel read from NAND */
static unsigned char kernel_buffer[4 * 1024 * 1024];  /* 4MB buffer for compressed kernel */

/* Page buffer for NAND reads - sized for maximum supported flash geometry */
static uint8_t page_buf[SPINAND_MAX_PAGE_SIZE];

/* Kernel command line strings (from config, in .rodata section) */
static const char cmdline_os1[] = CONFIG_NAND_CMDLINE_OS1;
static const char cmdline_os2[] = CONFIG_NAND_CMDLINE_OS2;

/* Kernel argv arrays (static const, like CONFIG_KERNEL_CMDLINE) */
static const char *const argv_os1[] = {
	NULL,
	cmdline_os1,
	NULL,
};

static const char *const argv_os2[] = {
	NULL,
	cmdline_os2,
	NULL,
};

/* Selected boot partition (0=OS1, 1=OS2) */
static int boot_partition = 0;

/*
 * Parse hex string to byte array
 * Hex string format: "30" or "0000000101000002" (pairs of hex digits)
 * Returns number of bytes parsed
 */
static int hex_to_bytes(const char *hex_str, uint8_t *bytes, int max_bytes)
{
    int len = 0;
    int i;

    if (hex_str == NULL)
        return 0;

    for (i = 0; hex_str[i] && hex_str[i+1] && len < max_bytes; i += 2, len++) {
        unsigned int byte;
        char hex_pair[3] = { hex_str[i], hex_str[i+1], '\0' };

        /* Simple hex parser */
        byte = 0;
        if (hex_pair[0] >= '0' && hex_pair[0] <= '9')
            byte = (hex_pair[0] - '0') << 4;
        else if (hex_pair[0] >= 'a' && hex_pair[0] <= 'f')
            byte = (hex_pair[0] - 'a' + 10) << 4;
        else if (hex_pair[0] >= 'A' && hex_pair[0] <= 'F')
            byte = (hex_pair[0] - 'A' + 10) << 4;

        if (hex_pair[1] >= '0' && hex_pair[1] <= '9')
            byte |= (hex_pair[1] - '0');
        else if (hex_pair[1] >= 'a' && hex_pair[1] <= 'f')
            byte |= (hex_pair[1] - 'a' + 10);
        else if (hex_pair[1] >= 'A' && hex_pair[1] <= 'F')
            byte |= (hex_pair[1] - 'A' + 10);

        bytes[len] = byte;
    }

    return len;
}

/*
 * Apply mask to byte array
 * Mask uses 'X' for bits to keep, '0' for bits to ignore
 * Example: mask "0X" keeps second nibble of each byte
 */
static void apply_mask(uint8_t *data, const char *mask, int len)
{
    int i, j;

    if (mask == NULL)
        return;

    for (i = 0; i < len && mask[i*2] && mask[i*2+1]; i++) {
        uint8_t byte_mask = 0;

        /* Check each nibble of the mask */
        for (j = 0; j < 2; j++) {
            char c = mask[i*2 + j];
            if (c == 'X' || c == 'x') {
                byte_mask |= (0xF << ((1-j) * 4));
            }
            /* '0' or any other char means mask out that nibble */
        }

        data[i] &= byte_mask;
    }
}

/*
 * Read boot flag from NAND flash
 * Returns: 0 for OS1 partition, 1 for OS2 partition
 */
static int get_boot_flag(void)
{
    const struct spinand_info *info = spi_nand_get_info();
    uint32_t page_size = info->page_size;
    uint32_t page = CONFIG_NAND_BOOT_FLAG_OFFSET / page_size;
    uint32_t offset = CONFIG_NAND_BOOT_FLAG_OFFSET % page_size;
    uint8_t flag_bytes[32];  /* Max 32 bytes for boot flag */
    uint8_t os1_code[32];
    uint8_t os2_code[32];
    int flag_len, os1_len, os2_len;
    int i;

    /* Read page containing boot flag */
    if (spi_nand_read_page(page, page_buf) < 0) {
        printf("ERROR: Failed to read boot flag page\n");
        return 0;  /* Default to OS1 partition */
    }

    /* Parse expected boot flag codes */
    os1_len = hex_to_bytes(CONFIG_NAND_BOOT_FLAG_OS1, os1_code, sizeof(os1_code));
    os2_len = hex_to_bytes(CONFIG_NAND_BOOT_FLAG_OS2, os2_code, sizeof(os2_code));

    if (os1_len == 0 || os2_len == 0 || os1_len != os2_len) {
        printf("ERROR: Invalid boot flag configuration\n");
        return 0;
    }

    flag_len = os1_len;

    /* Extract boot flag from page */
    for (i = 0; i < flag_len; i++) {
        flag_bytes[i] = page_buf[offset + i];
    }

    /* Apply mask if configured */
    if (CONFIG_NAND_BOOT_FLAG_MASK != NULL) {
        apply_mask(flag_bytes, CONFIG_NAND_BOOT_FLAG_MASK, flag_len);
        apply_mask(os1_code, CONFIG_NAND_BOOT_FLAG_MASK, flag_len);
        apply_mask(os2_code, CONFIG_NAND_BOOT_FLAG_MASK, flag_len);
    }

    /* Compare against OS1 code */
    if (memcmp(flag_bytes, os1_code, flag_len) == 0) {
        printf("Boot flag: OS1 (");
        for (i = 0; i < flag_len; i++)
            printf("%02x", page_buf[offset + i]);
        printf(")\n");
        return 0;  /* OS1 partition */
    }

    /* Compare against OS2 code */
    if (memcmp(flag_bytes, os2_code, flag_len) == 0) {
        printf("Boot flag: OS2 (");
        for (i = 0; i < flag_len; i++)
            printf("%02x", page_buf[offset + i]);
        printf(")\n");
        return 1;  /* OS2 partition */
    }

    /* Unknown flag value */
    printf("WARNING: Unknown boot flag (");
    for (i = 0; i < flag_len; i++)
        printf("%02x", page_buf[offset + i]);
    printf("), defaulting to OS1\n");
    return 0;  /* Default to OS1 partition */
}

/*
 * Select partition offset based on boot flag
 * Returns: Byte offset of selected partition
 */
static uint32_t select_partition(void)
{
    boot_partition = get_boot_flag();

    if (boot_partition == 0) {
        printf("Boot flag: 0 (OS1 partition)\n");
        printf("Kernel cmdline: %s\n", CONFIG_NAND_CMDLINE_OS1);
        return CONFIG_NAND_OS1_OFFSET;
    } else {
        printf("Boot flag: 1 (OS2 partition)\n");
        printf("Kernel cmdline: %s\n", CONFIG_NAND_CMDLINE_OS2);
        return CONFIG_NAND_OS2_OFFSET;
    }
}

/*
 * Get kernel argv based on selected boot partition
 * Returns pointer to static const argv array
 */
const char **nand_get_kernel_argv(void)
{
    if (boot_partition == 0) {
        return (const char **)argv_os1;
    } else {
        return (const char **)argv_os2;
    }
}

/*
 * Read data from NAND with BMT translation
 *
 * Reads len bytes from NAND starting at byte offset, applying BMT
 * translation for bad block handling. Reads are done page-by-page.
 *
 * Returns: Number of bytes read, or -1 on error
 */
static int nand_read_with_bmt(uint32_t offset, uint8_t *buf, uint32_t len)
{
    const struct spinand_info *info = spi_nand_get_info();
    uint32_t page_size = info->page_size;
    uint32_t bytes_read = 0;
    uint32_t page, page_offset;
    uint32_t copy_len;
    int physical_page;

    while (bytes_read < len) {
        /* Calculate page and offset within page */
        page = offset / page_size;
        page_offset = offset % page_size;

        /* Translate page through BMT */
        physical_page = bmt_translate_page(page);
        if (physical_page < 0) {
            printf("NAND: BMT translation failed for page %d\n", page);
            return -1;
        }

        /* Read page from NAND */
        if (spi_nand_read_page(physical_page, page_buf) < 0) {
            printf("NAND: Read failed for page %d (physical %d)\n",
                   page, physical_page);
            return -1;
        }

        /* Copy relevant portion to output buffer */
        copy_len = page_size - page_offset;
        if (copy_len > len - bytes_read)
            copy_len = len - bytes_read;

        memcpy(buf + bytes_read, page_buf + page_offset, copy_len);

        bytes_read += copy_len;
        offset += copy_len;
    }

    return bytes_read;
}

/*
 * Read and validate TRX header from partition
 *
 * Returns: 0 on success, -1 on error
 */
static int nand_read_trx_header(uint32_t partition_offset, struct trx_header *hdr)
{
    /* Read TRX header */
    if (nand_read_with_bmt(partition_offset, (uint8_t *)hdr, sizeof(*hdr)) < 0) {
        printf("NAND: Failed to read TRX header\n");
        return -1;
    }

    /* Validate magic */
    if (hdr->magic != TRX_MAGIC) {
        printf("NAND: Invalid TRX magic: 0x%08x (expected 0x%08x)\n",
               hdr->magic, TRX_MAGIC);
        return -1;
    }

    /* Validate header length */
    if (hdr->header_len != TRX_HEADER_SIZE) {
        printf("NAND: Invalid TRX header length: %d (expected %d)\n",
               hdr->header_len, TRX_HEADER_SIZE);
        return -1;
    }

    printf("TRX Header:\n");
    printf("  Magic: 0x%08x\n", hdr->magic);
    printf("  Total length: %d bytes\n", hdr->total_len);
    printf("  Kernel length: %d bytes\n", hdr->kernel_len);
    printf("  Rootfs length: %d bytes\n", hdr->rootfs_len);
    printf("  Load address: 0x%08x\n", hdr->load_addr);
    printf("  Model: %.32s\n", hdr->model);
    printf("  Version: %.32s\n", hdr->version);

    return 0;
}

/*
 * Read compressed kernel data from NAND
 *
 * Returns: Number of bytes read, or -1 on error
 */
static int nand_read_kernel(uint32_t kernel_offset, uint32_t kernel_len)
{
    printf("Reading compressed kernel from NAND...\n");
    printf("  Offset: 0x%08x\n", kernel_offset);
    printf("  Length: %d bytes\n", kernel_len);

    /* Sanity check kernel size */
    if (kernel_len == 0 || kernel_len > sizeof(kernel_buffer)) {
        printf("NAND: Invalid kernel length: %d\n", kernel_len);
        return -1;
    }

    /* Read kernel data */
    if (nand_read_with_bmt(kernel_offset, kernel_buffer, kernel_len) < 0) {
        printf("NAND: Failed to read kernel data\n");
        return -1;
    }

    printf("Kernel data loaded to buffer at 0x%08x\n", (uint32_t)kernel_buffer);

    return kernel_len;
}

/*
 * Main NAND boot initialization
 *
 * This replaces lzma_init_data() for NAND-based boot.
 * Sets up kernel_la, lzma_data, and lzma_datasize for LZMA decompression.
 */
void nand_init_data(void)
{
    const struct spinand_info *flash_info;
    struct trx_header hdr;
    uint32_t partition_offset;
    uint32_t kernel_offset;

    printf("\n=== NAND Boot Initialization ===\n");

    /* Initialize SPI-NAND controller and flash */
    printf("\nInitializing SPI-NAND flash...\n");
    if (spi_nand_init() < 0) {
        printf("ERROR: SPI-NAND initialization failed!\n");
        halt();
    }

    flash_info = spi_nand_get_info();
    printf("Flash: %d blocks, %d KB per block\n",
           flash_info->total_blocks,
           (flash_info->page_size * flash_info->pages_per_block) / 1024);

    /* Initialize BMT (Bad Block Management) */
    printf("\nInitializing BMT...\n");
    if (bmt_init(flash_info->total_blocks,
                 flash_info->pages_per_block,
                 flash_info->page_size) < 0) {
        printf("ERROR: BMT initialization failed!\n");
        halt();
    }

    /* Select partition based on boot flag */
    printf("\nSelecting boot partition...\n");
    partition_offset = select_partition();
    printf("Partition offset: 0x%08x\n", partition_offset);

    /* Read and validate TRX header */
    printf("\nReading TRX header...\n");
    if (nand_read_trx_header(partition_offset, &hdr) < 0) {
        printf("ERROR: Failed to read TRX header!\n");
        halt();
    }

    /* Calculate kernel offset (after TRX header + NAND loader) */
    kernel_offset = partition_offset + KERNEL_OFFSET;

    /* Read compressed kernel into buffer */
    printf("\n");
    if (nand_read_kernel(kernel_offset, hdr.kernel_len) < 0) {
        printf("ERROR: Failed to read kernel!\n");
        halt();
    }

    /* Set up LZMA decompression parameters */
    /* Use fixed kernel load address 0x80020000 instead of TRX header value */
    /* to avoid overwriting NAND loader running at 0x80002000 */
    kernel_la = 0x80020000;
    lzma_data = kernel_buffer;
    lzma_datasize = hdr.kernel_len;

    printf("\n=== NAND Boot Ready ===\n");
    printf("Kernel will be decompressed to 0x%08x\n", kernel_la);
    printf("Compressed size: %d bytes\n", lzma_datasize);
    printf("\n");
}
