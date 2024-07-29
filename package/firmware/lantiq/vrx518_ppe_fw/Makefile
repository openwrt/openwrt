# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=vrx518_ppe_fw
PKG_VERSION:=1.3.7
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/vrx518_ppe_fw.git
PKG_SOURCE_VERSION:=47c48d52ba59df733ab21fd0c18f6d1a7b0e7229
PKG_MIRROR_HASH:=33dd15b6c6205b5031498aac9a5a4876f8217aefea06dc511ac60ca1343b50d1

PKG_FLAGS:=nonshared
PKG_LICENSE:=MaxLinear-Software-License-Agreement
PKG_LICENSE_FILES:=platform/xrx500/LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=VRX518 PPE firmware
  URL:=http://www.intel.com
  DEPENDS:=@TARGET_ipq40xx
endef

define Package/$(PKG_NAME)/description
  VRX518 PPE firmware
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/platform/xrx500/LICENSE $(1)/lib/firmware/ppe_fw.bin.LICENSE
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/platform/xrx500/ppe_fw.bin $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
