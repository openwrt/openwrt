# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pptp
PKG_VERSION:=1.6.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/pptpclient
PKG_MD5SUM:=9a706327fb9827541d7c86d48ceb9631
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/pptp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=PPTP client
  DESCRIPTION:=\
	This package contains a PPTP (Point-to-Point Tunneling Protocol) client.
  URL:=http://pptpclient.sourceforge.net/
endef

define Package/pptp/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/pptp $(1)/usr/sbin/
	install -d -m0755 $(1)/etc/ppp
	install -m0644 ./files/options.pptp $(1)/etc/ppp/
	install -d -m0755 $(1)/lib/network
	install -m0755 ./files/pptp.sh $(1)/lib/network/
endef

$(eval $(call BuildPackage,pptp))
