# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017 Hauke Mehrtens

include $(TOPDIR)/rules.mk

CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
BOARDNAME:=RK33xx/RV1129/RV110x boards (32 bit)

define Target/Description
	Build firmware image for Rockchip RK33xx/RV1129/RV110x devices.
	This firmware features a 32 bit kernel.
endef
