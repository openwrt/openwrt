#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

all: $(if $(DUMP),dumpinfo,compile)

PKG_BUILD_DIR ?= $(BUILD_DIR)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
PKG_INSTALL_DIR ?= $(PKG_BUILD_DIR)/ipkg-install
PKG_MD5SUM ?= unknown
PKG_BUILD_PARALLEL ?=

ifeq ($(strip $(PKG_BUILD_PARALLEL)),0)
PKG_JOBS?=-j1
else
PKG_JOBS?=$(if $(PKG_BUILD_PARALLEL)$(CONFIG_PKG_DEFAULT_PARALLEL),\
	$(if $(CONFIG_PKG_BUILD_PARALLEL),-j$(CONFIG_PKG_BUILD_JOBS),-j1),-j1)
endif

include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/depends.mk

STAMP_NO_AUTOREBUILD=$(wildcard $(PKG_BUILD_DIR)/.no_autorebuild)
PREV_STAMP_PREPARED:=$(if $(STAMP_NO_AUTOREBUILD),$(wildcard $(PKG_BUILD_DIR)/.prepared*))
ifneq ($(PREV_STAMP_PREPARED),)
  STAMP_PREPARED:=$(PREV_STAMP_PREPARED)
  CONFIG_AUTOREBUILD:=
else
  STAMP_PREPARED=$(PKG_BUILD_DIR)/.prepared$(if $(QUILT)$(DUMP),,_$(shell $(call find_md5,${CURDIR} $(PKG_FILE_DEPENDS),)))
endif
STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/.configured$(if $(DUMP),,_$(call confvar,$(PKG_CONFIG_DEPENDS)))
STAMP_CONFIGURED_WILDCARD=$(patsubst %_$(call confvar,$(PKG_CONFIG_DEPENDS)),%_*,$(STAMP_CONFIGURED))
STAMP_BUILT:=$(PKG_BUILD_DIR)/.built
STAMP_INSTALLED:=$(STAGING_DIR)/stamp/.$(PKG_NAME)_installed

STAGING_FILES_LIST:=$(PKG_NAME)$(if $(BUILD_VARIANT),.$(BUILD_VARIANT),).list
ifneq ($(if $(CONFIG_SRC_TREE_OVERRIDE),$(wildcard ./git-src)),)
  USE_GIT_TREE:=1
  QUILT:=1
endif

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/quilt.mk
include $(INCLUDE_DIR)/package-defaults.mk
include $(INCLUDE_DIR)/package-dumpinfo.mk
include $(INCLUDE_DIR)/package-ipkg.mk
include $(INCLUDE_DIR)/package-bin.mk
include $(INCLUDE_DIR)/autotools.mk

override MAKEFLAGS=
CONFIG_SITE:=$(INCLUDE_DIR)/site/$(REAL_GNU_TARGET_NAME)
CUR_MAKEFILE:=$(filter-out Makefile,$(firstword $(MAKEFILE_LIST)))
SUBMAKE:=$(NO_TRACE_MAKE) $(if $(CUR_MAKEFILE),-f $(CUR_MAKEFILE))
PKG_CONFIG_PATH=$(STAGING_DIR)/usr/lib/pkgconfig

ifeq ($(DUMP)$(filter prereq clean refresh update,$(MAKECMDGOALS)),)
  ifneq ($(if $(QUILT),,$(CONFIG_AUTOREBUILD)),)
    define Build/Autoclean
      $(PKG_BUILD_DIR)/.dep_files: $(STAMP_PREPARED)
      $(call rdep,${CURDIR} $(PKG_FILE_DEPENDS),$(STAMP_PREPARED),$(PKG_BUILD_DIR)/.dep_files,-x "*/.dep_*")
      $(if $(filter prepare,$(MAKECMDGOALS)),,$(call rdep,$(PKG_BUILD_DIR),$(STAMP_BUILT),,-x "*/.dep_*" -x "*/ipkg*"))
    endef
  endif
endif

ifeq ($(CONFIG_$(PKG_NAME)_USE_CUSTOM_SOURCE_DIR),y)
# disable load stage
PKG_SOURCE_URL:=
# add hook to install a link to customer source path of dedicated package
Hooks/Prepare/Pre += prepare_custom_source_directory
# define empty default action
define Build/Prepare/Default
	@: 
endef
endif

