#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6562 2009-01-21 11:19:24 ghd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=compcache
PKG_VERSION:=0.5
PKG_RELEASE:=$(PKG_VERSION)-1
PKG_SOURCE_URL:=http://compcache.googlecode.com/files/
PKG_MD5SUM:=eea73324e9e69178866f6c0fbc852f1f

#PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define KernelPackage/compcache
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6 @BUSYBOX_CONFIG_SWAPONOFF
  TITLE:=Driver for compressed ram swap device
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)+$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/compcache.$(LINUX_KMOD_SUFFIX) \
         $(PKG_BUILD_DIR)/sub-projects/compression/lzo-kmod/lzo1x.$(LINUX_KMOD_SUFFIX) \
	 $(PKG_BUILD_DIR)/sub-projects/allocators/xvmalloc-kmod/xvmalloc.$(LINUX_KMOD_SUFFIX)
#  AUTOLOAD:=$(call AutoLoad,10,xvmalloc lzo1x compcache+compcache_size_kbytes=2048)
endef

BUILDFLAGS:=-DCONFIG_BLK_DEV_COMPCACHE_DEBUG=0 \
	    -DCONFIG_BLK_DEV_COMPCACHE_STATS=1 \
	    -DCONFIG_XV_DEBUG=0 \
	    -DCONFIG_XV_STATS=1 \
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
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/compcache.init $(1)/etc/init.d/compcache
endef

$(eval $(call KernelPackage,compcache))
