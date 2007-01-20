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
PKG_UNPACK:=unzip -d $(PKG_BUILD_DIR)/ $(DL_DIR)/$(PKG_SOURCE)

include $(INCLUDE_DIR)/unpack.mk

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(PKG_UNPACK)
	mv $(PKG_BUILD_DIR)/ixp400_xscale_sw/src/npeDl/IxNpeMicrocode.c $(PKG_BUILD_DIR)/
	rm -rf $(PKG_BUILD_DIR)/ixp400_xscale_sw
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/NPE-B: $(PKG_BUILD_DIR)/.prepared
	( cd $(PKG_BUILD_DIR) ; \
	  $(HOSTCC) -Wall IxNpeMicrocode.c -o IxNpeMicrocode ; \
	  ./IxNpeMicrocode -be )

download: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(PKG_BUILD_DIR)/NPE-B

ifneq ($(TARGET),)
install: compile
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPE-B $(TARGET)/
endif

clean:
	rm -rf $(PKG_BUILD_DIR)
