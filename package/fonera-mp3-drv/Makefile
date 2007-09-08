# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6501 2007-03-04 04:41:46Z pavlov $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fonera-mp3-driver
PKG_RELEASE:=1

PKG_BUILD_DIR := $(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/fonera-mp3-driver
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_atheros
  TITLE:=Fonera-MP3 driver (VS1011X)
  FILES:=$(PKG_BUILD_DIR)/mp3_drv.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,25,mp3-drv)
endef

define KernelPackage/fonera-mp3-driver/description
 This package a kernel driver for a vs1011X attached to a fonera.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -r src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" V="$(V)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SOURCES="$(LINUX_DIR)" \
		KDIR="$(LINUX_DIR)"
endef

$(eval $(call KernelPackage,fonera-mp3-driver))
