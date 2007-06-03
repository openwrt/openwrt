# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

QUILT?=$(strip $(shell test -f $(PKG_BUILD_DIR)/.quilt_patched && echo y))
ifneq ($(QUILT),)
  STAMP_PREPARED:=$(strip $(STAMP_PREPARED))_q
  CONFIG_AUTOREBUILD=
  PATCHES:=$(shell cd ./patches; ls)
  define Build/Patch/Default
	rm -rf $(PKG_BUILD_DIR)/patches
	mkdir -p $(PKG_BUILD_DIR)/patches
	for patch in $(PATCHES); do ( \
		cp "./patches/$$$$patch" $(PKG_BUILD_DIR); \
		cd $(PKG_BUILD_DIR); \
		quilt import -p 1 "$$$$patch"; \
		quilt push; \
		quilt refresh; \
		rm -f "$$$$patch"; \
	); done
	touch $(PKG_BUILD_DIR)/.quilt_patched
  endef
else
  define Build/Patch/Default
	@if [ -d ./patches -a "$$$$(ls ./patches | wc -l)" -gt 0 ]; then \
		$(PATCH) $(PKG_BUILD_DIR) ./patches; \
	fi
  endef
endif

refresh: $(STAMP_PREPARED)
	@[ -f "$(PKG_BUILD_DIR)/patches/series" ] || { \
		echo "The source directory was not unpacked using quilt. Please rebuild."; \
		false; \
	}
	@[ "$$(cat $(PKG_BUILD_DIR)/patches/series | md5sum)" = "$$(sort $(PKG_BUILD_DIR)/patches/series | md5sum)" ] || { \
		echo "The patches are not sorted in the right order. Please fix."; \
		false; \
	}
	rm -f ./patches/* 2>/dev/null >/dev/null
	@( \
		for patch in $$(cat $(PKG_BUILD_DIR)/patches/series); do \
			$(CP) -v "$(PKG_BUILD_DIR)/patches/$$patch" ./patches; \
		done; \
	)