define Download/default
  FILE:=$(PKG_SOURCE)
  URL:=$(PKG_SOURCE_URL)
  SUBDIR:=$(PKG_SOURCE_SUBDIR)
  PROTO:=$(PKG_SOURCE_PROTO)
  $(if $(PKG_SOURCE_MIRROR),MIRROR:=$(filter 1,$(PKG_MIRROR)))
  $(if $(PKG_MIRROR_MD5SUM),MIRROR_MD5SUM:=$(PKG_MIRROR_MD5SUM))
  VERSION:=$(PKG_SOURCE_VERSION)
  MD5SUM:=$(PKG_MD5SUM)
endef

ifdef USE_GIT_TREE
  define Build/Prepare/Default
	mkdir -p $(PKG_BUILD_DIR)
	ln -s $(CURDIR)/git-src $(PKG_BUILD_DIR)/.git
	( cd $(PKG_BUILD_DIR); git checkout .)
  endef
endif

define Build/Exports/Default
  $(1) : export ACLOCAL_INCLUDE=$$(foreach p,$$(wildcard $$(STAGING_DIR)/usr/share/aclocal $$(STAGING_DIR)/usr/share/aclocal-* $$(STAGING_DIR)/host/share/aclocal $$(STAGING_DIR)/host/share/aclocal-*),-I $$(p))
  $(1) : export STAGING_PREFIX=$$(STAGING_DIR)/usr
  $(1) : export PATH=$$(TARGET_PATH_PKG)
  $(1) : export CONFIG_SITE:=$$(CONFIG_SITE)
  $(1) : export PKG_CONFIG_PATH:=$$(PKG_CONFIG_PATH)
  $(1) : export PKG_CONFIG_LIBDIR:=$$(PKG_CONFIG_PATH)
  $(1) : export CCACHE_DIR:=$(STAGING_DIR)/ccache
endef
Build/Exports=$(Build/Exports/Default)

define Build/DefaultTargets
  $(if $(QUILT),$(Build/Quilt))
  $(if $(USE_GIT_TREE),,$(if $(strip $(PKG_SOURCE_URL)),$(call Download,default)))
  $(call Build/Autoclean)

  download:
	$(foreach hook,$(Hooks/Download),
		$(call $(hook))$(sep)
	)

  $(STAMP_PREPARED) : export PATH=$$(TARGET_PATH_PKG)
  $(STAMP_PREPARED):
	@-rm -rf $(PKG_BUILD_DIR)
	@mkdir -p $(PKG_BUILD_DIR)
	$(foreach hook,$(Hooks/Prepare/Pre),$(call $(hook))$(sep))
	$(Build/Prepare)
	$(foreach hook,$(Hooks/Prepare/Post),$(call $(hook))$(sep))
	touch $$@

  $(call Build/Exports,$(STAMP_CONFIGURED))
  $(STAMP_CONFIGURED): $(STAMP_PREPARED)
	$(foreach hook,$(Hooks/Configure/Pre),$(call $(hook))$(sep))
	$(Build/Configure)
	$(foreach hook,$(Hooks/Configure/Post),$(call $(hook))$(sep))
	rm -f $(STAMP_CONFIGURED_WILDCARD)
	touch $$@

  $(call Build/Exports,$(STAMP_BUILT))
  $(STAMP_BUILT): $(STAMP_CONFIGURED)
	$(foreach hook,$(Hooks/Compile/Pre),$(call $(hook))$(sep))
	$(Build/Compile)
	$(foreach hook,$(Hooks/Compile/Post),$(call $(hook))$(sep))
	$(Build/Install)
	$(foreach hook,$(Hooks/Install/Post),$(call $(hook))$(sep))
	touch $$@

  $(STAMP_INSTALLED) : export PATH=$$(TARGET_PATH_PKG)
  $(STAMP_INSTALLED): $(STAMP_BUILT)
	$(SUBMAKE) -j1 clean-staging
	rm -rf $(TMP_DIR)/stage-$(PKG_NAME)
	mkdir -p $(TMP_DIR)/stage-$(PKG_NAME)/host $(STAGING_DIR)/packages $(STAGING_DIR_HOST)/packages
	$(foreach hook,$(Hooks/InstallDev/Pre),\
		$(call $(hook),$(TMP_DIR)/stage-$(PKG_NAME),$(TMP_DIR)/stage-$(PKG_NAME)/host)$(sep)\
	)
	$(call Build/InstallDev,$(TMP_DIR)/stage-$(PKG_NAME),$(TMP_DIR)/stage-$(PKG_NAME)/host)
	$(foreach hook,$(Hooks/InstallDev/Post),\
		$(call $(hook),$(TMP_DIR)/stage-$(PKG_NAME),$(TMP_DIR)/stage-$(PKG_NAME)/host)$(sep)\
	)
	if [ -d $(TMP_DIR)/stage-$(PKG_NAME) ]; then \
		(cd $(TMP_DIR)/stage-$(PKG_NAME); find ./ > $(TMP_DIR)/stage-$(PKG_NAME).files); \
		$(call locked, \
			mv $(TMP_DIR)/stage-$(PKG_NAME).files $(STAGING_DIR)/packages/$(STAGING_FILES_LIST) && \
			$(CP) $(TMP_DIR)/stage-$(PKG_NAME)/* $(STAGING_DIR)/; \
		,staging-dir); \
	fi
	rm -rf $(TMP_DIR)/stage-$(PKG_NAME)
	touch $$@

  ifdef Build/InstallDev
    compile: $(STAMP_INSTALLED)
  endif

  define Build/DefaultTargets
  endef

  prepare: $(STAMP_PREPARED)
  configure: $(STAMP_CONFIGURED)
  dist: $(STAMP_CONFIGURED)
  distcheck: $(STAMP_CONFIGURED)
endef

define Build/IncludeOverlay
  $(eval -include $(wildcard $(TOPDIR)/overlay/*/$(PKG_NAME).mk))
  define Build/IncludeOverlay
  endef
