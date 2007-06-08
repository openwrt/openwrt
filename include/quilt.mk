# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifeq ($(KERNEL_BUILD),1)
  PKG_BUILD_DIR:=$(LINUX_DIR)
endif

define Quilt/Patch
	@for patch in $$$$( (cd $(1) && ls) 2>/dev/null ); do ( \
		cp "$(1)/$$$$patch" $(PKG_BUILD_DIR); \
		cd $(PKG_BUILD_DIR); \
		quilt import -P$(2)$$$$patch -p 1 "$$$$patch"; \
		quilt push -f >/dev/null 2>/dev/null; \
		rm -f "$$$$patch"; \
	); done
endef

QUILT?=$(strip $(shell test -f $(PKG_BUILD_DIR)/.quilt_used && echo y))
ifneq ($(QUILT),)
  STAMP_PREPARED:=$(strip $(STAMP_PREPARED))_q
  STAMP_PATCHED:=$(PKG_BUILD_DIR)/.quilt_patched
  CONFIG_AUTOREBUILD=
  PATCHES:=$(shell )
  define Build/Patch/Default
	rm -rf $(PKG_BUILD_DIR)/patches
	mkdir -p $(PKG_BUILD_DIR)/patches
	$(call Quilt/Patch,./patches,)
	@echo
	touch $(PKG_BUILD_DIR)/.quilt_used
  endef
  $(STAMP_CONFIGURED): $(STAMP_PATCHED) FORCE
  prepare: $(STAMP_PATCHED)
  quilt-check: $(STAMP_PATCHED)
else
  define Build/Patch/Default
	@if [ -d ./patches -a "$$$$(ls ./patches | wc -l)" -gt 0 ]; then \
		$(PATCH) $(PKG_BUILD_DIR) ./patches; \
	fi
  endef
endif

define Kernel/Patch/Default
	if [ -d $(GENERIC_PLATFORM_DIR)/files ]; then $(CP) $(GENERIC_PLATFORM_DIR)/files/* $(LINUX_DIR)/; fi
	if [ -d ./files ]; then $(CP) ./files/* $(LINUX_DIR)/; fi
	$(if $(strip $(QUILT)),$(call Quilt/Patch,$(GENERIC_PLATFORM_DIR)/patches,generic/), \
		if [ -d $(GENERIC_PLATFORM_DIR)/patches ]; then $(PATCH) $(LINUX_DIR) $(GENERIC_PLATFORM_DIR)/patches; fi \
	)
	$(if $(strip $(QUILT)),$(call Quilt/Patch,./patches,platform/), \
		if [ -d ./patches ]; then $(PATCH) $(LINUX_DIR) ./patches; fi \
	)
	$(if $(strip $(QUILT)),touch $(PKG_BUILD_DIR)/.quilt_used)
endef

$(STAMP_PATCHED): $(STAMP_PREPARED)
	@cd $(PKG_BUILD_DIR); quilt pop -a -f >/dev/null 2>/dev/null || true
	[ -f "$(PKG_BUILD_DIR)/patches/series" ] && cd $(PKG_BUILD_DIR); quilt push -a
	touch $@

define Quilt/RefreshDir
	mkdir -p $(1)
	-rm -f $(1)/* 2>/dev/null >/dev/null
	@( \
		for patch in $$($(if $(2),grep "^$(2)",cat) $(PKG_BUILD_DIR)/patches/series | awk '{print $$1}'); do \
			$(CP) -v "$(PKG_BUILD_DIR)/patches/$$patch" $(1); \
		done; \
	)
endef

define Quilt/Refresh/Package
	$(call Quilt/RefreshDir,./patches)
endef

define Quilt/Refresh/Kernel
	@[ -z "$$(grep -v '^generic/' $(PKG_BUILD_DIR)/patches/series | grep -v '^platform/')" ] || { \
		echo "All kernel patches must start with either generic/ or platform/"; \
		false; \
	}
	$(call Quilt/RefreshDir,$(GENERIC_PLATFORM_DIR)/patches,generic/)
	$(call Quilt/RefreshDir,./patches,platform/)
endef

quilt-check: $(STAMP_PREPARED) FORCE
	@[ -f "$(PKG_BUILD_DIR)/.quilt_used" ] || { \
		echo "The source directory was not unpacked using quilt. Please rebuild with QUILT=1"; \
		false; \
	}
	@[ -f "$(PKG_BUILD_DIR)/patches/series" ] || { \
		echo "The source directory contains no quilt patches."; \
		false; \
	}
	@[ "$$(cat $(PKG_BUILD_DIR)/patches/series | md5sum)" = "$$(sort $(PKG_BUILD_DIR)/patches/series | md5sum)" ] || { \
		echo "The patches are not sorted in the right order. Please fix."; \
		false; \
	}

refresh: quilt-check
	@cd $(PKG_BUILD_DIR); quilt pop -a -f >/dev/null 2>/dev/null
	@cd $(PKG_BUILD_DIR); while quilt next 2>/dev/null >/dev/null && quilt push; do \
		quilt refresh; \
	done; ! quilt next 2>/dev/null >/dev/null
	$(if $(KERNEL_BUILD),$(Quilt/Refresh/Kernel),$(Quilt/Refresh/Package))
	
update: quilt-check
	$(if $(KERNEL_BUILD),$(Quilt/Refresh/Kernel),$(Quilt/Refresh/Package))

