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
  DEPENDS:=@TARGET_pistachio_marduk
  $(call AddDepends/serial)
endef

$(eval $(call KernelPackage,serial-sc16is7xx))


define KernelPackage/sound-pistachio-soc
  TITLE:=SoC Audio support for the Pistachio SOC
  KCONFIG:= \
    CONFIG_SND_MIPS=y \
    CONFIG_SND_SOC_IMG=y \
    CONFIG_SND_SOC_IMG_I2S_IN \
    CONFIG_SND_SOC_IMG_I2S_OUT \
    CONFIG_SND_SOC_IMG_PARALLEL_OUT \
    CONFIG_SND_SOC_IMG_SPDIF_IN \
    CONFIG_SND_SOC_IMG_SPDIF_OUT \
    CONFIG_SND_SOC_IMG_PISTACHIO_INTERNAL_DAC \
    CONFIG_SND_SOC_IMG_PISTACHIO_BUB \
    CONFIG_SND_SOC_IMG_PISTACHIO \
    CONFIG_SND_SOC_IMG_PISTACHIO_EVENT_TIMER=y \
    CONFIG_SND_SOC_IMG_PISTACHIO_EVENT_TIMER_ATU=y \
    CONFIG_SND_SOC_IMG_PISTACHIO_EVENT_TIMER_LOCAL=n
  FILES:= \
    $(LINUX_DIR)/sound/soc/codecs/snd-soc-tpa6130a2.ko \
    $(LINUX_DIR)/sound/soc/img/img-i2s-in.ko \
    $(LINUX_DIR)/sound/soc/img/img-i2s-out.ko \
    $(LINUX_DIR)/sound/soc/img/img-parallel-out.ko \
    $(LINUX_DIR)/sound/soc/img/img-spdif-in.ko \
    $(LINUX_DIR)/sound/soc/img/img-spdif-out.ko \
    $(LINUX_DIR)/sound/soc/img/pistachio-event-timer-module.ko \
    $(LINUX_DIR)/sound/soc/img/pistachio-internal-dac.ko \
    $(LINUX_DIR)/sound/soc/img/pistachio.ko
  AUTOLOAD:=$(call AutoProbe,img-i2s-in img-i2s-out img-parallel-out img-spdif-in img-spdif-out pistachio-event-timer-module pistachio-internal-dac snd-soc-tpa6130a2 pistachio)
  DEPENDS:=@TARGET_pistachio_marduk +kmod-sound-soc-core +kmod-serial-sc16is7xx
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-pistachio-soc))

