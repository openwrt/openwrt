#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mac80211

PKG_VERSION:=2010-01-15
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
#	http://www.orbit-lab.org/kernel/compat-wireless-2.6/2010/11 \
#	http://wireless.kernel.org/download/compat-wireless-2.6
PKG_MD5SUM:=aa6f80ad3fcc2b663e62c7b80d37abd3

PKG_SOURCE:=compat-wireless-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/compat-wireless-$(PKG_VERSION)

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_kmod-mac80211 \
	CONFIG_PACKAGE_MAC80211_DEBUGFS \
	CONFIG_PACKAGE_ATH_DEBUG \
	CONFIG_ATH_USER_REGD \

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define KernelPackage/mac80211/Default
  SUBMENU:=$(WMENU)
  URL:=http://linuxwireless.org/
  DEPENDS:=@LINUX_2_6 @!TARGET_avr32 @(!(TARGET_ep93xx||TARGET_ps3||TARGET_pxcab)||BROKEN)
endef

define KernelPackage/mac80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS+= +kmod-crypto-arc4 +kmod-crypto-aes +wireless-tools +iw @!LINUX_2_6_21 @!LINUX_2_6_25
  FILES:= \
	$(PKG_BUILD_DIR)/compat/compat.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/compat/compat_firmware_class.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net/mac80211/mac80211.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/net/wireless/cfg80211.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,compat compat_firmware_class cfg80211 mac80211)
endef

define KernelPackage/mac80211/config
	source "$(SOURCE)/Config.in"
endef

define KernelPackage/mac80211/description
Linux 802.11 Wireless Networking Stack
endef

# Prism54 drivers
P54PCIFW:=2.13.12.0.arm
P54USBFW:=2.13.24.0.lm87.arm

define Download/p54usb
  FILE:=$(P54USBFW)
  URL:=http://daemonizer.de/prism54/prism54-fw/fw-usb
  MD5SUM:=8e8ab005a4f8f0123bcdc51bc25b47f6
endef
$(eval $(call Download,p54usb))

define Download/p54pci
  FILE:=$(P54PCIFW)
  URL:=http://daemonizer.de/prism54/prism54-fw/fw-softmac
  MD5SUM:=ff7536af2092b1c4b21315bd103ef4c4
endef
$(eval $(call Download,p54pci))

define KernelPackage/p54/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Prism54 Drivers
endef

define KernelPackage/p54/description
  Kernel module for Prism54 chipsets (mac80211)
endef

define KernelPackage/p54-common
  $(call KernelPackage/p54/Default)
  DEPENDS+= @PCI_SUPPORT||@USB_SUPPORT +kmod-mac80211
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
RT2870FW:=RT2870_Firmware_V8.zip

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

define Download/rt2870
  FILE:=$(RT2870FW)
  URL:=http://www.ralinktech.com.tw/data/drivers
  MD5SUM:=a7aae1d8cfd68e4d86a73000df0b6584
endef
$(eval $(call Download,rt2870))

AR9170FW:=ar9170.fw

define Download/ar9170
  FILE:=$(AR9170FW)
  URL:=http://www.kernel.org/pub/linux/kernel/people/mcgrof/firmware/ar9170
  MD5SUM:=34feec4ec0eae3bb92c7c1ea2dfb4530
endef
$(eval $(call Download,ar9170))

define KernelPackage/rt2x00/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Ralink Drivers for RT2x00 cards
endef

define KernelPackage/rt2x00-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @(PCI_SUPPORT||USB_SUPPORT) +kmod-mac80211 +kmod-crc-itu-t
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
  DEPENDS+= +kmod-rt2x00-pci +kmod-crc-ccitt
  TITLE+= (RT2860 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2800pci)
endef

define KernelPackage/rt2800-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= +kmod-rt2x00-usb +kmod-crc-ccitt
  TITLE+= (RT2870 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rt2800usb)
endef

define KernelPackage/rtl818x/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek Drivers for RTL818x devices
  URL:=http://wireless.kernel.org/en/users/Drivers/rtl8187
  DEPENDS+= +kmod-eeprom-93cx6 +kmod-mac80211
endef

define KernelPackage/rtl8180
  $(call KernelPackage/rtl818x/Default)
  DEPENDS+= @PCI_SUPPORT
  TITLE+= (RTL8180 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtl818x/rtl8180.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rtl8180)
