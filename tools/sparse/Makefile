#
# Copyright (C) 2014 Qualcomm-Atheros Inc.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sparse

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/devel/sparse/sparse.git
PKG_SOURCE_DATE:=2015-01-24
PKG_SOURCE_VERSION:=40791b94c56b1a6da2a0ddeb1f9d5c9d64de8f93
PKG_MIRROR_HASH:=ec232e48073488e5f7fc2168792932006cfc65240c266c9d395698d6ff15ba81

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
       $(INSTALL_BIN) $(HOST_BUILD_DIR)/sparse $(STAGING_DIR_HOST)/bin
endef

$(eval $(call HostBuild))
