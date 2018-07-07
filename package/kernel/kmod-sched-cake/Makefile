#
# Copyright (C) 2016 LEDE
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sched-cake
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/dtaht/sch_cake.git
PKG_SOURCE_DATE:=2018-07-06
PKG_SOURCE_VERSION:=c91b94f0b4456c43def2e77248a455a9a2449ed1
PKG_MIRROR_HASH:=e1b7081e1b559a9c01721ffc856debdb91e36488b51b9facb7db73f40ab85914
PKG_MAINTAINER:=Kevin Darbyshire-Bryant <ldir@darbyshire-bryant.me.uk>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/sched-cake
  SUBMENU:=Network Support
  TITLE:=Cake fq_codel/blue derived shaper
  URL:=https://github.com/dtaht/sch_cake
  FILES:=$(PKG_BUILD_DIR)/sch_cake.ko
  AUTOLOAD:=$(call AutoLoad,75,sch_cake)
  DEPENDS:=+kmod-ipt-conntrack @!LINUX_3_18
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define KernelPackage/sched-cake/description
  Common Applications Kept Enhanced fq_codel/blue derived shaper
endef

define Build/Compile
	$(KERNEL_MAKE) SUBDIRS="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,sched-cake))
