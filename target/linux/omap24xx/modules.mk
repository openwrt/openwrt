#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/sound-soc-omap
  TITLE:=OMAP SoC sound support
  KCONFIG:= \
	CONFIG_SND_OMAP_SOC
  FILES:=$(LINUX_DIR)/sound/soc/omap/snd-soc-omap.ko
  AUTOLOAD:=$(call AutoLoad,60,snd-soc-omap)
  DEPENDS:=@TARGET_omap24xx +kmod-sound-soc-core
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-omap))


define KernelPackage/sound-soc-omap-mcbsp
  TITLE:=OMAP SoC MCBSP support
  KCONFIG:= \
	CONFIG_SND_OMAP_SOC_MCBSP
  FILES:=$(LINUX_DIR)/sound/soc/omap/snd-soc-omap-mcbsp.ko
  AUTOLOAD:=$(call AutoLoad,61,snd-soc-omap-mcbsp)
  DEPENDS:=@TARGET_omap24xx +kmod-sound-soc-omap
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-omap-mcbsp))


define KernelPackage/sound-soc-n810
  TITLE:=Nokia n810 SoC sound support
  KCONFIG:= \
	CONFIG_SND_OMAP_SOC_N810
  FILES:= \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-tlv320aic3x.ko \
	$(LINUX_DIR)/sound/soc/omap/snd-soc-n810.ko
  AUTOLOAD:=$(call AutoLoad,65,snd-soc-tlv320aic3x snd-soc-n810)
  DEPENDS:=@TARGET_omap24xx +kmod-sound-soc-omap +kmod-sound-soc-omap-mcbsp
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-n810))
