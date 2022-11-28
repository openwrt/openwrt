#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lz4
PKG_VERSION:=1.9.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/lz4/lz4/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=0b0e3aa07c8c063ddf40b082bdf7e37a1562bda40a0ff5272957f3e987e0e54b

PKG_LICENSE:=BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE lib/LICENSE

include $(INCLUDE_DIR)/host-build.mk

# Always optimize for speed
HOST_CFLAGS := $(filter-out -O%,$(HOST_CFLAGS)) -O3

HOST_MAKE_FLAGS+=PREFIX=$(HOST_BUILD_PREFIX)

define Host/Configure
endef

define Host/Compile/Lib
	+$(HOST_MAKE_VARS) \
	$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/lib \
		$(HOST_MAKE_FLAGS) \
		$(1)
endef

define Host/Compile
	$(call Host/Compile/Lib,lib-release)
endef

define Host/Install
	$(call Host/Compile/Lib,install)
endef

define Host/Uninstall
	$(call Host/Compile/Lib,uninstall)
	$(call Host/Compile/Lib,clean)
endef

define Host/Clean
endef

$(eval $(call HostBuild))
