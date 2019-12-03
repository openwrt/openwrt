#
# Copyright (C) 2016-2017 Yousong Zhou <yszhou4tech@gmail.com>
# Copyright (C) 2018 Jianhui Zhao <jianhuizhao329@gmail.com>
# Copyright (C) 2019 Samik Gupta <samik.gupta@broadcom.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DTS_DIR := $(DTS_DIR)/broadcom

define Image/InstallKernel
	$(foreach k,$(filter Image,$(KERNELNAME)), \
		cp $(KDIR)/$(KERNELNAME) $(BIN_DIR)/$(IMG_PREFIX)$(if $(PROFILE_SANITIZED),-$(PROFILE_SANITIZED))-$(k) \
	)
	$$(foreach d,$$(shell ls $$(KDIR)/image-*.dtb),cp $$(d) $$(patsubst $$(KDIR)/image-%,$$(BIN_DIR)/$$(IMG_PREFIX)$$(if $$(PROFILE_SANITIZED),-$$(PROFILE_SANITIZED))-%,$$(d));)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) bs=128k conv=sync | \
		gzip -9n >$(BIN_DIR)/$(IMG_PREFIX)$(if $(PROFILE_SANITIZED),-$(PROFILE_SANITIZED))-rootfs.$(1).gz
endef

ifeq ($(SUBTARGET),arm64)
define Device/broadcom_ps225-hxx
	DEVICE_VENDOR := Broadcom
	DEVICE_MODEL := PS225-HXX
	DEVICE_DTS_DIR := $(DTS_DIR)/stingray
	DEVICE_DTS := bcm958802a802x
endef
TARGET_DEVICES += broadcom_ps225-hxx
endif

$(eval $(call BuildImage))
