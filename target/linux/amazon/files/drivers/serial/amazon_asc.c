/*
 *  Driver for AMAZONASC serial ports
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *  Based on drivers/serial/serial_s3c2400.c
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
#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>
#include <asm/amazon/serial.h>

#define PORT_AMAZONASC  111

#include <linux/serial_core.h>

#define UART_NR		1

#define UART_DUMMY_UER_RX 1

#define SERIAL_AMAZONASC_MAJOR	TTY_MAJOR
#define CALLOUT_AMAZONASC_MAJOR	TTYAUX_MAJOR
#define SERIAL_AMAZONASC_MINOR	64
#define SERIAL_AMAZONASC_NR	UART_NR

static void amazonasc_tx_chars(struct uart_port *port);
extern void prom_printf(const char * fmt, ...);
static struct uart_port amazonasc_ports[UART_NR];
static struct uart_driver amazonasc_reg;
static unsigned int uartclk = 0;
extern unsigned int amazon_get_fpi_hz(void);

static void amazonasc_stop_tx(struct uart_port *port)
{
	/* fifo underrun shuts up after firing once */
	return;
}

static void amazonasc_start_tx(struct uart_port *port)
{
	unsigned long flags;

	local_irq_save(flags);
	amazonasc_tx_chars(port);
	local_irq_restore(flags);

	return;
}

static void amazonasc_stop_rx(struct uart_port *port)
{
	/* clear the RX enable bit */
	amazon_writel(ASCWHBCON_CLRREN, AMAZON_ASC_WHBCON);
}

static void amazonasc_enable_ms(struct uart_port *port)
{
	/* no modem signals */
	return;
}

#include <linux/version.h>

static void
amazonasc_rx_chars(struct uart_port *port)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 26))
	struct tty_struct *tty = port->info->port.tty;
#else
	struct tty_struct *tty = port->info->tty;
