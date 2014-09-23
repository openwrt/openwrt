include $(TOPDIR)/rules.mk

PKG_NAME:=ustream-ssl
PKG_VERSION:=2014-03-25
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/ustream-ssl.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=fc0b5ec804ee43c532978dd04ab0509c34baefb0
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_SOURCE_SUBDIR)

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libustream/default
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ustream SSL Library
  DEPENDS:=+libubox
  ABI_VERSION:=$(PKG_VERSION)
endef

define Package/libustream-openssl
  $(Package/libustream/default)
  TITLE += (openssl)
  DEPENDS += +PACKAGE_libustream-openssl:libopenssl
  VARIANT:=openssl
endef

define Package/libustream-cyassl
  $(Package/libustream/default)
  TITLE += (cyassl)
  DEPENDS += +PACKAGE_libustream-cyassl:libcyassl
  VARIANT:=cyassl
endef

define Package/libustream-polarssl
  $(Package/libustream/default)
  TITLE += (polarssl)
  DEPENDS += +libpolarssl
  VARIANT:=polarssl
  DEFAULT_VARIANT:=1
endef

ifeq ($(BUILD_VARIANT),cyassl)
  TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/cyassl
  CMAKE_OPTIONS += -DCYASSL=on
endif
ifeq ($(BUILD_VARIANT),polarssl)
  CMAKE_OPTIONS += -DPOLARSSL=on
endif

define Package/libustream/default/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libustream-ssl.so $(1)/lib/
endef

Package/libustream-openssl/install = $(Package/libustream/default/install)
Package/libustream-cyassl/install = $(Package/libustream/default/install)
Package/libustream-polarssl/install = $(Package/libustream/default/install)

$(eval $(call BuildPackage,libustream-polarssl))
$(eval $(call BuildPackage,libustream-cyassl))
$(eval $(call BuildPackage,libustream-openssl))
