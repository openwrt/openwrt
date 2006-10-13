# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=binutils
PKG_VERSION:=$(strip $(subst ",, $(CONFIG_BINUTILS_VERSION)))#"))

PKG_SOURCE_URL:=http://ftp.gnu.org/gnu/binutils/ \
	ftp://gatekeeper.dec.com/pub/GNU/ \
        ftp://ftp.uu.net/archive/systems/gnu/ \
        ftp://ftp.eu.uu.net/pub/gnu/ \
        ftp://ftp.funet.fi/pub/gnu/prep/ \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/ 

PKG_SOURCE:=binutils-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=unknown
PKG_BUILD_DIR:=$(TOOLCHAIN_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/host-build.mk


define Build/Prepare
	$(call Build/Prepare/Default)
	if [ -d ./patches/$(PKG_VERSION) ]; then \
		$(SCRIPT_DIR)/patch-kernel.sh $(PKG_BUILD_DIR) ./patches/$(PKG_VERSION); \
	fi
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		./configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--disable-werror \
		--disable-nls \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) -j $(CONFIG_JLEVEL) all
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) -j $(CONFIG_JLEVEL) install
endef

$(eval $(call HostBuild))
