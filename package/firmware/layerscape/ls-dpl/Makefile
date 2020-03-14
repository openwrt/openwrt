#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ls-dpl
PKG_VERSION:=lsdk-1903
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://source.codeaurora.org/external/qoriq/qoriq-components/mc-utils
PKG_SOURCE_VERSION:=eeb8972a3d4137c87a54c2795452b17f254a68c7
PKG_MIRROR_HASH:=e96e72793fb9fea1d4f5112d318059ea4c836d65c02852d207407edf13fc92af

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/layerscape-dpl
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NXP DPL firmware
  DEPENDS:=@TARGET_layerscape
endef

MAKE_PATH:=config
MAKE_VARS+= \
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH)

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/config/ls1088a/RDB/dpl-eth.0x1D_0x0D.dtb \
		$(STAGING_DIR_IMAGE)/ls1088ardb-dpl.dtb
	$(CP) $(PKG_BUILD_DIR)/config/ls1088a/RDB/dpc.0x1D-0x0D.dtb \
		$(STAGING_DIR_IMAGE)/ls1088ardb-dpc.dtb
	$(CP) $(PKG_BUILD_DIR)/config/ls2088a/RDB/dpl-eth.0x2A_0x41.dtb \
		$(STAGING_DIR_IMAGE)/ls2088ardb-dpl.dtb
	$(CP) $(PKG_BUILD_DIR)/config/ls2088a/RDB/dpc.0x2A_0x41.dtb \
		$(STAGING_DIR_IMAGE)/ls2088ardb-dpc.dtb
endef

$(eval $(call BuildPackage,layerscape-dpl))
