# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dnsmasq
PKG_VERSION:=2.32
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://thekelleys.org.uk/dnsmasq
PKG_MD5SUM:=d1f593a8352db1d6ad3dccc9b775295d
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk

define Package/dnsmasq
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=A lightweight DNS and DHCP server
DESCRIPTION:=A lightweight DNS and DHCP server.  It is intended to provide \\\
coupled DNS and DHCP service to a LAN.
URL:=http://www.thekelleys.org.uk/dnsmasq/
endef

define Package/dnsmasq/conffiles
/etc/dnsmasq.conf
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BINDIR="/usr/sbin" MANDIR="/usr/man" \
		all
endef

define Package/dnsmasq/install
	install -m0755 -d $(1)/etc
	install -m0644 ./files/dnsmasq.conf $(1)/etc/dnsmasq.conf
	install -m0755 -d $(1)/etc/init.d
	install -m0755 ./files/dnsmasq.init $(1)/etc/init.d/S50dnsmasq
	install -m0755 -d $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/src/dnsmasq $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,dnsmasq))
