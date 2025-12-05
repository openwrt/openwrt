#
# Copyright (C) 2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nilfs-utils
PKG_VERSION:=2.2.12
PKG_RELEASE:=1
PKG_URL:=https://nilfs.sourceforge.io

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://nilfs.sourceforge.io/download/
PKG_HASH:=fc9a8520b68928d43fffa465c3b3845cc9b7d4973f4fbde4b3c7ecf25ce52d09

PKG_MAINTAINER:=Pavlo Samko <bulldozerbsg@gmail.com>
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:nilf:nilfs

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/nilfs-mkfs
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=create a NILFS2 filesystem
  DEPENDS:=+libuuid +libblkid
endef

define Package/nilfs-cleanerd
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=NILFS2 garbage collector daemon
  DEPENDS:=+libuuid @KERNEL_POSIX_MQUEUE
endef

define Package/nilfs-mount
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=mount, unmount a NILFS2 file system
  DEPENDS:=+libmount +libblkid +nilfs-cleanerd
endef

define Package/libnilfs
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libnilfs
endef

define Package/libnilfsgc
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libnilfsgc
  DEPENDS:=+libnilfs
endef

define Package/libnilfscleaner
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libnilfscleaner
  DEPENDS:=+libuuid +nilfs-cleanerd @KERNEL_POSIX_MQUEUE
endef

define Package/nilfs-clean
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=run garbage collector on NILFS file system
  DEPENDS:=+libnilfs +libnilfscleaner
endef

define Package/nilfs-resize
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=resize NILFS file system volume size
  DEPENDS:=+libnilfs +libnilfsgc
endef

define Package/nilfs-tune
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=adjust tunable file system parameters on NILFS file system
  DEPENDS:=+libnilfs
endef

define Package/nilfs-checkpoint
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=make, change, remove, list a NILFS2 checkpoint
  DEPENDS:=+libnilfs
endef

define Package/nilfs-dumpseg
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=print segment information of NILFS2
  DEPENDS:=+libnilfs
endef

define Package/nilfs-lssu
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=list usage state of NILFS2 segments
  DEPENDS:=+libnilfs +libnilfsgc
endef

# libmount is used and support of the selinux context mount options
# depends on the libmount that distro provides.
CONFIGURE_ARGS += \
	--without-selinux

define Package/nilfs-mkfs/install
	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/mkfs.nilfs2 $(1)/sbin/
endef

define Package/nilfs-cleanerd/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/nilfs_cleanerd.conf $(1)/etc/

	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/nilfs_cleanerd $(1)/sbin/
endef

define Package/nilfs-mount/install
	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/mount.nilfs2 $(1)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/umount.nilfs2 $(1)/sbin/
endef

define Package/libnilfs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfs.so* $(1)/usr/lib/
endef

define Package/libnilfsgc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfsgc.so* $(1)/usr/lib/
endef

define Package/libnilfscleaner/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnilfscleaner.so* $(1)/usr/lib/
endef

define Package/nilfs-clean/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nilfs-clean $(1)/usr/sbin/
endef

define Package/nilfs-resize/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nilfs-resize $(1)/usr/sbin/
endef

define Package/nilfs-tune/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nilfs-tune $(1)/usr/sbin/
endef

define Package/nilfs-checkpoint/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/chcp $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/lscp $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mkcp $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rmcp $(1)/usr/bin/
endef

define Package/nilfs-dumpseg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/dumpseg $(1)/usr/bin/
endef

define Package/nilfs-lssu/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/lssu $(1)/usr/bin/
endef

$(eval $(call BuildPackage,nilfs-mkfs))
$(eval $(call BuildPackage,nilfs-cleanerd))
$(eval $(call BuildPackage,nilfs-mount))
$(eval $(call BuildPackage,libnilfs))
$(eval $(call BuildPackage,libnilfsgc))
$(eval $(call BuildPackage,libnilfscleaner))
$(eval $(call BuildPackage,nilfs-clean))
$(eval $(call BuildPackage,nilfs-resize))
$(eval $(call BuildPackage,nilfs-tune))
$(eval $(call BuildPackage,nilfs-checkpoint))
$(eval $(call BuildPackage,nilfs-dumpseg))
$(eval $(call BuildPackage,nilfs-lssu))
