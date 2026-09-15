// SPDX-License-Identifier: GPL-2.0
/*
 * drivers/serial/serial_cortina.c
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */


#include <linux/module.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/sysrq.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/clk.h>

#define	CFG		(0x00)
#define	FC		(0x04)
#define	RX_SAMPLE	(0x08)
#define	TX_DAT		(0x10)
#define	RX_DAT		(0x14)
#define	INFO		(0x18)
#define	IE		(0x1C)
#define	INT		(0x24)
#define	STATUS		(0x2C)

#define	CFG_STOP_2BIT	(1 << 2)
#define	CFG_PARITY_EVEN	(1 << 3)
#define	CFG_PARITY_EN	(1 << 4)
#define	CFG_TX_EN	(1 << 5)
#define	CFG_RX_EN	(1 << 6)
#define	CFG_UART_EN	(1 << 7)
#define	CFG_BAUD_SART	8
#define	CFG_DATA_8BIT	0x3

#define	INFO_TX_EMPTY	(1 << 3)
#define	INFO_TX_FULL	(1 << 2)
#define	INFO_RX_EMPTY	(1 << 1)
#define	INFO_RX_FULL	(1 << 0)

#define	RX_DAT_VALID	(1 << 8)


#define UART_NR 4

#ifndef PORT_CORTINA
#define PORT_CORTINA 122
#endif

struct cortina_uart_port {
	struct uart_port	uart;
	char			name[16];
	struct clk		*clk;
};

#define	to_cortina_uart(a)	container_of((a), struct cortina_uart_port, uart)

static struct cortina_uart_port *cortina_uart_ports[UART_NR];

static irqreturn_t cortina_uart_int(int irq, void *dev_id);

static unsigned int cortina_uart_tx_empty(struct uart_port *port)
{
	return (readl(port->membase + INFO) & 0x8) ?  TIOCSER_TEMT : 0;
}

static void cortina_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static unsigned int cortina_uart_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CD;
}

static void cortina_uart_stop_tx(struct uart_port *port)
{
	unsigned int temp;

	temp = readl(port->membase + IE);
	writel(temp & ~CFG_TX_EN, port->membase + IE);
}

static inline void cortina_transmit_buffer(struct uart_port *port)
{
	u8 ch;

	uart_port_tx(port, ch,
		!(readl(port->membase + INFO) & INFO_TX_FULL),
		writel(ch, port->membase + TX_DAT));
}

static void cortina_uart_start_tx(struct uart_port *port)
{
	unsigned long temp;

	temp = readl(port->membase + IE);
	writel((temp | CFG_TX_EN), port->membase + IE);

	if (readl(port->membase + INFO) & INFO_TX_EMPTY)
		cortina_transmit_buffer(port);
}

static void cortina_uart_stop_rx(struct uart_port *port)
{
	unsigned long temp;

	temp = readl(port->membase + IE);
	writel(temp & ~CFG_RX_EN, port->membase + IE);
}

static void __maybe_unused cortina_uart_start_rx(struct uart_port *port)
{
	unsigned long temp;

	temp = readl(port->membase + IE);
	writel(temp | CFG_RX_EN, port->membase + IE);
}

static void cortina_uart_enable_ms(struct uart_port *port)
{
}

static void cortina_uart_break_ctl(struct uart_port *port, int ctl)
{
}

static int cortina_uart_startup(struct uart_port *port)
{
	unsigned long temp;
	int retval;
	struct cortina_uart_port *cortina_uart = to_cortina_uart(port);

	temp = readl(port->membase + IE);
	writel(temp & 0, port->membase + IE);

	retval = request_irq(port->irq, cortina_uart_int, 0, "cortina_uart", port);
	if (retval)
		return retval;

	port->uartclk = clk_get_rate(cortina_uart->clk);

	temp = readl(port->membase + CFG);
	temp |= (CFG_UART_EN | CFG_TX_EN | CFG_RX_EN | CFG_DATA_8BIT);
	writel(temp, port->membase + CFG);
	temp = readl(port->membase + IE);
	writel(temp | CFG_TX_EN | CFG_RX_EN, port->membase + IE);
	return 0;
}

