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
  KERNEL := kernel-bin | append-dtb
  KERNEL_NAME := zImage
  KERNEL_LOADADDR := 0x00008000
  SUPPORTED_DEVICES := notion,r291
endef
TARGET_DEVICES += notion_r291

# FIXME: needs U-Boot "mrvlboot" compatible image (bootdelay=1, loadaddr=0x8000)
#        expects: tftp 0x8000 <itb> ; bootm

$(eval $(call BuildImage))
