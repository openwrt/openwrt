#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
ifneq ($(findstring "0.9.29",$(CONFIG_UCLIBC_VERSION)),"0.9.29")
PKG_VERSION:=1.41.9
PKG_MD5SUM:=52f60a9e19a02f142f5546f1b5681927
else
PKG_VERSION:=1.40.11
PKG_MD5SUM:=004cea70d724fdc7f1a952dffe4c9db8
endif
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs

include $(INCLUDE_DIR)/package.mk

define Package/e2fsprogs/Default
  URL:=http://e2fsprogs.sourceforge.net/
  SUBMENU:=Filesystem
endef

define Package/e2fsprogs
$(call Package/e2fsprogs/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Ext2/3/4 filesystem utilities
  DEPENDS:=+libblkid +libuuid +libext2fs +libpthread
endef

define Package/e2fsprogs/description
 This package contains essential ext2 filesystem utilities which consists of 
 e2fsck, mke2fs, debugfs, dumpe2fs, tune2fs, and most of the other core ext2 
 filesystem utilities.
endef

define Package/libuuid
$(call Package/e2fsprogs/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=DCE compatible Universally Unique Identifier library
endef

define Package/libuuid/description
 Library for generating DCE compatible Universally Unique Identifiers.
endef

define Package/uuidgen
$(call Package/e2fsprogs)
  DEPENDS:=libuuid
  TITLE:=Command line utility to create a new UUID value
endef

define Package/uuidgen/description
 uuidgen program creates a new universally unique identifier (UUID) 
 using the libuuid library. The new UUID can reasonably be considered 
 unique among all UUIDs created on the local system, and among UUIDs 
 created on other systems in the past and in the future.
endef

define Package/libblkid
$(call Package/e2fsprogs/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=block device id library
endef

define Package/libblkid/description
 The blkid library which allows system programs like fsck and mount to 
 quickly and easily find block devices by filesystem UUID and LABEL.
endef

define Package/libext2fs
$(call Package/e2fsprogs/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ext2/3/4 filesystem library
endef

define Package/libext2fs/description
libext2fs is a library which can access ext2, ext3 and ext4 filesystems.
endef

define Package/tune2fs
$(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem tune utility
  DEPENDS:=e2fsprogs
endef

define Package/resize2fs
$(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem resize utility
  DEPENDS:=e2fsprogs
endef

define Package/badblocks
$(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem badblocks utility
  DEPENDS:=e2fsprogs
endef

define Package/blkid
$(call Package/e2fsprogs)
  TITLE:=Command-line utility to locate/print block device attributes
  DEPENDS:=libuuid libblkid
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--enable-elf-shlibs \
	--enable-dynamic-e2fsck \
	--disable-tls

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/config/
endef

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

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) \
		BUILDCC="$(HOSTCC)" \
		DESTDIR="$(1)" \
		install-libs
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/ext2fs \
		BUILDCC="$(HOSTCC)" \
		DESTDIR="$(1)" \
		install
endef

define Package/e2fsprogs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/e2fsck $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mke2fs $(1)/usr/sbin/
	ln -sf mke2fs $(1)/usr/sbin/mkfs.ext2
	ln -sf mke2fs $(1)/usr/sbin/mkfs.ext3
	ln -sf mke2fs $(1)/usr/sbin/mkfs.ext4
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(foreach lib,com_err e2p,$(PKG_INSTALL_DIR)/usr/lib/lib$(lib).so.*) $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/e2fsck.init $(1)/etc/init.d/e2fsck

endef

define Package/libuuid/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libuuid.so.* $(1)/usr/lib/
endef

define Package/uuidgen/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/uuidgen $(1)/usr/bin/
endef

define Package/libblkid/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libblkid.so.* $(1)/usr/lib/
endef

define Package/libext2fs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libext2fs.so* $(1)/usr/lib/
endef

define Package/tune2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/tune2fs $(1)/usr/sbin/
endef

define Package/resize2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/resize2fs $(1)/usr/sbin/
endef

define Package/badblocks/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/badblocks $(1)/usr/sbin/
endef

define Package/blkid/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/blkid $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,e2fsprogs))
$(eval $(call BuildPackage,libuuid))
$(eval $(call BuildPackage,uuidgen))
$(eval $(call BuildPackage,libblkid))
$(eval $(call BuildPackage,libext2fs))
$(eval $(call BuildPackage,tune2fs))
$(eval $(call BuildPackage,resize2fs))
$(eval $(call BuildPackage,badblocks))
$(eval $(call BuildPackage,blkid))
