# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=zlib
PKG_VERSION:=1.2.3
PKG_RELEASE:=3

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.zlib.net @SF/zlib
PKG_MD5SUM:=dee233bf288ee795ac96a98cc2e369b6
PKG_CAT:=bzcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/zlib
SECTION:=base
CATEGORY:=Libraries
TITLE:=Library implementing the deflate compression method
DESCRIPTION:=A library implementing the 'deflate' compression method
URL:=http://www.zlib.net/
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		LDSHARED="$(TARGET_CC) -shared -Wl,-soname,libz.so.1" \
		CFLAGS="$(TARGET_CFLAGS) $(CFLAGS_LARGEFILE)" \
		./configure \
		--prefix=/usr \
		--shared \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(CFLAGS_LARGEFILE)" \
		libz.a libz.so
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/z{conf,lib}.h \
		$(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libz.{a,so*} \
		$(STAGING_DIR)/usr/lib/
endef
	
define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/z{conf,lib}.h \
		$(STAGING_DIR)/usr/lib/libz.{a,so*}
endef

define Package/zlib/install
	install -m0755 -d $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libz.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,zlib))
