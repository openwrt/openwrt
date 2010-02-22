/*
 * drivers/serial/ubi32_serdes.c
 *   Ubicom32 On-Chip Serial Driver
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
#include <asm/ubicom32suart.h>


#define SERIAL_UBICOM_PIN_RXD	(1 << 0)
#define SERIAL_UBICOM_PIN_TXD	(1 << 6)
#define SERIAL_UBICOM_CTL0	0x8b300000
#define SERIAL_UBICOM_CTL1	0x00000009

#define SERIAL_UBICOM_DATA_BIT	8	/* Fixed parameter - do not change */
#define SERIAL_UBICOM_PAR_BIT	0	/* Fixed parameter - do not change */
#define SERIAL_UBICOM_STOP_BIT	1	/* Fixed parameter - do not change */

/* UART name and device definitions */
#define UBI32_SERDES_NAME	"ttyUS" // XXX
#define UBI32_SERDES_MAJOR	206 // XXX
#define UBI32_SERDES_MINOR	64 // XXX

#define PORT_UBI32_SERDES	1234
#define NR_PORTS 1

struct uart_port ubi32_serdes_ports[NR_PORTS];

struct ubi32_serdes_resource {
	void *uart_base_addr;
	int uart_irq;
	int uart_clock;
} ubi32_serdes_resource[NR_PORTS] = {
	/*
	 * Get params from kernel command line (required for early printk)
	 * or from platform resources.
	 */
	{0, 0, 0}
};

/*
 * Can get overridden by 'serdes=' kernel command line.
 */
static int ubi32_serdes_default_baud_rate = 115200;


#define IO_PORT(port) ((struct ubicom32_io_port *)port->membase)
#define IO_PORT_INT_STATUS(port) (IO_PORT(port)->int_status)
#define IO_PORT_INT_MASK(port) (IO_PORT(port)->int_mask)
#define IO_PORT_INT_CLR(port) (IO_PORT(port)->int_clr)


/*
 * ubi32_serdes_get_char()
 */
static u8_t ubi32_serdes_get_char(struct ubicom32_io_port *io_port)
{
	/*
	 * Read from hardware (forced 32-bit atomic read).
	 */
	u32_t data = 0;

	if ( io_port ) {
		io_port->int_clr = IO_PORTX_INT_SERDES_RXBF;
		asm volatile (
			"move.4		%0, %1		\n\t"
			: "=r" (data)
			: "m" (*(u32_t *)&(io_port->rx_fifo))
			);
	}

	return (u8_t)(data & 0x000000ff);
}

/*
 * ubi32_serdes_put_char()
 */
static void ubi32_serdes_put_char(struct ubicom32_io_port *io_port, u8_t c)
{
	u32_t data = 0x0000fe00 | (c << 1);

	if ( io_port ) {
		/*
		 * Fixed data format:
		 * [LSB]1 start bit - 8 data bits - no parity - 1 stop bit[MSB]
		 */
		io_port->int_clr = IO_PORTX_INT_SERDES_TXBE;
		io_port->ctl2 = data;
		io_port->int_set = IO_PORTX_INT_SERDES_TXBUF_VALID;
	}
}

static void ubi32_serdes_hw_init(struct uart_port *port, int baud)
{
	struct ubicom32_io_port *io_port = IO_PORT(port);

	if ( io_port ) {
		/*
		 * Put port functions 1-4 into reset state.
		 * Function 0 (GPIO) does not need or have a reset bit.
		 *
		 * Select SERDES function for restart below.
		 */
		io_port->function =
			IO_FUNC_FUNCTION_RESET(1) | IO_FUNC_FUNCTION_RESET(2) |
			IO_FUNC_FUNCTION_RESET(3) | IO_FUNC_FUNCTION_RESET(4) |
			IO_PORTX_FUNC_SERDES;

		/*
		 * Configure SERDES baudrate
		 */
		if ( baud == 0 ) {
			baud = ubi32_serdes_default_baud_rate;
		}

		io_port->ctl0 =
			SERIAL_UBICOM_CTL0 |
			((port->uartclk / (16 * baud)) - 1);

		io_port->ctl1 =
			SERIAL_UBICOM_CTL1;

		/*
		 * don't interrupt until startup and start_tx
		 */
		io_port->int_mask = 0;

		/*
		 * Set TXD pin output, RXD input and prevent GPIO
		 * override on the TXD & RXD pins
		 */
		io_port->gpio_ctl &= ~SERIAL_UBICOM_PIN_RXD;
		io_port->gpio_ctl |= SERIAL_UBICOM_PIN_TXD;
		io_port->gpio_mask &= ~(SERIAL_UBICOM_PIN_RXD | SERIAL_UBICOM_PIN_TXD);

		/*
		 * Restart (un-reset) the port's SERDES function.
		 */
		io_port->function &= ~(IO_FUNC_FUNCTION_RESET(IO_PORTX_FUNC_SERDES));
	}
}

