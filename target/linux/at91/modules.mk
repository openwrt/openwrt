#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/mmc-at91
  SUBMENU:=$(OTHER_MENU)
  TITLE:=MMC/SD Card Support on AT91
  DEPENDS:=@TARGET_at91 +kmod-mmc
  KCONFIG:=CONFIG_MMC_AT91
  FILES:=$(LINUX_DIR)/drivers/mmc/host/at91_mci.ko
  AUTOLOAD:=$(call AutoLoad,90,at91_mci,1)
endef

define KernelPackage/mmc-at91/description
 Kernel support for MMC/SD cards on the AT91 target
endef

$(eval $(call KernelPackage,mmc-at91))

define KernelPackage/pwm-atmel
  SUBMENU:=$(OTHER_MENU)
  TITLE:=PWM on atmel SoC
  DEPENDS:=@TARGET_at91
  KCONFIG:=CONFIG_GENERIC_PWM \
		CONFIG_ATMEL_PWM
  FILES:=$(LINUX_DIR)/drivers/pwm/atmel-pwm.ko
  AUTOLOAD:=$(call AutoLoad,51,atmel-pwm)
endef

define KernelPackage/pwm-atmel/description
 Kernel module to use the PWM channel on ATMEL SoC
endef

$(eval $(call KernelPackage,pwm-atmel))
