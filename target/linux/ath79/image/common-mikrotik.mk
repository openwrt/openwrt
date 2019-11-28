define Device/mikrotik
	DEVICE_VENDOR := MikroTik
	DEVICE_PACKAGES := rbextract rbcfg
	BOARD_NAME := routerboard
	LOADER_TYPE := elf
	KERNEL := kernel-bin | append-dtb | lzma | loader-kernel
	KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel
endef
