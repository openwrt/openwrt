#
# Copyright (C) 2007-2009 OpenWrt.org
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

# replace copies of ltmain.sh with the build system's version
update_libtool_common=$(call replace,ltmain.sh,$(STAGING_DIR)/host/share/libtool,$(CONFIGURE_PATH)/)$(call replace,libtool.m4,$(STAGING_DIR)/host/share/aclocal,$(CONFIGURE_PATH)/)
update_libtool=$(call replace,libtool,$(STAGING_DIR)/host/bin,$(CONFIGURE_PATH)/)$(call update_libtool_common)
update_libtool_ucxx=$(call replace,libtool,$(STAGING_DIR)/host/bin,$(CONFIGURE_PATH)/,libtool-ucxx)$(call update_libtool_common)


# prevent libtool from linking against host development libraries
define libtool_fixup_libdir
	find $(1) -name '*.la' | $(XARGS) \
		$(SED) "s,\(^libdir='\| \|-L\|^dependency_libs='\)/usr/lib,\1$(STAGING_DIR)/usr/lib,g" \
		    -e "s,$(STAGING_DIR)/usr/lib/\(libstdc++\|libsupc++\).la,$(TOOLCHAIN_DIR)/usr/lib/\1.la,g"
	find $(2) -name '*.la' | $(XARGS) \
		$(SED) "s,\(^libdir='\| \|-L\|^dependency_libs='\)/usr/lib,\1$(STAGING_DIR)/usr/lib,g" \
		    -e "s,$(STAGING_DIR)/usr/lib/\(libstdc++\|libsupc++\).la,$(TOOLCHAIN_DIR)/usr/lib/\1.la,g"
endef

define remove_version_check
	if [ -f "$(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/configure" ]; then \
		$(SED) \
			's,pardus_ltmain_version=.*,pardus_ltmain_version="$$$$pardus_lt_version",' \
			$(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/configure; \
	fi
endef

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
  Hooks/Configure/Pre += update_libtool remove_version_check
  Hooks/Configure/Post += update_libtool
  Hooks/InstallDev/Post += libtool_fixup_libdir
endif

ifneq ($(filter libtool-ucxx,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
  Hooks/Configure/Pre += update_libtool_ucxx remove_version_check
  Hooks/Configure/Post += update_libtool_ucxx
  Hooks/InstallDev/Post += libtool_fixup_libdir
endif

