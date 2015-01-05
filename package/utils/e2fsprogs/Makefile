#
# Copyright (C) 2006-2014 OpenWrt.org
# Copyright 2010 Vertical Communications
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.42.4
PKG_MD5SUM:=b6e296f210d642361b7394437ff0f318
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs

PKG_BUILD_DEPENDS:=util-linux
PKG_INSTALL:=1

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
  DEPENDS:=+libuuid +libext2fs
endef

define Package/e2fsprogs/description
 This package contains essential ext2 filesystem utilities which consists of
 e2fsck, mke2fs and most of the other core ext2 filesystem utilities.
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
  DEPENDS:= +e2fsprogs
endef

define Package/resize2fs
$(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem resize utility
  DEPENDS:= +e2fsprogs
endef

define Package/badblocks
$(call Package/e2fsprogs)
  TITLE:=Ext2 Filesystem badblocks utility
  DEPENDS:= +e2fsprogs
endef

TARGET_CFLAGS += $(FPIC) -ffunction-sections -fdata-sections

CONFIGURE_VARS += \
	ac_cv_lib_pthread_sem_init=no

CONFIGURE_ARGS += \
	--disable-testio-debug \
	--enable-elf-shlibs	\
	--disable-libuuid	\
	--enable-libblkid	\
	--disable-uuidd		\
	--disable-tls		\
	--disable-nls		\
	--disable-rpath

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
		LDFLAGS=-Wl,--gc-sections \
		BUILDCC="$(HOSTCC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		LIBBLKID="$(PKG_BUILD_DIR)/lib/libblkid.a -luuid" \
		all
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/lib/ext2fs/ext2fs.pc $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/lib/et/com_err.pc $(1)/usr/lib/pkgconfig

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libext2fs.{so,a}* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libcom_err.{so,a}* $(1)/usr/lib

	$(INSTALL_DIR) $(1)/usr/include/ext2fs
	$(CP) $(PKG_BUILD_DIR)/lib/ext2fs/*.h $(1)/usr/include/ext2fs
	$(INSTALL_DIR) $(1)/usr/include/et
	$(CP) $(PKG_BUILD_DIR)/lib/et/*.h $(1)/usr/include/et
endef

define Package/e2fsprogs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/e2fsck $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mke2fs $(1)/usr/sbin/
	$(LN) mke2fs $(1)/usr/sbin/mkfs.ext2
	$(LN) mke2fs $(1)/usr/sbin/mkfs.ext3
	$(LN) mke2fs $(1)/usr/sbin/mkfs.ext4
	$(LN) e2fsck $(1)/usr/sbin/fsck.ext2
	$(LN) e2fsck $(1)/usr/sbin/fsck.ext3
	$(LN) e2fsck $(1)/usr/sbin/fsck.ext4
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libe2p.so.* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/lib/functions/fsck
	$(INSTALL_DATA) ./files/e2fsck.sh $(1)/lib/functions/fsck/
	$(INSTALL_DATA) ./files/e2fsck.conf $(1)/etc/e2fsck.conf
endef

define Package/libcom_err/install
endef

define Package/libext2fs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libext2fs.so.* \
		$(PKG_INSTALL_DIR)/usr/lib/libcom_err.so.* \
		$(1)/usr/lib/
endef

define Package/libext2fs/install_lib
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/ext2fs/libext2fs.a $(1)/usr/lib/libext2fs_pic.a
endef

define Package/tune2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/tune2fs $(1)/usr/sbin/
	$(LN) tune2fs $(1)/usr/sbin/findfs
endef

define Package/resize2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/resize2fs $(1)/usr/sbin/
endef

define Package/badblocks/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/badblocks $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,e2fsprogs))
$(eval $(call BuildPackage,libext2fs))
$(eval $(call BuildPackage,tune2fs))
$(eval $(call BuildPackage,resize2fs))
$(eval $(call BuildPackage,badblocks))
