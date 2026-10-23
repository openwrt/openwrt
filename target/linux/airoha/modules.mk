# SPDX-License-Identifier: GPL-2.0-only

OTHER_MENU:=Other modules


define KernelPackage/pwm-airoha
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Airoha AN7581 and AN7583 PWM
  DEPENDS:=@TARGET_airoha_an7581||TARGET_airoha_an7583
  KCONFIG:= \
        CONFIG_PWM=y \
        CONFIG_PWM_AIROHA=y
  FILES:= \
        $(LINUX_DIR)/drivers/pwm/pwm-airoha.ko
  AUTOLOAD:=$(call AutoProbe,pwm-airoha)
endef

define KernelPackage/pwm-airoha/description
 Kernel module to use the PWM channel on Airoha SoC
endef

$(eval $(call KernelPackage,pwm-airoha))


define KernelPackage/sound-soc-an7581
  TITLE:=Airoha AN7581 Audio support
  KCONFIG:=CONFIG_SND_SOC_AN7581
  FILES:= \
        $(LINUX_DIR)/sound/soc/mediatek/an7581/snd-soc-an7581-afe.ko
  AUTOLOAD:=$(call AutoProbe,snd-soc-an7581-afe)
  DEPENDS:=@TARGET_airoha_an7581 +kmod-sound-soc-mediatek
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-an7581/description
 Support for audio on systems using the Airoha AN7581 SoC.
endef

$(eval $(call KernelPackage,sound-soc-an7581))


define KernelPackage/sound-soc-an7581-wm8960
  TITLE:=Airoha AN7581 Audio WM8960 codec support
  KCONFIG:=CONFIG_SND_SOC_AN7581_WM8960
  FILES:= \
        $(LINUX_DIR)/sound/soc/mediatek/an7581/an7581-wm8960.ko
  AUTOLOAD:=$(call AutoProbe,an7581-wm8960)
  DEPENDS:=@TARGET_airoha_an7581 +kmod-sound-soc-an7581 +kmod-sound-soc-wm8960
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-an7581-wm8960/description
 Support for use of the Wolfson Audio WM8960 codec with the Airoha AN7581 SoC.
endef

$(eval $(call KernelPackage,sound-soc-an7581-wm8960))


