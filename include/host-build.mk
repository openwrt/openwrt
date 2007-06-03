# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/unpack.mk
include $(INCLUDE_DIR)/depends.mk

STAMP_PREPARED:=$(PKG_BUILD_DIR)/.prepared_$(shell find ${CURDIR} $(PKG_FILE_DEPEND) $(DEP_FINDPARAMS) | md5s)
STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/.configured
STAMP_BUILT:=$(PKG_BUILD_DIR)/.built

include $(INCLUDE_DIR)/quilt.mk

Build/Patch:=$(Build/Patch/Default)
ifneq ($(strip $(PKG_UNPACK)),)
  define Build/Prepare/Default
  	$(PKG_UNPACK)
	$(Build/Patch)
  endef
endif

define Build/Prepare
  $(call Build/Prepare/Default)
endef

define Build/Configure/Default
	@(cd $(PKG_BUILD_DIR)/$(3); \
	[ -x configure ] && \
		$(2) \
		CPPFLAGS="-I$(STAGING_DIR)/host/include" \
		LDFLAGS="-L$(STAGING_DIR)/host/lib" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
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

		
ifneq ($(strip $(PKG_SOURCE)),)
  download: $(DL_DIR)/$(PKG_SOURCE)

  $(DL_DIR)/$(PKG_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl "$(DL_DIR)" "$(PKG_SOURCE)" "$(PKG_MD5SUM)" $(PKG_SOURCE_URL)

  $(STAMP_PREPARED): $(DL_DIR)/$(PKG_SOURCE)
endif

ifneq ($(CONFIG_AUTOREBUILD),)
  define HostBuild/Autoclean
    $(PKG_BUILD_DIR)/.dep_files: $(STAMP_PREPARED)
    $(call rdep,${CURDIR} $(PKG_FILE_DEPEND),$(STAMP_PREPARED))
    $(call rdep,$(PKG_BUILD_DIR),$(STAMP_BUILT),$(PKG_BUILD_DIR)/.dep_files, -and -not -path "/.*" -and -not -path "*/ipkg*")
  endef
endif

define HostBuild
  ifeq ($(DUMP),)
    $(call HostBuild/Autoclean)
  endif
  
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
	@$(NO_TRACE_MAKE) $(PKG_BUILD_DIR)/.dep_files
	touch $$@

  $(STAGING_DIR)/stampfiles/.host_$(PKG_NAME)-installed: $(STAMP_BUILT)
	$(call Build/Install)
	mkdir -p $$(shell dirname $$@)
	touch $$@
	
  ifdef Build/Install
    install: $(STAGING_DIR)/stampfiles/.host_$(PKG_NAME)-installed
  endif

  package-clean: FORCE
	$(call Build/Clean)
	$(call Build/Uninstall)
	rm -f $(STAGING_DIR)/stampfiles/.host_$(PKG_NAME)-installed

  download:
  prepare: $(STAMP_PREPARED)
  configure: $(STAMP_CONFIGURED)
  compile: $(STAMP_BUILT)
  install:
  clean: FORCE
	$(call Build/Clean)
	rm -rf $(PKG_BUILD_DIR)

endef
