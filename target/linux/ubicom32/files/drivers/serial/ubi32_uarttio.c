/*
 * drivers/serial/ubi32_uarttio.c
 *   Ubicom32 Serial Virtual Peripherial Driver
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
#include <asm/gpio.h>
#include <asm/thread.h>
#include <asm/uart_tio.h>

#define DRIVER_NAME	"ubi32_uarttio"

/*
 * For storing the module parameters.
 */
#define UBI32_UARTTIO_MAX_PARAM_LEN	80
static char utio_ports_param[UBI32_UARTTIO_MAX_PARAM_LEN];

/*
 * UART name and device definitions
 */
#define UBI32_UARTTIO_NAME	"ttyUV" // XXX
#define UBI32_UARTTIO_MAJOR	206 // XXX
#define UBI32_UARTTIO_MINOR	64 // XXX

/*
 * The following structures are allocated statically because the
 * memory allocation subsystem is not initialized this early on
 */

/*
 * Per port structure
 */
struct ubi32_uarttio_port {
	struct uarttio_uart		*uart;
	unsigned int			tx_pin;
	unsigned int			rx_pin;

	struct uart_port		port;

	u8_t				added;

	/*
	 * If this value is set, the port has had its direction set already
	 */
	u8_t				port_init;
};
static struct ubi32_uarttio_port uarttio_ports[CONFIG_SERIAL_UBI32_UARTTIO_NR_UARTS];

/*
 * Number of ports currently initialized
 */
static int uarttio_nports;

/*
 * Per device structure
 */
struct ubi32_uarttio_instance {
	struct uarttio_regs		*regs;
	struct ubi32_uarttio_port	*ports;

	u8_t				irq_requested;
	u8_t				driver_registered;
	u8_t				irq;
};
static struct ubi32_uarttio_instance uarttio_inst;

#ifdef CONFIG_SERIAL_UBI32_UARTTIO_CONSOLE
static struct console ubi32_uarttio_console;
#define UBI32_UARTTIO_CONSOLE	&ubi32_uarttio_console
#else
#define UBI32_UARTTIO_CONSOLE	NULL
#endif

static struct uart_driver ubi32_uarttio_uart_driver = {
	.owner			= THIS_MODULE,
	.driver_name		= DRIVER_NAME,
	.dev_name		= UBI32_UARTTIO_NAME,
	.major			= UBI32_UARTTIO_MAJOR,
	.minor			= UBI32_UARTTIO_MINOR,
	.cons			= UBI32_UARTTIO_CONSOLE,
};

#ifdef UBI32_UARTTIO_UNUSED
/*
 * ubi32_uarttio_get_send_space
 */
static int ubi32_uarttio_get_send_space(struct uarttio_uart *uart)
{
	int count = uart->tx_fifo_head - uart->tx_fifo_tail;
	if (count < 0) {
		count += uart->tx_fifo_size;
	}
	return uart->tx_fifo_size - count;
}
#endif

/*
 * ubi32_uarttio_get_recv_ready
 */
static int ubi32_uarttio_get_recv_ready(struct uarttio_uart *uart)
{
	int count = uart->rx_fifo_head - uart->rx_fifo_tail;
	if (count < 0) {
		count += uart->rx_fifo_size;
	}
	return count;
}

/*
 * ubi32_uarttio_get_char()
 */
static u8_t ubi32_uarttio_get_char(struct uarttio_uart *uart)
{
	/*
	 * Retrieve byte
	 */
	u32_t tail = uart->rx_fifo_tail;
	u8_t data = uart->rx_fifo[tail];

	if (++tail == uart->rx_fifo_size) {
		tail = 0;
	}
	uart->rx_fifo_tail = tail;

	return data;
}

/*
 * ubi32_uarttio_put_char()
 */
static int ubi32_uarttio_put_char(struct uarttio_uart *uart, u8_t c)
{
	u32_t head = uart->tx_fifo_head;
	u32_t prev = head;

	/*
	 * Wrap
	 */
	if (++head == uart->tx_fifo_size) {
		head = 0;
	}

	/*
	 * If there isn't any space, return EBUSY
	 */
	if (head == uart->tx_fifo_tail) {
		return -EBUSY;
	}

	/*
	 * Put the character in the queue
	 */
	uart->tx_fifo[prev] = c;
	uart->tx_fifo_head = head;

	return 0;
}

