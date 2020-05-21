#
# Copyright (C) 2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpio-nxp-74hc153
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpio-nxp-74hc153
  SUBMENU:=Other modules
  TITLE:= NXP 74HC153 GPIO expander support
  FILES:=$(PKG_BUILD_DIR)/gpio-nxp-74hc153.ko
  AUTOLOAD:=$(call AutoLoad,30,gpio-nxp-74hc153,1)
  KCONFIG:=
  DEPENDS:= @GPIO_SUPPORT
endef

define KernelPackage/gpio-nxp-74hc153/description
  Platform driver for NXP 74HC153 Dual 4-input Multiplexer.
  This provides a GPIO interface supporting input mode only.
endef

EXTRA_KCONFIG:= \
	CONFIG_GPIO_NXP_74HC153=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,gpio-nxp-74hc153))

