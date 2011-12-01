#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=ramips
BOARDNAME:=Ralink RT288x/RT305x
SUBTARGETS:=rt288x rt305x
CFLAGS:=-Os -pipe -mips32r2 -mtune=mips32r2 -fno-caller-saves
FEATURES:=squashfs

LINUX_VERSION:=2.6.39.4

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES+=\
	kmod-leds-gpio kmod-input-core kmod-input-polldev kmod-gpio-buttons \
	kmod-button-hotplug kmod-rt2800-pci wpad-mini maccalc

define Target/Description
	Build firmware images for Ralink RT288x/RT305x based boards.
endef

$(eval $(call BuildTarget))
