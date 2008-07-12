#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

WIRELESS_MENU:=Wireless Drivers

# NOTE: dependency on 2.6 was removed since it was inherited by kmod-hostap
#       and prevented it from even showing up in menuconfig on 2.4
define KernelPackage/ieee80211
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=802.11 Networking stack
  DEPENDS:=+kmod-crypto-arc4 +kmod-crypto-aes +kmod-crypto-michael-mic
  KCONFIG:= \
	CONFIG_IEEE80211 \
	CONFIG_IEEE80211_CRYPT_WEP \
	CONFIG_IEEE80211_CRYPT_CCMP \
	CONFIG_IEEE80211_CRYPT_TKIP
  FILES:= \
  	$(LINUX_DIR)/net/ieee80211/ieee80211_crypt.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/ieee80211/ieee80211.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/ieee80211/ieee80211_crypt_wep.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/ieee80211/ieee80211_crypt_tkip.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/ieee80211/ieee80211_crypt_ccmp.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,10, \
	ieee80211_crypt \
	ieee80211 \
	ieee80211_crypt_wep \
	ieee80211_crypt_tkip \
	ieee80211_crypt_ccmp \
  )
endef

define KernelPackage/ieee80211/description
 Kernel modules for 802.11 Networking stack
 Includes:
 - ieee80211_crypt
 - ieee80211
 - ieee80211_crypt_wep
 - ieee80211_crypt_tkip
 - ieee80211_crytp_ccmp
endef

$(eval $(call KernelPackage,ieee80211))


define KernelPackage/ieee80211-softmac
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=ieee80211 SoftMAC support
  DEPENDS:=+kmod-ieee80211
  KCONFIG:=CONFIG_IEEE80211_SOFTMAC
  FILES:=$(LINUX_DIR)/net/ieee80211/softmac/ieee80211softmac.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ieee80211softmac)
endef

define KernelPackage/ieee80211-softmac/description
 Kernel modules for 802.11 SoftMAC support
endef

$(eval $(call KernelPackage,ieee80211-softmac))


define KernelPackage/net-bcm43xx
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Broadcom BCM43xx driver
  DEPENDS:=@TARGET_brcm47xx||TARGET_brcm63xx +kmod-ieee80211-softmac
  KCONFIG:=CONFIG_BCM43XX
  FILES:=$(LINUX_DIR)/drivers/net/wireless/bcm43xx/bcm43xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,bcm43xx)
endef

define KernelPackage/net-bcm43xx/description
 Kernel support for Broadcom BCM43xx
 Includes:
 - bcm43xx
endef

$(eval $(call KernelPackage,net-bcm43xx))


define KernelPackage/net-ipw2100
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intel IPW2100 driver
  DEPENDS:=@PCI_SUPPORT +kmod-ieee80211
  KCONFIG:=CONFIG_IPW2100
  FILES:=$(LINUX_DIR)/drivers/net/wireless/ipw2100.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ipw2100)
endef

define KernelPackage/net-ipw2100/description
 Kernel support for Intel IPW2100
 Includes:
 - ipw2100
endef

$(eval $(call KernelPackage,net-ipw2100))


define KernelPackage/net-ipw2200
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intel IPW2200 driver
  DEPENDS:=@PCI_SUPPORT +kmod-ieee80211
  KCONFIG:=CONFIG_IPW2200
  FILES:=$(LINUX_DIR)/drivers/net/wireless/ipw2200.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ipw2200)
endef

define KernelPackage/net-ipw2200/description
 Kernel support for Intel IPW2200
 Includes:
 - ipw2200
endef

$(eval $(call KernelPackage,net-ipw2200))


define KernelPackage/net-airo
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Cisco Aironet driver
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:=CONFIG_AIRO
  FILES:=$(LINUX_DIR)/drivers/net/wireless/airo.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,airo)
endef

define KernelPackage/net-airo/description
 Kernel support for Cisco Aironet cards
endef

$(eval $(call KernelPackage,net-airo))


define KernelPackage/net-hermes
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Hermes 802.11b chipset support
  DEPENDS:=@LINUX_2_6 @PCI_SUPPORT||PCMCIA_SUPPORT
  KCONFIG:=CONFIG_HERMES
  FILES:= \
	$(LINUX_DIR)/drivers/net/wireless/hermes.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/wireless/orinoco.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,hermes orinoco)
endef

define KernelPackage/net-hermes/description
 Kernel support for Hermes 802.11b chipsets
endef

$(eval $(call KernelPackage,net-hermes))


define KernelPackage/net-hermes-pci
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intersil Prism 2.5 PCI support
  DEPENDS:=@PCI_SUPPORT kmod-net-hermes
  KCONFIG:=CONFIG_PCI_HERMES
  FILES:=$(LINUX_DIR)/drivers/net/wireless/orinoco_pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,orinoco_pci)
endef

define KernelPackage/net-hermes-pci/description
 Kernel modules for Intersil Prism 2.5 PCI support
endef

$(eval $(call KernelPackage,net-hermes-pci))


define KernelPackage/net-hermes-plx
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=PLX9052 based PCI adaptor
  DEPENDS:=@PCI_SUPPORT kmod-net-hermes
  KCONFIG:=CONFIG_PLX_HERMES
  FILES:=$(LINUX_DIR)/drivers/net/wireless/orinoco_plx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,orinoco_plx)
endef

define KernelPackage/net-hermes-plx/description
 Kernel modules for Hermes in PLX9052 based PCI adaptors
endef

$(eval $(call KernelPackage,net-hermes-plx))


define KernelPackage/net-prism54
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intersil Prism54 support
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:=CONFIG_PRISM54
  FILES:=$(LINUX_DIR)/drivers/net/wireless/prism54/prism54.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,prism54)
endef

define KernelPackage/net-prism54/description
 Kernel modules for Intersil Prism54 support
endef

$(eval $(call KernelPackage,net-prism54))

