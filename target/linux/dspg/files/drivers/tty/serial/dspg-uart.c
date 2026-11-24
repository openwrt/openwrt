// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2009 DSP Group Inc.
 * Copyright (C) 2010 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/clk.h>
#include <linux/console.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>

#define DSPG_UART_CTL			0x00
#define DSPG_UART_CFG			0x04
#define DSPG_UART_INT_DIV		0x08
#define DSPG_UART_FRAC_DIV		0x0c
#define DSPG_UART_TX_WM			0x10
#define DSPG_UART_RX_WM			0x14
#define DSPG_UART_FIFO_ISR		0x18
#define DSPG_UART_FIFO_IER		0x1c
#define DSPG_UART_FIFO_ICR		0x20
#define DSPG_UART_TX_DATA		0x2c
#define DSPG_UART_RX_DATA		0x30
#define DSPG_UART_STAT			0x34

#define DSPG_UART_CTL_EN		BIT(0)

#define DSPG_UART_CFG_DATA_7		BIT(2)
#define DSPG_UART_CFG_STOP_2		BIT(4)
#define DSPG_UART_CFG_PAR_ODD		BIT(5)
#define DSPG_UART_CFG_PAR_EN		BIT(6)
#define DSPG_UART_CFG_RTS_EN		BIT(9)
#define DSPG_UART_CFG_CTS_EN		BIT(10)
#define DSPG_UART_CFG_BREAK_EN		BIT(11)

#define DSPG_UART_INT_TX_OVER		BIT(0)
#define DSPG_UART_INT_TX_WM		BIT(1)
#define DSPG_UART_INT_RX_UNDER		BIT(8)
#define DSPG_UART_INT_RX_OVER		BIT(9)
#define DSPG_UART_INT_RX_WM		BIT(10)
#define DSPG_UART_INT_RX_TIMEOUT	BIT(11)
#define DSPG_UART_INT_BREAK		BIT(12)

#define DSPG_UART_INT_TX		(DSPG_UART_INT_TX_OVER | \
					 DSPG_UART_INT_TX_WM)
#define DSPG_UART_INT_RX		(DSPG_UART_INT_RX_OVER | \
					 DSPG_UART_INT_RX_WM | \
					 DSPG_UART_INT_RX_TIMEOUT | \
					 DSPG_UART_INT_BREAK)
#define DSPG_UART_INT_CLR		(DSPG_UART_INT_TX_OVER | \
					 DSPG_UART_INT_RX_UNDER | \
					 DSPG_UART_INT_RX_OVER | \
					 DSPG_UART_INT_BREAK)

#define DSPG_UART_RX_DATA_CHAR		GENMASK(7, 0)
#define DSPG_UART_RX_DATA_PE		BIT(9)
#define DSPG_UART_RX_DATA_FE		BIT(10)

#define DSPG_UART_STAT_TX_EMPTY		BIT(0)
#define DSPG_UART_STAT_TX_FULL		BIT(1)
#define DSPG_UART_STAT_CTS_N		BIT(3)
#define DSPG_UART_STAT_RX_EMPTY		BIT(8)

#define DSPG_UART_FIFO_SIZE		128
#define DSPG_UART_NR			2
#define DSPG_UART_DRAIN_TIMEOUT_US	20000
#define DSPG_UART_DIV_INT_MAX		0x3ff

struct dspg_uart {
	struct uart_port port;
	struct clk *clk;
};

static struct uart_port *dspg_uart_ports[DSPG_UART_NR];

static u16 dspg_uart_read(struct uart_port *port, unsigned int reg)
{
	return readw_relaxed(port->membase + reg);
}

static void dspg_uart_write(struct uart_port *port, unsigned int reg, u16 val)
{
	writew_relaxed(val, port->membase + reg);
}

static void dspg_uart_ier_update(struct uart_port *port, u16 clr, u16 set)
{
	u16 ier = dspg_uart_read(port, DSPG_UART_FIFO_IER);

	dspg_uart_write(port, DSPG_UART_FIFO_IER, (ier & ~clr) | set);
}

static void dspg_uart_drain(struct uart_port *port)
{
	u16 stat;

	readw_poll_timeout_atomic(port->membase + DSPG_UART_STAT, stat,
				  stat & DSPG_UART_STAT_TX_EMPTY, 1,
				  DSPG_UART_DRAIN_TIMEOUT_US);
}

