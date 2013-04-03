#
# Makefile for the Ralink RT3662/RT3883 SoC specific parts of the kernel
#
# Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-y	:= irq.o setup.o devices.o rt3883.o clock.o

obj-$(CONFIG_EARLY_PRINTK)		+= early_printk.o

obj-$(CONFIG_RT3883_MACH_DIR_645)	+= mach-dir-645.o
obj-$(CONFIG_RT3883_MACH_OMNI_EMB_HPM)	+= mach-omni-emb-hpm.o
obj-$(CONFIG_RT3883_MACH_RT_N56U)	+= mach-rt-n56u.o
obj-$(CONFIG_RT3883_MACH_TEW_691GR)	+= mach-tew-691gr.o
obj-$(CONFIG_RT3883_MACH_TEW_692GR)	+= mach-tew-692gr.o
