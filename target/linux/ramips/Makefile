#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ramips
BOARDNAME:=Ralink RT288x/RT3xxx
SUBTARGETS:=rt305x mt7620a mt7620n mt7621 rt3883 rt288x
FEATURES:=squashfs gpio
MAINTAINER:=John Crispin <blogic@openwrt.org>

LINUX_VERSION:=3.14.16

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug wpad-mini swconfig

define Target/Description
	Build firmware images for Ralink RT288x/RT3xxx based boards.
endef

$(eval $(call BuildTarget))
