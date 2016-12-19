#
# Copyright (C) 2016 LEDE
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sched-cake
PKG_SOURCE_VERSION:=70c8eb766d5afcf3cf187594b7cd776da92bee3c
PKG_VERSION:=2016-12-19-$(PKG_SOURCE_VERSION)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/kdarbyshirebryant/sch_cake.git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_MIRROR_HASH:=fcac68c73a822e67819bd3206a14f19159275fae19ab3cf01dbbcffaa975cb47
PKG_MAINTAINER:=Kevin Darbyshire-Bryant <kevin@darbyshire-bryant.me.uk>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/sched-cake
  SUBMENU:=Network Support
  TITLE:=Cake fq_codel/blue derived shaper
  URL:=https://github.com/dtaht/sch_cake
  FILES:=$(PKG_BUILD_DIR)/sch_cake.ko
  AUTOLOAD:=$(call AutoLoad,75,sch_cake)
  DEPENDS:=+kmod-ipt-conntrack
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define KernelPackage/sched-cake/description
  Common Applications Kept Enhanced fq_codel/blue derived shaper
endef

define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) SUBDIRS="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,sched-cake))