static void cortina_uart_shutdown(struct uart_port *port)
{
	cortina_uart_stop_tx(port);
	cortina_uart_stop_rx(port);
	free_irq(port->irq, port);
}

static void __maybe_unused cortina_uart_set_clock(struct uart_port *port, unsigned int freq)
{
	struct tty_struct *tty = port->state->port.tty;
	unsigned int cfg;

	port->uartclk = freq;

	if (!tty)
		return;

	freq /= uart_get_baud_rate(port, &tty->termios, NULL, 0, 115200);

	cfg = readl(port->membase + CFG);
	cfg &= (1 << CFG_BAUD_SART) - 1;
	cfg |= freq << CFG_BAUD_SART;

	writel(cfg, port->membase + CFG);
	writel(freq / 2, port->membase + RX_SAMPLE);
}

static void cortina_uart_set_termios(struct uart_port *port, struct ktermios *termios,
				     const struct ktermios *old)
{
	unsigned long flags, temp;
	int baud;
	unsigned int sample_fre = 0;
	struct cortina_uart_port *cortina_uart = to_cortina_uart(port);

	port->uartclk = clk_get_rate(cortina_uart->clk);

	baud = uart_get_baud_rate(port, termios, old, 0, 115200);
	temp = readl(port->membase + CFG);
	temp &= 0xff;
	switch (baud) {
	case 9600:
		temp |= (port->uartclk / 9600) << CFG_BAUD_SART;
		break;
	case 19200:
		temp |= (port->uartclk / 19200) << CFG_BAUD_SART;
		break;
	case 38400:
		temp |= (port->uartclk / 38400) << CFG_BAUD_SART;
		break;
	case 57600:
		temp |= (port->uartclk / 57600) << CFG_BAUD_SART;
		break;
	case 115200:
		temp |= (port->uartclk / 115200) << CFG_BAUD_SART;
		break;
	default:
		temp |= (port->uartclk / 38400) << CFG_BAUD_SART;
		break;
	}

	sample_fre = (temp >> CFG_BAUD_SART) / 2;
	temp &= 0xfffffffc;
	switch (termios->c_cflag & CSIZE) {
	case CS5:
		temp |= 0x0;
		break;
	case CS6:
		temp |= 0x1;
		break;
	case CS7:
		temp |= 0x2;
		break;
	case CS8:
	default:
		temp |= 0x3;
		break;
	}

	temp &= ~(CFG_STOP_2BIT);
	if (termios->c_cflag & CSTOPB)
		temp |= CFG_STOP_2BIT;
	temp &= ~(CFG_PARITY_EN);
	temp |= CFG_PARITY_EVEN;
	if (termios->c_cflag & PARENB) {
		temp |= CFG_PARITY_EN;
		if (termios->c_cflag & PARODD)
			temp &= ~(CFG_PARITY_EVEN);
	}

	spin_lock_irqsave(&port->lock, flags);
	writel(temp, port->membase + CFG);
	writel(sample_fre, port->membase + RX_SAMPLE);
	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *cortina_uart_type(struct uart_port *port)
{
	if (port->type != PORT_CORTINA)
		return NULL;

	return container_of(port, struct cortina_uart_port, uart)->name;
}

static void cortina_uart_release_port(struct uart_port *port)
{
}

static int cortina_uart_request_port(struct uart_port *port)
{
	return 0;
}

static void cortina_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_CORTINA;
}

static int cortina_uart_verify_port(struct uart_port *port,
				    struct serial_struct *ser)
{
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_CORTINA)
		return -EINVAL;
	return 0;
}

#if defined(CONFIG_KGDB_SERIAL_CONSOLE) || \
    defined(CONFIG_KGDB_SERIAL_CONSOLE_MODULE)

