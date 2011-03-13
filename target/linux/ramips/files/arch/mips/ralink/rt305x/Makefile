#
# Makefile for the Ralink RT305x SoC specific parts of the kernel
#
# Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= irq.o setup.o devices.o rt305x.o clock.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o

obj-$(CONFIG_RT305X_MACH_F5D8235_V2)    += mach-f5d8235-v2.o
obj-$(CONFIG_RT305X_MACH_DIR_300_REVB)	+= mach-dir-300-revb.o
obj-$(CONFIG_RT305X_MACH_V22RW_2X2)	+= mach-v22rw-2x2.o
obj-$(CONFIG_RT305X_MACH_WCR150GN)	+= mach-wcr150gn.o
obj-$(CONFIG_RT305X_MACH_WHR_G300N)	+= mach-whr-g300n.o
obj-$(CONFIG_RT305X_MACH_FONERA20N)	+= mach-fonera20n.o
obj-$(CONFIG_RT305X_MACH_PWH2004)	+= mach-pwh2004.o
obj-$(CONFIG_RT305X_MACH_HW550_3G)	+= mach-hw550-3g.o
