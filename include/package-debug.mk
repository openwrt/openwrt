# 
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifeq ($(DUMP),)
  define BuildTarget/debug

    DEBUG_STAMP_$(1) := $(DEBUG_DIR)/stamp/$(1)

    ifdef Package/$(1)/install
      ifneq ($(CONFIG_PACKAGE_$(1))$(SDK)$(DEVELOPER),)
        compile: $$(DEBUG_STAMP_$(1))
      endif
    endif

    $$(DEBUG_STAMP_$(1)): $(PKG_BUILD_DIR)/.built
		mkdir -p $(DEBUG_DIR)/stamp
		$(call Package/$(1)/install,$(DEBUG_DIR))
		touch $$(DEBUG_STAMP_$(1))


  endef
endif