/*
 * ubi32_uarttio_set_baud
 */
static int ubi32_uarttio_set_baud(struct ubi32_uarttio_port *uup, unsigned int baud)
{
	if (uup->uart->current_baud_rate == baud) {
		return 0;
	}

	uup->uart->baud_rate = baud;
	uup->uart->flags |= UARTTIO_UART_FLAG_SET_RATE;
	while (uup->uart->flags & UARTTIO_UART_FLAG_SET_RATE) {
		cpu_relax();
	}

	if (uup->uart->current_baud_rate != baud) {
		/*
		 * Failed to set baud rate
		 */
		printk(KERN_WARNING "Invalid baud rate %u, running at %u\n", baud, uup->uart->current_baud_rate);
		return -EINVAL;
	}

	return 0;
}

/*
 * ubi32_uarttio_handle_receive
 */
static void ubi32_uarttio_handle_receive(struct ubi32_uarttio_port *uup, int stat)
{
	struct uarttio_uart *uart = uup->uart;
	struct uart_port *port = &uup->port;
	struct tty_struct *tty = port->info->port.tty;
	unsigned char ch = 0;
	char flag = TTY_NORMAL;
	int count;

	if ((stat & (UARTTIO_UART_INT_RX | UARTTIO_UART_INT_RXFRAME | UARTTIO_UART_INT_RXOVF)) == 0) {
		return;
	}

	if (stat & UARTTIO_UART_INT_RX) {
		count = ubi32_uarttio_get_recv_ready(uart);
		port->icount.rx += count;
	}

	if (stat & UARTTIO_UART_INT_RXOVF) {
		port->icount.overrun++;
	}

	if (stat & UARTTIO_UART_INT_RXFRAME) {
		port->icount.frame++;
	}

	stat &= ~port->ignore_status_mask;

	if (stat & UARTTIO_UART_INT_RX) {
		int i;
		for (i = 0; i < count; i++) {
			ch = ubi32_uarttio_get_char(uart);
			tty_insert_flip_char(tty, ch, flag);
		}
	}

	if (stat & UARTTIO_UART_INT_RXFRAME) {
		tty_insert_flip_char(tty, 0, TTY_FRAME);
	}

	if (stat & UARTTIO_UART_INT_RXOVF) {
		tty_insert_flip_char(tty, 0, TTY_OVERRUN);
	}
}

/*
 * ubi32_uarttio_stop_tx
 *	interrupts are disabled on entry
 */
static void ubi32_uarttio_stop_tx(struct uart_port *port)
{
	struct ubi32_uarttio_port *uup = port->private_data;

	uup->uart->int_mask &= ~UARTTIO_UART_INT_TXBE;
}

/*
 * ubi32_uarttio_handle_transmit
 */
static void ubi32_uarttio_handle_transmit(struct ubi32_uarttio_port *uup, int stat)
{
	struct uarttio_uart *uart = uup->uart;
	struct uart_port *port = &uup->port;
	struct circ_buf *xmit  = &port->info->xmit;

	if (!(stat & UARTTIO_UART_INT_TXBE)) {
		return;
	}

	if (port->x_char) {
		if (ubi32_uarttio_put_char(uart, port->x_char)) {
			return;
		}
		port->x_char = 0;
		port->icount.tx++;
		return;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(port)) {
		ubi32_uarttio_stop_tx(port);
		return;
	}

	/*
	 * Send as many characters as we can
	 */
	while (ubi32_uarttio_put_char(uart, xmit->buf[xmit->tail]) == 0) {
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
		if (uart_circ_empty(xmit)) {
			break;
		}
	}

	/* wake up */
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
		uart_write_wakeup(port);
	}

	if (uart_circ_empty(xmit)) {
		ubi32_uarttio_stop_tx(port);
	}
}

/*
 * ubi32_uarttio_start_tx
 *	port is locked and interrupts are disabled
 */
static void ubi32_uarttio_start_tx(struct uart_port *port)
{
	struct ubi32_uarttio_port *uup = port->private_data;
	struct uarttio_uart *uart = uup->uart;

	uart->int_mask |= UARTTIO_UART_INT_TXBE;
}

