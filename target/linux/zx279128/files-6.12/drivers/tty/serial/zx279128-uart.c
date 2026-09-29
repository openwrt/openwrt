// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE ZX279128 on-chip UART driver
 *
 * The ZX279128/ZX279127 SoC family integrates an AMBA PL011-compatible
 * UART with two ZTE-specific deviations, recovered from the stock kernel:
 *
 *   - every register is shifted by 4 bytes compared to PL011 (DR at 0x04,
 *     FR at 0x14, CR at 0x34, IFLS at 0x38, IMSC at 0x40, RIS at 0x48,
 *     ICR at 0x4c),
 *   - registers are accessed as 16-bit halfwords; the RX error flags sit
 *     in the upper byte of DR exactly like on PL011.
 *
 * Interrupt and FIFO behaviour otherwise matches PL011: 16-entry FIFOs,
 * RXIM/RXIS 0x10, TXIM/TXIS 0x20, RTIM/RTIS 0x40, and IFLS (stock value
 * 0x12) selects the FIFO timeout/trigger levels.
 *
 * The bootloader configures the console UART for 115200n8 and leaves it
 * enabled; this driver keeps that configuration and never reprograms the
 * baud-rate dividers, so console output is glitch-free across the
 * earlycon handover.
 */

#include <linux/console.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/tty_flip.h>

#define ZX_UART_DR			0x04
#define ZX_UART_FR			0x14
#define ZX_UART_CR			0x34
#define ZX_UART_IFLS			0x38
#define ZX_UART_IMSC			0x40
#define ZX_UART_RIS			0x48
#define ZX_UART_ICR			0x4c

#define ZX_UART_DR_FE			BIT(8)
#define ZX_UART_DR_PE			BIT(9)
#define ZX_UART_DR_BE			BIT(10)
#define ZX_UART_DR_OE			BIT(11)
#define ZX_UART_DR_ERROR		(ZX_UART_DR_OE | ZX_UART_DR_BE | \
					 ZX_UART_DR_PE | ZX_UART_DR_FE)

#define ZX_UART_FR_CTS			BIT(0)
#define ZX_UART_FR_DSR			BIT(1)
#define ZX_UART_FR_DCD			BIT(2)
#define ZX_UART_FR_BUSY			BIT(3)
#define ZX_UART_FR_RXFE			BIT(4)
#define ZX_UART_FR_TXFF			BIT(5)

#define ZX_UART_CR_UARTEN		BIT(0)
#define ZX_UART_CR_TXE			BIT(8)
#define ZX_UART_CR_RXE			BIT(9)
#define ZX_UART_CR_RTS			BIT(10)
#define ZX_UART_CR_DTR			BIT(11)

#define ZX_UART_IM_RT			BIT(6)
#define ZX_UART_IM_TX			BIT(5)
#define ZX_UART_IM_RX			BIT(4)

/* RX, RX-timeout and error sources; TX is cleared by draining the FIFO. */
#define ZX_UART_ICR_KNOWN		0x7d0
/* FIFO half-full triggers with RX timeout, the stock IFLS value. */
#define ZX_UART_IFLS_STOCK		0x12

#define ZX_UART_FIFO_SIZE		16
#define ZX_UART_TX_BURST		(ZX_UART_FIFO_SIZE >> 1)
#define ZX_UART_RX_BUDGET		256
#define ZX_UART_BAUD			115200

struct zx_uart {
	struct uart_port port;
	unsigned int im;
	bool irq_requested;
	bool registered;
};

static struct zx_uart zx_uart;
static struct uart_driver zx_uart_driver;

static struct zx_uart *to_zx_uart(struct uart_port *port)
{
	return container_of(port, struct zx_uart, port);
}

