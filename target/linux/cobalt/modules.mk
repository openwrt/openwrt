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
  FILES:=$(LINUX_DIR)/drivers/video/cobalt_lcdfb.ko
  AUTOLOAD:=$(call AutoLoad,50,cobalt_lcdfb)
endef

define KernelPackage/fb-cobalt/descriptione
  Kernel module for the Cobalt Microservers framebuffer
endef

$(eval $(call KernelPackage,fb-cobalt))