/*
 * ubi32_uarttio_stop_rx
 *	Interrupts are enabled
 */
static void ubi32_uarttio_stop_rx(struct uart_port *port)
{
	struct ubi32_uarttio_port *uup = port->private_data;
	struct uarttio_uart *uart = uup->uart;

	/*
	 * don't forward any more data (like !CREAD)
	 */
	uart->int_mask &= ~UARTTIO_UART_INT_RX;
	port->ignore_status_mask = UARTTIO_UART_INT_RX;
}

/*
 * ubi32_uarttio_enable_ms
 *	Set the modem control timer to fire immediately.
 */
static void ubi32_uarttio_enable_ms(struct uart_port *port)
{
	/* N/A */
}

/*
 * ubi32_uarttio_isr
 */
static irqreturn_t ubi32_uarttio_isr(int irq, void *appdata)
{
	struct ubi32_uarttio_port *uup = uarttio_ports;
	int i;

	/*
	 * Service all of the ports
	 */
	for (i = 0; i < uarttio_nports; i++) {
		unsigned int flags;

		if (!(uup->uart->flags & UARTTIO_UART_FLAG_ENABLED)) {
			uup++;
			continue;
		}

		spin_lock(&uup->port.lock);

		flags = uup->uart->int_flags;

		uup->uart->int_flags = 0;

		ubi32_uarttio_handle_receive(uup, flags);
		ubi32_uarttio_handle_transmit(uup, flags);

		tty_flip_buffer_push(uup->port.info->port.tty);

		spin_unlock(&uup->port.lock);

		uup++;
	}

	return IRQ_HANDLED;
}

/*
 * ubi32_uarttio_tx_empty
 *	Return TIOCSER_TEMT when transmitter is not busy.
 */
static unsigned int ubi32_uarttio_tx_empty(struct uart_port *port)
{
	struct ubi32_uarttio_port *uup = port->private_data;

	if (uup->uart->tx_fifo_head == uup->uart->tx_fifo_tail) {
		return TIOCSER_TEMT;
	}

	return 0;
}

/*
 * ubi32_uarttio_get_mctrl
 */
static unsigned int ubi32_uarttio_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

/*
 * ubi32_uarttio_set_mctrl
 */
static void ubi32_uarttio_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	/* N/A */
}

/*
 * ubi32_uarttio_break_ctl
 */
static void ubi32_uarttio_break_ctl(struct uart_port *port, int break_state)
{
	/* N/A */
}

/*
 * ubi32_uarttio_startup
 */
static int ubi32_uarttio_startup(struct uart_port *port)
{
	struct ubi32_uarttio_port *uup = port->private_data;
	struct uarttio_uart *uart = uup->uart;

	uart->flags |= UARTTIO_UART_FLAG_ENABLED;

	uart->int_mask |= UARTTIO_UART_INT_TXBE | UARTTIO_UART_INT_RX;

	return 0;
}

/*
 * ubi32_uarttio_shutdown
 */
static void ubi32_uarttio_shutdown(struct uart_port *port)
{
	struct ubi32_uarttio_port *uup = port->private_data;
	struct uarttio_uart *uart = uup->uart;

	uart->int_mask = 0;
	uart->flags &= ~UARTTIO_UART_FLAG_ENABLED;
}

/*
 * ubi32_uarttio_set_termios
 */
static void ubi32_uarttio_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old)
{
	struct ubi32_uarttio_port *uup = port->private_data;
	unsigned long flags;
	unsigned int baud;

	spin_lock_irqsave(&port->lock, flags);

#if 0
	port->read_status_mask = UBI32_UARTTIO_RX | UBI32_UARTTIO_RXOVF | UBI32_UARTTIO_TXOVF;

	if (termios->c_iflag & INPCK) {
		port->read_status_mask |= UBI32_UARTTIO_RXFRAME;
	}
#endif

	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR) {
		port->ignore_status_mask |= UARTTIO_UART_INT_RXFRAME |
					    UARTTIO_UART_INT_RXOVF;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0) {
		port->ignore_status_mask |= UARTTIO_UART_INT_RX |
					    UARTTIO_UART_INT_RXFRAME |
					    UARTTIO_UART_INT_RXOVF;
	}

	/* update timeout */
	baud = uart_get_baud_rate(port, termios, old, 0, 460800);
	uart_update_timeout(port, termios->c_cflag, baud);

	ubi32_uarttio_set_baud(uup, baud);
	spin_unlock_irqrestore(&port->lock, flags);
}