static void zx_uart_rx_chars(struct uart_port *port)
{
	unsigned int budget = ZX_UART_RX_BUDGET;
	unsigned int ch;
	u8 flag;

	while (budget-- && !(readw(port->membase + ZX_UART_FR) & ZX_UART_FR_RXFE)) {
		ch = readw(port->membase + ZX_UART_DR);
		flag = TTY_NORMAL;
		port->icount.rx++;

		if (unlikely(ch & ZX_UART_DR_ERROR)) {
			if (ch & ZX_UART_DR_BE) {
				ch &= ~(ZX_UART_DR_FE | ZX_UART_DR_PE);
				port->icount.brk++;
				if (uart_handle_break(port))
					continue;
			} else if (ch & ZX_UART_DR_PE) {
				port->icount.parity++;
			} else if (ch & ZX_UART_DR_FE) {
				port->icount.frame++;
			}
			if (ch & ZX_UART_DR_OE)
				port->icount.overrun++;

			ch &= port->read_status_mask;

			if (ch & ZX_UART_DR_BE)
				flag = TTY_BREAK;
			else if (ch & ZX_UART_DR_PE)
				flag = TTY_PARITY;
			else if (ch & ZX_UART_DR_FE)
				flag = TTY_FRAME;
		}

		if (uart_prepare_sysrq_char(port, ch & 0xff))
			continue;

		uart_insert_char(port, ch, ZX_UART_DR_OE, ch & 0xff, flag);
	}

	tty_flip_buffer_push(&port->state->port);
}

static unsigned int zx_uart_tx_pio(struct uart_port *port)
{
	unsigned char character;

	return uart_port_tx_limited(port, character, ZX_UART_TX_BURST,
		!(readw(port->membase + ZX_UART_FR) & ZX_UART_FR_TXFF),
		writew(character, port->membase + ZX_UART_DR), ({}));
}

static void zx_uart_tx_chars(struct uart_port *port)
{
	/*
	 * uart_port_tx_limited() calls ops->stop_tx() once the xmit buffer
	 * drains, which clears TXIM again.
	 */
	zx_uart_tx_pio(port);
}

static void zx_uart_stop_tx(struct uart_port *port)
{
	struct zx_uart *zx = to_zx_uart(port);

	zx->im &= ~ZX_UART_IM_TX;
	writew(zx->im, port->membase + ZX_UART_IMSC);
}

static void zx_uart_start_tx(struct uart_port *port)
{
	struct zx_uart *zx = to_zx_uart(port);

	/*
	 * The interrupt output is level-sensitive (RIS & IMSC), so enabling
	 * TXIM after the FIFO refill cannot lose a completion interrupt.
	 */
	if (zx_uart_tx_pio(port)) {
		zx->im |= ZX_UART_IM_TX;
		writew(zx->im, port->membase + ZX_UART_IMSC);
	}
}

static void zx_uart_stop_rx(struct uart_port *port)
{
	struct zx_uart *zx = to_zx_uart(port);

	zx->im &= ~(ZX_UART_IM_RX | ZX_UART_IM_RT);
	writew(zx->im, port->membase + ZX_UART_IMSC);
}

static irqreturn_t zx_uart_irq(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	unsigned int status;
	unsigned int pass = 16;
	int handled = 0;

	uart_port_lock(port);
	status = readw(port->membase + ZX_UART_RIS) &
		readw(port->membase + ZX_UART_IMSC);
	if (status) {
		do {
			/* Clear modem and error sources; RX/TX/RT are
			 * cleared by servicing them.
			 */
			writew(status & ~(ZX_UART_IM_RX | ZX_UART_IM_TX |
					   ZX_UART_IM_RT),
			       port->membase + ZX_UART_ICR);

			if (status & (ZX_UART_IM_RX | ZX_UART_IM_RT))
				zx_uart_rx_chars(port);
			if (status & ZX_UART_IM_TX)
				zx_uart_tx_chars(port);
		} while (pass-- &&
			 (status = readw(port->membase + ZX_UART_RIS) &
				  readw(port->membase + ZX_UART_IMSC)));
		handled = 1;
	}
	uart_unlock_and_check_sysrq(port);

	return IRQ_RETVAL(handled);
}

static unsigned int zx_uart_tx_empty(struct uart_port *port)
{
	u16 status = readw(port->membase + ZX_UART_FR);

	return status & (ZX_UART_FR_BUSY | ZX_UART_FR_TXFF) ?
		0 : TIOCSER_TEMT;
}

static void zx_uart_set_mctrl(struct uart_port *port, unsigned int control)
{
}

