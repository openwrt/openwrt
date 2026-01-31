# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += DTB_SIZE

define Build/pad-dtb
	$(call Image/BuildDTB,$(DTS_DIR)/$(DEVICE_DTS).dts,$(dir $@)/image-$(DEVICE_DTS).dtb,,--space $(DTB_SIZE))
endef

define Device/Default
  PROFILES := Default
  DEVICE_DTS := $(lastword $(subst _, ,$(1)))
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL_ENTRY := 0x00000000
  KERNEL_LOADADDR := 0x00000000
  KERNEL := kernel-bin
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
