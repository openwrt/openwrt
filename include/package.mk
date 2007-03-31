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
include $(INCLUDE_DIR)/package-defaults.mk
include $(INCLUDE_DIR)/package-dumpinfo.mk
include $(INCLUDE_DIR)/package-ipkg.mk

STAMP_PREPARED:=$(PKG_BUILD_DIR)/.prepared
STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/.configured
STAMP_BUILT:=$(PKG_BUILD_DIR)/.built
export CONFIG_SITE:=$(INCLUDE_DIR)/site/$(REAL_GNU_TARGET_NAME)

define Build/DefaultTargets
  ifneq ($(strip $(PKG_SOURCE_URL)),)
    download: $(DL_DIR)/$(PKG_SOURCE)

    $(DL_DIR)/$(PKG_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_SOURCE)" "$(PKG_MD5SUM)" $(PKG_SOURCE_URL)

    $(STAMP_PREPARED): $(DL_DIR)/$(PKG_SOURCE)
  endif

  ifeq ($(DUMP),)
    ifeq ($(CONFIG_AUTOREBUILD),y)
      ifneq ($$(shell $(SCRIPT_DIR)/timestamp.pl -p $(PKG_BUILD_DIR) . $(PKG_FILE_DEPEND)),$(PKG_BUILD_DIR))
        $$(info Forcing package rebuild)
        $(STAMP_PREPARED): clean
      endif
    endif
  endif

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

  ifdef Build/InstallDev
    ifneq ($$(shell $(SCRIPT_DIR)/timestamp.pl -p -x ipkg -x ipkg-install $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed $(PKG_BUILD_DIR)),$(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed)
      $(STAMP_BUILT): package-rebuild
    endif

    compile: $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed
    $(STAGING_DIR)/stampfiles/.$(PKG_NAME)-installed: $(STAMP_BUILT)
	mkdir -p $(STAGING_DIR)/stampfiles
	$(Build/InstallDev)
	touch $$@
  endif

  package-rebuild: FORCE
	@-rm -f $(STAMP_BUILT)

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
