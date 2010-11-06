/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * ADM8668 serial driver, totally ripped the source from BCM63xx and changed
 *  all the registers to fit our hardware, and removed all the features that
 *  I didn't know because our GPL'd serial driver was way lame.
 *
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 * Derived directly from bcm63xx_uart
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 *
 */

#if defined(CONFIG_SERIAL_ADM8668_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/clk.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/sysrq.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <adm8668.h>

#define ADM8668_NR_UARTS	1

static struct uart_port ports[ADM8668_NR_UARTS];

/*
 * handy uart register accessor
 */
static inline unsigned int adm_uart_readl(struct uart_port *port,
					 unsigned int offset)
{
	return (*(volatile unsigned int *)(port->membase + offset));
}

static inline void adm_uart_writel(struct uart_port *port,
				  unsigned int value, unsigned int offset)
{
	(*((volatile unsigned int *)(port->membase + offset))) = value;
}

/*
 * serial core request to check if uart tx fifo is empty
 */
static unsigned int adm_uart_tx_empty(struct uart_port *port)
{
	/* we always wait for completion, no buffer is made... */
	return 1;
}

/*
 * serial core request to set RTS and DTR pin state and loopback mode
 */
static void adm_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

/*
 * serial core request to return RI, CTS, DCD and DSR pin state
 */
static unsigned int adm_uart_get_mctrl(struct uart_port *port)
{
	return 0;
}

/*
 * serial core request to disable tx ASAP (used for flow control)
 */
static void adm_uart_stop_tx(struct uart_port *port)
{
	unsigned int val;

	val = adm_uart_readl(port, UART_CR_REG);
	val &= ~(UART_TX_INT_EN);
	adm_uart_writel(port, val, UART_CR_REG);
}

/*
 * serial core request to (re)enable tx
 */
static void adm_uart_start_tx(struct uart_port *port)
{
	unsigned int val;

	val = adm_uart_readl(port, UART_CR_REG);
	val |= UART_TX_INT_EN;
	adm_uart_writel(port, val, UART_CR_REG);
}

/*
 * serial core request to stop rx, called before port shutdown
 */
static void adm_uart_stop_rx(struct uart_port *port)
{
	unsigned int val;

	val = adm_uart_readl(port, UART_CR_REG);
	val &= ~UART_RX_INT_EN;
	adm_uart_writel(port, val, UART_CR_REG);
}

/*
 * serial core request to enable modem status interrupt reporting
 */
static void adm_uart_enable_ms(struct uart_port *port)
{
}

/*
 * serial core request to start/stop emitting break char
 */
static void adm_uart_break_ctl(struct uart_port *port, int ctl)
{
}

/*
 * return port type in string format
 */
static const char *adm_uart_type(struct uart_port *port)
{
	return (port->type == PORT_ADM8668) ? "adm8668_uart" : NULL;
}

/*
 * read all chars in rx fifo and send them to core
 */
static void adm_uart_do_rx(struct uart_port *port)
{
	struct tty_struct *tty;
	unsigned int max_count;

	/* limit number of char read in interrupt, should not be
	 * higher than fifo size anyway since we're much faster than
	 * serial port */
	max_count = 32;
	tty = port->state->port.tty;
	do {
		unsigned int iestat, c, cstat;
		char flag;

		/* get overrun/fifo empty information from ier
		 * register */
		iestat = adm_uart_readl(port, UART_FR_REG);
		if (iestat & UART_RX_FIFO_EMPTY)
			break;

		/* recieve status */
		cstat = adm_uart_readl(port, UART_RSR_REG);
		/* clear errors */
		adm_uart_writel(port, cstat, UART_RSR_REG);

		c = adm_uart_readl(port, UART_DR_REG);
		port->icount.rx++;
		flag = TTY_NORMAL;

		if (unlikely((cstat & UART_RX_STATUS_MASK))) {
			/* do stats first */
			if (cstat & UART_BREAK_ERR) {
				port->icount.brk++;
				if (uart_handle_break(port))
					continue;
			}

			if (cstat & UART_PARITY_ERR)
				port->icount.parity++;
			if (cstat & UART_FRAMING_ERR)
				port->icount.frame++;
			if (cstat & UART_OVERRUN_ERR) {
				port->icount.overrun++;
				tty_insert_flip_char(tty, 0, TTY_OVERRUN);
			}

			/* update flag wrt read_status_mask */
			cstat &= port->read_status_mask;
			if (cstat & UART_BREAK_ERR)
				flag = TTY_BREAK;
			if (cstat & UART_FRAMING_ERR)
				flag = TTY_FRAME;
			if (cstat & UART_PARITY_ERR)
				flag = TTY_PARITY;
		}

		if (uart_handle_sysrq_char(port, c))
			continue;

		/* fixthis */
		if ((cstat & port->ignore_status_mask) == 0)
			tty_insert_flip_char(tty, c, flag);
	} while (max_count--);

	tty_flip_buffer_push(tty);
}

