#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=rb532
BOARDNAME:=Mikrotik RouterBoard 532
FEATURES:=jffs2 pci tgz

LINUX_VERSION:=2.6.30.6

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += hostapd-mini kmod-madwifi

$(eval $(call BuildTarget))
