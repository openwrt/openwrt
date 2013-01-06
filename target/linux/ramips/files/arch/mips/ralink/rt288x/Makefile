#
# Makefile for the Ralink RT288x SoC specific parts of the kernel
#
# Copyright (C) 2010 Joonas Lahtinen <joonas.lahtinen@gmail.com>
# Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
# Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= irq.o setup.o rt288x.o devices.o clock.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o

obj-$(CONFIG_RT288X_MACH_F5D8235_V1)	+= mach-f5d8235-v1.o
obj-$(CONFIG_RT288X_MACH_BR6524N)	+= mach-br6524n.o
obj-$(CONFIG_RT288X_MACH_RT_N15)	+= mach-rt-n15.o
obj-$(CONFIG_RT288X_MACH_V11ST_FE)	+= mach-v11st-fe.o
obj-$(CONFIG_RT288X_MACH_WLI_TX4_AG300N) += mach-wli-tx4-ag300n.o
obj-$(CONFIG_RT288X_MACH_WZR_AGL300NH)	+= mach-wzr-agl300nh.o
