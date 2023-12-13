#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=emc2305-nvidia
PKG_RELEASE:=0
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/emc2305
  SUBMENU:=Hardware Monitoring Support
  TITLE:=emc2305-nvidia
  DEPENDS:=+kmod-i2c-core +kmod-hwmon-core +kmod-regmap-i2c
  FILES:=$(PKG_BUILD_DIR)/emc2305.ko
  AUTOLOAD:=$(call AutoLoad,80,emc2305,1)
  KCONFIG:=
endef

define KernelPackage/emc2305/description
	Kernel package for emc2305 fan controller for the Xiaomi AX9000. The source code is backported from kernel 6.1
endef

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="-Wno-discarded-qualifiers -Wno-incompatible-pointer-types" \
	$(EXTRA_KCONFIG)


define Build/Compile
	$(KERNEL_MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,emc2305))
