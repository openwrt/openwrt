# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ncurses
PKG_VERSION:=5.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/ncurses
PKG_MD5SUM:=b6593abe1089d6aab1551c105c9300e3

include $(INCLUDE_DIR)/package.mk

define Package/libncurses
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Terminal handling library
  URL:=http://www.gnu.org/software/ncurses/
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	-$(MAKE) -C $(PKG_BUILD_DIR) distclean
	(cd $(PKG_BUILD_DIR); rm -f config.cache; \
		CONFIG_SITE="" \
		./configure \
			--target=$(GNU_HOST_NAME) \
			--host=$(GNU_HOST_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
			--enable-echo \
			--enable-const \
			--enable-overwrite \
			--disable-rpath \
			--without-ada \
			--without-cxx \
			--without-cxx-binding \
			--without-debug \
			--without-profile \
			--with-progs \
			--with-normal \
			--without-shared \
			--with-terminfo-dirs=/usr/share/terminfo \
			--with-default-terminfo-dir=/usr/share/terminfo \
	);
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		libs install.progs
	(cd $(PKG_BUILD_DIR); rm -f config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
			--enable-echo \
			--enable-const \
			--enable-overwrite \
			--disable-rpath \
			--without-ada \
			--without-cxx \
			--without-cxx-binding \
			--without-debug \
			--without-profile \
			--without-progs \
			--with-normal \
			--with-shared \
			--with-terminfo-dirs=/usr/share/terminfo \
			--with-default-terminfo-dir=/usr/share/terminfo \
	);
	rm -f $(PKG_BUILD_DIR)/lib/lib*
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BUILD_CC="$(TARGET_CC)" \
		HOSTCC="$(HOSTCC)" \
		HOSTCCFLAGS="" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		libs install.libs install.data
endef

define Package/libncurses/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{ncurses,panel,menu,form}*.so.* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/share/terminfo
	(cd $(PKG_INSTALL_DIR)/usr/share/terminfo; \
		for dir in ??; do \
			[ -d "$$$$dir" ] || continue; \
			mv $$$$dir $$$$(echo -ne "\x$$$$dir"); \
		done \
	)
	for file in a/ansi d/dumb l/linux r/rxvt s/screen v/vt100 v/vt102 x/xterm x/xterm-color; do \
		$(INSTALL_DIR) $(1)/usr/share/terminfo/`dirname $$$$file`; \
		$(CP) $(PKG_INSTALL_DIR)/usr/share/terminfo/$$$$file \
			$(1)/usr/share/terminfo/$$$$file; \
	done
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/curses.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ncurses.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ncurses_dll.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/menu.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/panel.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/term{,cap}.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/unctrl.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/form.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/eti.h $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcurses.so $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{ncurses,panel,menu,form}.{a,so*} $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libncurses))
