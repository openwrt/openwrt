#
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

HWMON_MENU:=Hardware Monitoring Support

define KernelPackage/hwmon-core
  SUBMENU:=$(HWMON_MENU)
  TITLE:=Hardware monitoring support
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_HWMON \
	CONFIG_HWMON_VID \
	CONFIG_HWMON_DEBUG_CHIP=n
  FILES:= \
	$(LINUX_DIR)/drivers/hwmon/hwmon.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/hwmon/hwmon-vid.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,hwmon hwmon-vid)
endef

define KernelPackage/hwmon-core/description
 Kernel modules for hardware monitoring
endef

$(eval $(call KernelPackage,hwmon-core))


define KernelPackage/hwmon-lm77
  SUBMENU:=$(HWMON_MENU)
  TITLE:=LM77 monitoring support
  DEPENDS:=kmod-hwmon-core
  KCONFIG:=CONFIG_SENSORS_LM77
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm77.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,lm77)
endef
 
define KernelPackage/hwmon-lm77/description
 Kernel module for LM77 thermal monitor chip 
endef

$(eval $(call KernelPackage,hwmon-lm77))


define KernelPackage/hwmon-pc87360
  SUBMENU:=$(HWMON_MENU)
  TITLE:=PC87360 monitoring support
  DEPENDS:=kmod-hwmon-core
  KCONFIG:=CONFIG_SENSORS_PC87360
  FILES:=$(LINUX_DIR)/drivers/hwmon/pc87360.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,pc87360)
endef

define KernelPackage/hwmon-pc87360/description
 Kernel modules for PC87360 chips
endef

$(eval $(call KernelPackage,hwmon-pc87360))

