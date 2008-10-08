#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

all: $(if $(DUMP),dumpinfo,compile)

PKG_BUILD_DIR ?= $(BUILD_DIR)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
PKG_INSTALL_DIR ?= $(PKG_BUILD_DIR)/ipkg-install
PKG_MD5SUM ?= unknown

include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/depends.mk

STAMP_PREPARED=$(PKG_BUILD_DIR)/.prepared$(if $(QUILT)$(DUMP),,_$(shell $(call find_md5,${CURDIR} $(PKG_FILE_DEPEND),)))
STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/.configured
STAMP_BUILT:=$(PKG_BUILD_DIR)/.built
STAMP_INSTALLED:=$(STAGING_DIR)/stamp/.$(PKG_NAME)_installed

include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/quilt.mk
include $(INCLUDE_DIR)/package-defaults.mk
include $(INCLUDE_DIR)/package-dumpinfo.mk
include $(INCLUDE_DIR)/package-ipkg.mk
include $(INCLUDE_DIR)/package-bin.mk
include $(INCLUDE_DIR)/autotools.mk

override MAKEFLAGS=
CONFIG_SITE:=$(INCLUDE_DIR)/site/$(patsubst %gnueabi,%,$(REAL_GNU_TARGET_NAME))
ifneq ($(CONFIG_LINUX_2_4),)
  CONFIG_SITE:=$(subst linux-,linux2.4-,$(CONFIG_SITE))
endif
export CONFIG_SITE
CUR_MAKEFILE:=$(filter-out Makefile,$(firstword $(MAKEFILE_LIST)))
SUBMAKE:=$(NO_TRACE_MAKE) $(if $(CUR_MAKEFILE),-f $(CUR_MAKEFILE))

ifeq ($(DUMP)$(filter prereq clean refresh update,$(MAKECMDGOALS)),)
  ifneq ($(if $(QUILT),,$(CONFIG_AUTOREBUILD)),)
    define Build/Autoclean
      $(PKG_BUILD_DIR)/.dep_files: $(STAMP_PREPARED)
      $(call rdep,${CURDIR} $(PKG_FILE_DEPEND),$(STAMP_PREPARED),$(PKG_BUILD_DIR)/.dep_files,-x "*/.dep_*")
      $(if $(filter prepare,$(MAKECMDGOALS)),,$(call rdep,$(PKG_BUILD_DIR),$(STAMP_BUILT),,-x "*/.dep_*" -x "*/ipkg*"))
    endef
  endif
endif

define Download/default
  FILE:=$(PKG_SOURCE)
  URL:=$(PKG_SOURCE_URL)
  SUBDIR:=$(PKG_SOURCE_SUBDIR)
  PROTO:=$(PKG_SOURCE_PROTO)
  $(if $(PKG_SOURCE_MIRROR),MIRROR:=$(filter 1,$(PKG_MIRROR)))
  VERSION:=$(PKG_SOURCE_VERSION)
  MD5SUM:=$(PKG_MD5SUM)
endef

define sep

endef

define Build/DefaultTargets
  $(if $(QUILT),$(Build/Quilt))
  $(if $(strip $(PKG_SOURCE_URL)),$(call Download,default))
  $(call Build/Autoclean)

  $(STAMP_PREPARED):
	@-rm -rf $(PKG_BUILD_DIR)
	@mkdir -p $(PKG_BUILD_DIR)
	$(foreach hook,$(Hooks/Prepare/Pre),$(call $(hook))$(sep))
	$(Build/Prepare)
	$(foreach hook,$(Hooks/Prepare/Post),$(call $(hook))$(sep))
	touch $$@

  $(STAMP_CONFIGURED): $(STAMP_PREPARED)
	$(foreach hook,$(Hooks/Configure/Pre),$(call $(hook))$(sep))
	$(Build/Configure)
	$(foreach hook,$(Hooks/Configure/Post),$(call $(hook))$(sep))
	touch $$@

  $(STAMP_BUILT): $(STAMP_CONFIGURED)
	$(foreach hook,$(Hooks/Compile/Pre),$(call $(hook))$(sep))
	$(Build/Compile)
	$(foreach hook,$(Hooks/Compile/Post),$(call $(hook))$(sep))
	$(Build/Install)
	$(foreach hook,$(Hooks/Install/Post),$(call $(hook))$(sep))
	touch $$@

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
		(cd $(TMP_DIR)/stage-$(PKG_NAME); find ./ > $(STAGING_DIR)/packages/$(PKG_NAME).list); \
		$(CP) $(TMP_DIR)/stage-$(PKG_NAME)/* $(STAGING_DIR)/; \
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
endef

define BuildPackage
  $(eval $(Package/Default))
  $(eval $(Package/$(1)))

ifdef DESCRIPTION
$$(error DESCRIPTION:= is obselete, use Package/PKG_NAME/description)
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
    $(Dumpinfo), \
    $(foreach target, \
      $(if $(Package/$(1)/targets),$(Package/$(1)/targets), \
        $(if $(PKG_TARGETS),$(PKG_TARGETS), ipkg ) \
      ), $(BuildTarget/$(target)) \
    ) \
  )
  $(if $(DUMP),,$(call Build/DefaultTargets,$(1)))
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
		if [ -f packages/$(PKG_NAME).list ]; then \
			cat packages/$(PKG_NAME).list | xargs -r rm -f 2>/dev/null; \
		fi; \
	)

clean: clean-staging FORCE
	$(call Build/UninstallDev,$(STAGING_DIR),$(STAGING_DIR_HOST))
	$(Build/Clean)
	rm -f $(STAGING_DIR)/packages/$(PKG_NAME).list $(STAGING_DIR_HOST)/packages/$(PKG_NAME).list
	rm -rf $(PKG_BUILD_DIR)
