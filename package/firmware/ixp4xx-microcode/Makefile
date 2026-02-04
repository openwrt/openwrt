# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2007-2023 OpenWrt.org

include $(TOPDIR)/rules.mk

PKG_NAME:=ixp4xx-microcode
PKG_VERSION:=2.4
PKG_RELEASE:=1

PKG_SOURCE:=IPL_ixp400NpeLibraryWithCrypto-2_4.zip
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_HASH:=1b1170d0657847248589d946048c0aeaa9cd671966fc5bec5933283309485eaa

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

# Ethernet or WAN support, pick one
define Package/ixp4xx-microcode-ethernet
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=IXP4xx ethernet firmware
  DEPENDS:=@TARGET_ixp4xx
endef
define Package/ixp4xx-microcode-wan
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=IXP4xx WAN firmware
  DEPENDS:=@TARGET_ixp4xx
endef

define Package/ixp4xx-microcode-ethernet/description
 This package contains the microcode needed to use the network engines in
 IXP4xx CPUs for ethernet on all three NPEs.
endef
define Package/ixp4xx-microcode-wan/description
 This package contains the microcode needed to use the network engines in
 IXP4xx CPUs for ethernet on NPE-A and NPE-B and WAN (HSS) on NPE-C.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	unzip -d $(PKG_BUILD_DIR)/ $(DL_DIR)/$(PKG_SOURCE)
	mv $(PKG_BUILD_DIR)/ixp400_xscale_sw/src/npeDl/IxNpeMicrocode.c $(PKG_BUILD_DIR)/
	rm -rf $(PKG_BUILD_DIR)/ixp400_xscale_sw
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR); \
		$(HOSTCC) -Wall -I$(STAGING_DIR_HOST)/include IxNpeMicrocode.c -o IxNpeMicrocode; \
		./IxNpeMicrocode -be \
	)
endef

define Package/ixp4xx-microcode-ethernet/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DIR) $(1)/usr/share/doc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-A $(1)/lib/firmware/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-B $(1)/lib/firmware/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-C $(1)/lib/firmware/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/LICENSE.IPL $(1)/usr/share/doc/
endef
define Package/ixp4xx-microcode-wan/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DIR) $(1)/usr/share/doc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-A-HSS $(1)/lib/firmware/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-B $(1)/lib/firmware/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-C $(1)/lib/firmware/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/LICENSE.IPL $(1)/usr/share/doc/
endef


$(eval $(call BuildPackage,ixp4xx-microcode-ethernet))
$(eval $(call BuildPackage,ixp4xx-microcode-wan))
