#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=rb532
BOARDNAME:=Mikrotik RouterBoard 532
FEATURES:=pci targz

LINUX_VERSION:=3.14.18

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += wpad-mini kmod-ath5k kmod-input-rb532

$(eval $(call BuildTarget))