static unsigned int dspg_uart_tx_empty(struct uart_port *port)
{
	return dspg_uart_read(port, DSPG_UART_STAT) & DSPG_UART_STAT_TX_EMPTY ?
	       TIOCSER_TEMT : 0;
}

static void dspg_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static unsigned int dspg_uart_get_mctrl(struct uart_port *port)
{
	unsigned int mctrl = TIOCM_CAR | TIOCM_DSR;

	if (!(dspg_uart_read(port, DSPG_UART_STAT) & DSPG_UART_STAT_CTS_N))
		mctrl |= TIOCM_CTS;

	return mctrl;
}

static void dspg_uart_stop_tx(struct uart_port *port)
{
	dspg_uart_ier_update(port, DSPG_UART_INT_TX, 0);
}

static void dspg_uart_start_tx(struct uart_port *port)
{
	dspg_uart_ier_update(port, 0, DSPG_UART_INT_TX);
}

static void dspg_uart_stop_rx(struct uart_port *port)
{
	dspg_uart_ier_update(port, DSPG_UART_INT_RX, 0);
}

static void dspg_uart_break_ctl(struct uart_port *port, int break_state)
{
}

static void dspg_uart_rx_chars(struct uart_port *port, u16 isr)
{
	unsigned int max_count = DSPG_UART_FIFO_SIZE;
	bool brk = isr & DSPG_UART_INT_BREAK;

	if (isr & DSPG_UART_INT_RX_OVER)
		port->icount.overrun++;

	while (!(dspg_uart_read(port, DSPG_UART_STAT) & DSPG_UART_STAT_RX_EMPTY) &&
	       max_count--) {
		u16 data = dspg_uart_read(port, DSPG_UART_RX_DATA);
		u16 err = data & (DSPG_UART_RX_DATA_PE | DSPG_UART_RX_DATA_FE);
		unsigned int flag = TTY_NORMAL;
		u8 ch = data & DSPG_UART_RX_DATA_CHAR;

		port->icount.rx++;

		if (brk) {
			brk = false;
			port->icount.brk++;
			if (uart_handle_break(port))
				continue;
			tty_insert_flip_char(&port->state->port, ch, TTY_BREAK);
			continue;
		}

		if (err & DSPG_UART_RX_DATA_PE)
			port->icount.parity++;
		else if (err & DSPG_UART_RX_DATA_FE)
			port->icount.frame++;

		if (err & port->ignore_status_mask)
			continue;

		if (err & port->read_status_mask & DSPG_UART_RX_DATA_PE)
			flag = TTY_PARITY;
		else if (err & port->read_status_mask & DSPG_UART_RX_DATA_FE)
			flag = TTY_FRAME;

		if (uart_prepare_sysrq_char(port, ch))
			continue;

		tty_insert_flip_char(&port->state->port, ch, flag);
	}

	if (brk) {
		port->icount.brk++;
		if (!uart_handle_break(port))
			tty_insert_flip_char(&port->state->port, 0, TTY_BREAK);
	}

	if (isr & DSPG_UART_INT_RX_OVER)
		tty_insert_flip_char(&port->state->port, 0, TTY_OVERRUN);

	tty_flip_buffer_push(&port->state->port);
}

