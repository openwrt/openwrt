# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gl-e750-mcu
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Serv Pol <servpol@w420.ru>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gl-e750-mcu
  SUBMENU:=Other modules
  TITLE:=GL.iNet GL-E750 status MCU (OLED and battery)
  DEPENDS:=@TARGET_ath79_nand
  KCONFIG:= \
	CONFIG_POWER_SUPPLY=y \
	CONFIG_SERIAL_DEV_BUS=y \
	CONFIG_SERIAL_DEV_CTRL_TTYPORT=y
  FILES:=$(PKG_BUILD_DIR)/gl-e750-mcu.ko
  AUTOLOAD:=$(call AutoProbe,gl-e750-mcu)
endef

define KernelPackage/gl-e750-mcu/description
  Driver for the status MCU of the GL.iNet GL-E750 (Mudi), attached to the
  SoC UART. It exposes the battery through the power_supply class and
  accepts the OLED screen content as a JSON object on /dev/gl-e750-mcu.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,gl-e750-mcu))
