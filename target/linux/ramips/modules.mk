#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

OTHER_MENU:=Other modules
define KernelPackage/sdhci-mt7620
  SUBMENU:=Other modules
  TITLE:=MT7620 SDCI
  DEPENDS:=@TARGET_ramips_mt7620a +kmod-sdhci
  KCONFIG:= \
	CONFIG_MMC_SDHCI_MT7620
  FILES:= \
	$(LINUX_DIR)/drivers/mmc/host/sdhci-mt7620.ko
  AUTOLOAD:=$(call AutoProbe,sdhci-mt7620,1)
endef

$(eval $(call KernelPackage,sdhci-mt7620))

I2C_RALINK_MODULES:= \
  CONFIG_I2C_RALINK:drivers/i2c/busses/i2c-ralink

define KernelPackage/i2c-ralink
  $(call i2c_defaults,$(I2C_RALINK_MODULES),59)
  TITLE:=Ralink I2C Controller
  DEPENDS:=@TARGET_ramips kmod-i2c-core
endef

define KernelPackage/i2c-ralink/description
 Kernel modules for enable ralink i2c controller.
endef

$(eval $(call KernelPackage,i2c-ralink))

define KernelPackage/sound-mt7620
  TITLE:=MT7620 PCM/I2S Alsa Driver
  DEPENDS:=@TARGET_ramips_mt7620a +kmod-sound-soc-core +kmod-regmap
  KCONFIG:= \
	CONFIG_SND_MT7620_SOC_I2S \
	CONFIG_SND_MT7620_SOC_WM8960
  FILES:= \
	$(LINUX_DIR)/sound/soc/ralink/snd-soc-mt7620-i2s.ko \
	$(LINUX_DIR)/sound/soc/ralink/snd-soc-mt7620-wm8960.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-wm8960.ko
  AUTOLOAD:=$(call AutoLoad,90,snd-soc-wm8960 snd-soc-mt7620-i2s snd-soc-mt7620-wm8960)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-mt7620/description
 Alsa modules for ralink i2s controller.
endef

$(eval $(call KernelPackage,sound-mt7620))
