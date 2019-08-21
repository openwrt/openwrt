# Copyright (C) 2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=vrx518_aca_fw
PKG_VERSION:=1.5.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/vrx518_aca_fw.git
PKG_SOURCE_DATE:=2019-10-27
PKG_SOURCE_VERSION:=1be6b6b0c3aec02a501240887d8800de1357274c
PKG_HASH:=44e528f0e59fb86358cf8f2bf5d07b19f6f839d30f37f143cde8fdde12ab5cfc

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
# ifeq ($(CONFIG_TARGET_lantiq_xrx220),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx200/aca_fw.bin $(1)/lib/firmware/09a9
# endif
# ifeq ($(CONFIG_TARGET_lantiq_xrx330),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx330/aca_fw.bin $(1)/lib/firmware/09a9
# endif
# ifeq ($(CONFIG_TARGET_lantiq_xrx500),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx500/aca_fw.bin $(1)/lib/firmware/09a9
# endif
# ifeq ($(CONFIG_TARGET_intel_mips_xrx500),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx500/aca_fw.bin $(1)/lib/firmware/09a9
# endif
ifeq ($(CONFIG_TARGET_ipq40xx),y)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx500/aca_fw.bin $(1)/lib/firmware/09a9
endif
# ifeq ($(CONFIG_TARGET_x86_puma),y)
# 	$(INSTALL_BIN) $(PKG_BUILD_DIR)/platform/xrx750/aca_fw.bin $(1)/lib/firmware/09a9
# endif
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
