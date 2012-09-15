#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gdb
PKG_VERSION:=linaro-7.2-2011.03-0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=747b655418058964451eff7c7a15013c
PKG_SOURCE_URL:=http://launchpad.net/gdb-linaro/7.2/7.2-2011.03-0/+download/

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/toolchain-build.mk

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		gdb_cv_func_sigsetjmp=yes \
		CFLAGS="-O2" \
		$(HOST_BUILD_DIR)/configure \
		--prefix=$(TOOLCHAIN_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--disable-werror \
		$(DISABLE_NLS) \
		--without-uiout \
		--disable-tui --disable-gdbtk --without-x \
		--without-included-gettext \
		--enable-threads \
	);
endef

define Host/Install
	mkdir -p $(TOOLCHAIN_DIR)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/gdb/gdb $(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)gdb
	ln -fs $(TARGET_CROSS)gdb $(TOOLCHAIN_DIR)/bin/$(GNU_TARGET_NAME)-gdb
	strip $(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)gdb
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)gdb \
		$(TOOLCHAIN_DIR)/bin/$(GNU_TARGET_NAME)-gdb
endef

$(eval $(call HostBuild))
