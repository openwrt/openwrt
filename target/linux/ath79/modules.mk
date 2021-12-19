LEDS_MENU:=LED modules

define KernelPackage/leds-reset
  SUBMENU:=$(LEDS_MENU)
  TITLE:=reset controller LED support
  DEPENDS:= @TARGET_ath79
  KCONFIG:=CONFIG_LEDS_RESET=m
  FILES:=$(LINUX_DIR)/drivers/leds/leds-reset.ko
  AUTOLOAD:=$(call AutoLoad,60,leds-reset,1)
endef

define KernelPackage/leds-reset/description
 Kernel module for LEDs on reset lines
endef

$(eval $(call KernelPackage,leds-reset))

define KernelPackage/leds-ubnt
  SUBMENU:=$(LEDS_MENU)
  TITLE:=UBNT SPI controller LED support
  DEPENDS:= @TARGET_ath79
  KCONFIG:=CONFIG_LEDS_UBNT_ACB=m
  FILES:=$(LINUX_DIR)/drivers/leds/leds-ubnt-acb.ko
  AUTOLOAD:=$(call AutoLoad,60,leds-ubnt-acb,1)
endef

define KernelPackage/leds-ubnt/description
 Kernel module for LEDs on SPI peripheral MCU
endef

$(eval $(call KernelPackage,leds-ubnt))
