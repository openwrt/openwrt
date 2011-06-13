/*
 *  linux/drivers/serial/hornet_serial.c
 *
 *  Driver for hornet serial ports
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 *  Copyright (C) 2010 Ryan Hsu.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  $Id$
 *
 * A note about mapbase / membase
 *
 *  mapbase is the physical address of the IO port.
 *  membase is an 'ioremapped' cookie.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_reg.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/nmi.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <asm/mach-ar71xx/ar933x_uart.h>
#include <asm/mach-ar71xx/ar933x_uart_platform.h>

#include <asm/io.h>
#include <asm/irq.h>

#include "8250.h"
#define ar7240_reg_rmw_clear(_reg, _val)	do {} while (0)

#define DRIVER_NAME "ar933x-uart"

#define AR933X_UART_REGS_SIZE	20
#define AR933X_UART_FIFO_SIZE	16

/*
 * uncomment below to enable WAR for EV81847.
 */
//#define AR933X_EV81847_WAR

static struct uart_driver ar933x_uart_driver;

/*
 * Debugging.
 */
#if 0
#define DEBUG_AUTOCONF(fmt...)	printk(fmt)
#else
#define DEBUG_AUTOCONF(fmt...)	do { } while (0)
#endif

#if 0
#define DEBUG_INTR(fmt...)	printk(fmt)
#else
#define DEBUG_INTR(fmt...)	do { } while (0)
#endif

/*
 * We default to IRQ0 for the "no irq" hack.   Some
 * machine types want others as well - they're free
 * to redefine this in their header file.
 */
#define is_real_interrupt(irq)	((irq) != 0)

#include <asm/serial.h>

struct ar933x_uart_port {
	struct uart_port	port;
	struct timer_list	timer;		/* "no irq" timer */
	unsigned char		acr;
	unsigned char		ier;
	unsigned char		lcr;
	unsigned char		mcr;
};

static inline int ar933x_ev81847_war(void)
{
#if defined(AR933X_EV81847_WAR)
	return 1;
#else
	return 0;
#endif
}

static inline unsigned int ar933x_uart_read(struct ar933x_uart_port *up,
					    int offset)
{
	return readl(up->port.membase + offset);
}

static inline void ar933x_uart_write(struct ar933x_uart_port *up,
				     int offset, unsigned int value)
{
	writel(value, up->port.membase + offset);
}

static inline void ar933x_uart_rmw(struct ar933x_uart_port *up,
				  unsigned int offset,
				  unsigned int mask,
				  unsigned int val)
{
	unsigned int t;

	t = ar933x_uart_read(up, offset);
	t &= ~mask;
	t |= val;
	ar933x_uart_write(up, offset, t);
}

static inline void ar933x_uart_rmw_set(struct ar933x_uart_port *up,
				       unsigned int offset,
				       unsigned int val)
{
	ar933x_uart_rmw(up, offset, 0, val);
}

static inline void ar933x_uart_rmw_clear(struct ar933x_uart_port *up,
					 unsigned int offset,
					 unsigned int val)
{
	ar933x_uart_rmw(up, offset, val, 0);
}

static inline void ar933x_uart_start_tx_interrupt(struct ar933x_uart_port *up)
{
	ar933x_uart_rmw_set(up, AR933X_UART_INT_EN_REG,
			    AR933X_UART_INT_TX_EMPTY);
}

static inline void ar933x_uart_stop_tx_interrupt(struct ar933x_uart_port *up)
{
	if (up->ier & UART_IER_THRI) {
		up->ier &= ~UART_IER_THRI;

		/* FIXME: why this uses RXVALIDINTEN? */
		ar933x_uart_rmw_clear(up, AR933X_UART_INT_EN_REG,
				      AR933X_UART_INT_RX_VALID);
	}
}

static unsigned int ar933x_uart_tx_empty(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;
	unsigned int rdata;

	spin_lock_irqsave(&up->port.lock, flags);
	rdata = ar933x_uart_read(up, AR933X_UART_DATA_REG);
	spin_unlock_irqrestore(&up->port.lock, flags);

	return (rdata & AR933X_UART_DATA_TX_CSR) ? 0 : TIOCSER_TEMT;
}

