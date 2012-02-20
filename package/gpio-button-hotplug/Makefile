#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-button-hotplug
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-button-hotplug
  SUBMENU:=Other modules
  TITLE:=Simple GPIO Button Hotplug driver
  FILES:=$(PKG_BUILD_DIR)/gpio-button-hotplug.ko
  AUTOLOAD:=$(call AutoLoad,30,gpio-button-hotplug)
  KCONFIG:=
endef

define KernelPackage/button-hotplug/description
  Kernel module to generate GPIO button hotplug events
endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)"

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,gpio-button-hotplug))