static unsigned int zx_uart_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static int zx_uart_startup(struct uart_port *port)
{
	struct zx_uart *zx = to_zx_uart(port);
	unsigned long flags;
	unsigned int cr;
	int result;

	/* Mask interrupts first so a bootloader-enabled source cannot
	 * fire before the handler is installed.
	 */
	writew(0, port->membase + ZX_UART_IMSC);
	writew(ZX_UART_ICR_KNOWN, port->membase + ZX_UART_ICR);

	result = request_irq(port->irq, zx_uart_irq, 0, "zx279128-uart", port);
	if (result)
		return result;
	zx->irq_requested = true;

	uart_port_lock_irqsave(port, &flags);
	writew(ZX_UART_IFLS_STOCK, port->membase + ZX_UART_IFLS);
	cr = readw(port->membase + ZX_UART_CR);
	cr = (cr & (ZX_UART_CR_RTS | ZX_UART_CR_DTR)) |
	     ZX_UART_CR_UARTEN | ZX_UART_CR_TXE | ZX_UART_CR_RXE;
	writew(cr, port->membase + ZX_UART_CR);
	zx->im = ZX_UART_IM_RX | ZX_UART_IM_RT;
	writew(zx->im, port->membase + ZX_UART_IMSC);
	uart_port_unlock_irqrestore(port, flags);

	return 0;
}

static void zx_uart_shutdown(struct uart_port *port)
{
	struct zx_uart *zx = to_zx_uart(port);
	unsigned long flags;

	uart_port_lock_irqsave(port, &flags);
	zx->im = 0;
	writew(0, port->membase + ZX_UART_IMSC);
	uart_port_unlock_irqrestore(port, flags);

	free_irq(port->irq, port);
	zx->irq_requested = false;
}

static void zx_uart_set_termios(struct uart_port *port, struct ktermios *termios,
				const struct ktermios *old)
{
	struct zx_uart *zx = to_zx_uart(port);
	unsigned long flags;
	unsigned int im;

	/*
	 * The dividers are not reprogrammed (the bootloader's 115200n8 is
	 * retained), so pin the software state to 8N1/115200.
	 */
	termios->c_cflag &= ~(CSIZE | CSTOPB | PARENB | PARODD | CMSPAR | CRTSCTS);
	termios->c_cflag |= CS8 | CLOCAL;
	tty_termios_encode_baud_rate(termios, ZX_UART_BAUD, ZX_UART_BAUD);

	uart_port_lock_irqsave(port, &flags);

	port->read_status_mask = ZX_UART_DR_OE | 255;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= ZX_UART_DR_FE | ZX_UART_DR_PE;
	if (termios->c_iflag & IGNBRK)
		port->read_status_mask |= ZX_UART_DR_BE;

	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= ZX_UART_DR_FE | ZX_UART_DR_PE;
	if (termios->c_iflag & (IGNBRK | PARMRK))
		port->ignore_status_mask |= ZX_UART_DR_BE;
	if (!(termios->c_cflag & CREAD))
		port->ignore_status_mask |= ZX_UART_DR_ERROR;

	uart_update_timeout(port, termios->c_cflag, ZX_UART_BAUD);

	im = zx->im;
	if (termios->c_cflag & CREAD)
		im |= ZX_UART_IM_RX | ZX_UART_IM_RT;
	else
		im &= ~(ZX_UART_IM_RX | ZX_UART_IM_RT);
	zx->im = im;
	if (zx->irq_requested)
		writew(im, port->membase + ZX_UART_IMSC);

	uart_port_unlock_irqrestore(port, flags);
}

static const char *zx_uart_type(struct uart_port *port)
{
	return "ZX279128 UART";
}

static int zx_uart_request_port(struct uart_port *port)
{
	return 0;
}

static void zx_uart_release_port(struct uart_port *port)
{
}

static void zx_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_GENERIC;
}

static int zx_uart_verify_port(struct uart_port *port, struct serial_struct *serial)
{
	return -EINVAL;
}

static const struct uart_ops zx_uart_ops = {
	.tx_empty = zx_uart_tx_empty,
	.set_mctrl = zx_uart_set_mctrl,
	.get_mctrl = zx_uart_get_mctrl,
	.stop_tx = zx_uart_stop_tx,
	.start_tx = zx_uart_start_tx,
	.stop_rx = zx_uart_stop_rx,
	.startup = zx_uart_startup,
	.shutdown = zx_uart_shutdown,
	.set_termios = zx_uart_set_termios,
	.type = zx_uart_type,
	.request_port = zx_uart_request_port,
	.release_port = zx_uart_release_port,
	.config_port = zx_uart_config_port,
	.verify_port = zx_uart_verify_port,
};