endef

define KernelPackage/rtl8187
$(call KernelPackage/rtl818x/Default)
  DEPENDS+= @USB_SUPPORT
  TITLE+= (RTL8187 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtl818x/rtl8187.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,rtl8187)
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

define KernelPackage/ath/config

config ATH_USER_REGD
	bool "Force Atheros drivers to respect the user's regdomain settings"
	depends on PACKAGE_kmod-ath
	help
		Atheros' idea of regulatory handling is that the EEPROM of the card defines
		the regulatory limits and the user is only allowed to restrict the settings
		even further, even if the country allows frequencies or power levels that
		are forbidden by the EEPROM settings.
		Select this option if you want the driver to respect the user's decision about
		regulatory settings.

config PACKAGE_ATH_DEBUG
	bool "Atheros wireless debugging"
	depends on PACKAGE_kmod-ath
	help
		Say Y, if you want to debug atheros wireless drivers.
		Right now only ath9k makes use of this.
endef

define KernelPackage/ath
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros common driver part
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,26,ath)
endef

define KernelPackage/ath/description
 This module contains some common parts needed by Atheros Wireless drivers.
endef

define KernelPackage/ath5k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 5xxx wireless cards support
  URL:=http://linuxwireless.org/en/users/Drivers/ath5k
  DEPENDS+= +kmod-ath
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath5k/ath5k.$(LINUX_KMOD_SUFFIX)
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
  DEPENDS+= +kmod-ath
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k_common.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k_hw.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,ath9k_hw ath9k_common ath9k)
endef

define KernelPackage/ath9k/description
This module adds support for wireless adapters based on
Atheros IEEE 802.11n AR5008 and AR9001 family of chipsets.
endef

define KernelPackage/ath9k/config
	source "$(SOURCE)/Config.in.ath9k"
endef

USB8388FW_NAME:=usb8388
USB8388FW_VERSION:=5.110.22.p23

define Download/usb8388
  URL:=http://dev.laptop.org/pub/firmware/libertas/
  FILE:=$(USB8388FW_NAME)-$(USB8388FW_VERSION).bin
  MD5SUM=5e38f55719df3d0c58dd3bd02575a09c
endef
$(eval $(call Download,usb8388))

define KernelPackage/libertas
  $(call KernelPackage/mac80211/Default)
  DEPENDS+= @USB_SUPPORT +kmod-mac80211 +kmod-usb-core +kmod-lib80211
  TITLE:=Marvell 88W8015 Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/libertas/libertas.$(LINUX_KMOD_SUFFIX) \
	$(PKG_BUILD_DIR)/drivers/net/wireless/libertas/usb8xxx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,libertas usb8xxx)
endef


define KernelPackage/ar9170
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros AR9170 802.11n USB support
  URL:=http://wireless.kernel.org/en/users/Drivers/ar9170
  DEPENDS+= @USB_SUPPORT @!LINUX_2_6_25 +kmod-ath +kmod-usb-core
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ar9170/ar9170usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,27,ar9170usb)
endef

define KernelPackage/ar9170/description
 This is a driver for the Atheros "otus" 802.11n USB devices.
endef

define KernelPackage/mac80211-hwsim
  $(call KernelPackage/mac80211/Default)
  TITLE:=mac80211 HW simulation device
  DEPENDS+= +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mac80211_hwsim.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,mac80211_hwsim)
endef

#Broadcom firmware
ifneq ($(CONFIG_B43_EXPERIMENTAL),)
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=4.178.10.4
  PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/linux/wl.o
  PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2
  PKG_B43_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
  PKG_B43_FWV4_MD5SUM:=14477e8cbbb91b11896affac9b219fdb
else
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=4.150.10.5
  PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/driver/wl_apsta_mimo.o
  PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2
  PKG_B43_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
  PKG_B43_FWV4_MD5SUM:=0c6ba9687114c6b598e8019e262d9a60
endif
ifneq ($(CONFIG_B43_OPENFIRMWARE),)
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=5.2
  PKG_B43_FWV4_OBJECT:=openfwwf-$(PKG_B43_FWV4_VERSION)
  PKG_B43_FWV4_SOURCE:=openfwwf-$(PKG_B43_FWV4_VERSION).tar.gz
  PKG_B43_FWV4_SOURCE_URL:=http://www.ing.unibs.it/openfwwf/firmware/
  PKG_B43_FWV4_MD5SUM:=e045a135453274e439ae183f8498b0fa
