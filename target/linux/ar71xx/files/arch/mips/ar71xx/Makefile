#
# Makefile for the Atheros AR71xx SoC specific parts of the kernel
#
# Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
# Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= prom.o irq.o setup.o platform.o gpio.o ar71xx.o

obj-$(CONFIG_AR71XX_MACH_GENERIC)	+= mach-generic.o
obj-$(CONFIG_AR71XX_MACH_RB_4XX)	+= mach-rb-4xx.o
obj-$(CONFIG_AR71XX_MACH_WP543)		+= mach-wp543.o
