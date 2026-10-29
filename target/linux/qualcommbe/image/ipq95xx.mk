DTS_DIR := $(DTS_DIR)/qcom

define Device/8devices_kiwi-dvk
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := 8devices
	DEVICE_MODEL := Kiwi-DVK
	DEVICE_DTS_CONFIG := config@8dev-kiwi
	SOC := ipq9570
	DEVICE_PACKAGES := kmod-ath12k ath12k-firmware-qcn9274 \
		ipq-wifi-8devices_kiwi f2fsck mkf2fs kmod-sfp \
		kmod-phy-maxlinear kmod-phy-realtek rtl826x-firmware
	IMAGE/factory.bin := qsdk-ipq-factory-nor
endef
TARGET_DEVICES += 8devices_kiwi-dvk

define Device/askey_sbe1v1k
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := Askey
	DEVICE_MODEL := SBE1V1K
	DEVICE_ALT0_VENDOR := Askey
	DEVICE_ALT0_MODEL := RTQ7300T
	DEVICE_ALT1_VENDOR := Spectrum
	DEVICE_ALT1_MODEL := SBE1V1K
	DEVICE_DTS_CONFIG := config@rtq7300t-rev0
	KERNEL_LOADADDR := 0x42200000
	SOC := ipq9570
	DEVICE_PACKAGES := ath12k-firmware-qcn9274 f2fsck ipq-wifi-askey_sbe1v1k kmod-ath12k \
		kmod-hwmon-pwmfan kmod-phy-realtek mkf2fs rtl826x-firmware
endef
TARGET_DEVICES += askey_sbe1v1k

define Device/linksys_ln6001
	$(call Device/FitImage)
	DEVICE_VENDOR := Linksys
	DEVICE_MODEL := LN6001
	DEVICE_DTS := ipq9554-linksys-ln6001
	SOC := ipq9554
	DEVICE_PACKAGES += bluetooth-uart kmod-leds-pwm kmod-fs-f2fs mkf2fs f2fsck
	IMAGE/sysupgrade.bin/squashfs := append-rootfs | pad-to 64k | \
		check-size 128m | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += linksys_ln6001

define Device/qcom_rdp433
	$(call Device/FitImageLzma)
	DEVICE_VENDOR := Qualcomm Technologies, Inc.
	DEVICE_MODEL := RDP433
	DEVICE_VARIANT := AP-AL02-C4
	BOARD_NAME := ap-al02.1-c4
	DEVICE_DTS_CONFIG := config@rdp433
	DEVICE_DTS_DIR := $(DTS_DIR)
	SOC := ipq9574
	KERNEL_INSTALL := 1
	KERNEL_SIZE := 6096k
	IMAGE_SIZE := 25344k
	IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += qcom_rdp433

# The installation's first U-Boot env (see Device/ubnt_ucgfiber), 64 KiB with
# CRC as U-Boot reads it from mtd9; the variables are the image's reference env
# with ucg_boot=$(1).
define Build/ucgfiber-uboot-env
	sed 's/^ucg_boot=.*/ucg_boot=$(1)/' \
		$(PLATFORM_SUBDIR)/base-files/usr/share/ucgfiber/uboot-env.txt > $@.txt
	mkenvimage -s 0x10000 -o $@ $@.txt
	rm -f $@.txt
endef

# Fail the build on a truncated or sparse kernel image instead of shipping it.
define Build/ucgfiber-check-kernel
	grep -qa "Linux version" $@ || { \
		echo "ucgfiber: kernel image has no version banner" >&2; exit 1; }
endef

define Device/ubnt_ucgfiber
	$(call Device/FitImageLzma)
	$(call Device/EmmcImage)
	# installed from the sysupgrade archive and the RAM boot (see the commit
	# adding the device); the raw factory.bin of EmmcImage has no use here
	IMAGES := sysupgrade.bin
	KERNEL = kernel-bin | ucgfiber-check-kernel | lzma | \
		fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
	# RAM-only test image: the kernel embeds the whole rootfs, nothing is
	# read from or written to the eMMC (load with tftpboot, start with bootm)
	KERNEL_INITRAMFS = kernel-bin | ucgfiber-check-kernel | lzma | \
		fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
	DEVICE_VENDOR := Ubiquiti
	DEVICE_MODEL := UniFi Cloud Gateway Fiber
	SOC := ipq9574
	# the first U-Boot env for the installation, written once to mtd9 (from
	# the stock firmware or the RAM boot): the image's reference env with
	# ucg_boot=stock, so the unit keeps booting the stock firmware; or with
	# ucg_boot=install, so U-Boot RAM-boots /ucg/initramfs.itb from stock's
	# p6 (no UART needed; without the file, or with the reset button held,
	# stock boots)
	ARTIFACTS := u-boot-env.bin u-boot-env-install.bin
	ARTIFACT/u-boot-env.bin := ucgfiber-uboot-env stock
	ARTIFACT/u-boot-env-install.bin := ucgfiber-uboot-env install
	DEVICE_PACKAGES := kmod-dsa-rtl8372n kmod-gpio-pca953x kmod-iio-st_accel-i2c \
			   kmod-hwmon-ctf2302 kmod-phy-realtek kmod-rtc-s35390a kmod-sfp \
			   rtl826x-firmware kmod-backlight-pwm ucgfiber-lcd \
			   mipi-dbi-ubnt-ucgfiber video-support ethtool-full uboot-envtools mkf2fs f2fsck \
			   resize2fs dumpe2fs sfdisk partx-utils \
			   -wpad-basic-mbedtls -kmod-usb3 -kmod-usb-dwc3 -kmod-usb-dwc3-qcom
endef
TARGET_DEVICES += ubnt_ucgfiber
