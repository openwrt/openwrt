

SOUND_MENU:=Sound Support

define KernelPackage/sound-soc-jz4740
  SUBMENU:=$(SOUND_MENU)
  DEPENDS:=kmod-sound-soc-core @TARGET_xburst
  TITLE:=JZ4740 SoC sound support
  KCONFIG:=CONFIG_SND_JZ4740_SOC CONFIG_SND_JZ4740_SOC_I2S @TARGET_xburst
  FILES:= \
	$(LINUX_DIR)/sound/soc/jz4740/snd-soc-jz4740.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/soc/jz4740/snd-soc-jz4740-i2s.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,snd-soc-jz4740 snd-soc-jz4740-i2s)
endef

define KernelPackage/sound-soc-jzcodec
  SUBMENU:=$(SOUND_MENU)
  DEPENDS:=kmod-sound-soc-core @TARGET_xburst
  TITLE:=JZ4740 SoC internal codec support
  KCONFIG:=CONFIG_SND_SOC_JZCODEC
  FILES:=$(LINUX_DIR)/sound/soc/codecs/snd-soc-jzcodec.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,snd-soc-jzcodec)
endef

define KernelPackage/sound-soc-xburst/default
  SUBMENU:=$(SOUND_MENU)
  DEPENDS:=kmod-sound-soc-jz4740 kmod-sound-soc-jzcodec @TARGET_xburst_$(if $(4),$(4),$(3))
  TITLE:=$(1) sound support
  KCONFIG:=CONFIG_SND_JZ4740_SOC_$(2)
  FILES:=$(LINUX_DIR)/sound/soc/jz4740/snd-soc-$(3).$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,snd-soc-$(3))
endef

define KernelPackage/sound-soc-qilb60
$(call KernelPackage/sound-soc-xburst/default,QI NanoNote,QI_LB60,qi-lb60,qi_lb60)
endef

define KernelPackage/sound-soc-n516
$(call KernelPackage/sound-soc-xburst/default,Hanvon N516,N516,n516)
endef

define KernelPackage/sound-soc-n526
$(call KernelPackage/sound-soc-xburst/default,Hanvon N526,N526,n526)
endef

$(eval $(call KernelPackage,sound-soc-jz4740))
$(eval $(call KernelPackage,sound-soc-jzcodec))
$(eval $(call KernelPackage,sound-soc-qilb60))
$(eval $(call KernelPackage,sound-soc-n516))
$(eval $(call KernelPackage,sound-soc-n526))
