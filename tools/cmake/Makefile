#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cmake
PKG_VERSION:=2.8.9

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cmake.org/files/v2.8/
PKG_MD5SUM:=801f4c87f8b604f727df5bf1f05a59e7

HOST_BUILD_PARALLEL:=1
HOST_CONFIGURE_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_VARS :=

HOST_CONFIGURE_ARGS := \
	--prefix=$(STAGING_DIR_HOST) \
	$(if $(MAKE_JOBSERVER),--parallel="$(MAKE_JOBSERVER)")

$(eval $(call HostBuild))
