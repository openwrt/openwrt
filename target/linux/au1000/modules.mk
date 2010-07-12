#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/pcmcia-au1000
  SUBMENU:=$(PCMCIA_MENU)
  TITLE:=RMI/AMD Au1000 PCMCIA support
  DEPENDS:=@TARGET_au1000 kmod-pcmcia-core
  FILES:=$(LINUX_DIR)/drivers/pcmcia/au1x00_ss.ko
  AUTOLOAD:=$(call AutoLoad,41,au1x00_ss)
endef

$(eval $(call KernelPackage,pcmcia-au1000))
