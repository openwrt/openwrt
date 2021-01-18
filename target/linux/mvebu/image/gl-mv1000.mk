ifeq ($(SUBTARGET),cortexa53)

define Device/gl-mv1000
  KERNEL_NAME := Image
  KERNEL_LOADADDR := 0x000080000
  KERNEL := kernel-bin | lzma | uImage lzma | pad-dtb | append-gl-metadata
  DEVICE_TITLE := GL.iNet GL-MV1000
  DEVICE_PACKAGES := e2fsprogs ethtool mkf2fs kmod-fs-vfat kmod-usb2 kmod-usb3 kmod-usb-storage
  BLOCKSIZE := 64k
  IMAGES := sysupgrade.bin
  IMAGE_SIZE := 15000k
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
  DEVICE_DTS := armada-gl-mv1000
  DTS_DIR := $(DTS_DIR)/marvell
  SUPPORTED_DEVICES := gl-mv1000
endef
TARGET_DEVICES += gl-mv1000

define Device/gl-mv1000-emmc
  $(call Device/Default-arm64-emmc)
  DEVICE_TITLE := GL.iNet GL-MV1000 EMMC
  DEVICE_DTS := armada-gl-mv1000-emmc
  SUPPORTED_DEVICES := gl-mv1000-emmc
endef

TARGET_DEVICES += gl-mv1000-emmc

endif