static int cortina_poll_get_char(struct uart_port *port)
{
	unsigned int rx;

	while (readl(port->membase + INFO) & INFO_RX_EMPTY)
		;
	rx = readl(port->membase + RX_DAT);

	return rx;
}

static void cortina_poll_put_char(struct uart_port *port, unsigned char c)
{
#define wait_tx_done()  while (!(readl(port->membase + INFO) & INFO_TX_EMPTY));
	unsigned int ie = readl(port->membase + IE);

	writel(0, port->membase + IE);
	wait_tx_done();

	writel(c, port->membase + TX_DAT);
	wait_tx_done();

	writel(ie, port->membase + IE);
}

#endif

static const struct uart_ops cortina_uart_ops = {
	.tx_empty = cortina_uart_tx_empty,
	.set_mctrl = cortina_uart_set_mctrl,
	.get_mctrl = cortina_uart_get_mctrl,
	.stop_tx = cortina_uart_stop_tx,
	.start_tx = cortina_uart_start_tx,
	.stop_rx = cortina_uart_stop_rx,
	.enable_ms = cortina_uart_enable_ms,
	.break_ctl = cortina_uart_break_ctl,
	.startup = cortina_uart_startup,
	.shutdown = cortina_uart_shutdown,
	.set_termios = cortina_uart_set_termios,
	.type = cortina_uart_type,
	.release_port = cortina_uart_release_port,
	.request_port = cortina_uart_request_port,
	.config_port = cortina_uart_config_port,
	.verify_port = cortina_uart_verify_port,
#if defined(CONFIG_KGDB_SERIAL_CONSOLE) || \
    defined(CONFIG_KGDB_SERIAL_CONSOLE_MODULE)
	.poll_get_char = cortina_poll_get_char,
	.poll_put_char = cortina_poll_put_char,
#endif
};

static inline void cortina_uart_int_rx_chars(struct uart_port *port)
{
	struct tty_port *tport = &port->state->port;
	unsigned int ch;
	unsigned int rx, flg;

	rx = readl(port->membase + INFO);
	if (INFO_RX_EMPTY & rx)
		return;

	do {
		flg = TTY_NORMAL;
		port->icount.rx++;
		ch = readl(port->membase + RX_DAT);
		if (!(ch & RX_DAT_VALID))
			goto ignore;
		if (uart_handle_sysrq_char(port, (unsigned char)ch))
			goto ignore;
		tty_insert_flip_char(tport, ch, flg);
ignore:
		rx = readl(port->membase + INFO);
	} while (!(INFO_RX_EMPTY & rx));

	tty_flip_buffer_push(tport);
}

static inline void cortina_uart_int_tx_chars(struct uart_port *port)
{
	cortina_transmit_buffer(port);
}

static irqreturn_t cortina_uart_int(int irq, void *dev_id)
{
	struct uart_port *port = (struct uart_port *)dev_id;
	unsigned long flags, temp;

	spin_lock_irqsave(&port->lock, flags);

	temp = readl(port->membase + INT);
	writel(temp, port->membase + INT);

	cortina_uart_int_rx_chars(port);
	cortina_uart_int_tx_chars(port);

	spin_unlock_irqrestore(&port->lock, flags);

	return IRQ_HANDLED;
}

#ifdef CONFIG_SERIAL_CORTINA_CONSOLE

static void cortina_console_write(struct console *co, const char *s,
				  unsigned int count)
{
	struct uart_port *port = &cortina_uart_ports[co->index]->uart;
	unsigned long previous, flags;
	int locked = 1;
	int i;

	if (port->sysrq || oops_in_progress)
		locked = uart_port_trylock_irqsave(port, &flags);
	else
		uart_port_lock_irqsave(port, &flags);

	previous = readl(port->membase + IE);
	cortina_uart_stop_tx(port);

	for (i = 0; i < count; i++) {
		while (!(readl(port->membase + INFO) & INFO_TX_EMPTY))
			udelay(1);

		writel(*s, port->membase + TX_DAT);

		if (*s++ == '\n') {
			while (!(readl(port->membase + INFO) & INFO_TX_EMPTY))
				udelay(1);
			writel('\r', port->membase + TX_DAT);
		}
	}

	writel(previous, port->membase + IE);
	if (locked)
		uart_port_unlock_irqrestore(port, flags);
}

