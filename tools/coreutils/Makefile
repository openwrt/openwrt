#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=coreutils
PKG_CPE_ID:=cpe:/a:gnu:coreutils
PKG_VERSION:=9.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/coreutils
PKG_HASH:=5f600d9093973b0afe25393d9bc18c44f2232657f4ca0d95ea31c702eb66b739

HOST_BUILD_PARALLEL := 1

PKG_PROGRAMS:=date readlink touch ln chown ginstall

include $(INCLUDE_DIR)/host-build.mk

export GNULIB_SRCDIR:=$(HOST_GNULIB_SRCDIR)

HOST_GNULIB_SKIP := \
	lib/math.in.h \
	lib/stdlib.in.h \
	lib/string.in.h \
	lib/uchar.in.h \
	lib/unistd.in.h \
	lib/wchar.in.h \
	lib/wctype.in.h

HOST_CONFIGURE_ARGS += \
	--disable-year2038 \
	 --enable-install-program=$(subst $(space),$(comma),$(strip $(PKG_PROGRAMS)))

HOST_MAKE_FLAGS += \
	$(AM_TOOL_PATHS_FAKE) \
	PROGRAMS="$(patsubst %,src/%,$(PKG_PROGRAMS))" \
	LIBRARIES= MANS= SUBDIRS=.

define Host/Bootstrap
	( \
		cd $(HOST_BUILD_DIR); \
		$(AM_TOOL_PATHS_FAKE) \
		./bootstrap \
			--bootstrap-sync \
			--force \
			--no-git \
			--skip-po \
			--gnulib-srcdir=$(GNULIB_SRCDIR) \
	)
endef

define Host/Prepare
	$(call Host/Prepare/Default)
	$(if $(QUILT),,$(call Host/Bootstrap))
endef

define Host/Configure
	$(if $(QUILT),$(call Host/Bootstrap))
	$(foreach src,$(HOST_GNULIB_SKIP),mv -f $(HOST_BUILD_DIR)/$(src)~ $(HOST_BUILD_DIR)/$(src) || true; )
	$(call Host/Configure/Default)
endef

define Host/Install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(patsubst %,$(HOST_BUILD_DIR)/src/%,$(PKG_PROGRAMS)) $(1)/bin/
	ln -sf ginstall $(1)/bin/install
endef

define Host/Uninstall
	rm -f $(STAGING_DIR_HOST)/bin/install
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
