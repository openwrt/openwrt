# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=shfs
PKG_VERSION:=0.35
PKG_RELEASE:=2
PKG_MD5SUM:=016f49d71bc32eee2b5d11fc1600cfbe

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=@SF/shfs
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/kmod-shfs
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=
TITLE:=Shell FileSystem (ShFS)
DESCRIPTION:=Shell FileSystem (ShFS) \\\
ShFS is a simple and easy to use Linux kernel module which allows you to \\\
mount remote filesystems using a plain shell (SSH) connection. When using \\\
ShFS, you can access all remote files just like the local ones, only the \\\
access is governed through the transport security of SSH.
URL:=http://shfs.sourceforge.net/
VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		OFLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)" \
		LINKER="$(TARGET_CC)" \
		KERNEL="$(LINUX_VERSION)" \
		KERNEL_SOURCES="$(LINUX_DIR)" \
		ROOT="$(PKG_INSTALL_DIR)" \
		module module-install
endef

define Package/kmod-shfs/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/shfs/shfs.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-shfs))
