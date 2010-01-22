#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

W1_MENU:=W1 support
W1_MASTERS_DIR:=$(LINUX_DIR)/drivers/w1/masters
W1_SLAVES_DIR:=$(LINUX_DIR)/drivers/w1/slaves

define KernelPackage/w1
  SUBMENU:=$(W1_MENU)
  TITLE:=Dallas's 1-wire support
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_W1
  FILES:=$(LINUX_DIR)/drivers/w1/wire.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,wire)
endef

define KernelPackage/w1/description
  Kernel module for Dallas's 1-wire support
endef

$(eval $(call KernelPackage,w1))


define KernelPackage/w1/Depends
  SUBMENU:=$(W1_MENU)
  DEPENDS+=kmod-w1 $(1)
endef


#
# 1-wire masters
#
define KernelPackage/w1-master-gpio
$(call KernelPackage/w1/Depends,@GPIO_SUPPORT)
  TITLE:=GPIO 1-wire bus master driver
  KCONFIG:=CONFIG_W1_MASTER_GPIO
  FILES:=$(W1_MASTERS_DIR)/w1-gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,w1-gpio)
endef

define KernelPackage/w1-master-gpio/description
  Kernel module for the GPIO 1-wire bus master driver
endef

$(eval $(call KernelPackage,w1-master-gpio))


#
# 1-wire slaves
#
define KernelPackage/w1-slave-therm
$(call KernelPackage/w1/Depends,)
  TITLE:=Thermal family implementation
  KCONFIG:=CONFIG_W1_SLAVE_THERM
  FILES:=$(W1_SLAVES_DIR)/w1_therm.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,w1_therm)
endef

define KernelPackage/w1-slave-therm/description
  Kernel module for 1-wire thermal sensors
endef

$(eval $(call KernelPackage,w1-slave-therm))


define KernelPackage/w1-slave-smem
$(call KernelPackage/w1/Depends,)
  TITLE:=Simple 64bit memory family implementation
  KCONFIG:=CONFIG_W1_SLAVE_SMEM
  FILES:=$(W1_SLAVES_DIR)/w1_smem.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,w1_smem)
endef

define KernelPackage/w1-slave-smem/description
  Kernel module for 1-wire simple 64bit memory rom(ds2401/ds2411/ds1990*)
endef

$(eval $(call KernelPackage,w1-slave-smem))

define KernelPackage/w1-slave-ds2433
$(call KernelPackage/w1/Depends,)
  TITLE:=Simple 64bit memory family implementation
  KCONFIG:= \
	CONFIG_W1_SLAVE_DS2433 \
	CONFIG_W1_SLAVE_DS2433_CRC=n
  FILES:=$(W1_SLAVES_DIR)/w1_ds2433.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,w1_ds2433)
endef

define KernelPackage/w1-slave-ds2433/description
  Kernel module for 1-wire simple 64bit memory rom(ds2401/ds2411/ds1990*)
endef

$(eval $(call KernelPackage,w1-slave-ds2433))


define KernelPackage/w1-slave-ds2760
$(call KernelPackage/w1/Depends,)
  TITLE:=Dallas 2760 battery monitor chip (HP iPAQ & others)
  KCONFIG:= \
	CONFIG_W1_SLAVE_DS2760 \
	CONFIG_W1_SLAVE_DS2433_CRC=n
  FILES:=$(W1_SLAVES_DIR)/w1_ds2760.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,w1_ds2760)
endef

define KernelPackage/w1-slave-ds2760/description
  Kernel module for 1-wire DS2760 battery monitor chip support
endef

$(eval $(call KernelPackage,w1-slave-ds2760))
