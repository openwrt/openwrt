#include <linux/console.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/serial_core.h>

#define ZX279128_UART_TX		0x04
#define ZX279128_UART_STATUS	0x14
#define ZX279128_UART_TX_FULL	0x20

static void zx279128_early_putc(struct uart_port *port, unsigned char character)
{
	while (readl(port->membase + ZX279128_UART_STATUS) & ZX279128_UART_TX_FULL)
		cpu_relax();
	writel(character, port->membase + ZX279128_UART_TX);
}

static void zx279128_early_write(struct console *console, const char *text,
				unsigned int count)
{
	struct earlycon_device *device = console->data;

	uart_console_write(&device->port, text, count, zx279128_early_putc);
}

static int __init zx279128_early_setup(struct earlycon_device *device,
				     const char *options)
{
	if (!device->port.membase)
		return -ENODEV;

	device->con->write = zx279128_early_write;
	return 0;
}

OF_EARLYCON_DECLARE(zx279128, "zte,ZX279127-uart", zx279128_early_setup);
