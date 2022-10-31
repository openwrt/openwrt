# SPDX-License-Identifier: GPL-2.0-only

define Device/f-secure_sense
  LOADADDR := 0x80000000
  LOADER_PLATFORM := rtl8198c
  LOADER_TYPE := bin
  LZMA_TEXT_START := 0x84000000
  SOC := rtl8198c
  DEVICE_VENDOR := F-Secure
  DEVICE_MODEL := Sense
  IMAGE_SIZE := 32768k
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel
endef

# TARGET_DEVICES += f-secure_sense
