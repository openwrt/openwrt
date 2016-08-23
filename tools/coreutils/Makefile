#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=coreutils
PKG_VERSION:=8.25

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/coreutils
PKG_MD5SUM:=070e43ba7f618d747414ef56ab248a48

HOST_BUILD_PARALLEL := 1

BUILD_PROGRAMS = date

include $(INCLUDE_DIR)/host-build.mk

BUILD_BINS = $(patsubst %,src/%,$(BUILD_PROGRAMS))

HOST_CONFIGURE_ARGS += \
	 --enable-install-program=date

HOST_MAKE_FLAGS += \
	PROGRAMS="$(BUILD_BINS)" \
	LIBRARIES= MANS= SUBDIRS=.

define Host/Install
	$(INSTALL_DIR) $(1)/bin
	$(CP) $(patsubst %,$(HOST_BUILD_DIR)/%,$(BUILD_BINS)) $(1)/bin/
endef

$(eval $(call HostBuild))
