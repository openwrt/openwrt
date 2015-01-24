#
# Copyright (C) 2006-2014 OpenWrt.org
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
 VID/VRM/VRD voltage conversion module for hardware monitoring
endef

$(eval $(call KernelPackage,hwmon-vid))


define KernelPackage/hwmon-adt7410
  TITLE:=ADT7410 monitoring support
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.10.0)),1)
  KCONFIG:= \
	CONFIG_SENSORS_ADT7X10 \
	CONFIG_SENSORS_ADT7410
  FILES:= \
	$(LINUX_DIR)/drivers/hwmon/adt7x10.ko \
	$(LINUX_DIR)/drivers/hwmon/adt7410.ko
  AUTOLOAD:=$(call AutoLoad,60,adt7x10 adt7410)
else
  KCONFIG:=CONFIG_SENSORS_ADT7410
  FILES:=$(LINUX_DIR)/drivers/hwmon/adt7410.ko
  AUTOLOAD:=$(call AutoLoad,60,adt7410)
endif
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-adt7410/description
 Kernel module for ADT7410/7420 I2C thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-adt7410))


define KernelPackage/hwmon-adt7475
  TITLE:=ADT7473/7475/7476/7490 monitoring support
  KCONFIG:=CONFIG_SENSORS_ADT7475
  FILES:=$(LINUX_DIR)/drivers/hwmon/adt7475.ko
  AUTOLOAD:=$(call AutoProbe,adt7475)
  $(call AddDepends/hwmon,+kmod-i2c-core +kmod-hwmon-vid)
endef

define KernelPackage/hwmon-adt7475/description
 Kernel module for ADT7473/7475/7476/7490 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-adt7475))


define KernelPackage/hwmon-ina2xx
  TITLE:=INA2XX monitoring support
  KCONFIG:=CONFIG_SENSORS_INA2XX
  FILES:=$(LINUX_DIR)/drivers/hwmon/ina2xx.ko
  AUTOLOAD:=$(call AutoProbe,ina2xx)
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
  AUTOLOAD:=$(call AutoProbe,lm63)
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
  AUTOLOAD:=$(call AutoProbe,lm75)
  $(call AddDepends/hwmon,+kmod-i2c-core +PACKAGE_kmod-thermal:kmod-thermal)
endef

define KernelPackage/hwmon-lm75/description
 Kernel module for lm75 thermal monitor chip
endef

$(eval $(call KernelPackage,hwmon-lm75))


define KernelPackage/hwmon-lm77
  TITLE:=LM77 monitoring support
  KCONFIG:=CONFIG_SENSORS_LM77
  FILES:=$(LINUX_DIR)/drivers/hwmon/lm77.ko
  AUTOLOAD:=$(call AutoProbe,lm77)
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
  AUTOLOAD:=$(call AutoProbe,lm85)
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
  AUTOLOAD:=$(call AutoProbe,lm90)
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
  AUTOLOAD:=$(call AutoProbe,lm92)
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
  AUTOLOAD:=$(call AutoProbe,lm95241)
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
  AUTOLOAD:=$(call AutoProbe,sht21)
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
  AUTOLOAD:=$(call AutoProbe,pc87360)
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

define KernelPackage/hwmon-w83627hf/description
  Kernel module for the Winbond W83627HF chips.
endef

$(eval $(call KernelPackage,hwmon-w83627hf))


define KernelPackage/hwmon-gsc
  TITLE:=Gateworks GSC monitoring support
  KCONFIG:=CONFIG_SENSORS_GSC
  FILES:=$(LINUX_DIR)/drivers/hwmon/gsc.ko
  AUTOLOAD:=$(call AutoLoad,60,gsc)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-gsc/description
  Kernel module for the Gateworks System Controller chips.
endef

$(eval $(call KernelPackage,hwmon-gsc))


define KernelPackage/hwmon-tmp421
  TITLE:=TI TMP421 and compatible monitoring support
  KCONFIG:=CONFIG_SENSORS_TMP421
  FILES:=$(LINUX_DIR)/drivers/hwmon/tmp421.ko
  AUTOLOAD:=$(call AutoLoad,60,tmp421)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-tmp421/description
  Kernel module for the Texas Instruments TMP421 and compatible chips.
endef

$(eval $(call KernelPackage,hwmon-tmp421))


define KernelPackage/hwmon-gpiofan
  TITLE:=Generic GPIO FAN support
  KCONFIG:=CONFIG_SENSORS_GPIO_FAN
  FILES:=$(LINUX_DIR)/drivers/hwmon/gpio-fan.ko
  AUTOLOAD:=$(call AutoLoad,60,gpio-fan)
  $(call AddDepends/hwmon,+kmod-i2c-core)
endef

define KernelPackage/hwmon-gpiofan/description
  Kernel module for GPIO controlled FANs
endef

$(eval $(call KernelPackage,hwmon-gpiofan))


define KernelPackage/hwmon-pwmfan
  TITLE:=Generic PWM FAN support
  KCONFIG:=CONFIG_SENSORS_PWM_FAN
  FILES:=$(LINUX_DIR)/drivers/hwmon/pwm-fan.ko
  AUTOLOAD:=$(call AutoLoad,60,pwm-fan)
  $(call AddDepends/hwmon,)
  DEPENDS+=@!LINUX_3_8 @!LINUX_3_10 @!LINUX_3_14
endef

define KernelPackage/hwmon-pwmfan/description
  Kernel module for PWM controlled FANs
endef

$(eval $(call KernelPackage,hwmon-pwmfan))


define KernelPackage/hwmon-k10temp
  TITLE:=AMD Family 10h+ temperature sensor
  KCONFIG:=CONFIG_SENSORS_K10TEMP
  FILES:=$(LINUX_DIR)/drivers/hwmon/k10temp.ko
  AUTOLOAD:=$(call AutoLoad,60,k10temp)
  $(call AddDepends/hwmon,@PCI_SUPPORT @(x86||x86_64))
endef

define KernelPackage/hwmon-k10temp/description
  Thermal sensor support for AMD 10h, 11h, 12h (Llano), 14h (Brazos),
  15h (Bulldozer/Trinity/Kaveri) and 16h (Kabini/Mullins) CPUs
endef

$(eval $(call KernelPackage,hwmon-k10temp))
