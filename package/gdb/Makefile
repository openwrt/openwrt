# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gdb
PKG_VERSION:=6.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=c9da266b884fb8fa54df786dfaadbc7a
PKG_SOURCE_URL:=http://ftp.gnu.org/gnu/gdb
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

define Package/gdb
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libreadline +libncurses
  TITLE:=GNU Debugger
endef

define Package/gdbserver
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Remote server for GNU Debugger
endef

define Package/gdb/description
 GNU debugger
endef

define Package/gdbserver/description
 GDBSERVER is a program that allows you to run GDB on a different machine 
 than the one which is running the program being debugged.
endef

TARGET_CPPFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib
CONFIGURE_ARGS += --with-system-readline

export ac_cv_search_tgetent=$(TARGET_LDFLAGS) -lncurses -lreadline

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		all install-gdb
endef

define Package/gdb/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/gdb $(1)/usr/bin/
endef

define Package/gdbserver/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/gdb $(1)/usr/bin/
endef

$(eval $(call BuildPackage,gdb))
$(eval $(call BuildPackage,gdbserver))

