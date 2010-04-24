#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/ata-magicbox-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Magicbox v2/OpenRB Compact flash support
  DEPENDS:=@TARGET_ppc40x
  KCONFIG:=CONFIG_PATA_MAGICBOX_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_magicbox_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_magicbox_cf,1)
  $(call AddDepends/ata)
endef

define KernelPackage/ata-magicbox-cf/description
  Support for Magicbox v2/OpenRB on-board CF slot.
endef

$(eval $(call KernelPackage,ata-magicbox-cf))
