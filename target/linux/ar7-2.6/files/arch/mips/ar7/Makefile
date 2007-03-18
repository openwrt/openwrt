
obj-y := \
	prom.o \
	setup.o \
	memory.o \
	irq.o \
	time.o \
	platform.o \
	gpio.o \
	vlynq.o

obj-$(CONFIG_PCI) += \
	vlynq-pci.o

EXTRA_AFLAGS := $(CFLAGS)
