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
  TITLE:=Ralink GPL Drivers
  DESCRIPTION:=Ralink GPL Drivers for rt2x00 cards
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(LINUX_RELEASE)
endef

define KernelPackage/rt2x00-lib
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-mac80211
  TITLE+= (LIB)
  DESCRIPTION+= (LIB)
  FILES:=$(PKG_BUILD_DIR)/rt2x00lib.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,rt2x00lib)
endef

define KernelPackage/rt2x00-pci
  $(call Package/rt2x00/Default)
  DEPENDS:=@PCI_SUPPORT +kmod-rt2x00-lib
  TITLE+= (PCI)
  DESCRIPTION+= (PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2x00pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00pci)
endef

define KernelPackage/rt2x00-usb
  $(call Package/rt2x00/Default)
  DEPENDS:=@USB_SUPPORT +kmod-rt2x00-lib
  TITLE+= (USB)
  DESCRIPTION+= (USB)
  FILES:=$(PKG_BUILD_DIR)/rt2x00usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00usb)
endef

define KernelPackage/rt2400-pci
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-rt2x00-pci
  TITLE+= (RT2400 PCI)
  DESCRIPTION+= (RT2400 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2400pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2400pci)
endef

define KernelPackage/rt2500-pci
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-rt2x00-pci
  TITLE+= (RT2500 PCI)
  DESCRIPTION+= (RT2500 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2500pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500pci)
endef

define KernelPackage/rt2500-usb
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-rt2x00-usb
  TITLE+= (RT2500 USB)
  DESCRIPTION+= (RT2500 USB)
  FILES:=$(PKG_BUILD_DIR)/rt2500usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500usb)
endef

define KernelPackage/rt61-pci
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-rt2x00-pci
  TITLE+= (RT2x61 PCI)
  DESCRIPTION+= (RT2x61 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt61pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt61pci)
endef

define KernelPackage/rt73-usb
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-rt2x00-usb
  TITLE+= (RT73 USB)
  DESCRIPTION+= (RT73 USB)
  FILES:=$(PKG_BUILD_DIR)/rt73usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt73usb)
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -r src/* $(PKG_BUILD_DIR)/
	sed 's/\$$$$(CONFIG_RT.*)/m\t\t/g' src/Makefile > $(PKG_BUILD_DIR)/Makefile
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
$(eval $(call KernelPackage,rt2400-pci))
$(eval $(call KernelPackage,rt2500-pci))
$(eval $(call KernelPackage,rt2500-usb))
$(eval $(call KernelPackage,rt61-pci))
$(eval $(call KernelPackage,rt73-usb))
