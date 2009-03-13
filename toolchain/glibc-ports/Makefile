#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=glibc-ports
PKG_VERSION:=$(call qstrip,$(CONFIG_GLIBC_VERSION))

ifeq ($(PKG_VERSION),2.3.6)
  PKG_MD5SUM:=40dce745d34ff80401da2fdfe58f6d53
endif
ifeq ($(PKG_VERSION),2.6.1)
  PKG_MD5SUM:=53d88ca624642dd267752ccce77b19d0
endif
ifeq ($(PKG_VERSION),2.7)
  PKG_MD5SUM:=eaeb8527b8fa286c2d887157214f9998
endif

PKG_SOURCE_URL:=@GNU/glibc/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

PATCH_DIR:=./patches/$(PKG_VERSION)

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.glibc-ports_built
HOST_STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.glibc-ports_installed

define Host/Prepare
$(call Host/Prepare/Default)
	ln -snf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))
