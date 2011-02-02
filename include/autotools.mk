#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

autoconf_bool = $(patsubst %,$(if $($(1)),--enable,--disable)-%,$(2))

# delete *.la-files from staging_dir - we can not yet remove respective lines within all package
# Makefiles, since backfire still uses libtool v1.5.x which (may) require those files
define libtool_remove_files
	find $(1) -name '*.la' | $(XARGS) rm -f;
endef


AM_TOOL_PATHS:= \
	AUTOM4TE=$(STAGING_DIR_HOST)/bin/autom4te \
	AUTOCONF=$(STAGING_DIR_HOST)/bin/autoconf \
	AUTOMAKE=$(STAGING_DIR_HOST)/bin/automake \
	ACLOCAL=$(STAGING_DIR_HOST)/bin/aclocal \
	AUTOHEADER=$(STAGING_DIR_HOST)/bin/autoheader \
	LIBTOOLIZE=$(STAGING_DIR_HOST)/bin/libtoolize \
	LIBTOOL=$(STAGING_DIR_HOST)/bin/libtool \
	M4=$(STAGING_DIR_HOST)/bin/m4 \
	AUTOPOINT=true

# 1: build dir
# 2: remove files
# 3: automake paths
# 4: libtool paths
# 5: extra m4 dirs
define autoreconf
	(cd $(1); \
		$(patsubst %,rm -f %;,$(2)) \
		$(foreach p,$(3), \
			if [ -f $(p)/configure.ac ] || [ -f $(p)/configure.in ]; then \
				[ -d $(p)/autom4te.cache ] && rm -rf autom4te.cache; \
				touch NEWS AUTHORS COPYING ChangeLog; \
				$(AM_TOOL_PATHS) $(STAGING_DIR_HOST)/bin/autoreconf -v -f -i -s \
					-B $(STAGING_DIR_HOST)/share/aclocal \
					$(patsubst %,-I %,$(5)) \
					$(patsubst %,-I %,$(4)) $(4) || true; \
			fi; \
		) \
	);
endef


PKG_LIBTOOL_PATHS?=$(CONFIGURE_PATH)
PKG_AUTOMAKE_PATHS?=$(CONFIGURE_PATH)
PKG_MACRO_PATHS?=m4
PKG_REMOVE_FILES?=aclocal.m4

Hooks/InstallDev/Post += libtool_remove_files

define autoreconf_target
  $(strip $(call autoreconf, \
    $(PKG_BUILD_DIR), $(PKG_REMOVE_FILES), \
    $(PKG_AUTOMAKE_PATHS), $(PKG_LIBTOOL_PATHS), \
    $(STAGING_DIR)/host/share/aclocal $(STAGING_DIR)/usr/share/aclocal $(PKG_MACRO_PATHS)))
endef

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool libintl libiconv
 ifeq ($(filter no-autoreconf,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += autoreconf_target
 endif
endif

ifneq ($(filter libtool-ucxx,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool libintl libiconv
 ifeq ($(filter no-autoreconf,$(PKG_FIXUP)),)
  Hooks/Configure/Pre += autoreconf_target
 endif
endif

ifneq ($(filter autoreconf,$(PKG_FIXUP)),)
  ifeq ($(filter autoreconf,$(Hooks/Configure/Pre)),)
    Hooks/Configure/Pre += autoreconf_target
  endif
endif


HOST_FIXUP?=$(PKG_FIXUP)
HOST_LIBTOOL_PATHS?=$(if $(PKG_LIBTOOL_PATHS),$(PKG_LIBTOOL_PATHS),.)
HOST_AUTOMAKE_PATHS?=$(if $(PKG_AUTOMAKE_PATHS),$(PKG_AUTOMAKE_PATHS),.)
HOST_MACRO_PATHS?=$(if $(PKG_MACRO_PATHS),$(PKG_MACRO_PATHS),m4)
HOST_REMOVE_FILES?=$(PKG_REMOVE_FILES)

define autoreconf_host
  $(strip $(call autoreconf, \
    $(HOST_BUILD_DIR), $(HOST_REMOVE_FILES), \
    $(HOST_AUTOMAKE_PATHS), $(HOST_LIBTOOL_PATHS), \
    $(HOST_MACRO_PATHS)))
endef

ifneq ($(filter libtool,$(HOST_FIXUP)),)
 ifeq ($(filter no-autoreconf,$(HOST_FIXUP)),)
  Hooks/HostConfigure/Pre += autoreconf_host
 endif
endif

ifneq ($(filter libtool-ucxx,$(HOST_FIXUP)),)
 ifeq ($(filter no-autoreconf,$(HOST_FIXUP)),)
  Hooks/HostConfigure/Pre += autoreconf_host
 endif
endif

ifneq ($(filter autoreconf,$(HOST_FIXUP)),)
  ifeq ($(filter autoreconf,$(Hooks/HostConfigure/Pre)),)
    Hooks/HostConfigure/Pre += autoreconf_host
  endif
endif