#define ULITE_STATUS_RXVALID IO_PORTX_INT_SERDES_RXBF
#define ULITE_STATUS_OVERRUN 0
#define ULITE_STATUS_FRAME 0
#define ULITE_STATUS_PARITY 0
#define ULITE_STATUS_TXEMPTY IO_PORTX_INT_SERDES_TXBE
#define ULITE_STATUS_TXFULL 0

static int ubi32_serdes_receive(struct uart_port *port, int stat)
{
	struct tty_struct *tty = port->info->port.tty;
	unsigned char ch = 0;
	char flag = TTY_NORMAL;

	if ((stat & (ULITE_STATUS_RXVALID | ULITE_STATUS_OVERRUN
		     | ULITE_STATUS_FRAME)) == 0)
		return 0;

	/* stats */
	if (stat & ULITE_STATUS_RXVALID) {
		port->icount.rx++;
		ch = ubi32_serdes_get_char((struct ubicom32_io_port *)port->membase);

		if (stat & ULITE_STATUS_PARITY)
			port->icount.parity++;
	}

	if (stat & ULITE_STATUS_OVERRUN)
		port->icount.overrun++;

	if (stat & ULITE_STATUS_FRAME)
		port->icount.frame++;


	/* drop byte with parity error if IGNPAR specificed */
	if (stat & port->ignore_status_mask & ULITE_STATUS_PARITY)
		stat &= ~ULITE_STATUS_RXVALID;

	stat &= port->read_status_mask;

	if (stat & ULITE_STATUS_PARITY)
		flag = TTY_PARITY;

	stat &= ~port->ignore_status_mask;

	if (stat & ULITE_STATUS_RXVALID)
		tty_insert_flip_char(tty, ch, flag);

	if (stat & ULITE_STATUS_FRAME)
		tty_insert_flip_char(tty, 0, TTY_FRAME);

	if (stat & ULITE_STATUS_OVERRUN)
		tty_insert_flip_char(tty, 0, TTY_OVERRUN);

	return 1;
}

/*
 * interrupts are disabled on entry
 */
static void ubi32_serdes_stop_tx(struct uart_port *port)
{
	IO_PORT_INT_MASK(port) = IO_PORT_INT_MASK(port) & ~IO_PORTX_INT_SERDES_TXBE;
}

static int ubi32_serdes_transmit(struct uart_port *port, int stat)
{
	struct circ_buf *xmit  = &port->info->xmit;

	if (!(stat & IO_PORTX_INT_SERDES_TXBE))
		return 0;

	if (port->x_char) {
		ubi32_serdes_put_char(IO_PORT(port), port->x_char);
		port->x_char = 0;
		port->icount.tx++;
		return 1;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(port)) {
		ubi32_serdes_stop_tx(port);
		return 0;
	}

	ubi32_serdes_put_char(IO_PORT(port), xmit->buf[xmit->tail]);
	xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE-1);
	port->icount.tx++;

	/* wake up */
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	if (uart_circ_empty(xmit))
		ubi32_serdes_stop_tx(port);

	return 1;
}

/*
 * port is locked and interrupts are disabled
 */
static void ubi32_serdes_start_tx(struct uart_port *port)
{
	IO_PORT_INT_MASK(port) = IO_PORT_INT_MASK(port) | IO_PORTX_INT_SERDES_TXBE;
	ubi32_serdes_transmit(port, IO_PORT_INT_STATUS(port));
}

