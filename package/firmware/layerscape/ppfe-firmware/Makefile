#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ppfe
PKG_VERSION:=2017.09
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/qoriq-open-source/engine-pfe-bin.git
PKG_SOURCE_VERSION:=97cd13d3070d7199e62881bc495b332194b67edd
PKG_MIRROR_HASH:=1ef60cdac49dd57c76ec9ef01d00f6eeb025ba62f0d75f54adeeb3362f5edad2

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

define Package/layerscape-ppfe
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@TARGET_layerscape
  TITLE:=NXP Layerscape PPFE firmware
  CONFIG_FW:=ls1012a/u-boot/pfe_fw_sbl.itb
  CONFIG_BIN1:=ls1012a/slow_path/ppfe_class_ls1012a.elf
  CONFIG_BIN2:=ls1012a/slow_path/ppfe_tmu_ls1012a.elf
  CONFIG_LIC:=Freescale-Binary-EULA
endef

define Build/Compile
endef

define Package/layerscape-ppfe/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(CONFIG_FW) \
		$(STAGING_DIR_IMAGE)/pfe.itb
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(CONFIG_BIN1) \
		$(PKG_BUILD_DIR)/$(CONFIG_BIN2) \
		$(PKG_BUILD_DIR)/$(CONFIG_LIC) \
		$(1)/lib/firmware/
endef

$(eval $(call BuildPackage,layerscape-ppfe))
