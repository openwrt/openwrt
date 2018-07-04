#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uboot-layerscape
PKG_VERSION:=lsdk-1806
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://source.codeaurora.org/external/qoriq/qoriq-components/u-boot
PKG_SOURCE_VERSION:=b3f98d438eefd1b355efdec0b50af5813ff8d0e1
PKG_MIRROR_HASH:=59e35957055658e7fdefb16b92f954356985a8191942d93f254861c75c47b32b

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=layerscape
  BUILD_SUBTARGET:=armv8_64b
  BUILD_DEVICES:=$(1)
  UBOOT_IMAGE:=u-boot-dtb.bin
endef

define U-Boot/ls1012ardb
  NAME:=NXP LS1012ARDB
  UBOOT_CONFIG:=ls1012ardb_qspi
endef

define U-Boot/ls1012afrdm
  NAME:=NXP LS1012AFRDM
  UBOOT_CONFIG:=ls1012afrdm_qspi
endef

define U-Boot/ls1043ardb
  NAME:=NXP LS1043ARDB
endef

define U-Boot/ls1046ardb
  NAME:=NXP LS1046ARDB
  UBOOT_CONFIG:=ls1046ardb_qspi
endef

define U-Boot/ls1088ardb
  NAME:=NXP LS1088ARDB
  UBOOT_CONFIG:=ls1088ardb_qspi
endef

define U-Boot/ls2088ardb
  NAME:=NXP LS2088ARDB
  UBOOT_CONFIG:=ls2080ardb
endef

UBOOT_TARGETS := \
  ls1012ardb \
  ls1012afrdm \
  ls1043ardb \
  ls1046ardb \
  ls1088ardb \
  ls2088ardb

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) \
		$(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot.bin
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
