# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME=kernel-headers
PKG_VERSION=2.4.32

PKG_SOURCE=linux-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=38f4d0830e95a20f4bfed17622d5557c
PKG_SOURCE_URL= \
		http://www.de.kernel.org/pub/linux/kernel/v2.4 \
		http://www.fi.kernel.org/pub/linux/kernel/v2.4 \
		http://www.fr.kernel.org/pub/linux/kernel/v2.4 \
		http://www.kernel.org/pub/linux/kernel/v2.4

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/linux-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/mipseb/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	-e 's/armeb/arm/' \
)

define Build/Prepare
	mkdir -p $(TOOL_BUILD_DIR)
	bzcat $(DL_DIR)/$(PKG_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) - \
		linux-$(PKG_VERSION)/include \
		linux-$(PKG_VERSION)/Makefile \
		linux-$(PKG_VERSION)/Rules.make \
		linux-$(PKG_VERSION)/arch/$(LINUX_KARCH)/Makefile \
		linux-$(PKG_VERSION)/scripts \
		linux-$(PKG_VERSION)/arch/$(LINUX_KARCH)/config*.in \
		linux-$(PKG_VERSION)/*/*/Config.in \
		linux-$(PKG_VERSION)/*/Config.in
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	$(SED) 's/@expr length/@-expr length/' $(PKG_BUILD_DIR)/Makefile 
	ln -sf $(PKG_BUILD_DIR) $(LINUX_HEADERS_DIR)
endef

define Build/Configure
	-$(CP) ./files/config.$(LINUX_KARCH) $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) ARCH=$(LINUX_KARCH) oldconfig include/linux/version.h
endef

define Build/Compile
endef

define Build/Install
endef

$(eval $(call HostBuild))
