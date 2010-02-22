/*
 * arch/ubicom32/include/asm/uart_tio.h
 *   Ubicom32 architecture UART TIO definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */
#ifndef _ASM_UBICOM32_UART_TIO_H
#define _ASM_UBICOM32_UART_TIO_H

#include <asm/devtree.h>

#define UARTTIO_RX_FIFO_SIZE		16
#define UARTTIO_TX_FIFO_SIZE		16

/*
 * Interrupt flags
 */
#define UARTTIO_UART_INT_RX		0x00000001	// set when a character has been recevied (TODO: add watermark)
#define UARTTIO_UART_INT_RXOVF		0x00000002	// set when the receive buffer has overflowed
#define UARTTIO_UART_INT_RXFRAME	0x00000004	// set when there has been a framing error

#define UARTTIO_UART_INT_TX		0x00000100	// set every time a character is transmitted
#define UARTTIO_UART_INT_TXBE		0x00000200	// set when the transmit buffer is empty (TODO: add watermark)

#define UARTTIO_UART_FLAG_ENABLED	0x80000000
#define UARTTIO_UART_FLAG_SET_RATE      0x00000001      // set to update baud rate
#define UARTTIO_UART_FLAG_RESET         0x00000002      // set to reset the port
struct uarttio_uart {
	volatile u32_t			flags;

	volatile u32_t			baud_rate;
	volatile u32_t			current_baud_rate;
	u32_t				bit_time;

	/*
	 * Modem status register
	 */
	volatile u32_t			status;

	/*
	 * Interrupt registers
	 */
	volatile u32_t			int_mask;
	volatile u32_t			int_flags;

	/*
	 * Ports and pins
	 */
	u32_t				rx_port;
	u32_t				tx_port;

	u8_t				rx_pin;
	u8_t				tx_pin;

	/*
	 * Configuration Data
	 */
	u8_t				rx_bits;
	u8_t				rx_stop_bits;
	u8_t				tx_bits;
	u8_t				tx_stop_bits;

	/*
	 * RX state machine data
	 */
	u32_t				rx_timer;
	u32_t				rx_bit_pos;
	u32_t				rx_byte;
	u32_t				rx_fifo_head;
	u32_t				rx_fifo_tail;
	u32_t				rx_fifo_size;

	/*
	 * TX state machine data
	 */
	u32_t				tx_timer;
	u32_t				tx_bit_pos;
	u32_t				tx_byte;
	u32_t				tx_fifo_head;
	u32_t				tx_fifo_tail;
	u32_t				tx_fifo_size;

	/*
	 * FIFOs
	 */
	u8_t				rx_fifo[UARTTIO_RX_FIFO_SIZE];
	u8_t				tx_fifo[UARTTIO_TX_FIFO_SIZE];
};

#define UARTTIO_VP_VERSION		1
struct uarttio_regs {
	u32_t				version;

	u32_t				thread;

	u32_t				max_uarts;

	struct uarttio_uart		uarts[0];
};

#define UARTTIO_NODE_VERSION		1
struct uarttio_node {
	struct devtree_node		dn;

	u32_t				version;
	struct uarttio_regs		*regs;
	u32_t				regs_sz;
};

#endif /* _ASM_UBICOM32_UART_TIO_H */
