define Device/rtd1319
  FILESYSTEMS := squashfs ext4
  KERNEL := kernel-bin
  DEVICE_NAME := realtek
  DEVICE_VENDOR := Realtek
  DEVICE_DTS_DIR := $(DTS_DIR)/realtek
  DEVICE_DTS := rtd1319-$(1)
  DEVICE_PACKAGES := nas
  PROFILES := Default $$(DEVICE_NAME)
  SOC := hank
  BOARD_NAME :=
  FLASH_SIZE :=
endef

define Device/pymparticles-emmc-2gb
  $(Device/rtd1319)
  DEVICE_MODEL := Pymparticles eMMC board
  DEVICE_VARIANT := 2GB
  DEVICE_PACKAGES +=
  BOARD_NAME := emmc
  FLASH_SIZE := 8gb
endef

TARGET_DEVICES += pymparticles-emmc-2gb
