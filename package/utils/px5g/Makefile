#
# Copyright (C) 2010-2015 Jo-Philipp Wich <jo@mein.io>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=px5g
PKG_RELEASE:=4
PKG_LICENSE:=LGPL-2.1
PKG_BUILD_DIR:=$(BUILD_DIR)/px5g

PKG_USE_MIPS16:=0

include $(INCLUDE_DIR)/package.mk

define Package/px5g-mbedtls
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Encryption
  TITLE:=X.509 certificate generator (using mbedtls)
  MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
  DEPENDS:=+libmbedtls
  PROVIDES:=px5g
endef

define Package/px5g-mbedtls/description
 Px5g is a tiny standalone X.509 certificate generator.
 It suitable to create key files and certificates in DER
 and PEM format for use with stunnel, uhttpd and others.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

TARGET_LDFLAGS := -lmbedtls -lmbedx509 -lmbedcrypto

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/px5g px5g.c $(TARGET_LDFLAGS)
endef

define Package/px5g-mbedtls/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/px5g $(1)/usr/sbin/px5g
endef

$(eval $(call BuildPackage,px5g-mbedtls))
