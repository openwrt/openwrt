#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fmc
PKG_SOURCE_DATE:=2017-09-22
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/qoriq-open-source/fmc.git
PKG_SOURCE_VERSION:=8c9f127138dc803bf58bef75e9fc092dc60f9c53
PKG_MIRROR_HASH:=199b91c0a49ab4a4aa1eded7703ea1c11241e22d21a41f00fe992c043b28b467

include $(INCLUDE_DIR)/package.mk

define Package/fmc
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@TARGET_layerscape +libxml2 +tclap +fmlib +libstdcpp
  TITLE:=Layerscape Frame Manager Configuration Tool
endef

define Package/fmc/description
  The Frame Manager Configuration tool is a software package whose
  primary purpose is converting Parse-Classify-Police-Distribute
  (PCD) descriptions of network packets flow into hardware
  configuration.
endef

MAKE_FLAGS += \
	-C source \
	FMD_USPACE_HEADER_PATH="$(STAGING_DIR)/include/fmd" \
	FMD_USPACE_LIB_PATH="$(STAGING_DIR)/lib" \
	LIBXML2_HEADER_PATH="$(STAGING_DIR)/usr/include/libxml2" \
	TCLAP_HEADER_PATH="$(STAGING_DIR)/usr/include"

define Package/fmc/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/source/fmc $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/
	$(CP) $(PKG_BUILD_DIR)/etc/* $(1)/etc/
endef

$(eval $(call BuildPackage,fmc))
