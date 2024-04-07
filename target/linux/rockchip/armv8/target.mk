# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017 Hauke Mehrtens

include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARDNAME:=RK33xx boards (64 bit)
KERNELNAME:=Image dtbs

define Target/Description
	Build firmware image for Rockchip RK33xx devices.
	This firmware features a 64 bit kernel.
endef
