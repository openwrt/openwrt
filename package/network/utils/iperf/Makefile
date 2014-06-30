#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iperf
PKG_VERSION:=2.0.5
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=44b5536b67719f4250faed632a3cd016

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_PARALLEL:=1
PKG_CHECK_FORMAT_SECURITY:=0

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/iperf/Default
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:= $(CXX_DEPENDS)
  TITLE:=Internet Protocol bandwidth measuring tool
  URL:=http://sourceforge.net/projects/iperf/
endef

define Package/iperf/Default/description
 Iperf is a modern alternative for measuring TCP and UDP bandwidth
 performance, allowing the tuning of various parameters and
 characteristics.
endef

define Package/iperf
$(call Package/iperf/Default)
  TITLE+= (with single thread support)
  VARIANT:=single
endef

define Package/iperf/description
$(call Package/iperf/Default/description)
 This package is built with single thread support.
endef

define Package/iperf-mt
$(call Package/iperf/Default)
  DEPENDS+= +libpthread
  TITLE+= (with multithread support)
  VARIANT:=mt
endef

define Package/iperf-mt/description
$(call Package/iperf/Default/description)
 This package is built with multithread support.
endef

CONFIGURE_ARGS += --disable-multicast
CONFIGURE_VARS += ac_cv_func_malloc_0_nonnull=yes

ifeq ($(BUILD_VARIANT),single)
  CONFIGURE_ARGS += --disable-threads
else
  CONFIGURE_ARGS += --enable-threads=posix
  CONFIGURE_VARS += ac_cv_func_pthread_cancel=no
endif

CONFIGURE_VARS += CXXFLAGS="$$$$CXXFLAGS -fno-rtti"

ifeq ($(BUILD_VARIANT),mt)
  CONFIGURE_VARS += LIBS="-lpthread"
endif

define Package/iperf/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/iperf $(1)/usr/bin/iperf
endef
Package/iperf-mt/install = $(Package/iperf/install)

$(eval $(call BuildPackage,iperf))
$(eval $(call BuildPackage,iperf-mt))
