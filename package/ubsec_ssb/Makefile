#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ubsec-ssb

CRYPTO_MENU:=OCF Configuration

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ocf-ubsec-ssb
  SUBMENU:=$(CRYPTO_MENU)
  DEPENDS:=@TARGET_brcm47xx
  TITLE:=BCM5365P IPSec Core driver
  FILES:=$(PKG_BUILD_DIR)/src/ubsec_ssb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,ubsec_ssb)
endef

define KernelPackage/ocf-ubsec-ssb/description
	This package contains the OCF driver for the BCM5365p IPSec Core
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -r ./src $(PKG_BUILD_DIR)/
endef

LINUX_PATCHLEVEL:=$(shell echo "$(LINUX_VERSION)" | cut -d. -f2)

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		PATCHLEVEL="$(LINUX_PATCHLEVEL)" \
		KERNDIR="$(LINUX_DIR)" \
		SUBDIRS="$(PKG_BUILD_DIR)/src" \
		modules
endef

$(eval $(call KernelPackage,ocf-ubsec-ssb))
