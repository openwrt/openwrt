#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=r2ec
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/r2ec
	SUBMENU:=Other modules
	TITLE:=STM32 R2EC (Router to Embedded board Communication) Driver
	FILES:=$(PKG_BUILD_DIR)/r2ec.ko
	AUTOLOAD:=$(call AutoLoad,30,r2ec,1)
	DEPENDS:=@TARGET_ipq40xx @LINUX_5_4
	KCONFIG:=
endef

MAKE_OPTS:= $(KERNEL_MAKE_FLAGS) M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,r2ec))
