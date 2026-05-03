#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PSE_MENU:=PSE-PD / PoE support

define KernelPackage/pse-pd
  SUBMENU:=$(PSE_MENU)
  TITLE:=PSE-PD Support
  DEPENDS:=@REGULATOR_SUPPORT @!SMALL_FLASH
  KCONFIG:=CONFIG_PSE_CONTROLLER=y
endef

define KernelPackage/pse-pd/description
 Kernel module for PSE-PD support.
endef

$(eval $(call KernelPackage,pse-pd))

define AddDepends/pse-pd
  SUBMENU:=$(PSE_MENU)
  DEPENDS+=+kmod-pse-pd $(1)
endef

define KernelPackage/pse-regulator
  SUBMENU:=$(PSE_MENU)
  TITLE:=Regulator based PSE controller support
  KCONFIG:=CONFIG_PSE_REGULATOR
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/pse_regulator.ko
  AUTOLOAD:=$(call AutoProbe,pse_regulator)
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-regulator/description
 This module provides support for simple regulator based Ethernet Power \
 Sourcing Equipment without automatic classification support. For \
 example for basic implementation of PoDL (802.3bu) specification.
endef

$(eval $(call KernelPackage,pse-regulator))

define KernelPackage/pse-pd692x0
  SUBMENU:=$(PSE_MENU)
  TITLE:=PD692X0 PSE controller support
  KCONFIG:=CONFIG_PSE_PD692X0
  DEPENDS:=+kmod-i2c-core
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/pd692x0.ko
  AUTOLOAD:=$(call AutoProbe,pd692x0)
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-pd692x0/description
 Kernel module for PD692X0 PSE controller chips
endef

$(eval $(call KernelPackage,pse-pd692x0))

define KernelPackage/pse-si3474
  SUBMENU:=$(PSE_MENU)
  TITLE:=Si3474 PSE controller support
  KCONFIG:=CONFIG_PSE_SI3474
  DEPENDS:=+kmod-i2c-core
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/si3474.ko
  AUTOLOAD:=$(call AutoProbe,si3474)
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-si3474/description
 Kernel module for Si3474 PSE controller chips
endef

$(eval $(call KernelPackage,pse-si3474))

define KernelPackage/pse-tps23881
  SUBMENU:=$(PSE_MENU)
  TITLE:=TPS23881 PSE controller support
  KCONFIG:=CONFIG_PSE_TPS23881
  DEPENDS:=+kmod-i2c-core
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/tps23881.ko
  AUTOLOAD:=$(call AutoProbe,tps23881)
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-tps23881/description
 Kernel module for TPS23881 PSE controller chips
endef

$(eval $(call KernelPackage,pse-tps23881))

define KernelPackage/pse-realtek-mcu
  SUBMENU:=$(PSE_MENU)
  TITLE:=Realtek/Broadcom PSE MCU driver (core)
  KCONFIG:=CONFIG_PSE_REALTEK_MCU
  DEPENDS:=@TARGET_realtek
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/realtek-pse-mcu-core.ko
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-realtek-mcu/description
 Library module shared by the drivers for the Realtek/Broadcom PSE MCU
 (kmod-pse-realtek-mcu-i2c, kmod-pse-realtek-mcu-uart).
 Pulled in automatically when one of the transport modules is selected.
endef

$(eval $(call KernelPackage,pse-realtek-mcu))

define KernelPackage/pse-realtek-mcu-i2c
  SUBMENU:=$(PSE_MENU)
  TITLE:=Realtek/Broadcom PSE MCU driver (I2C transport)
  KCONFIG:=CONFIG_PSE_REALTEK_MCU_I2C
  DEPENDS:=@TARGET_realtek +kmod-i2c-core +kmod-pse-realtek-mcu
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/realtek-pse-mcu-i2c.ko
  AUTOLOAD:=$(call AutoProbe,realtek-pse-mcu-i2c)
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-realtek-mcu-i2c/description
 Realtek/Broadcom PSE MCU driver, widely used on Realtek-based switches.
 Attached via I2C/SMBus.
endef

$(eval $(call KernelPackage,pse-realtek-mcu-i2c))

define KernelPackage/pse-realtek-mcu-uart
  SUBMENU:=$(PSE_MENU)
  TITLE:=Realtek/Broadcom PSE MCU driver (UART transport)
  KCONFIG:= \
	CONFIG_PSE_REALTEK_MCU_UART \
	CONFIG_SERIAL_DEV_BUS=y \
	CONFIG_SERIAL_DEV_CTRL_TTYPORT=y
  DEPENDS:=@TARGET_realtek +kmod-pse-realtek-mcu
  FILES:=$(LINUX_DIR)/drivers/net/pse-pd/realtek-pse-mcu-uart.ko
  AUTOLOAD:=$(call AutoProbe,realtek-pse-mcu-uart)
  $(call AddDepends/pse-pd)
endef

define KernelPackage/pse-realtek-mcu-uart/description
 Realtek/Broadcom PSE MCU driver, widely used on Realtek-based switches.
 Attached via UART.
endef

$(eval $(call KernelPackage,pse-realtek-mcu-uart))
