#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ar71xx
BOARDNAME:=Atheros AR71xx/AR7240/AR913x/AR934x
FEATURES:=squashfs jffs2 targz
CFLAGS:=-Os -pipe -mips32r2 -mtune=mips32r2 -fno-caller-saves
SUBTARGETS:=generic nand

LINUX_VERSION:=3.2.5

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-input-core kmod-input-polldev \
	kmod-input-gpio-keys-polled kmod-button-hotplug swconfig \
	kmod-ath9k wpad-mini uboot-envtools

define Target/Description
	Build firmware images for Atheros AR71xx/AR913x/AR934x based boards.
endef

$(eval $(call BuildTarget))
