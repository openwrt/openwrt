# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hasivo-stc8-hwmon
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define KernelPackage/hasivo-stc8-hwmon
  SUBMENU:=Hardware Monitoring Support
  TITLE:=Hasivo STC8 microcontroller hwmon
  FILES:=$(PKG_BUILD_DIR)/hasivo-stc8-hwmon.ko
  AUTOLOAD:=$(call AutoProbe,hasivo-stc8-hwmon)
  DEPENDS:=@TARGET_realtek +kmod-hwmon-core +kmod-mfd-hasivo-stc8 +kmod-regmap-i2c
endef

define KernelPackage/hasivo-stc8-hwmon/description
  hwmon child driver for the Hasivo STC8 microcontroller MFD. Exposes
  CPU and system temperatures plus a 3-state fan control knob
  (auto / force-on / force-off) and a fan-running alarm. The MCU
  manages fan speed internally; there is no host-side PWM duty
  register and no tach feedback register.

  The driver binds to a child node of the hasivo,stc8-mfd that
  carries compatible = "hasivo,stc8-hwmon".
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,hasivo-stc8-hwmon))
