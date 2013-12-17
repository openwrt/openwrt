# 
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=busybox
PKG_VERSION:=1.19.4
PKG_RELEASE:=7
PKG_FLAGS:=essential

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.busybox.net/downloads \
		http://distfiles.gentoo.org/distfiles/
PKG_MD5SUM:=9c0cae5a0379228e7b55e5b29528df8e

PKG_CONFIG_DEPENDS:=CONFIG_BUSYBOX_CONFIG_FEATURE_MOUNT_NFS
PKG_BUILD_PARALLEL:=1

PKG_LICENSE:=GPLv2 BSD-4c
PKG_LICENSE_FILES:=LICENSE archival/libarchive/bz/LICENSE

include $(INCLUDE_DIR)/package.mk

ifneq ($(findstring c,$(OPENWRT_VERBOSE)),)
  BB_MAKE_VERBOSE := V=1
else
  BB_MAKE_VERBOSE :=
endif

define Package/busybox
  SECTION:=base
  CATEGORY:=Base system
  MAINTAINER:=Nicolas Thill <nico@openwrt.org>
  TITLE:=Core utilities for embedded Linux
  URL:=http://busybox.net/
  DEPENDS:=+BUSYBOX_ENABLE_NFS_MOUNT:librpc
  MENU:=1
endef

define Package/busybox/description
 The Swiss Army Knife of embedded Linux.
 It slices, it dices, it makes Julian Fries.
endef

define Package/busybox/config
	source "$(SOURCE)/Config.in"
endef

CONFIG_TEMPLATE:=./config/default

LDLIBS:=m crypt
ifdef CONFIG_BUSYBOX_CONFIG_FEATURE_MOUNT_NFS
  TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
  export LDFLAGS=$(TARGET_LDFLAGS)
  LDLIBS += rpc
endif

CONFIG_TEMPLATE:=+ $(CONFIG_TEMPLATE) $(PKG_BUILD_DIR)/.config.build

ENV_CONFIG:=$(wildcard $(TOPDIR)/env/busybox-config)
ifneq ($(ENV_CONFIG),)
  CONFIG_TEMPLATE:=+ $(CONFIG_TEMPLATE) $(ENV_CONFIG)
  STAMP_CONFIGURED:=$(STAMP_CONFIGURED)_$(shell $(SH_FUNC) md5s < $(ENV_CONFIG))
endif

define Build/Configure
	grep -E '^(# )?CONFIG_BUSYBOX_CONFIG_' $(TOPDIR)/.config | \
		sed -e 's,CONFIG_BUSYBOX_CONFIG_,CONFIG_,' > $(PKG_BUILD_DIR)/.config.build
	$(SCRIPT_DIR)/kconfig.pl $(CONFIG_TEMPLATE) > $(PKG_BUILD_DIR)/.config
	yes 'n' | $(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KBUILD_HAVE_NLS=no \
		ARCH="$(ARCH)" \
		$(BB_MAKE_VERBOSE) \
		oldconfig
endef

ifdef CONFIG_GCC_VERSION_LLVM
  TARGET_CFLAGS += -fnested-functions
endif

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		SKIP_STRIP=y \
		LDLIBS="$(LDLIBS)" \
		$(BB_MAKE_VERBOSE) \
		all
	rm -rf $(PKG_INSTALL_DIR)
	$(FIND) $(PKG_BUILD_DIR) -lname "*busybox" -exec rm \{\} \;
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(PKG_INSTALL_DIR)" \
		LDLIBS="$(LDLIBS)" \
		$(BB_MAKE_VERBOSE) \
		install
endef

define Package/busybox/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	$(INSTALL_BIN) ./files/cron $(1)/etc/init.d/cron
	$(INSTALL_BIN) ./files/telnet $(1)/etc/init.d/telnet
	$(INSTALL_BIN) ./files/sysntpd $(1)/etc/init.d/sysntpd
	-rm -rf $(1)/lib64
endef

$(eval $(call BuildPackage,busybox))
