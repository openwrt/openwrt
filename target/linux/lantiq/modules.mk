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
  DEPENDS+=@(TARGET_lantiq_danube||TARGET_lantiq_ar9) +kmod-usb-core
  KCONFIG:=CONFIG_DWC_OTG \
  	CONFIG_DWC_OTG_DEBUG=n \
	CONFIG_DWC_OTG_LANTIQ=y \
	CONFIG_DWC_OTG_HOST_ONLY=y \
	CONFIG_DWC_OTG_DEVICE_ONLY=n
  FILES:=$(LINUX_DIR)/drivers/usb/dwc_otg/dwc_otg.ko
  AUTOLOAD:=$(call AutoLoad,50,dwc_otg,1)
endef

define KernelPackage/usb-dwc-otg/description
  Kernel support for Synopsis USB on XWAY
endef

$(eval $(call KernelPackage,usb-dwc-otg))

define KernelPackage/usb-ifxhcd
  TITLE:=IFXHCD usb driver
  SUBMENU:=$(USB_MENU)
  DEPENDS+=@(TARGET_lantiq_vr9||TARGET_lantiq_ar9) +kmod-usb-core
ifeq ($(CONFIG_TARGET_lantiq_ar9),)
  KCONFIG:=CONFIG_USB_HOST_IFX \
  	CONFIG_USB_HOST_IFX_B=y \
	CONFIG_IFX_VR9=y \
	CONFIG_IFX_AR9=n \
	CONFIG_USB_HOST_IFX_FORCE_USB11=n \
	CONFIG_USB_HOST_IFX_WITH_HS_ELECT_TST=n \
	CONFIG_USB_HOST_IFX_WITH_ISO=n \
	CONFIG_USB_HOST_IFX_UNALIGNED_ADJ=y
else
  KCONFIG:=CONFIG_USB_HOST_IFX \
  	CONFIG_USB_HOST_IFX_B=y \
	CONFIG_IFX_AR9=y \
	CONFIG_IFX_VR9=n \
	CONFIG_USB_HOST_IFX_FORCE_USB11=n \
	CONFIG_USB_HOST_IFX_WITH_HS_ELECT_TST=n \
	CONFIG_USB_HOST_IFX_WITH_ISO=n \
	CONFIG_USB_HOST_IFX_UNALIGNED_ADJ=y
endif
  FILES:=$(LINUX_DIR)/drivers/usb/ifxhcd/ifxusb_host.ko
  AUTOLOAD:=$(call AutoLoad,50,ifxusb_host)
endef

define KernelPackage/usb-ifxhcd/description
  Kernel support for Synopsis USB on XWAY
endef

$(eval $(call KernelPackage,usb-ifxhcd))

I2C_FALCON_MODULES:= \
  CONFIG_I2C_FALCON:drivers/i2c/busses/i2c-falcon

define KernelPackage/i2c-falcon-lantiq
  TITLE:=Falcon I2C controller
  $(call i2c_defaults,$(I2C_FALCON_MODULES),52)
  DEPENDS:=kmod-i2c-core @(TARGET_lantiq_falcon||TARGET_lantiq_falcon_stable)
endef

define KernelPackage/i2c-falcon-lantiq/description
  Kernel support for the Falcon I2C controller
endef

$(eval $(call KernelPackage,i2c-falcon-lantiq))

