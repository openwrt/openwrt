# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=pwc
PKG_VERSION:=10.0.12-rc1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.saillard.org/linux/pwc/files/
PKG_MD5SUM:=8763f3d6fd0f9738ef9854de205a126d

include $(INCLUDE_DIR)/package.mk

define KernelPackage/pwc
  TITLE:=USB Philips camera driver
  DESCRIPTION:=Kernel support for USB Philips camera
  SUBMENU:=USB Support
  DEPENDS:=@BROKEN @LINUX_2_6 @VIDEO_SUPPORT
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/pwc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,$(shell cat ./files/pwc.modules))
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		PATH="$(TARGET_PATH)" \
                KVER="$(LINUX_VERSION)" \
                KSRC="$(LINUX_DIR)" \
                SUBDIRS="$(PKG_BUILD_DIR)" \
                KERNEL_DIR="$(KERNEL_DIR)" \
                ARCH="$(LINUX_KARCH)" \
                CROSS_COMPILE="$(TARGET_CROSS)" \
		modules
endef

$(eval $(call KernelPackage,pwc))
