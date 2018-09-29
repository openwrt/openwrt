#
# Copyright (C) 2007-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mac80211

PKG_VERSION:=4.19-rc5-1
PKG_RELEASE:=1
PKG_SOURCE_URL:=https://mirrors.edge.kernel.org/pub/linux/kernel/projects/backports/stable/v4.19-rc5/
PKG_HASH:=5b61e64ea79d22bbac9e8612d5d5485974f223de00d4ec250b0faf4b7baf9957

PKG_SOURCE:=backports-$(PKG_VERSION).tar.xz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/backports-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

PKG_DRIVERS = \
	adm8211 \
	airo \
	hermes hermes-pci hermes-pcmcia hermes-plx\
	iwl-legacy iwl3945 iwl4965 iwlwifi \
	lib80211 \
	libipw ipw2100 ipw2200 \
	libertas-sdio libertas-usb libertas-spi \
	mac80211-hwsim \
	mt7601u \
	mwl8k mwifiex-pcie mwifiex-sdio \
	p54-common p54-pci p54-spi p54-usb \
	rsi91x rsi91x-usb rsi91x-sdio\
	rtl8180 rtl8187 \
	rtlwifi rtlwifi-pci rtlwifi-btcoexist rtlwifi-usb rtl8192c-common \
	rtl8192ce rtl8192se rtl8192de rtl8192cu rtl8821ae \
	rtl8xxxu \
	wlcore wl12xx wl18xx \
	zd1211rw

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_kmod-mac80211 \
	CONFIG_PACKAGE_MAC80211_DEBUGFS \
	CONFIG_PACKAGE_MAC80211_MESH \
	CONFIG_PACKAGE_MAC80211_TRACING \
	CONFIG_PACKAGE_IWLWIFI_DEBUG \
	CONFIG_PACKAGE_IWLWIFI_DEBUGFS \
	CONFIG_PACKAGE_RTLWIFI_DEBUG \

include $(INCLUDE_DIR)/package.mk

WMENU:=Wireless Drivers

define KernelPackage/mac80211/Default
  SUBMENU:=$(WMENU)
  URL:=https://wireless.wiki.kernel.org/
  MAINTAINER:=Felix Fietkau <nbd@nbd.name>
endef

config_package=$(if $(CONFIG_PACKAGE_kmod-$(1)),m)

config-y:= \
	WLAN \
	NL80211_TESTMODE \
	CFG80211_WEXT \
	CFG80211_CERTIFICATION_ONUS \
	MAC80211_RC_MINSTREL \
	MAC80211_RC_MINSTREL_HT \
	MAC80211_RC_MINSTREL_VHT \
	MAC80211_RC_DEFAULT_MINSTREL \
	WLAN_VENDOR_ADMTEK \
	WLAN_VENDOR_ATH \
	WLAN_VENDOR_ATMEL \
	WLAN_VENDOR_BROADCOM \
	WLAN_VENDOR_CISCO \
	WLAN_VENDOR_INTEL \
	WLAN_VENDOR_INTERSIL \
	WLAN_VENDOR_MARVELL \
	WLAN_VENDOR_MEDIATEK \
	WLAN_VENDOR_RALINK \
	WLAN_VENDOR_REALTEK \
	WLAN_VENDOR_RSI \
	WLAN_VENDOR_ST \
	WLAN_VENDOR_TI \
	WLAN_VENDOR_ZYDAS \

config-$(call config_package,cfg80211) += CFG80211

config-$(call config_package,mac80211) += MAC80211
config-$(CONFIG_PACKAGE_MAC80211_MESH) += MAC80211_MESH

include ath.mk
include broadcom.mk
include ralink.mk

PKG_CONFIG_DEPENDS += \
	$(patsubst %,CONFIG_PACKAGE_kmod-%,$(PKG_DRIVERS))

define KernelPackage/cfg80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=cfg80211 - wireless configuration API
  DEPENDS+= +iw +wireless-regdb
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
  # +kmod-crypto-cmac is a runtime only dependency of net/mac80211/aes_cmac.c
  DEPENDS+= +kmod-cfg80211 +hostapd-common
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
		default y
		help
		  Select this to see extensive information about
		  the internal state of mac80211 in debugfs.

	config PACKAGE_MAC80211_TRACING
		bool "Enable tracing (mac80211 and supported drivers)"
		select KERNEL_FTRACE
		select KERNEL_ENABLE_DEFAULT_TRACERS
		default n
		help
		  Select this to enable tracing of mac80211 and
		  related wifi drivers (using trace-cmd).

	config PACKAGE_MAC80211_MESH
		bool "Enable 802.11s mesh support"
		default y

  endif
