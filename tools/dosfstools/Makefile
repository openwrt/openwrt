#
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=dosfstools
PKG_VERSION:=3.0.27

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/dosfstools/dosfstools/releases/download/v$(PKG_VERSION)/ \
		http://fossies.org/linux/misc
PKG_MD5SUM:=2e31e7bdf92998e41ed17de505a4a552

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

ifeq ($(HOST_OS),Darwin)
LDLIBS += -liconv
endif

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) SHELL="$(BASH)" LDFLAGS="$(HOST_LDFLAGS)" LDLIBS="$(LDLIBS)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/mkfs.fat $(STAGING_DIR_HOST)/bin/
	$(LN) mkfs.fat $(STAGING_DIR_HOST)/bin/mkdosfs
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mk{dosfs,fs.fat}
endef

$(eval $(call HostBuild))
