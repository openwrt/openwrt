# SPDX-License-Identifier: GPL-2.0-only

OTHER_MENU:=Other modules


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