endef

define KernelPackage/mac80211/description
Generic IEEE 802.11 Networking Stack (mac80211)
endef

define KernelPackage/adm8211
  $(call KernelPackage/mac80211/Default)
  TITLE:=ADMTek 8211 support
  DEPENDS+=@PCI_SUPPORT +kmod-mac80211 +kmod-eeprom-93cx6
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/admtek/adm8211.ko
  AUTOLOAD:=$(call AutoProbe,adm8211)
endef

define KernelPackage/airo
  $(call KernelPackage/mac80211/Default)
  TITLE:=Cisco Aironet driver
  DEPENDS+=@PCI_SUPPORT +@DRIVER_WEXT_SUPPORT +kmod-cfg80211 @TARGET_x86
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/cisco/airo.ko
  AUTOLOAD:=$(call AutoProbe,airo)
endef

define KernelPackage/airo/description
 Kernel support for Cisco Aironet cards
endef

define KernelPackage/hermes
  $(call KernelPackage/mac80211/Default)
  TITLE:=Hermes 802.11b chipset support
  DEPENDS:=@PCI_SUPPORT||PCMCIA_SUPPORT +kmod-cfg80211 +@DRIVER_WEXT_SUPPORT +kmod-crypto-michael-mic
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/orinoco/orinoco.ko
  AUTOLOAD:=$(call AutoProbe,orinoco)
endef

define KernelPackage/hermes/description
 Kernel support for Hermes 802.11b chipsets
endef

define KernelPackage/hermes-pci
  $(call KernelPackage/mac80211/Default)
  TITLE:=Intersil Prism 2.5 PCI support
  DEPENDS:=@PCI_SUPPORT +kmod-hermes
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/orinoco/orinoco_pci.ko
  AUTOLOAD:=$(call AutoProbe,orinoco_pci)
endef

define KernelPackage/hermes-pci/description
 Kernel modules for Intersil Prism 2.5 PCI support
endef

define KernelPackage/hermes-plx
  $(call KernelPackage/mac80211/Default)
  TITLE:=PLX9052 based PCI adaptor
  DEPENDS:=@PCI_SUPPORT +kmod-hermes
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/orinoco/orinoco_plx.ko
  AUTOLOAD:=$(call AutoProbe,orinoco_plx)
endef

define KernelPackage/hermes-plx/description
 Kernel modules for Hermes in PLX9052 based PCI adaptors
endef

define KernelPackage/hermes-pcmcia
  $(call KernelPackage/mac80211/Default)
  TITLE:=Hermes based PCMCIA adaptors
  DEPENDS:=@PCMCIA_SUPPORT +kmod-hermes
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/orinoco/orinoco_cs.ko
  AUTOLOAD:=$(call AutoProbe,orinoco_cs)
endef

define KernelPackage/hermes-pcmcia/description
 Kernel modules for Hermes based PCMCIA adaptors
endef


define KernelPackage/iwlwifi
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 @PCI_SUPPORT +@DRIVER_11N_SUPPORT +@DRIVER_11AC_SUPPORT @!LINUX_3_18
  TITLE:=Intel AGN Wireless support
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwifi/iwlwifi.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwifi/dvm/iwldvm.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlwifi/mvm/iwlmvm.ko
  AUTOLOAD:=$(call AutoProbe,iwlwifi iwldvm iwlmvm)
  MENU:=1
endef

define KernelPackage/iwlwifi/description
 iwlwifi kernel module for
 Intel Wireless WiFi Link 6250AGN Adapter
 Intel 6000 Series Wi-Fi Adapters (6200AGN and 6300AGN)
 Intel WiFi Link 1000BGN
 Intel Wireless WiFi 5150AGN
 Intel Wireless WiFi 5100AGN, 5300AGN, and 5350AGN
 Intel 6005 Series Wi-Fi Adapters
 Intel 6030 Series Wi-Fi Adapters
 Intel Wireless WiFi Link 6150BGN 2 Adapter
 Intel 100 Series Wi-Fi Adapters (100BGN and 130BGN)
 Intel 2000 Series Wi-Fi Adapters
 Intel 7260 Wi-Fi Adapter
 Intel 3160 Wi-Fi Adapter
 Intel 7265 Wi-Fi Adapter
 Intel 8260 Wi-Fi Adapter
 Intel 3165 Wi-Fi Adapter
endef

