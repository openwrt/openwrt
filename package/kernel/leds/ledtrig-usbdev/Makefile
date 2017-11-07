#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ledtrig-usbdev
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ledtrig-usbdev
  SUBMENU:=LED modules
  TITLE:=LED USB device Trigger
  DEPENDS:=@USB_SUPPORT +kmod-usb-core
  FILES:=$(PKG_BUILD_DIR)/ledtrig-usbdev.ko
  AUTOLOAD:=$(call AutoLoad,50,ledtrig-usbdev)
endef

define KernelPackage/ledtrig-usbdev/description
 Kernel module to drive LEDs based on USB device presence/activity
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)"
endef

$(eval $(call KernelPackage,ledtrig-usbdev))
