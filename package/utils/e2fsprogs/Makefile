#
# Copyright (C) 2006-2012 OpenWrt.org
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
  DEPENDS:=+libblkid +libuuid +libext2fs
endef

define Package/e2fsprogs/description
 This package contains essential ext2 filesystem utilities which consists of
 e2fsck, mke2fs, tune2fs, and most of the other core ext2
 filesystem utilities.
endef

define Package/libext2fs
$(call Package/e2fsprogs/Default)
  DEPENDS:=+libcom_err
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ext2/3/4 filesystem library
endef

define Package/libext2fs/description
 libext2fs is a library which can access ext2, ext3 and ext4 filesystems.
endef

define Package/libcom_err
$(call Package/e2fsprogs/Default)
  DEPENDS:=+libpthread
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Common error description library
endef

define Package/libcom_err/description
 libcom_err is a library providing common error descriptions
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

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-elf-shlibs	\
	--disable-libuuid	\
	--disable-libblkid	\
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
		BUILDCC="$(HOSTCC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
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
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libe2p.so.* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/lib/functions/fsck
	$(INSTALL_DATA) ./files/e2fsck.sh $(1)/lib/functions/fsck/
	$(INSTALL_DATA) ./files/e2fsck.conf $(1)/etc/e2fsck.conf
endef

define Package/libcom_err/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcom_err.so.* $(1)/usr/lib/
endef

define Package/libext2fs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libext2fs.so.* $(1)/usr/lib/
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

$(eval $(call BuildPackage,e2fsprogs))
$(eval $(call BuildPackage,libext2fs))
$(eval $(call BuildPackage,libcom_err))
$(eval $(call BuildPackage,tune2fs))
$(eval $(call BuildPackage,resize2fs))
$(eval $(call BuildPackage,badblocks))
