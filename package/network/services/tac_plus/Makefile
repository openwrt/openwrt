#
# Copyright (C) 2006-2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tacacs
PKG_VERSION:=F4.0.4.28
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.shrubbery.net/pub/tac_plus
PKG_HASH:=147f2dc98d26d2f93f0aba76c988ced196ffe1c001dc2e91f788a1a2c747219e

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--disable-shared \
	--disable-static \
	--without-libwrap

define Package/tac_plus
  SECTION:=net
  CATEGORY:=Network
  TITLE:=TACACS+ authentication daemon
  URL:=http://www.shrubbery.net/tac_plus/
endef

define Package/tac_plus/description
  TACACS+ authentication daemon
endef

define Package/tac_plus/conffiles
/etc/tac_plus.conf
endef

define Package/tac_plus/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/sbin
	$(INSTALL_CONF) ./files/tac_plus.conf $(1)/etc/tac_plus.conf
	$(INSTALL_BIN) ./files/tac_plus.init $(1)/etc/init.d/tac_plus
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tac_plus $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,tac_plus))
