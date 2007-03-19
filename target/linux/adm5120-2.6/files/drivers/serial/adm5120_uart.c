/*
 *	Serial driver for ADM5120 SoC
 *
 *	Derived from drivers/serial/uart00.c
 *	Copyright 2001 Altera Corporation
 *
 *	Some pieces are derived from the ADMtek 2.4 serial driver.
 *	Copyright (C) ADMtek Incorporated, 2003
 *		daniell@admtek.com.tw
 *	Which again was derived from drivers/char/serial.c
 *	Copyright (C) Linus Torvalds et al.
 *
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/console.h>

#define ADM5120_UART_BASE0		0x12600000
#define ADM5120_UART_BASE1		0x12800000
#define ADM5120_UART_SIZE		0x20

#define ADM5120_UART_IRQ0		1
#define ADM5120_UART_IRQ1		2

#define ADM5120_UART_REG(base, reg) \
	(*(volatile u32 *)KSEG1ADDR((base)+(reg)))

#define ADM5120_UARTCLK_FREQ            62500000
#define ADM5120_UART_BAUDDIV(rate)	((unsigned long)(ADM5120_UARTCLK_FREQ/(16*(_rate)) - 1))

#define ADM5120_UART_BAUD115200		ADM5120_UART_BAUDDIV(115200)

#define ADM5120_UART_DATA		0x00
#define ADM5120_UART_RS			0x04
#define ADM5120_UART_LCR_H		0x08
#define ADM5120_UART_LCR_M		0x0c
#define ADM5120_UART_LCR_L		0x10
#define ADM5120_UART_CR			0x14
#define ADM5120_UART_FR			0x18
#define ADM5120_UART_IR			0x1c

#define ADM5120_UART_FE			0x01
#define ADM5120_UART_PE			0x02
#define ADM5120_UART_BE			0x04
#define ADM5120_UART_OE			0x08
#define ADM5120_UART_ERR		0x0f
#define ADM5120_UART_FIFO_EN		0x10
#define ADM5120_UART_EN			0x01
#define ADM5120_UART_TIE		0x20
#define ADM5120_UART_RIE		0x50
#define ADM5120_UART_IE			0x78
#define ADM5120_UART_CTS		0x01
#define ADM5120_UART_DSR		0x02
#define ADM5120_UART_DCD		0x04
#define ADM5120_UART_TXFF		0x20
#define ADM5120_UART_TXFE		0x80
#define ADM5120_UART_RXFE		0x10
#define ADM5120_UART_BRK		0x01
#define ADM5120_UART_PEN		0x02
#define ADM5120_UART_EPS		0x04
#define ADM5120_UART_STP2		0x08
#define ADM5120_UART_W5			0x00
#define ADM5120_UART_W6			0x20
#define ADM5120_UART_W7			0x40
#define ADM5120_UART_W8			0x60
#define ADM5120_UART_MIS		0x01
#define ADM5120_UART_RIS		0x02
#define ADM5120_UART_TIS		0x04
#define ADM5120_UART_RTIS		0x08

static void adm5120ser_stop_tx(struct uart_port *port)
{
	ADM5120_UART_REG(port->iobase, ADM5120_UART_CR) &= ~ADM5120_UART_TIE;
}

static void adm5120ser_irq_rx(struct uart_port *port)
{
	struct tty_struct *tty = port->info->tty;
	unsigned int status, ch, rds, flg, ignored = 0;

	status = ADM5120_UART_REG(port->iobase, ADM5120_UART_FR);
	while (!(status & ADM5120_UART_RXFE)) {
		/* 
		 * We need to read rds before reading the 
		 * character from the fifo
		 */
		rds = ADM5120_UART_REG(port->iobase, ADM5120_UART_RS);
		ch = ADM5120_UART_REG(port->iobase, ADM5120_UART_DATA);
		port->icount.rx++;

		if (tty->low_latency)
			tty_flip_buffer_push(tty);

		flg = TTY_NORMAL;

		/*
		 * Note that the error handling code is
		 * out of the main execution path
		 */
		if (rds & ADM5120_UART_ERR)
			goto handle_error;
		if (uart_handle_sysrq_char(port, ch))
			goto ignore_char;

	error_return:
		tty_insert_flip_char(tty, ch, flg);

	ignore_char:
		status = ADM5120_UART_REG(port->iobase, ADM5120_UART_FR);
	}
 out:
	tty_flip_buffer_push(tty);
	return;

 handle_error:
 	ADM5120_UART_REG(port->iobase, ADM5120_UART_RS) = 0xff;
	if (rds & ADM5120_UART_BE) {
		port->icount.brk++;
		if (uart_handle_break(port))
			goto ignore_char;
	} else if (rds & ADM5120_UART_PE)
		port->icount.parity++;
	else if (rds & ADM5120_UART_FE)
		port->icount.frame++;
	if (rds & ADM5120_UART_OE)
		port->icount.overrun++;

	if (rds & port->ignore_status_mask) {
		if (++ignored > 100)
			goto out;
		goto ignore_char;
	}
	rds &= port->read_status_mask;

	if (rds & ADM5120_UART_BE)
		flg = TTY_BREAK;
	else if (rds & ADM5120_UART_PE)
		flg = TTY_PARITY;
	else if (rds & ADM5120_UART_FE)
		flg = TTY_FRAME;

	if (rds & ADM5120_UART_OE) {
		/*
		 * CHECK: does overrun affect the current character?
		 * ASSUMPTION: it does not.
		 */
		tty_insert_flip_char(tty, ch, flg);
		ch = 0;
		flg = TTY_OVERRUN;
	}