endif


PKG_B43_FWV3_NAME:=wl_apsta
PKG_B43_FWV3_VERSION:=3.130.20.0
PKG_B43_FWV3_SOURCE:=$(PKG_B43_FWV3_NAME)-$(PKG_B43_FWV3_VERSION).o
PKG_B43_FWV3_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_B43_FWV3_MD5SUM:=e08665c5c5b66beb9c3b2dd54aa80cb3

ifneq ($(CONFIG_B43_OPENFIRMWARE),)
  PKG_B43_FWCUTTER_NAME:=b43-fwcutter
  PKG_B43_FWCUTTER_VERSION:=3e69531aa65b8f664a0ab00dfc3e2eefeb0cb417
  PKG_B43_FWCUTTER_SOURCE:=$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION).tar.bz2
  PKG_B43_FWCUTTER_PROTO:=git
  PKG_B43_FWCUTTER_SOURCE_URL:=http://git.bu3sch.de/git/b43-tools.git
  PKG_B43_FWCUTTER_SUBDIR:=$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION)
  PKG_B43_FWCUTTER_OBJECT:=$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION)/fwcutter/
else
  PKG_B43_FWCUTTER_NAME:=b43-fwcutter
  PKG_B43_FWCUTTER_VERSION:=012
  PKG_B43_FWCUTTER_SOURCE:=$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION).tar.bz2
  PKG_B43_FWCUTTER_PROTO:=default
  PKG_B43_FWCUTTER_SOURCE_URL:=http://bu3sch.de/b43/fwcutter/
  PKG_B43_FWCUTTER_MD5SUM:=69eadf67b459f313a8d6b37aaabef96c
  PKG_B43_FWCUTTER_SUBDIR:=b43-fwcutter-$(PKG_B43_FWCUTTER_VERSION)
  PKG_B43_FWCUTTER_OBJECT:=$(PKG_B43_FWCUTTER_NAME)-$(PKG_B43_FWCUTTER_VERSION)/
endif

define Download/b43-common
  FILE:=$(PKG_B43_FWCUTTER_SOURCE)
  URL:=$(PKG_B43_FWCUTTER_SOURCE_URL)
  MD5SUM:=$(PKG_B43_FWCUTTER_MD5SUM)
  PROTO:=$(PKG_B43_FWCUTTER_PROTO)
  VERSION:=$(PKG_B43_FWCUTTER_VERSION)
  SUBDIR:=$(PKG_B43_FWCUTTER_SUBDIR)
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
  KCONFIG:= \
  	CONFIG_HW_RANDOM=y
  DEPENDS+= +kmod-mac80211 +!(TARGET_brcm47xx||TARGET_brcm63xx):kmod-ssb
endef

define KernelPackage/b43
$(call KernelPackage/b43-common)
  TITLE:=Broadcom 43xx wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/b43/b43.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,b43)
endef

define KernelPackage/b43/config
	source "$(SOURCE)/Config.in.b43"
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

define KernelPackage/b43legacy/config
	source "$(SOURCE)/Config.in.b43legacy"
endef

define KernelPackage/b43legacy/description
Kernel module for Broadcom 43xx-legacy wireless support (mac80211 stack) new
endef

BUILDFLAGS:= \
	-I$(PKG_BUILD_DIR)/include \
	$(foreach opt,$(CONFOPTS),-DCONFIG_$(opt)) \
	$(if $(CONFIG_PCI),-DCONFIG_B43_PCI_AUTOSELECT -DCONFIG_B43_PCICORE_AUTOSELECT -DCONFIG_SSB_DRIVER_PCICORE_POSSIBLE -DCONFIG_SSB_DRIVER_PCICORE) \
	$(if $(CONFIG_PCI),-DCONFIG_SSB_PCIHOST_POSSIBLE -DCONFIG_SSB_PCIHOST) \
	$(if $(CONFIG_PCI),-DCONFIG_SSB_SPROM) \
	$(if $(CONFIG_LEDS_TRIGGERS), -DCONFIG_MAC80211_LEDS -DCONFIG_LEDS_TRIGGERS -DCONFIG_B43_LEDS -DCONFIG_B43LEGACY_LEDS -DCONFIG_AR9170_LEDS) \
	-DCONFIG_B43_HWRNG -DCONFIG_B43LEGACY_HWRNG \
	$(if $(CONFIG_PACKAGE_MAC80211_DEBUGFS),-DCONFIG_MAC80211_DEBUGFS -DCONFIG_ATH9K_DEBUGFS) \
	$(if $(CONFIG_PACKAGE_ATH_DEBUG),-DCONFIG_ATH_DEBUG) \
	-D__CONFIG_MAC80211_RC_DEFAULT=minstrel \
	$(if $(CONFIG_ATH_USER_REGD),-DATH_USER_REGD=1)