#endif
	unsigned int ch = 0, rsr = 0, fifocnt;

	fifocnt = amazon_readl(AMAZON_ASC_FSTAT) & ASCFSTAT_RXFFLMASK;
	while (fifocnt--)
	{
		u8 flag = TTY_NORMAL;
		ch = amazon_readl(AMAZON_ASC_RBUF);
		rsr = (amazon_readl(AMAZON_ASC_CON) & ASCCON_ANY) | UART_DUMMY_UER_RX;
		tty_flip_buffer_push(tty);
		port->icount.rx++;

		/*
		 * Note that the error handling code is
		 * out of the main execution path
		 */
		if (rsr & ASCCON_ANY) {
			if (rsr & ASCCON_PE) {
				port->icount.parity++;
				amazon_writel_masked(AMAZON_ASC_WHBCON, ASCWHBCON_CLRPE, ASCWHBCON_CLRPE);
			} else if (rsr & ASCCON_FE) {
				port->icount.frame++;
				amazon_writel_masked(AMAZON_ASC_WHBCON, ASCWHBCON_CLRFE, ASCWHBCON_CLRFE);
			}
			if (rsr & ASCCON_OE) {
				port->icount.overrun++;
				amazon_writel_masked(AMAZON_ASC_WHBCON, ASCWHBCON_CLROE, ASCWHBCON_CLROE);
			}

			rsr &= port->read_status_mask;

			if (rsr & ASCCON_PE)
				flag = TTY_PARITY;
			else if (rsr & ASCCON_FE)
				flag = TTY_FRAME;
		}

		if ((rsr & port->ignore_status_mask) == 0)
			tty_insert_flip_char(tty, ch, flag);

		if (rsr & ASCCON_OE)
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


static void amazonasc_tx_chars(struct uart_port *port)
{
	struct circ_buf *xmit = &port->info->xmit;

	if (uart_tx_stopped(port)) {
		amazonasc_stop_tx(port);
		return;
	}
	
	while (((amazon_readl(AMAZON_ASC_FSTAT) & ASCFSTAT_TXFFLMASK)
			>> ASCFSTAT_TXFFLOFF) != AMAZONASC_TXFIFO_FULL)
	{
		if (port->x_char) {
			amazon_writel(port->x_char, AMAZON_ASC_TBUF);
			port->icount.tx++;
			port->x_char = 0;
			continue;
		}

		if (uart_circ_empty(xmit))
			break;

		amazon_writel(xmit->buf[xmit->tail], AMAZON_ASC_TBUF);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);
}

static irqreturn_t amazonasc_tx_int(int irq, void *port)
{
	amazon_writel(ASC_IRNCR_TIR, AMAZON_ASC_IRNCR1);
	amazonasc_start_tx(port);

	/* clear any pending interrupts */
	amazon_writel_masked(AMAZON_ASC_WHBCON, 
			(ASCWHBCON_CLRPE | ASCWHBCON_CLRFE | ASCWHBCON_CLROE), 
			(ASCWHBCON_CLRPE | ASCWHBCON_CLRFE | ASCWHBCON_CLROE));

	return IRQ_HANDLED;
}

static irqreturn_t amazonasc_er_int(int irq, void *port)
{
	/* clear any pending interrupts */
	amazon_writel_masked(AMAZON_ASC_WHBCON, 
			(ASCWHBCON_CLRPE | ASCWHBCON_CLRFE | ASCWHBCON_CLROE), 
			(ASCWHBCON_CLRPE | ASCWHBCON_CLRFE | ASCWHBCON_CLROE));
	
	return IRQ_HANDLED;
}

static irqreturn_t amazonasc_rx_int(int irq, void *port)
{
	amazon_writel(ASC_IRNCR_RIR, AMAZON_ASC_IRNCR1);
	amazonasc_rx_chars((struct uart_port *) port);
	return IRQ_HANDLED;
}

static u_int amazonasc_tx_empty(struct uart_port *port)
{
	int status;

	/*
	 * FSTAT tells exactly how many bytes are in the FIFO.
	 * The question is whether we really need to wait for all
	 * 16 bytes to be transmitted before reporting that the
	 * transmitter is empty.
	 */
	status = amazon_readl(AMAZON_ASC_FSTAT) & ASCFSTAT_TXFFLMASK;
	return status ? 0 : TIOCSER_TEMT;
}

static u_int amazonasc_get_mctrl(struct uart_port *port)
{
	/* no modem control signals - the readme says to pretend all are set */
	return TIOCM_CTS|TIOCM_CAR|TIOCM_DSR;
}

static void amazonasc_set_mctrl(struct uart_port *port, u_int mctrl)
{
	/* no modem control - just return */
	return;
}

static void amazonasc_break_ctl(struct uart_port *port, int break_state)
{
	/* no way to send a break */
	return;
}

static int amazonasc_startup(struct uart_port *port)
{
	unsigned int con = 0;
	unsigned long flags;
	int retval;

	/* this assumes: CON.BRS = CON.FDE = 0 */
	if (uartclk == 0)
		uartclk = amazon_get_fpi_hz();

	amazonasc_ports[0].uartclk = uartclk;

	local_irq_save(flags);

	/* this setup was probably already done in u-boot */
	/* ASC and GPIO Port 1 bits 3 and 4 share the same pins
	 * P1.3 (RX) in, Alternate 10
	 * P1.4 (TX) in, Alternate 10
	 */
	 amazon_writel_masked(AMAZON_GPIO_P1_DIR, 0x18, 0x10); 	//P1.4 output, P1.3 input
	 amazon_writel_masked(AMAZON_GPIO_P1_ALTSEL0, 0x18, 0x18); 		//ALTSETL0 11
	 amazon_writel_masked(AMAZON_GPIO_P1_ALTSEL1, 0x18, 0);	 	//ALTSETL1 00
	 amazon_writel_masked(AMAZON_GPIO_P1_OD, 0x18, 0x10);
	
	/* set up the CLC */
	amazon_writel_masked(AMAZON_ASC_CLC, AMAZON_ASC_CLC_DISS, 0);
	amazon_writel_masked(AMAZON_ASC_CLC, ASCCLC_RMCMASK, 1 << ASCCLC_RMCOFFSET);
	
	/* asynchronous mode */
	con = ASCCON_M_8ASYNC | ASCCON_FEN | ASCCON_OEN | ASCCON_PEN;
	
	/* choose the line - there's only one */
	amazon_writel(0, AMAZON_ASC_PISEL);
	amazon_writel(((AMAZONASC_TXFIFO_FL << ASCTXFCON_TXFITLOFF) & ASCTXFCON_TXFITLMASK) | ASCTXFCON_TXFEN | ASCTXFCON_TXFFLU, 
		AMAZON_ASC_TXFCON);
	amazon_writel(((AMAZONASC_RXFIFO_FL << ASCRXFCON_RXFITLOFF) & ASCRXFCON_RXFITLMASK) | ASCRXFCON_RXFEN | ASCRXFCON_RXFFLU, 
		AMAZON_ASC_RXFCON);
	wmb();
	
	amazon_writel_masked(AMAZON_ASC_CON, con, con);

	retval = request_irq(AMAZONASC_RIR, amazonasc_rx_int, 0, "asc_rx", port);
	if (retval){
		printk("failed to request amazonasc_rx_int\n");
		return retval;
	}
	retval = request_irq(AMAZONASC_TIR, amazonasc_tx_int, 0, "asc_tx", port);
	if (retval){
		printk("failed to request amazonasc_tx_int\n");
		goto err1;
	}

	retval = request_irq(AMAZONASC_EIR, amazonasc_er_int, 0, "asc_er", port);
	if (retval){
		printk("failed to request amazonasc_er_int\n");
		goto err2;
	}
	
	local_irq_restore(flags);
	return 0;

err2:
	free_irq(AMAZONASC_TIR, port);
	
err1:
	free_irq(AMAZONASC_RIR, port);
	local_irq_restore(flags);
	return retval;
}

static void amazonasc_shutdown(struct uart_port *port)
{
	free_irq(AMAZONASC_RIR, port);
	free_irq(AMAZONASC_TIR, port);
	free_irq(AMAZONASC_EIR, port);
	/*
	 * disable the baudrate generator to disable the ASC
	 */
	amazon_writel(0, AMAZON_ASC_CON);

	/* flush and then disable the fifos */
	amazon_writel_masked(AMAZON_ASC_RXFCON, ASCRXFCON_RXFFLU, ASCRXFCON_RXFFLU);
	amazon_writel_masked(AMAZON_ASC_RXFCON, ASCRXFCON_RXFEN, 0);
	amazon_writel_masked(AMAZON_ASC_TXFCON, ASCTXFCON_TXFFLU, ASCTXFCON_TXFFLU);
	amazon_writel_masked(AMAZON_ASC_TXFCON, ASCTXFCON_TXFEN, 0);
}

static void amazonasc_set_termios(struct uart_port *port, struct ktermios *new, struct ktermios *old)
{
	unsigned int cflag;
	unsigned int iflag;
	unsigned int baud, quot;
	unsigned int con = 0;
	unsigned long flags;

	cflag = new->c_cflag;
	iflag = new->c_iflag;

	/* byte size and parity */
	switch (cflag & CSIZE) {
	/* 7 bits are always with parity */
	case CS7: con = ASCCON_M_7ASYNCPAR; break;
	/* the ASC only suports 7 and 8 bits */
	case CS5:
	case CS6:
	default:
		if (cflag & PARENB)
			con = ASCCON_M_8ASYNCPAR;
		else
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

	port->read_status_mask = ASCCON_OE;
	if (iflag & INPCK)
		port->read_status_mask |= ASCCON_FE | ASCCON_PE;
	
	port->ignore_status_mask = 0;
	if (iflag & IGNPAR)
		port->ignore_status_mask |= ASCCON_FE | ASCCON_PE;
	
	if (iflag & IGNBRK) {
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (iflag & IGNPAR)
			port->ignore_status_mask |= ASCCON_OE;
	}

	/*
	 * Ignore all characters if CREAD is not set.
	 */
	if ((cflag & CREAD) == 0)
		port->ignore_status_mask |= UART_DUMMY_UER_RX;

	/* set error signals  - framing, parity  and overrun */
	con |= ASCCON_FEN;
	con |= ASCCON_OEN;
	con |= ASCCON_PEN;
	/* enable the receiver */
	con |= ASCCON_REN;

	/* block the IRQs */
	local_irq_save(flags);

	/* set up CON */
	amazon_writel(con, AMAZON_ASC_CON);

	/* Set baud rate - take a divider of 2 into account */
    baud = uart_get_baud_rate(port, new, old, 0, port->uartclk/16);
	quot = uart_get_divisor(port, baud);
	quot = quot/2 - 1;

	/* the next 3 probably already happened when we set CON above */
	/* disable the baudrate generator */
	amazon_writel_masked(AMAZON_ASC_CON, ASCCON_R, 0);
	/* make sure the fractional divider is off */
	amazon_writel_masked(AMAZON_ASC_CON, ASCCON_FDE, 0);
	/* set up to use divisor of 2 */
	amazon_writel_masked(AMAZON_ASC_CON, ASCCON_BRS, 0);
	/* now we can write the new baudrate into the register */
	amazon_writel(quot, AMAZON_ASC_BTR);
	/* turn the baudrate generator back on */
	amazon_writel_masked(AMAZON_ASC_CON, ASCCON_R, ASCCON_R);

	local_irq_restore(flags);
}

static const char *amazonasc_type(struct uart_port *port)
{
	return port->type == PORT_AMAZONASC ? "AMAZONASC" : NULL;
}

/*
 * Release the memory region(s) being used by 'port'
 */
static void amazonasc_release_port(struct uart_port *port)
{
	return;
}

/*
 * Request the memory region(s) being used by 'port'
 */
static int amazonasc_request_port(struct uart_port *port)
{
	return 0;
}

/*
 * Configure/autoconfigure the port.
 */
static void amazonasc_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_AMAZONASC;
		amazonasc_request_port(port);
	}
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int amazonasc_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	int ret = 0;
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_AMAZONASC)
		ret = -EINVAL;
	if (ser->irq < 0 || ser->irq >= NR_IRQS)
		ret = -EINVAL;
	if (ser->baud_base < 9600)
		ret = -EINVAL;
	return ret;
}

