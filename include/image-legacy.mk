LegacyDevice/Dump = $(Device/Dump)

define LegacyDevice/Check
  _PROFILE_SET = $$(strip $$(foreach profile,$$(PROFILES) DEVICE_$(1),$$(call DEVICE_CHECK_PROFILE,$$(profile))))
  _TARGET_PREPARE := $$(if $$(_PROFILE_SET),legacy-images-prepare,prepare-disabled)
  _TARGET := $$(if $$(_PROFILE_SET),legacy-images,install-disabled)
  $$(if $$(_PROFILE_SET),install: legacy-images-make)
  ifndef IB
    $$(if $$(_PROFILE_SET),mkfs_prepare: legacy-images-prepare-make)
  endif
endef

define LegacyDevice/Build
  $$(_TARGET): legacy-image-$(1)
  $$(_TARGET_PREPARE): legacy-image-prepare-$(1)
  .PHONY: legacy-image-prepare-$(1) legacy-image-$(1)

  legacy-image-prepare-$(1):
	$$(call Image/Prepare/Profile,$(1))

  legacy-image-$(1):
	$$(call Image/BuildKernel/Profile,$(1))
	$(foreach fs,$(TARGET_FILESYSTEMS),
		$$(call Image/Build/Profile,$(1),$(fs))
	)

endef

define LegacyDevice
  $(call Device/InitProfile,$(1))
  $(call Device/Default,$(1))
  $(call LegacyDevice/Default,$(1))
  $(call LegacyDevice/$(1),$(1))
  $(call LegacyDevice/Check,$(1))
  $(call LegacyDevice/$(if $(DUMP),Dump,Build),$(1))

endef


