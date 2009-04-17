#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnl
PKG_VERSION:=1.1
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=ae970ccd9144e132b68664f98e7ceeb1

include $(INCLUDE_DIR)/package.mk

define Package/libnl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=netlink socket library
  URL:=http://people.suug.ch/~tgr/libnl/
endef

define Package/libnl/description
 This package contains a library for applications dealing with netlink sockets
endef

TARGET_CFLAGS += -ffunction-sections $(FPIC)

define Build/Compile
	$(call Build/Compile/Default)
	make -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

ifneq ($(CONFIG_LINUX_2_6),)
  define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	mkdir -p $(1)/usr/include/libnl
	$(CP) $(PKG_BUILD_DIR)/include/linux $(1)/usr/include/libnl/
  endef

  define Package/libnl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnl.so.* $(1)/usr/lib/
  endef
endif

$(eval $(call BuildPackage,libnl))
