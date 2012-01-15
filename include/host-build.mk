#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

HOST_BUILD_DIR ?= $(BUILD_DIR_HOST)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
HOST_INSTALL_DIR ?= $(HOST_BUILD_DIR)/host-install
HOST_BUILD_PARALLEL ?=

ifeq ($(strip $(HOST_BUILD_PARALLEL)),0)
HOST_JOBS?=-j1
else
HOST_JOBS?=$(if $(HOST_BUILD_PARALLEL)$(CONFIG_PKG_DEFAULT_PARALLEL),\
	$(if $(CONFIG_PKG_BUILD_PARALLEL),-j$(CONFIG_PKG_BUILD_JOBS),-j1),-j1)
endif

include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/depends.mk

BUILD_TYPES += host
HOST_STAMP_PREPARED=$(HOST_BUILD_DIR)/.prepared$(if $(HOST_QUILT)$(DUMP),,$(shell $(call find_md5,${CURDIR} $(PKG_FILE_DEPENDS),)))
HOST_STAMP_CONFIGURED:=$(HOST_BUILD_DIR)/.configured
HOST_STAMP_BUILT:=$(HOST_BUILD_DIR)/.built
HOST_STAMP_INSTALLED:=$(STAGING_DIR_HOST)/stamp/.$(PKG_NAME)_installed

override MAKEFLAGS=

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/quilt.mk
include $(INCLUDE_DIR)/autotools.mk

Host/Patch:=$(Host/Patch/Default)
ifneq ($(strip $(HOST_UNPACK)),)
  define Host/Prepare/Default
	$(HOST_UNPACK)
	$(Host/Patch)
  endef
endif

define Host/Prepare
  $(call Host/Prepare/Default)
endef

HOST_CONFIGURE_VARS = \
	CC="$(HOSTCC)" \
	CFLAGS="$(HOST_CFLAGS)" \
	CPPFLAGS="$(HOST_CFLAGS)" \
	LDFLAGS="$(HOST_LDFLAGS)" \
	SHELL="$(BASH)"

HOST_CONFIGURE_ARGS = \
	--target=$(GNU_HOST_NAME) \
	--host=$(GNU_HOST_NAME) \
	--build=$(GNU_HOST_NAME) \
	--program-prefix="" \
	--program-suffix="" \
	--prefix=$(STAGING_DIR_HOST) \
	--exec-prefix=$(STAGING_DIR_HOST) \
	--sysconfdir=$(STAGING_DIR_HOST)/etc \
	--localstatedir=$(STAGING_DIR_HOST)/var \
	--sbindir=$(STAGING_DIR_HOST)/bin

HOST_CONFIGURE_CMD = ./configure

define Host/Configure/Default
	(cd $(HOST_BUILD_DIR)/$(3); \
		if [ -x configure ]; then \
			$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(HOST_BUILD_DIR)/$(3)/ && \
			$(2) \
			$(HOST_CONFIGURE_CMD) \
			$(HOST_CONFIGURE_VARS) \
			$(HOST_CONFIGURE_ARGS) \
			$(1); \
		fi \
	)
endef

define Host/Configure
  $(call Host/Configure/Default)
endef

define Host/Compile/Default
	$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) $(1)
endef

define Host/Compile
  $(call Host/Compile/Default)
endef

define Host/Install/Default
	$(_SINGLE)$(MAKE) -C $(HOST_BUILD_DIR) install
endef

define Host/Install
  $(call Host/Install/Default)
endef


ifneq ($(if $(HOST_QUILT),,$(CONFIG_AUTOREBUILD)),)
  define HostHost/Autoclean
    $(call rdep,${CURDIR} $(PKG_FILE_DEPENDS),$(HOST_STAMP_PREPARED))
    $(if $(if $(Host/Compile),$(filter prepare,$(MAKECMDGOALS)),1),,$(call rdep,$(HOST_BUILD_DIR),$(HOST_STAMP_BUILT)))
  endef
endif

define Download/default
  FILE:=$(PKG_SOURCE)
  URL:=$(PKG_SOURCE_URL)
  PROTO:=$(PKG_SOURCE_PROTO)
  SUBDIR:=$(PKG_SOURCE_SUBDIR)
  VERSION:=$(PKG_SOURCE_VERSION)
  MD5SUM:=$(PKG_MD5SUM)
