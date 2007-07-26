# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-rt2x00
PKG_VERSION:=git-200706018

include $(INCLUDE_DIR)/package.mk

define Package/rt2x00/Default
  SUBMENU:=Wireless Drivers
  DEPENDS:=@LINUX_2_6 +kmod-mac80211 @LINUX_2_6_X86||@LINUX_2_6_RDC
  TITLE:=Ralink GPL Drivers
  DESCRIPTION:=Ralink GPL Drivers for rt2x00 cards
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(LINUX_RELEASE)
endef

define KernelPackage/rt2x00-lib
  $(call Package/rt2x00/Default)
  TITLE+= (LIB)
  DESCRIPTION+= (LIB)
  FILES:=$(PKG_BUILD_DIR)/rt2x00lib.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,rt2x00lib)
endef

define KernelPackage/rt2x00-pci
  $(call Package/rt2x00/Default)
  TITLE+= (PCI)
  DESCRIPTION+= (PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2x00pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00pci)
endef

define KernelPackage/rt2x00-usb
  $(call Package/rt2x00/Default)
  TITLE+= (USB)
  DESCRIPTION+= (USB)
  FILES:=$(PKG_BUILD_DIR)/rt2x00usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00usb)
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -r src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SOURCE="$(LINUX_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS) -include $(PKG_BUILD_DIR)/rt2x00_compat.h" \
		KDIR="$(LINUX_DIR)"
endef

$(eval $(call KernelPackage,rt2x00-lib))
$(eval $(call KernelPackage,rt2x00-pci))
$(eval $(call KernelPackage,rt2x00-usb))
