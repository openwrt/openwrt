# 
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=readline
PKG_VERSION:=6.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/readline
PKG_MD5SUM:=33c8fb279e981274f485fd91da77e94a

PKG_LICENSE:=GPL-3.0+
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libreadline
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Command lines edition library
  URL:=http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html
endef

define Package/libreadline/description
	The Readline library provides a set of functions for use by applications 
	that allow users to edit command lines as they are typed in. Both Emacs 
	and vi editing modes are available. The Readline library includes 
	additional functions to maintain a list of previously-entered command 
	lines, to recall and perhaps reedit those lines, and perform csh-like 
	history expansion on previous commands.
endef

define Build/Configure
	$(CP) $(SCRIPT_DIR)/config.guess $(SCRIPT_DIR)/config.sub $(PKG_BUILD_DIR)/support/
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		--without-curses \
		bash_cv_wcwidth_broken=no \
		bash_cv_func_sigsetjmp=yes \
	)
endef

TARGET_CPPFLAGS:=-I. -I.. $(TARGET_CPPFLAGS)

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/readline $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{history,readline}.{a,so,so.6,so.6.3} $(1)/usr/lib/
endef

define Package/libreadline/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{history,readline}.{so,so.6,so.6.3} $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libreadline))
