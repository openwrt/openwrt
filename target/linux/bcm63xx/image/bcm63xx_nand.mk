#
# BCM63XX NAND Profiles
#

DEVICE_VARS += CFE_RAM_FILE
DEVICE_VARS += CFE_RAM_JFFS2_NAME CFE_RAM_JFFS2_PAD
DEVICE_VARS += CFE_WFI_CHIP_ID CFE_WFI_FLASH_TYPE
DEVICE_VARS += CFE_WFI_FLAGS CFE_WFI_VERSION

# CFE expects a single JFFS2 partition with cferam and kernel. However,
# it's possible to fool CFE into properly loading both cferam and kernel
# from two different JFFS2 partitions by adding dummy files (see
# cfe-jffs2-cferam and cfe-jffs2-kernel).
# Separate JFFS2 partitions allow upgrading openwrt without reflashing cferam
# JFFS2 partition, which is much safer in case anything goes wrong.
define Device/bcm63xx-nand
  FILESYSTEMS := squashfs ubifs
  KERNEL := kernel-bin | append-dtb | relocate-kernel | lzma | cfe-jffs2-kernel
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-lzma elf
  IMAGES := cfe.bin sysupgrade.bin
  IMAGE/cfe.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | cfe-jffs2-cferam | append-ubi | cfe-wfi-tag
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  KERNEL_SIZE := 5120k
  CFE_CHIP_ID :=
  CFE_RAM_FILE :=
  CFE_RAM_JFFS2_NAME :=
  CFE_RAM_JFFS2_PAD :=
  CFE_WFI_VERSION := 0x5731
  CFE_WFI_CHIP_ID = 0x$$(CFE_CHIP_ID)
  CFE_WFI_FLASH_TYPE :=
  CFE_WFI_FLAGS :=
  UBINIZE_OPTS := -E 5
  DEVICE_PACKAGES += nand-utils
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
endef

### Comtrend ###
define Device/comtrend_vr-3032u
  $(Device/bcm63xx-nand)
  DEVICE_VENDOR := Comtrend
  DEVICE_MODEL := VR-3032u
  CFE_CHIP_ID := 63268
  SOC := bcm63168
  CFE_RAM_FILE := comtrend,vr-3032u/cferam.000
  CFE_RAM_JFFS2_NAME := cferam.000
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  DEVICE_PACKAGES += $(USB2_PACKAGES)
  CFE_WFI_FLASH_TYPE := 3
endef
TARGET_DEVICES += comtrend_vr-3032u
