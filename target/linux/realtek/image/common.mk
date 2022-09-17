# SPDX-License-Identifier: GPL-2.0-only

define Build/d-link_dgs-1210
  IMAGE_SIZE := 13824k
  DEVICE_VENDOR := D-Link
  DLINK_KERNEL_PART_SIZE := 1572864
  KERNEL := kernel-bin | append-dtb | gzip | uImage gzip | dlink-cameo
  CAMEO_KERNEL_PART := 2
  CAMEO_ROOTFS_PART := 3
  CAMEO_CUSTOMER_SIGNATURE := 2
  CAMEO_BOARD_VERSION := 32
  IMAGES += factory_image1.bin
  IMAGE/factory_image1.bin := append-kernel | pad-to 64k | \
        append-rootfs | pad-rootfs | pad-to 16 | check-size | \
        dlink-version | dlink-headers
endef
