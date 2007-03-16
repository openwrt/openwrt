# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bridge
PKG_VERSION:=1.0.6
PKG_RELEASE:=1

PKG_SOURCE:=bridge-utils-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/bridge
PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd

PKG_BUILD_DIR:=$(BUILD_DIR)/bridge-utils-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/bridge
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=Ethernet bridging configuration utility
  DESCRIPTION:=\
	Manage ethernet bridging: a way to connect networks together to \\\
	form a larger network.
  URL:=http://bridge.sourceforge.net/
endef

CONFIGURE_ARGS += --with-linux-headers="$(LINUX_DIR)"

define Package/bridge/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/brctl/brctl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,bridge))