/*
 * fill tx fifo with chars to send, stop when fifo is about to be full
 * or when all chars have been sent.
 */
static void adm_uart_do_tx(struct uart_port *port)
{
	struct circ_buf *xmit;

	if (port->x_char) {
		adm_uart_writel(port, port->x_char, UART_DR_REG);
		port->icount.tx++;
		port->x_char = 0;
		return;
	}

	if (uart_tx_stopped(port))
		adm_uart_stop_tx(port);

	xmit = &port->state->xmit;

	if (uart_circ_empty(xmit))
		goto txq_empty;
	do
	{
		while ((adm_uart_readl(port, UART_FR_REG) & UART_TX_FIFO_FULL) != 0)
			;
		adm_uart_writel(port, xmit->buf[xmit->tail], UART_DR_REG);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (1);

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	if (uart_circ_empty(xmit))
		goto txq_empty;

	return;

txq_empty:
	adm_uart_stop_tx(port);
}

/*
 * process uart interrupt
 */
static irqreturn_t adm_uart_interrupt(int irq, void *dev_id)
{
	struct uart_port *port;
	unsigned int irqstat;

	port = dev_id;
	spin_lock(&port->lock);

	irqstat = adm_uart_readl(port, UART_IIR_REG);

	if (irqstat & (UART_RX_INT|UART_RX_TIMEOUT_INT)) {
		adm_uart_do_rx(port);
	}

	if (irqstat & UART_TX_INT) {
		adm_uart_do_tx(port);
	}
	spin_unlock(&port->lock);
	return IRQ_HANDLED;
}

/*
 * enable rx & tx operation on uart
 */
static void adm_uart_enable(struct uart_port *port)
{
	unsigned int val;

	val = adm_uart_readl(port, UART_CR_REG);
	// BREAK_INT too
	val |= (UART_RX_INT_EN | UART_RX_TIMEOUT_INT_EN);
	adm_uart_writel(port, val, UART_CR_REG);
}

/*
 * disable rx & tx operation on uart
 */
static void adm_uart_disable(struct uart_port *port)
{
	unsigned int val;

	val = adm_uart_readl(port, UART_CR_REG);
	val &= ~(UART_TX_INT_EN | UART_RX_INT_EN | UART_RX_TIMEOUT_INT_EN);
	adm_uart_writel(port, val, UART_CR_REG);
}

/*
 * clear all unread data in rx fifo and unsent data in tx fifo
 */
static void adm_uart_flush(struct uart_port *port)
{
	/* read any pending char to make sure all irq status are
	 * cleared */
	(void)adm_uart_readl(port, UART_DR_REG);
}

/*
 * serial core request to initialize uart and start rx operation
 */
static int adm_uart_startup(struct uart_port *port)
{
	int ret;

	/* clear any pending external input interrupt */
	(void)adm_uart_readl(port, UART_IIR_REG);

	/* register irq and enable rx interrupts */
	ret = request_irq(port->irq, adm_uart_interrupt, 0,
			  adm_uart_type(port), port);
	if (ret)
		return ret;

	adm_uart_enable(port);

	return 0;
}

/*
 * serial core request to flush & disable uart
 */
static void adm_uart_shutdown(struct uart_port *port)
{
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);
//	adm_uart_writel(port, 0, UART_CR_REG);
	spin_unlock_irqrestore(&port->lock, flags);

	adm_uart_disable(port);
	adm_uart_flush(port);
	free_irq(port->irq, port);
}

