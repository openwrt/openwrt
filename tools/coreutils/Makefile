#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=coreutils
PKG_CPE_ID:=cpe:/a:gnu:coreutils
PKG_VERSION:=9.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/coreutils
PKG_HASH:=6885ff47b9cdb211de47d368c17853f406daaf98b148aaecdf10de29cc04b0b3

HOST_BUILD_PARALLEL := 1

BUILD_PROGRAMS = date readlink touch ln chown ginstall

include $(INCLUDE_DIR)/host-build.mk

BUILD_BINS = $(patsubst %,src/%,$(BUILD_PROGRAMS))

HOST_CONFIGURE_ARGS += \
	 --enable-install-program=$(subst $(space),$(comma),$(strip $(BUILD_PROGRAMS)))

HOST_MAKE_FLAGS += \
	PROGRAMS="$(BUILD_BINS)" \
	LIBRARIES= MANS= SUBDIRS=.

define Host/Install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(patsubst %,$(HOST_BUILD_DIR)/%,$(BUILD_BINS)) $(1)/bin/
	ln -sf ginstall $(1)/bin/install
endef

define Host/Uninstall
	rm -f $(STAGING_DIR_HOST)/bin/install
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