static unsigned int ar933x_uart_get_mctrl(struct uart_port *port)
{
	return TIOCM_CAR;
}

static void ar933x_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static void ar933x_uart_start_tx(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	ar933x_uart_start_tx_interrupt(up);
}

static void ar933x_uart_stop_tx(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	ar933x_uart_stop_tx_interrupt(up);
}

static void ar933x_uart_stop_rx(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	up->ier &= ~UART_IER_RLSI;
	up->port.read_status_mask &= ~UART_LSR_DR;

	ar933x_uart_rmw_clear(up, AR933X_UART_INT_EN_REG,
			      AR933X_UART_INT_RX_VALID);
}

static void ar933x_uart_break_ctl(struct uart_port *port, int break_state)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;
	unsigned long rdata;

	spin_lock_irqsave(&up->port.lock, flags);

	if (break_state == -1)
		up->lcr |= UART_LCR_SBC;
	else
		up->lcr &= ~UART_LCR_SBC;

	rdata = ar933x_uart_read(up, AR933X_UART_CS_REG);
	if (up->lcr & UART_LCR_SBC)
		rdata |= AR933X_UART_CS_TX_BREAK;
	else
		rdata &= ~AR933X_UART_CS_TX_BREAK;

	ar933x_uart_write(up, AR933X_UART_CS_REG, rdata);

	spin_unlock_irqrestore(&up->port.lock, flags);
}

static void ar933x_uart_enable_ms(struct uart_port *port)
{
}

static inline unsigned int ar933x_uart_get_divisor(struct uart_port *port,
						   unsigned int baud)
{
	return (port->uartclk / (16 * baud)) - 1;
}

static void ar933x_uart_set_termios(struct uart_port *port,
				    struct ktermios *termios,
				    struct ktermios *old)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned char cval;
	unsigned long flags;
	unsigned int baud, quot;

	switch (termios->c_cflag & CSIZE) {
	case CS5:
		cval = UART_LCR_WLEN5;
		break;
	case CS6:
		cval = UART_LCR_WLEN6;
		break;
	case CS7:
		cval = UART_LCR_WLEN7;
		break;
	default:
	case CS8:
		cval = UART_LCR_WLEN8;
		break;
	}

	if (termios->c_cflag & CSTOPB)
		cval |= UART_LCR_STOP;
	if (termios->c_cflag & PARENB)
		cval |= UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
		cval |= UART_LCR_EPAR;
#ifdef CMSPAR
	if (termios->c_cflag & CMSPAR)
		cval |= UART_LCR_SPAR;
#endif

	/*
	 * Ask the core to calculate the divisor for us.
	 */
	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk / 16);
	quot = ar933x_uart_get_divisor(port, baud);

#if 0
	if (up->capabilities & UART_CAP_FIFO && up->port.fifosize > 1) {
		if (baud < 2400)
			fcr = UART_FCR_ENABLE_FIFO | UART_FCR_TRIGGER_1;
		else
			fcr = uart_config[up->port.type].fcr;
	}

	/*
	 * MCR-based auto flow control.  When AFE is enabled, RTS will be
	 * deasserted when the receive FIFO contains more characters than
	 * the trigger, or the MCR RTS bit is cleared.  In the case where
	 * the remote UART is not using CTS auto flow control, we must
	 * have sufficient FIFO entries for the latency of the remote
	 * UART to respond.  IOW, at least 32 bytes of FIFO.
	 */
	if (up->capabilities & UART_CAP_AFE && up->port.fifosize >= 32) {
		up->mcr &= ~UART_MCR_AFE;
		if (termios->c_cflag & CRTSCTS)
			up->mcr |= UART_MCR_AFE;
	}
