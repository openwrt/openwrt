# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=elfutils
PKG_VERSION:=0.189
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://sourceware.org/$(PKG_NAME)/ftp/$(PKG_VERSION)
PKG_HASH:=39bd8f1a338e2b7cd4abc3ff11a0eddc6e690f69578a57478d8179b4148708c8

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=COPYING COPYING-GPLV2 COPYING-LGPLV3
PKG_CPE_ID:=cpe:/a:elfutils_project:elfutils

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--disable-debuginfod \
	--disable-libdebuginfod \
	--disable-nls \
	--disable-shared \
	--without-lzma \
	--without-zstd

HOST_MAKE_FLAGS += \
	SUBDIRS="lib libelf libcpu backends libebl libdwelf libdwfl libdw"

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