static void dspg_uart_tx_chars(struct uart_port *port)
{
	struct tty_port *tport = &port->state->port;
	unsigned char ch;

	if (port->x_char) {
		dspg_uart_write(port, DSPG_UART_TX_DATA, port->x_char);
		port->icount.tx++;
		port->x_char = 0;
		return;
	}

	if (kfifo_is_empty(&tport->xmit_fifo) || uart_tx_stopped(port)) {
		dspg_uart_stop_tx(port);
		return;
	}

	while (!(dspg_uart_read(port, DSPG_UART_STAT) & DSPG_UART_STAT_TX_FULL) &&
	       uart_fifo_get(port, &ch))
		dspg_uart_write(port, DSPG_UART_TX_DATA, ch);

	if (kfifo_len(&tport->xmit_fifo) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	if (kfifo_is_empty(&tport->xmit_fifo))
		dspg_uart_stop_tx(port);
}

static irqreturn_t dspg_uart_irq(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	u16 isr;

	uart_port_lock(port);

	isr = dspg_uart_read(port, DSPG_UART_FIFO_ISR) &
	      dspg_uart_read(port, DSPG_UART_FIFO_IER);
	if (!isr) {
		uart_port_unlock(port);
		return IRQ_NONE;
	}

	dspg_uart_write(port, DSPG_UART_FIFO_ICR, isr & DSPG_UART_INT_CLR);

	if (isr & DSPG_UART_INT_RX)
		dspg_uart_rx_chars(port, isr);

	if (isr & DSPG_UART_INT_TX)
		dspg_uart_tx_chars(port);

	uart_unlock_and_check_sysrq(port);

	return IRQ_HANDLED;
}

static int dspg_uart_startup(struct uart_port *port)
{
	unsigned long flags;
	int ret;

	uart_port_lock_irqsave(port, &flags);
	dspg_uart_write(port, DSPG_UART_FIFO_IER, 0);
	dspg_uart_write(port, DSPG_UART_FIFO_ICR, DSPG_UART_INT_CLR);
	dspg_uart_write(port, DSPG_UART_TX_WM, DSPG_UART_FIFO_SIZE / 2);
	dspg_uart_write(port, DSPG_UART_RX_WM, DSPG_UART_FIFO_SIZE / 2);
	dspg_uart_write(port, DSPG_UART_CTL, DSPG_UART_CTL_EN);
	uart_port_unlock_irqrestore(port, flags);

	ret = request_irq(port->irq, dspg_uart_irq, 0, dev_name(port->dev),
			  port);
	if (ret)
		return ret;

	uart_port_lock_irqsave(port, &flags);
	dspg_uart_ier_update(port, 0, DSPG_UART_INT_RX);
	uart_port_unlock_irqrestore(port, flags);

	return 0;
}

static void dspg_uart_shutdown(struct uart_port *port)
{
	unsigned long flags;

	uart_port_lock_irqsave(port, &flags);
	dspg_uart_write(port, DSPG_UART_FIFO_IER, 0);
	dspg_uart_write(port, DSPG_UART_FIFO_ICR, DSPG_UART_INT_CLR);
	uart_port_unlock_irqrestore(port, flags);

	free_irq(port->irq, port);
}

static void dspg_uart_set_termios(struct uart_port *port,
				  struct ktermios *termios,
				  const struct ktermios *old)
{
	unsigned int baud, quot;
	unsigned long flags;
	u16 cfg = 0;

	termios->c_cflag &= ~CMSPAR;

	baud = uart_get_baud_rate(port, termios, old,
				  DIV_ROUND_UP(port->uartclk,
					       16 * (DSPG_UART_DIV_INT_MAX + 1)),
				  port->uartclk / 16);
	quot = port->uartclk / baud;

	switch (termios->c_cflag & CSIZE) {
	case CS7:
		cfg |= DSPG_UART_CFG_DATA_7;
		break;
	default:
		termios->c_cflag &= ~CSIZE;
		termios->c_cflag |= CS8;
		break;
	}

	if (termios->c_cflag & CSTOPB)
		cfg |= DSPG_UART_CFG_STOP_2;

	if (termios->c_cflag & PARENB) {
		cfg |= DSPG_UART_CFG_PAR_EN;
		if (termios->c_cflag & PARODD)
			cfg |= DSPG_UART_CFG_PAR_ODD;
	}

	if (termios->c_cflag & CRTSCTS)
		cfg |= DSPG_UART_CFG_RTS_EN | DSPG_UART_CFG_CTS_EN;

	if (!(termios->c_iflag & IGNBRK))
		cfg |= DSPG_UART_CFG_BREAK_EN;

	uart_port_lock_irqsave(port, &flags);

	uart_update_timeout(port, termios->c_cflag, baud);

	port->read_status_mask = 0;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= DSPG_UART_RX_DATA_PE |
					  DSPG_UART_RX_DATA_FE;

	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= DSPG_UART_RX_DATA_PE |
					    DSPG_UART_RX_DATA_FE;

	dspg_uart_drain(port);

	dspg_uart_write(port, DSPG_UART_CTL, 0);
	dspg_uart_write(port, DSPG_UART_INT_DIV, quot >> 4);
	dspg_uart_write(port, DSPG_UART_FRAC_DIV, quot & 0xf);
	dspg_uart_write(port, DSPG_UART_CFG, cfg);
	dspg_uart_write(port, DSPG_UART_CTL, DSPG_UART_CTL_EN);

	uart_port_unlock_irqrestore(port, flags);
}

static const char *dspg_uart_type(struct uart_port *port)
{
	return port->type == PORT_DSPG ? "DSPG UART" : NULL;
}

static void dspg_uart_release_port(struct uart_port *port)
{
}

static int dspg_uart_request_port(struct uart_port *port)
{
	return 0;
}

static void dspg_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_DSPG;
}

