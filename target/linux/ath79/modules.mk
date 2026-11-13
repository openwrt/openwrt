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


define KernelPackage/gpio-ssr-mikrotik
  SUBMENU:=GPIO support
  TITLE:=MikroTik RouterBOARD SPI shift register GPIO support
  DEPENDS:=@TARGET_ath79_mikrotik
  KCONFIG:=CONFIG_GPIO_SSR_MIKROTIK
  FILES:=$(LINUX_DIR)/drivers/gpio/gpio-ssr-mikrotik.ko
  AUTOLOAD:=$(call AutoLoad,30,gpio-ssr-mikrotik,1)
endef

define KernelPackage/gpio-ssr-mikrotik/description
 GPIO driver for the 74HC595 shift register that shares the SPI bus with
 the boot flash on some MikroTik RouterBOARDs, such as the RB953GS.
endef

$(eval $(call KernelPackage,gpio-ssr-mikrotik))


define KernelPackage/nvmem-layout-mikrotik
  SUBMENU:=Other modules
  TITLE:=MikroTik RouterBOARD hard_config NVMEM layout
  DEPENDS:=@TARGET_ath79_mikrotik
  KCONFIG:=CONFIG_NVMEM_LAYOUT_MIKROTIK
  FILES:=$(LINUX_DIR)/drivers/platform/mikrotik/rb_nvmem.ko
  AUTOLOAD:=$(call AutoLoad,25,rb_nvmem,1)
endef

define KernelPackage/nvmem-layout-mikrotik/description
 Exposes the MikroTik RouterBOOT hard_config tags as NVMEM cells.
endef

$(eval $(call KernelPackage,nvmem-layout-mikrotik))


define KernelPackage/hwmon-mikrotik-555
  SUBMENU:=Hardware Monitoring Support
  TITLE:=MikroTik RouterBOARD 555 timer health monitor
  DEPENDS:=@TARGET_ath79_mikrotik +kmod-hwmon-core
  KCONFIG:=CONFIG_SENSORS_MIKROTIK_555
  FILES:=$(LINUX_DIR)/drivers/hwmon/mikrotik-555-hwmon.ko
  AUTOLOAD:=$(call AutoProbe,mikrotik-555-hwmon)
endef

define KernelPackage/hwmon-mikrotik-555/description
 Input voltage and PCB temperature monitor of some MikroTik RouterBOARDs,
 such as the RB953GS, which is a 555 timer sampled on a GPIO.
endef

$(eval $(call KernelPackage,hwmon-mikrotik-555))