define KernelPackage/iwlwifi/config
  if PACKAGE_kmod-iwlwifi

	config PACKAGE_IWLWIFI_DEBUG
		bool "Enable full debugging output in the iwlwifi driver"
		default n
		help
		  This option will enable debug tracing output for the iwlwifi drivers

		  This will result in the kernel module being ~100k larger.  You can
		  control which debug output is sent to the kernel log by setting the
		  value in

			/sys/module/iwlwifi/parameters/debug

		  This entry will only exist if this option is enabled.

		  To set a value, simply echo an 8-byte hex value to the same file:

			  % echo 0x43fff > /sys/module/iwlwifi/parameters/debug

		  You can find the list of debug mask values in:
			  drivers/net/wireless/intel/iwlwifi/iwl-debug.h

		  If this is your first time using this driver, you should say Y here
		  as the debug information can assist others in helping you resolve
		  any problems you may encounter.

	config PACKAGE_IWLWIFI_DEBUGFS
	        bool "iwlwifi debugfs support"
		depends on PACKAGE_MAC80211_DEBUGFS
		default n
		help
		  Enable creation of debugfs files for the iwlwifi drivers. This
		  is a low-impact option that allows getting insight into the
		  driver's state at runtime.

  endif
endef

define KernelPackage/iwl-legacy
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 @PCI_SUPPORT
  TITLE:=Intel legacy Wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlegacy/iwlegacy.ko
  AUTOLOAD:=$(call AutoProbe,iwlegacy)
endef

define KernelPackage/iwl-legacy/description
 iwl-legacy kernel module for legacy Intel wireless support
endef

define KernelPackage/iwl3945
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 +kmod-iwl-legacy +iwl3945-firmware
  TITLE:=Intel iwl3945 Wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlegacy/iwl3945.ko
  AUTOLOAD:=$(call AutoProbe,iwl3945)
endef

define KernelPackage/iwl3945/description
 iwl3945 kernel module for Intel 3945 support
endef

define KernelPackage/iwl4965
  $(call KernelPackage/mac80211/Default)
  DEPENDS:= +kmod-mac80211 +kmod-iwl-legacy +@DRIVER_11N_SUPPORT +iwl4965-firmware
  TITLE:=Intel iwl4965 Wireless support
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intel/iwlegacy/iwl4965.ko
  AUTOLOAD:=$(call AutoProbe,iwl4965)
endef

define KernelPackage/iwl4965/description
 iwl4965 kernel module for Intel 4965 support
endef


define KernelPackage/lib80211
  $(call KernelPackage/mac80211/Default)
  TITLE:=802.11 Networking stack
  DEPENDS:=+kmod-cfg80211 +kmod-crypto-hash
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


define KernelPackage/libipw
  $(call KernelPackage/mac80211/Default)
  TITLE:=libipw for ipw2100 and ipw2200
  DEPENDS:=@PCI_SUPPORT +kmod-crypto-michael-mic +kmod-crypto-ecb +kmod-lib80211 +kmod-cfg80211 +@DRIVER_WEXT_SUPPORT @!BIG_ENDIAN @!LINUX_3_18
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intel/ipw2x00/libipw.ko
  AUTOLOAD:=$(call AutoProbe,libipw)
endef

define KernelPackage/libipw/description
 Hardware independent IEEE 802.11 networking stack for ipw2100 and ipw2200.
endef

IPW2100_NAME:=ipw2100-fw
IPW2100_VERSION:=1.3

define Download/ipw2100
  URL:= \
	https://src.fedoraproject.org/repo/pkgs/ipw2100-firmware/ipw2100-fw-1.3.tgz/46aa75bcda1a00efa841f9707bbbd113/ \
	https://archlinux.mirror.pkern.at/other/packages/ipw2100-fw/ \
	http://mirror.ox.ac.uk/sites/ftp.openbsd.org/pub/OpenBSD/distfiles/firmware/ \
	http://firmware.openbsd.org/firmware-dist/
  FILE:=$(IPW2100_NAME)-$(IPW2100_VERSION).tgz
  HASH:=e1107c455e48d324a616b47a622593bc8413dcce72026f72731c0b03dae3a7a2
endef
$(eval $(call Download,ipw2100))

define KernelPackage/ipw2100
  $(call KernelPackage/mac80211/Default)
  TITLE:=Intel IPW2100 driver
  DEPENDS:=@PCI_SUPPORT +kmod-libipw
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intel/ipw2x00/ipw2100.ko
  AUTOLOAD:=$(call AutoProbe,ipw2100)
endef

define KernelPackage/ipw2100/description
 Kernel support for Intel IPW2100
 Includes:
 - ipw2100
