# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/ooredoo_r291-v2
  DEVICE_VENDOR := Ooredoo
  DEVICE_MODEL := R291
  DEVICE_VARIANT := v2
  SOC := asr1802s
  DEVICE_DTS_DIR := $(DTS_DIR)/asr
  DEVICE_DTS := asr1826s-kagu
  KERNEL_LOAD_ADDR := 0x00008000
  KERNEL_INITRAMFS := 1
  SUPPORTED_DEVICES := ooredoo,r291-v2
endef
TARGET_DEVICES += ooredoo_r291-v2

# FIXME: needs U-Boot "mrvlboot" compatible image (bootdelay=1, loadaddr=0x8000)
#        expects: tftp 0x8000 <itb> ; bootm

$(eval $(call BuildImage))
