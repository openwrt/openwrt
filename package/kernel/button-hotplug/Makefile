#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=button-hotplug
PKG_RELEASE:=3
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/button-hotplug
  SUBMENU:=Other modules
  TITLE:=Button Hotplug driver
  DEPENDS:=+kmod-input-core
  FILES:=$(PKG_BUILD_DIR)/button-hotplug.ko
  AUTOLOAD:=$(call AutoLoad,30,button-hotplug,1)
  KCONFIG:=
endef

define KernelPackage/button-hotplug/description
  Kernel module to generate button uevent-s from input subsystem events.
  If your device uses GPIO buttons, see gpio-button-hotplug.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,button-hotplug))
