define Device/rtd129x
  FILESYSTEMS := squashfs ext4
  KERNEL := kernel-bin
  DEVICE_NAME := realtek
  DEVICE_VENDOR := Realtek
  DEVICE_DTS_DIR := $(DTS_DIR)/realtek
  DEVICE_DTS := $(1)
  DEVICE_PACKAGES := nas
  PROFILES := Default $$(DEVICE_NAME)
  SOC := kylin
  BOARD_NAME :=
  FLASH_SIZE :=
endef
