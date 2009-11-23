#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6562 2009-10-05 08:30:14 ghd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=compcache
PKG_VERSION:=0.5.4
PKG_RELEASE:=$(PKG_VERSION)-1
PKG_SOURCE_URL:=http://compcache.googlecode.com/files/
PKG_MD5SUM:=e83535925a014ac34e1eaeb4f7a2f49a

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define KernelPackage/compcache
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6 @BUSYBOX_CONFIG_SWAPONOFF
  TITLE:=Driver for compressed ram swap device
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)+$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/ramzswap.$(LINUX_KMOD_SUFFIX) \
	  $(PKG_BUILD_DIR)/sub-projects/compression/lzo-kmod/lzo1x.$(LINUX_KMOD_SUFFIX) \
	  $(PKG_BUILD_DIR)/sub-projects/allocators/xvmalloc-kmod/xvmalloc.$(LINUX_KMOD_SUFFIX)
endef

XVM = sub-projects/allocators/xvmalloc-kmod
LZO = sub-projects/compression/lzo-kmod

BUILDFLAGS:=-DCONFIG_BLK_DEV_RAMZSWAP_STATS \
	    -I$(PKG_BUILD_DIR)/$(XVM) \
	    -I$(PKG_BUILD_DIR)/$(LZO) \
	    -g -Wall

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(KERNEL_CROSS)" \
		CC="$(KERNEL_CC)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

define KernelPackage/compcache/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/compcache.config $(1)/etc/config/compcache
	$(SED) 's,%ENABLED%,$(if $(CONFIG_COMPCACHE_ENABLE),1,0),g' \
	      -e 's,%RAM_REPORTED%,$(call qstrip,$(CONFIG_COMPCACHE_RAM_REPORTED)),g' \
	      -e 's,%BACKUP_DEV%,$(call qstrip,$(CONFIG_COMPCACHE_BACKUP_DEV)),g' \
	      $(1)/etc/config/compcache
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/compcache.init $(1)/etc/init.d/compcache
endef

define KernelPackage/compcache/config
	source "$(SOURCE)/Config.in"
endef

$(eval $(call KernelPackage,compcache))
