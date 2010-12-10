#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PKG_LIBTOOL_PATHS?=$(CONFIGURE_PATH)

autoconf_bool = $(patsubst %,$(if $($(1)),--enable,--disable)-%,$(2))

# delete *.la-files from staging_dir - we can not yet remove respective lines within all package
# Makefiles, since backfire still uses libtool v1.5.x which (may) require those files
define libtool_remove_files
	find $(1) -name '*.la' | $(XARGS) rm -f;
endef

define autoreconf
	(cd $(PKG_BUILD_DIR); \
		$(patsubst %,rm -f %;,$(PKG_REMOVE_FILES)) \
		if [ -x ./autogen.sh ]; then \
			./autogen.sh || true; \
		elif [ -f ./configure.ac ] || [ -f ./configure.in ]; then \
			[ -f ./aclocal.m4 ] && [ ! -f ./acinclude.m4 ] && mv aclocal.m4 acinclude.m4; \
			[ -d ./autom4te.cache ] && rm -rf autom4te.cache; \
			$(STAGING_DIR_HOST)/bin/autoreconf -v -f -i -s \
				-B $(STAGING_DIR)/host/share/aclocal \
				$(patsubst %,-I %,$(PKG_LIBTOOL_PATHS)) $(PKG_LIBTOOL_PATHS) || true; \
		fi \
	);
endef

Hooks/InstallDev/Post += libtool_remove_files

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
 ifeq ($(filter no-autoreconf,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += autoreconf
 endif
endif
 
ifneq ($(filter libtool-ucxx,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
 ifeq ($(filter no-autoreconf,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += autoreconf
 endif
endif

ifneq ($(filter autoreconf,$(PKG_FIXUP)),)
  ifeq ($(filter autoreconf,$(Hooks/Configure/Pre)),)
    Hooks/Configure/Pre += autoreconf
  endif
endif