static void zx_uart_console_putc(struct uart_port *port, unsigned char character)
{
	u16 status;

	if (!readw_poll_timeout_atomic(port->membase + ZX_UART_FR, status,
				       !(status & ZX_UART_FR_TXFF), 1, 10000))
		writew(character, port->membase + ZX_UART_DR);
}

static void zx_uart_console_write(struct console *console, const char *text,
				  unsigned int count)
{
	struct uart_port *port = &zx_uart.port;
	unsigned long flags;
	bool locked;

	local_irq_save(flags);
	if (oops_in_progress)
		locked = uart_port_trylock(port);
	else {
		uart_port_lock(port);
		locked = true;
	}
	uart_console_write(port, text, count, zx_uart_console_putc);
	if (locked)
		uart_port_unlock(port);
	local_irq_restore(flags);
}

static int zx_uart_console_setup(struct console *console, char *options)
{
	if (console->index > 0 || !zx_uart.port.membase)
		return -ENODEV;
	console->index = 0;
	return uart_set_options(&zx_uart.port, console, ZX_UART_BAUD, 'n', 8, 'n');
}

static struct console zx_uart_console = {
	.name = "ttyZX",
	.write = zx_uart_console_write,
	.device = uart_console_device,
	.setup = zx_uart_console_setup,
	.flags = CON_PRINTBUFFER,
	.index = -1,
	.data = &zx_uart_driver,
};

static struct uart_driver zx_uart_driver = {
	.owner = THIS_MODULE,
	.driver_name = "zx279128-uart",
	.dev_name = "ttyZX",
	.nr = 1,
	.cons = &zx_uart_console,
};

static int zx_uart_probe(struct platform_device *device)
{
	struct uart_port *port = &zx_uart.port;
	struct resource *resource;
	void __iomem *membase;
	int irq;
	int result;

	if (zx_uart.registered)
		return -EBUSY;

	membase = devm_platform_get_and_ioremap_resource(device, 0, &resource);
	if (IS_ERR(membase))
		return PTR_ERR(membase);

	irq = platform_get_irq(device, 0);
	if (irq < 0)
		return irq;

	port->dev = &device->dev;
	port->mapbase = resource->start;
	port->mapsize = resource_size(resource);
	port->membase = membase;
	port->iotype = UPIO_MEM;
	port->fifosize = ZX_UART_FIFO_SIZE;
	port->flags = UPF_BOOT_AUTOCONF | UPF_FIXED_PORT;
	port->ops = &zx_uart_ops;
	port->uartclk = ZX_UART_BAUD * 16;
	port->line = 0;
	port->irq = irq;
	port->has_sysrq = IS_ENABLED(CONFIG_MAGIC_SYSRQ);

	result = uart_add_one_port(&zx_uart_driver, port);
	if (result) {
		port->membase = NULL;
		return result;
	}
	zx_uart.registered = true;

	dev_info(&device->dev, "ttyZX%d at MMIO 0x%pa (irq = %d), retaining 115200n8 bootloader setup\n",
		 port->line, &port->mapbase, port->irq);
	return 0;
}

static const struct of_device_id zx_uart_of_match[] = {
	{ .compatible = "zte,ZX279127-uart" },
	{ }
};

static struct platform_driver zx_uart_platform_driver = {
	.probe = zx_uart_probe,
	.driver = {
		.name = "zx279128-uart",
		.of_match_table = zx_uart_of_match,
		.suppress_bind_attrs = true,
	},
};

static int __init zx_uart_init(void)
{
	int result;

	result = uart_register_driver(&zx_uart_driver);
	if (result)
		return result;
	result = platform_driver_register(&zx_uart_platform_driver);
	if (result)
		uart_unregister_driver(&zx_uart_driver);
	return result;
}

static void __exit zx_uart_exit(void)
{
	platform_driver_unregister(&zx_uart_platform_driver);
	uart_unregister_driver(&zx_uart_driver);
}

module_init(zx_uart_init);
module_exit(zx_uart_exit);

MODULE_DESCRIPTION("ZTE ZX279128 SoC UART driver");
MODULE_LICENSE("GPL");
