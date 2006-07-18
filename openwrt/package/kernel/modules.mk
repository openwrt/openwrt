# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

define KernelPackage/ide-core
TITLE:=Kernel support for IDE
DESCRIPTION:=Kernel modules for IDE support\\\
	useful for usb mass storage devices (e.g. on WL-HDD)\\\
	\\\
	Includes: \\\
	    - ide-core \\\
	    - ide-detect \\\
	    - ide-disk \\\
	    - pdc202xx_old
KCONFIG:=$(CONFIG_IDE)
FILES:=$(MODULES_DIR)/kernel/drivers/ide/*.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-detect ide-disk)
endef
$(eval $(call KernelPackage,ide-core))

define KernelPackage/ide-pdc202xx
TITLE:=PDC202xx IDE driver
DESCRIPTION:=PDC202xx IDE driver
KCONFIG:=$(CONFIG_BLK_DEV_PDC202XX_OLD)
FILES:=$(MODULES_DIR)/kernel/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef
$(eval $(call KernelPackage,ide-pdc202xx))