static int __init cortina_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int ret;
	int baud = 9600;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index < 0 || co->index >= UART_NR)
		return -ENODEV;

	if (!cortina_uart_ports[co->index])
		return -ENODEV;

	port = &cortina_uart_ports[co->index]->uart;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	ret = uart_set_options(port, co, baud, parity, bits, flow);

	return 0;
}

static struct uart_driver cortina_uart_driver;

static struct console cortina_console = {
	.name = "ttyS",
	.write = cortina_console_write,
	.device = uart_console_device,
	.setup = cortina_console_setup,
	.flags = CON_PRINTBUFFER,
	.index = -1,
	.data = &cortina_uart_driver,
};

#endif

static struct uart_driver cortina_uart_driver = {
	.owner = THIS_MODULE,
	.driver_name = "cortina_uart",
	.dev_name = "ttyS",
	.nr = UART_NR,
#if 0
	.major = TTY_MAJOR,
	.minor = 64,
#endif
#ifdef CONFIG_SERIAL_CORTINA_CONSOLE
	.cons = &cortina_console,
#endif
};

#ifdef NOT_YET
static cs_status_t cortina_uart_freq_callback(cs_pm_freq_notifier_data_t *data)
{
	struct cortina_uart_port *up = (struct cortina_uart_port *)data->data;
	struct uart_port *port = &up->uart;
	struct tty_struct *tty = port->state->port.tty;
	unsigned int tries, freq;

	if (!port || port->type != PORT_CORTINA)
		return CS_E_OK;

	if (data->event == CS_PM_FREQ_PRECHANGE) {
		dev_dbg(port->dev, "pre-suspend, freq %d\n", port->uartclk);

		if (port->cons)
			console_stop(port->cons);

		if (tty)
			tty_wait_until_sent(tty, port->timeout);
		for (tries = 20000; !cortina_uart_tx_empty(port) && tries; tries--)
			udelay(1);
		if (!tries)
			dev_err(port->dev, "unable to drain transmitter\n");

		spin_lock_irq(&port->lock);
		disable_irq_nosync(port->irq);
		cortina_uart_stop_tx(port);
		cortina_uart_stop_rx(port);
		spin_unlock_irq(&port->lock);
	} else if (data->event == CS_PM_FREQ_POSTCHANGE) {
		switch (data->new_peripheral_clk) {
		case CS_PERIPHERAL_FREQUENCY_150:
			freq = 150000000;
			break;
		case CS_PERIPHERAL_FREQUENCY_170:
			freq = 170000000;
			break;
		default:
			freq = 100000000;
		}

		spin_lock_irq(&port->lock);
		cortina_uart_set_clock(port, freq);
		cortina_uart_start_tx(port);
		cortina_uart_start_rx(port);
		enable_irq(port->irq);
		spin_unlock_irq(&port->lock);

		if (port->cons)
			console_start(port->cons);

		dev_dbg(port->dev, "resumed, freq %d\n", port->uartclk);
	}

	return CS_E_OK;
}

static cs_pm_freq_notifier_t cortina_uart_freq_notifier = {
	.notifier = cortina_uart_freq_callback,
};
#endif

