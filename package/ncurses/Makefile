# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ncurses
PKG_VERSION:=5.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/ncurses
PKG_MD5SUM:=cce05daf61a64501ef6cd8da1f727ec6

include $(INCLUDE_DIR)/package.mk

define Package/libncurses
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Terminal handling library
  URL:=http://www.gnu.org/software/ncurses/
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
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
	--with-default-terminfo-dir=/usr/share/terminfo

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BUILD_CC="$(HOSTCC)" \
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
	for file in a/ansi d/dumb l/linux r/rxvt r/rxvt-unicode s/screen v/vt100 v/vt102 x/xterm x/xterm-color; do \
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
