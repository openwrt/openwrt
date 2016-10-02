#
# Copyright (C) 2016 LEDE
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sched-cake
PKG_SOURCE_VERSION:=4f62bd17fa34036cb3c8fd4800a709b2734c3de3
PKG_VERSION:=2016-10-02-$(PKG_SOURCE_VERSION)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/dtaht/sch_cake.git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
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