/*
 * ubi32_uarttio_type
 */
static const char *ubi32_uarttio_type(struct uart_port *port)
{
	return (port->type == PORT_UBI32_UARTTIO) ? "UBI32_UARTTIO" : NULL;
}

/*
 * ubi32_uarttio_release_port
 *	Release the memory region(s) being used by 'port'.
 */
static void ubi32_uarttio_release_port(struct uart_port *port)
{
}

/*
 * ubi32_uarttio_request_port
 *	Request the memory region(s) being used by 'port'.
 */
static int ubi32_uarttio_request_port(struct uart_port *port)
{
	return 0;
}

/*
 * ubi32_uarttio_config_port
 *	Configure/autoconfigure the port.
 */
static void ubi32_uarttio_config_port(struct uart_port *port, int flags)
{
	if ((flags & UART_CONFIG_TYPE) && (ubi32_uarttio_request_port(port) == 0)) {
		port->type = PORT_UBI32_UARTTIO;
	}
}

/*
 * ubi32_uarttio_verify_port
 *	Verify the new serial_struct (for TIOCSSERIAL).
 *
 * The only change we allow are to the flags and type, and
 * even then only between PORT_UBI32_UARTTIO and PORT_UNKNOWN
 */
static int ubi32_uarttio_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static struct uart_ops ubi32_uarttio_pops = {
	.tx_empty	= ubi32_uarttio_tx_empty,
	.set_mctrl	= ubi32_uarttio_set_mctrl,
	.get_mctrl	= ubi32_uarttio_get_mctrl,
	.stop_tx	= ubi32_uarttio_stop_tx,
	.start_tx	= ubi32_uarttio_start_tx,
	.stop_rx	= ubi32_uarttio_stop_rx,
	.enable_ms	= ubi32_uarttio_enable_ms,
	.break_ctl	= ubi32_uarttio_break_ctl,
	.startup	= ubi32_uarttio_startup,
	.shutdown	= ubi32_uarttio_shutdown,
	.set_termios	= ubi32_uarttio_set_termios,
	.type		= ubi32_uarttio_type,
	.release_port	= ubi32_uarttio_release_port,
	.request_port	= ubi32_uarttio_request_port,
	.config_port	= ubi32_uarttio_config_port,
	.verify_port	= ubi32_uarttio_verify_port,
};

/*
 * ubi32_uarttio_add_ports
 */
static int __init ubi32_uarttio_add_ports(void)
{
	int res = 0;
	struct ubi32_uarttio_port *uup = uarttio_ports;
	int i = 0;

	for (i = 0; i < uarttio_nports; i++) {
		/*
		 * Setup the GPIOs
		 */
		res = gpio_request(uup->tx_pin, "ubi32_uarttio_tx");
		if (res) {
			printk(KERN_WARNING "Failed to request GPIO %d\n", uup->tx_pin);
			res = -EBUSY;
			goto next;
		}

		res = gpio_request(uup->rx_pin, "ubi32_uarttio_rx");
		if (res) {
			gpio_free(uup->tx_pin);
			printk(KERN_WARNING "Failed to request GPIO %d\n", uup->rx_pin);
			res = -EBUSY;
			goto next;
		}

		res = uart_add_one_port(&ubi32_uarttio_uart_driver, &uup->port);
		if (res) {
			gpio_free(uup->rx_pin);
			gpio_free(uup->tx_pin);
			res = -ENODEV;
			printk(KERN_WARNING "Failed to add port %d,%d\n", uup->tx_pin, uup->rx_pin);
			goto next;
		}
		uup->added = 1;

		/*
		 * Set the direction of the ports now, after we're sure that everything is ok
		 */
		if (!uup->port_init) {
			gpio_direction_output(uup->tx_pin, 1);
			gpio_direction_input(uup->rx_pin);
		}

next:
		uup++;
	}
	return res;
}

