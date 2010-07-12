#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=spi-ks8995
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/spi-ks8995
  SUBMENU:=SPI Support
  TITLE:=Micrel/Kendin KS8995 Ethernet switch control
  FILES:=$(PKG_BUILD_DIR)/spi_ks8995.ko
  KCONFIG:=CONFIG_SPI=y \
	CONFIG_SPI_MASTER=y
  AUTOLOAD:=$(call AutoLoad,50,spi_ks8995)
endef

define KernelPackage/spi-ks8995/description
  Kernel module for Micrel/Kendin KS8995 ethernet switch
endef

EXTRA_KCONFIG:= \
	CONFIG_SPI_KS8995=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

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

$(eval $(call KernelPackage,spi-ks8995))
