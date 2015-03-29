#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifeq ($(DUMP),)
  define BuildTarget/bin
    ifeq ($(if $(VARIANT),$(BUILD_VARIANT)),$(VARIANT))
    ifdef Package/$(1)/install
      ifneq ($(CONFIG_PACKAGE_$(1))$(DEVELOPER),)
        compile: install-bin-$(1)
      else
        compile: $(1)-disabled
        $(1)-disabled:
		@echo "WARNING: skipping $(1) -- package not selected" >&2
      endif
    endif
    endif

    install-bin-$(1): $(STAMP_BUILT)
	  rm -rf $(BIN_DIR)/$(1)
	  $(INSTALL_DIR) $(BIN_DIR)/$(1)
	  $(call Package/$(1)/install,$(BIN_DIR)/$(1))

    clean-$(1):
	  rm -rf $(BIN_DIR)/$(1)

    clean: clean-$(1)
    .PHONY: install-bin-$(1)
  endef
endif
