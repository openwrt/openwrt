include $(TOPDIR)/rules.mk

PKG_NAME:=ustream-ssl
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/ustream-ssl.git
PKG_SOURCE_DATE:=2020-03-13
PKG_SOURCE_VERSION:=40b563b1ea89dabb2b6cd90644908134a0c8eff2
PKG_MIRROR_HASH:=db562c9919b4045c7b735c935b0a93983686b827ec4d537e8aa875c7e6df21a1
CMAKE_INSTALL:=1

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libustream/default
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ustream SSL Library
  DEPENDS:=+libubox
  ABI_VERSION:=20150806
endef

define Package/libustream-openssl
  $(Package/libustream/default)
  TITLE += (openssl)
  DEPENDS += +PACKAGE_libustream-openssl:libopenssl
  VARIANT:=openssl
endef

define Package/libustream-wolfssl
  $(Package/libustream/default)
  TITLE += (wolfssl)
  DEPENDS += +PACKAGE_libustream-wolfssl:libwolfssl
  VARIANT:=wolfssl
endef

define Package/libustream-mbedtls
  $(Package/libustream/default)
  TITLE += (mbedtls)
  DEPENDS += +libmbedtls
  VARIANT:=mbedtls
  DEFAULT_VARIANT:=1
endef

ifeq ($(BUILD_VARIANT),wolfssl)
  TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/wolfssl
  CMAKE_OPTIONS += -DWOLFSSL=on
endif
ifeq ($(BUILD_VARIANT),mbedtls)
  CMAKE_OPTIONS += -DMBEDTLS=on
endif

define Package/libustream/default/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libustream-ssl.so $(1)/lib/
endef

Package/libustream-openssl/install = $(Package/libustream/default/install)
Package/libustream-wolfssl/install = $(Package/libustream/default/install)
Package/libustream-mbedtls/install = $(Package/libustream/default/install)

$(eval $(call BuildPackage,libustream-mbedtls))
$(eval $(call BuildPackage,libustream-wolfssl))
$(eval $(call BuildPackage,libustream-openssl))