endef

IPW2200_NAME:=ipw2200-fw
IPW2200_VERSION:=3.1

define Download/ipw2200
  URL:= \
	https://src.fedoraproject.org/repo/pkgs/ipw2200-firmware/ipw2200-fw-3.1.tgz/eaba788643c7cc7483dd67ace70f6e99/ \
	https://archlinux.mirror.pkern.at/other/packages/ipw2200-fw/ \
	http://mirror.ox.ac.uk/sites/ftp.openbsd.org/pub/OpenBSD/distfiles/firmware/ \
	http://firmware.openbsd.org/firmware-dist/
  FILE:=$(IPW2200_NAME)-$(IPW2200_VERSION).tgz
  HASH:=c6818c11c18cc030d55ff83f64b2bad8feef485e7742f84f94a61d811a6258bd
endef
$(eval $(call Download,ipw2200))

define KernelPackage/ipw2200
  $(call KernelPackage/mac80211/Default)
  TITLE:=Intel IPW2200 driver
  DEPENDS:=@PCI_SUPPORT +kmod-libipw
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intel/ipw2x00/ipw2200.ko
  AUTOLOAD:=$(call AutoProbe,ipw2200)
endef

define KernelPackage/ipw2200/description
 Kernel support for Intel IPW2200
 Includes:
 - ipw2200
endef


define KernelPackage/libertas-usb
  $(call KernelPackage/mac80211/Default)
  DEPENDS+= @USB_SUPPORT +kmod-cfg80211 +kmod-usb-core +kmod-lib80211 +@DRIVER_WEXT_SUPPORT +libertas-usb-firmware @!LINUX_3_18
  TITLE:=Marvell 88W8015 Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/libertas/libertas.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/libertas/usb8xxx.ko
  AUTOLOAD:=$(call AutoProbe,libertas usb8xxx)
endef

define KernelPackage/libertas-sdio
  $(call KernelPackage/mac80211/Default)
  DEPENDS+= +kmod-cfg80211 +kmod-lib80211 +kmod-mmc +@DRIVER_WEXT_SUPPORT @!TARGET_uml +libertas-sdio-firmware @!LINUX_3_18
  TITLE:=Marvell 88W8686 Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/libertas/libertas.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/libertas/libertas_sdio.ko
  AUTOLOAD:=$(call AutoProbe,libertas libertas_sdio)
endef

define KernelPackage/libertas-spi
  $(call KernelPackage/mac80211/Default)
  SUBMENU:=Wireless Drivers
  DEPENDS+= +kmod-cfg80211 +kmod-lib80211 +@DRIVER_WEXT_SUPPORT @!TARGET_uml +libertas-spi-firmware @!LINUX_3_18
  KCONFIG := \
	CONFIG_SPI=y \
	CONFIG_SPI_MASTER=y
  TITLE:=Marvell 88W8686 SPI Wireless Driver
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/libertas/libertas.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/libertas/libertas_spi.ko
  AUTOLOAD:=$(call AutoProbe,libertas libertas_spi)
endef

define KernelPackage/mac80211-hwsim
  $(call KernelPackage/mac80211/Default)
  TITLE:=mac80211 HW simulation device
  DEPENDS+= +kmod-mac80211 +@DRIVER_11AC_SUPPORT +@DRIVER_11N_SUPPORT +@DRIVER_11W_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mac80211_hwsim.ko
  AUTOLOAD:=$(call AutoProbe,mac80211_hwsim)
endef


define KernelPackage/mt7601u
  $(call KernelPackage/mac80211/Default)
  TITLE:=MT7601U-based USB dongles Wireless Driver
  DEPENDS+= +kmod-mac80211 +@DRIVER_11N_SUPPORT @USB_SUPPORT +kmod-usb-core +mt7601u-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/mediatek/mt7601u/mt7601u.ko
  AUTOLOAD:=$(call AutoProbe,mt7601u)
endef


define KernelPackage/mwl8k
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for Marvell TOPDOG 802.11 Wireless cards
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/mwl8k
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211 +@DRIVER_11N_SUPPORT +mwl8k-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/mwl8k.ko
  AUTOLOAD:=$(call AutoProbe,mwl8k)
endef

define KernelPackage/mwl8k/description
 Kernel modules for Marvell TOPDOG 802.11 Wireless cards
endef


