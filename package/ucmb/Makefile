#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ucmb
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk


define KernelPackage/ucmb
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6 +kmod-spi-gpio +kmod-crc16
  KCONFIG:=CONFIG_SPI=y \
          CONFIG_SPI_MASTER=y
  TITLE:=Microcontroller Message Bus (EXPERIMENTAL)
  FILES:=$(PKG_BUILD_DIR)/ucmb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,93,ucmb)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./driver/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,ucmb))
