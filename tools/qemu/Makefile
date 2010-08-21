#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=qemu
PKG_VERSION:=0.12.5
PKG_RELEASE:=2

PKG_SOURCE_URL:=http://download.savannah.gnu.org/releases/qemu/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=1d02ee0a04dfae2894340273372c1de4

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		CFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		$(HOST_CONFIGURE_CMD) \
		--extra-cflags="$(HOST_CFLAGS)" \
		--enable-uuid \
	)
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) qemu-img
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/qemu-img $(STAGING_DIR_HOST)/bin
endef

$(eval $(call HostBuild))
