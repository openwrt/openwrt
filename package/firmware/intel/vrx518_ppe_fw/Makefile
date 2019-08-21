# Copyright (C) 2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=vrx518_ppe_fw
PKG_VERSION:=1.3.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/vrx518_ppe_fw.git
PKG_SOURCE_DATE:=2019-10-27
PKG_SOURCE_VERSION:=0537848df78f7bb091fb5dc0f32bc7a16aaace98
PKG_HASH:=da6e8df6648f7e0cc159aa4408382eff666adba3ea95a3c8af054e21a969da4f

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

# ifeq ($(CONFIG_TARGET_lantiq_xrx330),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx330/ppe_fw.bin $(1)/lib/firmware/
# endif
# ifeq ($(CONFIG_TARGET_lantiq_xrx500),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx500/ppe_fw.bin $(1)/lib/firmware/
# endif
# ifeq ($(CONFIG_TARGET_intel_mips_xrx500),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx500/ppe_fw.bin $(1)/lib/firmware/
# endif
ifeq ($(CONFIG_TARGET_ipq40xx),y)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx500/ppe_fw.bin $(1)/lib/firmware/
endif
# ifeq ($(CONFIG_TARGET_x86_puma),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx750/ppe_fw.bin $(1)/lib/firmware/
# endif
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
