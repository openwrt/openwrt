# SPDX-License-Identifier: GPL-2.0-only

OTHER_MENU:=Other modules

I2C_MT7621_MODULES:= \
  CONFIG_I2C_MT7621:drivers/i2c/busses/i2c-mt7621

define KernelPackage/i2c-an7581
  SUBMENU:=$(OTHER_MENU)
  $(call i2c_defaults,$(I2C_MT7621_MODULES),79)
  TITLE:=Airoha I2C Controller
  DEPENDS:=+kmod-i2c-core @TARGET_airoha
endef

define KernelPackage/i2c-an7581/description
 Kernel modules for enable mt7621 i2c controller.
endef

$(eval $(call KernelPackage,i2c-an7581))


define KernelPackage/pwm-airoha
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Airoha PWM
  DEPENDS:=@TARGET_airoha
  KCONFIG:= \
        CONFIG_PWM=y \
        CONFIG_PWM_AIROHA=y \
        CONFIG_PWM_SYSFS=y
  FILES:= \
        $(LINUX_DIR)/drivers/pwm/pwm-airoha.ko
  AUTOLOAD:=$(call AutoProbe,pwm-airoha)
endef

define KernelPackage/pwm-airoha/description
  Kernel module to use the PWM channel on Airoha SoC
endef

$(eval $(call KernelPackage,pwm-airoha))


define KernelPackage/phy-airoha-en8801s
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Airoha EN8801S 1GbE Ethernet PHY
  KCONFIG:=CONFIG_AIROHA_EN8801s_PHY=y
  FILES:= \
   $(LINUX_DIR)/drivers/net/phy/en8801sc.o.ko
  AUTOLOAD:=$(call AutoLoad,18,en8801sc.o,1)
endef

define KernelPackage/phy-airoha-en8801s/description
  Kernel modules for Airoha EN8801S 1GbE Ethernet PHY
endef

$(eval $(call KernelPackage,phy-airoha-en8801s))


