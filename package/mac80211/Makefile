#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mac80211
PKG_RELEASE:=1

ifneq ($(CONFIG_LINUX_2_6_27)$(CONFIG_LINUX_2_6_28),)
  PKG_VERSION:=2009-01-23
  PKG_SOURCE_URL:= \
	http://www.orbit-lab.org/kernel/compat-wireless-2.6/2009/01 \
	http://wireless.kernel.org/download/compat-wireless-2.6
  PKG_MD5SUM:=f4c8df5e077432d7018112849e36f438
else
  PKG_VERSION:=2008-08-06
  PKG_SOURCE_URL:=http://www.orbit-lab.org/kernel/compat-wireless-2.6/2008/08
  PKG_MD5SUM:=9563ceeed86bca0859ad5f010623277c
  PATCH_DIR:=./patches-old
endif

PKG_SOURCE:=compat-wireless-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/compat-wireless-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
STAMP_CONFIGURED:=$(STAMP_CONFIGURED)_$(call confvar,CONFIG_PACKAGE_kmod-mac80211)

WMENU:=Wireless Drivers

define KernelPackage/mac80211/Default
  SUBMENU:=$(WMENU)
  URL:=http://linuxwireless.org/
  DEPENDS:=@LINUX_2_6 @!TARGET_avr32
endef

define KernelPackage/mac80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS+= +kmod-crypto-arc4 +kmod-crypto-aes +wireless-tools
  FILES:= \
	$(PKG_BUILD_DIR)/net/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,cfg80211 mac80211)
endef

define KernelPackage/mac80211/description
Linux 802.11 Wireless Networking Stack
endef

# Prism54 drivers
P54FW:=2.7.0.0.arm

define Download/p54
  FILE:=$(P54FW)
  URL:=http://prism54.org/firmware
  MD5SUM:=09f9da7ea757173c9de1a0322a1f9782
endef
$(eval $(call Download,p54))

define KernelPackage/p54/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Prism54 Drivers
endef

define KernelPackage/p54/description
  Kernel module for Prism54 chipsets (mac80211)
endef

define KernelPackage/p54-common
  $(call KernelPackage/p54/Default)
  DEPENDS+= @PCI_SUPPORT||@USB_SUPPORT
  TITLE+= (COMMON)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54common.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,p54common)
endef

define KernelPackage/p54-pci
  $(call KernelPackage/p54/Default)
  TITLE+= (PCI)
  DEPENDS+= @PCI_SUPPORT +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,31,p54pci)
endef

define KernelPackage/p54-usb
  $(call KernelPackage/p54/Default)
  TITLE+= (USB)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,31,p54usb)
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
  $(call KernelPackage/mac80211/Default)
  TITLE:=Ralink Drivers for RT2x00 cards
endef

define KernelPackage/rt2x00-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT||@USB_SUPPORT +kmod-mac80211 +kmod-crc-itu-t
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
  DEPENDS+= +kmod-rt2x00-pci @LINUX_2_6_27||@LINUX_2_6_28
  TITLE+= (RT2860 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2800pci)
endef

define KernelPackage/rt2800-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-rt2x00-usb @LINUX_2_6_27||@LINUX_2_6_28
  TITLE+= (RT2870 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2800usb)
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
  $(call KernelPackage/mac80211/Default)
  TITLE:=Zydas ZD1211 support
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/zd1211rw/zd1211rw.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,zd1211rw)
endef

define KernelPackage/ath5k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 5xxx wireless cards support
  URL:=http://linuxwireless.org/en/users/Drivers/ath5k
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath5k/ath5k.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,ath5k)
endef

define KernelPackage/ath5k/description
 This module adds support for wireless adapters based on
 Atheros 5xxx chipset.
endef

define KernelPackage/ath9k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 802.11n wireless cards support
  URL:=http://linuxwireless.org/en/users/Drivers/ath9k
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211 @LINUX_2_6_27||@LINUX_2_6_28
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath9k/ath9k.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,ath9k)
endef

define KernelPackage/ath9k/description
This module adds support for wireless adapters based on
Atheros IEEE 802.11n AR5008 and AR9001 family of chipsets.
endef

define KernelPackage/mac80211-hwsim
  $(call KernelPackage/mac80211/Default)
  TITLE:=mac80211 HW simulation device
  DEPENDS+= +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mac80211_hwsim.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,mac80211_hwsim)
endef

#Broadcom firmware
PKG_B43_FWV4_NAME:=broadcom-wl
PKG_B43_FWV4_VERSION:=4.150.10.5
PKG_B43_FWV4_OBJECT:=driver/wl_apsta_mimo.o
PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2
PKG_B43_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_B43_FWV4_MD5SUM:=0c6ba9687114c6b598e8019e262d9a60

PKG_B43_FWV3_NAME:=wl_apsta
PKG_B43_FWV3_VERSION:=3.130.20.0
PKG_B43_FWV3_SOURCE:=$(PKG_B43_FWV3_NAME)-$(PKG_B43_FWV3_VERSION).o
PKG_B43_FWV3_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_B43_FWV3_MD5SUM:=e08665c5c5b66beb9c3b2dd54aa80cb3

PKG_B43_FWCUTTER_NAME:=b43-fwcutter
PKG_B43_FWCUTTER_VERSION:=011
PKG_B43_FWCUTTER_SOURCE:=$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION).tar.bz2
PKG_B43_FWCUTTER_SOURCE_URL:=http://bu3sch.de/b43/fwcutter/
PKG_B43_FWCUTTER_MD5SUM:=3db2f4de85a459451f5b391cf67a8d44

