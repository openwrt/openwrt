# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifneq ($(__quilt_inc),1)
__quilt_inc:=1

ifeq ($(TARGET_BUILD),1)
  PKG_BUILD_DIR:=$(LINUX_DIR)
endif
PATCH_DIR?=./patches
FILES_DIR?=./files

ifeq ($(MAKECMDGOALS),refresh)
  override QUILT=1
endif

define filter_series
sed -e s,\\\#.*,, $(1) | grep -E \[a-zA-Z0-9\]
endef

define PatchDir/Quilt
	@if [ -s $(1)/series ]; then \
		mkdir -p $(PKG_BUILD_DIR)/patches/$(2); \
		cp $(1)/series $(PKG_BUILD_DIR)/patches/$(2); \
	fi
	@for patch in $$$$( (cd $(1) && if [ -f series ]; then $(call filter_series,series); else ls; fi; ) 2>/dev/null ); do ( \
		cp "$(1)/$$$$patch" $(PKG_BUILD_DIR); \
		cd $(PKG_BUILD_DIR); \
		quilt import -P$(2)$$$$patch -p 1 "$$$$patch"; \
		quilt push -f >/dev/null 2>/dev/null; \
		rm -f "$$$$patch"; \
	); done
	$(if $(2),@echo $(2) >> $(PKG_BUILD_DIR)/patches/.subdirs)
endef

define PatchDir/Default
	@if [ -d "$(1)" -a "$$$$(ls $(1) | wc -l)" -gt 0 ]; then \
		if [ -s "$(1)/series" ]; then \
			$(call filter_series,$(1)/series) | xargs -n1 \
				$(PATCH) $(PKG_BUILD_DIR) "$(1)"; \
		else \
			$(PATCH) $(PKG_BUILD_DIR) "$(1)"; \
		fi; \
	fi
endef

define PatchDir
$(call PatchDir/$(if $(strip $(QUILT)),Quilt,Default),$(strip $(1)),$(strip $(2)))
endef

QUILT?=$(strip $(shell test -f $(PKG_BUILD_DIR)/.quilt_used && echo y))
ifneq ($(QUILT),)
  STAMP_PATCHED:=$(PKG_BUILD_DIR)/.quilt_patched
  STAMP_CHECKED:=$(PKG_BUILD_DIR)/.quilt_checked
  override CONFIG_AUTOREBUILD=
  prepare: $(STAMP_PATCHED)
  quilt-check: $(STAMP_CHECKED)
endif

define Build/Patch/Default
	$(if $(QUILT),rm -rf $(PKG_BUILD_DIR)/patches; mkdir -p $(PKG_BUILD_DIR)/patches)
	$(call PatchDir,$(PATCH_DIR),)
endef

define Kernel/Patch/Default
	$(if $(QUILT),rm -rf $(PKG_BUILD_DIR)/patches; mkdir -p $(PKG_BUILD_DIR)/patches)
	if [ -d $(GENERIC_FILES_DIR) ]; then $(CP) $(GENERIC_FILES_DIR)/* $(LINUX_DIR)/; fi
	if [ -d $(FILES_DIR) ]; then \
		$(CP) $(FILES_DIR)/* $(LINUX_DIR)/; \
		find $(LINUX_DIR)/ -name \*.rej | xargs rm -f; \
	fi
	$(call PatchDir,$(GENERIC_PATCH_DIR),generic/)
	$(call PatchDir,$(PATCH_DIR),platform/)
endef

define Quilt/RefreshDir
	mkdir -p $(1)
	-rm -f $(1)/* 2>/dev/null >/dev/null
	@( \
		for patch in $$$$($(if $(2),grep "^$(2)",cat) $(PKG_BUILD_DIR)/patches/series | awk '{print $$$$1}'); do \
			$(CP) -v "$(PKG_BUILD_DIR)/patches/$$$$patch" $(1); \
		done; \
	)
endef

define Quilt/Refresh/Package
	$(call Quilt/RefreshDir,$(PATCH_DIR))
endef

define Quilt/Refresh/Kernel
	@[ -z "$$(grep -v '^generic/' $(PKG_BUILD_DIR)/patches/series | grep -v '^platform/')" ] || { \
		echo "All kernel patches must start with either generic/ or platform/"; \
		false; \
	}
	$(call Quilt/RefreshDir,$(GENERIC_PATCH_DIR),generic/)
	$(call Quilt/RefreshDir,$(PATCH_DIR),platform/)
endef

define Quilt/Refresh
$(if $(TARGET_BUILD),$(Quilt/Refresh/Kernel),$(Quilt/Refresh/Package))
endef

define Build/Quilt
  $(STAMP_PATCHED): $(STAMP_PREPARED)
	@( \
		cd $(PKG_BUILD_DIR)/patches; \
		quilt pop -a -f >/dev/null 2>/dev/null; \
		if [ -s ".subdirs" ]; then \
			rm -f series; \
			for file in $$$$(cat .subdirs); do \
				if [ -f $$$$file/series ]; then \
					echo "Converting $$file/series"; \
					$$(call filter_series,$$$$file/series) | awk -v file="$$$$file/" '$$$$0 !~ /^#/ { print file $$$$0 }' | sed -e s,//,/,g >> series; \
				else \
					echo "Sorting patches in $$$$file"; \
					find $$$$file/* -type f \! -name series | sed -e s,//,/,g | sort >> series; \
				fi; \
			done; \
		else \
			find * -type f \! -name series | sort > series; \
		fi; \
	)
	touch $$@

  $(STAMP_CONFIGURED): $(STAMP_CHECKED) FORCE
  $(STAMP_CHECKED): $(STAMP_PATCHED)
	if [ -s "$(PKG_BUILD_DIR)/patches/series" ]; then (cd $(PKG_BUILD_DIR); quilt next >/dev/null 2>&1 && quilt push -a || quilt top >/dev/null 2>&1); fi
	touch $$@

  quilt-check: $(STAMP_PREPARED) FORCE
	@[ -f "$(PKG_BUILD_DIR)/.quilt_used" ] || { \
		echo "The source directory was not unpacked using quilt. Please rebuild with QUILT=1"; \
		false; \
	}
	@[ -f "$(PKG_BUILD_DIR)/patches/series" ] || { \
		echo "The source directory contains no quilt patches."; \
		false; \
	}
	@[ -n "$$$$(ls $(PKG_BUILD_DIR)/patches/series)" -o "$$$$(cat $(PKG_BUILD_DIR)/patches/series | md5sum)" = "$$(sort $(PKG_BUILD_DIR)/patches/series | md5sum)" ] || { \
		echo "The patches are not sorted in the right order. Please fix."; \
		false; \
	}

  refresh: quilt-check
	@cd $(PKG_BUILD_DIR); quilt pop -a -f >/dev/null 2>/dev/null
	@cd $(PKG_BUILD_DIR); while quilt next 2>/dev/null >/dev/null && quilt push; do \
		quilt refresh -p ab --no-index --quiltrc=/dev/null --no-timestamps; \
	done; ! quilt next 2>/dev/null >/dev/null
	$(Quilt/Refresh)
	
  update: quilt-check
	$(Quilt/Refresh)
endef

endif
