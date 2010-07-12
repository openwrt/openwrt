#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/ps3lan
  SUBMENU:=Network Devices
  TITLE:=PS3-Lan support
  DEPENDS:=@TARGET_ps3
  KCONFIG:= \
	CONFIG_GELIC_NET \
	CONFIG_GELIC_WIRELESS=y
  FILES:=$(LINUX_DIR)/drivers/net/ps3_gelic.ko
  AUTOLOAD:=$(call AutoLoad,50,ps3_gelic)
endef

define KernelPackage/ps3lan/description
  Kernel module for PS3 Networking. Includes Gb Ethernet and WLAN
endef

$(eval $(call KernelPackage,ps3lan))


define KernelPackage/ps3vram
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=PS3 Video RAM Storage Driver
  DEPENDS:=@TARGET_ps3
  KCONFIG:=CONFIG_PS3_VRAM
  FILES:=$(LINUX_DIR)/drivers/block/ps3vram.ko
  AUTOLOAD:=$(call AutoLoad,01,ps3vram)
endef

define KernelPackage/ps3vram/description
  Kernel support for PS3 Video RAM Storage
endef

$(eval $(call KernelPackage,ps3vram))


define KernelPackage/sound-ps3
  SUBMENU:=$(SOUND_MENU)
  TITLE:=PS3 Audio
  DEPENDS:=@TARGET_ps3
  KCONFIG:=CONFIG_SND_PS3 \
		CONFIG_SND_PPC=y \
		CONFIG_SND_PS3_DEFAULT_START_DELAY=2000
  FILES:=$(LINUX_DIR)/sound/ppc/snd_ps3.ko
  AUTOLOAD:=$(call AutoLoad,35, snd_ps3)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-ps3/description
 support for the integrated PS3 audio device
endef

$(eval $(call KernelPackage,sound-ps3))