#endif

	/*
	 * Ok, we're now changing the port state.  Do it with
	 * interrupts disabled.
	 */
	spin_lock_irqsave(&up->port.lock, flags);

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	up->port.read_status_mask = UART_LSR_OE | UART_LSR_THRE | UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		up->port.read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		up->port.read_status_mask |= UART_LSR_BI;

	/*
	 * Characteres to ignore
	 */
	up->port.ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		up->port.ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		up->port.ignore_status_mask |= UART_LSR_BI;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			up->port.ignore_status_mask |= UART_LSR_OE;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0)
		up->port.ignore_status_mask |= UART_LSR_DR;

	/*
	 * CTS flow control flag and modem status interrupts
	 */
	up->ier &= ~UART_IER_MSI;
	if (UART_ENABLE_MS(&up->port, termios->c_cflag))
		up->ier |= UART_IER_MSI;

	ar933x_uart_rmw_set(up, AR933X_UART_CS_REG,
			    AR933X_UART_CS_HOST_INT_EN);

	/* Save LCR */
	up->lcr = cval;

	ar933x_uart_set_mctrl(&up->port, up->port.mctrl);
	spin_unlock_irqrestore(&up->port.lock, flags);
}

static void ar933x_uart_rx_chars(struct ar933x_uart_port *up, int *status)
{
	struct tty_struct *tty = up->port.state->port.tty;
	unsigned int lsr = *status;
	unsigned char ch;
	int max_count = 256;
	char flag;

	do {
		ch = lsr & AR933X_UART_DATA_TX_RX_MASK;

		flag = TTY_NORMAL;
		up->port.icount.rx++;

		lsr = AR933X_UART_DATA_RX_CSR;
		ar933x_uart_write(up, AR933X_UART_DATA_REG, lsr);

		if (unlikely(lsr & (UART_LSR_BI | UART_LSR_PE |
				    UART_LSR_FE | UART_LSR_OE))) {
			/*
			 * For statistics only
			 */
			if (lsr & UART_LSR_BI) {
				lsr &= ~(UART_LSR_FE | UART_LSR_PE);
				up->port.icount.brk++;
				/*
				 * We do the SysRQ and SAK checking
				 * here because otherwise the break
				 * may get masked by ignore_status_mask
				 * or read_status_mask.
				 */
				if (uart_handle_break(&up->port))
					goto ignore_char;
			} else if (lsr & UART_LSR_PE)
				up->port.icount.parity++;
			else if (lsr & UART_LSR_FE)
				up->port.icount.frame++;
			if (lsr & UART_LSR_OE)
				up->port.icount.overrun++;

			/*
			 * Mask off conditions which should be ignored.
			 */
			lsr &= up->port.read_status_mask;

			if (lsr & UART_LSR_BI) {
				DEBUG_INTR("handling break....");
				flag = TTY_BREAK;
			} else if (lsr & UART_LSR_PE)
				flag = TTY_PARITY;
			else if (lsr & UART_LSR_FE)
				flag = TTY_FRAME;
		}

		if (uart_handle_sysrq_char(&up->port, ch))
			goto ignore_char;

		uart_insert_char(&up->port, lsr, UART_LSR_OE, ch, flag);

ignore_char:
		lsr = ar933x_uart_read(up, AR933X_UART_DATA_REG);
	} while ((lsr & AR933X_UART_DATA_RX_CSR) && (max_count-- > 0));

	spin_unlock(&up->port.lock);
	tty_flip_buffer_push(tty);
	spin_lock(&up->port.lock);

	*status = lsr;
}

static void ar933x_uart_tx_chars(struct ar933x_uart_port *up)
{
	struct circ_buf *xmit = &up->port.state->xmit;
	int count;
	unsigned int rdata;

	rdata = ar933x_uart_read(up, AR933X_UART_DATA_REG);
	if ((rdata & AR933X_UART_DATA_TX_CSR) == 0) {
		ar933x_uart_start_tx_interrupt(up);
		return;
	}

	if (up->port.x_char) {
		rdata = up->port.x_char & AR933X_UART_DATA_TX_RX_MASK;
		rdata |= AR933X_UART_DATA_TX_CSR;
		ar933x_uart_write(up, AR933X_UART_DATA_REG, rdata);
		up->port.icount.tx++;
		up->port.x_char = 0;
		ar933x_uart_start_tx_interrupt(up);
		return;
	}

	if (uart_tx_stopped(&up->port)) {
		ar933x_uart_stop_tx(&up->port);
		return;
	}

	if (uart_circ_empty(xmit)) {
		ar933x_uart_stop_tx_interrupt(up);
		return;
	}

	count = up->port.fifosize / 4;
	do {
		rdata = ar933x_uart_read(up, AR933X_UART_DATA_REG);
		if ((rdata & AR933X_UART_DATA_TX_CSR) == 0) {
			ar933x_uart_start_tx_interrupt(up);
			return;
		}

		rdata = xmit->buf[xmit->tail] & AR933X_UART_DATA_TX_RX_MASK;
		rdata |= AR933X_UART_DATA_TX_CSR;
		ar933x_uart_write(up, AR933X_UART_DATA_REG, rdata);

		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		up->port.icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);

	rdata = ar933x_uart_read(up, AR933X_UART_DATA_REG);
	if ((rdata & AR933X_UART_DATA_TX_CSR) == 0) {
		ar933x_uart_start_tx_interrupt(up);
		return;
	}

	/* Re-enable TX Empty Interrupt to transmit pending chars */
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
		uart_write_wakeup(&up->port);
		ar933x_uart_start_tx_interrupt(up);
	}

	DEBUG_INTR("THRE...");

	if (uart_circ_empty(xmit))
		ar933x_uart_stop_tx_interrupt(up);
	else
		ar933x_uart_start_tx_interrupt(up);
}

