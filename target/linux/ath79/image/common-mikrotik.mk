define Device/mikrotik
	DEVICE_VENDOR := MikroTik
	KERNEL_NAME := vmlinuz
	KERNEL := kernel-bin | append-dtb-elf
	KERNEL_INITRAMFS := kernel-bin | append-dtb-elf
endef

define Device/mikrotik_nor
  $(Device/mikrotik)
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	append-metadata | check-size
endef

define Device/mikrotik_nand
  $(Device/mikrotik)
  IMAGE/sysupgrade.bin = append-kernel | sysupgrade-tar | \
	append-metadata
  DEVICE_PACKAGES := nand-utils
  DEFAULT := n
endef
