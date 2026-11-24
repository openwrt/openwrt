# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/sound-soc-dvf99
  TITLE:=DSPG DVF99 cordless subsystem sound support
  KCONFIG:= \
	CONFIG_SND_SOC_DVF99_CSS \
	CONFIG_SND_SOC_DVF9A
  FILES:= \
	$(LINUX_DIR)/sound/soc/dspg/snd-soc-dvf99-css.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-dvf9a.ko
  AUTOLOAD:=$(call AutoLoad,65,snd-soc-dvf9a snd-soc-dvf99-css)
  DEPENDS:=@TARGET_dspg +kmod-sound-soc-core
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-dvf99/description
  PCM devices of the DVF99 cordless subsystem firmware and the mixer
  controls of the DVF9A analog front end.
endef

$(eval $(call KernelPackage,sound-soc-dvf99))
