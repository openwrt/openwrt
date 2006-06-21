# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pptp
PKG_VERSION:=1.6.0
PKG_RELEASE:=3

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/pptpclient
PKG_MD5SUM:=9a706327fb9827541d7c86d48ceb9631
PKG_CAT:=zcat

include $(TOPDIR)/include/package.mk

define Package/pptp
SECTION:=base
# CATEGORY:=FIXME
TITLE:=PPTP (Point-to-Point Tunneling Protocol) client
DESCRIPTION:=A Point-to-Point Tunneling Protocol Client
URL:=http://pptpclient.sourceforge.net/
endef

define Package/pptp/install
	install -d -m0755 $(1)/etc/ppp
	install -m0644 ./files/options.pptp $(1)/etc/ppp/
	install -d -m0755 $(1)/sbin
	install -m0755 ./files/ifup.pptp $(1)/sbin/
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/pptp $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,pptp))