/*
 * serial core request to change current uart setting
 */
static void adm_uart_set_termios(struct uart_port *port,
				 struct ktermios *new,
				 struct ktermios *old)
{
	port->ignore_status_mask = 0;
	uart_update_timeout(port, new->c_cflag, 115200);
}

/*
 * serial core request to claim uart iomem
 */
static int adm_uart_request_port(struct uart_port *port)
{
	unsigned int size = 0xf;
	if (!request_mem_region(port->mapbase, size, "adm8668")) {
		dev_err(port->dev, "Memory region busy\n");
		return -EBUSY;
	}

	port->membase = ioremap(port->mapbase, size);
	if (!port->membase) {
		dev_err(port->dev, "Unable to map registers\n");
		release_mem_region(port->mapbase, size);
		return -EBUSY;
	}
	return 0;
}

/*
 * serial core request to release uart iomem
 */
static void adm_uart_release_port(struct uart_port *port)
{
	release_mem_region(port->mapbase, 0xf);
	iounmap(port->membase);
}

/*
 * serial core request to do any port required autoconfiguration
 */
static void adm_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		if (adm_uart_request_port(port))
			return;
		port->type = PORT_ADM8668;
	}
}

/*
 * serial core request to check that port information in serinfo are
 * suitable
 */
static int adm_uart_verify_port(struct uart_port *port,
				struct serial_struct *serinfo)
{
	if (port->type != PORT_ADM8668)
		return -EINVAL;
	if (port->irq != serinfo->irq)
		return -EINVAL;
	if (port->iotype != serinfo->io_type)
		return -EINVAL;
	if (port->mapbase != (unsigned long)serinfo->iomem_base)
		return -EINVAL;
	return 0;
}

/* serial core callbacks */
static struct uart_ops adm_uart_ops = {
	.tx_empty	= adm_uart_tx_empty,
	.get_mctrl	= adm_uart_get_mctrl,
	.set_mctrl	= adm_uart_set_mctrl,
	.start_tx	= adm_uart_start_tx,
	.stop_tx	= adm_uart_stop_tx,
	.stop_rx	= adm_uart_stop_rx,
	.enable_ms	= adm_uart_enable_ms,
	.break_ctl	= adm_uart_break_ctl,
	.startup	= adm_uart_startup,
	.shutdown	= adm_uart_shutdown,
	.set_termios	= adm_uart_set_termios,
	.type		= adm_uart_type,
	.release_port	= adm_uart_release_port,
	.request_port	= adm_uart_request_port,
	.config_port	= adm_uart_config_port,
	.verify_port	= adm_uart_verify_port,
};

#ifdef CONFIG_SERIAL_ADM8668_CONSOLE
static inline void wait_for_xmitr(struct uart_port *port)
{
        while ((adm_uart_readl(port, UART_FR_REG) & UART_TX_FIFO_FULL) != 0)
                ;
}

/*
 * output given char
 */
static void adm_console_putchar(struct uart_port *port, int ch)
{
	wait_for_xmitr(port);
	adm_uart_writel(port, ch, UART_DR_REG);
}

/*
 * console core request to output given string
 */
static void adm_console_write(struct console *co, const char *s,
			      unsigned int count)
{
	struct uart_port *port;
	unsigned long flags;
	int locked;

	port = &ports[co->index];

	local_irq_save(flags);
	if (port->sysrq) {
		/* adm_uart_interrupt() already took the lock */
		locked = 0;
	} else if (oops_in_progress) {
		locked = spin_trylock(&port->lock);
	} else {
		spin_lock(&port->lock);
		locked = 1;
	}

	/* call helper to deal with \r\n */
	uart_console_write(port, s, count, adm_console_putchar);

	/* and wait for char to be transmitted */
	wait_for_xmitr(port);

	if (locked)
		spin_unlock(&port->lock);
	local_irq_restore(flags);
}