endef

define Host/Exports/Default
  $(1) : export ACLOCAL_INCLUDE=$$(foreach p,$$(wildcard $$(STAGING_DIR_HOST)/share/aclocal $$(STAGING_DIR_HOST)/share/aclocal-*),-I $$(p))
  $(1) : export STAGING_PREFIX=$$(STAGING_DIR_HOST)
  $(1) : export PKG_CONFIG_PATH=$$(STAGING_DIR_HOST)/lib/pkgconfig
  $(1) : export PKG_CONFIG_LIBDIR=$$(STAGING_DIR_HOST)/lib/pkgconfig
endef
Host/Exports=$(Host/Exports/Default)

ifndef DUMP
  define HostBuild
  $(if $(HOST_QUILT),$(Host/Quilt))
  $(if $(if $(PKG_HOST_ONLY),,$(STAMP_PREPARED)),,$(if $(strip $(PKG_SOURCE_URL)),$(call Download,default)))
  $(if $(DUMP),,$(call HostHost/Autoclean))

  $(HOST_STAMP_PREPARED):
	@-rm -rf $(HOST_BUILD_DIR)
	@mkdir -p $(HOST_BUILD_DIR)
	$(foreach hook,$(Hooks/HostPrepare/Pre),$(call $(hook))$(sep))
	$(call Host/Prepare)
	$(foreach hook,$(Hooks/HostPrepare/Post),$(call $(hook))$(sep))
	touch $$@

  $(call Host/Exports,$(HOST_STAMP_CONFIGURED))
  $(HOST_STAMP_CONFIGURED): $(HOST_STAMP_PREPARED)
	$(foreach hook,$(Hooks/HostConfigure/Pre),$(call $(hook))$(sep))
	$(call Host/Configure)
	$(foreach hook,$(Hooks/HostConfigure/Post),$(call $(hook))$(sep))
	touch $$@

  $(call Host/Exports,$(HOST_STAMP_BUILT))
  ifdef Host/Install
    host-install: $(if $(STAMP_BUILT),$(HOST_STAMP_BUILT),$(HOST_STAMP_INSTALLED))
  endif

  ifndef STAMP_BUILT
    prepare: host-prepare
    compile: host-compile
    install: host-install
    clean: host-clean
    update: host-update
    refresh: host-refresh

    $(HOST_STAMP_BUILT): $(HOST_STAMP_CONFIGURED)
		$(foreach hook,$(Hooks/HostCompile/Pre),$(call $(hook))$(sep))
		$(call Host/Compile)
		$(foreach hook,$(Hooks/HostCompile/Post),$(call $(hook))$(sep))
		touch $$@

    $(HOST_STAMP_INSTALLED): $(HOST_STAMP_BUILT) $(if $(FORCE_HOST_INSTALL),FORCE)
		$(call Host/Install)
		$(foreach hook,$(Hooks/HostInstall/Post),$(call $(hook))$(sep))
		mkdir -p $$(shell dirname $$@)
		touch $$@
  else
    $(HOST_STAMP_BUILT): $(HOST_STAMP_CONFIGURED) $(if $(FORCE_HOST_INSTALL),FORCE)
		$(foreach hook,$(Hooks/HostCompile/Pre),$(call $(hook))$(sep))
		$(call Host/Compile)
		$(foreach hook,$(Hooks/HostCompile/Post),$(call $(hook))$(sep))
		$(call Host/Install)
		$(foreach hook,$(Hooks/HostInstall/Post),$(call $(hook))$(sep))
		touch $$@
  endif
  host-prepare: $(HOST_STAMP_PREPARED)
  host-configure: $(HOST_STAMP_CONFIGURED)
  host-compile: $(HOST_STAMP_BUILT)
  host-install:
  host-clean: FORCE
	$(call Host/Clean)
	$(call Host/Uninstall)
	rm -rf $(HOST_BUILD_DIR) $(HOST_STAMP_INSTALLED) $(HOST_STAMP_BUILT)

  endef

  download:
  prepare:
  compile:
  install:
  clean:

endif
