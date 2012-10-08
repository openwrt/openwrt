#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ncurses
PKG_VERSION:=5.7
PKG_RELEASE:=5

PKG_BUILD_DIR:=$(BUILD_DIR)/$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/ncurses
PKG_MD5SUM:=cce05daf61a64501ef6cd8da1f727ec6

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libncurses
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Terminal handling library
  URL:=http://www.gnu.org/software/ncurses/
  DEPENDS:= +terminfo
  VARIANT:=libncurses
endef

define Package/terminfo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Terminal Info Database (ncurses)
  URL:=http://www.gnu.org/software/ncurses/
  VARIANT:=libncurses
endef

define Package/libncursesw
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Terminal handling library (Unicode)
  URL:=http://www.gnu.org/software/ncurses/
  VARIANT:=libncursesw
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-echo \
	--enable-const \
	--enable-overwrite \
	--disable-rpath \
	--without-ada \
	--without-debug \
	--without-profile \
	--without-progs \
	--disable-big-core \
	--disable-home-terminfo \
	--with-normal \
	--with-shared \
	--with-terminfo-dirs=/usr/share/terminfo \
	--with-default-terminfo-dir=/usr/share/terminfo

ifeq ($(HOST_OS),FreeBSD)
	CONFIGURE_ARGS +=
		--with-terminfo=/usr/share/terminfo.db 
endif

ifeq ($(BUILD_VARIANT),libncursesw)
	CONFIGURE_ARGS += \
		--enable-widec \
		--with-build-cppflags=-D_GNU_SOURCE
endif

MAKE_FLAGS += \
	BUILD_CC="$(HOSTCC)" \
	HOSTCC="$(HOSTCC)" \
	HOSTCCFLAGS="" \
	libs

define Build/Install/Default
	$(MAKE_VARS) \
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_INSTALL_FLAGS) \
		$(1) install.libs install.data;
endef

define Package/terminfo/install
	echo ""
ifneq ($(HOST_OS),FreeBSD)
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
endif
endef

define Package/libncurses/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{curses,ncurses,panel,menu,form}.so* $(1)/usr/lib/
endef

define Package/libncursesw/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{ncurses,panel,menu,form}w.so* $(1)/usr/lib/
endef

ifeq ($(BUILD_VARIANT),libncursesw)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/ncursesw/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/ncursesw/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{ncurses,panel,menu,form}w.{a,so*} $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/bin $(2)/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ncursesw5-config $(2)/bin/
	$(SED) 's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' -e 's/$$$$INCS //g' \
		$(2)/bin/ncursesw5-config
	ln -sf $(STAGING_DIR)/host/bin/ncursesw5-config $(1)/usr/bin/ncursesw5-config
endef
else
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{curses,ncurses,panel,menu,form}.{a,so*} $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/bin $(2)/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ncurses5-config $(2)/bin/
	$(SED) 's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(2)/bin/ncurses5-config
	ln -sf $(STAGING_DIR)/host/bin/ncurses5-config $(1)/usr/bin/ncurses5-config
endef
endif

$(eval $(call BuildPackage,terminfo))
$(eval $(call BuildPackage,libncurses))
$(eval $(call BuildPackage,libncursesw))