/*! Hornet's interrupt status is not read clear, so that we have to...
 * a. read out the interrupt status
 * b. clear the interrupt mask to reset the interrupt status
 * c. enable the interrupt to reactivate interrupt
 *
 * Disable and clear the interrupt status
 */
static inline void ar933x_uart_clear_int(struct ar933x_uart_port *up)
{
#define BIT3 (0x1>>3)

	/* 1. clear MISC interrupt mask */
	//ar7240_reg_rmw_clear(AR7240_MISC_INT_MASK, BIT3);

	/* 2. clear uartcs hostinten mask, bit13 */
	ar933x_uart_rmw_clear(up, AR933X_UART_CS_REG,
			      AR933X_UART_CS_HOST_INT_EN);

	/* 3. clear rx uartint */
	ar933x_uart_write(up, AR933X_UART_INT_REG, AR933X_UART_INT_RX_VALID);

	/* 4. clear misc interrupt status  */
	ar7240_reg_rmw_clear(AR7240_MISC_INT_STATUS, BIT3);

	/* 5. clear rx uartinten*/
	ar933x_uart_rmw_clear(up, AR933X_UART_INT_EN_REG,
			      AR933X_UART_INT_RX_VALID);

	/* 6. enable rx int*/
	ar933x_uart_rmw_set(up, AR933X_UART_INT_EN_REG,
			    AR933X_UART_INT_RX_VALID);

	/* 7. set uartcs hostinten mask */
	ar933x_uart_rmw_set(up, AR933X_UART_CS_REG,
			    AR933X_UART_CS_HOST_INT_EN);

	/* 8. set misc int mask */
	//ar7240_reg_wr(AR7240_MISC_INT_MASK, BIT3);
}

static inline void ar933x_uart_handle_port(struct ar933x_uart_port *up)
{
	unsigned int status;
	unsigned int int_status;
	unsigned int en_status;
	unsigned long flags;

	status = ar933x_uart_read(up, AR933X_UART_DATA_REG);
	int_status = ar933x_uart_read(up, AR933X_UART_INT_REG);
	en_status = ar933x_uart_read(up, AR933X_UART_INT_EN_REG);

	spin_lock_irqsave(&up->port.lock, flags);

	if( (int_status & en_status) & AR933X_UART_INT_RX_VALID )
		ar933x_uart_rx_chars(up, &status);

	if (((int_status & en_status) & AR933X_UART_INT_TX_EMPTY)) {
		/* clear TX empty interrupts */
		ar933x_uart_write(up, AR933X_UART_INT_REG,
				  AR933X_UART_INT_TX_EMPTY);

		/* disable TX empty interrupts */
		ar933x_uart_rmw_clear(up, AR933X_UART_INT_EN_REG,
				      AR933X_UART_INT_TX_EMPTY);

		if (!uart_circ_empty(&up->port.state->xmit))
			ar933x_uart_tx_chars(up);
	}

	spin_unlock_irqrestore(&up->port.lock, flags);
}

