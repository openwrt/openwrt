include $(TOPDIR)/rules.mk

PKG_NAME:=uclient
PKG_VERSION:=2014-09-04
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=http://git.openwrt.org/project/uclient.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=57a1dda8f31878282e554bb98d5cc81307edea49
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MIRROR_MD5SUM:=
CMAKE_INSTALL:=1

PKG_BUILD_DEPENDS:=ustream-ssl

PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

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
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/uclient-fetch $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libuclient))
$(eval $(call BuildPackage,uclient-fetch))
