/*
 *  Driver for IFXMIPSASC serial ports
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2004 Infineon IFAP DC COM CPE
 * Copyright (C) 2007 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/circ_buf.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/irq.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <asm/danube/danube.h>
#include <asm/danube/danube_irq.h>
#include <asm/danube/danube_serial.h>

#define PORT_IFXMIPSASC  111

#include <linux/serial_core.h>

#define UART_DUMMY_UER_RX 1

static void danubeasc_tx_chars(struct uart_port *port);
extern void prom_printf(const char * fmt, ...);
static struct uart_port danubeasc_port;
static struct uart_driver danubeasc_reg;
static unsigned int uartclk = 0;
extern unsigned int danube_get_fpi_hz(void);

static void
danubeasc_stop_tx (struct uart_port *port)
{
	/* fifo underrun shuts up after firing once */
	return;
}

static void
danubeasc_start_tx (struct uart_port *port)
{
	unsigned long flags;

	local_irq_save(flags);
	danubeasc_tx_chars(port);
	local_irq_restore(flags);

	return;
}

static void
danubeasc_stop_rx (struct uart_port *port)
{
	/* clear the RX enable bit */
	writel(ASCWHBSTATE_CLRREN, IFXMIPS_ASC1_WHBSTATE);
}

static void
danubeasc_enable_ms (struct uart_port *port)
{
	/* no modem signals */
	return;
}

static void
danubeasc_rx_chars (struct uart_port *port)
{
	struct tty_struct *tty = port->info->tty;
	unsigned int ch = 0, rsr = 0, fifocnt;

	fifocnt = readl(IFXMIPS_ASC1_FSTAT) & ASCFSTAT_RXFFLMASK;
	while (fifocnt--)
	{
		u8 flag = TTY_NORMAL;
		ch = readl(IFXMIPS_ASC1_RBUF);
		rsr = (readl(IFXMIPS_ASC1_STATE) & ASCSTATE_ANY) | UART_DUMMY_UER_RX;
		tty_flip_buffer_push(tty);
		port->icount.rx++;

		/*
		 * Note that the error handling code is
		 * out of the main execution path
		 */
		if (rsr & ASCSTATE_ANY) {
			if (rsr & ASCSTATE_PE) {
				port->icount.parity++;
				writel(readl(IFXMIPS_ASC1_WHBSTATE) | ASCWHBSTATE_CLRPE, IFXMIPS_ASC1_WHBSTATE);
			} else if (rsr & ASCSTATE_FE) {
				port->icount.frame++;
				writel(readl(IFXMIPS_ASC1_WHBSTATE) | ASCWHBSTATE_CLRFE, IFXMIPS_ASC1_WHBSTATE);
			}
			if (rsr & ASCSTATE_ROE) {
				port->icount.overrun++;
				writel(readl(IFXMIPS_ASC1_WHBSTATE) | ASCWHBSTATE_CLRROE, IFXMIPS_ASC1_WHBSTATE);
			}

			rsr &= port->read_status_mask;

			if (rsr & ASCSTATE_PE)
				flag = TTY_PARITY;
			else if (rsr & ASCSTATE_FE)
				flag = TTY_FRAME;
		}

		if ((rsr & port->ignore_status_mask) == 0)
			tty_insert_flip_char(tty, ch, flag);

		if (rsr & ASCSTATE_ROE)
			/*
			 * Overrun is special, since it's reported
			 * immediately, and doesn't affect the current
			 * character
			 */
			tty_insert_flip_char(tty, 0, TTY_OVERRUN);
	}
	if (ch != 0)
		tty_flip_buffer_push(tty);

	return;
}


static void
danubeasc_tx_chars (struct uart_port *port)
{
	struct circ_buf *xmit = &port->info->xmit;

	if (uart_tx_stopped(port)) {
		danubeasc_stop_tx(port);
		return;
	}

	while(((readl(IFXMIPS_ASC1_FSTAT) & ASCFSTAT_TXFFLMASK)
			        >> ASCFSTAT_TXFFLOFF) != IFXMIPSASC_TXFIFO_FULL)
	{
		if (port->x_char) {
			writel(port->x_char, IFXMIPS_ASC1_TBUF);
			port->icount.tx++;
			port->x_char = 0;
			continue;
		}

		if (uart_circ_empty(xmit))
			break;

		writel(port->info->xmit.buf[port->info->xmit.tail], IFXMIPS_ASC1_TBUF);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);
}