/*
 * Interrupts are enabled
 */
static void ubi32_serdes_stop_rx(struct uart_port *port)
{
	/* don't forward any more data (like !CREAD) */
	port->ignore_status_mask = IO_PORTX_INT_SERDES_RXBF;
}

/*
 * Set the modem control timer to fire immediately.
 */
static void ubi32_serdes_enable_ms(struct uart_port *port)
{
	/* N/A */
}

static irqreturn_t ubi32_serdes_isr(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	int busy;

	spin_lock(&port->lock);

	do {
		int stat = IO_PORT_INT_STATUS(port);
		busy  = ubi32_serdes_receive(port, stat);
		busy |= ubi32_serdes_transmit(port, stat);
	} while (busy);

	tty_flip_buffer_push(port->info->port.tty);

	spin_unlock(&port->lock);

	return IRQ_HANDLED;
}

/*
 * Return TIOCSER_TEMT when transmitter is not busy.
 */
static unsigned int ubi32_serdes_tx_empty(struct uart_port *port)
{
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&port->lock, flags);
	ret = IO_PORT_INT_STATUS(port);
	spin_unlock_irqrestore(&port->lock, flags);

	return ret & ULITE_STATUS_TXEMPTY ? TIOCSER_TEMT : 0;
}

static unsigned int ubi32_serdes_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static void ubi32_serdes_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	/* N/A */
}

/*
 * Interrupts are always disabled.
 */
static void ubi32_serdes_break_ctl(struct uart_port *port, int break_state)
{
	/* N/A */
}

static int ubi32_serdes_startup(struct uart_port *port)
{
	if (request_irq(port->irq, ubi32_serdes_isr, IRQF_DISABLED,
	     "UBI32_SERDES", port)) {
		printk(KERN_NOTICE "Unable to attach port interrupt\n");
		return -EBUSY;
	}

	IO_PORT_INT_CLR(port) = IO_PORTX_INT_SERDES_RXBF;
	IO_PORT_INT_MASK(port) = IO_PORTX_INT_SERDES_RXBF;
	return 0;
}

static void ubi32_serdes_shutdown(struct uart_port *port)
{
	struct ubi32_serdes_port *uart = (struct ubi32_serdes_port *)port;

	IO_PORT_INT_MASK(port) = 0;
	free_irq(port->irq, uart);
}

static void
ubi32_serdes_set_termios(struct uart_port *port, struct ktermios *termios,
		   struct ktermios *old)
{
	unsigned long flags;
	unsigned int baud;

	spin_lock_irqsave(&port->lock, flags);

	port->read_status_mask = ULITE_STATUS_RXVALID | ULITE_STATUS_OVERRUN
		| ULITE_STATUS_TXFULL;

	if (termios->c_iflag & INPCK)
		port->read_status_mask |=
			ULITE_STATUS_PARITY | ULITE_STATUS_FRAME;

	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= ULITE_STATUS_PARITY
			| ULITE_STATUS_FRAME | ULITE_STATUS_OVERRUN;

	/* ignore all characters if CREAD is not set */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |=
			ULITE_STATUS_RXVALID | ULITE_STATUS_PARITY
			| ULITE_STATUS_FRAME | ULITE_STATUS_OVERRUN;

	/* update timeout */
	baud = uart_get_baud_rate(port, termios, old, 0, 460800);
	uart_update_timeout(port, termios->c_cflag, baud);

	IO_PORT(port)->ctl0 = SERIAL_UBICOM_CTL0 |
			((port->uartclk / (16 * baud)) - 1);

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *ubi32_serdes_type(struct uart_port *port)
{
	return port->type == PORT_UBI32_SERDES ? "UBI32_SERDES" : NULL;
}

/*
 * Release the memory region(s) being used by 'port'.
 */
static void ubi32_serdes_release_port(struct uart_port *port)
{
}

/*
 * Request the memory region(s) being used by 'port'.
 */
static int ubi32_serdes_request_port(struct uart_port *port)
{
	return 0;
}

/*
 * Configure/autoconfigure the port.
 */
static void ubi32_serdes_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE &&
	    ubi32_serdes_request_port(port) == 0)
		port->type = PORT_UBI32_SERDES;
}

