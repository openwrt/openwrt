define Target/Description
	Build firmware images for Airoha AN7563 / AN7552 (ARMv8 Cortex-A53
	running in AArch32 mode) based boards.
endef

# TP-Link XX530v V2 (hw id "XX530vV2", vendor platform "JEDI").
#
# The stock Econet U-Boot 2014.04 is kept and its default bootcmd is
#
#     bootcmd=flash tpimg;bootm 81800200
#
# so the kernel must sit at MTD_OFS_KERNEL (raw flash 0x620000) behind a
# 512-byte TP-Link image tag.  It reads nothing else: no partition names, no
# UBI.  The rootfs therefore goes into a separate UBI partition starting at
# the erase block right after the kernel, and nand_upgrade_tar writes the
# tagged kernel with "mtd write - kernel" exactly as on other TP-Link boards
# with a stock bootloader (see ramips mt7621_tplink_ex220-v1-nand).
define Device/tplink_xx530v-v2
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := XX530v
  DEVICE_VARIANT := v2
  DEVICE_DTS := an7563-tplink_xx530v-v2
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7916-firmware wpad-basic-mbedtls \
	kmod-usb3 kmod-usb-xhci-mtk kmod-usb-storage kmod-fs-vfat \
	i2c-tools ethtool tcpdump airoha-an7552-npu-firmware nand-utils

  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_LOADADDR := 0x80088000
  KERNEL_SIZE := 4096k

  # Shipped kernel: FIT wrapped in the TP-Link image tag, because the
  # bootloader reads it from flash without any help from Linux.
  KERNEL := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	tplink-econet-header $$(KERNEL_LOADADDR)

  # Recovery kernel: loaded into RAM from the U-Boot prompt, so it needs no
  # tag and must not collide with the decompressed kernel at 0x80088000.
  # "tftpboot 0x84000000 <image>; bootm 0x84000000".
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 64k

  # sysupgrade.bin is a tar with "kernel" (the tagged FIT) and "root" (the
  # ubinized image).  nand_upgrade_tar puts the first on the "kernel" mtd
  # partition and the second into the UBI volumes of the "ubi" one.
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata

endef
TARGET_DEVICES += tplink_xx530v-v2
