# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2022 Eneas Ulir de Queiroz

include $(TOPDIR)/rules.mk

PKG_NAME:=uencrypt
PKG_RELEASE:=1

PKG_FLAGS:=nonshared
PKG_LICENSE:=GPL-2.0-or-later
PKG_MAINTAINER:=Eneas U de Queiroz <cotequeiroz@gmail.com>
PKG_CONFIG_DEPENDS:=\
	CONFIG_UENCRYPT_OPENSSL \
	CONFIG_UENCRYPT_WOLFSSL

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1
CMAKE_OPTIONS+=$(if $(CONFIG_UENCRYPT_WOLFSSL),-DUSE_WOLFSSL=1)

define Package/uencrypt
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Decryption utility for Arcadyan WG4xx223
  DEPENDS:=@TARGET_ramips_mt7621 +UENCRYPT_WOLFSSL:libwolfssl +UENCRYPT_OPENSSL:libopenssl
endef

define Package/uencrypt/description
  This is a small AES-128-CBC encrypton/decryption program.
  Even though it can be used for regular encryption and
  decryption operations using AES-128-CBC, it is included
  here to unencrypt the configuration from mtd on Arcadyan
  WG430223 and WG443223 routers.
endef

define Package/uencrypt/config
  if PACKAGE_uencrypt
    choice
	prompt "Crypto provider"
	default UENCRYPT_WOLFSSL

	config UENCRYPT_OPENSSL
	bool "OpenSSL"

	config UENCRYPT_WOLFSSL
	bool "wolfSSL"
    endchoice
  endif
endef

define Package/uencrypt/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/uencrypt $(1)/usr/bin
endef

$(eval $(call BuildPackage,uencrypt))
