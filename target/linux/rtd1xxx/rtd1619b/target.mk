# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2015-2021 Realtek Semiconductor Corp.

include $(TOPDIR)/rules.mk

ARCH:=aarch64
SUBTARGET:=rtd1619b
CPU_TYPE:=cortex-a55
BOARDNAME:=Realtek RTD1619b Based Boards

define Target/Description
	Build NAS firmware image for Realtek RTD1619B SoC boards.
endef

