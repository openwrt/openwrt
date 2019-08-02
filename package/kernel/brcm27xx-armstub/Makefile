#
# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=brcm27xx-armstub

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/raspberrypi/tools
PKG_SOURCE_DATE:=2019-05-09
PKG_SOURCE_VERSION:=49719d5544cd33b8c146235e1420f68cd92420fe
PKG_MIRROR_HASH:=91e9d14b5f3d7ce7486de7199d15234ce46ead3d87decce77466cd2f28b9e085

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/brcm27xx-armstub
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=brcm27xx-armstub
  DEPENDS:=@TARGET_brcm2708
endef

define Package/brcm27xx-armstub/description
  ARM stubs for brcm27xx.
endef

MAKE_PATH = armstubs

MAKE_FLAGS += \
	CC8=$(TARGET_CC) \
	LD8=$(TARGET_CROSS)ld \
	OBJCOPY8=$(TARGET_CROSS)objcopy \
	OBJDUMP8=$(TARGET_CROSS)objdump

define Build/Compile
	$(call Build/Compile/Default,armstub8-gic.elf armstub8-gic.bin)
endef

define Build/InstallDev
	$(CP) $(PKG_BUILD_DIR)/armstubs/armstub8-gic.bin $(KERNEL_BUILD_DIR)
endef

define Package/brcm27xx-armstub/install
	true
endef

$(eval $(call BuildPackage,brcm27xx-armstub))