define KernelPackage/mwifiex-pcie
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for Marvell 802.11n/802.11ac PCIe Wireless cards
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/mwifiex
  DEPENDS+= @PCI_SUPPORT +kmod-mac80211 +@DRIVER_11N_SUPPORT +@DRIVER_11AC_SUPPORT +mwifiex-pcie-firmware
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/mwifiex/mwifiex.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/mwifiex/mwifiex_pcie.ko
  AUTOLOAD:=$(call AutoProbe,mwifiex_pcie)
endef

define KernelPackage/mwifiex-pcie/description
 Kernel modules for Marvell 802.11n/802.11ac PCIe Wireless cards
endef

define KernelPackage/mwifiex-sdio
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for Marvell 802.11n/802.11ac SDIO Wireless cards
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/mwifiex
  DEPENDS+= +kmod-mmc +kmod-mac80211 +@DRIVER_11N_SUPPORT +@DRIVER_11AC_SUPPORT +mwifiex-sdio-firmware
  FILES:= \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/mwifiex/mwifiex.ko \
	$(PKG_BUILD_DIR)/drivers/net/wireless/marvell/mwifiex/mwifiex_sdio.ko
  AUTOLOAD:=$(call AutoProbe,mwifiex_sdio)
endef

define KernelPackage/mwifiex-sdio/description
 Kernel modules for Marvell 802.11n/802.11ac SDIO Wireless cards
endef

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
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/p54/p54common.ko
endef

define KernelPackage/p54-pci
  $(call KernelPackage/p54/Default)
  TITLE+= (PCI)
  DEPENDS+= @PCI_SUPPORT +kmod-p54-common +p54-pci-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/p54/p54pci.ko
  AUTOLOAD:=$(call AutoProbe,p54pci)
endef

define KernelPackage/p54-usb
  $(call KernelPackage/p54/Default)
  TITLE+= (USB)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-p54-common +p54-usb-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/p54/p54usb.ko
  AUTOLOAD:=$(call AutoProbe,p54usb)
endef

define KernelPackage/p54-spi
  $(call KernelPackage/p54/Default)
  TITLE+= (SPI)
  DEPENDS+= @TARGET_omap24xx +kmod-p54-common +p54-spi-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/intersil/p54/p54spi.ko
  AUTOLOAD:=$(call AutoProbe,p54spi)
endef

define KernelPackage/rsi91x
  $(call KernelPackage/mac80211/Default)
  TITLE:=Redpine Signals Inc 91x WLAN driver support
  DEPENDS+= +kmod-mac80211 +rs9113-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rsi/rsi_91x.ko
endef

define KernelPackage/rsi91x-usb
  $(call KernelPackage/mac80211/Default)
  TITLE:=Redpine Signals USB bus support
  DEPENDS+= +kmod-mac80211 +kmod-usb2 +kmod-rsi91x +rs9113-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rsi/rsi_usb.ko
  AUTOLOAD:=$(call AutoProbe,rsi_usb)
endef

define KernelPackage/rsi91x-sdio
  $(call KernelPackage/mac80211/Default)
  TITLE:=Redpine Signals SDIO bus support
  DEPENDS+= +kmod-mac80211 +kmod-mmc +kmod-rsi91x +rs9113-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/rsi/rsi_sdio.ko
  AUTOLOAD:=$(call AutoProbe,rsi_sdio)
endef


define KernelPackage/rtl818x/Default
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek Drivers for RTL818x devices
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/rtl8187
  DEPENDS+= +kmod-eeprom-93cx6 +kmod-mac80211
endef

define KernelPackage/rtl8180
  $(call KernelPackage/rtl818x/Default)
  DEPENDS+= @PCI_SUPPORT
  TITLE+= (RTL8180 PCI)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtl818x/rtl8180/rtl818x_pci.ko
  AUTOLOAD:=$(call AutoProbe,rtl818x_pci)
endef

define KernelPackage/rtl8187
$(call KernelPackage/rtl818x/Default)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core
  TITLE+= (RTL8187 USB)
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtl818x/rtl8187/rtl8187.ko
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
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtlwifi.ko
  HIDDEN:=1
endef

define KernelPackage/rtlwifi-pci
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek common driver part (PCI support)
  DEPENDS+= @PCI_SUPPORT +kmod-rtlwifi
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl_pci.ko
  AUTOLOAD:=$(call AutoProbe,rtl_pci)
  HIDDEN:=1
endef

define KernelPackage/rtlwifi-btcoexist
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek BT coexist support
  DEPENDS+= +kmod-rtlwifi
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/btcoexist/btcoexist.ko
  AUTOLOAD:=$(call AutoProbe,btcoexist)
  HIDDEN:=1
endef

