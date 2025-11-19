#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fman-ucode
PKG_VERSION:=6.12.20.2.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/nxp-qoriq/qoriq-fm-ucode
PKG_SOURCE_VERSION:=lf-6.12.20-2.0.0
PKG_MIRROR_HASH:=15b5df228b23b4cbdd1af484c98069b558d32ece15a80f0dabe22af6e5ee1ba1

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/layerscape-fman
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NXP FMan ucode
  DEPENDS:=@TARGET_layerscape||TARGET_qoriq
  DEFAULT:=y if TARGET_layerscape_armv8_64b||TARGET_qoriq
  HIDDEN:=1
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/fsl_fman_ucode_ls1043_r1.1_106_4_18.bin \
		$(STAGING_DIR_IMAGE)/fsl_ls1043a-rdb-fman.bin
	$(CP) $(PKG_BUILD_DIR)/fsl_fman_ucode_ls1046_r1.0_106_4_18.bin \
		$(STAGING_DIR_IMAGE)/fsl_ls1046a-rdb-fman.bin
	$(CP) $(PKG_BUILD_DIR)/fsl_fman_ucode_t4240_r2.0_106_4_18.bin \
		$(STAGING_DIR_IMAGE)/fsl_t4240-rdb-fman.bin
endef

$(eval $(call BuildPackage,layerscape-fman))
