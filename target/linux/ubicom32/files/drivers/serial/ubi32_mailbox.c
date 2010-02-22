/*
 * drivers/serial/ubi32_mailbox.c
 *   Ubicom32 On-Chip Mailbox Driver
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>

#include <asm/ip5000.h>

#define SERIAL_UBICOM_BAUDRATE	115200
#define SERIAL_UBICOM_DATA_BIT	8	/* Fixed parameter - do not change */
#define SERIAL_UBICOM_PAR_BIT	0	/* Fixed parameter - do not change */
#define SERIAL_UBICOM_STOP_BIT	1	/* Fixed parameter - do not change */

/* UART name and device definitions */
#define UBI32_MAILBOX_NAME	"ttyUM" // XXX
#define UBI32_MAILBOX_MAJOR	207 // XXX
#define UBI32_MAILBOX_MINOR	64

#define PORT_UBI32_MAILBOX	1235
#define NR_PORTS 1

#define get_sclk() 0

struct ubi32_mailbox_port {
	struct uart_port port;
	/*
	 * NOTE (rkeller):
	 * the uart port is wrapped in another structure in case we need to hold more state than
	 * what we can hold in the uart_port.
	 * Not sure if we need this, I took over the concept from the blackfin driver.
	 */
} ubi32_mailbox_ports[NR_PORTS];

struct ubi32_mailbox_resource {
	int uart_base_addr;
	int uart_irq;
} ubi32_mailbox_resource[NR_PORTS] = {
	/*
	 * uart_base_addr has to be non-NULL because it is put in the uart_port membase.
	 * If membase if null the kernel skips the configuration and our port_type never gets set.
	 */
	{ISD_MAILBOX_BASE, ISD_MAILBOX_INT}
};

static volatile struct ubicom32_isd_mailbox {
	volatile u32_t in;
	volatile u32_t out;
	volatile u32_t status;
} *ubi32_mailbox = (struct ubicom32_isd_mailbox *)ISD_MAILBOX_BASE;

static void ubi32_mailbox_tx_chars(struct ubi32_mailbox_port *uart);

static void ubi32_mailbox_mctrl_check(struct ubi32_mailbox_port *uart);

#define TRUE 1
#define FALSE 0

static int mailbox_console_flg = TRUE;
static int num_timeouts = 0;

/*
 * dummy functions and defined to be able to compile the Blackfin code
 */
#define UART_GET_LSR(port) (1)
#define UART_PUT_LSR(port, bits)
#define UART_CLEAR_LSR(port) (1)
#define TEMT 1
#define TFI 1
#define BI 1
#define PE 1
#define OE 1
#define FE 1
#define THRE 1
#define DR 1
#define UART_GET_LCR(port) (1)
#define UART_PUT_LCR(port, bits)
#define SB 1
#define STB 1
#define PEN 1
#define EPS 1
#define STP 1
#define WLS(n) 0
#define UART_GET_IER(port) (1)
#define UART_SET_IER(port, bits)
#define UART_CLEAR_IER(port, bits)
#define ETBEI 0
#define ERBFI 0
#define UART_GET_CHAR(port) ubi32_mailbox_get_char()
#define UART_PUT_CHAR(port, ch) ubi32_mailbox_put_char(ch)
#define SSYNC()
#define UART_GET_DLL(port) 0
#define UART_PUT_DLL(port, ch)
#define UART_GET_DLH(port) 0
#define UART_PUT_DLH(port, ch)
#define UART_GET_GCTL(port) (0)
#define UART_PUT_GCTL(port, ch)
#define UCEN 1

/*
 * ubi32_mailbox_get_char_avail()
 */
static int ubi32_mailbox_get_char_avail(void)
{
	return !(ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY);
}

/*
 * ubi32_mailbox_get_char()
 */
static u32_t ubi32_mailbox_get_char(void)
{
	if (mailbox_console_flg == TRUE) {
		/*
		 * Mailbox console is connected.
		 */
		while (ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY);
		return ubi32_mailbox->in & 0xff;
	}

	/*
	 * Mailbox console was not connected.
	 */
	if (ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY) {
		return 0xff;
	}

	/*
	 * Mailbox console is connecting.
	 */
	mailbox_console_flg = TRUE;
	num_timeouts = 0;
	return ubi32_mailbox->in & 0xff;
}

