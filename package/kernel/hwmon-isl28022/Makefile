#
# Copyright (C) 2023 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hwmon-isl28022
PKG_RELEASE:=1
PKG_MAINTAINER:=Carsten Spieß <mail@carsten-spiess.de
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/hwmon-isl28022
  SUBMENU:=Hardware Monitoring Support
  TITLE:= hwmon ISL28022 support
  DEPENDS:= +kmod-hwmon-core +kmod-i2c-core +kmod-regmap-i2c
  FILES:=$(PKG_BUILD_DIR)/isl28022.ko
  AUTOLOAD:=$(call AutoLoad,30,isl28022,1)
endef

define KernelPackage/hwmon-isl28022/description
  Driver for ISL28022 power monitor.
endef

define Build/Compile
	$(KERNEL_MAKE) \
		$(MAKE_OPTS) \
		M="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,hwmon-isl28022))
