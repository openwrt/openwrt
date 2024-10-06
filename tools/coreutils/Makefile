#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=coreutils
PKG_CPE_ID:=cpe:/a:gnu:coreutils
PKG_VERSION:=9.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/coreutils
PKG_HASH:=767ae6a22950ec42f3ba5f7c1de79dd27800ee8e9b8642da5dedb5974a1741e5

HOST_BUILD_PARALLEL := 1

PKG_PROGRAMS:=chown cp date ginstall ln readlink realpath rmdir seq stat touch

include $(INCLUDE_DIR)/host-build.mk

export GNULIB_SRCDIR:=$(HOST_GNULIB_SRCDIR)

HOST_GNULIB_SKIP := \
	lib/fcntl.in.h

HOST_CONFIGURE_ARGS += \
	 --enable-install-program=$(subst $(space),$(comma),$(strip $(PKG_PROGRAMS)))

HOST_MAKE_FLAGS += \
	$(AM_TOOL_PATHS) AUTOPOINT="$(TRUE)" GTKDOCIZE="$(TRUE)" \
	PROGRAMS="$(patsubst %,src/%,$(PKG_PROGRAMS))" \
	LIBRARIES= MANS= SUBDIRS=.

define Host/Bootstrap
	( \
		cd $(HOST_BUILD_DIR); \
		$(AM_TOOL_PATHS) AUTOPOINT="$(TRUE)" GTKDOCIZE="$(TRUE)" \
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
	$(foreach prog,$(filter-out g%,$(PKG_PROGRAMS)),$(INSTALL_BIN) $(patsubst %,$(HOST_BUILD_DIR)/src/%,$(prog)) $(1)/bin/g$(prog); )
	$(foreach prog,$(filter g%,$(PKG_PROGRAMS)),$(INSTALL_BIN) $(patsubst %,$(HOST_BUILD_DIR)/src/%,$(prog)) $(1)/bin/$(prog); )
	$(foreach prog,$(filter-out g%,$(PKG_PROGRAMS)),$(1)/bin/g$(LN) g$(prog) $(1)/bin/$(prog); )
	$(foreach prog,$(filter g%,$(PKG_PROGRAMS)),$(1)/bin/g$(LN) $(prog) $(1)/bin/$(patsubst g%,%,$(prog)); )
endef

define Host/Uninstall
	rm -f $(STAGING_DIR_HOST)/bin/install
	#$(call Host/Compile/Default,uninstall) removes necessary symlinks
endef

$(eval $(call HostBuild))
