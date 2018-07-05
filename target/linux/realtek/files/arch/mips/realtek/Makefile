#
# Makefile for the Realtek RLX based SoC specific parts of the kernel
#
# Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= prom.o setup.o irq.o common.o clock.o
obj-y	+= cevt-rtl819x.o csrc-rtl819x.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o
obj-$(CONFIG_PCI)			+= pci.o

#
# Devices
#
obj-y					+= serial.o gpio.o
obj-y					+= dev-spi.o
obj-y					+= dev-m25p80.o
obj-y					+= dev-usb.o
obj-y					+= dev-eth.o
obj-y					+= dev-gpio-buttons.o
obj-y					+= dev-leds-gpio.o

#
# Machines
#
obj-$(CONFIG_REALTEK_MACH_AU_HOME_SPOT_CUBE)		+= mach-au-home-spot-cube.o