define KernelPackage/rtlwifi-usb
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek common driver part (USB support)
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-rtlwifi
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl_usb.ko
  AUTOLOAD:=$(call AutoProbe,rtl_usb)
  HIDDEN:=1
endef

define KernelPackage/rtl8192c-common
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192CE/RTL8192CU common support module
  DEPENDS+= +kmod-rtlwifi
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl8192c/rtl8192c-common.ko
  HIDDEN:=1
endef

define KernelPackage/rtl8192ce
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192CE/RTL8188CE support
  DEPENDS+= +kmod-rtlwifi-pci +kmod-rtl8192c-common +rtl8192ce-firmware
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl8192ce/rtl8192ce.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192ce)
endef

define KernelPackage/rtl8192se
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192SE/RTL8191SE support
  DEPENDS+= +kmod-rtlwifi-pci +rtl8192se-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl8192se/rtl8192se.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192se)
endef

define KernelPackage/rtl8192de
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192DE/RTL8188DE support
  DEPENDS+= +kmod-rtlwifi-pci +rtl8192de-firmware
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl8192de/rtl8192de.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192de)
endef

define KernelPackage/rtl8192cu
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8192CU/RTL8188CU support
  DEPENDS+= +kmod-rtlwifi-usb +kmod-rtl8192c-common +rtl8192cu-firmware
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl8192cu/rtl8192cu.ko
  AUTOLOAD:=$(call AutoProbe,rtl8192cu)
endef

define KernelPackage/rtl8821ae
  $(call KernelPackage/mac80211/Default)
  TITLE:=Realtek RTL8821AE support
  DEPENDS+= +kmod-rtlwifi-btcoexist +kmod-rtlwifi-pci +rtl8821ae-firmware
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtlwifi/rtl8821ae/rtl8821ae.ko
  AUTOLOAD:=$(call AutoProbe,rtl8821ae)
endef

define KernelPackage/rtl8xxxu
  $(call KernelPackage/mac80211/Default)
  TITLE:=alternative Realtek RTL8XXXU support
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-mac80211
  FILES:= $(PKG_BUILD_DIR)/drivers/net/wireless/realtek/rtl8xxxu/rtl8xxxu.ko
  AUTOLOAD:=$(call AutoProbe,rtl8xxxu)
endef

define KernelPackage/rtl8xxxu/description
  This is an alternative driver for various Realtek RTL8XXX
  parts written to utilize the Linux mac80211 stack.
  The driver is known to work with a number of RTL8723AU,
  RL8188CU, RTL8188RU, RTL8191CU, and RTL8192CU devices

  This driver is under development and has a limited feature
  set. In particular it does not yet support 40MHz channels
  and power management. However it should have a smaller
  memory footprint than the vendor drivers and benetifs
  from the in kernel mac80211 stack.

  It can coexist with drivers from drivers/staging/rtl8723au,
  drivers/staging/rtl8192u, and drivers/net/wireless/rtlwifi,
  but you will need to control which module you wish to load.

  RTL8XXXU_UNTESTED is enabled
  This option enables detection of Realtek 8723/8188/8191/8192 WiFi
  USB devices which have not been tested directly by the driver
  author or reported to be working by third parties.

  Please report your results!
endef


define KernelPackage/wlcore
  $(call KernelPackage/mac80211/Default)
  TITLE:=TI common driver part
  DEPENDS+= +kmod-mmc +kmod-mac80211 +@DRIVER_11N_SUPPORT
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
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/wl12xx
  DEPENDS+= +kmod-wlcore +wl12xx-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ti/wl12xx/wl12xx.ko
  AUTOLOAD:=$(call AutoProbe,wl12xx)
endef

define KernelPackage/wl12xx/description
 Kernel modules for TI WL12xx
endef

define KernelPackage/wl18xx
  $(call KernelPackage/mac80211/Default)
  TITLE:=Driver for TI WL18xx
  URL:=https://wireless.wiki.kernel.org/en/users/drivers/wl18xx
  DEPENDS+= +kmod-wlcore +wl18xx-firmware
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/ti/wl18xx/wl18xx.ko
  AUTOLOAD:=$(call AutoProbe,wl18xx)
endef

define KernelPackage/wl18xx/description
 Kernel modules for TI WL18xx
endef


ZD1211FW_NAME:=zd1211-firmware
ZD1211FW_VERSION:=1.4
define Download/zd1211rw
  FILE:=$(ZD1211FW_NAME)-$(ZD1211FW_VERSION).tar.bz2
  URL:=@SF/zd1211/
  HASH:=866308f6f59f7075f075d4959dff2ede47735c751251fecd1496df1ba4d338e1
