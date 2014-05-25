#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mac80211

PKG_VERSION:=2014-05-22
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_BACKPORT_VERSION:=
PKG_MD5SUM:=367937d4f8c05cb36ca989ee26abc3df

PKG_SOURCE:=compat-wireless-$(PKG_VERSION)$(PKG_BACKPORT_VERSION).tar.bz2
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/compat-wireless-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_DRIVERS = \
	adm8211 ath5k libertas-usb libertas-sd p54-common p54-pci p54-usb p54-spi \
	rt2x00-lib rt2x00-pci rt2x00-usb rt2800-lib rt2400-pci rt2500-pci \
	rt2500-usb rt61-pci rt73-usb rt2800-mmio rt2800-pci rt2800-usb rt2800-soc \
	rtl8180 rtl8187 zd1211rw mac80211-hwsim carl9170 b43 b43legacy \
	ath9k-common ath9k ath9k-htc ath10k ath net-libipw net-ipw2100 net-ipw2200 \
	mwl8k net-hermes net-hermes-pci net-hermes-plx net-hermes-pcmcia \
	iwl-legacy iwl3945 iwl4965 iwlagn wlcore wl12xx wl18xx lib80211 \
	rtlwifi rtlwifi-pci rtlwifi-usb rtl8192c-common rtl8192ce rtl8192se \
	rtl8192de rtl8192cu

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_kmod-mac80211 \
	$(patsubst %,CONFIG_PACKAGE_kmod-%,$(PKG_DRIVERS)) \
	CONFIG_PACKAGE_MAC80211_DEBUGFS \
	CONFIG_PACKAGE_MAC80211_MESH \
	CONFIG_PACKAGE_ATH_DEBUG \
	CONFIG_PACKAGE_ATH_DFS \
	CONFIG_PACKAGE_B43_DEBUG \
	CONFIG_PACKAGE_B43_PIO \
	CONFIG_PACKAGE_B43_N_PHY \
	CONFIG_PACKAGE_RTLWIFI_DEBUG \
	CONFIG_ATH_USER_REGD \

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define KernelPackage/mac80211/Default
  SUBMENU:=$(WMENU)
  URL:=http://linuxwireless.org/
  MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
  DEPENDS:=@(!(TARGET_avr32||TARGET_ps3||TARGET_pxcab)||BROKEN)
endef

define KernelPackage/cfg80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=cfg80211 - wireless configuration API
  DEPENDS+= +iw
  FILES:= \
	$(PKG_BUILD_DIR)/compat/compat.ko \
	$(PKG_BUILD_DIR)/net/wireless/cfg80211.ko
endef

define KernelPackage/cfg80211/description
cfg80211 is the Linux wireless LAN (802.11) configuration API.
endef

define KernelPackage/mac80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=Linux 802.11 Wireless Networking Stack
  DEPENDS+= +kmod-crypto-core +kmod-crypto-arc4 +kmod-crypto-aes +kmod-cfg80211 +hostapd-common
  KCONFIG:=\
	CONFIG_AVERAGE=y
  FILES:= $(PKG_BUILD_DIR)/net/mac80211/mac80211.ko
  MENU:=1
endef

define KernelPackage/mac80211/config
  if PACKAGE_kmod-mac80211

	config PACKAGE_MAC80211_DEBUGFS
		bool "Export mac80211 internals in DebugFS"
		select KERNEL_DEBUG_FS
		select KERNEL_RELAY if PACKAGE_kmod-ath9k-common
		default y
		help
		  Select this to see extensive information about
		  the internal state of mac80211 in debugfs.

	config PACKAGE_MAC80211_MESH
		bool "Enable 802.11s mesh support"
		default y

  endif
endef

define KernelPackage/mac80211/description
Generic IEEE 802.11 Networking Stack (mac80211)
endef

PKG_LINUX_FIRMWARE_NAME:=linux-firmware
PKG_LINUX_FIRMWARE_VERSION:=f8c22c692bdee57a20b092e647464ff6176df3ed
PKG_LINUX_FIRMWARE_SOURCE:=$(PKG_LINUX_FIRMWARE_NAME)-$(PKG_LINUX_FIRMWARE_VERSION).tar.bz2
PKG_LINUX_FIRMWARE_PROTO:=git
PKG_LINUX_FIRMWARE_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
PKG_LINUX_FIRMWARE_SUBDIR:=$(PKG_LINUX_FIRMWARE_NAME)-$(PKG_LINUX_FIRMWARE_VERSION)
PKG_LINUX_FIRMWARE_MIRROR_MD5SUM:=e219333f01835c6e556875a9e0deb3f9

define Download/linux-firmware
  FILE:=$(PKG_LINUX_FIRMWARE_SOURCE)
  URL:=$(PKG_LINUX_FIRMWARE_SOURCE_URL)
  MD5SUM:=$(PKG_LINUX_FIRMWARE_MD5SUM)
  PROTO:=$(PKG_LINUX_FIRMWARE_PROTO)
  VERSION:=$(PKG_LINUX_FIRMWARE_VERSION)
  SUBDIR:=$(PKG_LINUX_FIRMWARE_SUBDIR)
  MIRROR_MD5SUM:=$(PKG_LINUX_FIRMWARE_MIRROR_MD5SUM)
endef
$(eval $(call Download,linux-firmware))

PKG_ATH10K_LINUX_FIRMWARE_NAME:=ath10k-firmware
PKG_ATH10K_LINUX_FIRMWARE_VERSION:=38eeda3ae6f90fde5546bdd48ee4ff3090f238c0
PKG_ATH10K_LINUX_FIRMWARE_SOURCE:=$(PKG_ATH10K_LINUX_FIRMWARE_NAME)-$(PKG_ATH10K_LINUX_FIRMWARE_VERSION).tar.bz2
PKG_ATH10K_LINUX_FIRMWARE_PROTO:=git
PKG_ATH10K_LINUX_FIRMWARE_SOURCE_URL:=https://github.com/kvalo/ath10k-firmware.git
PKG_ATH10K_LINUX_FIRMWARE_SUBDIR:=$(PKG_ATH10K_LINUX_FIRMWARE_NAME)-$(PKG_ATH10K_LINUX_FIRMWARE_VERSION)
#PKG_ATH10K_LINUX_FIRMWARE_MIRROR_MD5SUM:=?

define Download/ath10k-firmware
  FILE:=$(PKG_ATH10K_LINUX_FIRMWARE_SOURCE)
  URL:=$(PKG_ATH10K_LINUX_FIRMWARE_SOURCE_URL)
  PROTO:=$(PKG_ATH10K_LINUX_FIRMWARE_PROTO)
  VERSION:=$(PKG_ATH10K_LINUX_FIRMWARE_VERSION)
  SUBDIR:=$(PKG_ATH10K_LINUX_FIRMWARE_SUBDIR)
  #MIRROR_MD5SUM:=$(PKG_ATH10K_LINUX_FIRMWARE_MIRROR_MD5SUM)
endef
$(eval $(call Download,ath10k-firmware))

# Prism54 drivers
P54PCIFW:=2.13.12.0.arm
P54USBFW:=2.13.24.0.lm87.arm
P54SPIFW:=2.13.0.0.a.13.14.arm

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

define Download/p54spi
  FILE:=$(P54SPIFW)
  URL:=http://daemonizer.de/prism54/prism54-fw/stlc4560
  MD5SUM:=42661f8ecbadd88012807493f596081d
endef
$(eval $(call Download,p54spi))

define KernelPackage/p54/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Prism54 Drivers
endef

define KernelPackage/p54/description
  Kernel module for Prism54 chipsets (mac80211)
endef

define KernelPackage/p54-common
  $(call KernelPackage/p54/Default)
  DEPENDS+= @PCI_SUPPORT||@USB_SUPPORT||@TARGET_omap24xx +kmod-mac80211 +kmod-lib-crc-ccitt
  TITLE+= (COMMON)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54common.ko
endef

define KernelPackage/p54-pci
  $(call KernelPackage/p54/Default)
  TITLE+= (PCI)
  DEPENDS+= @PCI_SUPPORT +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54pci.ko
  AUTOLOAD:=$(call AutoProbe,p54pci)
endef

define KernelPackage/p54-usb
  $(call KernelPackage/p54/Default)
  TITLE+= (USB)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54usb.ko
  AUTOLOAD:=$(call AutoProbe,p54usb)
