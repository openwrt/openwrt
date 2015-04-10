include $(TOPDIR)/rules.mk

PKG_NAME:=librpc
PKG_VERSION:=2015-04-10
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/uclibc-rpc.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=308e9964bfb623773dc0dcc99ef9d18d1551d6ae
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1
PKG_USE_MIPS16:=0

PKG_LICENSE:=LGPL-2.1
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/librpc
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=uClibc RPC library
endef

define Package/librpc/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/librpc.so $(1)/lib/
endef

$(eval $(call BuildPackage,librpc))
