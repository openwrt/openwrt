# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2024 Bootlin

define KernelPackage/phy-stm32-usbphyc
  SUBMENU:=$(USB_MENU)
  TITLE:=STM32 USB HS PHY Controller driver
  KCONFIG:=CONFIG_PHY_STM32_USBPHYC
  DEPENDS:=@TARGET_stm32
  FILES:=$(LINUX_DIR)/drivers/phy/st/phy-stm32-usbphyc.ko
  AUTOLOAD:=$(call AutoProbe,phy-stm32-usbphyc,)
endef

define KernelPackage/phy-stm32-usbphyc/description
  Kernel module for STM32 USB HS PHY Controller
endef

$(eval $(call KernelPackage,phy-stm32-usbphyc))
