#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

SOUND_MENU:=Sound Support

define KernelPackage/sound-core
  SUBMENU:=$(SOUND_MENU)
  TITLE:=Sound support
  DEPENDS:=@PCI_SUPPORT||USB_SUPPORT
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
	CONFIG_HOSTAUDIO
endef

define KernelPackage/sound-core/2.4
  FILES:=$(LINUX_DIR)/drivers/sound/soundcore.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,soundcore)
endef

define KernelPackage/sound-core/2.6
  FILES:= \
	$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-page-alloc.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-hwdep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/seq/snd-seq-device.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-rawmidi.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-timer.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-pcm.$(LINUX_KMOD_SUFFIX) \
	$(if $(CONFIG_SND_MIXER_OSS),$(LINUX_DIR)/sound/core/oss/snd-mixer-oss.$(LINUX_KMOD_SUFFIX)) \
	$(if $(CONFIG_SND_PCM_OSS),$(LINUX_DIR)/sound/core/oss/snd-pcm-oss.$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,30, \
	soundcore \
	snd \
	snd-page-alloc \
	snd-hwdep \
	snd-seq-device \
	snd-rawmidi \
	snd-timer \
	snd-pcm \
	$(if $(CONFIG_SND_MIXER_OSS),snd-mixer-oss) \
	$(if $(CONFIG_SND_PCM_OSS),snd-pcm-oss) \
  )
endef

define KernelPackage/sound-core/uml-2.6
  FILES:= \
	$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/arch/um/drivers/hostaudio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,soundcore hostaudio)
endef

define KernelPackage/sound-core/description
 Kernel modules for sound support
endef

$(eval $(call KernelPackage,sound-core))

define KernelPackage/sound-i8x0
  SUBMENU:=$(SOUND_MENU)
  TITLE:=Intel/SiS/nVidia/AMD/ALi AC97 Controller
  DEPENDS:=kmod-sound-core
  KCONFIG:=CONFIG_SND_INTEL8X0
  FILES:=$(LINUX_DIR)/sound/pci/snd-intel8x0.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,35,snd-i8x0)
endef

define KernelPackage/sound-i8x0/description
 support for the integrated AC97 sound device on motherboards
 with Intel/SiS/nVidia/AMD chipsets, or ALi chipsets using 
 the M5455 Audio Controller.
endef

$(eval $(call KernelPackage,sound-i8x0))

define KernelPackage/sound-cs5535audio
  SUBMENU:=$(SOUND_MENU)
  TITLE:=CS5535 PCI Controller
  DEPENDS:=kmod-sound-core
  KCONFIG:=CONFIG_SND_CS5535AUDIO
  FILES:=$(LINUX_DIR)/sound/pci/cs5535audio/snd-cs5535audio.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/ac97_bus.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/pci/ac97/snd-ac97-codec.$(LINUX_KMOD_SUFFIX) 
  AUTOLOAD:=$(call AutoLoad,35, ac97_bus snd-ac97-codec snd-cs5535audio)
endef

define KernelPackage/sound-cs5535audio/description
 support for the integrated AC97 sound device on olpc
endef

$(eval $(call KernelPackage,sound-cs5535audio))
