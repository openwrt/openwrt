# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/leds-sercomm-msp430
  SUBMENU:=$(LEDS_MENU)
  TITLE:=Sercomm MSP430G2513 LED support
  KCONFIG:=CONFIG_LEDS_SERCOMM_MSP430
  FILES:=$(LINUX_DIR)/drivers/leds/leds-sercomm-msp430.ko
  DEPENDS:=@TARGET_bmips +kmod-ledtrig-pattern
  AUTOLOAD:=$(call AutoLoad,60,leds-sercomm-msp430,1)
endef

define KernelPackage/leds-sercomm-msp430/description
  Kernel support for the Sercomm MSP430G2513 SPI LED controller.
endef

$(eval $(call KernelPackage,leds-sercomm-msp430))
