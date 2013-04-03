#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

HWMON_MENU:=Hardware Monitoring Support

define KernelPackage/hwmon-core
  SUBMENU:=$(HWMON_MENU)
  TITLE:=Hardware monitoring support
  KCONFIG:= \
	CONFIG_HWMON \
	CONFIG_HWMON_DEBUG_CHIP=n
  FILES:= \
	$(LINUX_DIR)/drivers/hwmon/hwmon.ko
  AUTOLOAD:=$(call AutoLoad,40,hwmon)
endef

define KernelPackage/hwmon-core/description
 Kernel modules for hardware monitoring
endef

$(eval $(call KernelPackage,hwmon-core))


define AddDepends/hwmon
  SUBMENU:=$(HWMON_MENU)
  DEPENDS:=kmod-hwmon-core $(1)
endef

define KernelPackage/hwmon-vid
  TITLE:=VID/VRM/VRD voltage conversion module.
  KCONFIG:=CONFIG_HWMON_VID
  FILES:=$(LINUX_DIR)/drivers/hwmon/hwmon-vid.ko
  AUTOLOAD:=$(call AutoLoad,41,hwmon-vid)
  $(call AddDepends/hwmon,)
endef

define KernelPackage/hwmon-vid/description
  VID/VRM/VRD voltage conversion module for hardware monitoring.
endef

$(eval $(call KernelPackage,hwmon-vid))


define KernelPackage/hwmon-adt7475
  TITLE:=ADT7473/7475/7476/7490 monitoring support
  KCONFIG:=CONFIG_SENSORS_ADT7475
  FILES:=$(LINUX_DIR)/drivers/hwmon/adt7475.ko
  AUTOLOAD:=$(call AutoLoad,60,adt7475)
  $(call AddDepends/hwmon,+kmod-i2c-core +kmod-hwmon-vid)
endef

define KernelPackage/hwmon-adt7475/description
  Kernel module for ADT7473/7475/7476/7490 thermal monitor chip.
endef

$(eval $(call KernelPackage,hwmon-adt7475))


define KernelPackage/hwmon-ina2xx
  TITLE:=INA2XX monitoring support
  KCONFIG:=CONFIG_SENSORS_INA2XX
  FILES:=$(LINUX_DIR)/drivers/hwmon/ina2xx.ko
  AUTOLOAD:=$(call AutoLoad,60,ina2xx)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-ina2xx/description
 Kernel module for ina2xx dc current monitor chips
endef

$(eval $(call KernelPackage,hwmon-ina2xx))


define KernelPackage/hwmon-lm63
  TITLE:=LM63/64 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM63
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm63.ko
  AUTOLOAD:=$(call AutoLoad,60,lm63)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-lm63/description
 Kernel module for lm63 and lm64 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm63))


define KernelPackage/hwmon-lm75
  TITLE:=LM75 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM75
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm75.ko
  AUTOLOAD:=$(call AutoLoad,60,lm75)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-lm75/description
 Kernel module for lm75 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm75))


define KernelPackage/hwmon-lm77
  TITLE:=LM77 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM77
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm77.ko
  AUTOLOAD:=$(call AutoLoad,60,lm77)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-lm77/description
 Kernel module for LM77 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm77))


define KernelPackage/hwmon-lm85
  TITLE:=LM85 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM85
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm85.ko
  AUTOLOAD:=$(call AutoLoad,60,lm85)
  $(call AddDepends/hwmon,+kmod-i2c-core +kmod-hwmon-vid)
endef

define KernelPackage/hwmon-lm85/description
 Kernel module for LM85 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm85))


define KernelPackage/hwmon-lm90
  TITLE:=LM90 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM90
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm90.ko
  AUTOLOAD:=$(call AutoLoad,60,lm90)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-lm90/description
 Kernel module for LM90 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm90))

define KernelPackage/hwmon-lm92
  TITLE:=LM92 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM92
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm92.ko
  AUTOLOAD:=$(call AutoLoad,60,lm92)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-lm92/description
 Kernel module for LM92 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm92))

define KernelPackage/hwmon-lm95241
  TITLE:=LM95241 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM95241
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm95241.ko
  AUTOLOAD:=$(call AutoLoad,60,lm95241)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-lm95241/description
 Kernel module for LM95241 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm95241))

define KernelPackage/hwmon-sht21
  TITLE:=Sensiron SHT21 and compat. monitoring support
  KCONFIG:=CONFIG_SENSORS_SHT21
  FILES:=$(LINUX_DIR)/drivers/hwmon/sht21.ko
  AUTOLOAD:=$(call AutoLoad,60,sht21)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-sht21/description
 Kernel module for Sensirion SHT21 and SHT25 temperature and humidity sensors chip
endef

$(eval $(call KernelPackage,hwmon-sht21))

define KernelPackage/hwmon-pc87360
  TITLE:=PC87360 monitoring support
  KCONFIG:=CONFIG_SENSORS_PC87360
  FILES:=$(LINUX_DIR)/drivers/hwmon/pc87360.ko
  AUTOLOAD:=$(call AutoLoad,50,pc87360)
  $(call AddDepends/hwmon,@TARGET_x86 +kmod-hwmon-vid)
endef

define KernelPackage/hwmon-pc87360/description
 Kernel modules for PC87360 chips
endef

$(eval $(call KernelPackage,hwmon-pc87360))


define KernelPackage/hwmon-w83627hf
  TITLE:=Winbond W83627HF monitoring support
  KCONFIG:=CONFIG_SENSORS_W83627HF
  FILES:=$(LINUX_DIR)/drivers/hwmon/w83627hf.ko
  AUTOLOAD:=$(call AutoLoad,50,w83627hf)
$(call AddDepends/hwmon,@TARGET_rdc||TARGET_x86 +kmod-hwmon-vid)
endef

define KernelPacakge/hwmon-w83627hf/description
  Kernel module for the Winbond W83627HF chips.
endef

$(eval $(call KernelPackage,hwmon-w83627hf))