#define MAILBOX_MAX_ATTEMPTS 1000000
#define MAILBOX_MAX_TIMEOUTS 5
/*
 * ubi32_mailbox_put_char()
 */
static void ubi32_mailbox_put_char(u32_t v)
{
	/*
	 * Wait to be able to output.
	 */
	u32_t num_attempts = 0;

	if(mailbox_console_flg == TRUE) {
		while(num_attempts++ < MAILBOX_MAX_ATTEMPTS) {
			if(ubi32_mailbox->status & ISD_MAILBOX_STATUS_OUT_EMPTY) {
				break;
			}
		}

		/*
		 * If timed out more than 5 times on send, mailbox console is disconnected now.
		 */
		if (num_attempts > MAILBOX_MAX_ATTEMPTS) {
			if (num_timeouts++ > MAILBOX_MAX_TIMEOUTS) {
				mailbox_console_flg = FALSE;
			}
		}
	}

	asm volatile(
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
	);

	ubi32_mailbox->out = v & 0xff;
}

static void ubi32_mailbox_hw_init(struct ubi32_mailbox_port *uart)
{
// NOTE: It does not do any good to do these here because we are running on the linux hardware thread,
//	and these have to be called on the ldsr thread.
//	ubicom32_clear_interrupt(ISD_MAILBOX_INT);
//	ubicom32_enable_interrupt(ISD_MAILBOX_INT);
}

/*
 * interrupts are disabled on entry
 */
static void ubi32_mailbox_stop_tx(struct uart_port *port)
{
//	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;
//	struct circ_buf *xmit = &uart->port.info->xmit;

	while (!(UART_GET_LSR(uart) & TEMT))
		cpu_relax();

	/* Clear TFI bit */
	UART_PUT_LSR(uart, TFI);
	UART_CLEAR_IER(uart, ETBEI);
}

/*
 * port is locked and interrupts are disabled
 */
static void ubi32_mailbox_start_tx(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	UART_SET_IER(uart, ETBEI);

	ubi32_mailbox_tx_chars(uart);
}

/*
 * Interrupts are enabled
 */
static void ubi32_mailbox_stop_rx(struct uart_port *port)
{
//	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;
	UART_CLEAR_IER(uart, ERBFI);
}

/*
 * Set the modem control timer to fire immediately.
 */
static void ubi32_mailbox_enable_ms(struct uart_port *port)
{
}

static void ubi32_mailbox_rx_chars(struct ubi32_mailbox_port *uart)
{
	struct uart_info *info = uart->port.info;
	struct tty_struct *tty = info->port.tty;
	unsigned int status, ch, flg;

	status = 0; // XXX? UART_GET_LSR(uart);
	UART_CLEAR_LSR(uart);

	ch = UART_GET_CHAR(uart);

	if(ch == 0xff)
		return;

	uart->port.icount.rx++;

	if (status & BI) {
		uart->port.icount.brk++;
		if (uart_handle_break(&uart->port))
			goto ignore_char;
		status &= ~(PE | FE);
	}
	if (status & PE)
		uart->port.icount.parity++;
	if (status & OE)
		uart->port.icount.overrun++;
	if (status & FE)
		uart->port.icount.frame++;

	status &= uart->port.read_status_mask;

	if (status & BI)
		flg = TTY_BREAK;
	else if (status & PE)
		flg = TTY_PARITY;
	else if (status & FE)
		flg = TTY_FRAME;
	else
		flg = TTY_NORMAL;

	if (uart_handle_sysrq_char(&uart->port, ch))
		goto ignore_char;

	uart_insert_char(&uart->port, status, OE, ch, flg);

 ignore_char:
	tty_flip_buffer_push(tty);
}