/*
 * ubi32_uarttio_cleanup
 */
static void ubi32_uarttio_cleanup(void)
{
	struct ubi32_uarttio_port *uup;
	int i;

	/*
	 * Stop the hardware thread
	 */
	if (uarttio_inst.regs) {
		thread_disable(uarttio_inst.regs->thread);
	}
	if (uarttio_inst.irq_requested) {
		free_irq(uarttio_inst.irq, NULL);
	}

	/*
	 * Get rid of the ports
	 */
	uup = uarttio_inst.ports;
	for (i = 0; i < uarttio_nports; i++) {
		gpio_free(uup->tx_pin);
		gpio_free(uup->rx_pin);
		if (uup->added) {
			uart_remove_one_port(&ubi32_uarttio_uart_driver, &uup->port);
		}
		uup++;
	}

	if (uarttio_inst.driver_registered) {
		uart_unregister_driver(&ubi32_uarttio_uart_driver);
	}
}

/*
 * ubi32_uarttio_setup_port
 *	Setup a port in the TIO registers
 */
static int ubi32_uarttio_setup_port(int index,
				    struct uarttio_uart *uart,
				    unsigned int baud, unsigned int tx_pin,
				    unsigned int rx_pin)
{
	struct ubi32_uarttio_port *uup = &uarttio_ports[index];
	void *tx_port = ubi_gpio_get_port(tx_pin);
	void *rx_port = ubi_gpio_get_port(rx_pin);

	/*
	 * Verify the ports are on chip
	 */
	if (!tx_port || !rx_port) {
		printk(KERN_WARNING "Invalid port(s) specified: %u or %u\n", tx_pin, rx_pin);
		return -EINVAL;
	}

	uup->tx_pin = tx_pin;
	uup->rx_pin = rx_pin;
	uup->uart = uart;

	/*
	 * Setup the port structure
	 */
	uup->port.ops		= &ubi32_uarttio_pops;
	uup->port.line		= index;
	uup->port.iotype	= UPIO_MEM;
	uup->port.flags		= UPF_BOOT_AUTOCONF;
	uup->port.fifosize	= uup->uart->tx_fifo_size;
	uup->port.private_data	= uup;

	/*
	 * We share this IRQ across all ports
	 */
	uup->port.irq		= uarttio_inst.irq;

	/*
	 * We really don't have a mem/map base but without these variables
	 * set, the serial_core won't startup.
	 */
	uup->port.membase	= (void __iomem *)uup;
	uup->port.mapbase	= (resource_size_t)uup;
	spin_lock_init(&uup->port.lock);

	/*
	 * Set up the hardware
	 */
	uart->flags = UARTTIO_UART_FLAG_SET_RATE | UARTTIO_UART_FLAG_RESET;

	uart->tx_port = (unsigned int)tx_port;
	uart->tx_pin = gpio_pin_index(tx_pin);
	uart->tx_bits = 8;
	uart->tx_stop_bits = 1;

	uart->rx_port = (unsigned int)rx_port;
	uart->rx_pin = gpio_pin_index(rx_pin);
	uart->rx_bits = 8;
	uart->rx_stop_bits = 1;

	uart->baud_rate = baud;

	return 0;
}

enum ubi32_uarttio_parse_states {
	UBI32_UARTTIO_PARSE_STATE_BAUD,
	UBI32_UARTTIO_PARSE_STATE_TX_PIN,
	UBI32_UARTTIO_PARSE_STATE_RX_PIN,
	UBI32_UARTTIO_PARSE_STATE_HS,
	UBI32_UARTTIO_PARSE_STATE_CTS_PIN,
	UBI32_UARTTIO_PARSE_STATE_RTS_PIN,
};

/*
 * ubi32_uarttio_parse_param
 */
