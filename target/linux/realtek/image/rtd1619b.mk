define Device/rtd1619b
  FILESYSTEMS := squashfs ext4
  KERNEL := kernel-bin
  DEVICE_NAME := realtek
  DEVICE_VENDOR := Realtek
  DEVICE_DTS_DIR := $(DTS_DIR)/realtek
  DEVICE_DTS := rtd1619b-$(1)
  DEVICE_PACKAGES := nas
  PROFILES := Default $$(DEVICE_NAME)
  SOC := stark
  BOARD_NAME :=
  FLASH_SIZE :=
endef

define Device/bleedingedge-emmc-2gb
  $(Device/rtd1619b)
  DEVICE_MODEL := Bleedingedge eMMC board
  DEVICE_VARIANT := 2GB
  DEVICE_PACKAGES +=
  BOARD_NAME := emmc
  FLASH_SIZE := 8gb
endef

TARGET_DEVICES += bleedingedge-emmc-2gb
