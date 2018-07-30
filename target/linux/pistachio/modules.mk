#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/serial-sc16is7xx
  TITLE:= SC16IS7xx Serial driver
  SUBMENU:=$(OTHER_MENU)
  KCONFIG:= \
    CONFIG_SPI=y \
    CONFIG_SPI_MASTER=y \
    CONFIG_SERIAL_SC16IS7XX \
    CONFIG_SERIAL_SC16IS7XX_CORE \
    CONFIG_SERIAL_SC16IS7XX_SPI=y \
    CONFIG_SERIAL_SC16IS7XX_I2C=n
  FILES:= $(LINUX_DIR)/drivers/tty/serial/sc16is7xx.ko
  AUTOLOAD:= $(call AutoProbe, sc16is7xx)
  DEPENDS:=@TARGET_pistachio
endef

$(eval $(call KernelPackage,serial-sc16is7xx))
