# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=binutils
PKG_VERSION:=$(call qstrip,$(CONFIG_BINUTILS_VERSION))
BIN_VERSION:=$(PKG_VERSION)

PKG_SOURCE_URL:=@GNU/binutils/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

ifeq ($(PKG_VERSION),2.18)
  PKG_MD5SUM:=9d22ee4dafa3a194457caf4706f9cf01
endif
ifeq ($(PKG_VERSION),2.19.1)
  PKG_MD5SUM:=09a8c5821a2dfdbb20665bc0bd680791
endif
ifeq ($(PKG_VERSION),2.19.1+cs)
  BIN_VERSION:=20090205
  PKG_SOURCE:=$(PKG_NAME)-$(BIN_VERSION).tar.bz2
  PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
  PKG_MD5SUM:=040740e8c864dd1a15886753f9c0bc0b
  HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/binutils-$(BIN_VERSION)
endif
ifeq ($(PKG_VERSION),2.20)
  PKG_MD5SUM:=ee2d3e996e9a2d669808713360fa96f8
endif

PATCH_DIR:=./patches/$(PKG_VERSION)

REAL_STAGING_DIR_HOST:=$(STAGING_DIR_HOST)

include $(INCLUDE_DIR)/toolchain-build.mk

BINUTILS_CONFIGURE:= \
	./configure \
		--prefix=$(TOOLCHAIN_DIR)/usr \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--with-sysroot=$(TOOLCHAIN_DIR) \
		--disable-multilib \
		--disable-werror \
		--disable-nls \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(call qstrip,$(CONFIG_EXTRA_BINUTILS_CONFIG_OPTIONS)) \

ifneq ($(CONFIG_SSP_SUPPORT),)
  BINUTILS_CONFIGURE+= \
		--enable-libssp
else
  BINUTILS_CONFIGURE+= \
		--disable-libssp
endif

ifneq ($(CONFIG_EXTRA_TARGET_ARCH),)
  BINUTILS_CONFIGURE+= \
		--enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-$(TARGET_SUFFIX)
endif

define Host/Prepare
	$(call Host/Prepare/Default)
	ln -snf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(HOST_BUILD_DIR)/
endef

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		$(BINUTILS_CONFIGURE) \
	);
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) all
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) install
	$(CP) $(TOOLCHAIN_DIR)/usr/bin/$(REAL_GNU_TARGET_NAME)-readelf $(REAL_STAGING_DIR_HOST)/bin/readelf
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
