#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

WIRELESS_MENU:=Wireless Drivers

# NOTE: dependency on 2.6 was removed since it was inherited by kmod-hostap
#       and prevented it from even showing up in menuconfig on 2.4
define KernelPackage/ieee80211
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=802.11 Networking stack
  DEPENDS:=+kmod-crypto-arc4 +kmod-crypto-aes +kmod-crypto-michael-mic @LINUX_2_4||@LINUX_2_6_21||LINUX_2_6_23||LINUX_2_6_24||LINUX_2_6_25||LINUX_2_6_26||LINUX_2_6_28
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


define KernelPackage/lib80211
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=802.11 Networking stack
  DEPENDS:=@LINUX_2_6_30||LINUX_2_6_31
  KCONFIG:= \
	CONFIG_LIB80211 \
	CONFIG_LIB80211_CRYPT_WEP \
	CONFIG_LIB80211_CRYPT_TKIP \
	CONFIG_LIB80211_CRYPT_CCMP
  FILES:= \
  	$(LINUX_DIR)/net/wireless/lib80211.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/wireless/lib80211_crypt_wep.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/wireless/lib80211_crypt_ccmp.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/net/wireless/lib80211_crypt_tkip.$(LINUX_KMOD_SUFFIX)
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


ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.29)),1)
  IPW_DIR:=ipw2x00/
endif

define KernelPackage/net-libipw
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=libipw for ipw2100 and ipw2200
  DEPENDS:=@PCI_SUPPORT +kmod-crypto-arc4 +kmod-crypto-aes +kmod-crypto-michael-mic +kmod-lib80211 @LINUX_2_6_30||LINUX_2_6_31
  KCONFIG:=CONFIG_LIBIPW
  FILES:=$(LINUX_DIR)/drivers/net/wireless/$(IPW_DIR)libipw.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,49,libipw)
endef

define KernelPackage/net-libipw/description
 Hardware independent IEEE 802.11 networking stack for ipw2100 and ipw2200.
endef

$(eval $(call KernelPackage,net-libipw))


define KernelPackage/net-ipw2100
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intel IPW2100 driver
  DEPENDS:=@PCI_SUPPORT +!(LINUX_2_6_30||LINUX_2_6_31):kmod-ieee80211 +LINUX_2_6_30||LINUX_2_6_31:kmod-net-libipw
  KCONFIG:=CONFIG_IPW2100
  FILES:=$(LINUX_DIR)/drivers/net/wireless/$(IPW_DIR)ipw2100.$(LINUX_KMOD_SUFFIX)
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
  DEPENDS:=@PCI_SUPPORT +!(LINUX_2_6_30||LINUX_2_6_31):kmod-ieee80211 +LINUX_2_6_30||LINUX_2_6_31:kmod-net-libipw
  KCONFIG:=CONFIG_IPW2200
  FILES:=$(LINUX_DIR)/drivers/net/wireless/$(IPW_DIR)ipw2200.$(LINUX_KMOD_SUFFIX)
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


ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.29)),1)
  ORINOCO_DIR:=orinoco/
endif

define KernelPackage/net-hermes
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Hermes 802.11b chipset support
  DEPENDS:=@LINUX_2_6 @PCI_SUPPORT||PCMCIA_SUPPORT
  KCONFIG:=CONFIG_HERMES \
	CONFIG_HERMES_CACHE_FW_ON_INIT=n
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.30)),1)
  FILES:= \
	$(LINUX_DIR)/drivers/net/wireless/$(ORINOCO_DIR)orinoco.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,orinoco)
else
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.28)),1)
  FILES:= \
	$(LINUX_DIR)/drivers/net/wireless/$(ORINOCO_DIR)hermes.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/wireless/$(ORINOCO_DIR)orinoco.$(LINUX_KMOD_SUFFIX) \
    $(LINUX_DIR)/drivers/net/wireless/$(ORINOCO_DIR)hermes_dld.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,hermes hermes_dld orinoco)
else
  FILES:= \
	$(LINUX_DIR)/drivers/net/wireless/hermes.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/wireless/orinoco.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,hermes orinoco)
endif
endif
endef

define KernelPackage/net-hermes/description
 Kernel support for Hermes 802.11b chipsets
endef

$(eval $(call KernelPackage,net-hermes))


define KernelPackage/net-hermes-pci
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intersil Prism 2.5 PCI support
  DEPENDS:=@PCI_SUPPORT +kmod-net-hermes
  KCONFIG:=CONFIG_PCI_HERMES
  FILES:=$(LINUX_DIR)/drivers/net/wireless/$(ORINOCO_DIR)orinoco_pci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,orinoco_pci)
endef

define KernelPackage/net-hermes-pci/description
 Kernel modules for Intersil Prism 2.5 PCI support
endef

$(eval $(call KernelPackage,net-hermes-pci))


define KernelPackage/net-hermes-plx
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=PLX9052 based PCI adaptor
  DEPENDS:=@PCI_SUPPORT +kmod-net-hermes
  KCONFIG:=CONFIG_PLX_HERMES
  FILES:=$(LINUX_DIR)/drivers/net/wireless/$(ORINOCO_DIR)orinoco_plx.$(LINUX_KMOD_SUFFIX)
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