static irqreturn_t ar933x_uart_interrupt(int irq, void *dev_id)
{
	struct ar933x_uart_port *up;
	unsigned int iir;

	up = (struct ar933x_uart_port *) dev_id;

	iir = ar933x_uart_read(up, AR933X_UART_CS_REG);
	if ((iir & AR933X_UART_CS_HOST_INT) == 0)
		return IRQ_NONE;

	DEBUG_INTR("ar933x_uart_interrupt(%d)...", irq);

	spin_lock(&up->port.lock);
	ar933x_uart_handle_port(up);
	ar933x_uart_clear_int(up);
	spin_unlock(&up->port.lock);

	DEBUG_INTR("end.\n");

	return IRQ_HANDLED;
}

static void ar933x_uart_timer(unsigned long data)
{
	struct uart_port *port = (void *)data;
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned int iir;

	if (ar933x_ev81847_war()) {
		struct circ_buf *xmit = &up->port.state->xmit;
		unsigned long flags;

		if (!uart_circ_empty(xmit)) {
			spin_lock_irqsave(&up->port.lock, flags);
			ar933x_uart_tx_chars(up);
			spin_unlock_irqrestore(&up->port.lock, flags);
		}
	} else {
		iir = ar933x_uart_read(up, AR933X_UART_CS_REG);
		if (iir & AR933X_UART_CS_HOST_INT) {
			spin_lock(&up->port.lock);
			ar933x_uart_handle_port(up);
			spin_unlock(&up->port.lock);
		}
	}

	mod_timer(&up->timer, jiffies + uart_poll_timeout(port));
}

static int ar933x_uart_startup(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;
	int ret;

	ret = request_irq(up->port.irq, ar933x_uart_interrupt,
			  up->port.irqflags, dev_name(up->port.dev), up);
	if (ret)
		return ret;

	up->mcr = 0;

	/*
	 * Clear the interrupt registers.
	 */
	ar933x_uart_read(up, AR933X_UART_CS_REG);
	ar933x_uart_read(up, AR933X_UART_INT_REG);

	if (!is_real_interrupt(up->port.irq) || ar933x_ev81847_war()) {
		setup_timer(&up->timer, ar933x_uart_timer, (unsigned long)port);
		mod_timer(&up->timer, jiffies + uart_poll_timeout(port));
		return 0;
	}

	spin_lock_irqsave(&up->port.lock, flags);

	/*
	 * Enable host interrupts
	 */
	ar933x_uart_rmw_set(up, AR933X_UART_CS_REG,
			    AR933X_UART_CS_HOST_INT_EN);

	/*
	 * Enable RX interrupts
	 */
	up->ier = UART_IER_RLSI | UART_IER_RDI;
	ar933x_uart_write(up, AR933X_UART_INT_EN_REG,
			  AR933X_UART_INT_RX_VALID);

	/*
	 * And clear the interrupt registers again for luck.
	 */
	ar933x_uart_read(up, AR933X_UART_INT_REG);

	spin_unlock_irqrestore(&up->port.lock, flags);

	return 0;
}

static void ar933x_uart_shutdown(struct uart_port *port)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned long flags;

	/*
	 * Disable all interrupts from this port
	 */
	up->ier = 0;
	ar933x_uart_write(up, AR933X_UART_INT_EN_REG, 0);

	spin_lock_irqsave(&up->port.lock, flags);
	up->port.mctrl &= ~TIOCM_OUT2;
	ar933x_uart_set_mctrl(&up->port, up->port.mctrl);
	spin_unlock_irqrestore(&up->port.lock, flags);

	/*
	 * Disable break condition
	 */
	ar933x_uart_rmw_clear(up, AR933X_UART_CS_REG,
			      AR933X_UART_CS_TX_BREAK);

	if (!is_real_interrupt(up->port.irq) ||
	    ar933x_ev81847_war())
		del_timer_sync(&up->timer);

	free_irq(up->port.irq, up);
}

static const char *ar933x_uart_type(struct uart_port *port)
{
	return (port->type == PORT_AR933X) ? "AR933X UART" : NULL;
}

static void ar933x_uart_release_port(struct uart_port *port)
{
	/* Nothing to release ... */
}

static int ar933x_uart_request_port(struct uart_port *port)
{
	/* UARTs always present */
	return 0;
}

