# 
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

all: $(if $(DUMP),dumpinfo,compile)

PKG_BUILD_DIR ?= $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR ?= $(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/depends.mk

STAMP_PREPARED:=$(PKG_BUILD_DIR)/.prepared$(if $(DUMP),,_$(shell find ${CURDIR} $(PKG_FILE_DEPEND) $(DEP_FINDPARAMS) | md5s))
STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/.configured
STAMP_BUILT:=$(PKG_BUILD_DIR)/.built

include $(INCLUDE_DIR)/quilt.mk
include $(INCLUDE_DIR)/package-defaults.mk
include $(INCLUDE_DIR)/package-dumpinfo.mk
include $(INCLUDE_DIR)/package-ipkg.mk

export CONFIG_SITE:=$(INCLUDE_DIR)/site/$(REAL_GNU_TARGET_NAME)

ifneq ($(CONFIG_AUTOREBUILD),)
  define Build/Autoclean
    $(PKG_BUILD_DIR)/.dep_files: $(STAMP_PREPARED)
    $(call rdep,${CURDIR} $(PKG_FILE_DEPEND),$(STAMP_PREPARED))
    $(call rdep,$(PKG_BUILD_DIR),$(STAMP_BUILT),$(PKG_BUILD_DIR)/.dep_files, -and -not -path "/.*" -and -not -path "*/ipkg*")
  endef
endif

define Build/DefaultTargets
  ifneq ($(strip $(PKG_SOURCE_URL)),)
    download: $(DL_DIR)/$(PKG_SOURCE)

    $(DL_DIR)/$(PKG_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_SOURCE)" "$(PKG_MD5SUM)" $(PKG_SOURCE_URL)

    $(STAMP_PREPARED): $(DL_DIR)/$(PKG_SOURCE)
  endif

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
	@$(NO_TRACE_MAKE) $(PKG_BUILD_DIR)/.dep_files
	touch $$@

  ifdef Build/InstallDev
    compile: $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed
    $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed: $(STAMP_BUILT)
	mkdir -p $(STAGING_DIR)/stampfiles
	$(Build/InstallDev)
	touch $$@
  endif

  define Build/DefaultTargets
  endef
endef

define BuildPackage
  $(eval $(Package/Default))
  $(eval $(Package/$(1)))

# <HACK> Support obsolete DESCRIPTION field
ifndef Package/$(1)/description
define Package/$(1)/description
$(TITLE)$(subst \,
,\ $(DESCRIPTION))
endef
endif
# </HACK>

  $(foreach FIELD, TITLE CATEGORY PRIORITY SECTION VERSION,
    ifeq ($($(FIELD)),)
      $$(error Package/$(1) is missing the $(FIELD) field)
    endif
  )

  $(call shexport,Package/$(1)/description)
  $(call shexport,Package/$(1)/config)

  $(Dumpinfo)
  $(BuildIPKG)
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
prepare: $(STAMP_PREPARED)
configure: $(STAMP_CONFIGURED)
compile:
install:
clean: FORCE
	$(Build/UninstallDev)
	$(Build/Clean)
	@rm -f $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed
	@rm -rf $(PKG_BUILD_DIR)
