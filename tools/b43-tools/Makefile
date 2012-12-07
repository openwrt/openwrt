#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=b43-tools
PKG_VERSION:=017

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://git.bues.ch/b43-tools.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_SOURCE_VERSION:=691cd291afdb3dbfcf6b9624f8f4bd068af153d0
#PKG_MIRROR_MD5SUM:=50ca3c763ee21ee213addd17cf1c1b86
HOST_BUILD_DIR=$(BUILD_DIR_HOST)/$(PKG_NAME)

include $(INCLUDE_DIR)/host-build.mk


define Host/Compile
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/fwcutter \
		CFLAGS="$(HOST_CFLAGS) -include endian.h" \
		$(HOST_MAKE_FLAGS) \
		$(1) QUIET_SPARSE=:
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/assembler \
		CFLAGS="$(HOST_CFLAGS) -include endian.h" \
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
