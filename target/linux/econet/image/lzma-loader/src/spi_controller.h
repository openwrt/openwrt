// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI Controller driver for EcoNet EN751221 SoC
 *
 * Register definitions and function prototypes for SPI controller access.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#ifndef _SPI_CONTROLLER_H_
#define _SPI_CONTROLLER_H_

#include <stdint.h>

/*
 * Memory-mapped I/O accessors
 */
#define readl(addr)         (*(volatile uint32_t *)(addr))
#define writel(val, addr)   (*(volatile uint32_t *)(addr) = (val))

/*
 * SPI Controller Register Definitions
 * Base address: 0xBFA10000 (KSEG1 mapped)
 */
#define SPI_CONTROLLER_BASE             0xBFA10000

/* Control registers */
#define SPI_REG_READ_MODE               (SPI_CONTROLLER_BASE + 0x0000)
#define SPI_REG_READ_IDLE_EN            (SPI_CONTROLLER_BASE + 0x0004)
#define SPI_REG_SIDLY                   (SPI_CONTROLLER_BASE + 0x0008)
#define SPI_REG_CSHEXT                  (SPI_CONTROLLER_BASE + 0x000C)
#define SPI_REG_CSLEXT                  (SPI_CONTROLLER_BASE + 0x0010)
#define SPI_REG_MTX_MODE_TOG            (SPI_CONTROLLER_BASE + 0x0014)
#define SPI_REG_RDCTL_FSM               (SPI_CONTROLLER_BASE + 0x0018)
#define SPI_REG_MACMUX_SEL              (SPI_CONTROLLER_BASE + 0x001C)
#define SPI_REG_MANUAL_EN               (SPI_CONTROLLER_BASE + 0x0020)

/* Operation FIFO registers */
#define SPI_REG_OPFIFO_EMPTY            (SPI_CONTROLLER_BASE + 0x0024)
#define SPI_REG_OPFIFO_WDATA            (SPI_CONTROLLER_BASE + 0x0028)
#define SPI_REG_OPFIFO_FULL             (SPI_CONTROLLER_BASE + 0x002C)
#define SPI_REG_OPFIFO_WR               (SPI_CONTROLLER_BASE + 0x0030)

/* Data FIFO registers */
#define SPI_REG_DFIFO_FULL              (SPI_CONTROLLER_BASE + 0x0034)
#define SPI_REG_DFIFO_WDATA             (SPI_CONTROLLER_BASE + 0x0038)
#define SPI_REG_DFIFO_EMPTY             (SPI_CONTROLLER_BASE + 0x003C)
#define SPI_REG_DFIFO_RD                (SPI_CONTROLLER_BASE + 0x0040)
#define SPI_REG_DFIFO_RDATA             (SPI_CONTROLLER_BASE + 0x0044)

/* Misc registers */
#define SPI_REG_DUMMY                   (SPI_CONTROLLER_BASE + 0x0080)
#define SPI_REG_PROBE_SEL               (SPI_CONTROLLER_BASE + 0x0088)
#define SPI_REG_INTERRUPT               (SPI_CONTROLLER_BASE + 0x0090)
#define SPI_REG_INTERRUPT_EN            (SPI_CONTROLLER_BASE + 0x0094)
#define SPI_REG_SI_CK_SEL               (SPI_CONTROLLER_BASE + 0x009C)

/*
 * Operation codes for OPFIFO
 * Format: (opcode << 9) | length
 */
#define SPI_OP_CSH                      0x00    /* Chip select high */
#define SPI_OP_CSL                      0x01    /* Chip select low */
#define SPI_OP_CK                       0x02    /* Clock cycles */
#define SPI_OP_OUTS                     0x08    /* Output single */
#define SPI_OP_OUTD                     0x09    /* Output dual */
#define SPI_OP_OUTQ                     0x0A    /* Output quad */
#define SPI_OP_INS                      0x0C    /* Input single */
#define SPI_OP_INS0                     0x0D    /* Input single (variant) */
#define SPI_OP_IND                      0x0E    /* Input dual */
#define SPI_OP_INQ                      0x0F    /* Input quad */
#define SPI_OP_OS2IS                    0x10    /* Output single, switch to input single */
#define SPI_OP_OS2ID                    0x11    /* Output single, switch to input dual */
#define SPI_OP_OS2IQ                    0x12    /* Output single, switch to input quad */
#define SPI_OP_OD2IS                    0x13    /* Output dual, switch to input single */
#define SPI_OP_OD2ID                    0x14    /* Output dual, switch to input dual */
#define SPI_OP_OD2IQ                    0x15    /* Output dual, switch to input quad */
#define SPI_OP_OQ2IS                    0x16    /* Output quad, switch to input single */
#define SPI_OP_OQ2ID                    0x17    /* Output quad, switch to input dual */
#define SPI_OP_OQ2IQ                    0x18    /* Output quad, switch to input quad */
#define SPI_OP_OSNIS                    0x19    /* Output single, no input switch */
#define SPI_OP_ODNID                    0x1A    /* Output dual, no input dual */

/* Operation code field masks and shifts */
#define SPI_OP_CMD_MASK                 0x1F
#define SPI_OP_CMD_SHIFT                9
#define SPI_OP_LEN_MASK                 0x1FF
#define SPI_OP_LEN_MAX                  0x1FF   /* Max 511 bytes per operation */

/* Data FIFO mask */
#define SPI_DFIFO_MASK                  0xFF

/* Manual mode values */
#define SPI_VAL_READ_IDLE_DISABLE       0x00
#define SPI_VAL_MTX_MODE_MANUAL         0x09
#define SPI_VAL_MANUAL_ENABLE           0x01

/*
 * Function prototypes
 */

/**
 * spi_controller_init - Initialize SPI controller in manual mode
 *
 * Disables auto-read mode and enables manual mode for direct
 * SPI bus access. Must be called before any SPI operations.
 */
void spi_controller_init(void);

/**
 * spi_cs_low - Assert chip select (active low)
 *
 * Pulls the SPI chip select line low to begin a transaction.
 */
void spi_cs_low(void);

/**
 * spi_cs_high - Deassert chip select
 *
 * Pulls the SPI chip select line high to end a transaction.
 * Also generates additional clock cycles for timing.
 */
void spi_cs_high(void);

/**
 * spi_write_byte - Write a single byte to SPI bus
 * @data: Byte to write
 *
 * Writes one byte using single-speed SPI.
 */
void spi_write_byte(uint8_t data);

/**
 * spi_write_nbyte - Write multiple bytes to SPI bus
 * @buf: Buffer containing data to write
 * @len: Number of bytes to write
 *
 * Writes up to len bytes using single-speed SPI.
 * Handles chunking for transfers larger than 511 bytes.
 */
void spi_write_nbyte(const uint8_t *buf, uint32_t len);

/**
 * spi_read_nbyte - Read multiple bytes from SPI bus
 * @buf: Buffer to store read data
 * @len: Number of bytes to read
 *
 * Reads len bytes using single-speed SPI.
 * Handles chunking for transfers larger than 511 bytes.
 */
void spi_read_nbyte(uint8_t *buf, uint32_t len);

#endif /* _SPI_CONTROLLER_H_ */