/*
 * Verify the new serial_struct (for TIOCSSERIAL).
 * The only change we allow are to the flags and type, and
 * even then only between PORT_UBI32_SERDES and PORT_UNKNOWN
 */
static int
ubi32_serdes_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static struct uart_ops ubi32_serdes_pops = {
	.tx_empty	= ubi32_serdes_tx_empty,
	.set_mctrl	= ubi32_serdes_set_mctrl,
	.get_mctrl	= ubi32_serdes_get_mctrl,
	.stop_tx	= ubi32_serdes_stop_tx,
	.start_tx	= ubi32_serdes_start_tx,
	.stop_rx	= ubi32_serdes_stop_rx,
	.enable_ms	= ubi32_serdes_enable_ms,
	.break_ctl	= ubi32_serdes_break_ctl,
	.startup	= ubi32_serdes_startup,
	.shutdown	= ubi32_serdes_shutdown,
	.set_termios	= ubi32_serdes_set_termios,
	.type		= ubi32_serdes_type,
	.release_port	= ubi32_serdes_release_port,
	.request_port	= ubi32_serdes_request_port,
	.config_port	= ubi32_serdes_config_port,
	.verify_port	= ubi32_serdes_verify_port,
};

static void __init ubi32_serdes_init_ports(void)
{
	int i;

	for (i = 0; i < NR_PORTS; i++) {
		ubi32_serdes_ports[i].uartclk   = ubi32_serdes_resource[i].uart_clock;
		ubi32_serdes_ports[i].ops       = &ubi32_serdes_pops;
		ubi32_serdes_ports[i].line      = i;
		ubi32_serdes_ports[i].iotype    = UPIO_MEM;
		ubi32_serdes_ports[i].membase   =
			(void __iomem *)ubi32_serdes_resource[i].uart_base_addr;
		ubi32_serdes_ports[i].mapbase   =
			(resource_size_t)ubi32_serdes_resource[i].uart_base_addr;
		ubi32_serdes_ports[i].irq       =
			ubi32_serdes_resource[i].uart_irq;
		ubi32_serdes_ports[i].flags     = UPF_BOOT_AUTOCONF;

		ubi32_serdes_hw_init(&ubi32_serdes_ports[i], 0);
	}

}

#ifdef CONFIG_SERIAL_UBI32_SERDES_CONSOLE
/*
 * If the port was already initialised (eg, by a boot loader),
 * try to determine the current setup.
 */
static void __init
ubi32_serdes_console_get_options(struct uart_port *port, int *baud)
{
	u32 round_to = 1200;
	u32 real_baud;

	/*
	 * We might get called before platform init and with no
	 * kernel command line options, so port might be NULL.
	 */
	*baud = ubi32_serdes_default_baud_rate;;
	if ( IO_PORT(port) == 0 )
		return;

	real_baud = port->uartclk
		/ (16 * ((IO_PORT(port)->ctl0 & ~SERIAL_UBICOM_CTL0) + 1));

	*baud = ((real_baud + round_to - 1) / round_to) * round_to;

	pr_debug("%s:baud = %d, real_baud = %d\n", __FUNCTION__, *baud, real_baud);
}
#endif

#if defined(CONFIG_SERIAL_UBI32_SERDES_CONSOLE) || defined(CONFIG_EARLY_PRINTK)
static struct uart_driver ubi32_serdes_reg;

static int __init
ubi32_serdes_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
#ifdef CONFIG_SERIAL_UBI32_SERDES_CONSOLE
	int baud = ubi32_serdes_default_baud_rate;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';
#endif

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index == -1 || co->index >= NR_PORTS)
		co->index = 0;
	port = &ubi32_serdes_ports[co->index];

#ifdef CONFIG_SERIAL_UBI32_SERDES_CONSOLE
	if (options) {
		uart_parse_options(options, &baud, &parity, &bits, &flow);
		ubi32_serdes_hw_init(port, baud);
	}
	else
		ubi32_serdes_console_get_options(port, &baud);

	return uart_set_options(port, co, baud, parity, bits, flow);