/*
 * console core request to setup given console, find matching uart
 * port and setup it.
 */
static int adm_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index < 0 || co->index >= ADM8668_NR_UARTS)
		return -EINVAL;
	port = &ports[co->index];
	if (!port->membase)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver adm_uart_driver;

static struct console adm8668_console = {
	.name		= "ttyS",
	.write		= adm_console_write,
	.device		= uart_console_device,
	.setup		= adm_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &adm_uart_driver,
};

static int __init adm8668_console_init(void)
{
	register_console(&adm8668_console);
	return 0;
}

console_initcall(adm8668_console_init);

#define ADM8668_CONSOLE	(&adm8668_console)
#else
#define ADM8668_CONSOLE	NULL
#endif /* CONFIG_SERIAL_ADM8668_CONSOLE */

static struct uart_driver adm_uart_driver = {
	.owner		= THIS_MODULE,
	.driver_name	= "adm8668_uart",
	.dev_name	= "ttyS",
	.major		= TTY_MAJOR,
	.minor		= 64,
	.nr		= 1,
	.cons		= ADM8668_CONSOLE,
};

/*
 * platform driver probe/remove callback
 */
static int __devinit adm_uart_probe(struct platform_device *pdev)
{
	struct resource *res_mem, *res_irq;
	struct uart_port *port;
	int ret;

	if (pdev->id < 0 || pdev->id >= ADM8668_NR_UARTS)
		return -EINVAL;

	if (ports[pdev->id].membase)
		return -EBUSY;

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res_mem)
		return -ENODEV;

	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_irq)
		return -ENODEV;

	port = &ports[pdev->id];
	memset(port, 0, sizeof(*port));
	port->iotype = UPIO_MEM;
	port->mapbase = res_mem->start;
	port->irq = res_irq->start;
	port->ops = &adm_uart_ops;
	port->flags = UPF_BOOT_AUTOCONF;
	port->dev = &pdev->dev;
	port->fifosize = 8;
	port->uartclk = ADM8668_UARTCLK_FREQ;

	ret = uart_add_one_port(&adm_uart_driver, port);
	if (ret) {
		ports[pdev->id].membase = 0;
		return ret;
	}
	platform_set_drvdata(pdev, port);
	return 0;
}

static int __devexit adm_uart_remove(struct platform_device *pdev)
{
	struct uart_port *port;

	port = platform_get_drvdata(pdev);
	uart_remove_one_port(&adm_uart_driver, port);
	platform_set_drvdata(pdev, NULL);
	/* mark port as free */
	ports[pdev->id].membase = 0;
	return 0;
}

/*
 * platform driver stuff
 */
static struct platform_driver adm_uart_platform_driver = {
	.probe	= adm_uart_probe,
	.remove	= __devexit_p(adm_uart_remove),
	.driver	= {
		.owner = THIS_MODULE,
		.name  = "adm8668_uart",
	},
};

static int __init adm_uart_init(void)
{
	int ret;

	ret = uart_register_driver(&adm_uart_driver);
	if (ret)
		return ret;

	ret = platform_driver_register(&adm_uart_platform_driver);
	if (ret)
		uart_unregister_driver(&adm_uart_driver);

	return ret;
}

static void __exit adm_uart_exit(void)
{
	platform_driver_unregister(&adm_uart_platform_driver);
	uart_unregister_driver(&adm_uart_driver);
}

module_init(adm_uart_init);
module_exit(adm_uart_exit);

MODULE_AUTHOR("Scott Nicholas <neutronscott@scottn.us>");
MODULE_DESCRIPTION("ADM8668 integrated uart driver");
MODULE_LICENSE("GPL");
