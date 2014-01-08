include $(TOPDIR)/rules.mk

PKG_NAME:=jsonpath
PKG_VERSION:=2014-01-08
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.openwrt.org/project/jsonpath.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=f1e3aeb2526ea28db34c7bc714277c1dff26e5fc
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/jsonpath
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libubox +libjson-c
  TITLE:=OpenWrt JSON query utility
endef

define Package/jsonpath/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/jsonpath $(1)/usr/bin/
endef

$(eval $(call BuildPackage,jsonpath))
