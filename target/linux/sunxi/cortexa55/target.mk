# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017 Hauke Mehrtens

include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARDNAME:=Allwinner A52x/T52x (cortexa55)
CPU_TYPE:=cortex-a55
KERNELNAME:=Image dtbs
FEATURES+=fpu