#else
	return 0;
#endif
}
#endif /* defined (CONFIG_SERIAL_UBI32_SERDES_CONSOLE) ||
				 defined (CONFIG_EARLY_PRINTK) */

#ifdef CONFIG_SERIAL_UBI32_SERDES_CONSOLE
static void
ubi32_serdes_console_putchar(struct uart_port *port, int ch)
{
	if ( IO_PORT(port) ) {
		while (!(IO_PORT_INT_STATUS(port) & IO_PORTX_INT_SERDES_TXBE))
			barrier();
		ubi32_serdes_put_char(IO_PORT(port), ch);
	}
}

/*
 * Interrupts are disabled on entering
 */
static void
ubi32_serdes_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_port *port = &ubi32_serdes_ports[co->index];
	unsigned long flags = 0;

	spin_lock_irqsave(&port->lock, flags);
	uart_console_write(port, s, count, ubi32_serdes_console_putchar);
	spin_unlock_irqrestore(&port->lock, flags);

}

static struct console ubi32_serdes_console = {
	.name		= UBI32_SERDES_NAME,
	.write		= ubi32_serdes_console_write,
	.device		= uart_console_device,
	.setup		= ubi32_serdes_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &ubi32_serdes_reg,
};

static int __init ubi32_serdes_console_init(void)
{
	ubi32_serdes_init_ports();
	register_console(&ubi32_serdes_console);
	return 0;
}
console_initcall(ubi32_serdes_console_init);

#define UBI32_SERDES_CONSOLE	&ubi32_serdes_console
#else
#define UBI32_SERDES_CONSOLE	NULL
#endif /* CONFIG_SERIAL_UBI32_SERDES_CONSOLE */


#ifdef CONFIG_EARLY_PRINTK
static __init void ubi32_serdes_early_putc(struct uart_port *port, int ch)
{
	unsigned timeout = 0xffff;

	while ((!(IO_PORT_INT_STATUS(port) & IO_PORTX_INT_SERDES_TXBE)) && --timeout)
		cpu_relax();
	ubi32_serdes_put_char(IO_PORT(port), ch);
}

static __init void ubi32_serdes_early_write(struct console *con, const char *s,
					unsigned int n)
{
	struct uart_port *port = &ubi32_serdes_ports[con->index];
	unsigned int i;

	for (i = 0; i < n; i++, s++) {
		if (*s == '\n')
			ubi32_serdes_early_putc(port, '\r');
		ubi32_serdes_early_putc(port, *s);
	}
}

static struct __init console ubi32_serdes_early_console = {
	.name = "early_US",
	.write = ubi32_serdes_early_write,
	.device = uart_console_device,
	.flags = CON_PRINTBUFFER,
	.setup = ubi32_serdes_console_setup,
	.index = -1,
	.data  = &ubi32_serdes_reg,
};

/*
 * XXX Unused in our driver. Need to find out what the termios initialization is good/needed for.
 */
struct console __init *ubi32_serdes_early_init(unsigned int port_index,
						unsigned int cflag)
{
	struct uart_port *uart;
	struct ktermios t;

	if (port_index == -1 || port_index >= NR_PORTS)
		port_index = 0;
	ubi32_serdes_init_ports();
	ubi32_serdes_early_console.index = port_index;
	uart = &ubi32_serdes_ports[port_index];
	t.c_cflag = cflag;
	t.c_iflag = 0;
	t.c_oflag = 0;
	t.c_lflag = ICANON;
	t.c_line = port_index;
	ubi32_serdes_set_termios(uart, &t, &t);
	return &ubi32_serdes_early_console;
}

#endif /* CONFIG_SERIAL_UBI32_SERDES_CONSOLE */

static struct uart_driver ubi32_serdes_reg = {
	.owner			= THIS_MODULE,
	.driver_name		= "ubi32_serdes",
	.dev_name		= UBI32_SERDES_NAME,
	.major			= UBI32_SERDES_MAJOR,
	.minor			= UBI32_SERDES_MINOR,
	.nr			= NR_PORTS,
	.cons			= UBI32_SERDES_CONSOLE,
};

