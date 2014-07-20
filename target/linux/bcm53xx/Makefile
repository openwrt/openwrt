#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=bcm53xx
BOARDNAME:=Broadcom BCM47xx/53xx with ARM CPU
FEATURES:=squashfs usb pci pcie gpio
MAINTAINER:=Hauke Mehrtens <hauke@hauke-m.de>
CPU_TYPE:=cortex-a9

LINUX_VERSION:=3.10.49

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Broadcom based BCM47xx/53xx routers with ARM CPU, *not* MIPS.
endef

KERNELNAME:="zImage dtbs"

DEFAULT_PACKAGES += swconfig

$(eval $(call BuildTarget))
