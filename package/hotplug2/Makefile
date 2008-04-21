# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6372 2007-02-25 23:34:57Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=hotplug2
PKG_VERSION:=0.9
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://isteve.bofh.cz/~isteve/hotplug2
PKG_MD5SUM:=ea2c01d027b4002e4e6b0ff266f51a51

include $(INCLUDE_DIR)/package.mk

define Package/hotplug2
  SECTION:=base
  CATEGORY:=Base system
  VERSION:=0.9+r102-$(PKG_RELEASE)
  TITLE:=Dynamic device management subsystem for embedded systems
  URL:=http://isteve.bofh.cz/~isteve/hotplug2/
endef

define Package/hotplug2/description
 Hotplug2 is a trivial replacement of some of the UDev functionality
 in a tiny pack, intended for Linux early userspace: Init RAM FS and InitRD.
endef

MAKE_FLAGS += CFLAGS="$(TARGET_CFLAGS) -DHAVE_RULES -I."

define Package/hotplug2/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/hotplug2.rules $(1)/etc/
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hotplug2 $(1)/sbin/
endef

$(eval $(call BuildPackage,hotplug2))
