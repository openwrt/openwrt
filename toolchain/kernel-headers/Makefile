# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME=kernel-headers
PKG_VERSION=$(LINUX_VERSION)

PKG_SOURCE=$(LINUX_SOURCE)
PKG_SOURCE_URL=$(LINUX_SITE)

PKG_BUILD_DIR:=$(TOOLCHAIN_BUILD_DIR)/linux-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Build/Prepare
	mkdir -p $(TOOLCHAIN_BUILD_DIR)
	bzcat $(DL_DIR)/$(PKG_SOURCE) | $(HOST_TAR) -C $(TOOLCHAIN_BUILD_DIR) $(TAR_OPTIONS)
	$(PATCH) $(PKG_BUILD_DIR) $(TOPDIR)/target/linux/generic-$(KERNEL)/patches
	$(SED) 's/@expr length/@-expr length/' $(PKG_BUILD_DIR)/Makefile
	ln -sf $(PKG_BUILD_DIR) $(LINUX_HEADERS_DIR)
endef

define Build/Configure
	yes '' | $(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=$(LINUX_KARCH) \
		CONFIG_SHELL=$(BASH) \
		oldconfig include/linux/version.h
endef

define Build/Compile
endef

define Build/Install
endef

$(eval $(call HostBuild))
