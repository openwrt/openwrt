define Device/FitImage
	KERNEL_SUFFIX := -uImage.itb
	KERNEL = kernel-bin | libdeflate-gzip | fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/FitImageLzma
	KERNEL_SUFFIX := -uImage.itb
	KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/EmmcImage
	IMAGES += factory.bin sysupgrade.bin
	IMAGE/factory.bin := append-rootfs | pad-rootfs | pad-to 64k
	IMAGE/sysupgrade.bin/squashfs := append-rootfs | pad-to 64k | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef

define Device/UbiFit
	KERNEL_IN_UBI := 1
	IMAGES := factory.ubi sysupgrade.bin
	IMAGE/factory.ubi := append-ubi
	IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/buffalo_wxr-5950ax12
	$(call Device/FitImage)
	DEVICE_VENDOR := Buffalo
	DEVICE_MODEL := WXR-5950AX12
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01
	SOC := ipq8074
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
	DEVICE_PACKAGES := ipq-wifi-buffalo_wxr-5950ax12
endef
TARGET_DEVICES += buffalo_wxr-5950ax12

define Device/compex_wpq873
       $(call Device/FitImage)
       $(call Device/UbiFit)
       DEVICE_VENDOR := Compex
       DEVICE_MODEL := WPQ873
       BLOCKSIZE := 128k
       PAGESIZE := 2048
       DEVICE_DTS_CONFIG := config@hk09.wpq873
       SOC := ipq8072
       DEVICE_PACKAGES := ipq-wifi-compex_wpq873
       IMAGE/factory.ubi := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += compex_wpq873

define Device/dynalink_dl-wrx36
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Dynalink
	DEVICE_MODEL := DL-WRX36
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@rt5010w-d350-rev0
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-dynalink_dl-wrx36
endef
TARGET_DEVICES += dynalink_dl-wrx36

define Device/edgecore_eap102
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Edgecore
	DEVICE_MODEL := EAP102
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac02
	SOC := ipq8071
	DEVICE_PACKAGES := ipq-wifi-edgecore_eap102
	IMAGE/factory.ubi := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += edgecore_eap102

define Device/edimax_cax1800
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Edimax
	DEVICE_MODEL := CAX1800
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac03
	SOC := ipq8070
	DEVICE_PACKAGES := ipq-wifi-edimax_cax1800
endef
TARGET_DEVICES += edimax_cax1800

define Device/netgear_rax120v2
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := RAX120v2
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01
	SOC := ipq8074
	KERNEL_SIZE := 29696k
	NETGEAR_BOARD_ID := RAX120
	NETGEAR_HW_ID := 29765589+0+512+1024+4x4+8x8
	DEVICE_PACKAGES := ipq-wifi-netgear_rax120v2 kmod-spi-gpio \
		kmod-spi-bitbang kmod-gpio-nxp-74hc164 kmod-hwmon-g761
	IMAGES = web-ui-factory.img sysupgrade.bin
	IMAGE/web-ui-factory.img := append-image initramfs-uImage.itb | \
		pad-offset $$$$(BLOCKSIZE) 64 | append-uImage-fakehdr filesystem | \
		netgear-dni
	IMAGE/sysupgrade.bin := append-kernel | pad-offset $$$$(BLOCKSIZE) 64 | \
		append-uImage-fakehdr filesystem | sysupgrade-tar kernel=$$$$@ | \
		append-metadata
endef
TARGET_DEVICES += netgear_rax120v2

define Device/netgear_wax620
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := WAX620
	DEVICE_DTS_CONFIG := config@hk07
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8072
	DEVICE_PACKAGES += kmod-spi-gpio kmod-gpio-nxp-74hc164 \
		ipq-wifi-netgear_wax620
endef
TARGET_DEVICES += netgear_wax620

define Device/netgear_wax218
	$(call Device/FitImage)
	$(call Device/UbiFit)
	ARTIFACTS := web-ui-factory.fit
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := WAX218
	DEVICE_DTS_CONFIG := config@hk07
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8072
	ARTIFACT/web-ui-factory.fit := append-image initramfs-uImage.itb | \
		ubinize-kernel | qsdk-ipq-factory-nand
	DEVICE_PACKAGES := kmod-spi-gpio kmod-spi-bitbang kmod-gpio-nxp-74hc164 \
		ipq-wifi-netgear_wax218
endef
TARGET_DEVICES += netgear_wax218

define Device/prpl_haze
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := prpl Foundation
	DEVICE_MODEL := Haze
	DEVICE_DTS_CONFIG := config@hk09
	SOC := ipq8072
	DEVICE_PACKAGES += ath11k-firmware-qcn9074 ipq-wifi-prpl_haze kmod-ath11k-pci \
		mkf2fs f2fsck kmod-fs-f2fs
endef
TARGET_DEVICES += prpl_haze

define Device/qnap_301w
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := QNAP
	DEVICE_MODEL := 301w
	DEVICE_DTS_CONFIG := config@hk01
	KERNEL_SIZE := 16384k
	SOC := ipq8072
	DEVICE_PACKAGES += ipq-wifi-qnap_301w
endef
TARGET_DEVICES += qnap_301w

define Device/redmi_ax6
	$(call Device/xiaomi_ax3600)
	DEVICE_VENDOR := Redmi
	DEVICE_MODEL := AX6
	DEVICE_PACKAGES := ipq-wifi-redmi_ax6
endef
TARGET_DEVICES += redmi_ax6

define Device/xiaomi_ax3600
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Xiaomi
	DEVICE_MODEL := AX3600
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac04
	SOC := ipq8071
	KERNEL_SIZE := 36608k
	DEVICE_PACKAGES := ipq-wifi-xiaomi_ax3600 kmod-ath10k-ct-smallbuffers ath10k-firmware-qca9887-ct
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	ARTIFACTS := initramfs-factory.ubi
	ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-uImage.itb | ubinize-kernel
endif
endef
TARGET_DEVICES += xiaomi_ax3600

define Device/xiaomi_ax9000
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Xiaomi
	DEVICE_MODEL := AX9000
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk14
	SOC := ipq8072
	KERNEL_SIZE := 57344k
	DEVICE_PACKAGES := ipq-wifi-xiaomi_ax9000 kmod-ath11k-pci ath11k-firmware-qcn9074 \
	kmod-ath10k-ct ath10k-firmware-qca9887-ct
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	ARTIFACTS := initramfs-factory.ubi
	ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-uImage.itb | ubinize-kernel
endif
endef
TARGET_DEVICES += xiaomi_ax9000

define Device/zyxel_nbg7815
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := ZYXEL
	DEVICE_MODEL := NBG7815
	DEVICE_DTS_CONFIG := config@nbg7815
	SOC := ipq8074
	DEVICE_PACKAGES += ipq-wifi-zyxel_nbg7815 kmod-ath11k-pci kmod-hwmon-tmp103 \
		kmod-bluetooth
endef
TARGET_DEVICES += zyxel_nbg7815