static irqreturn_t
danubeasc_tx_int (int irq, void *port)
{
	writel(ASC_IRNCR_TIR, IFXMIPS_ASC1_IRNCR);
	danubeasc_start_tx(port);
	mask_and_ack_danube_irq(irq);

	return IRQ_HANDLED;
}

static irqreturn_t
danubeasc_er_int (int irq, void *port)
{
	/* clear any pending interrupts */
	writel(readl(IFXMIPS_ASC1_WHBSTATE) | ASCWHBSTATE_CLRPE |
			ASCWHBSTATE_CLRFE | ASCWHBSTATE_CLRROE, IFXMIPS_ASC1_WHBSTATE);

	return IRQ_HANDLED;
}

static irqreturn_t
danubeasc_rx_int (int irq, void *port)
{
	writel(ASC_IRNCR_RIR, IFXMIPS_ASC1_IRNCR);
	danubeasc_rx_chars((struct uart_port *) port);
	mask_and_ack_danube_irq(irq);

	return IRQ_HANDLED;
}

static unsigned int
danubeasc_tx_empty (struct uart_port *port)
{
	int status;

	status = readl(IFXMIPS_ASC1_FSTAT) & ASCFSTAT_TXFFLMASK;

	return status ? 0 : TIOCSER_TEMT;
}

static unsigned int
danubeasc_get_mctrl (struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_CAR | TIOCM_DSR;
}

static void
danubeasc_set_mctrl (struct uart_port *port, u_int mctrl)
{
	return;
}

static void
danubeasc_break_ctl (struct uart_port *port, int break_state)
{
	return;
}

static void
danubeasc1_hw_init (void)
{
	/* this setup was probably already done in ROM/u-boot  but we do it again*/
	/* TODO: GPIO pins are multifunction */
	writel(readl(IFXMIPS_ASC1_CLC) & ~IFXMIPS_ASC1_CLC_DISS, IFXMIPS_ASC1_CLC);
	writel((readl(IFXMIPS_ASC1_CLC) & ~ASCCLC_RMCMASK) | (1 << ASCCLC_RMCOFFSET), IFXMIPS_ASC1_CLC);
	writel(0, IFXMIPS_ASC1_PISEL);
	writel(((IFXMIPSASC_TXFIFO_FL << ASCTXFCON_TXFITLOFF) &
		ASCTXFCON_TXFITLMASK) | ASCTXFCON_TXFEN | ASCTXFCON_TXFFLU, IFXMIPS_ASC1_TXFCON);
	writel(((IFXMIPSASC_RXFIFO_FL << ASCRXFCON_RXFITLOFF) &
		ASCRXFCON_RXFITLMASK) | ASCRXFCON_RXFEN | ASCRXFCON_RXFFLU, IFXMIPS_ASC1_RXFCON);
	wmb ();

	/*framing, overrun, enable */
	writel(readl(IFXMIPS_ASC1_CON) | ASCCON_M_8ASYNC | ASCCON_FEN | ASCCON_TOEN | ASCCON_ROEN,
		IFXMIPS_ASC1_CON);
}

static int
danubeasc_startup (struct uart_port *port)
{
	unsigned long flags;
	int retval;

	/* this assumes: CON.BRS = CON.FDE = 0 */
	if (uartclk == 0)
		uartclk = danube_get_fpi_hz();

	danubeasc_port.uartclk = uartclk;

	danubeasc1_hw_init();

	local_irq_save(flags);

	retval = request_irq(IFXMIPSASC1_RIR, danubeasc_rx_int, IRQF_DISABLED, "asc_rx", port);
	if (retval){
		printk("failed to request danubeasc_rx_int\n");
		return retval;
	}

	retval = request_irq(IFXMIPSASC1_TIR, danubeasc_tx_int, IRQF_DISABLED, "asc_tx", port);
	if (retval){
		printk("failed to request danubeasc_tx_int\n");
		goto err1;
	}

	retval = request_irq(IFXMIPSASC1_EIR, danubeasc_er_int, IRQF_DISABLED, "asc_er", port);
	if (retval){
		printk("failed to request danubeasc_er_int\n");
		goto err2;
	}

	writel(ASC_IRNREN_RX_BUF | ASC_IRNREN_TX_BUF | ASC_IRNREN_ERR | ASC_IRNREN_TX,
		IFXMIPS_ASC1_IRNREN);

	local_irq_restore(flags);

	return 0;

err2:
	free_irq(IFXMIPSASC1_TIR, port);

err1:
	free_irq(IFXMIPSASC1_RIR, port);
	local_irq_restore(flags);

	return retval;
}

