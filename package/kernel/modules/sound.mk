#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

SOUND_MENU:=Sound Support

define KernelPackage/pcspkr
  SUBMENU:=$(SOUND_MENU)
  TITLE:=PC speaker support
  KCONFIG:=CONFIG_INPUT_PCSPKR
  FILES:=$(LINUX_DIR)/drivers/input/misc/pcspkr.ko
  AUTOLOAD:=$(call AutoLoad,50,pcspkr)
endef

define KernelPackage/pcspkr/description
 This enables sounds (tones) through the pc speaker
endef

$(eval $(call KernelPackage,pcspkr))


# allow targets to override the soundcore stuff
SOUNDCORE_LOAD ?= \
	soundcore \
	snd \
	snd-page-alloc \
	snd-hwdep \
	snd-seq-device \
	snd-rawmidi \
	snd-timer \
	snd-pcm \
	snd-mixer-oss \
	snd-pcm-oss

SOUNDCORE_FILES ?= \
	$(LINUX_DIR)/sound/soundcore.ko \
	$(LINUX_DIR)/sound/core/snd.ko \
	$(LINUX_DIR)/sound/core/snd-page-alloc.ko \
	$(LINUX_DIR)/sound/core/snd-hwdep.ko \
	$(LINUX_DIR)/sound/core/seq/snd-seq-device.ko \
	$(LINUX_DIR)/sound/core/snd-rawmidi.ko \
	$(LINUX_DIR)/sound/core/snd-timer.ko \
	$(LINUX_DIR)/sound/core/snd-pcm.ko \
	$(LINUX_DIR)/sound/core/oss/snd-mixer-oss.ko \
	$(LINUX_DIR)/sound/core/oss/snd-pcm-oss.ko

define KernelPackage/sound-core
  SUBMENU:=$(SOUND_MENU)
  TITLE:=Sound support
  DEPENDS:=@AUDIO_SUPPORT
  KCONFIG:= \
	CONFIG_SOUND \
	CONFIG_SND \
	CONFIG_SND_HWDEP \
	CONFIG_SND_RAWMIDI \
	CONFIG_SND_TIMER \
	CONFIG_SND_PCM \
	CONFIG_SND_SEQUENCER \
	CONFIG_SND_VIRMIDI \
	CONFIG_SND_SEQ_DUMMY \
	CONFIG_SND_SEQUENCER_OSS=y \
	CONFIG_HOSTAUDIO \
	CONFIG_SND_PCM_OSS \
	CONFIG_SND_MIXER_OSS \
	CONFIG_SOUND_OSS_CORE_PRECLAIM=y
  FILES:=$(SOUNDCORE_FILES)
  AUTOLOAD:=$(call AutoLoad,30,$(SOUNDCORE_LOAD))
  $(call AddDepends/input)
endef

define KernelPackage/sound-core/uml
  FILES:= \
	$(LINUX_DIR)/sound/soundcore.ko \
	$(LINUX_DIR)/arch/um/drivers/hostaudio.ko
  AUTOLOAD:=$(call AutoLoad,30,soundcore hostaudio)
endef

define KernelPackage/sound-core/description
 Kernel modules for sound support
endef

$(eval $(call KernelPackage,sound-core))


define AddDepends/sound
  SUBMENU:=$(SOUND_MENU)
  DEPENDS+=kmod-sound-core $(1) @!TARGET_uml
endef


define KernelPackage/ac97
  TITLE:=ac97 controller
  KCONFIG:=CONFIG_SND_AC97_CODEC
  FILES:= \
	$(LINUX_DIR)/sound/ac97_bus.ko \
	$(LINUX_DIR)/sound/pci/ac97/snd-ac97-codec.ko 
  AUTOLOAD:=$(call AutoLoad,35,ac97_bus snd-ac97-codec)
  $(call AddDepends/sound)
endef

define KernelPackage/ac97/description
 The ac97 controller
endef

$(eval $(call KernelPackage,ac97))


define KernelPackage/sound-i8x0
  TITLE:=Intel/SiS/nVidia/AMD/ALi AC97 Controller
  DEPENDS:=+kmod-ac97
  KCONFIG:=CONFIG_SND_INTEL8X0
  FILES:=$(LINUX_DIR)/sound/pci/snd-intel8x0.ko
  AUTOLOAD:=$(call AutoLoad,36,snd-intel8x0)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-i8x0/description
 support for the integrated AC97 sound device on motherboards
 with Intel/SiS/nVidia/AMD chipsets, or ALi chipsets using 
 the M5455 Audio Controller.
endef

$(eval $(call KernelPackage,sound-i8x0))


define KernelPackage/sound-cs5535audio
  TITLE:=CS5535 PCI Controller
  DEPENDS:=+kmod-ac97
  KCONFIG:=CONFIG_SND_CS5535AUDIO
  FILES:=$(LINUX_DIR)/sound/pci/cs5535audio/snd-cs5535audio.ko
  AUTOLOAD:=$(call AutoLoad,36,snd-cs5535audio)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-cs5535audio/description
 support for the integrated AC97 sound device on olpc
endef

$(eval $(call KernelPackage,sound-cs5535audio))


define KernelPackage/sound-soc-core
  TITLE:=SoC sound support
  KCONFIG:= \
	CONFIG_SND_SOC \
	CONFIG_SND_SOC_ALL_CODECS=n
  FILES:=$(LINUX_DIR)/sound/soc/snd-soc-core.ko
  AUTOLOAD:=$(call AutoLoad,55, snd-soc-core)
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-core))


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
