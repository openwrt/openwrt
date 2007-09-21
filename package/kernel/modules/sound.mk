#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

SOUND_MENU:=Sound Support

# XXX: remove @!TARGET_* later when we have PCI & USB support properly detected on all targets
define KernelPackage/sound-core
  SUBMENU:=$(SOUND_MENU)
  TITLE:=Sound support
  DEPENDS:=@USB_SUPPORT @!TARGET_atheros
  KCONFIG:= \
	CONFIG_SOUND \
	CONFIG_SND \
	CONFIG_SND_HWDEP \
	CONFIG_SND_RAWMIDI \
	CONFIG_SND_TIMER \
	CONFIG_SND_PCM \
	CONFIG_HOSTAUDIO
endef

define KernelPackage/sound-core/2.4
  FILES:=$(LINUX_DIR)/drivers/sound/soundcore.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,soundcore)
endef

define KernelPackage/sound-core/2.6
#  KCONFIG+= \
#	CONFIG_SND \
#	CONFIG_SND_HWDEP \
#	CONFIG_SND_RAWMIDI \
#	CONFIG_SND_TIMER \
#	CONFIG_SND_PCM
  FILES:= \
	$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-page-alloc.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/snd-hwdep.$(LINUX_KMOD_SUFFIX) \
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
	snd-rawmidi \
	snd-timer \
	snd-pcm \
	$(if $(CONFIG_SND_MIXER_OSS),snd-mixer-oss) \
	$(if $(CONFIG_SND_PCM_OSS),snd-pcm-oss) \
  )
endef

define KernelPackage/sound-core/uml-2.6
#  KCONFIG+= \
#	CONFIG_HOSTAUDIO
  FILES:= \
	$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/arch/um/drivers/hostaudio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,soundcore hostaudio)
endef

define KernelPackage/sound-core/description
 Kernel modules for sound support
endef

$(eval $(call KernelPackage,sound-core))