static void ar933x_uart_config_port(struct uart_port *port, int flags)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;

	port->type = PORT_AR933X;

	/* Clear mask, so no surprise interrupts. */
	ar933x_uart_read(up, AR933X_UART_CS_REG);
	/* Clear interrupts status register */
	ar933x_uart_read(up, AR933X_UART_INT_REG);
}

static int ar933x_uart_verify_port(struct uart_port *port,
				   struct serial_struct *ser)
{
	return -EINVAL;
}

static struct uart_ops ar933x_uart_ops = {
	.tx_empty	= ar933x_uart_tx_empty,
	.set_mctrl	= ar933x_uart_set_mctrl,
	.get_mctrl	= ar933x_uart_get_mctrl,
	.stop_tx	= ar933x_uart_stop_tx,
	.start_tx	= ar933x_uart_start_tx,
	.stop_rx	= ar933x_uart_stop_rx,
	.enable_ms	= ar933x_uart_enable_ms,
	.break_ctl	= ar933x_uart_break_ctl,
	.startup	= ar933x_uart_startup,
	.shutdown	= ar933x_uart_shutdown,
	.set_termios	= ar933x_uart_set_termios,
	.type		= ar933x_uart_type,
	.release_port	= ar933x_uart_release_port,
	.request_port	= ar933x_uart_request_port,
	.config_port	= ar933x_uart_config_port,
	.verify_port	= ar933x_uart_verify_port,
};

#ifdef CONFIG_SERIAL_AR933X_CONSOLE

static struct ar933x_uart_port *ar933x_console_ports[CONFIG_SERIAL_AR933X_NR_UARTS];

static void ar933x_uart_wait_xmitr(struct ar933x_uart_port *up)
{
	unsigned int status;
	unsigned int timeout = 60000;

	/* Wait up to 60ms for the character(s) to be sent. */
	do {
		status = ar933x_uart_read(up, AR933X_UART_DATA_REG);
		if (--timeout == 0)
			break;
		udelay(1);
	} while ((status & AR933X_UART_DATA_TX_CSR) == 0);
}

static void ar933x_uart_console_putchar(struct uart_port *port, int ch)
{
	struct ar933x_uart_port *up = (struct ar933x_uart_port *) port;
	unsigned int rdata;

	ar933x_uart_wait_xmitr(up);

	rdata = ch & AR933X_UART_DATA_TX_RX_MASK;
	rdata |= AR933X_UART_DATA_TX_CSR;
	ar933x_uart_write(up, AR933X_UART_DATA_REG, rdata);
}

static void ar933x_uart_console_write(struct console *co, const char *s,
				      unsigned int count)
{
	struct ar933x_uart_port *up = ar933x_console_ports[co->index];
	unsigned long flags;
	unsigned int ier;
	int locked = 1;

	local_irq_save(flags);

	if (up->port.sysrq) {
		locked = 0;
	} else if (oops_in_progress) {
		locked = spin_trylock(&up->port.lock);
	} else
		spin_lock(&up->port.lock);

	/*
	 * First save the IER then disable the interrupts
	 */
	ier = ar933x_uart_read(up, AR933X_UART_INT_EN_REG);
	ar933x_uart_write(up, AR933X_UART_INT_EN_REG, 0);

	uart_console_write(&up->port, s, count, ar933x_uart_console_putchar);

	/*
	 * Finally, wait for transmitter to become empty
	 * and restore the IER
	 */
	ar933x_uart_wait_xmitr(up);

	ar933x_uart_write(up, AR933X_UART_INT_EN_REG, ier);
	ar933x_uart_write(up, AR933X_UART_INT_REG, AR933X_UART_INT_ALLINTS);

	if (locked)
		spin_unlock(&up->port.lock);

	local_irq_restore(flags);
}

static int ar933x_uart_console_setup(struct console *co, char *options)
{
	struct ar933x_uart_port *up;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index < 0 || co->index >= CONFIG_SERIAL_AR933X_NR_UARTS)
		return -EINVAL;

	up = ar933x_console_ports[co->index];
	if (!up)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(&up->port, co, baud, parity, bits, flow);
}

static struct console ar933x_uart_console = {
	.name		= "ttyATH",
	.write		= ar933x_uart_console_write,
	.device		= uart_console_device,
	.setup		= ar933x_uart_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &ar933x_uart_driver,
};

