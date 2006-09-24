# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mini_fo
PKG_VERSION:=0.6.2pre1
PKG_RELEASE:=1

PKG_SOURCE:=mini_fo-0-6-2-pre1.tar.bz2
PKG_SOURCE_URL:=http://www.denx.de/twiki/pub/Know/MiniFOHome/
PKG_MD5SUM:=acd12157be25d7b3c8df90914049403e
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/mini_fo-0-6-2-pre1

include $(INCLUDE_DIR)/package.mk

define Package/kmod-mini-fo
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  TITLE:=The mini fanout overlay file system
  URL:=http://www.denx.de/wiki/Know.MiniFOHome
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SRC="$(LINUX_DIR)" \
		all
endef

define Package/kmod-mini-fo/install
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/mini_fo.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	install -m0755 -d $(1)/etc/modules.d
	echo "mini_fo" > $(1)/etc/modules.d/15-mini-fo
endef

$(eval $(call BuildPackage,kmod-mini-fo))
