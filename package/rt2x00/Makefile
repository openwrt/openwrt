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
PKG_VERSION:=$(KERNEL_VERSION)

include $(INCLUDE_DIR)/package.mk


RT61FW:=RT61_Firmware_V1.2.zip
RT71FW:=RT71W_Firmware_V1.8.zip

define Download/rt61
  FILE:=$(RT61FW)
  URL:=http://www.ralinktech.com.tw/data/
  MD5SUM:=d4c690c93b470bc9a681297c2adc6281
endef
$(eval $(call Download,rt61))

define Download/rt71w
  FILE:=$(RT71FW)
  URL:=http://www.ralinktech.com.tw/data/
  MD5SUM:=1e7a5dc574e0268574fcda3fd5cf52f7
endef
$(eval $(call Download,rt71w))

# XXX: remove @!TARGET_* later when we have PCI & USB support properly detected on all targets
define KernelPackage/rt2x00/Default
  SUBMENU:=Wireless Drivers
  TITLE:=Ralink Drivers for RT2x00 cards
  DEPENDS:=@LINUX_2_6 @!TARGET_atheros @!TARGET_uml
endef

define KernelPackage/rt2x00-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT||USB_SUPPORT +kmod-mac80211
  TITLE+= (LIB)
  FILES:=$(PKG_BUILD_DIR)/rt2x00lib.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,blkcipher ecb arc4 rt2x00lib)
endef

define KernelPackage/rt2x00-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-lib +kmod-eeprom-93cx6
  TITLE+= (PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2x00pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00pci)
endef

define KernelPackage/rt2x00-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-lib +kmod-usb-core
  TITLE+= (USB)
  FILES:=$(PKG_BUILD_DIR)/rt2x00usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00usb)
endef

define KernelPackage/rt2400-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2400 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2400pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2400pci)
endef

define KernelPackage/rt2500-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2500 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2500pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500pci)
endef

define KernelPackage/rt2500-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb +kmod-crc-itu-t
  TITLE+= (RT2500 USB)
  FILES:=$(PKG_BUILD_DIR)/rt2500usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500usb)
endef

define KernelPackage/rt61-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci +kmod-crc-itu-t
  TITLE+= (RT2x61 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt61pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt61pci)
endef

define KernelPackage/rt73-usb
  $(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb
  TITLE+= (RT73 USB)
  FILES:=$(PKG_BUILD_DIR)/rt73usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt73usb)
endef

$(STAMP_PREPARED): $(DL_DIR)/$(PKG_FW6X_NAME) $(DL_DIR)/$(PKG_FW7X_NAME)

PKG_EXTRA_KCONFIG:= \
	CONFIG_MAC80211=y \
	CONFIG_RT2X00=y \
	CONFIG_RT2X00_DEBUG=y \
	CONFIG_RT2X00_LIB_FIRMWARE=y \

ifneq ($(CONFIG_PACKAGE_kmod-rt2x00-pci),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT2X00PCI=y
endif
ifneq ($(CONFIG_PACKAGE_kmod-rt2x00-usb),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT2X00USB=y
endif
ifneq ($(CONFIG_PACKAGE_kmod-rt2400-pci),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT2400PCI=y
endif
ifneq ($(CONFIG_PACKAGE_kmod-rt2500-pci),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT2500PCI=y
endif
ifneq ($(CONFIG_PACKAGE_kmod-rt2500-usb),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT2500USB=y
endif
ifneq ($(CONFIG_PACKAGE_kmod-rt61-pci),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT61PCI=y
endif
ifneq ($(CONFIG_PACKAGE_kmod-rt73-usb),)
  PKG_EXTRA_KCONFIG+= CONFIG_RT73USB=y
endif

PKG_EXTRA_CFLAGS:= \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) src/* $(PKG_BUILD_DIR)/
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT61FW)
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT71FW)
endef

define Build/Configure
	touch $(PKG_BUILD_DIR)/rt2x00_config.h
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		V="$(V)" \
		modules
endef

define KernelPackage/rt61-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rt2?61*.bin $(1)/lib/firmware/
endef

define KernelPackage/rt73-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rt73.bin $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,rt2x00-lib))
$(eval $(call KernelPackage,rt2x00-pci))
$(eval $(call KernelPackage,rt2x00-usb))
$(eval $(call KernelPackage,rt2400-pci))
$(eval $(call KernelPackage,rt2500-pci))
$(eval $(call KernelPackage,rt2500-usb))
$(eval $(call KernelPackage,rt61-pci))
$(eval $(call KernelPackage,rt73-usb))
