#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ar71xx
BOARDNAME:=Atheros AR71xx/AR7240/AR913x
FEATURES:=squashfs jffs2 tgz
CFLAGS:=-Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time

LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-input-core kmod-input-polldev \
	kmod-input-gpio-buttons kmod-button-hotplug

define Target/Description
	Build firmware images for Atheros AR71xx/AR913x based boards.
endef

$(eval $(call BuildTarget))
