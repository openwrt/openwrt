#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_SOURCE_URL:=https://github.com/chunkeey/FritzBox-4040-UBOOT
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=572ff7ff5a8b98022e75c4cca25fdede90eda1c3
PKG_SOURCE_DATE:=2019-09-07
PKG_MIRROR_HASH:=72a63dd6d4a968d0736938d1c7e1dda2afc54475c5ff456eff0f56c6bd6b84e9

PKG_RELEASE:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=ipq40xx
  BUILD_SUBTARGET:=generic
  UBOOT_BOARD:=$(1)
  UBOOT_IMAGE:=uboot-$(1).bin
endef

define U-Boot/fritz3000
  NAME:=FritzRepeater 3000
  BUILD_DEVICES:=avm_fritzrepeater-3000
endef

define U-Boot/fritz4040
  NAME:=FritzBox 4040
  BUILD_DEVICES:=avm_fritzbox-4040
endef

define U-Boot/fritz7530
  NAME:=FritzBox 7530
  BUILD_DEVICES:=avm_fritzbox-7530
endef

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes
UBOOT_MAKE_FLAGS = USE_PRIVATE_LIBGCC=yes
export DTC

define Build/Configure
	$(Build/Configure/U-Boot)
	$(HOSTCC) $(HOST_CFLAGS) $(HOST_LDFLAGS) -o $(PKG_BUILD_DIR)/fritz/lzma2eva $(PKG_BUILD_DIR)/fritz/src/lzma2eva.c -lz
	$(HOSTCC) $(HOST_CFLAGS) $(HOST_LDFLAGS) -o $(PKG_BUILD_DIR)/fritz/tichksum $(PKG_BUILD_DIR)/fritz/src/tichksum.c
	ln -sf $(STAGING_DIR_HOST)/bin/lzma $(PKG_BUILD_DIR)/fritz
endef

define Build/Compile
	$(Build/Compile/U-Boot)
	(cd $(PKG_BUILD_DIR); ./fritz/fritzcreator.sh $(UBOOT_BOARD);)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(UBOOT_IMAGE)
endef

define Package/u-boot/install
	$(Package/u-boot/install/default)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/upload-to-f4040.sh $(1)/
endef

UBOOT_TARGETS := fritz3000 fritz4040 fritz7530

$(eval $(call BuildPackage/U-Boot))
