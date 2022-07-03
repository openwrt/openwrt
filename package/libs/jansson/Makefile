#
# Copyright (C) 2011-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jansson
PKG_VERSION:=2.13.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/akheron/$(PKG_NAME)/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=f22901582138e3203959c9257cf83eba9929ac41d7be4a42557213a22ebcc7a0

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL:=1
CMAKE_OPTIONS += \
	-DJANSSON_BUILD_DOCS:BOOL=OFF \
	-DJANSSON_BUILD_SHARED_LIBS:BOOL=ON \
	-DJANSSON_EXAMPLES:BOOL=OFF \
	-DJANSSON_WITHOUT_TESTS:BOOL=ON

define Package/jansson
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Jansson library
  URL:=http://www.digip.org/jansson/
  ABI_VERSION:=4
endef

define Package/jansson/description
  Jansson is a C library for encoding, decoding and manipulating JSON data
endef

TARGET_CFLAGS += -flto
TARGET_LDFLAGS += -flto

define Package/jansson/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjansson*so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,jansson))
