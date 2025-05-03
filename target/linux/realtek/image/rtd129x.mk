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


define Device/rtd1296-saola-emmc-2gb
  $(Device/rtd129x)
  DEVICE_MODEL := Saola eMMC board
  DEVICE_VARIANT := 2GB
  BOARD_NAME := emmc
  FLASH_SIZE := 8gb
endef

TARGET_DEVICES += rtd1296-saola-emmc-2gb

define Device/rtd1295-giraffe-emmc-2gb
  $(Device/rtd129x)
  DEVICE_MODEL := Giraffe eMMC board
  DEVICE_VARIANT := 2GB
  BOARD_NAME := emmc
  FLASH_SIZE := 8gb
endef

TARGET_DEVICES += rtd1295-giraffe-emmc-2gb
