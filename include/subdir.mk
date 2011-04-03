# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

SUBTARGETS:=clean download prepare compile install update refresh prereq dist distcheck configure

subtarget-default = $(filter-out ., \
	$(if $($(1)/builddirs-$(2)),$($(1)/builddirs-$(2)), \
	$(if $($(1)/builddirs-default),$($(1)/builddirs-default), \
	$($(1)/builddirs))))

define subtarget
  $(call warn_eval,$(1),t,T,$(1)/$(2): $($(1)/) $(foreach bd,$(call subtarget-default,$(1),$(2)),$(1)/$(bd)/$(2)))

endef


lastdir=$(word $(words $(subst /, ,$(1))),$(subst /, ,$(1)))
diralias=$(if $(findstring $(1),$(call lastdir,$(1))),,$(call lastdir,$(1)))

# Parameters: <subdir>
define subdir
  $(call warn,$(1),d,D $(1))
  $(foreach bd,$($(1)/builddirs),
    $(call warn,$(1),d,BD $(1)/$(bd))
    $(foreach target,$(SUBTARGETS),
      $(foreach btype,$(buildtypes-$(bd)),
        $(call warn_eval,$(1)/$(bd),t,T,$(1)/$(bd)/$(btype)/$(target): $(if $(QUILT),,$($(1)/$(bd)/$(btype)/$(target)) $(call $(1)//$(btype)/$(target),$(1)/$(bd)/$(btype))))
		  $(if $(call debug,$(1)/$(bd),v),,@)+$$(SUBMAKE) -C $(1)/$(bd) $(btype)-$(target) $(if $(findstring $(bd),$($(1)/builddirs-ignore-$(btype)-$(target))), || $(call MESSAGE,   ERROR: $(1)/$(bd) [$(btype)] failed to build.))
        $(if $(call diralias,$(bd)),$(call warn_eval,$(1)/$(bd),l,T,$(1)/$(call diralias,$(bd))/$(btype)/$(target): $(1)/$(bd)/$(btype)/$(target)))
      )
      $(call warn_eval,$(1)/$(bd),t,T,$(1)/$(bd)/$(target): $(if $(QUILT),,$($(1)/$(bd)/$(target)) $(call $(1)//$(target),$(1)/$(bd))))
	  	$(if $(BUILD_LOG),@mkdir -p $(BUILD_LOG_DIR)/$(1)/$(bd))
        $(foreach variant,$(if $(BUILD_VARIANT),$(BUILD_VARIANT),$(if $(strip $($(1)/$(bd)/variants)),$($(1)/$(bd)/variants),$(if $($(1)/$(bd)/default-variant),$($(1)/$(bd)/default-variant),__default))),
			$(if $(call debug,$(1)/$(bd),v),,@)+$(if $(BUILD_LOG),set -o pipefail;) $$(SUBMAKE) -C $(1)/$(bd) $(target) BUILD_VARIANT="$(filter-out __default,$(variant))" $(if $(BUILD_LOG),SILENT= 2>&1 | tee $(BUILD_LOG_DIR)/$(1)/$(bd)/$(target).txt) $(if $(findstring $(bd),$($(1)/builddirs-ignore-$(target))), || $(call MESSAGE,   ERROR: $(1)/$(bd) failed to build$(if $(filter-out __default,$(variant)), (build variant: $(variant))).))
        )
      $(if $(DUMP_TARGET_DB),,
        # legacy targets
        $(call warn_eval,$(1)/$(bd),l,T,$(1)/$(bd)-$(target): $(1)/$(bd)/$(target))
        # aliases
        $(if $(call diralias,$(bd)),$(call warn_eval,$(1)/$(bd),l,T,$(1)/$(call diralias,$(bd))/$(target): $(1)/$(bd)/$(target)))
	  )
	)
  )
  $(foreach target,$(SUBTARGETS),$(call subtarget,$(1),$(target)))
endef

ifndef DUMP_TARGET_DB
# Parameters: <subdir> <name> <target> <depends> <config options> <stampfile location>
define stampfile
  $(1)/stamp-$(3):=$(if $(6),$(6),$(STAGING_DIR))/stamp/.$(2)_$(3)$(if $(5),_$(call confvar,$(5)))
  $$($(1)/stamp-$(3)): $(TMP_DIR)/.build $(4)
	@+$(SCRIPT_DIR)/timestamp.pl -n $$($(1)/stamp-$(3)) $(1) $(4) || \
		$(MAKE) $$($(1)/flags-$(3)) $(1)/$(3)
	@mkdir -p $$$$(dirname $$($(1)/stamp-$(3)))
	@touch $$($(1)/stamp-$(3))

  $$(if $(call debug,$(1),v),,.SILENT: $$($(1)/stamp-$(3)))

  .PRECIOUS: $$($(1)/stamp-$(3)) # work around a make bug

  $(1)//clean:=$(1)/stamp-$(3)/clean
  $(1)/stamp-$(3)/clean: FORCE
	@rm -f $$($(1)/stamp-$(3))

endef
endif