endef

define BuildPackage
  $(Build/IncludeOverlay)
  $(eval $(Package/Default))
  $(eval $(Package/$(1)))

ifdef DESCRIPTION
$$(error DESCRIPTION:= is obsolete, use Package/PKG_NAME/description)
endif

ifndef Package/$(1)/description
define Package/$(1)/description
	$(TITLE)
endef
endif

  $(foreach FIELD, TITLE CATEGORY PRIORITY SECTION VERSION,
    ifeq ($($(FIELD)),)
      $$(error Package/$(1) is missing the $(FIELD) field)
    endif
  )

  $(call shexport,Package/$(1)/description)
  $(call shexport,Package/$(1)/config)

  $(if $(DUMP), \
    $(Dumpinfo/Package), \
    $(foreach target, \
      $(if $(Package/$(1)/targets),$(Package/$(1)/targets), \
        $(if $(PKG_TARGETS),$(PKG_TARGETS), ipkg) \
      ), $(BuildTarget/$(target)) \
    ) \
  )
  $(if $(PKG_HOST_ONLY)$(DUMP),,$(call Build/DefaultTargets,$(1)))
endef

define pkg_install_files
	$(foreach install_file,$(1),$(INSTALL_DIR) $(3)/`dirname $(install_file)`; $(INSTALL_DATA) $(2)/$(install_file) $(3)/`dirname $(install_file)`;)
endef

define pkg_install_bin
	$(foreach install_apps,$(1),$(INSTALL_DIR) $(3)/`dirname $(install_apps)`; $(INSTALL_BIN) $(2)/$(install_apps) $(3)/`dirname $(install_apps)`;)
endef

Build/Prepare=$(call Build/Prepare/Default,)
Build/Configure=$(call Build/Configure/Default,)
Build/Compile=$(call Build/Compile/Default,)
Build/Install=$(if $(PKG_INSTALL),$(call Build/Install/Default,))
Build/Dist=$(call Build/Dist/Default,)
Build/DistCheck=$(call Build/DistCheck/Default,)

$(PACKAGE_DIR):
	mkdir -p $@
	
dumpinfo:
download:
prepare:
configure:
compile:
install:
clean-staging: FORCE
	rm -f $(STAMP_INSTALLED)
	@-(\
		cd "$(STAGING_DIR)"; \
		if [ -f packages/$(STAGING_FILES_LIST) ]; then \
			cat packages/$(STAGING_FILES_LIST) | xargs -r rm -f 2>/dev/null; \
		fi; \
	)

clean: clean-staging FORCE
	$(call Build/UninstallDev,$(STAGING_DIR),$(STAGING_DIR_HOST))
	$(Build/Clean)
	rm -f $(STAGING_DIR)/packages/$(STAGING_FILES_LIST) $(STAGING_DIR_HOST)/packages/$(STAGING_FILES_LIST)
	rm -rf $(PKG_BUILD_DIR)

dist:
	$(Build/Dist)
   
distcheck:
	$(Build/DistCheck) 
