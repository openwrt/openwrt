#
# Copyright (C) 2016 adron@yapic.net
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=kernel2minor
PKG_VERSION:=0.26
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/adron-s/kernel2minor.git
PKG_MIRROR_HASH:=dd15c137f07e61d6aacbd5146cb45e39259dde02f41ebb0ae5f2594651c767fb
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=062eed9f47e8f47a5aeb659558ad5548697dc8e2
PKG_HASH:=984cbb981b05085a812855080cc5a6aa781f762bb3ca68afe4f81cdccc9de208

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/kernel2minor $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/kernel2minor
endef

$(eval $(call HostBuild))
