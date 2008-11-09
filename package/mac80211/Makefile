#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 7440 2007-06-02 02:22:01Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mac80211
PKG_RELEASE:=1

ifneq ($(CONFIG_LINUX_2_6_27),)
  PKG_VERSION:=2008-11-03
  PKG_SOURCE_URL:=http://www.orbit-lab.org/kernel/compat-wireless-2.6/2008/11
else
  PKG_VERSION:=2008-08-06
  PKG_SOURCE_URL:=http://www.orbit-lab.org/kernel/compat-wireless-2.6/2008/08
  PATCH_DIR:=./patches-old
endif

PKG_SOURCE:=compat-wireless-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/compat-wireless-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
STAMP_CONFIGURED:=$(STAMP_CONFIGURED)_$(call confvar,CONFIG_PACKAGE_kmod-mac80211)

define KernelPackage/mac80211
  SUBMENU:=Wireless Drivers
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS:=@LINUX_2_6 +kmod-crypto-arc4 +kmod-crypto-aes +wireless-tools @!TARGET_avr32
  FILES:= \
	$(PKG_BUILD_DIR)/net/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211)
endef

define KernelPackage/mac80211/description
Linux 802.11 Wireless Networking Stack
endef

# Ralink rt2x00 drivers
RT61FW:=RT61_Firmware_V1.2.zip
RT71FW:=RT71W_Firmware_V1.8.zip
RT2860FW:=RT2860_Firmware_V11.zip

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

define Download/rt2860
  FILE:=$(RT2860FW)
  URL:=http://www.ralinktech.com.tw/data/drivers
  MD5SUM:=440a81756a52c53528f16faa41c40124
endef
$(eval $(call Download,rt2860))

define KernelPackage/rt2x00/Default
  SUBMENU:=Wireless Drivers
  TITLE:=Ralink Drivers for RT2x00 cards
endef

define KernelPackage/rt2x00-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT||USB_SUPPORT +kmod-mac80211 +kmod-crc-itu-t
  TITLE+= (LIB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00lib.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,blkcipher ecb arc4 rt2x00lib)
endef

define KernelPackage/rt2x00-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-lib +kmod-eeprom-93cx6
  TITLE+= (PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00pci)
endef

define KernelPackage/rt2x00-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-lib +kmod-usb-core
  TITLE+= (USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,rt2x00usb)
endef

define KernelPackage/rt2400-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2400 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2400pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2400pci)
endef

define KernelPackage/rt2500-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2500 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2500pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500pci)
endef

define KernelPackage/rt2500-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb
  TITLE+= (RT2500 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2500usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2500usb)
endef

define KernelPackage/rt61-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2x61 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt61pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt61pci)
endef

define KernelPackage/rt73-usb
  $(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb
  TITLE+= (RT73 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt73usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt73usb)
endef

define KernelPackage/rt2800-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci @LINUX_2_6_27
  TITLE+= (RT2860 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2800pci)
endef

ZD1211FW_NAME:=zd1211-firmware
ZD1211FW_VERSION:=1.4
define Download/zd1211rw
  FILE:=$(ZD1211FW_NAME)-$(ZD1211FW_VERSION).tar.bz2
  URL:=@SF/zd1211/
  MD5SUM:=19f28781d76569af8551c9d11294c870
endef
$(eval $(call Download,zd1211rw))

define KernelPackage/zd1211rw
  SUBMENU:=Wireless Drivers
  TITLE:=Zydas ZD1211 support
  DEPENDS:=@LINUX_2_6_24||@LINUX_2_6_25||@LINUX_2_6_26||@LINUX_2_6_27 @USB_SUPPORT +kmod-usb-core +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/zd1211rw/zd1211rw.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,zd1211rw)
endef

define KernelPackage/ath5k
  SUBMENU:=Wireless Drivers
  TITLE:=Atheros 5xxx wireless cards support
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath5k/ath5k.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,ath5k)
endef

define KernelPackage/mac80211-hwsim
  SUBMENU:=Wireless Drivers
  TITLE:=mac80211 HW simulation device
  DEPENDS+= +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mac80211_hwsim.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,mac80211_hwsim)
endef

BUILDFLAGS:= \
	-I$(PKG_BUILD_DIR)/include \
	$(foreach opt,$(CONFOPTS),-DCONFIG_$(opt)) \
	$(if $(CONFIG_LEDS_TRIGGERS), -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS) \
	$(if $(CONFIG_DEBUG_FS), -DCONFIG_MAC80211_DEBUGFS) \
	-D__CONFIG_MAC80211_RC_DEFAULT=minstrel \

MAKE_OPTS:= \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="$(BUILDFLAGS)" \
	$(foreach opt,$(CONFOPTS),CONFIG_$(opt)=m) \
	CONFIG_NL80211=y \
	CONFIG_MAC80211_RC_PID=y \
	CONFIG_MAC80211_RC_MINSTREL=y \
	CONFIG_MAC80211_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	CONFIG_MAC80211_DEBUGFS=$(CONFIG_DEBUG_FS) \
	KLIB_BUILD="$(LINUX_DIR)" \
	MODPROBE=:

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(PKG_UNPACK)
	$(Build/Patch)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT61FW)
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT71FW)
	-unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT2860FW)
	$(TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(ZD1211FW_NAME)-$(ZD1211FW_VERSION).tar.bz2
	$(if $(CONFIG_TARGET_brcm47xx),rm -rf $(PKG_BUILD_DIR)/include/linux/ssb)
endef

ifneq ($(CONFIG_PACKAGE_kmod-mac80211),)
 define Build/Compile/kmod
	$(MAKE) -C "$(PKG_BUILD_DIR)" $(MAKE_OPTS) all
 endef
endif

define Build/Compile
	$(call Build/Compile/kmod)
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/mac80211 $(1)/usr/include/net/mac80211
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/*.h $(PKG_BUILD_DIR)/include/* $(1)/usr/include/mac80211/
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/rate.h $(1)/usr/include/net/mac80211/
endef

define KernelPackage/mac80211/install
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/lib/wifi/mac80211.sh $(1)/lib/wifi
endef

define KernelPackage/rt61-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rt2?61*.bin $(1)/lib/firmware/
endef

define KernelPackage/rt73-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rt73.bin $(1)/lib/firmware/
endef

define KernelPackage/rt2800-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rt2860.bin $(1)/lib/firmware/
endef

define KernelPackage/zd1211rw/install
	$(INSTALL_DIR) $(1)/lib/firmware/zd1211
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(ZD1211FW_NAME)/zd1211* $(1)/lib/firmware/zd1211
endef

$(eval $(call KernelPackage,ath5k))
$(eval $(call KernelPackage,mac80211))
$(eval $(call KernelPackage,rt2x00-lib))
$(eval $(call KernelPackage,rt2x00-pci))
$(eval $(call KernelPackage,rt2x00-usb))
$(eval $(call KernelPackage,rt2400-pci))
$(eval $(call KernelPackage,rt2500-pci))
$(eval $(call KernelPackage,rt2500-usb))
$(eval $(call KernelPackage,rt61-pci))
$(eval $(call KernelPackage,rt73-usb))
$(eval $(call KernelPackage,rt2800-pci))
$(eval $(call KernelPackage,zd1211rw))
$(eval $(call KernelPackage,mac80211-hwsim))