#ifdef CONFIG_MAGIC_SYSRQ
	port->sysrq = 0;
#endif
	goto error_return;
}

static void adm5120ser_irq_tx(struct uart_port *port)
{
	struct circ_buf *xmit = &port->info->xmit;
	int count;

	if (port->x_char) {
		ADM5120_UART_REG(port->iobase, ADM5120_UART_DATA) =
		    port->x_char;
		port->icount.tx++;
		port->x_char = 0;
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(port)) {
		adm5120ser_stop_tx(port);
		return;
	}

	count = port->fifosize >> 1;
	do {
		ADM5120_UART_REG(port->iobase, ADM5120_UART_DATA) =
		    xmit->buf[xmit->tail];
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	if (uart_circ_empty(xmit))
		adm5120ser_stop_tx(port);
}

static void adm5120ser_irq_modem(struct uart_port *port)
{
	unsigned int status;

	status = ADM5120_UART_REG(port->iobase, ADM5120_UART_FR);

	if (status & ADM5120_UART_DCD)
		uart_handle_dcd_change(port, status & ADM5120_UART_DCD);

	if (status & ADM5120_UART_DSR)
		port->icount.dsr++;

	if (status & ADM5120_UART_CTS)
		uart_handle_cts_change(port, status & ADM5120_UART_CTS);

	wake_up_interruptible(&port->info->delta_msr_wait);
}

static irqreturn_t adm5120ser_irq(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	unsigned long ir = ADM5120_UART_REG(port->iobase, ADM5120_UART_IR);

	if (ir & (ADM5120_UART_RIS | ADM5120_UART_RTIS))
		adm5120ser_irq_rx(port);
	if (ir & ADM5120_UART_TIS)
		adm5120ser_irq_tx(port);
	if (ir & ADM5120_UART_MIS) {
		adm5120ser_irq_modem(port);
		ADM5120_UART_REG(port->iobase, ADM5120_UART_IR) = 0xff;
	}

	return IRQ_HANDLED;
}

static unsigned int adm5120ser_tx_empty(struct uart_port *port)
{
	unsigned int fr = ADM5120_UART_REG(port->iobase, ADM5120_UART_FR);
	return (fr & ADM5120_UART_TXFE) ? TIOCSER_TEMT : 0;
}

static void adm5120ser_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static unsigned int adm5120ser_get_mctrl(struct uart_port *port)
{
	unsigned int result = 0;
	unsigned int fr = ADM5120_UART_REG(port->iobase, ADM5120_UART_FR);

	if (fr & ADM5120_UART_CTS)
		result |= TIOCM_CTS;
	if (fr & ADM5120_UART_DSR)
		result |= TIOCM_DSR;
	if (fr & ADM5120_UART_DCD)
		result |= TIOCM_CAR;
	return result;
}

static void adm5120ser_start_tx(struct uart_port *port)
{
	ADM5120_UART_REG(port->iobase, ADM5120_UART_CR) |= ADM5120_UART_TIE;
}

static void adm5120ser_stop_rx(struct uart_port *port)
{
	ADM5120_UART_REG(port->iobase, ADM5120_UART_CR) &= ~ADM5120_UART_RIE;
}

static void adm5120ser_enable_ms(struct uart_port *port)
{
}

static void adm5120ser_break_ctl(struct uart_port *port, int break_state)
{
	unsigned long flags;
	unsigned long lcrh;

	spin_lock_irqsave(&port->lock, flags);
	lcrh = ADM5120_UART_REG(port->iobase, ADM5120_UART_LCR_H);
	if (break_state == -1)
		lcrh |= ADM5120_UART_BRK;
	else
		lcrh &= ~ADM5120_UART_BRK;
	ADM5120_UART_REG(port->iobase, ADM5120_UART_LCR_H) = lcrh;
	spin_unlock_irqrestore(&port->lock, flags);
}

static int adm5120ser_startup(struct uart_port *port)
{
	int ret;

	ret = request_irq(port->irq, adm5120ser_irq, 0, "ADM5120 UART", port);
	if (ret) {
		printk(KERN_ERR "Couldn't get irq %d\n", port->irq);
		return ret;
	}
	ADM5120_UART_REG(port->iobase, ADM5120_UART_LCR_H) |=
	    ADM5120_UART_FIFO_EN;
	ADM5120_UART_REG(port->iobase, ADM5120_UART_CR) |=
	    ADM5120_UART_EN | ADM5120_UART_IE;
	return 0;
}

static void adm5120ser_shutdown(struct uart_port *port)
{
	ADM5120_UART_REG(port->iobase, ADM5120_UART_CR) &= ~ADM5120_UART_IE;
	free_irq(port->irq, port);
}

static void adm5120ser_set_termios(struct uart_port *port,
    struct termios *termios, struct termios *old)
{
	unsigned int baud, quot, lcrh;
	unsigned long flags;

	termios->c_cflag |= CREAD;

	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk/16);
	quot = uart_get_divisor(port, baud);

	lcrh = ADM5120_UART_FIFO_EN;
	switch (termios->c_cflag & CSIZE) {
		case CS5:
			lcrh |= ADM5120_UART_W5;
			break;
		case CS6:
			lcrh |= ADM5120_UART_W6;
			break;
		case CS7:
			lcrh |= ADM5120_UART_W7;
			break;
		default:
			lcrh |= ADM5120_UART_W8;
			break;
	}
	if (termios->c_cflag & CSTOPB)
		lcrh |= ADM5120_UART_STP2;
	if (termios->c_cflag & PARENB) {
		lcrh |= ADM5120_UART_PEN;
		if (!(termios->c_cflag & PARODD))
			lcrh |= ADM5120_UART_EPS;
	}

	spin_lock_irqsave(port->lock, flags);

	ADM5120_UART_REG(port->iobase, ADM5120_UART_LCR_H) = lcrh;

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	port->read_status_mask = ADM5120_UART_OE;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= ADM5120_UART_FE | ADM5120_UART_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		port->read_status_mask |= ADM5120_UART_BE;

	/*
	 * Characters to ignore
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= ADM5120_UART_FE | ADM5120_UART_PE;
	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= ADM5120_UART_BE;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns to (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= ADM5120_UART_OE;
	}

	quot = ADM5120_UART_BAUD115200;
	ADM5120_UART_REG(port->iobase, ADM5120_UART_LCR_L) = quot & 0xff;
	ADM5120_UART_REG(port->iobase, ADM5120_UART_LCR_M) = quot >> 8;

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *adm5120ser_type(struct uart_port *port)
{
	return port->type == PORT_ADM5120 ? "ADM5120" : NULL;
}

static void adm5120ser_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_ADM5120;
}

static void adm5120ser_release_port(struct uart_port *port)
{
	release_mem_region(port->iobase, ADM5120_UART_SIZE);
}

static int adm5120ser_request_port(struct uart_port *port)
{
	return request_mem_region(port->iobase, ADM5120_UART_SIZE,
	    "adm5120-uart") != NULL ? 0 : -EBUSY; 
}

static struct uart_ops adm5120ser_ops = {
	.tx_empty =	adm5120ser_tx_empty,
	.set_mctrl =	adm5120ser_set_mctrl,
	.get_mctrl =	adm5120ser_get_mctrl,
	.stop_tx =	adm5120ser_stop_tx,
	.start_tx =	adm5120ser_start_tx,
	.stop_rx =	adm5120ser_stop_rx,
	.enable_ms =	adm5120ser_enable_ms,
	.break_ctl =	adm5120ser_break_ctl,
	.startup =	adm5120ser_startup,
	.shutdown =	adm5120ser_shutdown,
	.set_termios =	adm5120ser_set_termios,
	.type =		adm5120ser_type,
	.config_port =	adm5120ser_config_port,
	.release_port =	adm5120ser_release_port,
	.request_port =	adm5120ser_request_port,
};

static void adm5120console_put(const char c)
{
	while ((ADM5120_UART_REG(ADM5120_UART_BASE0, ADM5120_UART_FR) &
	     ADM5120_UART_TXFF) != 0);
	ADM5120_UART_REG(ADM5120_UART_BASE0, ADM5120_UART_DATA) = c;
}

static void adm5120console_write(struct console *con, const char *s,
    unsigned int count)
{
	while (count--) {
		if (*s == '\n')
			adm5120console_put('\r');
		adm5120console_put(*s);
		s++;
	}
}

static int __init adm5120console_setup(struct console *con, char *options)
{
	/* Set to 115200 baud, 8N1 and enable FIFO */
	ADM5120_UART_REG(ADM5120_UART_BASE0, ADM5120_UART_LCR_L) =
	    ADM5120_UART_BAUD115200 & 0xff;
	ADM5120_UART_REG(ADM5120_UART_BASE0, ADM5120_UART_LCR_M) =
	    ADM5120_UART_BAUD115200 >> 8;
	ADM5120_UART_REG(ADM5120_UART_BASE0, ADM5120_UART_LCR_H) =
	    ADM5120_UART_W8 | ADM5120_UART_FIFO_EN;
	/* Enable port */
	ADM5120_UART_REG(ADM5120_UART_BASE0, ADM5120_UART_CR) =
	    ADM5120_UART_EN;

	return 0;
}

