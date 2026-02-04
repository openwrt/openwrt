# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/leds-bcm6328
  SUBMENU:=$(LEDS_MENU)
  TITLE:=BCM6328 LED support
  KCONFIG:=CONFIG_LEDS_BCM6328
  FILES:=$(LINUX_DIR)/drivers/leds/leds-bcm6328.ko
  DEPENDS:=@TARGET_bmips
  AUTOLOAD:=$(call AutoLoad,60,leds-bcm6328,1)
endef

define KernelPackage/leds-bcm6328/description
  Kernel support for the BCM6328 LED controller.
endef

$(eval $(call KernelPackage,leds-bcm6328))

define KernelPackage/leds-bcm6358
  SUBMENU:=$(LEDS_MENU)
  TITLE:=BCM6358 LED support
  KCONFIG:=CONFIG_LEDS_BCM6358
  FILES:=$(LINUX_DIR)/drivers/leds/leds-bcm6358.ko
  DEPENDS:=@TARGET_bmips
  AUTOLOAD:=$(call AutoLoad,60,leds-bcm6358,1)
endef

define KernelPackage/leds-bcm6358/description
  Kernel support for the BCM6358 LED controller.
endef

$(eval $(call KernelPackage,leds-bcm6358))

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