static int ubi32_uarttio_parse_param(char *str)
{
	int res;
	int i;
	int baud = 0;
	int tx_pin = 0;
	int rx_pin = 0;
	int hs = 0;
	int cts_pin = 0;
	int rts_pin = 0;
	int nfound = 0;
	enum ubi32_uarttio_parse_states state = UBI32_UARTTIO_PARSE_STATE_BAUD;
	struct uarttio_uart *uart = uarttio_inst.regs->uarts;

	/*
	 * Run though the options and generate the proper structures
	 */
	res = get_option(&str, &i);
	while ((res == 2) || (res == 1)) {
		switch (state) {
		case UBI32_UARTTIO_PARSE_STATE_BAUD:
			/*
			 * If we are here and nfound > 0 then create the port
			 * based on the previous input
			 */
			if (nfound) {
				/*
				 * Create the port
				 */
				if (ubi32_uarttio_setup_port(nfound - 1, uart, baud, tx_pin, rx_pin)) {
					/*
					 * Port was invalid
					 */
					goto fail;
				} else {
					printk(KERN_INFO "Serial port %d: tx=%d:rx=%d @ %d\n", nfound, tx_pin, rx_pin, baud);
					uart++;
				}
			}

			/*
			 * Reset the variables and go to the next state
			 */
			hs = 0;
			baud = i;
			state = UBI32_UARTTIO_PARSE_STATE_TX_PIN;
			break;

		case UBI32_UARTTIO_PARSE_STATE_TX_PIN:
			tx_pin = i;
			state = UBI32_UARTTIO_PARSE_STATE_RX_PIN;
			break;

		case UBI32_UARTTIO_PARSE_STATE_RX_PIN:
			rx_pin = i;
			state = UBI32_UARTTIO_PARSE_STATE_HS;
			break;

		case UBI32_UARTTIO_PARSE_STATE_HS:
			hs = i;
			if (hs) {
				state = UBI32_UARTTIO_PARSE_STATE_CTS_PIN;
				break;
			}

			if (nfound == uarttio_inst.regs->max_uarts) {
				printk(KERN_WARNING "Maximum number of serial ports reached\n");
				goto done;
			}
			nfound++;
			state = UBI32_UARTTIO_PARSE_STATE_BAUD;
			break;

		case UBI32_UARTTIO_PARSE_STATE_CTS_PIN:
			cts_pin = i;
			state = UBI32_UARTTIO_PARSE_STATE_RTS_PIN;
			break;

		case UBI32_UARTTIO_PARSE_STATE_RTS_PIN:
			rts_pin = i;

			if (nfound == uarttio_inst.regs->max_uarts) {
				printk(KERN_WARNING "Maximum number of serial ports reached\n");
				goto done;
			}
			nfound++;
			state = UBI32_UARTTIO_PARSE_STATE_BAUD;
			break;
		}
		res = get_option(&str, &i);
	}

	if ((res > 2) || state != UBI32_UARTTIO_PARSE_STATE_BAUD) {
		printk(KERN_WARNING "Parameter syntax error.\n");
		res = -EINVAL;
		goto fail;
	}

	/*
	 * Create the final port
	 */
	if (ubi32_uarttio_setup_port(nfound - 1, uart, baud, tx_pin, rx_pin)) {
		goto fail;
	}
	printk(KERN_INFO "Serial port %d: tx=%d:rx=%d @ %d\n", nfound, tx_pin, rx_pin, baud);

done:
	uarttio_nports = nfound;

	return nfound ? 0 : -ENODEV;

fail:
	/*
	 * Reset the ports
	 */
	uart = uarttio_inst.regs->uarts;
	for (i = 0; i < uarttio_inst.regs->max_uarts; i++) {
		uart->flags = 0;
		uart++;
	}

	return res;
}

/*
 * ubi32_uarttio_probe
 */
static int ubi32_uarttio_probe(void)
{
	int ret;
	struct uarttio_node *uart_node;
	char *str = utio_ports_param;
	static int probed;
	static int probe_result;

	/*
	 * We only want to be probed once, we could be probed twice
	 * for example if we are used as a console
	 */
	if (probed) {
		return probe_result;
	}
	probed = 1;

	/*
	 * Extract the TIO name from the setup string
	 */
	while (*str) {
		if (*str == ',') {
			*str++ = 0;
			break;
		}
		str++;
	}

	if (!*str) {
		probe_result = -EINVAL;
		return -EINVAL;
	}

	uart_node = (struct uarttio_node *)devtree_find_node(utio_ports_param);
	if (!uart_node) {
		probe_result = -ENODEV;
		return -ENODEV;
	}

	uarttio_inst.irq = uart_node->dn.recvirq;
	uarttio_inst.regs = uart_node->regs;

	/*
	 * Parse module parameters.
	 */
	ret = ubi32_uarttio_parse_param(str);
	if (ret != 0) {
		ubi32_uarttio_cleanup();
		probe_result = ret;
		return ret;
	}

	ubi32_uarttio_uart_driver.nr = uarttio_nports;

	return 0;
}