static void
danubeasc_shutdown (struct uart_port *port)
{
	free_irq(IFXMIPSASC1_RIR, port);
	free_irq(IFXMIPSASC1_TIR, port);
	free_irq(IFXMIPSASC1_EIR, port);
	/*
	 * disable the baudrate generator to disable the ASC
	 */
	writel(0, IFXMIPS_ASC1_CON);

	/* flush and then disable the fifos */
	writel(readl(IFXMIPS_ASC1_RXFCON) | ASCRXFCON_RXFFLU, IFXMIPS_ASC1_RXFCON);
	writel(readl(IFXMIPS_ASC1_RXFCON) & ~ASCRXFCON_RXFEN, IFXMIPS_ASC1_RXFCON);
	writel(readl(IFXMIPS_ASC1_TXFCON) | ASCTXFCON_TXFFLU, IFXMIPS_ASC1_TXFCON);
	writel(readl(IFXMIPS_ASC1_TXFCON) & ~ASCTXFCON_TXFEN, IFXMIPS_ASC1_TXFCON);
}

static void danubeasc_set_termios(struct uart_port *port, struct ktermios *new, struct ktermios *old)
{
	unsigned int cflag;
	unsigned int iflag;
	unsigned int quot;
	unsigned int baud;
	unsigned int con = 0;
	unsigned long flags;

	cflag = new->c_cflag;
	iflag = new->c_iflag;

	/* byte size and parity */
	switch (cflag & CSIZE) {
	case CS7:
		con = ASCCON_M_7ASYNC;
		break;

	case CS5:
	case CS6:
	default:
		con = ASCCON_M_8ASYNC;
		break;
	}

	if (cflag & CSTOPB)
		con |= ASCCON_STP;

	if (cflag & PARENB) {
		if (!(cflag & PARODD))
			con &= ~ASCCON_ODD;
		else
			con |= ASCCON_ODD;
	}

	port->read_status_mask = ASCSTATE_ROE;
	if (iflag & INPCK)
		port->read_status_mask |= ASCSTATE_FE | ASCSTATE_PE;

	port->ignore_status_mask = 0;
	if (iflag & IGNPAR)
		port->ignore_status_mask |= ASCSTATE_FE | ASCSTATE_PE;

	if (iflag & IGNBRK) {
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (iflag & IGNPAR)
			port->ignore_status_mask |= ASCSTATE_ROE;
	}

	if ((cflag & CREAD) == 0)
		port->ignore_status_mask |= UART_DUMMY_UER_RX;

	/* set error signals  - framing, parity  and overrun, enable receiver */
	con |= ASCCON_FEN | ASCCON_TOEN | ASCCON_ROEN;

	local_irq_save(flags);

	/* set up CON */
	writel(readl(IFXMIPS_ASC1_CON) | con, IFXMIPS_ASC1_CON);

	/* Set baud rate - take a divider of 2 into account */
    baud = uart_get_baud_rate(port, new, old, 0, port->uartclk / 16);
	quot = uart_get_divisor(port, baud);
	quot = quot / 2 - 1;

	/* disable the baudrate generator */
	writel(readl(IFXMIPS_ASC1_CON) & ~ASCCON_R, IFXMIPS_ASC1_CON);

	/* make sure the fractional divider is off */
	writel(readl(IFXMIPS_ASC1_CON) & ~ASCCON_FDE, IFXMIPS_ASC1_CON);

	/* set up to use divisor of 2 */
	writel(readl(IFXMIPS_ASC1_CON) & ~ASCCON_BRS, IFXMIPS_ASC1_CON);

	/* now we can write the new baudrate into the register */
	writel(quot, IFXMIPS_ASC1_BG);

	/* turn the baudrate generator back on */
	writel(readl(IFXMIPS_ASC1_CON) | ASCCON_R, IFXMIPS_ASC1_CON);

	/* enable rx */
	writel(ASCWHBSTATE_SETREN, IFXMIPS_ASC1_WHBSTATE);

	local_irq_restore(flags);
}

static const char*
danubeasc_type (struct uart_port *port)
{
	return port->type == PORT_IFXMIPSASC ? "IFXMIPSASC" : NULL;
}

static void
danubeasc_release_port (struct uart_port *port)
{
	return;
}

static int
danubeasc_request_port (struct uart_port *port)
{
	return 0;
}

static void
danubeasc_config_port (struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_IFXMIPSASC;
		danubeasc_request_port(port);
	}
}

