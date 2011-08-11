#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/input-keyboard-ep93xx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=EP93xx Matrix Keypad support
  DEPENDS:=@TARGET_ep93xx
  KCONFIG:= \
	CONFIG_KEYBOARD_EP93XX \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/ep93xx_keypad.ko
  AUTOLOAD:=$(call AutoLoad,62,ep93xx_keypad)
  $(call AddDepends/input)
endef

define KernelPackage/input-keyboard-ep93xx/description
  EP93xx Matrix Keypad support
endef

$(eval $(call KernelPackage,input-keyboard-ep93xx))


define KernelPackage/sound-soc-ep93xx
  SUBMENU:=$(SOUND_MENU)
  TITLE:=EP93xx SoC sound support
  DEPENDS:=@TARGET_ep93xx +kmod-sound-core +kmod-sound-soc-core
  KCONFIG:=CONFIG_SND_EP93XX_SOC
  FILES:=$(LINUX_DIR)/sound/soc/ep93xx/snd-soc-ep93xx.ko
  AUTOLOAD:=$(call AutoLoad,56,snd-soc-ep93xx)
endef

define KernelPackage/sound-soc-ep93xx/description
  EP93xx SoC sound support
endef

$(eval $(call KernelPackage,sound-soc-ep93xx))

define KernelPackage/sound-soc-ep93xx-ac97
  SUBMENU:=$(SOUND_MENU)
  TITLE:=EP93xx SoC AC97 support
  DEPENDS:=+kmod-sound-soc-ep93xx
  KCONFIG:=CONFIG_SND_EP93XX_SOC_AC97
  FILES:=$(LINUX_DIR)/sound/soc/ep93xx/snd-soc-ep93xx-ac97.ko
  AUTOLOAD:=$(call AutoLoad,57,snd-soc-ep93xx-ac97)
endef

define KernelPackage/sound-soc-ep93xx-ac97/description
  EP93xx SoC AC97 support
endef

$(eval $(call KernelPackage,sound-soc-ep93xx-ac97))

define KernelPackage/sound-soc-ep93xx-simone
  SUBMENU:=$(SOUND_MENU)
  TITLE:=Sim.One EP93xx Soc sound support
  DEPENDS:=+kmod-sound-soc-ep93xx +kmod-sound-soc-ep93xx-ac97
  KCONFIG:=CONFIG_SND_EP93XX_SOC_SIMONE
  FILES:=$(LINUX_DIR)/sound/soc/ep93xx/snd-soc-simone.ko
  AUTOLOAD:=$(call AutoLoad,57,snd-soc-ep93xx)
endef

define KernelPackage/sound-soc-ep93xx-simone/description
  SimpleMachines Sim.One sound support
endef

$(eval $(call KernelPackage,sound-soc-ep93xx-simone))