#if defined(CONFIG_SERIAL_UBI32_UARTTIO_CONSOLE)
/*
 * ubi32_uarttio_console_setup
 */
static int __init ubi32_uarttio_console_setup(struct console *co, char *options)
{
	int baud;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';
	struct ubi32_uarttio_port *uup;

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index == -1 || co->index >= uarttio_nports) {
		co->index = 0;
	}
	uup = &uarttio_ports[co->index];
	baud = uup->uart->baud_rate;
	uup->uart->flags |= UARTTIO_UART_FLAG_ENABLED;

	/*
	 * Setup the GPIOs
	 *	We have to use the direct interface because the gpio
	 *	subsystem is not available at this point.
	 */
	uup->port_init = 1;
	UBICOM32_GPIO_SET_PIN_HIGH(uup->tx_pin);
	UBICOM32_GPIO_SET_PIN_OUTPUT(uup->tx_pin);
	UBICOM32_GPIO_SET_PIN_INPUT(uup->rx_pin);

	/*
	 * Start the thread
	 */
	thread_enable(uarttio_inst.regs->thread);

	/*
	 * Process options
	 */
	if (options) {
		uart_parse_options(options, &baud, &parity, &bits, &flow);
		if (ubi32_uarttio_set_baud(uup, baud)) {
			baud = uup->uart->current_baud_rate;
		}
	}

	return uart_set_options(&uup->port, co, baud, 'n', 8, 'n');
}

/*
 * ubi32_uarttio_console_putchar
 */
static void ubi32_uarttio_console_putchar(struct uart_port *port, int ch)
{
	struct ubi32_uarttio_port *uup = port->private_data;

	while (ubi32_uarttio_put_char(uup->uart, ch)) {
		cpu_relax();
	}
}

/*
 * ubi32_uarttio_console_write
 *	Interrupts are disabled on entering
 */
static void ubi32_uarttio_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_port *port = &(uarttio_ports[co->index].port);
	unsigned long flags = 0;

	spin_lock_irqsave(&port->lock, flags);
	uart_console_write(port, s, count, ubi32_uarttio_console_putchar);
	spin_unlock_irqrestore(&port->lock, flags);
}

static struct console ubi32_uarttio_console = {
	.name		= UBI32_UARTTIO_NAME,
	.write		= ubi32_uarttio_console_write,
	.device		= uart_console_device,
	.setup		= ubi32_uarttio_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &ubi32_uarttio_uart_driver,
};

static int __init ubi32_uarttio_console_init(void)
{
	int res;

	res = ubi32_uarttio_probe();
	if (res) {
		return res;
	}

	register_console(&ubi32_uarttio_console);
	return 0;
}
console_initcall(ubi32_uarttio_console_init);
#endif /* CONFIG_SERIAL_UBI32_UARTTIO_CONSOLE */

/*
 * ubi32_serial_suspend
 */
static int ubi32_uarttio_suspend(struct platform_device *pdev, pm_message_t state)
{
	int i;
	for (i = 0; i < uarttio_nports; i++) {
		uart_suspend_port(&ubi32_uarttio_uart_driver, &uarttio_ports[i].port);
	}

	return 0;
}

/*
 * ubi32_serial_resume
 */
static int ubi32_uarttio_resume(struct platform_device *pdev)
{
	int i;
	for (i = 0; i < uarttio_nports; i++) {
		uart_resume_port(&ubi32_uarttio_uart_driver, &uarttio_ports[i].port);
	}

	return 0;
}

/*
 * ubi32_uarttio_remove
 */
static int __devexit ubi32_uarttio_remove(struct platform_device *pdev)
{
	ubi32_uarttio_cleanup();

	uart_unregister_driver(&ubi32_uarttio_uart_driver);

	return 0;
}

