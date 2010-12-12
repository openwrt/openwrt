#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/lantiq-deu
  TITLE:=Lantiq data encryption unit
  SUBMENU:=$(CRYPTO_MENU)
  DEPENDS:=@TARGET_lantiq
  KCONFIG:=CONFIG_CRYPTO_DEV_LANTIQ \
	   CONFIG_CRYPTO_HW=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_AES=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_DES=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_MD5=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_SHA1=y
  $(call AddDepends/crypto)
endef

define KernelPackage/lantiq-deu/description
  Kernel support for the Lantiq crypto HW
endef

$(eval $(call KernelPackage,lantiq-deu))

USB_MENU:=USB Support

define KernelPackage/usb-dwc-otg
  TITLE:=Synopsis DWC_OTG support
  SUBMENU:=$(USB_MENU)
  DEPENDS+=@TARGET_lantiq_xway +kmod-usb-core
  KCONFIG:=CONFIG_DWC_OTG \
  	CONFIG_DWC_OTG_DEBUG=n \
	CONFIG_DWC_OTG_LANTIQ=y \
	CONFIG_DWC_OTG_HOST_ONLY=y
  FILES:=$(LINUX_DIR)/drivers/usb/dwc_otg/dwc_otg.ko
  AUTOLOAD:=$(call AutoLoad,50,dwc_otg)
endef

define KernelPackage/usb-dwc-otg/description
  Kernel support for Synopsis USB on XWAY
endef

$(eval $(call KernelPackage,usb-dwc-otg))


