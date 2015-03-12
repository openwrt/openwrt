#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/fb-cobalt
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Cobalt framebuffer support
  DEPENDS:=@TARGET_cobalt +kmod-fb
  KCONFIG:=CONFIG_FB_COBALT
  FILES:=$(LINUX_DIR)/drivers/video/fbdev/cobalt_lcdfb.ko
  AUTOLOAD:=$(call AutoLoad,50,cobalt_lcdfb)
endef

define KernelPackage/fb-cobalt/descriptione
  Kernel module for the Cobalt Microservers framebuffer
endef

$(eval $(call KernelPackage,fb-cobalt))

define KernelPackage/cobalt-btns
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Cobalt buttons support
  DEPENDS:=@TARGET_cobalt +kmod-input-evdev +kmod-input-polldev
  KCONFIG:=CONFIG_INPUT_COBALT_BTNS
  FILES:=$(LINUX_DIR)/drivers/input/misc/cobalt_btns.ko
  AUTOLOAD:=$(call AutoLoad,62,cobalt_btns)
endef

define KernelPackage/cobalt-btns/description
  Kernel module for the Cobalt Microservers buttons
endef

$(eval $(call KernelPackage,cobalt-btns))
