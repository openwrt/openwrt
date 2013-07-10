#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/sound-arm-bcm2835
  TITLE:=Broadcom 2708,2835 SoC sound support
  KCONFIG:= \
	CONFIG_SND_ARM=y \
	CONFIG_SND_BCM2835 \
	CONFIG_SND_ARMAACI=n
  FILES:= \
	$(LINUX_DIR)/sound/arm/snd-bcm2835.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-bcm2835)
  DEPENDS:=@TARGET_brcm2708
  $(call AddDepends/sound)
endef

define KernelPackage/sound-arm-bcm2835/description
  This package contains the Broadcom 2708/2835 sound driver
endef

$(eval $(call KernelPackage,sound-arm-bcm2835))

define KernelPackage/random-bcm2708
  SUBMENU:=$(OTHER_MENU)
  TITLE:=BCM2708 H/W Random Number Generator
  KCONFIG:=CONFIG_HW_RANDOM_BCM2708
  FILES:=$(LINUX_DIR)/drivers/char/hw_random/bcm2708-rng.ko
  AUTOLOAD:=$(call AutoLoad,11,bcm2708-rng)
  DEPENDS:=@TARGET_brcm2708 +kmod-random-core
endef

define KernelPackage/random-bcm2708/description
  This package contains the Broadcom 2708 HW random number generator driver
endef

$(eval $(call KernelPackage,random-bcm2708))


define KernelPackage/spi-bcm2708
  SUBMENU:=$(SPI_MENU)
  TITLE:=BCM2708 SPI controller driver (SPI0)
  KCONFIG:=CONFIG_SPI_BCM2708 \
	   CONFIG_SPI=y \
	   CONFIG_SPI_MASTER=y \
	   CONFIG_BCM2708_SPIDEV=y
  FILES:=$(LINUX_DIR)/drivers/spi/spi-bcm2708.ko
  AUTOLOAD:=$(call AutoLoad,89,spi-bcm2708)
  DEPENDS:=@TARGET_brcm2708
endef

define KernelPackage/spi-bcm2708/description
  This package contains the Broadcom 2708 SPI master controller driver
endef

$(eval $(call KernelPackage,spi-bcm2708))


I2C_BCM2708_MODULES:=\
  CONFIG_I2C_BCM2708:drivers/i2c/busses/i2c-bcm2708

define KernelPackage/i2c-bcm2708
  $(call i2c_defaults,$(I2C_BCM2708_MODULES),59)
  KCONFIG+= \
	   CONFIG_I2C_BCM2708_BAUDRATE=100000
  TITLE:=Broadcom BCM2708 I2C master controller driver
  DEPENDS:=@TARGET_brcm2708 +kmod-i2c-core
endef

define KernelPackage/i2c-bcm2708/description
  This package contains the Broadcom 2708 I2C master controller driver
endef

$(eval $(call KernelPackage,i2c-bcm2708))
