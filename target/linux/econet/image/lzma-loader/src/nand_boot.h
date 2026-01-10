// SPDX-License-Identifier: GPL-2.0-only
/*
 * NAND boot support for EcoNet EN751221 SoC
 *
 * Defines partition layout, TRX header structure, and boot parameters
 * for dual-image (OS1/OS2) boot support.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#ifndef _NAND_BOOT_H_
#define _NAND_BOOT_H_

#include <stdint.h>

/*
 * Partition Layout
 *
 * These offsets are device-specific and can be overridden via Makefile:
 *   NAND_OS1_OFFSET        - OS1 firmware partition offset
 *   NAND_OS2_OFFSET        - OS2 firmware partition offset
 *   NAND_BOOT_FLAG_OFFSET  - Boot flag absolute address
 *   NAND_CMDLINE_OS1       - Kernel cmdline for OS1 partition
 *   NAND_CMDLINE_OS2       - Kernel cmdline for OS2 partition
 *
 * Default layout (Genexis Platinum 4410):
 * - bootloader @ 0x000000 (256KB)   - Stock bootloader
 * - romfile @ 0x040000 (256KB)      - Configuration
 * - tclinux (OS1) @ 0x080000 (16MB) - Primary firmware image
 * - tclinux_slave (OS2) @ 0x1080000 (16MB) - Secondary firmware image
 * - reservearea @ 0x6E40000          - BMT tables and reserve blocks
 */

/* OS1 firmware partition offset - override with CONFIG_NAND_OS1_OFFSET */
#ifndef CONFIG_NAND_OS1_OFFSET
#define CONFIG_NAND_OS1_OFFSET      0x00080000
#endif

/* OS2 firmware partition offset - override with CONFIG_NAND_OS2_OFFSET */
#ifndef CONFIG_NAND_OS2_OFFSET
#define CONFIG_NAND_OS2_OFFSET      0x01080000
#endif

/*
 * Boot Flag Location and Format
 *
 * The boot flag is stored at a fixed absolute address (device-specific).
 * Different devices use different flag formats:
 *
 * Override with:
 *   CONFIG_NAND_BOOT_FLAG_OFFSET - Absolute byte address in NAND
 *   CONFIG_NAND_BOOT_FLAG_OS1    - Hex string for OS1 code (e.g., "30")
 *   CONFIG_NAND_BOOT_FLAG_OS2    - Hex string for OS2 code (e.g., "31")
 *   CONFIG_NAND_BOOT_FLAG_MASK   - Optional mask string (use 'X' for bits to compare)
 *
 * Examples:
 *   Platinum 4410:   OS1="30", OS2="31" (1 byte, ASCII)
 *   TP-Link VR1200v: OS1="0000000101000002", OS2="0000000101010003" (8 bytes)
 *   Nokia G-240G-E:  OS1="000000000000000000000001000000010000000000000000",
 *                    OS2="000000000000000100000001000000010000000000000000" (24 bytes)
 *                    MASK="000000000000000X00000000000000000000000000000000"
 *   SmartFiber:      OS1="30000000", OS2="31000000" (4 bytes, little-endian)
 *   Zyxel PMG5617GA: OS1="30", OS2="31" (1 byte at offset 4095)
 *
 * Default: 0x6FC0000 (reservearea + 12 blocks * 128KB on Genexis)
 */
#ifndef CONFIG_NAND_BOOT_FLAG_OFFSET
#define CONFIG_NAND_BOOT_FLAG_OFFSET  0x6FC0000
#endif

/* Default boot flag codes (Platinum 4410 format: single ASCII byte) */
#ifndef CONFIG_NAND_BOOT_FLAG_OS1
#define CONFIG_NAND_BOOT_FLAG_OS1     "30"
#endif

#ifndef CONFIG_NAND_BOOT_FLAG_OS2
#define CONFIG_NAND_BOOT_FLAG_OS2     "31"
#endif

