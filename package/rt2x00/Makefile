# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt2x00
PKG_VERSION:=cvs-20070712

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

define KernelPackage/rt61-pci/install
	mkdir -p $(1)/lib/firmware
	$(CP) -L $(PKG_BUILD_DIR)/rt2?61*.bin $(1)/lib/firmware
endef

define KernelPackage/rt73-usb
  $(call Package/rt2x00/Default)
  DEPENDS:=+kmod-rt2x00-usb
  TITLE+= (RT73 USB)
  DESCRIPTION+= (RT73 USB)
  FILES:=$(PKG_BUILD_DIR)/rt73usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt73usb)
endef

define KernelPackage/rt73-usb/install
	mkdir -p $(1)/lib/firmware
	$(CP) -L $(PKG_BUILD_DIR)/rt73.bin $(1)/lib/firmware
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -r src/* $(PKG_BUILD_DIR)/
	wget -N -P $(DL_DIR) http://www.ralinktech.com.tw/data/RT61_Firmware_V1.2.zip
	wget -N -P $(DL_DIR) http://www.ralinktech.com.tw/data/RT71W_Firmware_V1.8.zip
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/RT61_Firmware_V1.2.zip
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/RT71W_Firmware_V1.8.zip
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" config_header
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" V="$(V)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SOURCE="$(LINUX_DIR)" \
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
