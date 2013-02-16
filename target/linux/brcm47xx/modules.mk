#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/usb-brcm47xx
  SUBMENU:=$(USB_MENU)
  TITLE:=Support for USB on bcm47xx
  DEPENDS:=@USB_SUPPORT @TARGET_brcm47xx
  KCONFIG:= \
  	CONFIG_USB_HCD_BCMA \
  	CONFIG_USB_HCD_SSB
  FILES:= \
  	$(LINUX_DIR)/drivers/usb/host/bcma-hcd.ko \
  	$(LINUX_DIR)/drivers/usb/host/ssb-hcd.ko
  AUTOLOAD:=$(call AutoLoad,19,bcma-hcd ssb-hcd,1)
  $(call AddDepends/usb)
endef

$(eval $(call KernelPackage,usb-brcm47xx))


define KernelPackage/ocf-ubsec-ssb
  TITLE:=BCM5365P IPSec Core driver
  DEPENDS:=@TARGET_brcm47xx +kmod-crypto-ocf
  KCONFIG:=CONFIG_OCF_UBSEC_SSB
  FILES:=$(LINUX_DIR)/crypto/ocf/ubsec_ssb/ubsec_ssb.ko
  AUTOLOAD:=$(call AutoLoad,10,ubsec_ssb)
  $(call AddDepends/crypto)
endef

define KernelPackage/ocf-ubsec-ssb/description
  This package contains the OCF driver for the BCM5365p IPSec Core
endef

$(eval $(call KernelPackage,ocf-ubsec-ssb))

define KernelPackage/bgmac
  TITLE:=Broadcom bgmac driver
  KCONFIG:=CONFIG_BGMAC
  DEPENDS:=@TARGET_brcm47xx
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/broadcom/bgmac.ko
  AUTOLOAD:=$(call AutoLoad,19,bgmac)
endef

define KernelPackage/bgmac/description
 Kernel modules for Broadcom bgmac Ethernet adapters.
endef

$(eval $(call KernelPackage,bgmac))
