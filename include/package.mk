# 
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

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

override MAKEFLAGS=
export CONFIG_SITE:=$(INCLUDE_DIR)/site/$(REAL_GNU_TARGET_NAME)

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
  VERSION:=$(PKG_SOURCE_VERSION)
  MD5SUM:=$(PKG_MD5SUM)
endef

define Build/DefaultTargets
  $(if $(QUILT),$(Build/Quilt))
  $(if $(strip $(PKG_SOURCE_URL)),$(call Download,default))
  $(call Build/Autoclean)

  $(STAMP_PREPARED):
	@-rm -rf $(PKG_BUILD_DIR)
	@mkdir -p $(PKG_BUILD_DIR)
	$(Build/Prepare)
	touch $$@

  $(STAMP_CONFIGURED): $(STAMP_PREPARED)
	$(Build/Configure)
	touch $$@

  $(STAMP_BUILT): $(STAMP_CONFIGURED)
	$(Build/Compile)
	touch $$@

  $(STAMP_INSTALLED): $(STAMP_BUILT)
	$(call Build/InstallDev,$(STAGING_DIR),$(STAGING_DIR_HOST))
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

# prevent libtool from setting rpath when linking
define libtool_disable_rpath
	find $(PKG_BUILD_DIR) -name 'libtool' | $(XARGS) \
		$(SED) 's,^hardcode_libdir_flag_spec=.*,hardcode_libdir_flag_spec=" -D__LIBTOOL_IS_A_FOOL__ ",g'
endef

# prevent libtool from linking against host development libraries
define libtool_fixup_libdir
	find $(PKG_BUILD_DIR) -name '*.la' | $(XARGS) \
		$(SED) "s,^libdir='/usr/lib',libdir='$(strip $(1))/usr/lib',g"
endef

define pkg_install_files
	$(foreach install_file,$(1),$(INSTALL_DIR) $(3)/`dirname $(install_file)`; $(INSTALL_DATA) $(2)/$(install_file) $(3)/`dirname $(install_file)`;)
endef

define pkg_install_bin
	$(foreach install_apps,$(1),$(INSTALL_DIR) $(3)/`dirname $(install_apps)`; $(INSTALL_BIN) $(2)/$(install_apps) $(3)/`dirname $(install_apps)`;)
endef

define Build/Prepare
  $(call Build/Prepare/Default,)
endef

define Build/Configure
  $(call Build/Configure/Default,)
endef

define Build/Compile
  $(call Build/Compile/Default,)
endef

$(PACKAGE_DIR):
	mkdir -p $@
		
dumpinfo:
download:
prepare:
configure:
compile:
install:
clean: FORCE
	$(call Build/UninstallDev,$(STAGING_DIR),$(STAGING_DIR_HOST))
	$(Build/Clean)
	@rm -f $(STAGING_DIR)/stamp/.$(PKG_NAME)-installed
	@rm -rf $(PKG_BUILD_DIR)
