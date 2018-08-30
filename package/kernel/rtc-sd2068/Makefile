#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtc-sd2068
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtc-sd2068
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_ar71xx
  KCONFIG:=CONFIG_RTC_CLASS=y
  TITLE:=Driver for RTC SD2068
  AUTOLOAD:=$(call AutoLoad,70,rtc-sd2068)
  FILES:=$(PKG_BUILD_DIR)/rtc-sd2068.ko
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
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,rtc-sd2068))
