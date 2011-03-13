#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=rb532
BOARDNAME:=Mikrotik RouterBoard 532
FEATURES:=jffs2 pci targz

LINUX_VERSION:=2.6.32.32

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += wpad-mini kmod-madwifi kmod-input-rb532

$(eval $(call BuildTarget))
