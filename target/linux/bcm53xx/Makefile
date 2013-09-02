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

LINUX_VERSION:=3.10.10

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Broadcom based BCM47xx/53xx routers with ARM CPU, *not* MIPS.
endef

CFLAGS:=-Os -pipe -mtune=cortex-a9 -march=armv7-a -mfpu=vfp -mfloat-abi=softfp

KERNELNAME:="zImage dtbs"

DEFAULT_PACKAGES +=

$(eval $(call BuildTarget))
