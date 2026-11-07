#
# Copyright (C) 2011-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mbedtls
PKG_VERSION:=4.1.1
PKG_RELEASE:=1
PKG_BUILD_FLAGS:=no-mips16 gc-sections no-lto

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL=https://github.com/Mbed-TLS/$(PKG_NAME)/releases/download/$(PKG_NAME)-$(PKG_VERSION)
PKG_HASH:=3359a349e23db3d5536fcee032ae7b2ecbfc08972fab643089b5cbf2a375c98c

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=LICENSE
PKG_CPE_ID:=cpe:/a:arm:mbed_tls

MBEDTLS_BUILD_OPTS_CURVES= \
  CONFIG_PSA_WANT_ECC_SECP_R1_256 \
  CONFIG_PSA_WANT_ECC_SECP_R1_384 \
  CONFIG_PSA_WANT_ECC_SECP_R1_521 \
  CONFIG_PSA_WANT_ECC_SECP_K1_256 \
  CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_256 \
  CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_384 \
  CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_512 \
  CONFIG_PSA_WANT_ECC_MONTGOMERY_255 \
  CONFIG_PSA_WANT_ECC_MONTGOMERY_448

MBEDTLS_BUILD_OPTS_CIPHERS= \
  CONFIG_PSA_WANT_KEY_TYPE_AES \
  CONFIG_PSA_WANT_KEY_TYPE_ARIA \
  CONFIG_PSA_WANT_KEY_TYPE_CAMELLIA \
  CONFIG_PSA_WANT_ALG_CCM \
  CONFIG_PSA_WANT_ALG_CMAC \
  CONFIG_PSA_WANT_ALG_GCM \
  CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED \
  CONFIG_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED \
  CONFIG_MBEDTLS_NIST_KW_C \
  CONFIG_PSA_WANT_ALG_RIPEMD160 \
  CONFIG_MBEDTLS_RSA_NO_CRT \
  CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED \
  CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED \
  CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED

MBEDTLS_BUILD_OPTS= \
  $(MBEDTLS_BUILD_OPTS_CURVES) \
  $(MBEDTLS_BUILD_OPTS_CIPHERS) \
  CONFIG_PSA_WANT_ALG_OFB \
  CONFIG_MBEDTLS_DEBUG_C \
  CONFIG_PSA_WANT_ALG_HKDF \
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
  ABI_VERSION:=23
  MENU:=1
endef

define Package/libmbedtls/config
	source "$(SOURCE)/Config.in"
endef

define Package/mbedtls-util
$(call Package/mbedtls/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (utilities)
  DEPENDS:=+libmbedtls
endef

define Package/libmbedtls/description
$(call Package/mbedtls/Default/description)
This package contains the mbedtls library.
endef

define Package/mbedtls-util/description
$(call Package/mbedtls/Default/description)
This package contains mbedtls helper program for CSR generation
(cert_req)
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
			-c $(PKG_BUILD_DIR)/tf-psa-crypto/include/psa/crypto_config.h \
			$(if $($(opt)),set,unset) $(patsubst CONFIG_%,%,$(opt))),)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/mbedtls \
		$(PKG_INSTALL_DIR)/usr/include/psa \
		$(PKG_INSTALL_DIR)/usr/include/tf-psa-crypto \
		$(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtfpsacrypto.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbedtls.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbedx509.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtfpsacrypto.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbedtls.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbedx509.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmake $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mbedcrypto.pc \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mbedtls.pc \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/mbedx509.pc \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/tfpsacrypto.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libmbedtls/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtfpsacrypto.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbedtls.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbedx509.so.* $(1)/usr/lib/
endef

define Package/mbedtls-util/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/cert_req $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libmbedtls))
$(eval $(call BuildPackage,mbedtls-util))
