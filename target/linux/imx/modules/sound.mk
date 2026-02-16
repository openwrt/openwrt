# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/sound-soc-fsl-utils
  TITLE:=Support for Freescale Sound SoC utils module
  KCONFIG:= CONFIG_SND_SOC_FSL_UTILS
  FILES:= \
    $(LINUX_DIR)/sound/soc/fsl/snd-soc-fsl-utils.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-fsl-utils)
  DEPENDS:=+kmod-sound-soc-imx
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-fsl-utils/description
  This package contains support for Freescale Sound SoC
  utils module on i.MX boards
endef

$(eval $(call KernelPackage,sound-soc-fsl-utils))


define KernelPackage/sound-soc-fsl-sai
  TITLE:=Support for SAI module support
  KCONFIG:= CONFIG_SND_SOC_FSL_SAI
  FILES:= \
    $(LINUX_DIR)/sound/soc/fsl/snd-soc-fsl-sai.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-fsl-sai)
  DEPENDS:=+kmod-sound-soc-imx +kmod-sound-soc-fsl-utils
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-fsl-sai/description
  This package contains support for Synchronous Audio
  Interface (SAI) module on i.MX boards
endef

$(eval $(call KernelPackage,sound-soc-fsl-sai))

define KernelPackage/sound-soc-fsl-mqs
  TITLE:=Medium Quality Sound (MQS) module support
  KCONFIG:= CONFIG_SND_SOC_FSL_MQS
  FILES:= \
    $(LINUX_DIR)/sound/soc/fsl/snd-soc-fsl-mqs.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-fsl-mqs)
  DEPENDS:=+kmod-sound-soc-fsl-sai
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-fsl-mqs/description
  This package contains support for Medium Quality
  Sound (MQS) module on i.MX boards
endef

$(eval $(call KernelPackage,sound-soc-fsl-mqs))


define KernelPackage/sound-soc-fsl-esai
  TITLE:=Support for ESAI module support
  KCONFIG:= CONFIG_SND_SOC_FSL_ESAI
  FILES:= \
    $(LINUX_DIR)/sound/soc/fsl/snd-soc-fsl-esai.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-fsl-esai)
  DEPENDS:=+kmod-sound-soc-imx
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-fsl-esai/description
  This package contains support for Enhanced Serial Audio
  Interface (ESAI) module on i.MX boards
endef

$(eval $(call KernelPackage,sound-soc-fsl-esai))


define KernelPackage/sound-soc-tfa9882
  TITLE:=SoC Audio support for i.MX boards with tfa9882
  KCONFIG:= \
	CONFIG_SND_SIMPLE_CARD \
	CONFIG_SND_SIMPLE_CARD_UTILS \
	CONFIG_SND_SOC_TFA9882
  FILES:= \
	$(LINUX_DIR)/sound/soc/generic/snd-soc-simple-card.ko \
	$(LINUX_DIR)/sound/soc/generic/snd-soc-simple-card-utils.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-tfa9882.ko
  AUTOLOAD:=$(call AutoLoad,68,snd-soc-tfa9882 snd-soc-simple-card)
  DEPENDS:=@TARGET_imx +kmod-sound-soc-imx \
    +kmod-sound-soc-fsl-sai \
    +kmod-sound-soc-fsl-utils
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-tfa9882/description
  This package contains support for SoC audio tfa9882 on i.MX boards
endef

$(eval $(call KernelPackage,sound-soc-tfa9882))
