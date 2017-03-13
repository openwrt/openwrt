#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtool
PKG_VERSION:=2.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libtool
PKG_MD5SUM:=b32b04148ecdd7344abc6fe8bd1bb021

PKG_LICENSE:=GPL-2.0+
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

CONFIGURE_PREFIX=$(STAGING_DIR)/host
export GLOBAL_LIBDIR=$(STAGING_DIR)/usr/lib

define Package/libltdl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A generic dynamic object loading library
  URL:=http://www.gnu.org/software/libtool/
endef

define Package/libtool
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=GNU libtool
  URL:=http://www.gnu.org/software/libtool/
  DEPENDS:=+gcc +file
endef

define Package/libtool/description
GNU libtool is a generic library support script. Libtool hides the complexity
of using shared libraries behind a consistent, portable interface.
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
endef

define Package/libltdl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libltdl/.libs/libltdl.so.* $(1)/usr/lib/
endef

define Package/libtool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libtoolize $(1)/usr/bin
	$(SED) 's|$$(STAGING_DIR_HOST)|/usr|g' \
	  $(1)/usr/bin/libtoolize
	$(SED) 's|$$(CONFIGURE_PREFIX)|/usr|g' \
	  $(1)/usr/bin/libtoolize
	$(SED) 's|/usr/bin/sed|/bin/sed|g' \
	  $(1)/usr/bin/libtoolize
	$(INSTALL_DIR) $(1)/usr/share/aclocal
	for file in argz.m4 ltversion.m4 lt~obsolete.m4 libtool.m4 \
	  ltoptions.m4 ltsugar.m4; do \
		$(INSTALL_DATA) $(PKG_BUILD_DIR)/libltdl/m4/$$$$file \
		  $(1)/usr/share/aclocal/; \
	done
	$(INSTALL_DIR) $(1)/usr/share/libtool/config
	for file in $$$$(cd $(PKG_BUILD_DIR)/libltdl/config && \
	  find . -maxdepth 1 -type f); do \
		$(INSTALL_DATA) $(PKG_BUILD_DIR)/libltdl/config/$$$$file \
		  $(1)/usr/share/libtool/config/; \
		$(SED) 's|$$(STAGING_DIR_HOST)|/usr|g' \
		  $(1)/usr/share/libtool/config/$$$$file; \
	done
endef

$(eval $(call BuildPackage,libltdl))
$(eval $(call BuildPackage,libtool))
