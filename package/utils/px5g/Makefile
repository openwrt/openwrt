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
PKG_BUILD_DIR:=$(BUILD_DIR)/px5g-$(BUILD_VARIANT)

PKG_USE_MIPS16:=0

include $(INCLUDE_DIR)/package.mk

define Package/px5g/Template
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Encryption
  TITLE:=X.509 certificate generator (using $(1))
  MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
  DEPENDS:=+lib$(1)
  PROVIDES:=px5g
  VARIANT:=$(1)
endef

define Package/px5g-polarssl/description
 Px5g is a tiny standalone X.509 certificate generator.
 It suitable to create key files and certificates in DER
 and PEM format for use with stunnel, uhttpd and others.
endef

Package/px5g-mbedtls=$(call Package/px5g/Template,mbedtls)
Package/px5g-polarssl=$(call Package/px5g/Template,polarssl)
Package/px5g-mbedtls/description=$(Package/px5g-polarssl/description)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

ifeq ($(BUILD_VARIANT),mbedtls)
TARGET_CFLAGS += -DMBEDTLS
TARGET_LDFLAGS := -lmbedtls -lmbedx509 -lmbedcrypto
else
TARGET_LDFLAGS := -lpolarssl
endif

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/px5g px5g.c $(TARGET_LDFLAGS)
endef

define Package/px5g-polarssl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/px5g $(1)/usr/sbin/px5g
endef

Package/px5g-mbedtls/install=$(Package/px5g-polarssl/install)

$(eval $(call BuildPackage,px5g-polarssl))
$(eval $(call BuildPackage,px5g-mbedtls))
