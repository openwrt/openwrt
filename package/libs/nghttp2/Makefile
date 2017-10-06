include $(TOPDIR)/rules.mk

PKG_NAME:=nghttp2
PKG_VERSION:=1.26.0
PKG_RELEASE:=1
PKG_SOURCE_URL:=https://github.com/nghttp2/nghttp2.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=0389af5724010627e19425237e9e5e0bf343bc6f
PKG_SOURCE_DIR:=$(PKG_NAME)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MIRROR_HASH:=e7d6d6239cb5a6957b9aafe22a15ea01fafefdf472eff84ec959bb31c5023f79
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
