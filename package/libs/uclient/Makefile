include $(TOPDIR)/rules.mk

PKG_NAME:=uclient
PKG_RELEASE=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/uclient.git
PKG_MIRROR_HASH:=a6dbdd653400f3907deaa3e90f6517431677a4dccb9cefef6c2fc4386cba5ab7
PKG_SOURCE_DATE:=2025-10-03
PKG_SOURCE_VERSION:=dc909ca71bc884c0e5362e1d7cc7808696cb2add
CMAKE_INSTALL:=1

PKG_BUILD_DEPENDS:=ustream-ssl

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_CONFIG_DEPENDS := CONFIG_PACKAGE_ucode-mod-uclient

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libuclient
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=HTTP/1.1 client library
  ABI_VERSION:=20201210
  DEPENDS:=+libubox
endef

define Package/uclient-fetch
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Tiny wget replacement using libuclient
  ALTERNATIVES:=200:/usr/bin/wget:/bin/uclient-fetch
  DEFAULT_VARIANT:=1
  PROVIDES:=@wget-any
  DEPENDS:=+libuclient
endef

define Package/ucode-mod-uclient
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ucode uclient module
  DEPENDS:=+libucode +libuclient
endef

CMAKE_OPTIONS += \
	-DBUILD_UCODE=$(if $(CONFIG_PACKAGE_ucode-mod-uclient),ON,OFF)

define Package/libuclient/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libuclient.so $(1)/usr/lib/
endef

define Package/uclient-fetch/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/uclient-fetch $(1)/bin/
endef

define Package/ucode-mod-uclient/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/ucode/*.so $(1)/usr/lib/ucode
endef

$(eval $(call BuildPackage,libuclient))
$(eval $(call BuildPackage,uclient-fetch))
$(eval $(call BuildPackage,ucode-mod-uclient))
