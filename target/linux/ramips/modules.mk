#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/usb-rt305x-dwc_otg
  TITLE:=RT305X USB controller driver
  DEPENDS:=@TARGET_ramips_rt305x
  KCONFIG:= \
	CONFIG_DWC_OTG \
	CONFIG_DWC_OTG_HOST_ONLY=y \
	CONFIG_DWC_OTG_DEVICE_ONLY=n \
	CONFIG_DWC_OTG_DEBUG=n
  FILES:=$(LINUX_DIR)/drivers/usb/dwc_otg/dwc_otg.ko
  AUTOLOAD:=$(call AutoLoad,54,dwc_otg,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-rt305x-dwc_otg/description
  This driver provides USB Device Controller support for the
  Synopsys DesignWare USB OTG Core used in the Ralink RT305X SoCs.
endef

$(eval $(call KernelPackage,usb-rt305x-dwc_otg))