static int dspg_uart_verify_port(struct uart_port *port,
				 struct serial_struct *ser)
{
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_DSPG)
		return -EINVAL;

	return 0;
}

static const struct uart_ops dspg_uart_ops = {
	.tx_empty	= dspg_uart_tx_empty,
	.set_mctrl	= dspg_uart_set_mctrl,
	.get_mctrl	= dspg_uart_get_mctrl,
	.stop_tx	= dspg_uart_stop_tx,
	.start_tx	= dspg_uart_start_tx,
	.stop_rx	= dspg_uart_stop_rx,
	.break_ctl	= dspg_uart_break_ctl,
	.startup	= dspg_uart_startup,
	.shutdown	= dspg_uart_shutdown,
	.set_termios	= dspg_uart_set_termios,
	.type		= dspg_uart_type,
	.release_port	= dspg_uart_release_port,
	.request_port	= dspg_uart_request_port,
	.config_port	= dspg_uart_config_port,
	.verify_port	= dspg_uart_verify_port,
};

#ifdef CONFIG_SERIAL_DSPG_UART_CONSOLE
static void dspg_uart_console_putchar(struct uart_port *port, unsigned char ch)
{
	while (dspg_uart_read(port, DSPG_UART_STAT) & DSPG_UART_STAT_TX_FULL)
		cpu_relax();

	dspg_uart_write(port, DSPG_UART_TX_DATA, ch);
}

static void dspg_uart_console_write(struct console *co, const char *s,
				    unsigned int count)
{
	struct uart_port *port = dspg_uart_ports[co->index];
	unsigned long flags;
	int locked = 1;
	u16 ier, cfg;

	if (oops_in_progress)
		locked = uart_port_trylock_irqsave(port, &flags);
	else
		uart_port_lock_irqsave(port, &flags);

	cfg = dspg_uart_read(port, DSPG_UART_CFG);
	dspg_uart_write(port, DSPG_UART_CFG, cfg & ~DSPG_UART_CFG_CTS_EN);

	ier = dspg_uart_read(port, DSPG_UART_FIFO_IER);
	dspg_uart_write(port, DSPG_UART_FIFO_IER, ier & ~DSPG_UART_INT_TX);

	uart_console_write(port, s, count, dspg_uart_console_putchar);
	dspg_uart_drain(port);

	dspg_uart_write(port, DSPG_UART_FIFO_IER, ier);
	dspg_uart_write(port, DSPG_UART_CFG, cfg);

	if (locked)
		uart_port_unlock_irqrestore(port, flags);
}

