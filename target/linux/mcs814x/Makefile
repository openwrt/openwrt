#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mcs814x
BOARDNAME:=Moschip MCS814x
FEATURES:=usb squashfs jffs2
CFLAGS=-Os -pipe -march=armv5te -mtune=arm926ej-s -fno-caller-saves
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

LINUX_VERSION:=3.3.8

DEVICE_TYPE:=other

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES := $(filter-out ppp%,$(DEFAULT_PACKAGES)) \
		  kmod-usb-core kmod-usb2 kmod-usb-ohci \
		  kmod-usb-storage kmod-fs-vfat \
		  kmod-nls-cp437 kmod-nls-iso8859-1 kmod-nls-iso8859-15 \
		  kmod-nls-utf8 \
		  kmod-mcs814x-wdt kmod-ledtrig-usbdev

KERNELNAME:="zImage dtbs"

$(eval $(call BuildTarget))
