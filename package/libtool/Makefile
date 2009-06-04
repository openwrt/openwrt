# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtool
PKG_VERSION:=1.5.24
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libtool
PKG_MD5SUM:=d0071c890101fcf4f2be8934a37841b0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

HOST_PATCH_DIR=/dev/null

PKG_BUILD_DEPENDS:=libtool/host

include $(INCLUDE_DIR)/host-build.mk
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
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	mv $(2)/lib/* $(1)/usr/lib/
	mv $(2)/include/* $(1)/usr/include/
	$(SED) 's,\(hardcode_into_libs\)=yes,\1=no,g' $(2)/bin/libtool
	$(CP) $(2)/bin/libtool $(2)/bin/libtool-ucxx
	$(SED) 's,-lstdc++,-luClibc++,g' $(2)/bin/libtool-ucxx
endef

define Package/libltdl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libltdl/.libs/libltdl.so.* $(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libltdl))
