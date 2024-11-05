#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=reset-boot-count
PKG_RELEASE:=1
PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk

define KernelPackage/reset-boot-count
  SUBMENU:=Other modules
  TITLE:=Reset dual-boot boot counter for MediaTek platform
  FILES:=$(PKG_BUILD_DIR)/reset-boot-count.ko
  KCONFIG:=
endef

define KernelPackage/reset-boot-count/description
  This is used to reset dual-boot boot retry counter to indicate that the
  system has booted up successfully
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

define KernelPackage/reset-boot-count/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/reset-boot-count.init $(1)/etc/init.d/reset-boot-count
endef

$(eval $(call KernelPackage,reset-boot-count))
