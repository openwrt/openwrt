include $(TOPDIR)/rules.mk

PKG_NAME:=ustream-ssl
PKG_VERSION:=2013-01-22
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/ustream-ssl.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=da607e6272d789ed5dae3b0efff90912fda6f81f
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

ifeq ($(BUILD_VARIANT),cyassl)
  TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/cyassl
  CMAKE_OPTIONS += -DCYASSL=on
endif

define Package/libustream/default/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libustream-ssl.so $(1)/lib/
endef

Package/libustream-openssl/install = $(Package/libustream/default/install)
Package/libustream-cyassl/install = $(Package/libustream/default/install)

$(eval $(call BuildPackage,libustream-cyassl))
$(eval $(call BuildPackage,libustream-openssl))
