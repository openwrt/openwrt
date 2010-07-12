#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/ata-octeon-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Octeon Compact Flash support
  DEPENDS:=@TARGET_octeon
  KCONFIG:=CONFIG_PATA_OCTEON_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_octeon_cf.ko
  AUTOLOAD:=$(call AutoLoad,41,pata_octeon_cf,1)
  $(call AddDepends/ata)
endef

define KernelPackage/ata-octeon-cf/description
  Octeon Compact Flash support.
endef

$(eval $(call KernelPackage,ata-octeon-cf))


define KernelPackage/usb-octeon
  SUBMENU:=$(USB_MENU)
  TITLE:=Support for the Octeon USB OTG controller
  DEPENDS:=@TARGET_octeon
  KCONFIG:=CONFIG_USB_DWC_OTG
  FILES:=$(LINUX_DIR)/drivers/usb/host/dwc_otg/dwc_otg.ko
  AUTOLOAD:=$(call AutoLoad,50,dwc_otg)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-octeon/description
  Kernel support for the Octeon USB host controller
endef

$(eval $(call KernelPackage,usb-octeon))
