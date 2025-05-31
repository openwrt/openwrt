DEVICE_VARS += BOOT_SCRIPT

define Build/mstc-header
	$(eval version=$(1))
	gzip -c $@ | tail -c8 > $@.crclen
	( \
		printf "CMOC"; \
		tail -c+5 $@.crclen; head -c4 $@.crclen; \
		printf '$(call toupper,$(LINUX_KARCH)) $(VERSION_DIST) Linux-$(LINUX_VERSION)' | \
			dd bs=64 count=1 conv=sync 2>/dev/null; \
		printf "$(version)" | \
			dd bs=64 count=1 conv=sync 2>/dev/null; \
		dd if=/dev/zero bs=884 count=1 2>/dev/null; \
		cat $@; \
	) > $@.new
	mv $@.new $@
	rm -f $@.crclen
endef

define Device/elecom_wrc-x3000gs2
	$(call Device/FitImageLzma)
	DEVICE_VENDOR := ELECOM
	DEVICE_MODEL := WRC-X3000GS2
	DEVICE_DTS_CONFIG := config@mp03.3
	SOC := ipq5018
	KERNEL_IN_UBI := 1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	IMAGE_SIZE := 52480k
	NAND_SIZE := 128m
	IMAGES += factory.bin
	IMAGE/factory.bin := append-ubi | qsdk-ipq-factory-nand | \
		mstc-header 4.04(XZF.0)b90 | elecom-product-header WRC-X3000GS2
	DEVICE_PACKAGES := ath11k-firmware-qcn6122 ipq-wifi-elecom_wrc-x3000gs2
endef
TARGET_DEVICES += elecom_wrc-x3000gs2

define Device/glinet_gl-b3000
	$(call Device/FitImage)
	DEVICE_VENDOR := GL.iNet
	DEVICE_MODEL := GL-B3000
	SOC := ipq5018
	KERNEL_IN_UBI := 1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	NAND_SIZE := 128m
	DEVICE_DTS_CONFIG := config@mp03.5-c1
	SUPPORTED_DEVICES += b3000
	BOOT_SCRIPT:= glinet_gl-b3000.bootscript
	IMAGES := factory.img sysupgrade.bin
	IMAGE/factory.img := append-ubi | gl-qsdk-factory | append-metadata
	DEVICE_PACKAGES := \
		ath11k-firmware-qcn6122 \
		ipq-wifi-glinet_gl-b3000 \
		dumpimage
endef
TARGET_DEVICES += glinet_gl-b3000

define Device/linksys_ipq50xx_mx_base
	$(call Device/FitImageLzma)
	DEVICE_VENDOR := Linksys
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	KERNEL_SIZE := 8192k
	IMAGE_SIZE := 83968k
	NAND_SIZE := 256m
	SOC := ipq5018
	IMAGES += factory.bin
	IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | linksys-image type=$$$$(DEVICE_MODEL)
endef

define Device/linksys_mr5500
	$(call Device/linksys_ipq50xx_mx_base)
	DEVICE_MODEL := MR5500
	DEVICE_DTS_CONFIG := config@mp03.1
	DEVICE_PACKAGES := kmod-ath11k-pci \
		ath11k-firmware-qcn9074 \
		ipq-wifi-linksys_mr5500 \
		kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += linksys_mr5500

define Device/linksys_mx2000
	$(call Device/linksys_ipq50xx_mx_base)
	DEVICE_MODEL := MX2000
	DEVICE_DTS_CONFIG := config@mp03.5-c1
	DEVICE_PACKAGES := ath11k-firmware-qcn6122 \
		ipq-wifi-linksys_mx2000
endef
TARGET_DEVICES += linksys_mx2000

define Device/linksys_mx5500
	$(call Device/linksys_ipq50xx_mx_base)
	DEVICE_MODEL := MX5500
	DEVICE_DTS_CONFIG := config@mp03.1
	DEVICE_PACKAGES := kmod-ath11k-pci \
		ath11k-firmware-qcn9074 \
		ipq-wifi-linksys_mx5500
endef
TARGET_DEVICES += linksys_mx5500

define Device/linksys_spnmx56
	$(call Device/linksys_ipq50xx_mx_base)
	DEVICE_MODEL := SPNMX56
	DEVICE_DTS_CONFIG := config@mp03.1
	DEVICE_PACKAGES := kmod-ath11k-pci \
		ath11k-firmware-qcn9074 \
		ipq-wifi-linksys_spnmx56
endef
TARGET_DEVICES += linksys_spnmx56

define Device/yuncore_ax830
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Yuncore
	DEVICE_MODEL := AX830
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq5018
	DEVICE_DTS_CONFIG := config@mp03.5-c1
	DEVICE_PACKAGES := kmod-ath11k-pci \
		ath11k-firmware-ipq5018 \
		ath11k-firmware-qcn6122
endef
TARGET_DEVICES += yuncore_ax830
