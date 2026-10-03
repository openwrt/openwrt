# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

ARCH:=aarch64
SUBTARGET:=gxbb
BOARDNAME:=Amlogic Meson GXBB boards (S905)
CPU_TYPE:=cortex-a53

KERNELNAME:=Image dtbs

define Target/Description
	Build firmware images for Amlogic Meson GXBB devices.
	This firmware features a 64-bit kernel.
endef
