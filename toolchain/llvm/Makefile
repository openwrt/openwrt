# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=llvm
PKG_VERSION:=r83568
PKG_SOURCE_VERSION:=5687d8a26c08d089c60df2c92d048ab85bcdb2b9
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://repo.or.cz/llvm.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
BUILD_DIR_TOOLCHAIN:=$(BUILD_DIR_HOST)

include $(INCLUDE_DIR)/toolchain-build.mk

ifeq ($(HOST_OS),Darwin)
  DARWIN_VERSION=$(shell sw_vers -productVersion)
  DARWIN_MAJVERS=$(firstword $(subst ., ,$(DARWIN_VERSION)))

  HOST_MAKE_FLAGS += \
	DARWIN_VERSION=$(DARWIN_VERSION) \
	DARWIN_MAJVERS=$(DARWIN_MAJVERS)

endif

HOST_CONFIGURE_ARGS += \
	--enable-optimized --enable-assertions

define Host/Compile
	@-$(MAKE) -C $(HOST_BUILD_DIR) $(HOST_MAKE_FLAGS) uninstall
	$(MAKE) -C $(HOST_BUILD_DIR) $(HOST_MAKE_FLAGS)
endef

define Host/Install
	rm -f $(BUILD_DIR_HOST)/llvm
	ln -s llvm-$(PKG_VERSION) $(BUILD_DIR_HOST)/llvm
	$(MAKE) -C $(HOST_BUILD_DIR) $(HOST_MAKE_FLAGS) install
endef

$(eval $(call HostBuild))
