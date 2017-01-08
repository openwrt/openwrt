define Device/mikrotik
  PROFILES := Default
  BOARD_NAME := routerboard
  KERNEL_INITRAMFS :=
  KERNEL_NAME := loader-generic.elf
  KERNEL := kernel-bin | kernel2minor -s 2048 -e -c
  FILESYSTEMS := squashfs
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar
endef

define Device/nand-64m
$(Device/mikrotik)
  KERNEL := kernel-bin | kernel2minor -s 512 -e -c
endef

define Device/nand-large
$(Device/mikrotik)
  KERNEL := kernel-bin | kernel2minor -s 2048 -e -c
endef

TARGET_DEVICES += nand-64m nand-large
