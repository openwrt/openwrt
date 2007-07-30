# debug flags:
#
# d: show subdirectory tree
# t: show added targets
# l: show legacy targets

ifeq ($(DEBUG),all)
  build_debug:=dlt
else
  build_debug:=$(DEBUG)
endif

define debug
$$(findstring $(2),$$(if $$(DEBUG_DIR),$$(if $$(filter $$(DEBUG_DIR)%,$(1)),$(build_debug)),$(build_debug)))
endef

define warn
$$(if $(call debug,$(1),$(2)),$$(warning $(3)))
endef

define warn_eval
$(call warn,$(1),$(2),$(3)	$(4))
$(4)
endef

SUBTARGETS:=clean download prepare compile install update refresh prereq

define subtarget
  $(call warn_eval,$(1),t,T,$(1)/$(2): $($(1)/) $(foreach bd,$(if $($(1)/builddirs-$(2)),$($(1)/builddirs-$(2)),$($(1)/builddirs)),$(1)/$(bd)/$(2)))

endef

# Parameters: <subdir>
define subdir
  $(call warn,$(1),d,D $(1))
  $(foreach bd,$($(1)/builddirs),
    $(call warn,$(1),d,BD $(1)/$(bd))
    $(foreach target,$(SUBTARGETS),
      $(call warn_eval,$(1)/$(bd),t,T,$(1)/$(bd)/$(target): $($(1)/$(bd)/$(target)) $(call $(1)//$(target),$(1)/$(bd)))
		@+$$(MAKE) -j1 -C $(1)/$(bd) $(target)

      # legacy targets
      $(call warn_eval,$(1)/$(bd),l,T,$(1)/$(bd)-$(target): $(1)/$(bd)/$(target))
	)
  )
  $(foreach target,$(SUBTARGETS),$(call subtarget,$(1),$(target)))
endef

# Parameters: <subdir> <name>
define stampfile
  $(1)/stamp:=$(STAGING_DIR)/stampfiles/.$(2)_installed
  $(call rdep,$(1),$$($(1)/stamp))

  $$($(1)/stamp):
	@+$(MAKE) $(1)/install
	@mkdir -p $$$$(dirname $$($(1)/stamp))
	@touch $$($(1)/stamp)
  .PRECIOUS: $$($(1)/stamp) # work around a make bug

  $(1)//clean:=$(1)/stamp/clean
  $(1)/stamp/clean: FORCE
	@rm -f $$($(1)/stamp)

endef
