include $(TOPDIR)/rules.mk

PKG_NAME:=jsonfilter
PKG_VERSION:=2014-06-19
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(OPENWRT_GIT)/project/jsonpath.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=cdc760c58077f44fc40adbbe41e1556a67c1b9a9
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>
PKG_LICENSE:=ISC

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/jsonfilter
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libubox +libjson-c
  TITLE:=OpenWrt JSON filter utility
  URL:=http://git.openwrt.org/?p=project/jsonpath.git
endef

define Package/jsonfilter/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/jsonpath $(1)/usr/bin/jsonfilter
endef

$(eval $(call BuildPackage,jsonfilter))
