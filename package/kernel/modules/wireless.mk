#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

WIRELESS_MENU:=Wireless Drivers

define KernelPackage/lib80211
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=802.11 Networking stack
  KCONFIG:= \
	CONFIG_LIB80211 \
	CONFIG_LIB80211_CRYPT_WEP \
	CONFIG_LIB80211_CRYPT_TKIP \
	CONFIG_LIB80211_CRYPT_CCMP
  FILES:= \
  	$(LINUX_DIR)/net/wireless/lib80211.ko \
  	$(LINUX_DIR)/net/wireless/lib80211_crypt_wep.ko \
  	$(LINUX_DIR)/net/wireless/lib80211_crypt_ccmp.ko \
  	$(LINUX_DIR)/net/wireless/lib80211_crypt_tkip.ko
  AUTOLOAD:=$(call AutoLoad,10, \
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

$(eval $(call KernelPackage,lib80211))

define KernelPackage/net-airo
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Cisco Aironet driver
  DEPENDS:=@PCI_SUPPORT +@DRIVER_WEXT_SUPPORT
  KCONFIG:=CONFIG_AIRO
  FILES:=$(LINUX_DIR)/drivers/net/wireless/airo.ko
  AUTOLOAD:=$(call AutoLoad,50,airo)
endef

define KernelPackage/net-airo/description
 Kernel support for Cisco Aironet cards
endef

$(eval $(call KernelPackage,net-airo))


define KernelPackage/net-zd1201
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Zydas ZD1201 support
  DEPENDS:=@USB_SUPPORT +@DRIVER_WEXT_SUPPORT
  KCONFIG:=CONFIG_USB_ZD1201
  FILES:=$(LINUX_DIR)/drivers/net/wireless/zd1201.ko
  AUTOLOAD:=$(call AutoLoad,60,zd1201)
endef

define KernelPackage/net-zd1201/description
 Kernel modules for Zydas ZD1201 support
 Devices using this chip:
   * Sweex LC100020
   * Zyxel ZyAir B-220
   * Peabird USB
   * Gigafast WF741-UIC
   * E-Tech Wireless USB Adapter
   * DSE 802.11b USB wireless LAN adapter
   * CC and C WLAN USB Adapter (WL 1202)
   * Edimax EW-7117U
   * X-Micro WLAN 11b USB Adapter
   * Belkin F5D6051
   * Topcom SKYR@CER WIRELESS USB STICK 11
   * Surecom EP-9001
   * JAHT WN-1011U
   * BeWAN Wi-Fi USB 11
   * NorthQ NQ9000
   * MSI UB11B
   * Origo WLL-1610
   * Longshine LCS-8131R
   * Gigabyte GN-WLBZ201
endef

ZD1201FW_NAME:=zd1201
ZD1201FW_VERSION:=0.14
ZD1201FW_DIR:=$(ZD1201FW_NAME)-$(ZD1201FW_VERSION)-fw
ZD1201FW_FILE:=$(ZD1201FW_DIR).tar.gz

define Download/net-zd1201
  FILE:=$(ZD1201FW_FILE)
  #http://downloads.sourceforge.net/project/linux-lc100020/%28NEW%29%20zd1201%20driver/zd1201.%20Version%200.14/zd1201-0.14-fw.tar.gz
  URL:=@SF/linux-lc100020/\(NEW\)\ $(ZD1201FW_NAME)\ driver/$(ZD1201FW_NAME).\ Version\ $(ZD1201FW_VERSION)/
  MD5SUM:=07a4febc365121f975e2c5e59791d55d
endef

define KernelPackage/net-zd1201/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(TAR) -C $(1)/lib/firmware -zxf $(DL_DIR)/$(ZD1201FW_FILE) --strip-components=1 $(ZD1201FW_DIR)/$(ZD1201FW_NAME).fw $(ZD1201FW_DIR)/$(ZD1201FW_NAME)-ap.fw
endef

$(eval $(call Download,net-zd1201))
$(eval $(call KernelPackage,net-zd1201))


define KernelPackage/net-prism54
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intersil Prism54 support
  DEPENDS:=@PCI_SUPPORT +@DRIVER_WEXT_SUPPORT
  KCONFIG:=CONFIG_PRISM54
  FILES:=$(LINUX_DIR)/drivers/net/wireless/prism54/prism54.ko
  AUTOLOAD:=$(call AutoLoad,60,prism54)
endef

define KernelPackage/net-prism54/description
 Kernel modules for Intersil Prism54 support
endef

# Prism54 FullMAC firmware (jbnore.free.fr seems to be rather slow, so we use daemonizer.de)
PRISM54_FW:=1.0.4.3.arm

define Download/net-prism54
  FILE:=$(PRISM54_FW)
  URL:=http://daemonizer.de/prism54/prism54-fw/fw-fullmac/
  MD5SUM:=8bd4310971772a486b9784c77f8a6df9
endef

define KernelPackage/net-prism54/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PRISM54_FW) $(1)/lib/firmware/isl3890
endef

$(eval $(call Download,net-prism54))
$(eval $(call KernelPackage,net-prism54))

