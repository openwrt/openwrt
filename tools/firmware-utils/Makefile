#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME := firmware-utils

include $(INCLUDE_DIR)/host-build.mk

define cc
	$(CC) $(HOST_CFLAGS) -include endian.h -o $(PKG_BUILD_DIR)/bin/$(1) src/$(1).c $(2)
endef

define Build/Compile
	mkdir -p $(PKG_BUILD_DIR)/bin
	$(call cc,addpattern)
	$(call cc,trx)
	$(call cc,motorola-bin)
	$(call cc,dgfirmware)
	$(call cc,trx2usr)
	$(call cc,ptgen)
	$(call cc,airlink)
	$(call cc,srec2bin)
	$(call cc,mkmylofw)
	$(call cc,mkcsysimg)
	$(call cc,mkzynfw)
	$(call cc,lzma2eva,-lz)
	$(call cc,mkcasfw)
	$(call cc,mkfwimage,-lz)
	$(call cc,imagetag)
	$(call cc,add_header)
endef

define Build/Install
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/* $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))
