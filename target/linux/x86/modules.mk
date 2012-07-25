#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/cs5535
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=NSC/AMD CS5535 chipset support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_BLK_DEV_CS5535
  FILES=$(LINUX_DIR)/drivers/ide/cs5535.ko
  AUTOLOAD:=$(call AutoLoad,30,cs5535,1)
  $(call AddDepends/ide)
endef

define KernelPackage/cs5535/description
  Kernel module for the NSC/AMD CS5535 companion chip
endef

$(eval $(call KernelPackage,cs5535))


define KernelPackage/cs5536
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=AMD CS5536 chipset support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_BLK_DEV_CS5536
  FILES=$(LINUX_DIR)/drivers/ide/cs5536.ko
  AUTOLOAD:=$(call AutoLoad,30,cs5536,1)
  $(call AddDepends/ide)
endef

define KernelPackage/cs5536/description
  Kernel module for the AMD CS5536 Geode LX companion chip
endef

$(eval $(call KernelPackage,cs5536))


define KernelPackage/pata-cs5535
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=CS5535 PATA support
  DEPENDS:=@TARGET_x86 @PCI_SUPPORT
  KCONFIG:=CONFIG_PATA_CS5535
  FILES=$(LINUX_DIR)/drivers/ata/pata_cs5535.ko
  AUTOLOAD:=$(call AutoLoad,30,pata_cs5535,1)
  $(call AddDepends/ata)
endef

define KernelPackage/pata-cs5535/description
  Kernel module for the NSC/AMD CS5535 companion chip
endef

$(eval $(call KernelPackage,pata-cs5535))


define KernelPackage/pata-cs5536
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=CS5536 PATA support
  DEPENDS:=@TARGET_x86 @PCI_SUPPORT
  KCONFIG:=CONFIG_PATA_CS5536
  FILES=$(LINUX_DIR)/drivers/ata/pata_cs5536.ko
  AUTOLOAD:=$(call AutoLoad,30,pata_cs5536,1)
  $(call AddDepends/ata)
endef

define KernelPackage/pata-cs5536/description
  Kernel module for the AMD CS5536 Geode LX companion chip
endef

$(eval $(call KernelPackage,pata-cs5536))


define KernelPackage/cpu-msr
  SUBMENU:=$(OTHER_MENU)
  TITLE:=x86 CPU MSR support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_X86_MSR
  FILES:=$(LINUX_DIR)/arch/x86/kernel/msr.ko
  AUTOLOAD:=$(call AutoLoad,20,msr)
endef

define KernelPackage/cpu-msr/description
 Kernel module for Model Specific Registers support in x86 CPUs
endef

$(eval $(call KernelPackage,cpu-msr))

define KernelPackage/gpio-cs5535-new
  SUBMENU:=$(OTHER_MENU)
  TITLE:=AMD CS5535/CS5536 GPIO driver with improved sysfs support
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfd
  KCONFIG:=CONFIG_GPIO_CS5535
  FILES:=$(LINUX_DIR)/drivers/gpio/gpio-cs5535.ko
  AUTOLOAD:=$(call AutoLoad,50,gpio-cs5535)
endef

define KernelPackage/gpio-cs5535-new/description
 This package contains the new AMD CS5535/CS5536 GPIO driver
endef

$(eval $(call KernelPackage,gpio-cs5535-new))

define KernelPackage/gpio-nsc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi GPIO support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_NSC_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/nsc_gpio.ko
  AUTOLOAD:=$(call AutoLoad,40,nsc_gpio)
endef

define KernelPackage/gpio-nsc/description
 Kernel module for Natsemi GPIO
endef

$(eval $(call KernelPackage,gpio-nsc))


define KernelPackage/gpio-pc8736x
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PC8736x GPIO support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_PC8736x_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/pc8736x_gpio.ko
  AUTOLOAD:=$(call AutoLoad,40,pc8736x_gpio)
endef

define KernelPackage/gpio-pc8736x/description
 Kernel module for PC8736x GPIO
endef

$(eval $(call KernelPackage,gpio-pc8736x))


define KernelPackage/gpio-scx200
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SCX200 GPIO support
  DEPENDS:=@TARGET_x86 +kmod-gpio-nsc
  KCONFIG:=CONFIG_SCx200_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/scx200_gpio.ko
  AUTOLOAD:=$(call AutoLoad,50,scx200_gpio)
endef

define KernelPackage/gpio-scx200/description
 Kernel module for SCX200 GPIO
endef

$(eval $(call KernelPackage,gpio-scx200))

define KernelPackage/wdt-geode
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Geode/LX Watchdog timer
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfgpt
  KCONFIG:=CONFIG_GEODE_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/geodewdt.ko
  AUTOLOAD:=$(call AutoLoad,50,geodewdt)
endef

define KernelPackage/wdt-geode/description
  Kernel module for Geode watchdog timer.
endef

$(eval $(call KernelPackage,wdt-geode))


