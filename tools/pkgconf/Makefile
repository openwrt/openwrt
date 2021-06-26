#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=pkgconf
PKG_VERSION:=1.7.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://distfiles.dereferenced.org/pkgconf
PKG_HASH:=d73f32c248a4591139a6b17777c80d4deab6b414ec2b3d21d0a24be348c476ab

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/meson.mk

unexport PKG_CONFIG

HOSTCC := $(HOSTCC_NOCACHE)

MESON_HOST_ARGS += \
	-Dtests=false

define Host/Install
	$(call Host/Install/Meson)
	mv $(STAGING_DIR_HOST)/bin/pkgconf $(STAGING_DIR_HOST)/bin/pkg-config.real
	$(INSTALL_BIN) ./files/pkg-config $(STAGING_DIR_HOST)/bin/pkg-config
endef

define Host/Clean
	+$(NINJA) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
