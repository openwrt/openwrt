include $(TOPDIR)/rules.mk

PKG_NAME:=uclient
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(LEDE_GIT)/project/uclient.git
PKG_SOURCE_DATE:=2016-07-30
PKG_SOURCE_VERSION:=ccb314db2e99463de15eed9d7626360d82aec4d2
PKG_MIRROR_HASH:=76102eabb687fa34369b2216f6b379ff56fcc835e3e89cbd21dcc150ffbc9071
CMAKE_INSTALL:=1

PKG_BUILD_DEPENDS:=ustream-ssl

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libuclient
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=HTTP/1.1 client library
  ABI_VERSION:=$(PKG_VERSION)
  DEPENDS:=+libubox
endef

define Package/uclient-fetch
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Tiny wget replacement using libuclient
  DEPENDS:=+libuclient
endef

define Package/libuclient/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libuclient.so $(1)/usr/lib/
endef

define Package/uclient-fetch/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/uclient-fetch $(1)/bin/
	ln -s uclient-fetch $(1)/bin/wget
endef

$(eval $(call BuildPackage,libuclient))
$(eval $(call BuildPackage,uclient-fetch))