endef
$(eval $(call Download,zd1211rw))

define KernelPackage/zd1211rw
  $(call KernelPackage/mac80211/Default)
  TITLE:=Zydas ZD1211 support
  DEPENDS+= @USB_SUPPORT +kmod-usb-core +kmod-mac80211
  FILES:=$(PKG_BUILD_DIR)/drivers/net/wireless/zydas/zd1211rw/zd1211rw.ko
  AUTOLOAD:=$(call AutoProbe,zd1211rw)
endef

ifdef CONFIG_PACKAGE_MAC80211_DEBUGFS
  config-y += \
	CFG80211_DEBUGFS \
	MAC80211_DEBUGFS
endif

ifdef CONFIG_PACKAGE_MAC80211_TRACING
  config-y += \
	IWLWIFI_DEVICE_TRACING
endif

config-$(call config_package,lib80211) += LIB80211 LIB80211_CRYPT_WEP LIB80211_CRYPT_CCMP LIB80211_CRYPT_TKIP

config-$(call config_package,airo) += AIRO

config-$(call config_package,mac80211-hwsim) += MAC80211_HWSIM
config-$(call config_package,mt7601u) += MT7601U
config-y += WL_MEDIATEK

config-$(call config_package,iwl-legacy) += IWLEGACY
config-$(call config_package,iwl3945) += IWL3945
config-$(call config_package,iwl4965) += IWL4965
config-$(call config_package,iwlwifi) += IWLWIFI IWLDVM IWLMVM
config-$(CONFIG_PACKAGE_IWLWIFI_DEBUG)+= IWLWIFI_DEBUG
config-$(CONFIG_PACKAGE_IWLWIFI_DEBUGFS)+= IWLWIFI_DEBUGFS

config-$(call config_package,libipw) += LIBIPW
config-$(call config_package,ipw2100) += IPW2100
config-$(call config_package,ipw2200) += IPW2200

config-$(call config_package,p54-common) += P54_COMMON
config-$(call config_package,p54-pci) += P54_PCI
config-$(call config_package,p54-usb) += P54_USB
config-$(call config_package,p54-spi) += P54_SPI

config-$(call config_package,hermes) += HERMES
config-$(call config_package,hermes-pci) += PCI_HERMES
config-$(call config_package,hermes-plx) += PLX_HERMES
config-$(call config_package,hermes-pcmcia) += PCMCIA_HERMES
config-y += HERMES_PRISM

config-$(call config_package,adm8211) += ADM8211
config-$(call config_package,libertas-sdio) += LIBERTAS LIBERTAS_SDIO
config-$(call config_package,libertas-usb) += LIBERTAS LIBERTAS_USB
config-$(call config_package,libertas-spi) += LIBERTAS LIBERTAS_SPI
config-$(call config_package,mwl8k) += MWL8K
config-$(call config_package,mwifiex-pcie) += MWIFIEX MWIFIEX_PCIE
config-$(call config_package,mwifiex-sdio) += MWIFIEX MWIFIEX_SDIO
config-$(call config_package,rtl8180) += RTL8180
config-$(call config_package,rtl8187) += RTL8187
config-$(call config_package,wlcore) += WLCORE WLCORE_SDIO
config-$(call config_package,wl12xx) += WL12XX
config-$(call config_package,wl18xx) += WL18XX
config-y += WL_TI WILINK_PLATFORM_DATA
config-$(call config_package,zd1211rw) += ZD1211RW
config-$(call config_package,rsi91x) += RSI_91X
config-$(call config_package,rsi91x-usb) += RSI_USB
config-$(call config_package,rsi91x-sdio) += RSI_SDIO

config-$(call config_package,rtlwifi) += RTL_CARDS RTLWIFI
config-$(call config_package,rtlwifi-pci) += RTLWIFI_PCI
config-$(call config_package,rtlwifi-btcoexist) += RTLBTCOEXIST
config-$(call config_package,rtlwifi-usb) += RTLWIFI_USB
config-$(call config_package,rtl8192c-common) += RTL8192C_COMMON
config-$(call config_package,rtl8192ce) += RTL8192CE
config-$(call config_package,rtl8192se) += RTL8192SE
config-$(call config_package,rtl8192de) += RTL8192DE
config-$(call config_package,rtl8192cu) += RTL8192CU
config-$(call config_package,rtl8821ae) += RTL8821AE
config-$(CONFIG_PACKAGE_RTLWIFI_DEBUG) += RTLWIFI_DEBUG

