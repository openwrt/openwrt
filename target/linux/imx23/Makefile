#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=imx23
BOARDNAME:=Olinuxino i.MX233
FEATURES:=ext4 rtc usb gpio
CFLAGS:=-Os -pipe -march=armv5te -mtune=arm926ej-s -fno-caller-saves
MAINTAINER:=Zoltan HERPAI <wigyori@uid0.hu>
LINUX_VERSION:=3.9.4
KERNELNAME:="zImage dtbs"
DEPENDS:=+imx-bootlets

define Target/Description
	Support for Olimex Olinuxino boards
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += dnsmasq imx-bootlets kmod-usb-chipidea kmod-usb-net kmod-usb-net-smsc95xx \
	kmod-wdt-stmp3xxx kmod-gpio-mcp23s08 kmod-leds-gpio kmod-ledtrig-heartbeat

$(eval $(call BuildTarget))
