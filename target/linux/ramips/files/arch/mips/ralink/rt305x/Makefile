#
# Makefile for the Ralink RT305x SoC specific parts of the kernel
#
# Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= prom.o irq.o setup.o devices.o rt305x.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o

obj-$(CONFIG_RT305X_MACH_WHR_G300N)	+= mach-whr-g300n.o
