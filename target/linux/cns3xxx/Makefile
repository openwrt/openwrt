#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=cns3xxx
BOARDNAME:=Cavium Networks Econa CNS3xxx
FEATURES:=squashfs fpu gpio pcie usb usbgadget
CPU_TYPE:=mpcore
CPU_SUBTYPE:=vfp
MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

KERNEL_PATCHVER:=3.14

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for Cavium Networks Econa CNS3xxx based boards,
	eg. the Gateworks Laguna family
endef

KERNELNAME:=zImage

DEFAULT_PACKAGES += kmod-ath9k kmod-usb2 wpad-mini

$(eval $(call BuildTarget))
