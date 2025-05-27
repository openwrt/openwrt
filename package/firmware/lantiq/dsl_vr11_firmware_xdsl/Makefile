# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=dsl_vr11_firmware_xdsl
PKG_VERSION:=8.13.1.5.0.7
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/dsl_vr11_firmware_xdsl.git
PKG_SOURCE_VERSION:=99cf1fe7a1711b9aa128eeb8419eab698448df9f
PKG_MIRROR_HASH:=7fb37723f8db2558d774ba972f011598d2399609158c5dbc287eca0873b040f1

PKG_FLAGS:=nonshared
PKG_LICENSE:=MaxLinear-Software-License-Agreement
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

ANNEX_A_VER:=8D1507_8D0901

define Package/$(PKG_NAME)
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=VRX518 / VR11 CPE xDSL Annex A firmware
  URL:=http://www.intel.com
  DEPENDS:=@TARGET_ipq40xx
endef

define Package/$(PKG_NAME)/description
  VRX518 / VR11 CPE VDSL and ADSL Annex A firmware
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/LICENSE $(1)/lib/firmware/xcpe_$(ANNEX_A_VER).bin.LICENSE
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/xcpe_$(ANNEX_A_VER).bin $(1)/lib/firmware/
	ln -s xcpe_$(ANNEX_A_VER).bin $(1)/lib/firmware/vdsl.bin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
