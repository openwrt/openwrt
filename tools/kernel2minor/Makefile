#
# Copyright (C) 2016 adron@yapic.net
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=kernel2minor
PKG_VERSION:=0.22
PKG_RELEASE:=1

PKG_MIRROR_HASH:=8e509deb78e4eddc5f4d0798d3c5fbc99bf6dd151faaf0332a640607042d7f82
PKG_SOURCE_URL:=https://github.com/adron-s/kernel2minor.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=0587740de3e398dbe89d965334c492609cb9739d

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/kernel2minor $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/kernel2minor
endef

$(eval $(call HostBuild))