static void ubi32_mailbox_tx_chars(struct ubi32_mailbox_port *uart)
{
	struct circ_buf *xmit = &uart->port.info->xmit;

	if (uart->port.x_char) {
		UART_PUT_CHAR(uart, uart->port.x_char);
		uart->port.icount.tx++;
		uart->port.x_char = 0;
	}
	/*
	 * Check the modem control lines before
	 * transmitting anything.
	 */
	ubi32_mailbox_mctrl_check(uart);

	if (uart_circ_empty(xmit) || uart_tx_stopped(&uart->port)) {
		ubi32_mailbox_stop_tx(&uart->port);
		return;
	}

	while ((UART_GET_LSR(uart) & THRE) && xmit->tail != xmit->head) {
		UART_PUT_CHAR(uart, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		uart->port.icount.tx++;
		SSYNC();
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&uart->port);

	if (uart_circ_empty(xmit))
		ubi32_mailbox_stop_tx(&uart->port);
}

static irqreturn_t ubi32_mailbox_isr(int irq, void *dev_id)
{
	struct ubi32_mailbox_port *uart = dev_id;

	spin_lock(&uart->port.lock);

	//XXX?while (UART_GET_LSR(uart) & DR)

	/*
	 * RX process
	 */
	while (ubi32_mailbox_get_char_avail()) {
		ubi32_mailbox_rx_chars(uart);
	}

#if 0
	/*
	 * TX process
	 */
	if (this_uart.tx_in == this_uart.tx_out) {
		UBICOM32_IO_PORT(SERIAL_UBICOM_PORT)->int_mask &= ~IO_PORTX_INT_SERDES_TXBE;
	} else if (UBICOM32_IO_PORT(SERIAL_UBICOM_PORT)->int_status & IO_PORTX_INT_SERDES_TXBE) {
		uart_ubicom32_send(this_uart.tx_buf[this_uart.tx_out & (SERIAL_UBICOM_BUF_SIZE - 1)]);
		this_uart.tx_out++;
		UBICOM32_IO_PORT(SERIAL_UBICOM_PORT)->int_mask |= IO_PORTX_INT_SERDES_TXBE;
	}
#endif

	spin_unlock(&uart->port.lock);

	return IRQ_HANDLED;
}
#if 0
static irqreturn_t ubi32_mailbox_tx_int(int irq, void *dev_id)
{
	struct ubi32_mailbox_port *uart = dev_id;

	spin_lock(&uart->port.lock);
	if (UART_GET_LSR(uart) & THRE)
		ubi32_mailbox_tx_chars(uart);
	spin_unlock(&uart->port.lock);

	return IRQ_HANDLED;
}
#endif

/*
 * Return TIOCSER_TEMT when transmitter is not busy.
 */
static unsigned int ubi32_mailbox_tx_empty(struct uart_port *port)
{
//	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;
	unsigned short lsr;

	lsr = UART_GET_LSR(uart);
	if (lsr & TEMT)
		return TIOCSER_TEMT;
	else
		return 0;
}

static unsigned int ubi32_mailbox_get_mctrl(struct uart_port *port)
{
		return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static void ubi32_mailbox_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

/*
 * Handle any change of modem status signal since we were last called.
 */
static void ubi32_mailbox_mctrl_check(struct ubi32_mailbox_port *uart)
{
}

/*
 * Interrupts are always disabled.
 */
static void ubi32_mailbox_break_ctl(struct uart_port *port, int break_state)
{
//	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;
	u16 lcr = UART_GET_LCR(uart);
	if (break_state)
		lcr |= SB;
	else
		lcr &= ~SB;
	UART_PUT_LCR(uart, lcr);
	SSYNC();
}

static int ubi32_mailbox_startup(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	if (request_irq(uart->port.irq, ubi32_mailbox_isr, IRQF_DISABLED,
	     "UBI32_MAILBOX", uart)) {
		printk(KERN_NOTICE "Unable to attach Ubicom32 SERDES interrupt\n");
		return -EBUSY;
	}

	UART_SET_IER(uart, ERBFI);
	return 0;
}

static void ubi32_mailbox_shutdown(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	free_irq(uart->port.irq, uart);
}

static void
ubi32_mailbox_set_termios(struct uart_port *port, struct ktermios *termios,
		   struct ktermios *old)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;
	unsigned long flags;
	unsigned int baud, quot;
	unsigned short val, ier, lsr, lcr = 0;

	switch (termios->c_cflag & CSIZE) {
	case CS8:
		lcr = WLS(8);
		break;
	case CS7:
		lcr = WLS(7);
		break;
	case CS6:
		lcr = WLS(6);
		break;
	case CS5:
		lcr = WLS(5);
		break;
	default:
		printk(KERN_ERR "%s: word lengh not supported\n",
			__FUNCTION__);
	}

	if (termios->c_cflag & CSTOPB)
		lcr |= STB;
	if (termios->c_cflag & PARENB)
		lcr |= PEN;
	if (!(termios->c_cflag & PARODD))
		lcr |= EPS;
	if (termios->c_cflag & CMSPAR)
		lcr |= STP;

	port->read_status_mask = OE;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= (FE | PE);
	if (termios->c_iflag & (BRKINT | PARMRK))
		port->read_status_mask |= BI;

	/*
	 * Characters to ignore
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= FE | PE;
	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= BI;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= OE;
	}

	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk/16);
	quot = uart_get_divisor(port, baud);
	spin_lock_irqsave(&uart->port.lock, flags);

	do {
		lsr = UART_GET_LSR(uart);
	} while (!(lsr & TEMT));

	/* Disable UART */
	ier = UART_GET_IER(uart);
	UART_CLEAR_IER(uart, 0xF);

	UART_PUT_DLL(uart, quot & 0xFF);
	SSYNC();
	UART_PUT_DLH(uart, (quot >> 8) & 0xFF);
	SSYNC();

	UART_PUT_LCR(uart, lcr);

	/* Enable UART */
	UART_SET_IER(uart, ier);

	val = UART_GET_GCTL(uart);
	val |= UCEN;
	UART_PUT_GCTL(uart, val);

	spin_unlock_irqrestore(&uart->port.lock, flags);
}

static const char *ubi32_mailbox_type(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	return uart->port.type == PORT_UBI32_MAILBOX ? "UBI32_MAILBOX" : NULL;
}

/*
 * Release the memory region(s) being used by 'port'.
 */
static void ubi32_mailbox_release_port(struct uart_port *port)
{
}

/*
 * Request the memory region(s) being used by 'port'.
 */
static int ubi32_mailbox_request_port(struct uart_port *port)
{
	return 0;
}

/*
 * Configure/autoconfigure the port.
 */
static void ubi32_mailbox_config_port(struct uart_port *port, int flags)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	if (flags & UART_CONFIG_TYPE && ubi32_mailbox_request_port(&uart->port) == 0)
		uart->port.type = PORT_UBI32_MAILBOX;
}

