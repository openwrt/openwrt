#
# Copyright (C) 2103 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/usb2-cns21xx
  TITLE:=Support for the built-in EHCI controller of the CNS21xx SoCs
  DEPENDS:=@TARGET_cns21xx +kmod-usb2
  KCONFIG:=CONFIG_USB_EHCI_CNS21XX
  FILES:=$(LINUX_DIR)/drivers/usb/host/ehci-cns21xx.ko
  AUTOLOAD:=$(call AutoLoad,41,ehci-cns21xx,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb2-cns21xx/description
 Kernel support for the built-in EHCI controller of the CNS21xx SoCs.
endef

$(eval $(call KernelPackage,usb2-cns21xx))


