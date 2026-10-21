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

# The stock U-Boot loads uImage from an ext4 partition and the rootfs is a
# squashfs file on another one. The kernel therefore boots a small initramfs
# that loop mounts that file; see ucgfiber-initrd/.
define Build/ucgfiber-initrd
	READELF=$(TARGET_CROSS)readelf CPIO=$(STAGING_DIR_HOST)/bin/cpio \
		$(SHELL) $(PLATFORM_DIR)/image/ucgfiber-initrd/mkinitrd.sh \
		$(TARGET_DIR) $(PLATFORM_DIR)/image/ucgfiber-initrd/init \
		$(KDIR)/ucgfiber-initrd.cpio
endef

# Fail the build on a truncated or sparse kernel image instead of shipping it.
define Build/ucgfiber-check-kernel
	grep -qa "Linux version" $@ || { \
		echo "ucgfiber: kernel image has no version banner" >&2; exit 1; }
endef

# Same as "fit lzma <dtb> with-initrd", but with our own initrd instead of
# $(KDIR)/initrd.cpio, which holds the whole rootfs.
define Build/ucgfiber-fit
	$(call Build/ucgfiber-initrd)
	$(TOPDIR)/scripts/mkits.sh \
		-D $(DEVICE_NAME) -o $@.its -k $@ \
		-C lzma \
		-d $(KDIR)/image-$(DEVICE_DTS).dtb \
		-i $(KDIR)/ucgfiber-initrd.cpio \
		-a $(KERNEL_LOADADDR) -e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		$(if $(DEVICE_FDT_NUM),-n $(DEVICE_FDT_NUM)) \
		$(if $(DEVICE_DTS_DELIMITER),-l $(DEVICE_DTS_DELIMITER)) \
		$(if $(DEVICE_DTS_LOADADDR),-s $(DEVICE_DTS_LOADADDR)) \
		-c $(if $(DEVICE_DTS_CONFIG),$(DEVICE_DTS_CONFIG),"config-1") \
		-A $(LINUX_KARCH) -v $(LINUX_VERSION)
	$(call Build/fit-image,lzma)
endef

define Device/ubnt_ucgfiber
	$(call Device/FitImageLzma)
	$(call Device/EmmcImage)
	KERNEL = kernel-bin | ucgfiber-check-kernel | lzma | ucgfiber-fit
	# RAM-only test image: the kernel embeds the whole rootfs, nothing is
	# read from or written to the eMMC (load with tftpboot, start with bootm)
	KERNEL_INITRAMFS = kernel-bin | ucgfiber-check-kernel | lzma | \
		fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
	DEVICE_VENDOR := Ubiquiti
	DEVICE_MODEL := UniFi Cloud Gateway Fiber
	SOC := ipq9574
	DEVICE_PACKAGES := kmod-dsa-rtl8372n kmod-gpio-pca953x kmod-iio-st_accel-i2c \
			   kmod-hwmon-ctf2302 kmod-phy-realtek kmod-rtc-s35390a kmod-sfp \
			   rtl826x-firmware kmod-backlight-pwm ucgfiber-lcd \
			   mipi-dbi-ubnt-ucgfiber video-support ethtool-full mkf2fs f2fsck \
			   resize2fs dumpe2fs sfdisk partx-utils \
			   -wpad-basic-mbedtls -kmod-usb3 -kmod-usb-dwc3 -kmod-usb-dwc3-qcom
endef
TARGET_DEVICES += ubnt_ucgfiber