/*
 * Verify the new serial_struct (for TIOCSSERIAL).
 * The only change we allow are to the flags and type, and
 * even then only between PORT_UBI32_MAILBOX and PORT_UNKNOWN
 */
static int
ubi32_mailbox_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static struct uart_ops ubi32_mailbox_pops = {
	.tx_empty	= ubi32_mailbox_tx_empty,
	.set_mctrl	= ubi32_mailbox_set_mctrl,
	.get_mctrl	= ubi32_mailbox_get_mctrl,
	.stop_tx	= ubi32_mailbox_stop_tx,
	.start_tx	= ubi32_mailbox_start_tx,
	.stop_rx	= ubi32_mailbox_stop_rx,
	.enable_ms	= ubi32_mailbox_enable_ms,
	.break_ctl	= ubi32_mailbox_break_ctl,
	.startup	= ubi32_mailbox_startup,
	.shutdown	= ubi32_mailbox_shutdown,
	.set_termios	= ubi32_mailbox_set_termios,
	.type		= ubi32_mailbox_type,
	.release_port	= ubi32_mailbox_release_port,
	.request_port	= ubi32_mailbox_request_port,
	.config_port	= ubi32_mailbox_config_port,
	.verify_port	= ubi32_mailbox_verify_port,
};

static void __init ubi32_mailbox_init_ports(void)
{
	static int first = 1;
	int i;

	if (!first)
		return;
	first = 0;

	for (i = 0; i < NR_PORTS; i++) {
		ubi32_mailbox_ports[i].port.uartclk   = get_sclk();
		ubi32_mailbox_ports[i].port.ops       = &ubi32_mailbox_pops;
		ubi32_mailbox_ports[i].port.line      = i;
		ubi32_mailbox_ports[i].port.iotype    = UPIO_MEM;
		ubi32_mailbox_ports[i].port.membase   =
			(void __iomem *)ubi32_mailbox_resource[i].uart_base_addr;
		ubi32_mailbox_ports[i].port.mapbase   =
			ubi32_mailbox_resource[i].uart_base_addr;
		ubi32_mailbox_ports[i].port.irq       =
			ubi32_mailbox_resource[i].uart_irq;
		ubi32_mailbox_ports[i].port.flags     = UPF_BOOT_AUTOCONF;
		spin_lock_init(&ubi32_mailbox_ports[i].port.lock);

		ubi32_mailbox_hw_init(&ubi32_mailbox_ports[i]);
	}

}

#ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
/*
 * If the port was already initialised (eg, by a boot loader),
 * try to determine the current setup.
 */