static struct uart_ops amazonasc_pops = {
	.tx_empty =		amazonasc_tx_empty,
	.set_mctrl =	amazonasc_set_mctrl,
	.get_mctrl =	amazonasc_get_mctrl,
	.stop_tx =		amazonasc_stop_tx,
	.start_tx =		amazonasc_start_tx,
	.stop_rx =		amazonasc_stop_rx,
	.enable_ms =	amazonasc_enable_ms,
	.break_ctl =	amazonasc_break_ctl,
	.startup =		amazonasc_startup,
	.shutdown =		amazonasc_shutdown,
	.set_termios =	amazonasc_set_termios,
	.type =			amazonasc_type,
	.release_port =	amazonasc_release_port,
	.request_port =	amazonasc_request_port,
	.config_port =	amazonasc_config_port,
	.verify_port =	amazonasc_verify_port,
};

static struct uart_port amazonasc_ports[UART_NR] = {
	{
		membase:	(void *)AMAZON_ASC,
		mapbase:	AMAZON_ASC,
		iotype:		SERIAL_IO_MEM,
		irq:		AMAZONASC_RIR, /* RIR */
		uartclk:	0, /* filled in dynamically */
		fifosize:	16,
		unused:		{ AMAZONASC_TIR, AMAZONASC_EIR}, /* xmit/error/xmit-buffer-empty IRQ */
		type:		PORT_AMAZONASC,
		ops:		&amazonasc_pops,
		flags:		ASYNC_BOOT_AUTOCONF,
	},
};

