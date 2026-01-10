// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI Controller driver for EcoNet EN751221 SoC
 *
 * Minimal implementation for lzma-loader SPI-NAND boot support.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#include "spi_controller.h"

/*
 * Internal helper: Set operation FIFO command
 *
 * Queues an operation to the SPI controller's command FIFO.
 * The controller processes these commands to generate SPI bus activity.
 *
 * @op_cmd: Operation code (SPI_OP_*)
 * @op_len: Number of bytes/cycles for this operation (1-511)
 */
static void spi_set_opfifo(uint8_t op_cmd, uint32_t op_len)
{
    uint32_t cmd;

    /* Build command word: (opcode << 9) | length */
    cmd = ((op_cmd & SPI_OP_CMD_MASK) << SPI_OP_CMD_SHIFT) |
          (op_len & SPI_OP_LEN_MASK);

    /* Write command to OPFIFO data register */
    writel(cmd, SPI_REG_OPFIFO_WDATA);

    /* Wait until OPFIFO has room */
    while (readl(SPI_REG_OPFIFO_FULL))
        ;

    /* Trigger write from WDATA register to OPFIFO */
    writel(1, SPI_REG_OPFIFO_WR);

    /* Wait until OPFIFO is empty (command processed) */
    while (!readl(SPI_REG_OPFIFO_EMPTY))
        ;
}

/*
 * Internal helper: Write bytes to data FIFO
 *
 * Queues data bytes to the SPI controller's data FIFO for transmission.
 * Must be called after setting up the corresponding output operation.
 *
 * @buf: Data buffer to write
 * @len: Number of bytes to write
 */
static void spi_write_dfifo(const uint8_t *buf, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        /* Wait until DFIFO has room */
        while (readl(SPI_REG_DFIFO_FULL))
            ;

        /* Write data byte */
        writel(buf[i] & SPI_DFIFO_MASK, SPI_REG_DFIFO_WDATA);

        /* Wait for byte to be accepted */
        while (readl(SPI_REG_DFIFO_FULL))
            ;
    }
}

/*
 * Internal helper: Read bytes from data FIFO
 *
 * Reads data bytes from the SPI controller's data FIFO.
 * Must be called after setting up the corresponding input operation.
 *
 * @buf: Buffer to store read data
 * @len: Number of bytes to read
 */
static void spi_read_dfifo(uint8_t *buf, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        /* Wait until DFIFO has data */
        while (readl(SPI_REG_DFIFO_EMPTY))
            ;

        /* Read data byte */
        buf[i] = readl(SPI_REG_DFIFO_RDATA) & SPI_DFIFO_MASK;

        /* Advance to next byte */
        writel(1, SPI_REG_DFIFO_RD);
    }
}

/*
 * Initialize SPI controller in manual mode
 */
void spi_controller_init(void)
{
    /* Disable read idle state (stops auto-read mode) */
    writel(SPI_VAL_READ_IDLE_DISABLE, SPI_REG_READ_IDLE_EN);

    /* Wait for read controller FSM to reach idle state */
    while (readl(SPI_REG_RDCTL_FSM))
        ;

    /* Switch from auto mode to manual mode */
    writel(SPI_VAL_MTX_MODE_MANUAL, SPI_REG_MTX_MODE_TOG);

    /* Enable manual mode */
    writel(SPI_VAL_MANUAL_ENABLE, SPI_REG_MANUAL_EN);
}

/*
 * Assert chip select (active low)
 *
 * Note: CSL is called twice as a timing workaround for the hardware.
 */
void spi_cs_low(void)
{
    spi_set_opfifo(SPI_OP_CSL, 1);
    spi_set_opfifo(SPI_OP_CSL, 1);
}

/*
 * Deassert chip select
 *
 * Note: 5 clock cycles are added after CSH for timing margin.
 */
void spi_cs_high(void)
{
    spi_set_opfifo(SPI_OP_CSH, 1);
    spi_set_opfifo(SPI_OP_CK, 5);
}

/*
 * Write a single byte to SPI bus
 */
void spi_write_byte(uint8_t data)
{
    spi_set_opfifo(SPI_OP_OUTS, 1);
    spi_write_dfifo(&data, 1);
}

/*
 * Write multiple bytes to SPI bus
 *
 * Handles chunking for transfers larger than the hardware
 * maximum of 511 bytes per operation.
 */
void spi_write_nbyte(const uint8_t *buf, uint32_t len)
{
    uint32_t chunk_len;
    uint32_t offset = 0;

    while (len > 0) {
        /* Limit to maximum transfer size */
        chunk_len = (len > SPI_OP_LEN_MAX) ? SPI_OP_LEN_MAX : len;

        /* Set up output operation */
        spi_set_opfifo(SPI_OP_OUTS, chunk_len);

        /* Write data to DFIFO */
        spi_write_dfifo(buf + offset, chunk_len);

        offset += chunk_len;
        len -= chunk_len;
    }
}

/*
 * Read multiple bytes from SPI bus
 *
 * Handles chunking for transfers larger than the hardware
 * maximum of 511 bytes per operation.
 */
void spi_read_nbyte(uint8_t *buf, uint32_t len)
{
    uint32_t chunk_len;
    uint32_t offset = 0;

    while (len > 0) {
        /* Limit to maximum transfer size */
        chunk_len = (len > SPI_OP_LEN_MAX) ? SPI_OP_LEN_MAX : len;

        /* Set up input operation */
        spi_set_opfifo(SPI_OP_INS, chunk_len);

        /* Read data from DFIFO */
        spi_read_dfifo(buf + offset, chunk_len);

        offset += chunk_len;
        len -= chunk_len;
    }
}
