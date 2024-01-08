define Device/Default
  PROFILES := Default
  FILESYSTEMS := squashfs ubifs ext4
  DEVICE_DTS_DIR := $(DTS_DIR)/freescale
  KERNEL_INSTALL := 1
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

define Device/imx8m
  DEVICE_VENDOR := NXP
  DEVICE_MODEL := i.MX8M
  DEVICE_DTS := $(basename $(notdir $(wildcard $(DTS_DIR)/freescale/imx8m*.dts)))
endef
TARGET_DEVICES += imx8m
