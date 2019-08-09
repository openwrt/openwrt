#
# Copyright (C) 2019 CZ.NIC z.s.p.o. (https://www.nic.cz/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ninja
PKG_VERSION:=1.10.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/ninja-build/ninja/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=3810318b08489435f8efc19c05525e80a993af5a55baa0dfeae0465a9d45f99f

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=COPYING

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
	cd $(HOST_BUILD_DIR) && \
		CXX="$(HOSTCXX)" \
		CXXFLAGS="$(HOST_CXXFLAGS) $(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		$(STAGING_DIR_HOST)/bin/$(PYTHON) configure.py --bootstrap
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/usr/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/ninja $(STAGING_DIR_HOST)/usr/bin/
endef

define Host/Clean
	$(call Host/Clean/Default)
	rm -f $(STAGING_DIR_HOST)/usr/bin/ninja
endef

$(eval $(call HostBuild))
