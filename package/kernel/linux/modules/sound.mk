#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

SOUND_MENU:=Sound Support

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

ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.3.0)),1)
SOUNDCORE_LOAD += \
	snd-compress

SOUNDCORE_FILES += \
	$(LINUX_DIR)/sound/core/snd-compress.ko
endif

ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.12.0)),1)
SOUNDCORE_LOAD += \
	snd-pcm-dmaengine

SOUNDCORE_FILES += \
	$(LINUX_DIR)/sound/core/snd-pcm-dmaengine.ko
endif

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
	CONFIG_SOUND_OSS_CORE_PRECLAIM=y \
	CONFIG_SND_COMPRESS_OFFLOAD
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


define KernelPackage/sound-seq
  TITLE:=Sequencer support
  FILES:= \
	$(LINUX_DIR)/sound/core/seq/snd-seq.ko \
	$(LINUX_DIR)/sound/core/seq/snd-seq-midi-event.ko \
	$(LINUX_DIR)/sound/core/seq/snd-seq-midi.ko
  AUTOLOAD:=$(call AutoLoad,35,snd-seq snd-seq-midi-event snd-seq-midi)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-seq/description
 Kernel modules for sequencer support
endef

$(eval $(call KernelPackage,sound-seq))


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
 Support for the integrated AC97 sound device on olpc
endef

$(eval $(call KernelPackage,sound-cs5535audio))


define KernelPackage/sound-soc-core
  TITLE:=SoC sound support
  DEPENDS:=+kmod-regmap +kmod-ac97
  KCONFIG:= \
	CONFIG_SND_SOC \
	CONFIG_SND_SOC_DMAENGINE_PCM=y \
	CONFIG_SND_SOC_ALL_CODECS=n
  FILES:=$(LINUX_DIR)/sound/soc/snd-soc-core.ko
  AUTOLOAD:=$(call AutoLoad,55, snd-soc-core)
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-core))


define KernelPackage/sound-soc-ac97
  TITLE:=AC97 Codec support
  KCONFIG:=CONFIG_SND_SOC_AC97_CODEC
  FILES:=$(LINUX_DIR)/sound/soc/codecs/snd-soc-ac97.ko
  AUTOLOAD:=$(call AutoLoad,57,snd-soc-ac97)
  DEPENDS:=+kmod-ac97 +kmod-sound-soc-core +TARGET_ep93xx:kmod-sound-soc-ep93xx-ac97
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-ac97))


define KernelPackage/sound-soc-imx
  TITLE:=IMX SoC support
  KCONFIG:=\
	CONFIG_SND_IMX_SOC \
	CONFIG_SND_SOC_IMX_AUDMUX \
	CONFIG_SND_SOC_FSL_SSI \
	CONFIG_SND_SOC_IMX_PCM
  FILES:= \
	$(LINUX_DIR)/sound/soc/fsl/snd-soc-imx-audmux.ko \
	$(LINUX_DIR)/sound/soc/fsl/snd-soc-fsl-ssi.ko \
	$(LINUX_DIR)/sound/soc/fsl/snd-soc-imx-pcm.ko
  AUTOLOAD:=$(call AutoLoad,56,snd-soc-imx-audmux snd-soc-fsl-ssi snd-soc-imx-pcm)
  DEPENDS:=@TARGET_imx6 +kmod-sound-soc-core
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-imx/description
 Support for i.MX6 Platform sound (ssi/audmux/pcm)
endef

$(eval $(call KernelPackage,sound-soc-imx))


define KernelPackage/sound-soc-imx-sgtl5000
  TITLE:=IMX SoC support for SGTL5000
  KCONFIG:=CONFIG_SND_SOC_IMX_SGTL5000
  FILES:=\
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-sgtl5000.ko \
	$(LINUX_DIR)/sound/soc/fsl/snd-soc-imx-sgtl5000.ko
  AUTOLOAD:=$(call AutoLoad,57,snd-soc-sgtl5000 snd-soc-imx-sgtl5000)
  DEPENDS:=@TARGET_imx6 +kmod-sound-soc-imx
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-imx-sgtl5000/description
 Support for i.MX6 Platform sound SGTL5000 codec
endef

$(eval $(call KernelPackage,sound-soc-imx-sgtl5000))


define KernelPackage/sound-soc-gw_avila
  TITLE:=Gateworks Avila SoC sound support
  KCONFIG:= \
	CONFIG_SND_GW_AVILA_SOC \
	CONFIG_SND_GW_AVILA_SOC_PCM \
	CONFIG_SND_GW_AVILA_SOC_HSS
  FILES:= \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-tlv320aic3x.ko \
	$(LINUX_DIR)/sound/soc/gw-avila/snd-soc-gw-avila.ko \
	$(LINUX_DIR)/sound/soc/gw-avila/snd-soc-gw-avila-pcm.ko \
	$(LINUX_DIR)/sound/soc/gw-avila/snd-soc-gw-avila-hss.ko
  AUTOLOAD:=$(call AutoLoad,65,snd-soc-tlv320aic3x snd-soc-gw-avila snd-soc-gw-avila-pcm snd-soc-gw-avila-hss)
  DEPENDS:=@TARGET_ixp4xx +kmod-sound-soc-core
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-gw_avila))


define KernelPackage/pcspkr
  DEPENDS:=@!TARGET_x86
  TITLE:=PC speaker support
  KCONFIG:= \
	CONFIG_INPUT_PCSPKR \
	CONFIG_SND_PCSP
  FILES:= \
	$(LINUX_DIR)/drivers/input/misc/pcspkr.ko \
	$(LINUX_DIR)/sound/drivers/pcsp/snd-pcsp.ko
  AUTOLOAD:=$(call AutoLoad,50,pcspkr snd-pcsp)
  $(call AddDepends/input)
  $(call AddDepends/sound)
endef

define KernelPackage/pcspkr/description
 This enables sounds (tones) through the pc speaker
endef

$(eval $(call KernelPackage,pcspkr))