endef

define KernelPackage/p54-spi
  $(call KernelPackage/p54/Default)
  TITLE+= (SPI)
  DEPENDS+= @TARGET_omap24xx +kmod-p54-common
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/p54/p54spi.ko
  AUTOLOAD:=$(call AutoProbe,p54spi)
endef

define KernelPackage/rt2x00/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Ralink Drivers for RT2x00 cards
endef

define KernelPackage/rt2x00-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @(PCI_SUPPORT||USB_SUPPORT||TARGET_ramips) +kmod-mac80211 +kmod-lib-crc-itu-t
  TITLE+= (LIB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00lib.ko
  MENU:=1
endef

define KernelPackage/rt2x00-lib/config
  if PACKAGE_kmod-rt2x00-lib

	config PACKAGE_RT2X00_LIB_DEBUGFS
		bool "Enable rt2x00 debugfs support"
		depends on PACKAGE_MAC80211_DEBUGFS
		help
		  Enable creation of debugfs files for the rt2x00 drivers.
		  These debugfs files support both reading and writing of the
		  most important register types of the rt2x00 hardware.

	config PACKAGE_RT2X00_DEBUG
		bool "Enable rt2x00 debug output"
		help
		  Enable debugging output for all rt2x00 modules

  endif
endef

define KernelPackage/rt2x00-mmio
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @(PCI_SUPPORT||TARGET_ramips) +kmod-rt2x00-lib +kmod-eeprom-93cx6
  HIDDEN:=1
  TITLE+= (MMIO)
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00mmio.ko
endef

define KernelPackage/rt2x00-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-mmio +kmod-rt2x00-lib
  HIDDEN:=1
  TITLE+= (PCI)
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00pci.ko
  AUTOLOAD:=$(call AutoProbe,rt2x00pci)
endef

define KernelPackage/rt2x00-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-lib +kmod-usb-core
  HIDDEN:=1
  TITLE+= (USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00usb.ko
  AUTOLOAD:=$(call AutoProbe,rt2x00usb)
endef

define KernelPackage/rt2800-lib
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @(PCI_SUPPORT||USB_SUPPORT||TARGET_ramips) +kmod-rt2x00-lib +kmod-lib-crc-ccitt +@DRIVER_11N_SUPPORT
  HIDDEN:=1
  TITLE+= (rt2800 LIB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800lib.ko
endef

define KernelPackage/rt2400-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2400 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2400pci.ko
  AUTOLOAD:=$(call AutoProbe,rt2400pci)
endef

define KernelPackage/rt2500-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2500 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2500pci.ko
  AUTOLOAD:=$(call AutoProbe,rt2500pci)
endef

define KernelPackage/rt2500-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb
  TITLE+= (RT2500 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2500usb.ko
  AUTOLOAD:=$(call AutoProbe,rt2500usb)
endef

define KernelPackage/rt61-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci
  TITLE+= (RT2x61 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt61pci.ko
  AUTOLOAD:=$(call AutoProbe,rt61pci)
endef

define KernelPackage/rt73-usb
  $(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb
  TITLE+= (RT73 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt73usb.ko
  AUTOLOAD:=$(call AutoProbe,rt73usb)
endef

define KernelPackage/rt2800-mmio
$(call KernelPackage/rt2x00/Default)
  TITLE += (RT28xx/RT3xxx MMIO)
  DEPENDS += +kmod-rt2800-lib +kmod-rt2x00-mmio
  HIDDEN:=1
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800mmio.ko
endef

define KernelPackage/rt2800-soc
$(call KernelPackage/rt2x00/Default)
  DEPENDS += @(TARGET_ramips_rt288x||TARGET_ramips_rt305x||TARGET_ramips_rt3883) +kmod-rt2800-mmio +kmod-rt2800-lib
  TITLE += (RT28xx/RT3xxx SoC)
  FILES := \
	$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2x00soc.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800soc.ko
  AUTOLOAD:=$(call AutoProbe,rt2800soc)
endef

define KernelPackage/rt2800-pci
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @PCI_SUPPORT +kmod-rt2x00-pci +kmod-rt2800-lib +kmod-rt2800-mmio
  TITLE+= (RT2860 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800pci.ko
  AUTOLOAD:=$(call AutoProbe,rt2800pci)
endef

define KernelPackage/rt2800-usb
$(call KernelPackage/rt2x00/Default)
  DEPENDS+= @USB_SUPPORT +kmod-rt2x00-usb +kmod-rt2800-lib +kmod-lib-crc-ccitt
  TITLE+= (RT2870 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rt2x00/rt2800usb.ko
  AUTOLOAD:=$(call AutoProbe,rt2800usb)
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
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtl818x/rtl8180/rtl818x_pci.ko
  AUTOLOAD:=$(call AutoProbe,rtl818x_pci)
endef

define KernelPackage/rtl8187
$(call KernelPackage/rtl818x/Default)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core
  TITLE+= (RTL8187 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtl818x/rtl8187/rtl8187.ko
  AUTOLOAD:=$(call AutoProbe,rtl8187)
endef

define KernelPackage/rtlwifi/config
	config PACKAGE_RTLWIFI_DEBUG
		bool "Realtek wireless debugging"
		depends on PACKAGE_kmod-rtlwifi
		help
		  Say Y, if you want to debug realtek wireless drivers.

endef

define KernelPackage/rtlwifi
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek common driver part
  DEPENDS+= @(PCI_SUPPORT||USB_SUPPORT) +kmod-mac80211 +@DRIVER_11N_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtlwifi.ko
  HIDDEN:=1
endef

define KernelPackage/rtlwifi-pci
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek common driver part (PCI support)
  DEPENDS+= @PCI_SUPPORT +kmod-rtlwifi
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl_pci.ko
  AUTOLOAD:=$(call AutoProbe,rtl_pci)
  HIDDEN:=1
endef

define KernelPackage/rtlwifi-usb
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek common driver part (USB support)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-rtlwifi
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl_usb.ko
  AUTOLOAD:=$(call AutoProbe,rtl_usb)
  HIDDEN:=1
endef

define KernelPackage/rtl8192c-common
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192CE/RTL8192CU common support module
  DEPENDS+= +kmod-rtlwifi
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl8192c/rtl8192c-common.ko
  HIDDEN:=1
endef

define KernelPackage/rtl8192ce
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192CE/RTL8188CE support
  DEPENDS+= +kmod-rtlwifi-pci +kmod-rtl8192c-common
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl8192ce/rtl8192ce.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192ce)
endef

define KernelPackage/rtl8192ce/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rtlwifi/rtl8192cfw.bin $(1)/lib/firmware/rtlwifi
endef

define KernelPackage/rtl8192se
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192SE/RTL8191SE support
  DEPENDS+= +kmod-rtlwifi-pci
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl8192se/rtl8192se.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192se)
endef

define KernelPackage/rtl8192se/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rtlwifi/rtl8192sefw.bin $(1)/lib/firmware/rtlwifi
endef

define KernelPackage/rtl8192de
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192DE/RTL8188DE support
  DEPENDS+= +kmod-rtlwifi-pci
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl8192de/rtl8192de.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192de)
endef

define KernelPackage/rtl8192de/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rtlwifi/rtl8192defw.bin $(1)/lib/firmware/rtlwifi
endef

define KernelPackage/rtl8192cu
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192CU/RTL8188CU support
  DEPENDS+= +kmod-rtlwifi-usb +kmod-rtl8192c-common
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/rtlwifi/rtl8192cu/rtl8192cu.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192cu)
endef

define KernelPackage/rtl8192cu/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rtlwifi/rtl8192cufw.bin $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rtlwifi/rtl8192cufw_TMSC.bin $(1)/lib/firmware/rtlwifi
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
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/zd1211rw/zd1211rw.ko
  AUTOLOAD:=$(call AutoProbe,zd1211rw)
endef

define KernelPackage/adm8211
  $(call KernelPackage/mac80211/Default)
  TITLE:=ADMTek 8211 support
  DEPENDS+=@PCI_SUPPORT +kmod-mac80211 +kmod-eeprom-93cx6
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/adm8211.ko
  AUTOLOAD:=$(call AutoProbe,adm8211)
endef

define KernelPackage/ath/config
  if PACKAGE_kmod-ath
	config ATH_USER_REGD
		bool "Force Atheros drivers to respect the user's regdomain settings"
		help
		  Atheros' idea of regulatory handling is that the EEPROM of the card defines
		  the regulatory limits and the user is only allowed to restrict the settings
		  even further, even if the country allows frequencies or power levels that
		  are forbidden by the EEPROM settings.

		  Select this option if you want the driver to respect the user's decision about
		  regulatory settings.

	config PACKAGE_ATH_DEBUG
		bool "Atheros wireless debugging"
		help
		  Say Y, if you want to debug atheros wireless drivers.
		  Right now only ath9k makes use of this.

	config PACKAGE_ATH_DFS
		bool "Enable DFS support"
		help
		  Dynamic frequency selection (DFS) is required for most of the 5 GHz band
		  channels in Europe, US, and Japan.

		  Select this option if you want to use such channels. Only EU (ETSI) DFS
		  is supported at the moment.

  endif
endef

define KernelPackage/ath
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros common driver part
  DEPENDS+= @PCI_SUPPORT||USB_SUPPORT||TARGET_ar71xx||TARGET_atheros +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath.ko
  MENU:=1
endef

define KernelPackage/ath/description
 This module contains some common parts needed by Atheros Wireless drivers.
endef

define KernelPackage/ath5k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 5xxx wireless cards support
  URL:=http://linuxwireless.org/en/users/Drivers/ath5k
  DEPENDS+= @PCI_SUPPORT||@TARGET_atheros +kmod-ath
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath5k/ath5k.ko
  AUTOLOAD:=$(call AutoProbe,ath5k)
endef

define KernelPackage/ath5k/description
 This module adds support for wireless adapters based on
 Atheros 5xxx chipset.
endef

define KernelPackage/ath9k-common
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 802.11n wireless devices (common code for ath9k and ath9k_htc)
  URL:=http://linuxwireless.org/en/users/Drivers/ath9k
  DEPENDS+= @PCI_SUPPORT||USB_SUPPORT||TARGET_ar71xx +kmod-ath +@DRIVER_11N_SUPPORT
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k_common.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k_hw.ko
endef

define KernelPackage/ath9k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 802.11n PCI wireless cards support
  URL:=http://linuxwireless.org/en/users/Drivers/ath9k
  DEPENDS+= @PCI_SUPPORT||TARGET_ar71xx +kmod-ath9k-common
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k.ko
  AUTOLOAD:=$(call AutoProbe,ath9k)
endef

define KernelPackage/ath9k/description
This module adds support for wireless adapters based on
Atheros IEEE 802.11n AR5008 and AR9001 family of chipsets.
endef

define KernelPackage/ath9k/config

	config ATH9K_SUPPORT_PCOEM
		bool "Support chips used in PC OEM cards"
		depends PACKAGE_kmod-ath9k
	
endef

define KernelPackage/ath9k-htc
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 802.11n USB device support
  URL:=http://linuxwireless.org/en/users/Drivers/ath9k
  DEPENDS+= @USB_SUPPORT +kmod-ath9k-common +kmod-usb-core
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath9k/ath9k_htc.ko
  AUTOLOAD:=$(call AutoProbe,ath9k_htc)
endef

define KernelPackage/ath9k-htc/description
This module adds support for wireless adapters based on
Atheros USB AR9271 and AR7010 family of chipsets.
endef

define KernelPackage/ath10k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Atheros 802.11ac wireless cards support
  URL:=http://wireless.kernel.org/en/users/Drivers/ath10k
  DEPENDS+= @PCI_SUPPORT +kmod-ath +@DRIVER_11N_SUPPORT
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath10k/ath10k_core.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ath/ath10k/ath10k_pci.ko
  AUTOLOAD:=$(call AutoLoad,55,ath10k_core ath10k_pci)
endef

define KernelPackage/ath10k/description
This module adds support for wireless adapters based on
Atheros IEEE 802.11ac family of chipsets. For now only
PCI is supported.
endef

define KernelPackage/ath10k/config
  if PACKAGE_kmod-ath10k

	config ATH10K_STA_FW
		bool "Firmware optimized for STA operation"
		default n
		help
		  Use the ath10k firmware optimized for wireless client instead
		  of access point operation.

  endif
endef

define KernelPackage/carl9170
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for Atheros AR9170 USB sticks
  DEPENDS:=@USB_SUPPORT +kmod-mac80211 +kmod-ath +kmod-usb-core +kmod-input-core +@DRIVER_11N_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ath/carl9170/carl9170.ko
  AUTOLOAD:=$(call AutoProbe,carl9170)
endef

define KernelPackage/lib80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=802.11 Networking stack
  DEPENDS:=+kmod-cfg80211
  FILES:= \
	$(PKG_BUILD_DIR)/net/wireless/lib80211.ko \
	$(PKG_BUILD_DIR)/net/wireless/lib80211_crypt_wep.ko \
	$(PKG_BUILD_DIR)/net/wireless/lib80211_crypt_ccmp.ko \
	$(PKG_BUILD_DIR)/net/wireless/lib80211_crypt_tkip.ko
  AUTOLOAD:=$(call AutoProbe, \
	lib80211 \
	lib80211_crypt_wep \
	lib80211_crypt_ccmp \
	lib80211_crypt_tkip \
  )
endef

define KernelPackage/lib80211/description
 Kernel modules for 802.11 Networking stack
 Includes:
 - lib80211
 - lib80211_crypt_wep
 - lib80211_crypt_tkip
 - lib80211_crytp_ccmp
endef

define KernelPackage/libertas-usb
  $(call KernelPackage/mac80211/Default)
  DEPENDS+= @USB_SUPPORT +kmod-cfg80211 +kmod-usb-core +kmod-lib80211 +@DRIVER_WEXT_SUPPORT
  TITLE:=Marvell 88W8015 Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/libertas/libertas.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/libertas/usb8xxx.ko
  AUTOLOAD:=$(call AutoProbe,libertas usb8xxx)
endef

define KernelPackage/libertas-sd
  $(call KernelPackage/mac80211/Default)
  DEPENDS+= +kmod-cfg80211 +kmod-lib80211 +kmod-mmc +@DRIVER_WEXT_SUPPORT @!TARGET_uml
  TITLE:=Marvell 88W8686 Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/libertas/libertas.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/libertas/libertas_sdio.ko
  AUTOLOAD:=$(call AutoProbe,libertas libertas_sdio)
endef

define KernelPackage/mac80211-hwsim
  $(call KernelPackage/mac80211/Default)
  TITLE:=mac80211 HW simulation device
  DEPENDS+= +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mac80211_hwsim.ko
  AUTOLOAD:=$(call AutoProbe,mac80211_hwsim)
endef

define KernelPackage/net-libipw
  $(call KernelPackage/mac80211/Default)
  TITLE:=libipw for ipw2100 and ipw2200
  DEPENDS:=@PCI_SUPPORT +kmod-crypto-core +kmod-crypto-arc4 +kmod-crypto-aes +kmod-crypto-michael-mic +kmod-lib80211 +kmod-cfg80211 +@DRIVER_WEXT_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ipw2x00/libipw.ko
  AUTOLOAD:=$(call AutoProbe,libipw)
endef

define KernelPackage/net-libipw/description
 Hardware independent IEEE 802.11 networking stack for ipw2100 and ipw2200.
endef

IPW2100_NAME:=ipw2100-fw
IPW2100_VERSION:=1.3

define Download/net-ipw2100
  URL:=http://bughost.org/firmware/
  FILE:=$(IPW2100_NAME)-$(IPW2100_VERSION).tgz
  MD5SUM=46aa75bcda1a00efa841f9707bbbd113
endef
$(eval $(call Download,net-ipw2100))

define KernelPackage/net-ipw2100
  $(call KernelPackage/mac80211/Default)
  TITLE:=Intel IPW2100 driver
  DEPENDS:=@PCI_SUPPORT +kmod-net-libipw
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ipw2x00/ipw2100.ko
  AUTOLOAD:=$(call AutoProbe,ipw2100)
endef

define KernelPackage/net-ipw2100/description
 Kernel support for Intel IPW2100
 Includes:
 - ipw2100
endef

IPW2200_NAME:=ipw2200-fw
IPW2200_VERSION:=3.1

define Download/net-ipw2200
  URL:=http://bughost.org/firmware/
  FILE:=$(IPW2200_NAME)-$(IPW2200_VERSION).tgz
  MD5SUM=eaba788643c7cc7483dd67ace70f6e99
endef
$(eval $(call Download,net-ipw2200))

define KernelPackage/net-ipw2200
  $(call KernelPackage/mac80211/Default)
  TITLE:=Intel IPW2200 driver
  DEPENDS:=@PCI_SUPPORT +kmod-net-libipw
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ipw2x00/ipw2200.ko
  AUTOLOAD:=$(call AutoProbe,ipw2200)
endef

define KernelPackage/net-ipw2200/description
 Kernel support for Intel IPW2200
 Includes:
 - ipw2200
endef


define KernelPackage/net-hermes
  $(call KernelPackage/mac80211/Default)
  TITLE:=Hermes 802.11b chipset support
  DEPENDS:=@PCI_SUPPORT||PCMCIA_SUPPORT +kmod-cfg80211 +@DRIVER_WEXT_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/orinoco/orinoco.ko
  AUTOLOAD:=$(call AutoProbe,orinoco)
endef

define KernelPackage/net-hermes/description
 Kernel support for Hermes 802.11b chipsets
endef

define KernelPackage/net-hermes-pci
  $(call KernelPackage/mac80211/Default)
  TITLE:=Intersil Prism 2.5 PCI support
  DEPENDS:=@PCI_SUPPORT +kmod-net-hermes
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/orinoco/orinoco_pci.ko
  AUTOLOAD:=$(call AutoProbe,orinoco_pci)
endef

define KernelPackage/net-hermes-pci/description
 Kernel modules for Intersil Prism 2.5 PCI support
endef

define KernelPackage/net-hermes-plx
  $(call KernelPackage/mac80211/Default)
  TITLE:=PLX9052 based PCI adaptor
  DEPENDS:=@PCI_SUPPORT +kmod-net-hermes
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/orinoco/orinoco_plx.ko
  AUTOLOAD:=$(call AutoProbe,orinoco_plx)
endef

define KernelPackage/net-hermes-plx/description
 Kernel modules for Hermes in PLX9052 based PCI adaptors
endef

define KernelPackage/net-hermes-pcmcia
  $(call KernelPackage/mac80211/Default)
  TITLE:=Hermes based PCMCIA adaptors
  DEPENDS:=@PCMCIA_SUPPORT +kmod-net-hermes @BROKEN
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/orinoco/orinoco_cs.ko
  AUTOLOAD:=$(call AutoProbe,orinoco_cs)
endef

define KernelPackage/net-hermes-pcmcia/description
 Kernel modules for Hermes based PCMCIA adaptors
endef

define KernelPackage/iwlagn
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 @PCI_SUPPORT +@DRIVER_11N_SUPPORT
  TITLE:=Intel AGN Wireless support
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/iwlwifi/iwlwifi.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/iwlwifi/dvm/iwldvm.ko
  AUTOLOAD:=$(call AutoProbe,iwlwifi iwldvm)
  MENU:=1
endef

define KernelPackage/iwlagn/description
 iwlagn kernel module for Intel 5000/5150/1000/6000/6050/6005/6030/100 support
endef

define KernelPackage/iwlagn/config
  if PACKAGE_kmod-iwlagn

	config IWL5000_FW
		bool "Intel 5000 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Wireless WiFi 5100AGN, 5300AGN, and 5350AGN

	config IWL5150_FW
		bool "Intel 5150 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Wireless WiFi 5150AGN

	config IWL1000_FW
		bool "Intel 1000 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Wireless-N 1000

	config IWL6000_FW
		bool "Intel 6000 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Ultimate-N 6300 and Advanced-N 6200

	config IWL6050_FW
		bool "Intel 6050 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Advanced-N + WiMAX 6250 and Wireless-N + WiMAX 6150

	config IWL6005_FW
		bool "Intel 6005 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Advanced-N 6205

	config IWL6030_FW
		bool "Intel 6030 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Advanced-N 6230, Wireless-N 1030, Wireless-N 130 and Advanced-N 6235

	config IWL7260_FW
		bool "Intel 7260 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Dual Band Wireless-N 7260 and Intel Dual Band Wireless-AC 7260

	config IWL100_FW
		bool "Intel 100 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Wireless-N 100

	config IWL2000_FW
		bool "Intel 2000 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Wireless-N 2200

	config IWL2030_FW
		bool "Intel 2030 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Wireless-N 2230

	config IWL105_FW
		bool "Intel 105 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Wireless-N 105

	config IWL135_FW
		bool "Intel 135 Firmware"
		default y
		help
		  Download and install firmware for:
		    Intel Centrino Wireless-N 135
  endif
endef

define KernelPackage/iwl-legacy
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 @PCI_SUPPORT
  TITLE:=Intel legacy Wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/iwlegacy/iwlegacy.ko
  AUTOLOAD:=$(call AutoProbe,iwlegacy)
endef

define KernelPackage/iwl-legacy/description
 iwl-legacy kernel module for legacy Intel wireless support
endef

define KernelPackage/iwl3945
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 +kmod-iwl-legacy
  TITLE:=Intel iwl3945 Wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/iwlegacy/iwl3945.ko
  AUTOLOAD:=$(call AutoProbe,iwl3945)
endef

define KernelPackage/iwl3945/description
 iwl3945 kernel module for Intel 3945 support
endef

define KernelPackage/iwl4965
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 +kmod-iwl-legacy +@DRIVER_11N_SUPPORT
  TITLE:=Intel iwl4965 Wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/iwlegacy/iwl4965.ko
  AUTOLOAD:=$(call AutoProbe,iwl4965)
endef

define KernelPackage/iwl4965/description
 iwl4965 kernel module for Intel 4965 support
endef


define KernelPackage/mwl8k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for Marvell TOPDOG 802.11 Wireless cards
  URL:=http://wireless.kernel.org/en/users/Drivers/mwl8k
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211 +@DRIVER_11N_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mwl8k.ko
  AUTOLOAD:=$(call AutoProbe,mwl8k)
endef

define KernelPackage/mwl8k/description
 Kernel modules for Marvell TOPDOG 802.11 Wireless cards
endef


define KernelPackage/wlcore
  $(call KernelPackage/mac80211/Default)
  TITLE:=TI common driver part
  DEPENDS+= @TARGET_omap +kmod-mac80211 +@DRIVER_11N_SUPPORT
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ti/wlcore/wlcore.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/ti/wlcore/wlcore_sdio.ko
  AUTOLOAD:=$(call AutoProbe,wlcore wlcore_sdio)
endef

define KernelPackage/wlcore/description
 This module contains some common parts needed by TI Wireless drivers.
endef

define KernelPackage/wl12xx
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for TI WL12xx
  URL:=http://wireless.kernel.org/en/users/Drivers/wl12xx
  DEPENDS+= +kmod-wlcore
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ti/wl12xx/wl12xx.ko
  AUTOLOAD:=$(call AutoProbe,wl12xx)
endef

define KernelPackage/wl12xx/description
 Kernel modules for TI WL12xx
endef

define KernelPackage/wl18xx
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for TI WL18xx
  URL:=http://wireless.kernel.org/en/users/Drivers/wl18xx
  DEPENDS+= +kmod-wlcore
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ti/wl18xx/wl18xx.ko
  AUTOLOAD:=$(call AutoProbe,wl18xx)
endef

define KernelPackage/wl18xx/description
 Kernel modules for TI WL18xx
endef


#Broadcom firmware
ifneq ($(CONFIG_B43_FW_5_10),)
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=5.10.56.27.3
  PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/driver/wl_apsta/wl_prebuilt.o
  PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)_mipsel.tar.bz2
  PKG_B43_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
  PKG_B43_FWV4_MD5SUM:=3363e3a6b3d9d73c49dea870c7834eac
else
ifneq ($(CONFIG_B43_FW_4_178),)
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=4.178.10.4
  PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/linux/wl_apsta.o
  PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2
  PKG_B43_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
  PKG_B43_FWV4_MD5SUM:=14477e8cbbb91b11896affac9b219fdb
else
ifneq ($(CONFIG_B43_FW_5_100_138),)
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=5.100.138
  PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/linux/wl_apsta.o
  PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2
  PKG_B43_FWV4_SOURCE_URL:=http://www.lwfinger.com/b43-firmware/
  PKG_B43_FWV4_MD5SUM:=f4e357b09eaf5d8b1f1920cf3493a555
else
  PKG_B43_FWV4_NAME:=broadcom-wl
  PKG_B43_FWV4_VERSION:=4.150.10.5
  PKG_B43_FWV4_OBJECT:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION)/driver/wl_apsta_mimo.o
  PKG_B43_FWV4_SOURCE:=$(PKG_B43_FWV4_NAME)-$(PKG_B43_FWV4_VERSION).tar.bz2
  PKG_B43_FWV4_SOURCE_URL:=http://mirror2.openwrt.org/sources/
  PKG_B43_FWV4_MD5SUM:=0c6ba9687114c6b598e8019e262d9a60
endif
endif
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


define KernelPackage/b43
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom 43xx wireless support
  URL:=http://linuxwireless.org/en/users/Drivers/b43
  KCONFIG:= \
  	CONFIG_HW_RANDOM=y
  DEPENDS+= +kmod-mac80211 +!TARGET_brcm47xx:kmod-bcma +!(TARGET_brcm47xx||TARGET_brcm63xx):kmod-ssb
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/b43/b43.ko
  AUTOLOAD:=$(call AutoProbe,b43)
  MENU:=1
endef

define KernelPackage/b43/config
  if PACKAGE_kmod-b43

	choice
		prompt "b43 firmware version"
		default B43_FW_5_100_138
		help
		  This option allows you to select the version of the b43 firmware.

	config B43_FW_4_150
		bool "Firmware 410.2160 from driver 4.150.10.5 (old stable)"
		help
		  Stable firmware for BCM43xx devices.

		  If unsure, select this.

	config B43_FW_4_178
		bool "Firmware 478.104 from driver 4.178.10.4"
		help
		  Experimental firmware for BCM43xx devices.

		  This firmware is not tested as much as the "stable" firmware.

		  If unsure, select the "stable" firmware.

	config B43_FW_5_10
		bool "Firmware 508.1084 from driver 5.10.56.27"
		help
		  Newer experimental firmware for BCM43xx devices.

		  This firmware is mostly untested. It is needed for some N-PHY devices.

		  If unsure, select the "stable" firmware.

	config B43_FW_5_100_138
		bool "Firmware 666.2 from driver 5.100.138 (stable)"
		help
		  Newer experimental firmware for BCM43xx devices.

		  This firmware is mostly untested. It is needed for some N-PHY devices.

		  If unsure, select the "stable" firmware.

	config B43_OPENFIRMWARE
		bool "Open FirmWare for WiFi networks"
		help
		  Opensource firmware for BCM43xx devices.

		  Do _not_ select this, unless you know what you are doing.
		  The Opensource firmware is not suitable for embedded devices, yet.
		  It does not support QoS, which is bad for AccessPoints.
		  It does not support hardware crypto acceleration, which is a showstopper
		  for embedded devices with low CPU resources.

		  If unsure, select the "stable" firmware.

	endchoice

	config B43_FW_SQUASH
		bool "Remove unnecessary firmware files"
		depends on !B43_OPENFIRMWARE
		default y
		help
		  This options allows you to remove unnecessary b43 firmware files
		  from the final rootfs image. This can reduce the rootfs size by
		  up to 200k.

		  If unsure, say Y.

	config B43_FW_SQUASH_COREREVS
		string "Core revisions to include"
		depends on B43_FW_SQUASH
		default "5,6,7,8,9,10,11,13,15,16,29"
		help
		  This is a comma seperated list of core revision numbers.

		  Example (keep files for rev5 only):
		    5

		  Example (keep files for rev5 and rev11):
		    5,11

	config B43_FW_SQUASH_PHYTYPES
		string "PHY types to include"
		depends on B43_FW_SQUASH
		default "G,LP,N,HT"
		help
		  This is a comma seperated list of PHY types:
		    A  => A-PHY
		    AG => Dual A-PHY G-PHY
		    G  => G-PHY
		    LP => LP-PHY
		    N  => N-PHY
		    HT  => HT-PHY
		    LCN  => LCN-PHY

		  Example (keep files for G-PHY only):
		    G

		  Example (keep files for G-PHY and N-PHY):
		    G,N

	config PACKAGE_B43_DEBUG
		bool "Enable debug output and debugfs for b43"
		default n
		help
		  Enable additional debug output and runtime sanity checks for b43
		  and enables the debugfs interface.

		  If unsure, say N.

	config PACKAGE_B43_PIO
		bool "Enable support for PIO transfer mode"
		default n
		help
		  Enable support for using PIO instead of DMA. Unless you have DMA
		  transfer problems you don't need this.

		  If unsure, say N.

	config PACKAGE_B43_PHY_N
		bool "Enable support for N-PHYs"
		default y
		help
		  Enable support for N-PHY. This includes support for the following devices:
		  PCI: BCM4321, BCM4322, BCM43222, BCM43224, BCM43225
		  SoC: BCM4716, BCM4717, BCM4718

		  Currently only 11g speed is available.

		  If unsure, say Y.

	config PACKAGE_B43_PHY_HT
		bool "Enable support for HT-PHYs"
		default y
		help
		  Enable support for HT-PHY. This includes support for the following devices:
		  PCI: BCM4331

		  Currently only 11g speed is available.

		  If unsure, say Y.

	config PACKAGE_B43_PHY_LCN
		bool "Enable support for LCN-PHYs"
		depends on BROKEN
		default n
		help
		  Currently broken.

		  If unsure, say N.

  endif
endef

define KernelPackage/b43/description
Kernel module for Broadcom 43xx wireless support (mac80211 stack) new
endef

define KernelPackage/b43legacy
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom 43xx-legacy wireless support
  URL:=http://linuxwireless.org/en/users/Drivers/b43
  KCONFIG:= \
  	CONFIG_HW_RANDOM=y
  DEPENDS+= +kmod-mac80211 +!(TARGET_brcm47xx||TARGET_brcm63xx):kmod-ssb
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/b43legacy/b43legacy.ko
  AUTOLOAD:=$(call AutoProbe,b43legacy)
  MENU:=1
endef

define KernelPackage/b43legacy/config
  if PACKAGE_kmod-b43legacy

	config B43LEGACY_FW_SQUASH
		bool "Remove unnecessary firmware files"
		default y
		help
		  This options allows you to remove unnecessary b43legacy firmware files
		  from the final rootfs image. This can reduce the rootfs size by
		  up to 50k.

		  If unsure, say Y.

	config B43LEGACY_FW_SQUASH_COREREVS
		string "Core revisions to include"
		depends on B43LEGACY_FW_SQUASH
		default "1,2,3,4"
		help
		  This is a comma seperated list of core revision numbers.

		  Example (keep files for rev4 only):
		    4

		  Example (keep files for rev2 and rev4):
		    2,4

  endif
endef

define KernelPackage/b43legacy/description
Kernel module for Broadcom 43xx-legacy wireless support (mac80211 stack) new
endef


define KernelPackage/brcmutil
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom IEEE802.11n common driver parts
  URL:=http://linuxwireless.org/en/users/Drivers/brcm80211
  DEPENDS+=@PCI_SUPPORT||USB_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/brcm80211/brcmutil/brcmutil.ko
  AUTOLOAD:=$(call AutoProbe,brcmutil)
  MENU:=1
endef

define KernelPackage/brcmutil/description
 This module contains some common parts needed by Broadcom Wireless drivers brcmsmac and brcmfmac.
endef

define KernelPackage/brcmutil/config
  if PACKAGE_kmod-brcmutil

	config PACKAGE_BRCM80211_DEBUG
		bool "Broadcom wireless driver debugging"
		help
		  Say Y, if you want to debug brcmsmac and brcmfmac wireless driver.

  endif
endef

PKG_BRCMSMAC_FW_NAME:=broadcom-wl
PKG_BRCMSMAC_FW_VERSION:=5.100.138
PKG_BRCMSMAC_FW_OBJECT:=$(PKG_BRCMSMAC_FW_NAME)-$(PKG_BRCMSMAC_FW_VERSION)/linux/wl_apsta.o
PKG_BRCMSMAC_FW_SOURCE:=$(PKG_BRCMSMAC_FW_NAME)-$(PKG_BRCMSMAC_FW_VERSION).tar.bz2
PKG_BRCMSMAC_FW_SOURCE_URL:=http://www.lwfinger.com/b43-firmware/
PKG_BRCMSMAC_FW_MD5SUM:=f4e357b09eaf5d8b1f1920cf3493a555

define Download/brcmsmac
  FILE:=$(PKG_BRCMSMAC_FW_SOURCE)
  URL:=$(PKG_BRCMSMAC_FW_SOURCE_URL)
  MD5SUM:=$(PKG_BRCMSMAC_FW_MD5SUM)
endef
$(eval $(call Download,brcmsmac))

define KernelPackage/brcmsmac
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom IEEE802.11n PCIe SoftMAC WLAN driver
  URL:=http://linuxwireless.org/en/users/Drivers/brcm80211
  DEPENDS+= +kmod-mac80211 +@DRIVER_11N_SUPPORT +!TARGET_brcm47xx:kmod-bcma +kmod-lib-cordic +kmod-lib-crc8 +kmod-brcmutil
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/brcm80211/brcmsmac/brcmsmac.ko
  AUTOLOAD:=$(call AutoProbe,brcmsmac)
  MENU:=1
endef

define KernelPackage/brcmsmac/description
 Kernel module for Broadcom IEEE802.11n PCIe Wireless cards
endef

define KernelPackage/brcmsmac/config
  if PACKAGE_kmod-brcmsmac

	config BRCMSMAC_USE_FW_FROM_WL
		bool "Use firmware extracted from broadcom proprietary driver"
		default y
		help
		  Instead of using the official brcmsmac firmware a firmware
		  version 666.2 extracted from the proprietary Broadcom driver
		  is used. This is needed to get core rev 17 used in bcm4716
		  to work.

		  If unsure, say Y.

  endif
endef


define KernelPackage/brcmfmac
  $(call KernelPackage/mac80211/Default)
  TITLE:=Broadcom IEEE802.11n USB FullMAC WLAN driver
  URL:=http://linuxwireless.org/en/users/Drivers/brcm80211
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-cfg80211 +@DRIVER_11N_SUPPORT +kmod-brcmutil
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/brcm80211/brcmfmac/brcmfmac.ko
  AUTOLOAD:=$(call AutoProbe,brcmfmac)
endef

define KernelPackage/brcmfmac/description
 Kernel module for Broadcom IEEE802.11n USB Wireless cards
endef

config_package=$(if $(CONFIG_PACKAGE_kmod-$(1)),m)

config-y:= \
	WLAN \
	NL80211_TESTMODE \
	CFG80211_WEXT \
	CFG80211_INTERNAL_REGDB \
	CFG80211_CERTIFICATION_ONUS \
	MAC80211_RC_MINSTREL \
	MAC80211_RC_MINSTREL_HT \
	MAC80211_RC_DEFAULT_MINSTREL \

config-$(call config_package,cfg80211) += CFG80211

config-$(call config_package,mac80211) += MAC80211
config-$(CONFIG_PACKAGE_MAC80211_MESH) += MAC80211_MESH
ifdef CONFIG_PACKAGE_MAC80211_DEBUGFS
  config-y += \
	CFG80211_DEBUGFS \
	MAC80211_DEBUGFS \
	ATH9K_DEBUGFS \
	ATH9K_HTC_DEBUGFS \
	ATH10K_DEBUGFS \
	CARL9170_DEBUGFS \
	ATH5K_DEBUG
endif

config-$(call config_package,lib80211) += LIB80211 LIB80211_CRYPT_WEP LIB80211_CRYPT_CCMP LIB80211_CRYPT_TKIP

config-$(call config_package,ath) += ATH_CARDS ATH_COMMON
config-$(CONFIG_PACKAGE_ATH_DEBUG) += ATH_DEBUG ATH10K_DEBUG
config-$(CONFIG_PACKAGE_ATH_DFS) += ATH9K_DFS_CERTIFIED ATH10K_DFS_CERTIFIED

config-$(call config_package,ath9k) += ATH9K
config-$(call config_package,ath9k-common) += ATH9K_COMMON
config-$(CONFIG_TARGET_ar71xx) += ATH9K_AHB
config-$(CONFIG_PCI) += ATH9K_PCI
config-$(CONFIG_ATH_USER_REGD) += ATH_USER_REGD
config-$(CONFIG_ATH9K_SUPPORT_PCOEM) += ATH9K_PCOEM

config-$(call config_package,ath9k-htc) += ATH9K_HTC
config-$(call config_package,ath10k) += ATH10K ATH10K_PCI

config-$(call config_package,ath5k) += ATH5K
ifdef CONFIG_TARGET_atheros
  config-y += ATH5K_AHB
else
  config-y += ATH5K_PCI
endif

config-$(call config_package,carl9170) += CARL9170

config-$(call config_package,b43) += B43
config-y += B43_SSB B43_BCMA B43_PHY_LP
config-$(CONFIG_PACKAGE_B43_PHY_N) += B43_PHY_N
config-$(CONFIG_PACKAGE_B43_PHY_HT) += B43_PHY_HT
config-$(CONFIG_PACKAGE_B43_PIO) += B43_PIO
config-$(CONFIG_PACKAGE_B43_DEBUG) += B43_DEBUG

config-$(call config_package,b43legacy) += B43LEGACY
config-y += B43LEGACY_DMA_MODE

config-$(call config_package,brcmutil) += BRCMUTIL
config-$(call config_package,brcmsmac) += BRCMSMAC
config-$(call config_package,brcmfmac) += BRCMFMAC
config-y += BRCMFMAC_USB
config-$(CONFIG_PACKAGE_BRCM80211_DEBUG) += BRCMDBG

config-$(call config_package,mac80211-hwsim) += MAC80211_HWSIM

config-$(call config_package,rt2x00-lib) += RT2X00 RT2X00_LIB
config-$(call config_package,rt2x00-pci) += RT2X00_LIB_PCI
config-$(call config_package,rt2x00-mmio) += RT2X00_LIB_MMIO
config-$(call config_package,rt2x00-usb) += RT2X00_LIB_USB
config-$(CONFIG_PACKAGE_RT2X00_LIB_DEBUGFS) += RT2X00_LIB_DEBUGFS
config-$(CONFIG_PACKAGE_RT2X00_DEBUG) += RT2X00_DEBUG

config-$(call config_package,rt2400-pci) += RT2400PCI
config-$(call config_package,rt2500-pci) += RT2500PCI
config-$(call config_package,rt2500-usb) += RT2500USB
config-$(call config_package,rt61-pci) += RT61PCI
config-$(call config_package,rt73-usb) += RT73USB

config-$(call config_package,rt2800-lib) += RT2800_LIB

config-$(call config_package,rt2800-soc) += RT2800SOC
config-$(call config_package,rt2800-pci) += RT2800PCI
config-y += RT2800PCI_RT33XX RT2800PCI_RT35XX RT2800PCI_RT53XX RT2800PCI_RT3290

config-$(call config_package,rt2800-usb) += RT2800USB
config-y += RT2800USB_RT33XX RT2800USB_RT35XX RT2800USB_RT3573 RT2800USB_RT53XX RT2800USB_RT55XX

config-$(call config_package,iwl-legacy) += IWLEGACY
config-$(call config_package,iwl3945) += IWL3945
config-$(call config_package,iwl4965) += IWL4965
config-$(call config_package,iwlagn) += IWLWIFI IWLDVM

config-$(call config_package,net-libipw) += LIBIPW
config-$(call config_package,net-ipw2100) += IPW2100
config-$(call config_package,net-ipw2200) += IPW2200

config-$(call config_package,p54-common) += P54_COMMON
config-$(call config_package,p54-pci) += P54_PCI
config-$(call config_package,p54-usb) += P54_USB
config-$(call config_package,p54-spi) += P54_SPI

config-$(call config_package,net-hermes) += HERMES
config-$(call config_package,net-hermes-pci) += PCI_HERMES
config-$(call config_package,net-hermes-plx) += PLX_HERMES
config-$(call config_package,net-hermes-pcmcia) += PCMCIA_HERMES
config-y += HERMES_PRISM

config-$(call config_package,adm8211) += ADM8211
config-$(call config_package,libertas-sd) += LIBERTAS LIBERTAS_SDIO
config-$(call config_package,libertas-usb) += LIBERTAS LIBERTAS_USB
config-$(call config_package,mwl8k) += MWL8K
config-$(call config_package,rtl8180) += RTL8180
config-$(call config_package,rtl8187) += RTL8187
config-$(call config_package,wlcore) += WLCORE WLCORE_SDIO
config-$(call config_package,wl12xx) += WL12XX
config-$(call config_package,wl18xx) += WL18XX
config-y += WL_TI WILINK_PLATFORM_DATA
config-$(call config_package,zd1211rw) += ZD1211RW

config-$(call config_package,rtlwifi) += RTL_CARDS RTLWIFI
config-$(call config_package,rtlwifi-pci) += RTLWIFI_PCI
config-$(call config_package,rtlwifi-usb) += RTLWIFI_USB
config-$(call config_package,rtl8192c-common) += RTL8192C_COMMON
config-$(call config_package,rtl8192ce) += RTL8192CE
config-$(call config_package,rtl8192se) += RTL8192SE
config-$(call config_package,rtl8192de) += RTL8192DE
config-$(call config_package,rtl8192cu) += RTL8192CU
config-$(CONFIG_PACKAGE_RTLWIFI_DEBUG) += RTLWIFI_DEBUG

config-$(CONFIG_LEDS_TRIGGERS) += MAC80211_LEDS B43_LEDS B43LEGACY_LEDS

MAKE_OPTS:= -C "$(PKG_BUILD_DIR)" \
	CROSS_COMPILE="$(KERNEL_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	EXTRA_CFLAGS="-I$(PKG_BUILD_DIR)/include" \
	KLIB_BUILD="$(LINUX_DIR)" \
	MODPROBE=true \
	KLIB=$(TARGET_MODULES_DIR) \
	KERNEL_SUBLEVEL=$(lastword $(subst ., ,$(KERNEL_PATCHVER))) \
	KBUILD_LDFLAGS_MODULE_PREREQ=

ifneq ($(findstring c,$(OPENWRT_VERBOSE)),)
  MAKE_OPTS += V=1
endif

define ConfigVars
$(subst $(space),,$(foreach opt,$(config-$(1)),CPTCFG_$(opt)=$(1)
))
endef

define mac80211_config
$(call ConfigVars,m)$(call ConfigVars,y)
endef
$(eval $(call shexport,mac80211_config))

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(PKG_UNPACK)
	$(Build/Patch)
	$(TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(IPW2100_NAME)-$(IPW2100_VERSION).tgz
	$(TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(IPW2200_NAME)-$(IPW2200_VERSION).tgz
	$(TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(ZD1211FW_NAME)-$(ZD1211FW_VERSION).tar.bz2
	$(TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(PKG_LINUX_FIRMWARE_SOURCE)
	$(TAR) -C $(PKG_BUILD_DIR) -xjf $(DL_DIR)/$(PKG_ATH10K_LINUX_FIRMWARE_SOURCE)
	rm -rf \
		$(PKG_BUILD_DIR)/include/linux/ssb \
		$(PKG_BUILD_DIR)/include/linux/bcma \
		$(PKG_BUILD_DIR)/include/net/bluetooth

	rm -f \
		$(PKG_BUILD_DIR)/include/linux/cordic.h \
		$(PKG_BUILD_DIR)/include/linux/crc8.h \
		$(PKG_BUILD_DIR)/include/linux/eeprom_93cx6.h \
		$(PKG_BUILD_DIR)/include/linux/wl12xx.h \
		$(PKG_BUILD_DIR)/include/linux/spi/libertas_spi.h \
		$(PKG_BUILD_DIR)/include/net/ieee80211.h

	echo 'compat-wireless-$(PKG_VERSION)-$(PKG_RELEASE)-$(REVISION)' > $(PKG_BUILD_DIR)/compat_version
	$(CP) ./files/regdb.txt $(PKG_BUILD_DIR)/net/wireless/db.txt
endef

ifneq ($(CONFIG_PACKAGE_kmod-cfg80211)$(CONFIG_PACKAGE_kmod-lib80211),)
 define Build/Compile/kmod
	rm -rf $(PKG_BUILD_DIR)/modules
	+$(MAKE) $(PKG_JOBS) $(MAKE_OPTS) modules
 endef
endif

define Build/Configure
	cmp $(PKG_BUILD_DIR)/include/linux/ath9k_platform.h $(LINUX_DIR)/include/linux/ath9k_platform.h
	cmp $(PKG_BUILD_DIR)/include/linux/ath5k_platform.h $(LINUX_DIR)/include/linux/ath5k_platform.h
	cmp $(PKG_BUILD_DIR)/include/linux/rt2x00_platform.h $(LINUX_DIR)/include/linux/rt2x00_platform.h
endef

define Build/Compile
	$(SH_FUNC) var2file "$(call shvar,mac80211_config)" $(PKG_BUILD_DIR)/.config
	$(MAKE) $(MAKE_OPTS) allnoconfig
	$(call Build/Compile/kmod)
endef

define Build/InstallDev
	mkdir -p \
		$(1)/usr/include/mac80211 \
		$(1)/usr/include/mac80211-backport \
		$(1)/usr/include/mac80211/ath \
		$(1)/usr/include/net/mac80211
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/*.h $(PKG_BUILD_DIR)/include/* $(1)/usr/include/mac80211/
	$(CP) $(PKG_BUILD_DIR)/backport-include/* $(1)/usr/include/mac80211-backport/
	$(CP) $(PKG_BUILD_DIR)/net/mac80211/rate.h $(1)/usr/include/net/mac80211/
	$(CP) $(PKG_BUILD_DIR)/drivers/net/wireless/ath/*.h $(1)/usr/include/mac80211/ath/
endef

define KernelPackage/libertas-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware/libertas
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/usb8388_v9.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/usb8682.bin \
		$(1)/lib/firmware/libertas/
endef

define KernelPackage/libertas-sd/install
	$(INSTALL_DIR) $(1)/lib/firmware/libertas
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/sd8385_helper.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/sd8385.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/sd8686_v9_helper.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/sd8686_v9.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/sd8688_helper.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/libertas/sd8688.bin \
		$(1)/lib/firmware/libertas
endef

define KernelPackage/cfg80211/install
	$(INSTALL_DIR) $(1)/lib/wifi $(1)/lib/netifd/wireless
	$(INSTALL_DATA) ./files/lib/wifi/mac80211.sh $(1)/lib/wifi
	$(INSTALL_BIN) ./files/lib/netifd/wireless/mac80211.sh $(1)/lib/netifd/wireless
endef

define KernelPackage/p54-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54PCIFW) $(1)/lib/firmware/isl3886pci
endef

define KernelPackage/p54-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54USBFW) $(1)/lib/firmware/isl3887usb
endef

define KernelPackage/p54-spi/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(P54SPIFW) $(1)/lib/firmware/3826.arm
endef

define KernelPackage/rt61-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt2561.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt2561s.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt2661.bin \
		$(1)/lib/firmware/
endef

define KernelPackage/rt73-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt73.bin $(1)/lib/firmware/
endef

define KernelPackage/rt2800-pci/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt2860.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt3290.bin \
		$(1)/lib/firmware
endef

define KernelPackage/rt2800-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/rt2870.bin $(1)/lib/firmware/
endef

define KernelPackage/wl12xx/install
	$(INSTALL_DIR) $(1)/lib/firmware/ti-connectivity
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl127x-fw-5-mr.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl127x-fw-5-plt.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl127x-fw-5-sr.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl1271-nvs.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl128x-fw-5-mr.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl128x-fw-5-plt.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl128x-fw-5-sr.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl128x-nvs.bin \
		$(1)/lib/firmware/ti-connectivity
endef

define KernelPackage/wl18xx/install
	$(INSTALL_DIR) $(1)/lib/firmware/ti-connectivity
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl18xx-conf.bin \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/ti-connectivity/wl18xx-fw-3.bin \
		$(1)/lib/firmware/ti-connectivity
endef

define KernelPackage/zd1211rw/install
	$(INSTALL_DIR) $(1)/lib/firmware/zd1211
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(ZD1211FW_NAME)/zd1211* $(1)/lib/firmware/zd1211
endef

define KernelPackage/carl9170/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/carl9170-1.fw $(1)/lib/firmware
endef

define KernelPackage/ath9k-htc/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/htc_9271.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/htc_7010.fw \
		$(1)/lib/firmware/
endef

define KernelPackage/ath10k/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA988X/hw2.0
ifeq ($(CONFIG_ATH10K_STA_FW),y)
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_ATH10K_LINUX_FIRMWARE_SUBDIR)/ath10k/QCA988X/hw2.0/board.bin \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_ATH10K_LINUX_FIRMWARE_SUBDIR)/main/firmware-2.bin_999.999.0.636 \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/firmware-2.bin
else
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_ATH10K_LINUX_FIRMWARE_SUBDIR)/ath10k/QCA988X/hw2.0/board.bin \
		$(PKG_BUILD_DIR)/$(PKG_ATH10K_LINUX_FIRMWARE_SUBDIR)/ath10k/QCA988X/hw2.0/firmware-2.bin \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/
endif
endef

define KernelPackage/mwl8k/install
	$(INSTALL_DIR) $(1)/lib/firmware/mwl8k
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/mwl8k/fmimage_8366_ap-3.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/mwl8k/fmimage_8366.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/mwl8k/helper_8366.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/mwl8k/fmimage_8687.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/mwl8k/helper_8687.fw \
		$(1)/lib/firmware/mwl8k/
endef

define KernelPackage/net-ipw2100/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ipw2100-$(IPW2100_VERSION)*.fw $(1)/lib/firmware
endef

define KernelPackage/net-ipw2200/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(IPW2200_NAME)-$(IPW2200_VERSION)/ipw2200*.fw $(1)/lib/firmware
endef

define KernelPackage/iwlagn/install
	$(INSTALL_DIR) $(1)/lib/firmware
ifneq ($(CONFIG_IWL5000_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-5000-5.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL5150_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-5150-2.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL1000_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-1000-5.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL6000_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-6000-4.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL6050_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-6050-5.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL6005_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-6000g2a-6.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL6030_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-6000g2b-6.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL7260_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-7260-7.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL100_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-100-5.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL2000_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-2000-6.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL2030_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-2030-6.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL105_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-105-6.ucode $(1)/lib/firmware
endif
ifneq ($(CONFIG_IWL135_FW),)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-135-6.ucode $(1)/lib/firmware
endif
endef

define KernelPackage/iwl3945/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-3945-2.ucode $(1)/lib/firmware
endef

define KernelPackage/iwl4965/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/iwlwifi-4965-2.ucode $(1)/lib/firmware
endef

define KernelPackage/b43/install
	rm -rf $(1)/lib/firmware/
ifeq ($(CONFIG_B43_OPENFIRMWARE),y)
	tar xzf "$(DL_DIR)/$(PKG_B43_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
else
	tar xjf "$(DL_DIR)/$(PKG_B43_FWV4_SOURCE)" -C "$(PKG_BUILD_DIR)"
endif
	$(INSTALL_DIR) $(1)/lib/firmware/
ifeq ($(CONFIG_B43_OPENFIRMWARE),y)
	$(MAKE) -C "$(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/"
	$(INSTALL_DIR) $(1)/lib/firmware/b43-open/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/ucode5.fw $(1)/lib/firmware/b43-open/ucode5.fw
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/b0g0bsinitvals5.fw $(1)/lib/firmware/b43-open/b0g0bsinitvals5.fw
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)/b0g0initvals5.fw $(1)/lib/firmware/b43-open/b0g0initvals5.fw
else
	b43-fwcutter -w $(1)/lib/firmware/ $(PKG_BUILD_DIR)/$(PKG_B43_FWV4_OBJECT)
endif
ifneq ($(CONFIG_B43_FW_SQUASH),)
	b43-fwsquash.py "$(CONFIG_B43_FW_SQUASH_PHYTYPES)" "$(CONFIG_B43_FW_SQUASH_COREREVS)" "$(1)/lib/firmware/b43"
endif
endef

define KernelPackage/b43legacy/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	b43-fwcutter --unsupported -w $(1)/lib/firmware/ $(DL_DIR)/$(PKG_B43_FWV3_SOURCE)
ifneq ($(CONFIG_B43LEGACY_FW_SQUASH),)
	b43-fwsquash.py "G" "$(CONFIG_B43LEGACY_FW_SQUASH_COREREVS)" "$(1)/lib/firmware/b43legacy"
endif
endef

define KernelPackage/brcmsmac/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
ifeq ($(CONFIG_BRCMSMAC_USE_FW_FROM_WL),y)
	tar xjf "$(DL_DIR)/$(PKG_BRCMSMAC_FW_SOURCE)" -C "$(PKG_BUILD_DIR)"
	b43-fwcutter --brcmsmac -w $(1)/lib/firmware/ $(PKG_BUILD_DIR)/$(PKG_BRCMSMAC_FW_OBJECT)
else
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/brcm/bcm43xx-0.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/brcm/bcm43xx_hdr-0.fw \
		$(1)/lib/firmware/brcm/
endif
endef

define KernelPackage/brcmfmac/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/brcm/brcmfmac43236b.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call KernelPackage,adm8211))
$(eval $(call KernelPackage,ath5k))
$(eval $(call KernelPackage,lib80211))
$(eval $(call KernelPackage,libertas-usb))
$(eval $(call KernelPackage,libertas-sd))
$(eval $(call KernelPackage,cfg80211))
$(eval $(call KernelPackage,mac80211))
$(eval $(call KernelPackage,p54-common))
$(eval $(call KernelPackage,p54-pci))
$(eval $(call KernelPackage,p54-usb))
$(eval $(call KernelPackage,p54-spi))
$(eval $(call KernelPackage,rt2x00-lib))
$(eval $(call KernelPackage,rt2x00-mmio))
$(eval $(call KernelPackage,rt2x00-pci))
$(eval $(call KernelPackage,rt2x00-usb))
$(eval $(call KernelPackage,rt2800-lib))
$(eval $(call KernelPackage,rt2400-pci))
$(eval $(call KernelPackage,rt2500-pci))
$(eval $(call KernelPackage,rt2500-usb))
$(eval $(call KernelPackage,rt61-pci))
$(eval $(call KernelPackage,rt73-usb))
$(eval $(call KernelPackage,rt2800-mmio))
$(eval $(call KernelPackage,rt2800-soc))
$(eval $(call KernelPackage,rt2800-pci))
$(eval $(call KernelPackage,rt2800-usb))
$(eval $(call KernelPackage,rtl8180))
$(eval $(call KernelPackage,rtl8187))
$(eval $(call KernelPackage,rtlwifi))
$(eval $(call KernelPackage,rtlwifi-pci))
$(eval $(call KernelPackage,rtlwifi-usb))
$(eval $(call KernelPackage,rtl8192c-common))
$(eval $(call KernelPackage,rtl8192ce))
$(eval $(call KernelPackage,rtl8192se))
$(eval $(call KernelPackage,rtl8192de))
$(eval $(call KernelPackage,rtl8192cu))
$(eval $(call KernelPackage,zd1211rw))
$(eval $(call KernelPackage,mac80211-hwsim))
$(eval $(call KernelPackage,ath9k-common))
$(eval $(call KernelPackage,ath9k))
$(eval $(call KernelPackage,ath9k-htc))
$(eval $(call KernelPackage,ath10k))
$(eval $(call KernelPackage,ath))
$(eval $(call KernelPackage,carl9170))
$(eval $(call KernelPackage,b43))
$(eval $(call KernelPackage,b43legacy))
$(eval $(call KernelPackage,brcmutil))
$(eval $(call KernelPackage,brcmsmac))
$(eval $(call KernelPackage,brcmfmac))
$(eval $(call KernelPackage,net-libipw))
$(eval $(call KernelPackage,net-ipw2100))
$(eval $(call KernelPackage,net-ipw2200))
$(eval $(call KernelPackage,iwlagn))
$(eval $(call KernelPackage,iwl-legacy))
$(eval $(call KernelPackage,iwl4965))
$(eval $(call KernelPackage,iwl3945))
$(eval $(call KernelPackage,mwl8k))
$(eval $(call KernelPackage,net-hermes))
$(eval $(call KernelPackage,net-hermes-pci))
$(eval $(call KernelPackage,net-hermes-plx))
$(eval $(call KernelPackage,net-hermes-pcmcia))
$(eval $(call KernelPackage,wlcore))
$(eval $(call KernelPackage,wl12xx))
$(eval $(call KernelPackage,wl18xx))
