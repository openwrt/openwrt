# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define replace
	if [ -f "$(PKG_BUILD_DIR)/$(3)$(1)" -a -e "$(2)/$(1)" ]; then \
		rm -f $(PKG_BUILD_DIR)/$(3)$(1); \
		ln -s $(2)/$(1) $(PKG_BUILD_DIR)/$(3); \
	fi
	
endef

# replace copies of ltmain.sh with the build system's version
update_libtool=$(call replace,libtool,$(STAGING_DIR)/host/bin,$(CONFIGURE_PATH)/)$(call replace,ltmain.sh,$(STAGING_DIR)/host/share/libtool,$(CONFIGURE_PATH)/)

# prevent libtool from linking against host development libraries
define libtool_fixup_libdir
	find $(PKG_BUILD_DIR) -name '*.la' | $(XARGS) \
		$(SED) "s,^libdir='/usr/lib',libdir='$(strip $(1))/usr/lib',g"
endef

ifneq ($(filter libtool,$(PKG_FIXUP)),)
  PKG_BUILD_DEPENDS += libtool
  Hooks/Configure/Pre += update_libtool
  Hooks/Configure/Post += update_libtool
  Hooks/Build/Post += libtool_fixup_libdir
endif