static void amazonasc_console_write(struct console *co, const char *s, u_int count)
{
	int i, fifocnt;
	unsigned long flags;
	local_irq_save(flags);
	for (i = 0; i < count;)
	{
		/* wait until the FIFO is not full */
		do
		{
			fifocnt = (amazon_readl(AMAZON_ASC_FSTAT) & ASCFSTAT_TXFFLMASK)
					>> ASCFSTAT_TXFFLOFF;
		} while (fifocnt == AMAZONASC_TXFIFO_FULL);
		if (s[i] == '\0')
		{
			break;
		}
		if (s[i] == '\n')
		{
			amazon_writel('\r', AMAZON_ASC_TBUF);
			do
			{
				fifocnt = (amazon_readl(AMAZON_ASC_FSTAT) &
				ASCFSTAT_TXFFLMASK) >> ASCFSTAT_TXFFLOFF;
			} while (fifocnt == AMAZONASC_TXFIFO_FULL);
		}
		amazon_writel(s[i], AMAZON_ASC_TBUF);
		i++;
	} 

	local_irq_restore(flags);
}

static void __init
amazonasc_console_get_options(struct uart_port *port, int *baud, int *parity, int *bits)
{
	u_int lcr_h;

	lcr_h = amazon_readl(AMAZON_ASC_CON);
	/* do this only if the ASC is turned on */
	if (lcr_h & ASCCON_R) {
		u_int quot, div, fdiv, frac;

		*parity = 'n';
		if ((lcr_h & ASCCON_MODEMASK) == ASCCON_M_7ASYNCPAR ||
		            (lcr_h & ASCCON_MODEMASK) == ASCCON_M_8ASYNCPAR) {
			if (lcr_h & ASCCON_ODD)
				*parity = 'o';
			else
				*parity = 'e';
		}

		if ((lcr_h & ASCCON_MODEMASK) == ASCCON_M_7ASYNCPAR)
			*bits = 7;
		else
			*bits = 8;

		quot = amazon_readl(AMAZON_ASC_BTR) + 1;
		
		/* this gets hairy if the fractional divider is used */
		if (lcr_h & ASCCON_FDE)
		{
			div = 1;
			fdiv = amazon_readl(AMAZON_ASC_FDV);
			if (fdiv == 0)
				fdiv = 512;
			frac = 512;
		}
		else
		{
			div = lcr_h & ASCCON_BRS ? 3 : 2;
			fdiv = frac = 1;
		}
		/*
		 * This doesn't work exactly because we use integer
		 * math to calculate baud which results in rounding
		 * errors when we try to go from quot -> baud !!
		 * Try to make this work for both the fractional divider
		 * and the simple divider. Also try to avoid rounding
		 * errors using integer math.
		 */
		
		*baud = frac * (port->uartclk / (div * 512 * 16 * quot));
		if (*baud > 1100 && *baud < 2400)
			*baud = 1200;
		if (*baud > 2300 && *baud < 4800)
			*baud = 2400;
		if (*baud > 4700 && *baud < 9600)
			*baud = 4800;
		if (*baud > 9500 && *baud < 19200)
			*baud = 9600;
		if (*baud > 19000 && *baud < 38400)
			*baud = 19200;
		if (*baud > 38400 && *baud < 57600)
			*baud = 38400;
		if (*baud > 57600 && *baud < 115200)
			*baud = 57600;
		if (*baud > 115200 && *baud < 230400)
			*baud = 115200;
	}
}

