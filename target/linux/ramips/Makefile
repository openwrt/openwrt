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
SUBTARGETS:=rt305x rt3883 mt7620a rt288x
FEATURES:=squashfs gpio

LINUX_VERSION:=3.10.17

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES+=\
	kmod-leds-gpio kmod-input-core kmod-input-polldev \
	kmod-input-gpio-keys-polled kmod-button-hotplug \
	kmod-rt2800-pci wpad-mini swconfig

define Target/Description
	Build firmware images for Ralink RT288x/RT3xxx based boards.
endef

$(eval $(call BuildTarget))
