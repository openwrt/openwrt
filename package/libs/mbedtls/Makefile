#
# Copyright (C) 2011-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mbedtls
PKG_VERSION:=4.0.0
PKG_RELEASE:=1
PKG_BUILD_FLAGS:=no-mips16 gc-sections no-lto

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL=https://github.com/Mbed-TLS/$(PKG_NAME)/releases/download/$(PKG_NAME)-$(PKG_VERSION)
PKG_HASH:=2f3a47f7b3a541ddef450e4867eeecb7ce2ef7776093f3a11d6d43ead6bf2827

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=LICENSE
PKG_CPE_ID:=cpe:/a:arm:mbed_tls

MBEDTLS_BUILD_OPTS_CURVES= \
  CONFIG_MBEDTLS_ECP_DP_SECP192R1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_SECP224R1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_SECP192K1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_SECP224K1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_BP256R1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_BP384R1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_BP512R1_ENABLED \
  CONFIG_MBEDTLS_ECP_DP_CURVE448_ENABLED

MBEDTLS_BUILD_OPTS_CIPHERS= \
  CONFIG_MBEDTLS_ARIA_C \
  CONFIG_MBEDTLS_CAMELLIA_C \
  CONFIG_MBEDTLS_CCM_C \
  CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED \
  CONFIG_MBEDTLS_NIST_KW_C \
  CONFIG_MBEDTLS_RIPEMD160_C \
  CONFIG_MBEDTLS_RSA_NO_CRT \
  CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
  CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED \
  CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED

MBEDTLS_BUILD_OPTS= \
  $(MBEDTLS_BUILD_OPTS_CURVES) \
  $(MBEDTLS_BUILD_OPTS_CIPHERS) \
  CONFIG_MBEDTLS_CIPHER_MODE_OFB \
  CONFIG_MBEDTLS_CIPHER_MODE_XTS \
  CONFIG_MBEDTLS_DEBUG_C \
  CONFIG_MBEDTLS_PLATFORM_C \
  CONFIG_MBEDTLS_SELF_TEST \
  CONFIG_MBEDTLS_SSL_RENEGOTIATION \
  CONFIG_MBEDTLS_THREADING_C \
  CONFIG_MBEDTLS_THREADING_PTHREAD \
  CONFIG_MBEDTLS_VERSION_C \
  CONFIG_MBEDTLS_VERSION_FEATURES \
  CONFIG_MBEDTLS_PSA_CRYPTO_CLIENT \
  CONFIG_MBEDTLS_DEPRECATED_WARNING \
  CONFIG_MBEDTLS_SSL_PROTO_TLS1_2 \
  CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 \
  CONFIG_MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE

PKG_CONFIG_DEPENDS := $(MBEDTLS_BUILD_OPTS)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/mbedtls/Default
  TITLE:=Embedded SSL
  URL:=https://www.trustedfirmware.org/projects/mbed-tls/
endef

define Package/mbedtls/Default/description
The aim of the mbedtls project is to provide a quality, open-source
cryptographic library written in C and targeted at embedded systems.
endef

define Package/libmbedtls
$(call Package/mbedtls/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  SUBMENU:=SSL
  TITLE+= (library)
  ABI_VERSION:=21
  MENU:=1
endef

define Package/libmbedtls/config
	source "$(SOURCE)/Config.in"
endef

define Package/libmbedtls/description
$(call Package/mbedtls/Default/description)
This package contains the mbedtls library.
endef

TARGET_CFLAGS := $(filter-out -O%,$(TARGET_CFLAGS)) -Wno-unterminated-string-initialization

CMAKE_OPTIONS += \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DUSE_SHARED_MBEDTLS_LIBRARY:Bool=ON \
	-DENABLE_TESTING:Bool=OFF \
	-DENABLE_PROGRAMS:Bool=ON

define Build/Configure
	$(call Build/Configure/Default)

	$(if $(strip $(foreach opt,$(MBEDTLS_BUILD_OPTS),$($(opt)))),
		$(foreach opt,$(MBEDTLS_BUILD_OPTS),
			$(PKG_BUILD_DIR)/scripts/config.py \
			-f $(PKG_BUILD_DIR)/include/mbedtls/mbedtls_config.h \
			$(if $($(opt)),set,unset) $(patsubst CONFIG_%,%,$(opt))),)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/mbedtls \
		$(PKG_INSTALL_DIR)/usr/include/psa \
		$(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmake $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mbedcrypto.pc \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mbedtls.pc \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mbedx509.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libmbedtls/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libmbedtls))
