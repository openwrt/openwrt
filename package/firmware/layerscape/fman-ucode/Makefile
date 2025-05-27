#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fman-ucode
PKG_VERSION:=6.6.3.1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/nxp-qoriq/qoriq-fm-ucode
PKG_SOURCE_VERSION:=lf-6.6.3-1.0.0
PKG_MIRROR_HASH:=6bb9dd8ae0ac7b2ba0e5bc5e0590732167844a1b2c9316fbdcdd04e600785b0c

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/layerscape-fman
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NXP FMan ucode
  DEPENDS:=@TARGET_layerscape
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/fsl_fman_ucode_ls1043_r1.1_106_4_18.bin \
		$(STAGING_DIR_IMAGE)/fsl_ls1043a-rdb-fman.bin
	$(CP) $(PKG_BUILD_DIR)/fsl_fman_ucode_ls1046_r1.0_106_4_18.bin \
		$(STAGING_DIR_IMAGE)/fsl_ls1046a-rdb-fman.bin
endef

$(eval $(call BuildPackage,layerscape-fman))
