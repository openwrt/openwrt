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

I2C_OCTEON_MODULES:=\
  CONFIG_I2C_OCTEON:drivers/i2c/busses/i2c-octeon

define KernelPackage/octeon-i2c
  $(call i2c_defaults,$(I2C_OCTEON_MODULES),59)
  TITLE:=I2C master driver for Cavium Octeon
  DEPENDS:=@TARGET_octeon +kmod-i2c-core
endef

define KernelPackage/octeon-i2c/description
  Kernel module to use the I2C master driver on Cavium Octeon
endef

$(eval $(call KernelPackage,octeon-i2c))

