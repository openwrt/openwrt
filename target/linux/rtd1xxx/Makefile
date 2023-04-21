# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2015-2021 Realtek Semiconductor Corp.

include $(TOPDIR)/rules.mk

BOARD:=rtd1xxx
BOARDNAME:=Realtek RTD Media SoC
FEATURES:=squashfs usb pcie
SUBTARGETS:=rtd129x rtd1319 rtd1619b
DEVICE_TYPE:=nas
CFLAGS:=-O2 -pipe

KERNEL_PATCHVER:=5.15

define Target/Description
	Build firmware images for Reatek RTD1xxx SoC family for NAS
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=Image dtbs

DEFAULT_PACKAGES +=e2fsprogs ethtool mkf2fs

$(eval $(call BuildTarget))
