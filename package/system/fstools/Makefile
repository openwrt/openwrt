#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fstools
PKG_VERSION:=2014-10-13

PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/fstools.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=d71297353dc45eaf8f7c252246490746708530f9
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1
PKG_CHECK_FORMAT_SECURITY:=0

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_USE_MIPS16:=0

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_EGLIBC),-lrt)

define Package/fstools
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+ubox +USE_EGLIBC:librt +NAND_SUPPORT:ubi-utils
  TITLE:=OpenWrt filesystem tools
endef

define Package/block-mount
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Block device mounting and checking
  DEPENDS:=+ubox +libubox +libuci
endef

define Package/fstools/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{mount_root,jffs2reset,snapshot_tool} $(1)/sbin/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libfstools.so $(1)/lib/
	$(INSTALL_BIN) ./files/snapshot $(1)/sbin/
	ln -s /sbin/jffs2reset $(1)/sbin/jffs2mark
endef

define Package/block-mount/install
	$(INSTALL_DIR) $(1)/sbin $(1)/lib $(1)/usr/sbin $(1)/etc/hotplug.d/block $(1)/etc/init.d/ $(1)/etc/uci-defaults/

	$(INSTALL_BIN) ./files/fstab.init $(1)/etc/init.d/fstab
	$(INSTALL_DATA) ./files/fstab.default $(1)/etc/uci-defaults/10-fstab
	$(INSTALL_DATA) ./files/mount.hotplug $(1)/etc/hotplug.d/block/10-mount

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/block $(1)/sbin/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libblkid-tiny.so $(1)/lib/
	ln -s /sbin/block $(1)/usr/sbin/swapon
	ln -s /sbin/block $(1)/usr/sbin/swapoff

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libubi-utils.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,fstools))
$(eval $(call BuildPackage,block-mount))