static int ubi32_serdes_suspend(struct platform_device *dev, pm_message_t state)
{
	struct uart_port *port = platform_get_drvdata(dev);

	if (port)
		uart_suspend_port(&ubi32_serdes_reg, port);

	return 0;
}

static int ubi32_serdes_resume(struct platform_device *dev)
{
	struct uart_port *port = platform_get_drvdata(dev);

	if (port)
		uart_resume_port(&ubi32_serdes_reg, port);

	return 0;
}

static int ubi32_serdes_probe(struct platform_device *dev)
{
	struct resource *res = dev->resource;
	int i;

	for (i = 0; i < dev->num_resources; i++, res++) {
		if (res->flags & IORESOURCE_MEM) {
			ubi32_serdes_resource[0].uart_base_addr = (void *) res->start;
		}
		else if (res->flags & IORESOURCE_IRQ) {
			ubi32_serdes_resource[0].uart_irq = res->start;
		}
		else if (res->flags & UBICOM32_SUART_IORESOURCE_CLOCK) {
			ubi32_serdes_resource[0].uart_clock = res->start;
		}
	}

	ubi32_serdes_init_ports();

	return 0;
}

static int ubi32_serdes_remove(struct platform_device *pdev)
{
	struct uart_port *port = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	if (port)
		uart_remove_one_port(&ubi32_serdes_reg, port);

	return 0;
}

static struct platform_driver ubi32_serdes_driver = {
	.remove		= ubi32_serdes_remove,
	.suspend	= ubi32_serdes_suspend,
	.resume		= ubi32_serdes_resume,
	.driver		= {
		.name	= "ubicom32suart",
		.owner	= THIS_MODULE,
	},
};


#ifndef MODULE
/*
 * Called at boot time.
 *
 * You can specify IO base, IRQ, and clock for the serdes serial port
 * using kernel command line "serdes=0xiobase,irq,clock".  Values
 * specified will be overwritten by platform device data, if present.
 */
static int __init ubi32_serdes_setup(char *str)
{
#define N_PARMS   (4+1)
	int ints[N_PARMS];
	int i;

	str = get_options(str, ARRAY_SIZE(ints), ints);

	for (i = 0; i < N_PARMS; i++) {
		if (i < ints[0]) {
			if (i == 0) {
				ubi32_serdes_resource[0].uart_base_addr = (void *) ints[i+1];
			}
			else if (i == 1) {
				ubi32_serdes_resource[0].uart_irq = ints[i+1];
			}
			else if (i == 2) {
				ubi32_serdes_resource[0].uart_clock = ints[i+1];
			}
			else if (i == 3) {
				ubi32_serdes_default_baud_rate = ints[i+1];
			}
		}
	}
	return 1;
}

__setup("serdes=", ubi32_serdes_setup);
#endif

static int __init ubi32_serdes_init(void)
{
	int ret;

	pr_info("Serial: Ubicom32 serdes uart serial driver\n");

	ret = platform_driver_probe(&ubi32_serdes_driver, ubi32_serdes_probe);
	if (ret != 0) {
		printk(KERN_INFO "serdes platform_driver_probe() failed: %d\n", ret);
		return ret;
	}

	ubi32_serdes_init_ports();

	ret = uart_register_driver(&ubi32_serdes_reg);
	if ( ret == 0 ) {
		ret = uart_add_one_port(&ubi32_serdes_reg, &ubi32_serdes_ports[0]);
		if ( ret != 0 ) {
			uart_unregister_driver(&ubi32_serdes_reg);
		}
	}

	return ret;
}

static void __exit ubi32_serdes_exit(void)
{
	platform_driver_unregister(&ubi32_serdes_driver);
	uart_unregister_driver(&ubi32_serdes_reg);
}

module_init(ubi32_serdes_init);
module_exit(ubi32_serdes_exit);

MODULE_AUTHOR("Rainer Keller <rkeller@ubicom.com>");
MODULE_DESCRIPTION("Ubicom generic serial port driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_CHARDEV_MAJOR(UBI32_SERDES_MAJOR);
MODULE_ALIAS("platform:ubi32_serdes");
