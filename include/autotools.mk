#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define replace
	if [ -f "$(PKG_BUILD_DIR)/$(3)$(1)" -a -e "$(2)/$(if $(4),$(4),$(1))" ]; then \
		rm -f $(PKG_BUILD_DIR)/$(3)$(1); \
		ln -s $(2)/$(if $(4),$(4),$(1)) $(PKG_BUILD_DIR)/$(3)$(1); \
	fi
	
endef

PKG_LIBTOOL_PATHS?=$(CONFIGURE_PATH)

# replace copies of ltmain.sh with the build system's version
update_libtool_common = \
	$(foreach p,$(LIBTOOL_PATHS), \
		$(call replace,ltmain.sh,$(STAGING_DIR)/host/share/libtool,$(p)/) \
		$(call replace,libtool.m4,$(STAGING_DIR)/host/share/aclocal,$(p)/) \
	)
update_libtool = \
	$(foreach p,$(PKG_LIBTOOL_PATHS), \
		$(call replace,libtool,$(STAGING_DIR)/host/bin,$(p)/) \
	) \
	$(call update_libtool_common)
update_libtool_ucxx = \
	$(foreach p,$(PKG_LIBTOOL_PATHS), \
		$(call replace,libtool,$(STAGING_DIR)/host/bin,$(p)/,libtool-ucxx) \
	) \
	$(call update_libtool_common)

autoconf_bool = $(patsubst %,$(if $($(1)),--enable,--disable)-%,$(2))

# prevent libtool from linking against host development libraries
define libtool_fixup_libdir
	find $(1) -name '*.la' | $(XARGS) \
		$(SED) "s,\(^libdir='\| \|-L\|^dependency_libs='\)/usr/lib,\1$(STAGING_DIR)/usr/lib,g" \
		    -e "s,$(STAGING_DIR)/usr/lib/\(libstdc++\|libsupc++\).la,$(TOOLCHAIN_DIR)/lib/\1.la,g"
endef

define remove_version_check
	if [ -f "$(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/configure" ]; then \
		$(SED) \
			's,\(gentoo\|pardus\)_ltmain_version=.*,\1_ltmain_version="$$$$\1_lt_version",' \
			$(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/configure; \
	fi
endef

# delete *.la-files from staging_dir - we can not yet remove respective lines within all package
# Makefiles, since backfire still uses libtool v1.5.x which (may) require those files
define libtool_remove_files
	find $(1) -name '*.la' | $(XARGS) rm -f
endef

Hooks/InstallDev/Post += libtool_remove_files

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  # WARNING: the use of PKG_FIXUP is DEPRECATED
  PKG_BUILD_DEPENDS += libtool
endif
 
ifneq ($(filter libtool-ucxx,$(PKG_FIXUP)),)
  # WARNING: the use of PKG_FIXUP is DEPRECATED
  PKG_BUILD_DEPENDS += libtool
endif

