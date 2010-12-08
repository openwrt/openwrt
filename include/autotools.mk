#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PKG_LIBTOOL_PATHS?=$(CONFIGURE_PATH)

autoconf_bool = $(patsubst %,$(if $($(1)),--enable,--disable)-%,$(2))

# prevent libtool from linking against host development libraries
define libtool_fixup_libdir
	find $(1) -name '*.la' | $(XARGS) \
		$(SED) "s,\(^libdir='\| \|-L\|^dependency_libs='\)/usr/lib,\1$(STAGING_DIR)/usr/lib,g" \
		    -e "s,$(STAGING_DIR)/usr/lib/\(libstdc++\|libsupc++\).la,$(TOOLCHAIN_DIR)/lib/\1.la,g";
endef

# delete *.la-files from staging_dir - we can not yet remove respective lines within all package
# Makefiles, since backfire still uses libtool v1.5.x which (may) require those files
define libtool_remove_files
	find $(1) -name '*.la' | $(XARGS) rm -f;
endef

define autoreconf
	(cd $(PKG_BUILD_DIR); $(STAGING_DIR_HOST)/bin/autoreconf -v -f -i -s $(patsubst %,-I %,$(PKG_LIBTOOL_PATHS)) $(PKG_LIBTOOL_PATHS));
endef

Hooks/InstallDev/Post += libtool_remove_files

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
  Hooks/Configure/Pre += autoreconf
  Hooks/InstallDev/Post += libtool_fixup_libdir
endif
 
ifneq ($(filter libtool-ucxx,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
  Hooks/Configure/Pre += autoreconf
  Hooks/InstallDev/Post += libtool_fixup_libdir
endif

