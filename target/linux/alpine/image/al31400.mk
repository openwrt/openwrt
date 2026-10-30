define Device/checkpoint_l-72
	DEVICE_VENDOR := Check Point
	DEVICE_MODEL := L-72
	DEVICE_DTS := alpine-checkpoint-l72
	KERNEL := kernel-bin | append-dtb | uImage none
	KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
	KERNEL_LOADADDR := 0x8000
	KERNEL_NAME := zImage
	KERNEL_SIZE := 16384k
	BLOCKSIZE := 256k
	PAGESIZE := 4096
	DEVICE_PACKAGES := \
		kmod-gpio-pca953x \
		kmod-gpio-pcf857x \
		kmod-hwmon-nct7802 \
		kmod-i2c-designware-platform \
		kmod-phy-marvell \
		kmod-rtc-ds1307 \
		kmod-sfp \
		kmod-sp805-wdt
endef
TARGET_DEVICES += checkpoint_l-72
