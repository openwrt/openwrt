# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dnsmasq
PKG_VERSION:=2.39
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://thekelleys.org.uk/dnsmasq
PKG_MD5SUM:=1ffccc1f0d9b8a08f2b99e03ba9cc08b

include $(INCLUDE_DIR)/package.mk

define Package/dnsmasq
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=A lightweight DNS and DHCP server
  URL:=http://www.thekelleys.org.uk/dnsmasq/
endef

define Package/dnsmasq/conffiles
/etc/dnsmasq.conf
endef

define Package/dnsmasq/description
 It is intended to provide coupled DNS and DHCP service to a LAN.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DHAVE_ISC_READER=1" \
		BINDIR="/usr/sbin" MANDIR="/usr/man" \
		all
endef

define Package/dnsmasq/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dnsmasq $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/dhcp.conf $(1)/etc/config/dhcp
	$(INSTALL_DATA) ./files/dnsmasq.conf $(1)/etc/dnsmasq.conf
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dnsmasq.init $(1)/etc/init.d/dnsmasq
endef

$(eval $(call BuildPackage,dnsmasq))