MAKE_OPTS:= \
	CROSS_COMPILE="$(KERNEL_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="$(BUILDFLAGS)" \
	$(foreach opt,$(CONFOPTS),CONFIG_$(opt)=m) \
	CONFIG_MAC80211_RC_PID=y \
	CONFIG_MAC80211_RC_MINSTREL=y \
	CONFIG_MAC80211_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	CONFIG_MAC80211_DEBUGFS=$(if $(CONFIG_PACKAGE_MAC80211_DEBUGFS),y) \
	CONFIG_B43_PCMCIA=n CONFIG_B43_PIO=n \
	CONFIG_B43_PCI_AUTOSELECT=$(if $(CONFIG_PCI),y) \
	CONFIG_B43_PCICORE_AUTOSELECT=$(if $(CONFIG_PCI),y) \
	CONFIG_SSB_DRIVER_PCICORE_POSSIBLE=$(if $(CONFIG_PCI),y) \
	CONFIG_SSB_DRIVER_PCICORE=$(if $(CONFIG_PCI),y) \
	CONFIG_SSB_PCIHOST_POSSIBLE=$(if $(CONFIG_PCI),y) \
	CONFIG_SSB_PCIHOST=$(if $(CONFIG_PCI),y) \
	CONFIG_B43LEGACY_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	CONFIG_B43_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	CONFIG_B43_HWRNG=$(if $(CONFIG_HW_RANDOM),y) \
	CONFIG_B43LEGACY_HWRNG=$(if $(CONFIG_HW_RANDOM),y) \
	CONFIG_B43=$(if $(CONFIG_PACKAGE_kmod-b43),m) \
	CONFIG_B43LEGACY=$(if $(CONFIG_PACKAGE_kmod-b43legacy),m) \
	CONFIG_ATH_COMMON=$(if $(CONFIG_PACKAGE_kmod-ath),m) \
	CONFIG_ATH_DEBUG=$(if $(CONFIG_PACKAGE_ATH_DEBUG),y) \
	CONFIG_ATH5K=$(if $(CONFIG_PACKAGE_kmod-ath5k),m) \
	CONFIG_ATH9K=$(if $(CONFIG_PACKAGE_kmod-ath9k),m) \
	CONFIG_ATH9K_DEBUGFS=$(if $(CONFIG_PACKAGE_MAC80211_DEBUGFS),y) \
	CONFIG_ZD1211RW=$(if $(CONFIG_PACKAGE_kmod-zd1211rw),m) \
	CONFIG_P54_COMMON=$(if $(CONFIG_PACKAGE_kmod-p54-common),m) \
	CONFIG_P54_PCI=$(if $(CONFIG_PACKAGE_kmod-p54-pci),m) \
	CONFIG_P54_USB=$(if $(CONFIG_PACKAGE_kmod-p54-usb),m) \
	CONFIG_P54_SPI= \
	CONFIG_RT2X00=$(if $(CONFIG_PACKAGE_kmod-rt2x00-lib),m) \
	CONFIG_RT2X00_LIB_PCI=$(if $(CONFIG_PACKAGE_kmod-rt2x00-pci),m) \
	CONFIG_RT2X00_LIB_USB=$(if $(CONFIG_PACKAGE_kmod-rt2x00-usb),m) \
	CONFIG_RT2400PCI=$(if $(CONFIG_PACKAGE_kmod-rt2400-pci),m) \
	CONFIG_RT2500PCI=$(if $(CONFIG_PACKAGE_kmod-rt2500-pci),m) \
	CONFIG_RT2500USB=$(if $(CONFIG_PACKAGE_kmod-rt2500-usb),m) \
	CONFIG_RT61PCI=$(if $(CONFIG_PACKAGE_kmod-rt61-pci),m) \
	CONFIG_RT73USB=$(if $(CONFIG_PACKAGE_kmod-rt73-usb),m) \
	CONFIG_RT2800PCI=$(if $(CONFIG_PACKAGE_kmod-rt2800-pci),m) \
	CONFIG_RT2800USB=$(if $(CONFIG_PACKAGE_kmod-rt2800-usb),m) \
	CONFIG_RTL8180=$(if $(CONFIG_PACKAGE_kmod-rtl8180),m) \
	CONFIG_RTL8187=$(if $(CONFIG_PACKAGE_kmod-rtl8187),m) \
	CONFIG_MAC80211_HWSIM=$(if $(CONFIG_PACKAGE_kmod-mac80211-hwsim),m) \
	CONFIG_PCMCIA= \
	CONFIG_LIBIPW= \
	CONFIG_LIBERTAS=$(if $(CONFIG_PACKAGE_kmod-libertas),m) \
	CONFIG_LIBERTAS_CS= \
	CONFIG_LIBERTAS_SDIO= \
	CONFIG_LIBERTAS_THINFIRM= \
	CONFIG_LIBERTAS_USB=$(if $(CONFIG_PACKAGE_kmod-libertas),m) \
	CONFIG_IPW2100= \
	CONFIG_IPW2200= \
	CONFIG_NL80211=y \
	CONFIG_LIB80211= \
	CONFIG_LIB80211_CRYPT_WEP= \
	CONFIG_LIB80211_CRYPT_CCMP= \
	CONFIG_LIB80211_CRYPT_TKIP= \
	CONFIG_IWLWIFI= \
	CONFIG_IWLAGN= \
	CONFIG_MWL8K= \
	CONFIG_ATMEL= \
	CONFIG_PCMCIA_ATMEL= \
	CONFIG_ADM8211= \
	CONFIG_USB_NET_RNDIS_HOST= \
	CONFIG_USB_NET_RNDIS_WLAN= \
	CONFIG_USB_NET_CDCETHER= \
	CONFIG_USB_USBNET= \
	CONFIG_AT76C50X_USB= \
	CONFIG_WL12XX= \
	CONFIG_EEPROM_93CX6= \
	CONFIG_AR9170_USB=$(if $(CONFIG_PACKAGE_kmod-ar9170),m) \
	CONFIG_AR9170_LEDS=$(CONFIG_LEDS_TRIGGERS) \
	CONFIG_IWM= \
	MADWIFI= \
	OLD_IWL= \
	KLIB_BUILD="$(LINUX_DIR)" \
	MODPROBE=: \
	KLIB=$(TARGET_MODULES_DIR) \
	KERNEL_SUBLEVEL=$(lastword $(subst ., ,$(KERNEL_PATCHVER)))

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(PKG_UNPACK)
	$(Build/Patch)
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT61FW)
	unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT71FW)
	-unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT2860FW)
	-unzip -jod $(PKG_BUILD_DIR) $(DL_DIR)/$(RT2870FW)
	$(TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(ZD1211FW_NAME)-$(ZD1211FW_VERSION).tar.bz2
	rm -rf $(PKG_BUILD_DIR)/include/linux/ssb
	rm -f $(PKG_BUILD_DIR)/include/net/ieee80211.h
	rm $(PKG_BUILD_DIR)/include/linux/eeprom_93cx6.h
endef

ifneq ($(CONFIG_PACKAGE_kmod-mac80211),)
 define Build/Compile/kmod
	rm -rf $(PKG_BUILD_DIR)/modules
	$(MAKE) -C "$(PKG_BUILD_DIR)" $(MAKE_OPTS) all
 endef
endif

define Build/Compile
	$(call Build/Compile/kmod)
endef

define Build/InstallDev
	mkdir -p \
		$(1)/usr/include/mac80211 \
		$(1)/usr/include/mac80211/ath \
		$(1)/usr/include/net/mac80211
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/*.h $(PKG_BUILD_DIR)/include/* $(1)/usr/include/mac80211/
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/rate.h $(1)/usr/include/net/mac80211/
	$(CP) $(PKG_BUILD_DIR)/drivers/net/wireless/ath/*.h $(1)/usr/include/mac80211/ath/
endef

define KernelPackage/libertas/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(USB8388FW_NAME)-$(USB8388FW_VERSION).bin $(1)/lib/firmware/$(USB8388FW_NAME).bin
endef

define KernelPackage/mac80211/install
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/lib/wifi/mac80211.sh $(1)/lib/wifi
endef

define KernelPackage/p54-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54PCIFW) $(1)/lib/firmware/isl3886pci
endef

define KernelPackage/p54-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54USBFW) $(1)/lib/firmware/isl3887usb
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

define KernelPackage/rt2800-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/rt2870.bin $(1)/lib/firmware/
endef

define KernelPackage/zd1211rw/install
	$(INSTALL_DIR) $(1)/lib/firmware/zd1211
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(ZD1211FW_NAME)/zd1211* $(1)/lib/firmware/zd1211
endef

define KernelPackage/ar9170/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(AR9170FW) $(1)/lib/firmware/
endef

define Build/b43-common
	tar xjf "$(DL_DIR)/$(PKG_B43_FWCUTTER_SOURCE)" -C "$(PKG_BUILD_DIR)"
	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_OBJECT)" \
		CFLAGS="-I$(STAGING_DIR_HOST)/include -include endian.h"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_OBJECT)/b43-fwcutter $(STAGING_DIR_HOST)/bin/
ifneq ($(CONFIG_B43_OPENFIRMWARE),)
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin/
	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_SUBDIR)/assembler/"
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_SUBDIR)/assembler/b43-asm $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_B43_FWCUTTER_SUBDIR)/assembler/b43-asm.bin $(STAGING_DIR_HOST)/bin/
endif
	$(INSTALL_BIN) ./files/host_bin/b43-fwsquash.py $(STAGING_DIR_HOST)/bin/
endef

define KernelPackage/b43/install
	rm -rf $(1)/lib/firmware/
	$(call Build/b43-common)
ifneq ($(CONFIG_B43_OPENFIRMWARE),)
	tar xzf "$(DL_DIR)/$(PKG_B43_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
else
	tar xjf "$(DL_DIR)/$(PKG_B43_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
endif
	$(INSTALL_DIR) $(1)/lib/firmware/
ifneq ($(CONFIG_B43_OPENFIRMWARE),)
	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/"
	$(INSTALL_DIR) $(1)/lib/firmware/b43-open/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/ucode5.fw $(1)/lib/firmware/b43-open/ucode5.fw
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/b0g0bsinitvals5.fw $(1)/lib/firmware/b43-open/b0g0bsinitvals5.fw
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/b0g0initvals5.fw $(1)/lib/firmware/b43-open/b0g0initvals5.fw
else
	b43-fwcutter --unsupported -w $(1)/lib/firmware/ $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)
endif
ifneq ($(CONFIG_B43_FW_SQUASH),)
	b43-fwsquash.py "$(CONFIG_B43_FW_SQUASH_PHYTYPES)" "$(CONFIG_B43_FW_SQUASH_COREREVS)" "$(1)/lib/firmware/b43"
endif
endef

define KernelPackage/b43legacy/install
	$(call Build/b43-common)
	$(INSTALL_DIR) $(1)/lib/firmware/

	b43-fwcutter --unsupported -w $(1)/lib/firmware/ $(DL_DIR)/$(PKG_B43_FWV3_SOURCE)
ifneq ($(CONFIG_B43LEGACY_FW_SQUASH),)
	b43-fwsquash.py "G" "$(CONFIG_B43LEGACY_FW_SQUASH_COREREVS)" "$(1)/lib/firmware/b43legacy"
endif
endef

$(eval $(call KernelPackage,ath5k))
$(eval $(call KernelPackage,libertas))
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
$(eval $(call KernelPackage,rtl8180))
$(eval $(call KernelPackage,rtl8187))
$(eval $(call KernelPackage,zd1211rw))
$(eval $(call KernelPackage,mac80211-hwsim))
$(eval $(call KernelPackage,ath9k))
$(eval $(call KernelPackage,ath))
$(eval $(call KernelPackage,ar9170))
$(eval $(call KernelPackage,b43))
$(eval $(call KernelPackage,b43legacy))
