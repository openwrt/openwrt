#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/usb-etrax
  SUBMENU:=$(USB_MENU)
  TITLE:=Support for the ETRAX USB host controller
  DEPENDS:=@TARGET_etrax
  KCONFIG:=CONFIG_ETRAX_USB_HOST \
	CONFIG_ETRAX_USB_HOST_PORT1=y CONFIG_ETRAX_USB_HOST_PORT2=y
  FILES:=$(LINUX_DIR)/drivers/usb/host/hc-crisv10.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,hc-crisv10,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-etrax/description
 Kernel support for the ETRAX USB host controller
endef

$(eval $(call KernelPackage,usb-etrax))
