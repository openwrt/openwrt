#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=storm
BOARDNAME:=Storm SL3512
FEATURES:=squashfs pci broken
CFLAGS:=-Os -pipe -march=armv4 -mtune=arm9tdmi -funit-at-a-time

LINUX_VERSION:=2.6.23.17

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for boards based on the Storm Semiconductor SL3512, eg. Wiligear WBD-111
endef

$(eval $(call BuildTarget))
