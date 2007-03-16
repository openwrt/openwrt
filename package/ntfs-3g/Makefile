# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ntfs-3g
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.ntfs-3g.org/
PKG_MD5SUM:=873a8de662849d129fc7c475ad3f5447

PKG_BUILD_DEPENDS:=libfuse

include $(INCLUDE_DIR)/package.mk

define Package/ntfs-3g
  TITLE:=NTFS-3G
  DEPENDS:=+libfuse +fuse-utils
  SECTION:=utils
  CATEGORY:=Utilities
  DESCRIPTION:=\
	Third generation Read/Write NTFS driver
  URL:=http://www.ntfs-3g.org
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -f config.cache; \
		touch configure.in ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch include/config.h.in ; \
		touch configure ; \
		$(TARGET_CONFIGURE_OPTS) \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(STAGING_DIR)/usr/lib/pkgconfig" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--enable-kernel-module \
			--enable-lib \
			--enable-util \
			--disable-example \
			--disable-auto-modprobe \
			--with-kernel="$(LINUX_DIR)" \
			--disable-mtab \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/ntfs-3g $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libntfs-3g.{a,so*} $(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/ntfs-3g \
		$(STAGING_DIR)/usr/lib/libntfs-3g.{a,so*}
endef

define Package/ntfs-3g/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ntfs-3g $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libntfs-3g.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,ntfs-3g))
