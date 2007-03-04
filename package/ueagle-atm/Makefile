# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ueagle-atm
PKG_VERSION:=1.0d1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://castet.matthieu.free.fr/eagle/release/
PKG_MD5SUM:=5da1fceb15168f4c7ad407a3e38ddecb
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ueagle-atm
  SUBMENU:=Network Devices
  DEPENDS:=@ATM_SUPPORT kmod-usb-core 
  TITLE:=Driver for Eagle 8051 based USB ADSL modems
  URL:=http://www.eagle-usb.org/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/usbatm_iso/usbatm.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/driver/ueagle-atm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,usbatm ueagle-atm)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TARGET="$(HAL_TARGET)" \
		KERNELDIR="$(LINUX_DIR)" \
		all
endef

$(eval $(call KernelPackage,ueagle-atm))
