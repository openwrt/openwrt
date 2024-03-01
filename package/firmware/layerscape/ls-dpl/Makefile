#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ls-dpl
PKG_VERSION:=10.38.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/nxp-qoriq/mc-utils
PKG_SOURCE_VERSION:=mc_release_10.38.0
PKG_MIRROR_HASH:=c5032ad73a04cf7fa23a2afeee85108677e665871a6c48ca352f415eea9b6390

PKG_FLAGS:=nonshared

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
	$(CP) $(PKG_BUILD_DIR)/config/ls1088a/LS1088A-RDB/dpl-eth.0x1D_0x0D.dtb \
		$(STAGING_DIR_IMAGE)/fsl_ls1088a-rdb-dpl.dtb
	$(CP) $(PKG_BUILD_DIR)/config/ls1088a/LS1088A-RDB/dpc.0x1D-0x0D.dtb \
		$(STAGING_DIR_IMAGE)/fsl_ls1088a-rdb-dpc.dtb
	$(CP) $(PKG_BUILD_DIR)/config/ls2088a/LS2088A-RDB/dpl-eth.0x2A_0x41.dtb \
		$(STAGING_DIR_IMAGE)/fsl_ls2088a-rdb-dpl.dtb
	$(CP) $(PKG_BUILD_DIR)/config/ls2088a/LS2088A-RDB/dpc.0x2A_0x41.dtb \
		$(STAGING_DIR_IMAGE)/fsl_ls2088a-rdb-dpc.dtb
	$(CP) $(PKG_BUILD_DIR)/config/lx2160a/LX2160A-RDB/dpl-eth.19.dtb \
		$(STAGING_DIR_IMAGE)/fsl_lx2160a-rdb-dpl.dtb
	$(CP) $(PKG_BUILD_DIR)/config/lx2160a/LX2160A-RDB/dpc-usxgmii.dtb \
		$(STAGING_DIR_IMAGE)/fsl_lx2160a-rdb-dpc.dtb
endef

$(eval $(call BuildPackage,layerscape-dpl))
