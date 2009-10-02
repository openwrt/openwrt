# Copyright (C) 2009 OpenWrt.org
# All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# blogic@openwrt.org
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ifxmips-atm
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ifxmips-atm
  SUBMENU:=Network Devices
  DEPENDS:=@BROKEN @TARGET_ifxmips +kmod-atm
  TITLE:=ifxmips atm driver
  FILES:=$(PKG_BUILD_DIR)/ifx-atm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ifx-atm)
endef

define Kernel/Package/ifxmips-atm/description
	This package provides the atm driver needed to make dsl work on ifxmips based boards
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	modules
endef

define KernelPackage/ifxmips-atm/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION) 
	$(CP) $(PKG_BUILD_DIR)/ifx-atm.ko $(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call KernelPackage,ifxmips-atm))

