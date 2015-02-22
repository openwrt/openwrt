#
# Copyright (C) 2010-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn-easy-rsa

PKG_REV:=ff5bfd1dd8e548cb24d302742af3894f893ef92f
PKG_VERSION:=2013-01-30
PKG_RELEASE=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/OpenVPN/easy-rsa.git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_REV).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/openvpn-easy-rsa
  TITLE:=Simple shell scripts to manage a Certificate Authority
  SECTION:=net
  CATEGORY:=Network
  URL:=http://openvpn.net
  SUBMENU:=VPN
  DEPENDS:=+openssl-util
endef

define Package/openvpn-easy-rsa/conffiles
/etc/easy-rsa/keys/serial
/etc/easy-rsa/keys/index.txt
/etc/easy-rsa/vars
endef

define Build/Configure

endef

define Build/Compile

endef

define Package/openvpn-easy-rsa/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/easy-rsa/2.0/{build-*,clean-all,inherit-inter,list-crl,pkitool,revoke-full,sign-req,whichopensslcnf} $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/easy-rsa
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/easy-rsa/2.0/openssl-1.0.0.cnf $(1)/etc/easy-rsa/openssl-1.0.0.cnf
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/easy-rsa/2.0/vars $(1)/etc/easy-rsa/vars
	$(INSTALL_DIR) $(1)/etc/easy-rsa/keys
	$(INSTALL_DATA) files/easy-rsa.index $(1)/etc/easy-rsa/keys/index.txt
	$(INSTALL_DATA) files/easy-rsa.serial $(1)/etc/easy-rsa/keys/serial
endef

$(eval $(call BuildPackage,openvpn-easy-rsa))
