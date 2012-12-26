#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/ata-rbppc-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=RB600 Compact Flash support
  DEPENDS:=@TARGET_mpc83xx
  KCONFIG:= \
  	CONFIG_PATA_PLATFORM \
  	CONFIG_PATA_RB_PPC
  FILES:=\
  	$(LINUX_DIR)/drivers/ata/pata_rbppc_cf.ko
  AUTOLOAD:=$(call AutoLoad,41,pata_rbppc_cf,1)
  $(call AddDepends/ata)
endef

define KernelPackage/ata-rbppc-cf/description
  RB600 Compact Flash support.
endef

$(eval $(call KernelPackage,ata-rbppc-cf))