/* Optional mask - use 'X' to mark bits to compare, '0' to ignore */
#ifndef CONFIG_NAND_BOOT_FLAG_MASK
#define CONFIG_NAND_BOOT_FLAG_MASK    NULL
#endif

/*
 * Kernel Command Line
 *
 * Root device varies by partition layout. Override with:
 *   CONFIG_NAND_CMDLINE_OS1  - cmdline when booting from OS1
 *   CONFIG_NAND_CMDLINE_OS2  - cmdline when booting from OS2
 */
#ifndef CONFIG_NAND_CMDLINE_OS1
#define CONFIG_NAND_CMDLINE_OS1     "root=/dev/mtdblock3"
#endif

#ifndef CONFIG_NAND_CMDLINE_OS2
#define CONFIG_NAND_CMDLINE_OS2     "root=/dev/mtdblock5"
#endif

/*
 * TRX Header Structure
 *
 * EcoNet uses a custom TRX header format with "HDR2" magic.
 * The header is 256 bytes and contains kernel/rootfs sizes and load address.
 *
 * Layout (NAND boot variant):
 * - TRX Header (256 bytes at offset 0x000)
 * - LZMA compressed NAND loader (padded to 64KB at offset 0x100)
 * - LZMA compressed kernel (at offset 0x10100)
 * - Squashfs rootfs
 */

#define TRX_MAGIC                   0x32524448  /* "HDR2" little-endian */
#define TRX_HEADER_SIZE             256
#define NAND_LOADER_SIZE            (64 * 1024) /* NAND loader padded to 64KB */
#define KERNEL_OFFSET               (TRX_HEADER_SIZE + NAND_LOADER_SIZE) /* 0x10100 */

struct trx_header {
    uint32_t magic;                 /* "HDR2" (0x32524448) */
    uint32_t header_len;            /* Header length (256) */
    uint32_t total_len;             /* Total image size */
    uint32_t crc32;                 /* CRC32 of content */
    char     version[32];           /* Firmware version string */
    char     customer_ver[32];      /* Customer version string */
    uint32_t kernel_len;            /* Kernel size (compressed) */
    uint32_t rootfs_len;            /* Rootfs size */
    uint32_t romfile_len;           /* Romfile size (usually 0) */
    char     model[32];             /* Model name */
    uint32_t load_addr;             /* Kernel load address (0x80020000) */
    char     reserved[128];         /* Reserved space */
} __attribute__((packed));

/*
 * Memory Layout
 *
 * The loader operates with the following memory layout:
 *
 * 0x80000000 - 0x8001FFFF: Loader code + stack (~128KB)
 * 0x80020000 - .........: Kernel decompression target (from TRX load_addr)
 * Workspace: After loader code, used for LZMA decoder state
 *
 * The loader reads compressed kernel data from NAND through BMT translation,
 * decompresses it to the load address specified in TRX header, and jumps to it.
 */

/*
 * Function prototypes
 */

/**
 * nand_init_data - Initialize NAND boot data
 *
 * Replacement for lzma_init_data() in NAND boot mode.
 * This function:
 * 1. Initializes SPI-NAND controller and BMT
 * 2. Reads boot flag to select OS1 or OS2 partition
 * 3. Reads and validates TRX header from selected partition
 * 4. Sets up LZMA decompression parameters (kernel_la, lzma_data, lzma_datasize)
 * 5. Sets up kernel command line with correct root device
 *
 * After this function returns, the loader proceeds with LZMA decompression
 * as usual.
 */
void nand_init_data(void);

/**
 * Get kernel command line arguments for NAND boot
 *
 * Returns pointer to argv array based on boot flag:
 * - OS1 partition: argv with root=/dev/mtdblock3
 * - OS2 partition: argv with root=/dev/mtdblock5
 *
 * Format matches CONFIG_KERNEL_CMDLINE: argc=2, argv={NULL, cmdline, NULL}
 */
const char **nand_get_kernel_argv(void);

#endif /* _NAND_BOOT_H_ */
