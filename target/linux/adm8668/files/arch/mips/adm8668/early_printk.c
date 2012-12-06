#include <linux/io.h>
#include <linux/serial_core.h>
#include <adm8668.h>

#define UART_READ(r) \
	__raw_readl((void __iomem *)(KSEG1ADDR(ADM8668_UART0_BASE) + (r)))

#define UART_WRITE(v, r) \
	__raw_writel((v), (void __iomem *)(KSEG1ADDR(ADM8668_UART0_BASE) + (r)))

void prom_putchar(char c)
{
	UART_WRITE(c, UART_DR_REG);
	while ((UART_READ(UART_FR_REG) & UART_TX_FIFO_FULL) != 0)
		;
}
