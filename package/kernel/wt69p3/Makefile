include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=wt69p3
PKG_RELEASE:=1
PKG_MAINTAINER:=Lutz Müller <github@topfrose.de>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk

define KernelPackage/wt69p3
  SUBMENU:=Other modules
  TITLE:=Weltrend Control MCU serdev GPIO, hwmon & RTC driver
  DEPENDS:=+@TARGET_mvebu_cortexa9 +kmod-hwmon-core
  KCONFIG:= \
	  CONFIG_SERIAL_DEV_BUS=y \
	  CONFIG_SERIAL_DEV_CTRL_TTYPORT=y
  FILES:=$(PKG_BUILD_DIR)/wt69p3.ko
  AUTOLOAD:=$(call AutoProbe,wt69p3)
endef

define KernelPackage/wt69p3/description
The Weltrend external micro-controller-unit controls the power led, shutdown, wake-on-lan, RTC and board temperature.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,wt69p3))
