#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=pwm-gpio-custom
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/pwm-gpio-custom
  SUBMENU:=Other modules
  TITLE:=Custom GPIO-based PWM device
  DEPENDS:=+kmod-pwm-gpio
  FILES:=$(PKG_BUILD_DIR)/pwm-gpio-custom.ko
  KCONFIG:=
endef

define KernelPackage/pwm-gpio-custom/description
 Kernel module to register a custom pwm-gpio platform device.
endef

EXTRA_KCONFIG:= \
	CONFIG_PWM_GPIO_CUSTOM=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,pwm-gpio-custom))

