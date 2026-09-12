#
# Copyright (C) openwrt.org
#
# This is free software, licensed under the GNU General Public License v2.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=leds-aw9106
PKG_RELEASE:=1

PKG_MAINTAINER:=Ardeshir Barani<barani.ardeshir@gmail.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-aw9106
  SUBMENU:=LED modules
  TITLE:=Awinic AW9106B multi-channel I2C LED driver
  DEPENDS:=+kmod-i2c-core +kmod-leds-gpio
  FILES:=$(PKG_BUILD_DIR)/leds-aw9106.ko
  AUTOLOAD:=$(call AutoProbe,leds-aw9106)
endef

define KernelPackage/leds-aw9106/description
  Kernel driver for the Awinic AW9106B 6-channel I2C LED/GPIO
  controller. Registers one independent led_classdev per DT child
  node (one per output channel), so each LED die is individually
  controllable and visible under /sys/class/leds.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	echo "obj-m += leds-aw9106.o" > $(PKG_BUILD_DIR)/Makefile
endef

# NOTE: modern kernels (this target builds 6.x) removed SUBDIRS= entirely —
# out-of-tree module directories must be pointed at with M= instead.
# $(KERNEL_MAKE) already carries -C $(LINUX_DIR) plus ARCH/CROSS_COMPILE,
# so nothing else needs to be passed here.
define Build/Compile
	+$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,leds-aw9106))
