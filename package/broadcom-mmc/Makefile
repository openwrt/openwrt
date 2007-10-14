#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-mmc
PKG_VERSION:=0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

MAKEFLAGS_KMOD:= -C "$(LINUX_DIR)" \
                CROSS_COMPILE="$(TARGET_CROSS)" \
                ARCH="$(LINUX_KARCH)" \
                PATH="$(TARGET_PATH)" \
                SUBDIRS="$(PKG_BUILD_DIR)"

define KernelPackage/broadcom-mmc
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_brcm_2_4
  TITLE:=Kernel driver for the Linksys WRT54G MM/SD-Card Mod
  URL:=http://kiel.kool.dk/
  FILES:=$(PKG_BUILD_DIR)/mmc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,80,mmc)
endef 

define KernelPackage/broadcom-mmc/description
	Kernel driver for the Linksys WRT54G MM/SD-Card Mod
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) $(MAKEFLAGS_KMOD) \
		modules
endef

$(eval $(call KernelPackage,broadcom-mmc))
