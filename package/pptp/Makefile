# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pptp
PKG_VERSION:=1.6.0
PKG_RELEASE:=7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/pptpclient
PKG_MD5SUM:=9a706327fb9827541d7c86d48ceb9631

include $(INCLUDE_DIR)/package.mk

define Package/pptp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=PPTP client
  URL:=http://pptpclient.sourceforge.net/
  DEPENDS:=+ppp +kmod-gre
endef

define Package/pptp/daemon
 This package contains a PPTP (Point-to-Point Tunneling Protocol) client.
endef

define Package/pptp/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pptp $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/options.pptp $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/pptp.sh $(1)/lib/network/
endef

$(eval $(call BuildPackage,pptp))
