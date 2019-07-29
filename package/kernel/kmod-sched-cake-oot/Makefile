#
# Copyright (C) 2016 LEDE
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sched-cake-oot
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/dtaht/sch_cake.git
PKG_SOURCE_DATE:=2020-01-10
PKG_SOURCE_VERSION:=aeff7a3e61ffa3159a8a95384d33e47a006211f4
PKG_MIRROR_HASH:=5bf06a804824db36ae393fc174aeec7b12633176e05a765c0931b39df5bd34df
PKG_MAINTAINER:=Kevin Darbyshire-Bryant <ldir@darbyshire-bryant.me.uk>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/sched-cake-oot
  SUBMENU:=Network Support
  TITLE:=OOT Cake fq_codel/blue derived shaper
  URL:=https://github.com/dtaht/sch_cake
  FILES:=$(PKG_BUILD_DIR)/sch_cake.ko
  AUTOLOAD:=$(call AutoLoad,75,sch_cake)
  DEPENDS:=@LINUX_4_14 +kmod-sched-core +kmod-ipt-conntrack
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define KernelPackage/sched-cake/description
  O(ut) O(f) T(ree) Common Applications Kept Enhanced fq_codel/blue derived shaper
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,sched-cake-oot))
