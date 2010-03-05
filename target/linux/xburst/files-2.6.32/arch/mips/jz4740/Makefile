#
# Makefile for the Ingenic JZ4740.
#

# Object file lists.

obj-y += prom.o irq.o time.o reset.o setup.o dma.o \
	gpio.o clock.o platform.o

obj-$(CONFIG_DEBUG_FS) += clock-debugfs.o

# board specific support

obj-$(CONFIG_JZ4740_QI_LB60)	+= board-qi_lb60.o
obj-$(CONFIG_JZ4740_N516)	+= board-n516.o board-n516-display.o
obj-$(CONFIG_JZ4740_N526)	+= board-n526.o

# PM support

obj-$(CONFIG_PM)         += pm.o

