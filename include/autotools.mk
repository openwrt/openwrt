#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PKG_LIBTOOL_PATHS?=$(CONFIGURE_PATH)
PKG_AUTOMAKE_PATHS?=$(CONFIGURE_PATH)
PKG_REMOVE_FILES?=aclocal.m4

autoconf_bool = $(patsubst %,$(if $($(1)),--enable,--disable)-%,$(2))

# delete *.la-files from staging_dir - we can not yet remove respective lines within all package
# Makefiles, since backfire still uses libtool v1.5.x which (may) require those files
define libtool_remove_files
	find $(1) -name '*.la' | $(XARGS) rm -f;
endef

define autoreconf
	(cd $(PKG_BUILD_DIR); \
		$(patsubst %,rm -f %;,$(PKG_REMOVE_FILES)) \
		$(foreach p,$(PKG_AUTOMAKE_PATHS), \
			if [ -x $(p)/autogen.sh ]; then \
				$(p)/autogen.sh || true; \
			elif [ -f $(p)/configure.ac ] || [ -f $(p)/configure.in ]; then \
				[ -f $(p)/aclocal.m4 ] && [ ! -f $(p)/acinclude.m4 ] && mv aclocal.m4 acinclude.m4; \
				[ -d $(p)/autom4te.cache ] && rm -rf autom4te.cache; \
				$(STAGING_DIR_HOST)/bin/autoreconf -v -f -i -s \
					-B $(STAGING_DIR_HOST)/share/aclocal \
					-B $(STAGING_DIR)/host/share/aclocal \
					$(patsubst %,-I %,$(PKG_LIBTOOL_PATHS)) $(PKG_LIBTOOL_PATHS) || true; \
			fi; \
		) \
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
