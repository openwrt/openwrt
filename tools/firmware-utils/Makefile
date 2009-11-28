#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME := firmware-utils

include $(INCLUDE_DIR)/host-build.mk

include $(INCLUDE_DIR)/kernel.mk

define cc
	$(CC) $(HOST_CFLAGS) -include endian.h -o $(HOST_BUILD_DIR)/bin/$(1) src/$(1).c $(2)
endef

define cc2
	$(CC) $(HOST_CFLAGS) -include endian.h -o $(HOST_BUILD_DIR)/bin/$(firstword $(1)) $(foreach src,$(1),src/$(src).c) $(2)
endef

define cc3
	$(CC) $(HOST_CFLAGS) -I$(CURDIR)/../../target/linux/brcm63xx/files/include/asm-mips/mach-bcm63xx -include endian.h -o $(HOST_BUILD_DIR)/bin/$(1) src/$(1).c $(2)
endef


define Host/Compile
	mkdir -p $(HOST_BUILD_DIR)/bin
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
	$(call cc,mkfwimage2,-lz)
	$(call cc3,imagetag)
	$(call cc,add_header)
	$(call cc,makeamitbin)
	$(call cc,encode_crc)
	$(call cc,nand_ecc)
	$(call cc2,mkplanexfw sha1)
	$(call cc2,mktplinkfw md5)
	$(call cc,pc1crypt)
	$(call cc,osbridge-crc)
	$(call cc2,wrt400n cyg_crc32)
	$(call cc,wndr3700)
	$(call cc,mkdniimg)
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/bin/* $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))
