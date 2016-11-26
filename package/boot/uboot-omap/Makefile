#
# Copyright (C) 2012-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2015.10
PKG_RELEASE:=1

PKG_HASH:=bdc68d5f9455ad933b059c735d983f2c8b6b552dafb062e5ff1444f623021955

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=omap
  BUILD_DEVICES:=Default
  UBOOT_IMAGE:=u-boot.img MLO
endef

define U-Boot/omap4_panda
  NAME:=Pandaboard
endef

define U-Boot/am335x_evm
  NAME:=AM335x EVM
endef

define U-Boot/omap3_overo
  NAME:=Gumstix Overo
endef

define U-Boot/omap3_beagle
  NAME:=BeagleBoard
endef

UBOOT_TARGETS:=omap4_panda am335x_evm omap3_overo omap3_beagle
UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

$(eval $(call BuildPackage/U-Boot))
