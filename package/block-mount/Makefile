#
# Copyright (C) 2006-2009 OpenWrt.org
# Copyright 2010 Vertical Communications
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=block-mount
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/block-mount/Default
  SECTION:=base
  CATEGORY:=Base system
endef

define Package/block-mount
  $(call Package/block-mount/Default)
  TITLE:=Block device mounting and checking
endef

define Package/block-mount/config
  select BUSYBOX_CONFIG_MKSWAP
  select BUSYBOX_CONFIG_FEATURE_MKSWAP_UUID
  select BUSYBOX_CONFIG_SWAPONOFF
  select BUSYBOX_CONFIG_BLKID
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_EXT
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_REISERFS
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_FAT
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_HFS
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_JFS
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_XFS
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_NTFS
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_SYSV
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_OCFS2
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_ISO9660
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_UDF
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_LINUXSWAP
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_CRAMFS
  select BUSYBOX_CONFIG_FEATURE_VOLUMEID_ROMFS
endef

define Package/block-mount/description
  Scripts used to mount and check block devices (filesystems and swap)
endef

define Package/block-hotplug
  $(call Package/block-mount/Default)
  TITLE:=Automount and autocheck block devices
  DEPENDS:=block-mount hotplug2
endef

define Package/block-hotplug/description
  Scripts used to automatically check and mount filesystem and/or swap
endef

define Build/Compile
endef

define Package/block-mount/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/fstab.init $(1)/etc/init.d/fstab
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/fstab.config $(1)/etc/config/fstab
	$(INSTALL_DIR) $(1)/lib/functions
	$(INSTALL_DATA) ./files/mount.sh $(1)/lib/functions/
	$(INSTALL_DATA) ./files/fsck.sh $(1)/lib/functions/
	$(INSTALL_DATA) ./files/block.sh $(1)/lib/functions/
endef

define Package/block-hotplug/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/block
	$(INSTALL_DATA) ./files/10-swap $(1)/etc/hotplug.d/block/
	$(INSTALL_DATA) ./files/20-fsck $(1)/etc/hotplug.d/block/
	$(INSTALL_DATA) ./files/40-mount $(1)/etc/hotplug.d/block/
endef

$(eval $(call BuildPackage,block-mount))
$(eval $(call BuildPackage,block-hotplug))