static struct platform_driver ubi32_uarttio_platform_driver = {
	.remove		= __devexit_p(ubi32_uarttio_remove),
	.suspend	= ubi32_uarttio_suspend,
	.resume		= ubi32_uarttio_resume,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};

#ifndef MODULE
/*
 * Called at boot time.
 *
 * uarttio=TIONAME,(baud,tx_pin,rx_pin,handshake[,cts_pin,rts_pin],...)
 *	TIONAME is the name of the devtree node which describes the UARTTIO
 *	pin is the index of the pin, i.e. PA4 is 5 [(port * 32) + pin]
 *	handshake = 1 to enable handshaking, provide cts_pin, rts_pin (UNSUPPORTED)
 *	handshake = 0 to disable handshaking, do not provide cts_pin, rts_pin
 *	Ex: uarttio=UARTTIO,57600,7,6,0,9600,8,9,0
 */
static int __init ubi32_uarttio_setup(char *str)
{
	strncpy(utio_ports_param, str, UBI32_UARTTIO_MAX_PARAM_LEN);
	utio_ports_param[UBI32_UARTTIO_MAX_PARAM_LEN - 1] = 0;
	return 1;
}
__setup("uarttio=", ubi32_uarttio_setup);
#endif

/*
 * ubi32_uarttio_init
 */
static int __init ubi32_uarttio_init(void)
{
	int ret;
	int i;

	ret = ubi32_uarttio_probe();
	if (ret) {
		return ret;
	}

	/*
	 * Request the IRQ (do it here since many ports share the same IRQ)
	 */
	ret = request_irq(uarttio_inst.irq, ubi32_uarttio_isr, IRQF_DISABLED, DRIVER_NAME, NULL);
	if (ret != 0) {
		printk(KERN_WARNING "Could not request IRQ %d\n", uarttio_inst.irq);
		goto fail;
	}
	uarttio_inst.irq_requested = 1;

	/*
	 * Register the UART driver and add the ports
	 */
	ret = uart_register_driver(&ubi32_uarttio_uart_driver);
	if (ret != 0) {
		goto fail;
	}
	uarttio_inst.driver_registered = 1;

	ret = ubi32_uarttio_add_ports();
	if (ret != 0) {
		ubi32_uarttio_cleanup();
		return ret;
	}

	/*
	 * Start the thread
	 */
	thread_enable(uarttio_inst.regs->thread);

	for (i = 0; i < uarttio_nports; i++) {
		pr_info("Serial: Ubicom32 uarttio #%d: tx:%d rx:%d baud:%d\n",
			i, uarttio_ports[i].tx_pin, uarttio_ports[i].rx_pin,
			uarttio_ports[i].uart->current_baud_rate);
	}
	pr_info("Serial: Ubicom32 uarttio started on thread:%d irq:%d\n", uarttio_inst.regs->thread, uarttio_inst.irq);

	return ret;

fail:
	ubi32_uarttio_cleanup();
	return ret;
}
module_init(ubi32_uarttio_init);

/*
 * ubi32_uarttio_exit
 */
static void __exit ubi32_uarttio_exit(void)
{
	platform_driver_unregister(&ubi32_uarttio_platform_driver);
}
module_exit(ubi32_uarttio_exit);

module_param_string(ports, utio_ports_param, sizeof(utio_ports_param), 0444);
MODULE_PARM_DESC(ports, "Sets the ports to allocate: ports=TIONAME,(baud,txpin,rxpin,handshake[,ctspin,rtspin],...)\n"
			"     TIONAME is the name of the devtree node which describes the UARTTIO\n"
			"     pin is the index of the pin, i.e. PA4 is 5 [(port * 32) + pin]\n"
			"     handshake = 1 to enable handshaking, provide ctspin, rtspin (UNSUPPORTED)\n"
			"     handshake = 0 to disable handshaking, do not provide ctspin, rtspin\n"
			"     Ex: ports=UARTTIO,57600,7,6,0,9600,8,9,0\n");
MODULE_AUTHOR("Patrick Tjin <pat.tjin@ubicom.com>");
MODULE_DESCRIPTION("Ubicom serial virtual peripherial driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_CHARDEV_MAJOR(UBI32_UARTTIO_MAJOR);
MODULE_ALIAS("platform:" DRIVER_NAME);
