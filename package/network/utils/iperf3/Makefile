#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iperf
PKG_VERSION:=3.0.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.es.net/pub/iperf
PKG_MD5SUM:=a3b2bed7961ba184566df3c3d47f96a6

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
PKG_LICENSE:=BSD-3-Clause

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/iperf3
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Internet Protocol bandwidth measuring tool
  URL:=http://sourceforge.net/projects/iperf/
endef

TARGET_CFLAGS += -D_GNU_SOURCE
CONFIGURE_ARGS += --disable-shared

MAKE_FLAGS += noinst_PROGRAMS=

define Package/iperf3/description
 Iperf is a modern alternative for measuring TCP and UDP bandwidth
 performance, allowing the tuning of various parameters and
 characteristics.
endef

define Package/iperf3/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/iperf3 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,iperf3))