define Download/b43-common
  FILE:=$(PKG_B43_FWCUTTER_SOURCE)
  URL:=$(PKG_B43_FWCUTTER_SOURCE_URL)
  MD5SUM:=$(PKG_B43_FWCUTTER_MD5SUM)
endef
$(eval $(call Download,b43-common))

define Download/b43
  FILE:=$(PKG_B43_FWV4_SOURCE)
  URL:=$(PKG_B43_FWV4_SOURCE_URL)
  MD5SUM:=$(PKG_B43_FWV4_MD5SUM)
endef
$(eval $(call Download,b43))

define Download/b43legacy
  FILE:=$(PKG_B43_FWV3_SOURCE)
  URL:=$(PKG_B43_FWV3_SOURCE_URL)
  MD5SUM:=$(PKG_B43_FWV3_MD5SUM)
endef
$(eval $(call Download,b43legacy))

define KernelPackage/b43-common
  $(call KernelPackage/mac80211/Default)
  TITLE:=Generic stuff for Broadcom wireless devices
  URL:=http://linuxwireless.org/en/users/Drivers/b43
  DEPENDS+= +kmod-mac80211 +!TARGET_brcm47xx:kmod-ssb @TARGET_brcm47xx||@TARGET_brcm63xx @LINUX_2_6_25||@LINUX_2_6_26||@LINUX_2_6_27||@LINUX_2_6_28
endef

define KernelPackage/b43
$(call KernelPackage/b43-common)
  TITLE:=Broadcom 43xx wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/b43/b43.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,b43)
endef

define KernelPackage/b43/description
Kernel module for Broadcom 43xx wireless support (mac80211 stack) new
endef

define KernelPackage/b43legacy
$(call KernelPackage/b43-common)
  TITLE:=Broadcom 43xx-legacy wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/b43legacy/b43legacy.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,b43legacy)
endef

define KernelPackage/b43legacy/description
Kernel module for Broadcom 43xx-legacy wireless support (mac80211 stack) new
endef

BUILDFLAGS:= \
	-I$(PKG_BUILD_DIR)/include \
	$(foreach opt,$(CONFOPTS),-DCONFIG_$(opt)) \
	$(if $(CONFIG_LEDS_TRIGGERS), -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS -DCONFIG_B43_LEDS -DCONFIG_B43LEGACY_LEDS) \
	$(if $(CONFIG_RFKILL),-DCONFIG_B43_RFKILL -DCONFIG_B43LEGACY_RFKILL) \
	$(if $(CONFIG_PCMCIA),-DCONFIG_B43_PCMCIA) \
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
	CONFIG_B43_PCMCIA=$(CONFIG_PCMCIA) \
	CONFIG_B43_RFKILL=$(CONFIG_RFKILL) \
	CONFIG_B43LEGACY_RFKILL=$(CONFIG_RFKILL) \
	CONFIG_B43LEGACY_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	CONFIG_B43_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	$(if $(CONFIG_PACKAGE_kmod-b43),CONFIG_B43=m) \
	$(if $(CONFIG_PACKAGE_kmod-b43legacy),CONFIG_B43LEGACY=m) \
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
	rm -rf $(PKG_BUILD_DIR)/include/linux/ssb
	rm $(PKG_BUILD_DIR)/include/net/ieee80211.h
	rm $(PKG_BUILD_DIR)/include/net/ieee80211_radiotap.h
	rm $(PKG_BUILD_DIR)/include/linux/eeprom_93cx6.h
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

define KernelPackage/p54-common/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54FW) $(1)/lib/firmware/isl3886
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

define Build/b43-common
	tar xjf "$(DL_DIR)/$(PKG_B43_FWCUTTER_SOURCE)" -C "$(PKG_BUILD_DIR)"
	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION)" \
		CFLAGS="-I$(STAGING_DIR_HOST)/include -include endian.h"
endef

define KernelPackage/b43/install
	$(call Build/b43-common)
	tar xjf "$(DL_DIR)/$(PKG_B43_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
	$(INSTALL_DIR) $(1)/lib/firmware/

	$(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION)/b43-fwcutter \
	-w $(1)/lib/firmware/ \
	$(PKG_BUILD_DIR)/$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/$(PKG_B43_FWV4_OBJECT)
endef

define KernelPackage/b43legacy/install
	$(call Build/b43-common)
	$(INSTALL_DIR) $(1)/lib/firmware/

	$(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION)/b43-fwcutter \
	-w $(1)/lib/firmware/ \
	$(DL_DIR)/$(PKG_B43_FWV3_SOURCE)
endef

$(eval $(call KernelPackage,ath5k))
$(eval $(call KernelPackage,mac80211))
$(eval $(call KernelPackage,p54-common))
$(eval $(call KernelPackage,p54-pci))
$(eval $(call KernelPackage,p54-usb))
$(eval $(call KernelPackage,rt2x00-lib))
$(eval $(call KernelPackage,rt2x00-pci))
$(eval $(call KernelPackage,rt2x00-usb))
$(eval $(call KernelPackage,rt2400-pci))
$(eval $(call KernelPackage,rt2500-pci))
$(eval $(call KernelPackage,rt2500-usb))
$(eval $(call KernelPackage,rt61-pci))
$(eval $(call KernelPackage,rt73-usb))
$(eval $(call KernelPackage,rt2800-pci))
$(eval $(call KernelPackage,rt2800-usb))
$(eval $(call KernelPackage,zd1211rw))
$(eval $(call KernelPackage,mac80211-hwsim))
$(eval $(call KernelPackage,ath9k))
$(eval $(call KernelPackage,b43))
$(eval $(call KernelPackage,b43legacy))
