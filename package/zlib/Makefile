# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zlib
PKG_VERSION:=1.2.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.zlib.net @SF/libpng
PKG_MD5SUM:=be1e89810e66150f5b0327984d8625a0

include $(INCLUDE_DIR)/package.mk

define Package/zlib
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Library implementing the deflate compression method
  URL:=http://www.zlib.net/
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		LDSHARED="$(TARGET_CC) -shared -Wl,-soname,libz.so.1" \
		CFLAGS="$(TARGET_CFLAGS) $(FPIC)" \
		./configure \
			--prefix=/usr \
			--shared \
			--uname=Linux \
	);
endef

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		libz.a libz.so.$(PKG_VERSION)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/z{conf,lib}.h \
		$(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libz.{a,so*} \
		$(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/zlib.pc \
		$(1)/usr/lib/pkgconfig/
endef

# libz.so is needed for openssl (zlib-dynamic)
define Package/zlib/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libz.so $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libz.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,zlib))