static int __init ar933x_uart_console_init(void)
{
	register_console(&ar933x_uart_console);
	return 0;
}
console_initcall(ar933x_uart_console_init);

static void ar933x_uart_add_console_port(struct ar933x_uart_port *up)
{
	ar933x_console_ports[up->port.line] = up;
}

#define AR933X_SERIAL_CONSOLE	&ar933x_uart_console

#else

static inline void ar933x_uart_add_console_port(struct ar933x_uart_port *up) {}

#define AR933X_SERIAL_CONSOLE	NULL

#endif /* CONFIG_SERIAL_AR933X_CONSOLE */

static struct uart_driver ar933x_uart_driver = {
	.owner		= THIS_MODULE,
	.driver_name	= DRIVER_NAME,
	.dev_name	= "ttyATH",
	.nr		= CONFIG_SERIAL_AR933X_NR_UARTS,
	.cons		= AR933X_SERIAL_CONSOLE,
};

static int __devinit ar933x_uart_probe(struct platform_device *pdev)
{
	struct ar933x_uart_platform_data *pdata;
	struct ar933x_uart_port *up;
	struct uart_port *port;
	struct resource *mem_res;
	struct resource *irq_res;
	int id;
	int ret;

	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -EINVAL;

	id = pdev->id;
	if (id == -1)
		id = 0;

	if (id > CONFIG_SERIAL_AR933X_NR_UARTS)
		return -EINVAL;

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_res) {
		dev_err(&pdev->dev, "no MEM resource\n");
		return -EINVAL;
	}

	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!irq_res) {
		dev_err(&pdev->dev, "no IRQ resource\n");
		return -EINVAL;
	}

	up = kzalloc(sizeof(struct ar933x_uart_port), GFP_KERNEL);
	if (!up)
		return -ENOMEM;

	port = &up->port;
	port->mapbase = mem_res->start;

	port->membase = ioremap(mem_res->start, AR933X_UART_REGS_SIZE);
	if (!port->membase) {
		ret = -ENOMEM;
		goto err_free_up;
	}

	port->line = id;
	port->irq = irq_res->start;
	port->dev = &pdev->dev;
	port->type = PORT_AR933X;
	port->iotype = UPIO_MEM32;
	port->uartclk = pdata->uartclk;

	port->regshift = 2;
	port->fifosize = AR933X_UART_FIFO_SIZE;
	port->ops = &ar933x_uart_ops;

	ar933x_uart_add_console_port(up);

	ret = uart_add_one_port(&ar933x_uart_driver, &up->port);
	if (ret)
		goto err_unmap;

	platform_set_drvdata(pdev, up);
	return 0;

err_unmap:
	iounmap(up->port.membase);
err_free_up:
	kfree(up);
	return ret;
}

static int __devexit ar933x_uart_remove(struct platform_device *pdev)
{
	struct ar933x_uart_port *up;

	up = platform_get_drvdata(pdev);
	platform_set_drvdata(pdev, NULL);

	if (up) {
		uart_remove_one_port(&ar933x_uart_driver, &up->port);
		iounmap(up->port.membase);
		kfree(up);
	}

	return 0;
}

static struct platform_driver ar933x_uart_platform_driver = {
	.probe		= ar933x_uart_probe,
	.remove		= __devexit_p(ar933x_uart_remove),
	.driver		= {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
};

static int __init ar933x_uart_init(void)
{
	int ret;

	ar933x_uart_driver.nr = CONFIG_SERIAL_AR933X_NR_UARTS;
	ret = uart_register_driver(&ar933x_uart_driver);
	if (ret)
		goto err_out;

	ret = platform_driver_register(&ar933x_uart_platform_driver);
	if (ret)
		goto err_unregister_uart_driver;

	return 0;

err_unregister_uart_driver:
	uart_unregister_driver(&ar933x_uart_driver);
err_out:
	return ret;
}

static void __exit ar933x_uart_exit(void)
{
	platform_driver_unregister(&ar933x_uart_platform_driver);
	uart_unregister_driver(&ar933x_uart_driver);
}

module_init(ar933x_uart_init);
module_exit(ar933x_uart_exit);

MODULE_DESCRIPTION("Atheros AR933X UART driver");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
