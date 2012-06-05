#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=cns21xx
BOARDNAME:=Cavium Networks Econa CNS21xx
FEATURES:=squashfs broken
CFLAGS:=-Os -pipe -march=armv4 -mtune=fa526 -fno-caller-saves

LINUX_VERSION:=3.3.8

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-input-core kmod-input-gpio-keys \
	kmod-button-hotplug

define Target/Description
 Build firmware images for Cavium Networks CNS21XX based boards.
endef

$(eval $(call BuildTarget))