static int __init amazonasc_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	/* this assumes: CON.BRS = CON.FDE = 0 */
	if (uartclk == 0)
		uartclk = amazon_get_fpi_hz();
	co->index = 0;	
	port = &amazonasc_ports[0];
	amazonasc_ports[0].uartclk = uartclk;
	amazonasc_ports[0].type = PORT_AMAZONASC;

	if (options){
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	}

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver amazonasc_reg;
static struct console amazonasc_console = {
	name:		"ttyS",
	write:		amazonasc_console_write,
	device:		uart_console_device,
	setup:		amazonasc_console_setup,
	flags:		CON_PRINTBUFFER,
	index:		-1,
	data:		&amazonasc_reg,
};

static int __init amazonasc_console_init(void)
{
	register_console(&amazonasc_console);
	return 0;
}
console_initcall(amazonasc_console_init);

static struct uart_driver amazonasc_reg = {
	.owner =			THIS_MODULE,
	.driver_name = 		"serial",
	.dev_name =			"ttyS",
	.major =			TTY_MAJOR,
	.minor =			64,
	.nr =				UART_NR,
	.cons =				&amazonasc_console,
};

static int __init amazonasc_init(void)
{
	unsigned char res;
	uart_register_driver(&amazonasc_reg);
	res = uart_add_one_port(&amazonasc_reg, &amazonasc_ports[0]);
	return res;
}

static void __exit amazonasc_exit(void)
{
	uart_unregister_driver(&amazonasc_reg);
}

module_init(amazonasc_init);
module_exit(amazonasc_exit);

MODULE_AUTHOR("Gary Jennejohn, Felix Fietkau, John Crispin");
MODULE_DESCRIPTION("MIPS AMAZONASC serial port driver");
MODULE_LICENSE("GPL");
