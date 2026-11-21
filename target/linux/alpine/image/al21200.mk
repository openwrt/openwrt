define Device/checkpoint_l-71
	DEVICE_VENDOR := Check Point
	DEVICE_MODEL := L-71
	DEVICE_DTS := alpine-checkpoint-l71
	KERNEL := kernel-bin | append-dtb | uImage none
	KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
	KERNEL_LOADADDR := 0x8000
	KERNEL_NAME := zImage
	KERNEL_SIZE := 16384k
	BLOCKSIZE := 256k
	PAGESIZE := 4096
	DEVICE_PACKAGES := \
		kmod-dsa-mv88e6xxx \
		kmod-gpio-pca953x \
		kmod-hwmon-nct7802 \
		kmod-i2c-designware-platform \
		kmod-phy-marvell \
		kmod-rtc-ds1307 \
		kmod-usb3 \
		kmod-usb-storage \
		kmod-usb-xhci-pci-renesas
endef
TARGET_DEVICES += checkpoint_l-71

define Device/checkpoint_l-71w
	DEVICE_VENDOR := Check Point
	DEVICE_MODEL := L-71W
	DEVICE_DTS := alpine-checkpoint-l71w
	KERNEL := kernel-bin | append-dtb | uImage none
	KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
	KERNEL_LOADADDR := 0x8000
	KERNEL_NAME := zImage
	KERNEL_SIZE := 16384k
	BLOCKSIZE := 256k
	PAGESIZE := 4096
	DEVICE_PACKAGES := \
		ath10k-firmware-qca988x \
		kmod-ath10k \
		kmod-dsa-mv88e6xxx \
		kmod-gpio-pca953x \
		kmod-hwmon-nct7802 \
		kmod-i2c-designware-platform \
		kmod-phy-marvell \
		kmod-rtc-ds1307 \
		kmod-usb3 \
		kmod-usb-storage \
		kmod-usb-xhci-pci-renesas \
		wpad-basic-mbedtls
endef
TARGET_DEVICES += checkpoint_l-71w
