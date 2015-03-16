#
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=omap
BOARDNAME:=TI OMAP3/4/AM33xx
FEATURES:=usb usbgadget ext4 targz fpu audio display nand ubifs
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=vfpv3

KERNEL_PATCHVER:=3.18

MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

KERNELNAME:=zImage dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += uboot-omap-am335x_evm uboot-omap-omap3_beagle uboot-omap-omap3_overo uboot-omap-omap4_panda

define Target/Description
	TI OMAP boards
endef

$(eval $(call BuildTarget))
