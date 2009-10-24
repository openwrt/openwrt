#
# Makefile for the Ralink RT305x SoC specific parts of the kernel
#
# Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= irq.o setup.o devices.o rt305x.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o

obj-$(CONFIG_RT305X_MACH_DIR_300_REVB)	+= mach-dir-300-revb.o
obj-$(CONFIG_RT305X_MACH_V22RW_2X2)	+= mach-v22rw-2x2.o
obj-$(CONFIG_RT305X_MACH_WHR_G300N)	+= mach-whr-g300n.o
obj-$(CONFIG_RT305X_MACH_FONERA20N)	+= mach-fonera20n.o
