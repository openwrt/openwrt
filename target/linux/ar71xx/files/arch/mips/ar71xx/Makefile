#
# Makefile for the Atheros AR71xx SoC specific parts of the kernel
#
# Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
# Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= prom.o irq.o setup.o devices.o gpio.o ar71xx.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o
obj-$(CONFIG_PCI)			+= pci.o

obj-$(CONFIG_AR71XX_DEV_AP94_PCI)	+= dev-ap94-pci.o
obj-$(CONFIG_AR71XX_DEV_AR913X_WMAC)	+= dev-ar913x-wmac.o
obj-$(CONFIG_AR71XX_DEV_M25P80)		+= dev-m25p80.o

obj-$(CONFIG_AR71XX_MACH_AP81)		+= mach-ap81.o
obj-$(CONFIG_AR71XX_MACH_AP83)		+= mach-ap83.o
obj-$(CONFIG_AR71XX_MACH_AW_NR580)	+= mach-aw-nr580.o
obj-$(CONFIG_AR71XX_MACH_DIR_615_C1)	+= mach-dir-615-c1.o
obj-$(CONFIG_AR71XX_MACH_DIR_825_B1)	+= mach-dir-825-b1.o
obj-$(CONFIG_AR71XX_MACH_GENERIC)	+= mach-generic.o
obj-$(CONFIG_AR71XX_MACH_MZK_W04NU)	+= mach-mzk-w04nu.o
obj-$(CONFIG_AR71XX_MACH_MZK_W300NH)	+= mach-mzk-w300nh.o
obj-$(CONFIG_AR71XX_MACH_PB42)		+= mach-pb42.o
obj-$(CONFIG_AR71XX_MACH_PB44)		+= mach-pb44.o
obj-$(CONFIG_AR71XX_MACH_RB_4XX)	+= mach-rb-4xx.o
obj-$(CONFIG_AR71XX_MACH_TEW_632BRP)	+= mach-tew-632brp.o
obj-$(CONFIG_AR71XX_MACH_TL_WR741ND)	+= mach-tl-wr741nd.o
obj-$(CONFIG_AR71XX_MACH_TL_WR941ND)	+= mach-tl-wr941nd.o
obj-$(CONFIG_AR71XX_MACH_TL_WR1043ND)	+= mach-tl-wr1043nd.o
obj-$(CONFIG_AR71XX_MACH_UBNT)		+= mach-ubnt.o
obj-$(CONFIG_AR71XX_MACH_WNDR3700)	+= mach-wndr3700.o
obj-$(CONFIG_AR71XX_MACH_WNR2000)	+= mach-wnr2000.o
obj-$(CONFIG_AR71XX_MACH_WP543)		+= mach-wp543.o
obj-$(CONFIG_AR71XX_MACH_WRT160NL)	+= mach-wrt160nl.o
obj-$(CONFIG_AR71XX_MACH_WRT400N)	+= mach-wrt400n.o