define KernelPackage/cs5535-clockevt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CS5535/CS5536 high-res timer (MFGPT) events
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfgpt
  KCONFIG:=CONFIG_CS5535_CLOCK_EVENT_SRC
  FILES:=$(LINUX_DIR)/drivers/clocksource/cs5535-clockevt.ko
  AUTOLOAD:=$(call AutoLoad,50,cs5535-clockevt)
endef

define KernelPackage/cs5535-clockevt/description
  Kernel module for CS5535/6 high-res clock event source
endef

$(eval $(call KernelPackage,cs5535-clockevt))


define KernelPackage/cs5535-mfgpt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CS5535/6 Multifunction General Purpose Timer
  DEPENDS:=@TARGET_x86 +kmod-cs5535-mfd
  KCONFIG:=CONFIG_CS5535_MFGPT
  FILES:=$(LINUX_DIR)/drivers/misc/cs5535-mfgpt.ko
  AUTOLOAD:=$(call AutoLoad,45,cs5535-mfgpt)
endef

define KernelPackage/cs5535-mfgpt/description
  Kernel module for CS5535/6 multifunction general purpose timer.
endef

$(eval $(call KernelPackage,cs5535-mfgpt))


define KernelPackage/cs5535-mfd
  SUBMENU:=$(OTHER_MENU)
  TITLE:=CS5535/6 Multifunction General Purpose Driver
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_MFD_CS5535
  FILES:= \
  	$(LINUX_DIR)/drivers/mfd/mfd-core.ko \
  	$(LINUX_DIR)/drivers/mfd/cs5535-mfd.ko
  AUTOLOAD:=$(call AutoLoad,44,mfd-core cs5535-mfd)
endef

define KernelPackage/cs5535-mfd/description
  Core driver for CS5535/CS5536 MFD functions.
endef

$(eval $(call KernelPackage,cs5535-mfd))

define KernelPackage/wdt-sc520
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SC520 Watchdog support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SC520_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/sc520_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,sc520_wdt)
endef

define KernelPackage/wdt-sc520/description
  Kernel module for SC520 Watchdog
endef

$(eval $(call KernelPackage,wdt-sc520))


define KernelPackage/wdt-scx200
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Natsemi SCX200 Watchdog support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_SCx200_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/scx200_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,scx200_wdt)
endef

define KernelPackage/wdt-scx200/description
 Kernel module for SCX200 Watchdog
endef

$(eval $(call KernelPackage,wdt-scx200))

I2C_SCX200_MODULES:=\
  CONFIG_SCx200_I2C:drivers/i2c/busses/scx200_i2c

define KernelPackage/i2c-scx200
  $(call i2c_defaults,$(I2C_SCX200_MODULES),59)
  TITLE:=Geode SCx200 I2C using GPIO pins
  DEPENDS:=@PCI_SUPPORT @TARGET_x86 +kmod-i2c-algo-bit
  KCONFIG+= \
	CONFIG_SCx200_I2C_SCL=12 \
	CONFIG_SCx200_I2C_SDA=13
endef

define KernelPackage/i2c-scx200/description
 Kernel module for I2C using GPIO pins on the Geode SCx200 processors.
endef

$(eval $(call KernelPackage,i2c-scx200))


I2C_SCX200_ACB_MODULES:=\
  CONFIG_SCx200_ACB:drivers/i2c/busses/scx200_acb

define KernelPackage/i2c-scx200-acb
  $(call i2c_defaults,$(I2C_SCX200_ACB_MODULES),59)
  TITLE:=Geode SCx200 ACCESS.bus support
  DEPENDS:=@PCI_SUPPORT @TARGET_x86 +kmod-i2c-algo-bit
endef

define KernelPackage/i2c-scx200-acb/description
 Kernel module for I2C using the ACCESS.bus controllers on the Geode SCx200
 and SC1100 processors and the CS5535 and CS5536 Geode companion devices.
endef

$(eval $(call KernelPackage,i2c-scx200-acb))


define KernelPackage/leds-wrap
  SUBMENU:=$(LEDS_MENU)
  TITLE:=PCengines WRAP LED support
  DEPENDS:=@TARGET_x86 +kmod-gpio-scx200
  KCONFIG:=CONFIG_LEDS_WRAP
  FILES:=$(LINUX_DIR)/drivers/leds/leds-wrap.ko
  AUTOLOAD:=$(call AutoLoad,50,leds-wrap)
endef

define KernelPackage/leds-wrap/description
 Kernel module for PCengines WRAP LEDs
endef

$(eval $(call KernelPackage,leds-wrap))


define KernelPackage/leds-net48xx
  SUBMENU:=$(LEDS_MENU)
  TITLE:=Soekris Net48xx LED support
  DEPENDS:=@TARGET_x86 +kmod-gpio-scx200
  KCONFIG:=CONFIG_LEDS_NET48XX
  FILES:=$(LINUX_DIR)/drivers/leds/leds-net48xx.ko
  AUTOLOAD:=$(call AutoLoad,50,leds-net48xx)
endef

define KernelPackage/leds-net48xx/description
 Kernel module for Soekris Net48xx LEDs
endef

$(eval $(call KernelPackage,leds-net48xx))
