include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR := 0x40008000

define Device/e1600
  DEVICE_TITLE := ZTE ZXHN E1600
  DEVICE_DTS_DIR := $(DTS_DIR)/zte
  DEVICE_DTS := zxhn-e1600
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
  KERNEL_NAME := zImage
  KERNEL_SUFFIX := -uImage
  IMAGES :=
  ARTIFACTS :=
endef
TARGET_DEVICES += e1600

$(eval $(call BuildImage))