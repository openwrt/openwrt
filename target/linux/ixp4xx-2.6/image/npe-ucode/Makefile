# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=npe-ucode
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=IPL_ixp400NpeLibrary-2_3.zip

include $(INCLUDE_DIR)/package.mk

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
		$(HOSTCC) -Wall IxNpeMicrocode.c -o IxNpeMicrocode; \
		./IxNpeMicrocode -be \
	)
endef

define Build/InstallDev
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-B $(TARGET)/
endef

$(eval $(call Build/DefaultTargets))
