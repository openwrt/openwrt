#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/copy-file
	cat "$(1)" > "$@"
endef

define Build/sysupgrade-dtb-ramdisk-tar
	$(eval dtb_file=$(word 1,$(1)))
	$(eval ramdisk_file=$(word 2,$(1)))
	sh $(TOPDIR)/scripts/sysupgrade-tar.sh \
		--board $(if $(BOARD_NAME),$(BOARD_NAME),$(DEVICE_NAME)) \
		--kernel $(call param_get_default,kernel,$(1),$(IMAGE_KERNEL)) \
		--rootfs $(call param_get_default,rootfs,$(1),$(IMAGE_ROOTFS)) \
		--dtb $(call param_get_default,dtb,$(1),$(dtb_file)) \
		--ramdisk $(call param_get_default,ramdisk,$(1),$(ramdisk_file)) \
		$@
endef

define Device/Default
  PROFILES := Default
  DEVICE_DTS := $(lastword $(subst _, ,$(1)))
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL_ENTRY := 0x00000000
  KERNEL_LOADADDR := 0x00000000
  KERNEL := kernel-bin
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
