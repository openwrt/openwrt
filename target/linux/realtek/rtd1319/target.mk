# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2015-2023 Realtek Semiconductor Corp.

ARCH:=aarch64
SUBTARGET:=rtd1319
CPU_TYPE:=cortex-a55
BOARDNAME:=Realtek RTD1319 Based Boards
FEATURES+=usb pcie
DEVICE_TYPE:=nas
CFLAGS:=-O2 -pipe

KERNELNAME:=Image dtbs

define Target/Description
	Build NAS firmware image for Realtek RTD1319 SoC boards.
endef

