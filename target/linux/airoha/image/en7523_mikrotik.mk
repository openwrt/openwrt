#
# AN7523 Mikrotik Profile
#

define Device/MikroTik_nand
	KERNEL_LOADADDR := 0x80208000
	DEVICE_VENDOR := MikroTik
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	KERNEL_UBIFS_OPTS = -m $$(PAGESIZE) -e 124KiB -c $$(PAGESIZE) -x none
	KERNEL_NAME := vmlinux
	KERNEL_INITRAMFS := kernel-bin | append-dtb-elf
	KERNEL := kernel-bin | append-dtb-elf | package-kernel-ubifs | \
		ubinize-kernel
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/mikrotik_e60iugs
	$(Device/MikroTik_nand)
  DEVICE_MODEL := E60iUGS (hEX S 2025)
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb3 kmod-sfp
endef
TARGET_DEVICES += mikrotik_e60iugs
