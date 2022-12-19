#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

WIRELESS_MENU:=Wireless Drivers

define KernelPackage/net-prism54
  SUBMENU:=$(WIRELESS_MENU)
  TITLE:=Intersil Prism54 support
  DEPENDS:=@PCI_SUPPORT +@DRIVER_WEXT_SUPPORT +prism54-firmware
  KCONFIG:=CONFIG_PRISM54
  FILES:= \
	$(LINUX_DIR)/drivers/net/wireless/intersil/prism54/prism54.ko
  AUTOLOAD:=$(call AutoProbe,prism54)
endef

define KernelPackage/net-prism54/description
 Kernel modules for Intersil Prism54 support
endef

$(eval $(call KernelPackage,net-prism54))