static int serial_cortina_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct resource *mem;
	int irq;
	void __iomem *base;
	struct clk *clk;
	struct cortina_uart_port *port;
	int id = -1;
	int err;

	port = devm_kzalloc(&pdev->dev,
			    sizeof(struct cortina_uart_port), GFP_KERNEL);
	if (!port)
		return -ENOMEM;

	if (pdev->dev.of_node)
		id = of_alias_get_id(pdev->dev.of_node, "serial");
	if (id < 0)
		id = pdev->id;
	if (id < 0 || id >= UART_NR)
		return -EINVAL;
	if (pdev->id < 0)
		pdev->id = id;

	snprintf(port->name, sizeof(port->name), "Cortina UART%d", id);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(base)) {
		dev_err(&pdev->dev, "no memory resource\n");
		return PTR_ERR(base);
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no irq resource\n");
		return -ENODEV;
	}

	clk = of_clk_get(np, 0);
	if (!IS_ERR(clk)) {
		err = clk_prepare_enable(clk);
		if (err)
			goto l_unmap;
	} else {
		pr_warn("cs75xx-uart: clk not found\n");
		err = -EINVAL;
		goto l_unmap;
	}

	port->clk = clk;
	port->uart.irq = irq;
	port->uart.iotype = UPIO_MEM;
	port->uart.membase = base;
	port->uart.mapbase = mem->start;
	port->uart.ops = &cortina_uart_ops;
	port->uart.dev = &pdev->dev;
	port->uart.line = id;
	port->uart.uartclk = clk_get_rate(port->clk);
	port->uart.type = PORT_CORTINA;
	port->uart.has_sysrq = IS_ENABLED(CONFIG_SERIAL_CORTINA_CONSOLE);

	cortina_uart_ports[port->uart.line] = port;

	err = uart_add_one_port(&cortina_uart_driver, &port->uart);
	if (err) {
		cortina_uart_ports[port->uart.line] = NULL;
		goto l_unmap;
	}

#ifdef NOT_YET
	cortina_uart_freq_notifier.data = (void *)port;
	cs_pm_freq_register_notifier(&cortina_uart_freq_notifier, 0);
#endif

	platform_set_drvdata(pdev, port);

	return 0;

l_unmap:
	return err;
}

static void serial_cortina_remove(struct platform_device *pdev)
{
	struct uart_port *port = platform_get_drvdata(pdev);

	if (port) {
#ifdef NOT_YET
		cs_pm_freq_unregister_notifier(&cortina_uart_freq_notifier);
#endif
		uart_remove_one_port(&cortina_uart_driver, port);
	}

	platform_set_drvdata(pdev, NULL);
}

#if defined(CONFIG_OF)
static const struct of_device_id cortina_serial_of_match[] = {
	{ .compatible = "cortina,cs75xx-uart" },
	{},
};
MODULE_DEVICE_TABLE(of, cortina_serial_of_match);
#endif

static struct platform_driver serial_cortina_driver = {
	.probe	  = serial_cortina_probe,
	.remove	 = serial_cortina_remove,
	.driver	 = {
		.name   = "cortina_serial",
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(cortina_serial_of_match),
	},
};

static void cortina_earlycon_putc(struct uart_port *port, unsigned char c)
{
	while (readl(port->membase + INFO) & INFO_TX_FULL)
		cpu_relax();
	writel(c, port->membase + TX_DAT);
}

static void cortina_earlycon_write(struct console *con, const char *s, unsigned int n)
{
	struct earlycon_device *dev = con->data;

	uart_console_write(&dev->port, s, n, cortina_earlycon_putc);
}

static int __init cortina_earlycon_setup(struct earlycon_device *device, const char *opt)
{
	if (!device->port.membase)
		return -ENODEV;

	device->con->write = cortina_earlycon_write;
	return 0;
}
OF_EARLYCON_DECLARE(cortina, "cortina,cs75xx-uart", cortina_earlycon_setup);

static int __init cortina_uart_init(void)
{
	int ret;

	ret = uart_register_driver(&cortina_uart_driver);
	if (ret)
		return ret;

	ret = platform_driver_register(&serial_cortina_driver);
	if (ret)
		uart_unregister_driver(&cortina_uart_driver);

	return ret;
}

static void __exit cortina_uart_exit(void)
{
	platform_driver_unregister(&serial_cortina_driver);
	uart_unregister_driver(&cortina_uart_driver);
}

module_init(cortina_uart_init);
module_exit(cortina_uart_exit);

MODULE_AUTHOR("Cortina-Systems");
MODULE_DESCRIPTION(" Cortina UART driver");
MODULE_LICENSE("GPL");
