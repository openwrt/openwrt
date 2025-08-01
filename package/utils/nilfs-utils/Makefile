#
# Copyright (C) 2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nilfs-utils
PKG_VERSION:=2.2.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://nilfs.sourceforge.io/download/
PKG_HASH:=8602897ff0d2c49be9bc76311f0b102088e58b6de4f749009403de06ff2c34cd

PKG_MAINTAINER:=Pavlo Samko <bulldozerbsg@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:nilf:nilfs

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/nilfs-utils
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=Userland utilities for NILFS2
  URL:=https://nilfs.sourceforge.io
  DEPENDS:=+libuuid +libmount +libblkid +libpthread +librt @KERNEL_POSIX_MQUEUE
endef

define Package/nilfs-utils/description
  NILFS is a log-structured file system developed for Linux. 
  NILFS provides versioning capability of an entire file system and 
  continuous snapshotting that allows users to restore files mistakenly 
  overwritten or destroyed a while ago. 
endef

# libmount is used and support of the selinux context mount options
# depends on the libmount that distro provides.
CONFIGURE_ARGS += \
	--without-selinux

define Package/nilfs-utils/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/nilfs_cleanerd.conf $(1)/etc

	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/mkfs.nilfs2 $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/nilfs_cleanerd $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/mount.nilfs2 $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/umount.nilfs2 $(1)/sbin/

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/chcp $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/dumpseg $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/lscp $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/lssu $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mkcp $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rmcp $(1)/usr/bin/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfsgc.so $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfsgc.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfs.so $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfs.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfscleaner.so.* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nilfs-clean $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nilfs-resize $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nilfs-tune $(1)/usr/sbin
endef

$(eval $(call BuildPackage,nilfs-utils))
