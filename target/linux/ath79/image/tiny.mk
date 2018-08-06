DEVICE_VARS += ROOTFS_SIZE

define Device/buffalo_bhr-4grv2
  ATH_SOC := qca9558
  DEVICE_TITLE := Buffalo BHR-4GRV2
  BOARDNAME := BHR-4GRV2
  ROOTFS_SIZE := 14528k
  KERNEL_SIZE := 1472k
  IMAGE_SIZE := 16000k
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := \
    append-rootfs | pad-rootfs | pad-to $$$$(ROOTFS_SIZE) | \
    append-kernel | append-metadata | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.bin := append-kernel | \
    pad-to $$$$(KERNEL_SIZE) | append-rootfs | pad-rootfs | mkbuffaloimg
  SUPPORTED_DEVICES += bhr-4grv2
endef
TARGET_DEVICES += buffalo_bhr-4grv2
