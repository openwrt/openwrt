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

PKG_FW6X_NAME:=RT61_Firmware_V1.2.zip
PKG_FW6X_URL:=http://www.ralinktech.com.tw/data/
PKG_FW6X_MD5SUM:=fe64f389f0863ef349a8c97e0fbc8838

PKG_FW7X_NAME:=RT71W_Firmware_V1.8.zip
PKG_FW7X_URL:=http://www.ralinktech.com.tw/data/
PKG_FW7X_MD5SUM:=db4ef70b61e483f34db496738507b275

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  include $(LINUX_DIR)/.config
endif

define KernelPackage/rt2x00/Default
  SUBMENU:=Wireless Drivers
  TITLE:=Ralink Drivers for RT2x00 cards
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_MAC80211
endef

define KernelPackage/rt2x00-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-mac80211 +kmod-crypto
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
  DEPENDS+= +kmod-rt2x00-pci
  TITLE+= (RT2400 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2400pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2400pci)
endef

define KernelPackage/rt2500-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-rt2x00-pci
  TITLE+= (RT2500 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt2500pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500pci)
endef

define KernelPackage/rt2500-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-rt2x00-usb +kmod-crc-itu-t
  TITLE+= (RT2500 USB)
  FILES:=$(PKG_BUILD_DIR)/rt2500usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500usb)
endef

define KernelPackage/rt61-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-rt2x00-pci +kmod-crc-itu-t
  TITLE+= (RT2x61 PCI)
  FILES:=$(PKG_BUILD_DIR)/rt61pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt61pci)
endef

define KernelPackage/rt73-usb
  $(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-rt2x00-usb
  TITLE+= (RT73 USB)
  FILES:=$(PKG_BUILD_DIR)/rt73usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt73usb)
endef

$(STAMP_PREPARED): $(DL_DIR)/$(PKG_FW6X_NAME) $(DL_DIR)/$(PKG_FW7X_NAME)

$(DL_DIR)/$(PKG_FW6X_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW6X_NAME)" "$(PKG_FW6X_MD5SUM)" $(PKG_FW6X_URL)

$(DL_DIR)/$(PKG_FW7X_NAME):
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_FW7X_NAME)" "$(PKG_FW7X_MD5SUM)" $(PKG_FW7X_URL)

# only try to build if the kernel is recent enough (has in-kernel mac80211)
ifndef ($(CONFIG_MAC80211),)

  PKG_EXTRA_KCONFIG:= \
	CONFIG_RT2X00=y \
	CONFIG_RT2X00_DEBUG=y \

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

  define Build/Compile/it
	$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		V="$(V)" \
		modules
  endef

endif

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -r src/* $(PKG_BUILD_DIR)/
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(PKG_FW6X_NAME)
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(PKG_FW7X_NAME)
endef

define Build/Configure
	touch $(PKG_BUILD_DIR)/rt2x00_config.h
endef

define Build/Compile
$(call Build/Compile/it)
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
