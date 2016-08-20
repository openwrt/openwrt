#
# Copyright (C) 2011-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zyimage
PKG_VERSION:=1

include $(INCLUDE_DIR)/host-build.mk

define Host/Prepare
	mkdir -p $(HOST_BUILD_DIR)
	$(CP) ./src/* $(HOST_BUILD_DIR)/
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) \
			CC=$(HOSTCC) \
			CFLAGS="$(HOST_CFLAGS)" \
			CPPFLAGS="$(HOST_CPPFLAGS)" \
			LDFLAGS="$(HOST_LDFLAGS)"
endef

define Host/Configure
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/zyimage $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/zyimage
endef

$(eval $(call HostBuild))
