# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/asr1826s_p298
  DEVICE_VENDOR := Notion
  DEVICE_MODEL := P298
  DEVICE_VARIANT := ASR
  SOC := asr1802s
  DEVICE_DTS_DIR := $(DTS_DIR)/asr
  DEVICE_DTS := asr1826s-p298

  # Raw zImage + appended DTB — mrvlboot uses bootz, no uImage/FIT
  KERNEL := kernel-bin | append-dtb
  KERNEL_NAME := zImage
  KERNEL_LOADADDR := 0x00008000
  KERNEL_SIZE := 5120k

  IMAGES := kernel.bin rootfs.bin
  IMAGE/kernel.bin := append-kernel | check-size $$(KERNEL_SIZE)
  IMAGE/rootfs.bin := append-rootfs | pad-rootfs | check-size $$(IMAGE_SIZE)

  IMAGE_SIZE := 20480k
  FILESYSTEMS := squashfs

  SUPPORTED_DEVICES := asr,asr1826s-p298
endef
TARGET_DEVICES += asr1826s_p298

$(eval $(call BuildImage))
