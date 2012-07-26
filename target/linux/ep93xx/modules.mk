#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/fb-ep93xx
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=EP93xx framebuffer support
  DEPENDS:=@TARGET_ep93xx +kmod-fb +kmod-fb-cfb-fillrect \
	   +kmod-fb-cfb-copyarea +kmod-fb-cfb-imgblt
  KCONFIG:=CONFIG_FB_EP93XX
  FILES:=$(LINUX_DIR)/drivers/video/ep93xx-fb.ko
  AUTOLOAD:=$(call AutoLoad,51,ep93xx-fb)
endef

define KernelPackage/fb-ep93xx/description
  EP93xx framebuffer support
endef

$(eval $(call KernelPackage,fb-ep93xx))

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
  DEPENDS:=@TARGET_ep93xx +kmod-sound-soc-ac97
  KCONFIG:=CONFIG_SND_EP93XX_SOC
  FILES:=$(LINUX_DIR)/sound/soc/ep93xx/snd-soc-ep93xx.ko
  AUTOLOAD:=$(call AutoLoad,57,snd-soc-ep93xx)
endef

define KernelPackage/sound-soc-ep93xx/description
  EP93xx SoC sound support
endef

$(eval $(call KernelPackage,sound-soc-ep93xx))

# Must be loaded prior to sound-soc-ac97 because it exports
# soc_ac97_ops
define KernelPackage/sound-soc-ep93xx-ac97
  SUBMENU:=$(SOUND_MENU)
  TITLE:=EP93xx SoC AC97 support
  DEPENDS:=@TARGET_ep93xx +kmod-sound-soc-ep93xx
  KCONFIG:=CONFIG_SND_EP93XX_SOC_AC97
  FILES:=$(LINUX_DIR)/sound/soc/ep93xx/snd-soc-ep93xx-ac97.ko
  AUTOLOAD:=$(call AutoLoad,56,snd-soc-ep93xx-ac97)
endef

define KernelPackage/sound-soc-ep93xx-ac97/description
  EP93xx SoC AC97 support
endef

$(eval $(call KernelPackage,sound-soc-ep93xx-ac97))

define KernelPackage/sound-soc-ep93xx-simone
  SUBMENU:=$(SOUND_MENU)
  TITLE:=Sim.One EP93xx Soc sound support
  DEPENDS:=@TARGET_ep93xx +kmod-sound-soc-ep93xx +kmod-sound-soc-ep93xx-ac97
  KCONFIG:=CONFIG_SND_EP93XX_SOC_SIMONE
  FILES:=$(LINUX_DIR)/sound/soc/ep93xx/snd-soc-simone.ko
  AUTOLOAD:=$(call AutoLoad,59,snd-soc-ep93xx)
endef

define KernelPackage/sound-soc-ep93xx-simone/description
  SimpleMachines Sim.One sound support
endef

$(eval $(call KernelPackage,sound-soc-ep93xx-simone))

define KernelPackage/touchscreen-ep93xx
  SUBMENU:=$(OTHER_MENU)
  TITLE:=EP93xx input touchscreen support
  DEPENDS:=@TARGET_ep93xx
  KCONFIG:=CONFIG_TOUCHSCREEN_EP93XX
  FILES:=$(LINUX_DIR)/drivers/input/touchscreen/ep93xx_ts.ko
  AUTOLOAD:=$(call AutoLoad,51,ep93xx_ts)
endef

define KernelPackage/touchscreen-ep93xx/description
  EP93xx SoC input touchscreen support
endef

$(eval $(call KernelPackage,touchscreen-ep93xx))

