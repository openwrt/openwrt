# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/notion_r291
  DEVICE_VENDOR := Notion
  DEVICE_MODEL := R291
  DEVICE_VARIANT := ASR
  SOC := asr1802s
  DEVICE_DTS_DIR := $(DTS_DIR)/asr
  DEVICE_DTS := asr1826s-r291

  # Raw zImage + appended DTB — mrvlboot uses bootz, no uImage/FIT
  KERNEL := kernel-bin | append-dtb
  KERNEL_NAME := zImage
  KERNEL_LOADADDR := 0x00008000
  KERNEL_SIZE := 5120k

  IMAGES := kernel.bin rootfs.bin
  IMAGE/kernel.bin := append-kernel | check-size $$(KERNEL_SIZE)
  IMAGE/rootfs.bin := append-rootfs | check-size $$(IMAGE_SIZE)

  IMAGE_SIZE := 20480k
  FILESYSTEMS := squashfs

  SUPPORTED_DEVICES := notion,r291
endef
TARGET_DEVICES += notion_r291

$(eval $(call BuildImage))
