#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=cns21xx
BOARDNAME:=Cavium Networks CNS21xx
FEATURES:=squashfs broken
CFLAGS:=-Os -pipe -march=armv4 -mtune=arm9tdmi -funit-at-a-time

LINUX_VERSION:=2.6.36.4

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-input-core kmod-input-gpio-keys \
	kmod-button-hotplug

define Target/Description
 Build firmware images for Cavium Networks CNS21XX based boards.
endef

$(eval $(call BuildTarget))
