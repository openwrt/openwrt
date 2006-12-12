# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt2x00
PKG_VERSION:=1.1.0-b1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/rt61-$(PKG_VERSION)/
PKG_SOURCE:=rt61-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rt2400
PKG_MD5SUM:=3f2600400a3d6d878a5d27d0a2e47eed
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk

define Package/kmod-rt2x00
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DEPENDS:=@LINUX_2_6_RDC
  TITLE:=Driver for Ralink RT61xx based boards
  URL:=http://rt2x00.serialmonkey.com/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		PATCHLEVEL="$(LINUX_VERSION)" \
		KERNDIR="$(LINUX_DIR)" \
		SUBDIRS="$(PKG_BUILD_DIR)/Module" \
		modules
endef

define Package/kmod-rt2x00/install
	mkdir -p $(1)/etc/modules.d
	$(INSTALL_DATA) ./files/rt2x00.modules $(1)/etc/modules.d/10-ralink
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/Module/rt61.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-rt2x00))