static void __init
ubi32_mailbox_console_get_options(struct ubi32_mailbox_port *uart, int *baud,
			   int *parity, int *bits)
{
	unsigned short status;

	status = UART_GET_IER(uart) & (ERBFI | ETBEI);
	if (status == (ERBFI | ETBEI)) {
		/* ok, the port was enabled */
		unsigned short lcr;
		unsigned short dlh, dll;

		lcr = UART_GET_LCR(uart);

		*parity = 'n';
		if (lcr & PEN) {
			if (lcr & EPS)
				*parity = 'e';
			else
				*parity = 'o';
		}
		switch (lcr & 0x03) {
			case 0:	*bits = 5; break;
			case 1:	*bits = 6; break;
			case 2:	*bits = 7; break;
			case 3:	*bits = 8; break;
		}

		dll = UART_GET_DLL(uart);
		dlh = UART_GET_DLH(uart);

		*baud = get_sclk() / (16*(dll | dlh << 8));
	}
	pr_debug("%s:baud = %d, parity = %c, bits= %d\n", __FUNCTION__, *baud, *parity, *bits);
}
#endif

#if defined(CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE) || defined(CONFIG_EARLY_PRINTK)
static struct uart_driver ubi32_mailbox_reg;

static int __init
ubi32_mailbox_console_setup(struct console *co, char *options)
{
	struct ubi32_mailbox_port *uart;
# ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
	int baud = SERIAL_UBICOM_BAUDRATE;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';
# endif

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index == -1 || co->index >= NR_PORTS)
		co->index = 0;
	uart = &ubi32_mailbox_ports[co->index];

# ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	else
		ubi32_mailbox_console_get_options(uart, &baud, &parity, &bits);

	//JB return uart_set_options(&uart->port, co, baud, parity, bits, flow);
	return 0;
# else
	return 0;
# endif
}
#endif /* defined (CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE) ||
				 defined (CONFIG_EARLY_PRINTK) */

#ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
static void ubi32_mailbox_console_putchar(struct uart_port *port, int ch)
{
//	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;
	while (!(UART_GET_LSR(uart) & THRE))
		barrier();
	UART_PUT_CHAR(uart, ch);
	SSYNC();
}

/*
 * Interrupts are disabled on entering
 */
static void
ubi32_mailbox_console_write(struct console *co, const char *s, unsigned int count)
{
	struct ubi32_mailbox_port *uart = &ubi32_mailbox_ports[co->index];
	unsigned long flags = 0;

	spin_lock_irqsave(&uart->port.lock, flags);
	uart_console_write(&uart->port, s, count, ubi32_mailbox_console_putchar);
	spin_unlock_irqrestore(&uart->port.lock, flags);

}

static struct console ubi32_mailbox_console = {
	.name		= UBI32_MAILBOX_NAME,
	.write		= ubi32_mailbox_console_write,
	.device		= uart_console_device,
	.setup		= ubi32_mailbox_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &ubi32_mailbox_reg,
};

static int __init ubi32_mailbox_console_init(void)
{
	ubi32_mailbox_init_ports();
	register_console(&ubi32_mailbox_console);
	return 0;
}
console_initcall(ubi32_mailbox_console_init);

#define UBI32_MAILBOX_CONSOLE	&ubi32_mailbox_console
#else
#define UBI32_MAILBOX_CONSOLE	NULL
#endif /* CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE */


#ifdef CONFIG_EARLY_PRINTK
static __init void ubi32_mailbox_early_putc(struct uart_port *port, int ch)
{
	UART_PUT_CHAR(uart, ch);
}

static __init void ubi32_mailbox_early_write(struct console *con, const char *s,
					unsigned int n)
{
	struct ubi32_mailbox_port *uart = &ubi32_mailbox_ports[con->index];
	unsigned int i;

	for (i = 0; i < n; i++, s++) {
		if (*s == '\n')
			ubi32_mailbox_early_putc(&uart->port, '\r');
		ubi32_mailbox_early_putc(&uart->port, *s);
	}
}

static struct __init console ubi32_mailbox_early_console = {
	.name = "early_UM",
	.write = ubi32_mailbox_early_write,
	.device = uart_console_device,
	.flags = CON_PRINTBUFFER,
	.setup = ubi32_mailbox_console_setup,
	.index = -1,
	.data  = &ubi32_mailbox_reg,
};

