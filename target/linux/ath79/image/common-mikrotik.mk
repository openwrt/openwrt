define Device/mikrotik
	DEVICE_VENDOR := MikroTik
	LOADER_TYPE := elf
	KERNEL_NAME := vmlinuz
	KERNEL := kernel-bin | append-dtb-elf
	KERNEL_INITRAMFS_NAME := vmlinux-initramfs
	KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel
endef

define Device/mikrotik_nor
  $(Device/mikrotik)
  DEVICE_PACKAGES := -yafut
  IMAGE/sysupgrade.bin := append-kernel | kernel2minor -s 1024 -e | \
	pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | \
	check-size | append-metadata
endef

define Device/mikrotik_nand
  $(Device/mikrotik)
  IMAGE/sysupgrade.bin = append-kernel | sysupgrade-tar | append-metadata
endef
