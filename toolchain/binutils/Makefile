# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=binutils
PKG_VERSION:=$(call qstrip,$(CONFIG_BINUTILS_VERSION))

PKG_SOURCE_URL:=@GNU/binutils/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

ifeq ($(PKG_VERSION),2.17)
  PKG_MD5SUM:=e26e2e06b6e4bf3acf1dc8688a94c0d1
endif
ifeq ($(PKG_VERSION),2.18)
  PKG_MD5SUM:=9d22ee4dafa3a194457caf4706f9cf01
endif

PATCH_DIR:=./patches/$(PKG_VERSION)

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)
override CONFIG_AUTOREBUILD=

include $(INCLUDE_DIR)/host-build.mk

EXTRA_TARGET=$(if $(CONFIG_EXTRA_TARGET_ARCH),--enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-uclibc)

define Build/Configure
	$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/
	(cd $(PKG_BUILD_DIR); \
		./configure \
		--prefix=$(STAGING_DIR_HOST) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--disable-werror \
		--disable-nls \
		$(EXTRA_TARGET) \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(call qstrip,$(CONFIG_EXTRA_BINUTILS_CONFIG_OPTIONS)) \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) install
endef

$(eval $(call HostBuild))