/*
 * XXX Unused in our driver. Need to find out what the termios initialization is good/needed for.
 */
struct console __init *ubi32_mailbox_early_init(unsigned int port,
						unsigned int cflag)
{
	struct ubi32_mailbox_port *uart;
	struct ktermios t;

	if (port == -1 || port >= NR_PORTS)
		port = 0;
	ubi32_mailbox_init_ports();
	ubi32_mailbox_early_console.index = port;
	uart = &ubi32_mailbox_ports[port];
	t.c_cflag = cflag;
	t.c_iflag = 0;
	t.c_oflag = 0;
	t.c_lflag = ICANON;
	t.c_line = port;
	ubi32_mailbox_set_termios(&uart->port, &t, &t);
	return &ubi32_mailbox_early_console;
}

#endif /* CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE */

static struct uart_driver ubi32_mailbox_reg = {
	.owner			= THIS_MODULE,
	.driver_name		= "ubi32_mailbox",
	.dev_name		= UBI32_MAILBOX_NAME,
	.major			= UBI32_MAILBOX_MAJOR,
	.minor			= UBI32_MAILBOX_MINOR,
	.nr			= NR_PORTS,
	.cons			= UBI32_MAILBOX_CONSOLE,
};

static int ubi32_mailbox_suspend(struct platform_device *dev, pm_message_t state)
{
	struct ubi32_mailbox_port *uart = platform_get_drvdata(dev);

	if (uart)
		uart_suspend_port(&ubi32_mailbox_reg, &uart->port);

	return 0;
}

static int ubi32_mailbox_resume(struct platform_device *dev)
{
	struct ubi32_mailbox_port *uart = platform_get_drvdata(dev);

	if (uart)
		uart_resume_port(&ubi32_mailbox_reg, &uart->port);

	return 0;
}

static int ubi32_mailbox_probe(struct platform_device *dev)
{
	struct resource *res = dev->resource;
	int i;

	for (i = 0; i < dev->num_resources; i++, res++)
		if (res->flags & IORESOURCE_MEM)
			break;

	if (i < dev->num_resources) {
		for (i = 0; i < NR_PORTS; i++, res++) {
			if (ubi32_mailbox_ports[i].port.mapbase != res->start)
				continue;
			ubi32_mailbox_ports[i].port.dev = &dev->dev;
			uart_add_one_port(&ubi32_mailbox_reg, &ubi32_mailbox_ports[i].port);
			platform_set_drvdata(dev, &ubi32_mailbox_ports[i]);
		}
	}

	return 0;
}

static int ubi32_mailbox_remove(struct platform_device *pdev)
{
	struct ubi32_mailbox_port *uart = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	if (uart)
		uart_remove_one_port(&ubi32_mailbox_reg, &uart->port);

	return 0;
}

static struct platform_driver ubi32_mailbox_driver = {
	.probe		= ubi32_mailbox_probe,
	.remove		= ubi32_mailbox_remove,
	.suspend	= ubi32_mailbox_suspend,
	.resume		= ubi32_mailbox_resume,
	.driver		= {
		.name	= "ubi32-mbox",
		.owner	= THIS_MODULE,
	},
};

static int __init ubi32_mailbox_init(void)
{
	int ret;

	pr_info("Serial: Ubicom32 mailbox serial driver.\n");

	mailbox_console_flg = TRUE;
	num_timeouts = 0;
	ubi32_mailbox_init_ports();

	ret = uart_register_driver(&ubi32_mailbox_reg);
	if (ret == 0) {
		ret = platform_driver_register(&ubi32_mailbox_driver);
		if (ret) {
			pr_debug("uart register failed\n");
			uart_unregister_driver(&ubi32_mailbox_reg);
		}
	}

	/*
	 * XXX HACK: currently probe does not get called, but the port needs to be added to work.
	 */
	uart_add_one_port(&ubi32_mailbox_reg, &ubi32_mailbox_ports[0].port);
	return ret;
}

static void __exit ubi32_mailbox_exit(void)
{
	platform_driver_unregister(&ubi32_mailbox_driver);
	uart_unregister_driver(&ubi32_mailbox_reg);
}

module_init(ubi32_mailbox_init);
module_exit(ubi32_mailbox_exit);

MODULE_ALIAS_CHARDEV_MAJOR(UBI32_MAILBOX_MAJOR);
MODULE_ALIAS("platform:ubi32_mailbox");
