# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright 2022-2025 NXP
#

include $(TOPDIR)/rules.mk

PKG_NAME:=firmware-imx
PKG_VERSION:=8.28-994fa14
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).bin
PKG_SOURCE_URL:=https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/
PKG_HASH:=55996f340e87825685a00cd309901189066ec9545ee607734f942c3cde4d69dc

PKG_LICENSE:=LA_OPT_NXP_Software_License
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/firmware-imx
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NXP firmware for i.MX SoC
  DEPENDS:=@TARGET_imx_cortexa53
endef

UNPACK_CMD:=
define Build/Compile
	$(PKG_UNPACK)
	$(CP) $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)
	chmod a+x $(PKG_BUILD_DIR)/$(PKG_SOURCE)
	cd $(PKG_BUILD_DIR) && ./$(PKG_SOURCE) --auto-accept
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/firmware-imx
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)/firmware/ddr/synopsys/* \
		$(STAGING_DIR_IMAGE)/firmware-imx/
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)/firmware/hdmi/cadence/* \
		$(STAGING_DIR_IMAGE)/firmware-imx/
endef

$(eval $(call BuildPackage,firmware-imx))
