# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

WIMENU:=Wireless Drivers

define KernelPackage/ieee80211
  TITLE:=802.11 Networking stack
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- ieee80211_crypt \\\
	- ieee80211 \\\
	- ieee80211_crypt_wep \\\
	- ieee80211_crypt_tkip \\\
	- ieee80211_crytp_ccmp \\\
  DEPENDS:=@LINUX_2_6 kmod-crypto
  KCONFIG:=$(CONFIG_IEEE80211)
  FILES:= \
  	$(MODULES_DIR)/kernel/net/ieee80211/*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,10, \
	ieee80211_crypt \
	ieee80211 \
	ieee80211_crypt_wep \
	ieee80211_crypt_tkip \
	ieee80211_crypt_ccmp \
  )
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,ieee80211))


define KernelPackage/ieee80211-softmac
  TITLE:=ieee80211 SoftMAC support
  DEPENDS:=kmod-ieee80211
  KCONFIG:=$(CONFIG_IEEE80211_SOFTMAC)
  FILES:= \
	$(MODULES_DIR)/kernel/net/ieee80211/softmac/*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ieee80211softmac)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,ieee80211-softmac))


define KernelPackage/net-bcm43xx
  TITLE:=Broadcom BCM43xx driver
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- bcm43xx
  DEPENDS:=kmod-ieee80211-softmac
  KCONFIG:=$(CONFIG_BCM43XX)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/bcm43xx/bcm43xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,bcm43xx)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,net-bcm43xx))

define KernelPackage/net-ipw2100
  TITLE:=Intel IPW2100 driver
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- ipw2100
  DEPENDS:=kmod-ieee80211
  KCONFIG:=$(CONFIG_IPW2100)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/ipw2100.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ipw2100)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,net-ipw2100))

define KernelPackage/net-ipw2200
  TITLE:=Intel IPW2200 driver
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- ipw2200
  DEPENDS:=kmod-ieee80211
  KCONFIG:=$(CONFIG_IPW2200)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/ipw2200.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ipw2200)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,net-ipw2200))


define KernelPackage/net-airo
  TITLE:=Cisco Aironet driver
  DESCRIPTION:=Driver for Cisco Aironet cards
  KCONFIG:=$(CONFIG_AIRO)
  SUBMENU:=$(WIMENU)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/airo.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,airo)
endef
$(eval $(call KernelPackage,net-airo))


define KernelPackage/net-hermes
  TITLE:=Hermes 802.11b chipset support
  DESCRIPTION:=Kernel support for Hermes 802.11b chipsets
  KCONFIG:=$(CONFIG_HERMES)
  SUBMENU:=$(WIMENU)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/net/wireless/hermes.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,hermes orinoco)
endef
$(eval $(call KernelPackage,net-hermes))


define KernelPackage/net-hermes-pci
  TITLE:=Intersil Prism 2.5 PCI support
  DESCRIPTION:=Kernel modules for Intersil Prism 2.5 PCI support
  KCONFIG:=$(CONFIG_PCI_HERMES)
  DEPENDS:=kmod-net-hermes
  SUBMENU:=$(WIMENU)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco_pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,orinoco_pci)
endef
$(eval $(call KernelPackage,net-hermes-pci))


define KernelPackage/net-hermes-plx
  TITLE:=PLX9052 based PCI adaptor
  DESCRIPTION:=Kernel modules for Hermes in PLX9052 based PCI adaptors
  KCONFIG:=$(CONFIG_PLX_HERMES)
  DEPENDS:=kmod-net-hermes
  SUBMENU:=$(WIMENU)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco_plx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,orinoco_plx)
endef
$(eval $(call KernelPackage,net-hermes-plx))


define KernelPackage/net-prism54
  TITLE:=Intersil Prism54 support
  DESCRIPTION:=Kernel modules for Intersil Prism54 support
  KCONFIG:=$(CONFIG_PRISM54)
  SUBMENU:=$(WIMENU)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/prism54/prism54.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,prism54)
endef
$(eval $(call KernelPackage,net-prism54))


