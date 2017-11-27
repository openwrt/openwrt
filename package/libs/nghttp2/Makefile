include $(TOPDIR)/rules.mk

PKG_NAME:=nghttp2
PKG_VERSION:=1.28.0
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz

PKG_SOURCE_URL:=https://github.com/nghttp2/nghttp2/releases/download/v$(PKG_VERSION)
PKG_HASH:=0d6c3f00614deca3935e42a27f6ad0ea87c31d8c1baa3a9c52755955c599fd8d

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=COPYING
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libnghttp2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Library implementing the framing layer of HTTP/2
  MAINTAINER:=Hans Dedecker <dedeckeh@gmail.com>
endef

define Package/libnghttp2/description
 C library implementing the framing layer of the HTTP/2 protocol. It can be used to build a HTTP/2-capable HTTP client or server
endef

CMAKE_OPTIONS += \
        -DENABLE_LIB_ONLY=ON

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/nghttp2
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/include/nghttp2/*.h $(1)/usr/include/nghttp2/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc $(1)/usr/lib/pkgconfig/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libnghttp2.so* $(1)/usr/lib/
endef

define Package/libnghttp2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libnghttp2.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,libnghttp2))