config-$(call config_package,rtl8xxxu) += RTL8XXXU
config-y += RTL8XXXU_UNTESTED

config-$(CONFIG_LEDS_TRIGGERS) += MAC80211_LEDS

MAKE_OPTS:= -C "$(PKG_BUILD_DIR)" \
	$(KERNEL_MAKE_FLAGS) \
	EXTRA_CFLAGS="-I$(PKG_BUILD_DIR)/include $(IREMAP_CFLAGS)" \
	KLIB_BUILD="$(LINUX_DIR)" \
	MODPROBE=true \
	KLIB=$(TARGET_MODULES_DIR) \
	KERNEL_SUBLEVEL=$(lastword $(subst ., ,$(KERNEL_PATCHVER))) \
	KBUILD_LDFLAGS_MODULE_PREREQ=

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
		$(PKG_BUILD_DIR)/include/net/ieee80211.h \
		$(PKG_BUILD_DIR)/backport-include/linux/bcm47xx_nvram.h

	echo 'compat-wireless-$(PKG_VERSION)-$(PKG_RELEASE)-$(REVISION)' > $(PKG_BUILD_DIR)/compat_version
endef

ifneq ($(CONFIG_PACKAGE_kmod-cfg80211)$(CONFIG_PACKAGE_kmod-lib80211),)
 define Build/Compile/kmod
	rm -rf $(PKG_BUILD_DIR)/modules
	+$(MAKE) $(PKG_JOBS) $(MAKE_OPTS) modules
 endef
endif

#do not Build/Configure for EXTERNAL KERNEL
ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
  ifeq ($(strip $(CONFIG_KERNEL_GIT_CLONE_URI)),"")
    define Build/Configure
	  cmp $(PKG_BUILD_DIR)/include/linux/ath9k_platform.h $(LINUX_DIR)/include/linux/ath9k_platform.h
	  cmp $(PKG_BUILD_DIR)/include/linux/ath5k_platform.h $(LINUX_DIR)/include/linux/ath5k_platform.h
	  cmp $(PKG_BUILD_DIR)/include/linux/rt2x00_platform.h $(LINUX_DIR)/include/linux/rt2x00_platform.h
    endef
  endif
endif

define Build/Patch
	$(if $(QUILT),rm -rf $(PKG_BUILD_DIR)/patches; mkdir -p $(PKG_BUILD_DIR)/patches)
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/build,build/)
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/subsys,subsys/)
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/ath,ath/)
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/rt2x00,rt2x00/)
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/mwl,mwl/)
	$(call PatchDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/brcm,brcm/)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
endef

define Quilt/Refresh/Package
	$(call Quilt/RefreshDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/build,build/)
	$(call Quilt/RefreshDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/subsys,subsys/)
	$(call Quilt/RefreshDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/ath,ath/)
	$(call Quilt/RefreshDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/rt2x00,rt2x00/)
	$(call Quilt/RefreshDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/mwl,mwl/)
	$(call Quilt/RefreshDir,$(PKG_BUILD_DIR),$(PATCH_DIR)/brcm,brcm/)
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
	rm -f $(1)/usr/include/mac80211-backport/linux/module.h
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

define KernelPackage/cfg80211/install
	$(INSTALL_DIR) $(1)/lib/wifi $(1)/lib/netifd/wireless
	$(INSTALL_DATA) ./files/lib/wifi/mac80211.sh $(1)/lib/wifi
	$(INSTALL_BIN) ./files/lib/netifd/wireless/mac80211.sh $(1)/lib/netifd/wireless
	$(INSTALL_DIR) $(1)/etc/hotplug.d/ieee80211
	$(INSTALL_DATA) ./files/mac80211.hotplug $(1)/etc/hotplug.d/ieee80211/10-wifi-detect
endef

define KernelPackage/ipw2100/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ipw2100-$(IPW2100_VERSION)*.fw $(1)/lib/firmware
endef

define KernelPackage/ipw2200/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(IPW2200_NAME)-$(IPW2200_VERSION)/ipw2200*.fw $(1)/lib/firmware
endef

define KernelPackage/zd1211rw/install
	$(INSTALL_DIR) $(1)/lib/firmware/zd1211
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(ZD1211FW_NAME)/zd1211* $(1)/lib/firmware/zd1211
endef

$(eval $(foreach drv,$(PKG_DRIVERS),$(call KernelPackage,$(drv))))
$(eval $(call KernelPackage,cfg80211))
$(eval $(call KernelPackage,mac80211))