static struct uart_driver adm5120ser_reg;

static struct console adm5120_serconsole = {
	.name =		"ttyS",
	.write =	adm5120console_write,
	.device =	uart_console_device,
	.setup =	adm5120console_setup,
	.flags =	CON_PRINTBUFFER,
	.cflag =	B115200 | CS8 | CREAD,
	.index =	0,
	.data =		&adm5120ser_reg,
};

static int __init adm5120console_init(void)
{
	register_console(&adm5120_serconsole);
	return 0;
}

console_initcall(adm5120console_init);


static struct uart_port adm5120ser_ports[] = {
	{
		.iobase =	ADM5120_UART_BASE0,
		.irq =		ADM5120_UART_IRQ0,
		.uartclk =	ADM5120_UARTCLK_FREQ,
		.fifosize =	16,
		.ops =		&adm5120ser_ops,
		.line =		0,
		.flags =	ASYNC_BOOT_AUTOCONF,
	},
#if (CONFIG_ADM5120_NR_UARTS > 1)
	{
		.iobase =	ADM5120_UART_BASE1,
		.irq =		ADM5120_UART_IRQ1,
		.uartclk =	ADM5120_UARTCLK_FREQ,
		.fifosize =	16,
		.ops =		&adm5120ser_ops,
		.line =		1,
		.flags =	ASYNC_BOOT_AUTOCONF,
	},
#endif
};

static struct uart_driver adm5120ser_reg = {
	.owner	=	THIS_MODULE,
	.driver_name =	"ttyS",
	.dev_name =	"ttyS",
	.major =	TTY_MAJOR,
	.minor =	64,
	.nr =		CONFIG_ADM5120_NR_UARTS,
	.cons =		&adm5120_serconsole,
};

static int __init adm5120ser_init(void)
{
	int ret, i;

	ret = uart_register_driver(&adm5120ser_reg);
	if (!ret) {
		for (i = 0; i < CONFIG_ADM5120_NR_UARTS; i++)
			uart_add_one_port(&adm5120ser_reg, &adm5120ser_ports[i]);
	}

	return ret;
}

__initcall(adm5120ser_init);
