# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libtool
PKG_VERSION:=1.5.24
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libtool
PKG_MD5SUM:=8e0ac9797b62ba4dcc8a2fb7936412b0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

CONFIGURE_PREFIX=$(STAGING_DIR)/host
export GLOBAL_LIBDIR=$(STAGING_DIR)/usr/lib

define Package/libltdl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A generic dynamic object loading library
  URL:=http://www.gnu.org/software/libtool/
endef

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) \
		bindir="$(2)/bin" \
		datadir="$(2)/share" \
		prefix="$(2)" \
		exec_prefix="$(2)" \
		install
	$(INSTALL_DIR) $(1)/lib
	mv $(2)/lib/* $(1)/lib/
#	$(SED) 's,^dlopen_self=unknown,dlopen_self=yes,' \
#	    -e 's,^dlopen_self_static=unknown,dlopen_self_static=no,' \
#	    -e 's,^link_all_deplibs=unknown,link_all_deplibs=no,' \
#		$(2)/bin/libtool
endef

define Package/libltdl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libltdl/.libs/libltdl.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libltdl))
