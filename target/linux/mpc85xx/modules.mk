define KernelPackage/leds-turris-1x
  SUBMENU:=$(LEDS_MENU)
  TITLE:=LED support for CZ.NIC's Turris 1.x
  DEPENDS:=@TARGET_mpc85xx_p2020
  KCONFIG:=CONFIG_LEDS_TURRIS_1X
  FILES:=$(LINUX_DIR)/drivers/leds/leds-turris-1x.ko
  AUTOLOAD:=$(call AutoProbe,leds-turris-1x)
endef

define KernelPackage/leds-turris-1x/description
  This option enables support for the RGB LEDs on the front panel of the
  CZ.NIC Turris 1.x routers, which are driven by the on-board CPLD.
endef

$(eval $(call KernelPackage,leds-turris-1x))
