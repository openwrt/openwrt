include $(TOPDIR)/rules.mk

PKG_NAME:=librpc
PKG_VERSION:=2015-11-04
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(OPENWRT_GIT)/project/librpc-uclibc.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=a921e3ded051746f9f7cd5e5a312fb6771716aac
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
