# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=vrx518_aca_fw
PKG_VERSION:=1.5.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/vrx518_aca_fw.git
PKG_SOURCE_VERSION:=c509b89c77c26a7df0f0999aabf78b82ca9c9ff0
PKG_MIRROR_HASH:=fba91071f18599617434d93e78c67dad91b3e4c5811b77c15961e3a13b506d2e

PKG_FLAGS:=nonshared
PKG_LICENSE:=MaxLinear-Software-License-Agreement
PKG_LICENSE_FILES:=platform/xrx500/LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=VRX518 ACA firmware
  URL:=http://www.intel.com
  DEPENDS:=@TARGET_ipq40xx
endef

define Package/$(PKG_NAME)/description
  VRX518 ACA firmware
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/lib/firmware/09a9
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/platform/xrx500/LICENSE $(1)/lib/firmware/09a9/aca_fw.bin.LICENSE
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/platform/xrx500/aca_fw.bin $(1)/lib/firmware/09a9
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