static int dspg_uart_console_setup(struct console *co, char *options)
{
	int baud = 115200, bits = 8, parity = 'n', flow = 'n';
	struct uart_port *port;

	if (co->index < 0 || co->index >= DSPG_UART_NR)
		return -EINVAL;

	port = dspg_uart_ports[co->index];
	if (!port)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver dspg_uart_driver;

static struct console dspg_uart_console = {
	.name	= "ttyS",
	.device	= uart_console_device,
	.write	= dspg_uart_console_write,
	.setup	= dspg_uart_console_setup,
	.flags	= CON_PRINTBUFFER,
	.index	= -1,
	.data	= &dspg_uart_driver,
};

static void dspg_uart_early_putchar(struct uart_port *port, unsigned char ch)
{
	while (readw_relaxed(port->membase + DSPG_UART_STAT) &
	       DSPG_UART_STAT_TX_FULL)
		cpu_relax();

	writew_relaxed(ch, port->membase + DSPG_UART_TX_DATA);
}

static void dspg_uart_early_write(struct console *con, const char *s,
				  unsigned int n)
{
	struct earlycon_device *dev = con->data;

	uart_console_write(&dev->port, s, n, dspg_uart_early_putchar);
}

static int __init dspg_uart_early_setup(struct earlycon_device *dev,
					const char *opt)
{
	if (!dev->port.membase)
		return -ENODEV;

	dev->con->write = dspg_uart_early_write;

	return 0;
}
OF_EARLYCON_DECLARE(dspg, "dspg,uart", dspg_uart_early_setup);

#define DSPG_UART_CONSOLE	(&dspg_uart_console)
#else
#define DSPG_UART_CONSOLE	NULL
#endif

static struct uart_driver dspg_uart_driver = {
	.owner		= THIS_MODULE,
	.driver_name	= "dspg-uart",
	.dev_name	= "ttyS",
	.nr		= DSPG_UART_NR,
	.cons		= DSPG_UART_CONSOLE,
};

static int dspg_uart_probe(struct platform_device *pdev)
{
	struct dspg_uart *uart;
	struct resource *res;
	int id, irq, ret;

	id = of_alias_get_id(pdev->dev.of_node, "serial");
	if (id < 0 || id >= DSPG_UART_NR)
		return dev_err_probe(&pdev->dev, -EINVAL, "invalid serial alias\n");

	uart = devm_kzalloc(&pdev->dev, sizeof(*uart), GFP_KERNEL);
	if (!uart)
		return -ENOMEM;

	uart->port.membase = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (IS_ERR(uart->port.membase))
		return PTR_ERR(uart->port.membase);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	uart->clk = devm_clk_get_enabled(&pdev->dev, NULL);
	if (IS_ERR(uart->clk))
		return dev_err_probe(&pdev->dev, PTR_ERR(uart->clk),
				     "failed to get clock\n");

	uart->port.dev = &pdev->dev;
	uart->port.mapbase = res->start;
	uart->port.irq = irq;
	uart->port.iotype = UPIO_MEM;
	uart->port.uartclk = clk_get_rate(uart->clk);
	uart->port.fifosize = DSPG_UART_FIFO_SIZE;
	uart->port.ops = &dspg_uart_ops;
	uart->port.line = id;
	uart->port.type = PORT_DSPG;
	uart->port.flags = UPF_BOOT_AUTOCONF;
	uart->port.has_sysrq = IS_ENABLED(CONFIG_SERIAL_DSPG_UART_CONSOLE);

	dspg_uart_ports[id] = &uart->port;
	platform_set_drvdata(pdev, uart);

	ret = uart_add_one_port(&dspg_uart_driver, &uart->port);
	if (ret)
		dspg_uart_ports[id] = NULL;

	return ret;
}

static void dspg_uart_remove(struct platform_device *pdev)
{
	struct dspg_uart *uart = platform_get_drvdata(pdev);

	uart_remove_one_port(&dspg_uart_driver, &uart->port);
	dspg_uart_ports[uart->port.line] = NULL;
}

static const struct of_device_id dspg_uart_of_match[] = {
	{ .compatible = "dspg,uart" },
	{ }
};
MODULE_DEVICE_TABLE(of, dspg_uart_of_match);

static struct platform_driver dspg_uart_platform_driver = {
	.probe	= dspg_uart_probe,
	.remove	= dspg_uart_remove,
	.driver	= {
		.name		= "dspg-uart",
		.of_match_table	= dspg_uart_of_match,
	},
};

static int __init dspg_uart_init(void)
{
	int ret;

	ret = uart_register_driver(&dspg_uart_driver);
	if (ret)
		return ret;

	ret = platform_driver_register(&dspg_uart_platform_driver);
	if (ret)
		uart_unregister_driver(&dspg_uart_driver);

	return ret;
}
module_init(dspg_uart_init);

static void __exit dspg_uart_exit(void)
{
	platform_driver_unregister(&dspg_uart_platform_driver);
	uart_unregister_driver(&dspg_uart_driver);
}
module_exit(dspg_uart_exit);

MODULE_DESCRIPTION("DSPG DVF UART driver");
MODULE_LICENSE("GPL");