static int
danubeasc_verify_port (struct uart_port *port, struct serial_struct *ser)
{
	int ret = 0;
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_IFXMIPSASC)
		ret = -EINVAL;
	if (ser->irq < 0 || ser->irq >= NR_IRQS)
		ret = -EINVAL;
	if (ser->baud_base < 9600)
		ret = -EINVAL;
	return ret;
}

static struct uart_ops danubeasc_pops = {
	.tx_empty =		danubeasc_tx_empty,
	.set_mctrl =	danubeasc_set_mctrl,
	.get_mctrl =	danubeasc_get_mctrl,
	.stop_tx =		danubeasc_stop_tx,
	.start_tx =		danubeasc_start_tx,
	.stop_rx =		danubeasc_stop_rx,
	.enable_ms =	danubeasc_enable_ms,
	.break_ctl =	danubeasc_break_ctl,
	.startup =		danubeasc_startup,
	.shutdown =		danubeasc_shutdown,
	.set_termios =	danubeasc_set_termios,
	.type =			danubeasc_type,
	.release_port =	danubeasc_release_port,
	.request_port =	danubeasc_request_port,
	.config_port =	danubeasc_config_port,
	.verify_port =	danubeasc_verify_port,
};

static struct uart_port danubeasc_port = {
		membase:	(void *)IFXMIPS_ASC1_BASE_ADDR,
		mapbase:	IFXMIPS_ASC1_BASE_ADDR,
		iotype:		SERIAL_IO_MEM,
		irq:		IFXMIPSASC1_RIR,
		uartclk:	0,
		fifosize:	16,
		unused:		{IFXMIPSASC1_TIR, IFXMIPSASC1_EIR},
		type:		PORT_IFXMIPSASC,
		ops:		&danubeasc_pops,
		flags:		ASYNC_BOOT_AUTOCONF,
};

static void
danubeasc_console_write (struct console *co, const char *s, u_int count)
{
	int i, fifocnt;
	unsigned long flags;

	local_irq_save(flags);
	for (i = 0; i < count; i++)
	{
		/* wait until the FIFO is not full */
		do
		{
			fifocnt = (readl(IFXMIPS_ASC1_FSTAT) & ASCFSTAT_TXFFLMASK)
			                >> ASCFSTAT_TXFFLOFF;
		} while (fifocnt == IFXMIPSASC_TXFIFO_FULL);

		if (s[i] == '\0')
		{
			break;
		}

		if (s[i] == '\n')
		{
			writel('\r', IFXMIPS_ASC1_TBUF);
			do
			{
				fifocnt = (readl(IFXMIPS_ASC1_FSTAT) & ASCFSTAT_TXFFLMASK)
					>> ASCFSTAT_TXFFLOFF;
			} while (fifocnt == IFXMIPSASC_TXFIFO_FULL);
		}
		writel(s[i], IFXMIPS_ASC1_TBUF);
	}

	local_irq_restore(flags);
}

static int __init
danubeasc_console_setup (struct console *co, char *options)
{
	struct uart_port *port;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (uartclk == 0)
		uartclk = danube_get_fpi_hz();
	co->index = 0;
	port = &danubeasc_port;
	danubeasc_port.uartclk = uartclk;
	danubeasc_port.type = PORT_IFXMIPSASC;

	if (options){
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	}

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver danubeasc_reg;
static struct console danubeasc_console = {
	name:		"ttyS",
	write:		danubeasc_console_write,
	device:		uart_console_device,
	setup:		danubeasc_console_setup,
	flags:		CON_PRINTBUFFER,
	index:		-1,
	data:		&danubeasc_reg,
};

static int __init
danubeasc_console_init (void)
{
	register_console(&danubeasc_console);
	return 0;
}
console_initcall(danubeasc_console_init);

static struct uart_driver danubeasc_reg = {
	.owner =			THIS_MODULE,
	.driver_name =		"serial",
	.dev_name =			"ttyS",
	.major =			TTY_MAJOR,
	.minor =			64,
	.nr =				1,
	.cons =				&danubeasc_console,
};

static int __init
danubeasc_init (void)
{
	unsigned char res;

	uart_register_driver(&danubeasc_reg);
	res = uart_add_one_port(&danubeasc_reg, &danubeasc_port);

	return res;
}

static void __exit
danubeasc_exit (void)
{
	uart_unregister_driver(&danubeasc_reg);
}

module_init(danubeasc_init);
module_exit(danubeasc_exit);

MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("MIPS Danube serial port driver");
MODULE_LICENSE("GPL");
