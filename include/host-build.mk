# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PKG_BUILD_DIR ?= $(BUILD_DIR_HOST)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
PKG_INSTALL_DIR ?= $(PKG_BUILD_DIR)/host-install

include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/depends.mk

STAMP_PREPARED=$(PKG_BUILD_DIR)/.prepared$(if $(QUILT)$(DUMP),,$(shell $(call find_md5,${CURDIR} $(PKG_FILE_DEPEND),)))
STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/.configured
STAMP_BUILT:=$(PKG_BUILD_DIR)/.built
STAMP_INSTALLED:=$(STAGING_DIR_HOST)/stamp/.$(PKG_NAME)_installed

override MAKEFLAGS=

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/quilt.mk

Build/Patch:=$(Build/Patch/Default)
ifneq ($(strip $(PKG_UNPACK)),)
  define Build/Prepare/Default
  	$(PKG_UNPACK)
	$(Build/Patch)
	$(if $(QUILT),touch $(PKG_BUILD_DIR)/.quilt_used)
  endef
endif

define Build/Prepare
  $(call Build/Prepare/Default)
endef

define Build/Configure/Default
	@(cd $(PKG_BUILD_DIR)/$(3); \
	[ -x configure ] && \
		$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/$(3)/ && \
		$(2) \
		CPPFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		SHELL="$(BASH)" \
		./configure \
		--target=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(STAGING_DIR_HOST) \
		--exec-prefix=$(STAGING_DIR_HOST) \
		--sysconfdir=$(STAGING_DIR_HOST)/etc \
		--localstatedir=$(STAGING_DIR_HOST)/var \
		$(DISABLE_NLS) \
		$(1); \
		true; \
	)
endef

define Build/Configure
  $(call Build/Configure/Default)
endef

define Build/Compile/Default
	$(MAKE) -C $(PKG_BUILD_DIR) $(1)
endef

define Build/Compile
  $(call Build/Compile/Default)
endef

ifneq ($(if $(QUILT),,$(CONFIG_AUTOREBUILD)),)
  define HostBuild/Autoclean
    $(call rdep,${CURDIR} $(PKG_FILE_DEPEND),$(STAMP_PREPARED))
    $(if $(if $(Build/Compile),$(filter prepare,$(MAKECMDGOALS)),1),,$(call rdep,$(PKG_BUILD_DIR),$(STAMP_BUILT)))
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

define HostBuild
  $(if $(QUILT),$(Build/Quilt))
  $(if $(strip $(PKG_SOURCE_URL)),$(call Download,default))
  $(if $(DUMP),,$(call HostBuild/Autoclean))
  
  $(STAMP_PREPARED):
	@-rm -rf $(PKG_BUILD_DIR)
	@mkdir -p $(PKG_BUILD_DIR)
	$(call Build/Prepare)
	touch $$@

  $(STAMP_CONFIGURED): $(STAMP_PREPARED)
	$(call Build/Configure)
	touch $$@

  $(STAMP_BUILT): $(STAMP_CONFIGURED)
	$(call Build/Compile)
	touch $$@

  $(STAMP_INSTALLED): $(STAMP_BUILT)
	$(call Build/Install)
	mkdir -p $$(shell dirname $$@)
	touch $$@
	
  ifdef Build/Install
    install: $(STAMP_INSTALLED)
  endif

  package-clean: FORCE
	$(call Build/Clean)
	$(call Build/Uninstall)
	rm -f $(STAMP_INSTALLED) $(STAMP_BUILT)

  download:
  prepare: $(STAMP_PREPARED)
  configure: $(STAMP_CONFIGURED)
  compile: $(STAMP_BUILT)
  install:
  clean: FORCE
	$(call Build/Clean)
	rm -rf $(PKG_BUILD_DIR)

endef
