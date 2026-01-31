# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2022 Eneas Ulir de Queiroz

include $(TOPDIR)/rules.mk

PKG_NAME:=uencrypt
PKG_RELEASE:=5

PKG_LICENSE:=GPL-2.0-or-later
PKG_MAINTAINER:=Eneas U de Queiroz <cotequeiroz@gmail.com>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1
ifeq ($(BUILD_VARIANT),mbedtls)
  CMAKE_OPTIONS+=-DUSE_MBEDTLS=1
else ifeq ($(BUILD_VARIANT),wolfssl)
  CMAKE_OPTIONS+=-DUSE_WOLFSSL=1
endif

TARGET_CFLAGS+=-Wall

define Package/uencrypt/default
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Small Decryption utility
endef

define Package/uencrypt/default/description
  This is a small encrypton/decryption program. It defaults
  to AES-128-CBC, but supports any encryption provided by
  the crypto library. Even though it can be used for
  non-critical* regular encryption and decryption operations,
  it is included here to unencrypt the configuration from mtd
  on some devices.

  * Key and IV are exposed on cmdline

  This variant uses $(1) as crypto provider
endef

define Package/uencrypt-mbedtls
  $(Package/uencrypt/default)
  VARIANT:=mbedtls
  TITLE+= using mbedTLS
  DEPENDS:=+libmbedtls
  CONFLICTS:=uencrypt-openssl uencrypt-wolfssl
endef

Package/uencrypt-mbedtls/description= \
  $(call Package/uencrypt/default/description,mbedTLS)

define Package/uencrypt-openssl
  $(Package/uencrypt/default)
  VARIANT:=openssl
  TITLE+= using OpenSSL
  DEPENDS:=+libopenssl
  CONFLICTS:=uencrypt-wolfssl
endef

Package/uencrypt-openssl/description= \
  $(call Package/uencrypt/default/description,OpenSSL)

define Package/uencrypt-wolfssl
  $(Package/uencrypt/default)
  VARIANT:=wolfssl
  TITLE+= using wolfSSL
  DEPENDS:=+libwolfssl
endef

Package/uencrypt-wolfssl/description= \
  $(call Package/uencrypt/default/description,wolfSSL)

define Package/uencrypt/default/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/uencrypt $(1)/usr/bin
endef
Package/uencrypt-openssl/install = $(Package/uencrypt/default/install)
Package/uencrypt-wolfssl/install = $(Package/uencrypt/default/install)
Package/uencrypt-mbedtls/install = $(Package/uencrypt/default/install)

$(eval $(call BuildPackage,uencrypt-mbedtls))
$(eval $(call BuildPackage,uencrypt-openssl))
$(eval $(call BuildPackage,uencrypt-wolfssl))
