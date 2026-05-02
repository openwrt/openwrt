define Device/mikrotik_nand
	DEVICE_VENDOR := MikroTik
	KERNEL_NAME := vmlinux
	KERNEL_INITRAMFS := kernel-bin | append-dtb-elf
	KERNEL := kernel-bin | append-dtb-elf | package-kernel-ubifs | \
		ubinize-kernel
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/mikrotik_rb1100ahx4
	$(call Device/mikrotik_nand)
	DEVICE_MODEL := RouterBOARD 1100AHx4
	SOC := al21400
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	KERNEL_UBIFS_OPTS = -m $$(PAGESIZE) -e 124KiB -c $$(PAGESIZE) -x none
	DEVICE_PACKAGES := yafut nand-utils -kmod-swconfig -swconfig
endef
TARGET_DEVICES += mikrotik_rb1100ahx4
