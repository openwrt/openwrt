# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=lua
PKG_VERSION:=5.1.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.lua.org/ftp/ \
	http://ftp.gwdg.de/pub/languages/lua/ \
	http://mirrors.dotsrc.org/lua/ \
	http://www.tecgraf.puc-rio.br/lua/ftp/
PKG_MD5SUM:=a70a8dfaa150e047866dc01a46272599

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/lua/Default
  SUBMENU:=LUA
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LUA programming language
  URL:=http://www.lua.org/
endef

define Package/lua/Default/description
 Lua is a powerful light-weight programming language designed for extending 
 applications. Lua is also frequently used as a general-purpose, stand-alone 
 language. Lua is free software.
endef

define Package/liblua
$(call Package/lua/Default)
  SUBMENU:=
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (libraries)
endef

define Package/liblua/description
$(call Package/lua/Default/description)
 This package contains the LUA shared libraries, needed by other programs.
endef

define Package/lua
$(call Package/lua/Default)
  DEPENDS:=+liblua +libreadline +libncurses
  TITLE+= (interpreter)
endef

define Package/lua/description
$(call Package/lua/Default/description)
 This package contains the LUA language interpreter.
endef

define Package/luac
$(call Package/lua/Default)
  DEPENDS:=+liblua
  TITLE+= (compiler)
endef

define Package/luac/description
$(call Package/lua/Default/description)
 This package contains the LUA language compiler.
endef

define Package/lua-examples
$(call Package/lua/Default)
  DEPENDS:=lua
  TITLE+= (examples)
endef

define Package/lua-examples/description
$(call Package/lua/Default/description)
 This package contains LUA language examples.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CROSS)gcc" \
		LD="$(TARGET_CROSS)ld" \
		AR="$(TARGET_CROSS)ar rcu" \
		RANLIB="$(TARGET_CROSS)ranlib" \
		INSTALL_ROOT=/usr \
		MYCFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
		MYLDFLAGS="$(TARGET_LDFLAGS)" \
		PKG_VERSION=$(PKG_VERSION) \
		all linux 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INSTALL_TOP="$(PKG_INSTALL_DIR)/usr" \
		install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lua{,lib,conf}.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lauxlib.h $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblua.{a,so.*} $(1)/usr/lib/
	ln -sf liblua.so.$(PKG_VERSION) $(1)/usr/lib/liblua.so
	ln -sf liblua.so.$(PKG_VERSION) $(1)/usr/lib/liblualib.so
endef

define Package/liblua/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblua.so.* $(1)/usr/lib/
endef

define Package/lua/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lua $(1)/usr/bin/
endef

define Package/luac/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/luac $(1)/usr/bin/
endef

define Package/lua-examples/install
	$(INSTALL_DIR) $(1)/usr/share/lua/examples
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/test/*.lua \
		$(1)/usr/share/lua/examples/
endef

$(eval $(call BuildPackage,liblua))
$(eval $(call BuildPackage,lua))
$(eval $(call BuildPackage,luac))
$(eval $(call BuildPackage,lua-examples))
