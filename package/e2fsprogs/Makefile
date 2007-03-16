# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.39
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs
PKG_MD5SUM:=06f7806782e357797fad1d34b7ced0c6

include $(INCLUDE_DIR)/package.mk

define Package/e2fsprogs
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Ext2/3 filesystem utilities
  DESCRIPTION:=\
	This package contains essential ext2 filesystem utilities which consists of \\\
	e2fsck, mke2fs, debugfs, dumpe2fs, tune2fs, and most of the other core ext2 \\\
	filesystem utilities.
  URL:=http://e2fsprogs.sourceforge.net/
endef

define Package/tune2fs
  $(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem tune utility
  DESCRIPTION:=Ext2 Filesystem tune utility
  DEPENDS:=e2fsprogs
endef

define Package/resize2fs
  $(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem resize utility
  DESCRIPTION:=Ext2 Filesystem resize utility
  DEPENDS:=e2fsprogs
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--enable-elf-shlibs \
	--enable-dynamic-e2fsck

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/util \
		BUILDCC="$(HOSTCC)" \
		CFLAGS="" \
		CPPFLAGS="" \
		LDFLAGS="" \
		subst
	$(MAKE) -C $(PKG_BUILD_DIR) \
		BUILDCC="$(HOSTCC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/e2fsprogs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/e2fsck $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/mke2fs $(1)/usr/sbin/
	ln -sf mke2fs $(1)/usr/sbin/mkfs.ext2
	ln -sf mke2fs $(1)/usr/sbin/mkfs.ext3
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{blkid,com_err,e2p,ext2fs,uuid}.so.* $(1)/usr/lib/
endef

define Package/tune2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/tune2fs $(1)/usr/sbin/
endef

define Package/resize2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/resize2fs $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,e2fsprogs))
$(eval $(call BuildPackage,tune2fs))
$(eval $(call BuildPackage,resize2fs))
