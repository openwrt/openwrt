# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gdb
PKG_VERSION:=6.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNU/gdb
PKG_MD5SUM:=c9da266b884fb8fa54df786dfaadbc7a

include $(INCLUDE_DIR)/package.mk

define Package/gdb/Default
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:= @!avr32
endef

define Package/gdb
$(call Package/gdb/Default)
  TITLE:=GNU Debugger
  DEPENDS+= +libreadline +libncurses
endef

define Package/gdb/description
 GNU debugger
endef

define Package/gdbserver
$(call Package/gdb/Default)
  TITLE:=Remote server for GNU Debugger
endef

define Package/gdbserver/description
 GDBSERVER is a program that allows you to run GDB on a different machine 
 than the one which is running the program being debugged.
endef

CONFIGURE_ARGS+= \
	--with-system-readline

CONFIGURE_VARS+= \
	ac_cv_search_tgetent="$(TARGET_LDFLAGS) -lncurses -lreadline"

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
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/gdbserver $(1)/usr/bin/
endef

$(eval $(call BuildPackage,gdb))
$(eval $(call BuildPackage,gdbserver))

