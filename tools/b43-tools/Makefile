#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=b43-tools
PKG_DATE:=2025-09-23

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/mbuesch/b43-tools.git
PKG_MIRROR_HASH:=421520ea1983c4774864ec47f9333e5dfa947ef759ce543830b958c63a9c5974
PKG_SOURCE_VERSION:=35fe4385a8d738874ef004fa47c5aa40b2c08b3e

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/fwcutter \
		CFLAGS="$(HOST_CFLAGS) -include endian.h" \
		$(HOST_MAKE_FLAGS) \
		$(1) QUIET_SPARSE=:
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/assembler \
		CFLAGS="$(HOST_CFLAGS) -include endian.h -Wno-error=int-conversion" \
		$(HOST_MAKE_FLAGS) \
		LDFLAGS= \
		$(1) QUIET_SPARSE=:
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/fwcutter/b43-fwcutter $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/assembler/b43-asm $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/assembler/b43-asm.bin $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) ./files/b43-fwsquash.py $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/b43-fwcutter
	rm -f $(STAGING_DIR_HOST)/bin/b43-asm
	rm -f $(STAGING_DIR_HOST)/bin/b43-asm.bin
	rm -f $(STAGING_DIR_HOST)/bin/b43-fwsquash.py
endef

$(eval $(call HostBuild))
