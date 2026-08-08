DTS_DIR := $(DTS_DIR)/qcom

define Build/be7000-append-ubi
	rm -f $@.tmp $@.ubinize.cfg
	{ \
		rootsize="$$(stat -c%s "$(IMAGE_ROOTFS)")"; \
		rootsize="$$(( (($$rootsize + 1023) / 1024) * 1024 ))"; \
		echo "[kernel]"; \
		echo "mode=ubi"; \
		echo "vol_id=0"; \
		echo "vol_type=dynamic"; \
		echo "vol_name=kernel"; \
		echo "image=$(IMAGE_KERNEL)"; \
		echo "[ubi_rootfs]"; \
		echo "mode=ubi"; \
		echo "vol_id=1"; \
		echo "vol_type=dynamic"; \
		echo "vol_name=ubi_rootfs"; \
		echo "image=$(IMAGE_ROOTFS)"; \
		echo "vol_size=$$rootsize"; \
		echo "[rootfs_data]"; \
		echo "mode=ubi"; \
		echo "vol_id=2"; \
		echo "vol_type=dynamic"; \
		echo "vol_name=rootfs_data"; \
		echo "vol_size=1MiB"; \
		echo "vol_flags=autoresize"; \
	} > $@.ubinize.cfg
	$(STAGING_DIR_HOST)/bin/ubinize \
		$(if $(SOURCE_DATE_EPOCH),-Q $(SOURCE_DATE_EPOCH)) \
		-o $@.tmp \
		-p $(BLOCKSIZE:%k=%KiB) -m $(PAGESIZE) \
		$(if $(SUBPAGESIZE),-s $(SUBPAGESIZE)) \
		$(if $(VID_HDR_OFFSET),-O $(VID_HDR_OFFSET)) \
		$(UBINIZE_OPTS) \
		$@.ubinize.cfg
	cat $@.tmp >> $@
	rm -f $@.tmp $@.ubinize.cfg
	$(if $(and $(IMAGE_SIZE),$(NAND_SIZE)),\
		$(call Build/check-size,$(UBI_NAND_SIZE_LIMIT)))
endef

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

define Device/xiaomi_be7000
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Xiaomi
	DEVICE_MODEL := BE7000
	DEVICE_DTS_CONFIG := config@al02-c6
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq9574
	KERNEL_SIZE := 6096k
	IMAGE_SIZE := 32116k
	DEVICE_PACKAGES := ath11k-firmware-ipq9574 \
					kmod-ath12k ath12k-firmware-qcn9274 \
					kmod-qrtr-smd \
					kmod-usb-storage be7000-nfc-i2ctransfer \
					luci luci-app-be7000-nfc luci-proto-wireguard \
					iperf3 ethtool-full mdio-tools qrencode uboot-envtools \
					kmod-wireguard wireguard-tools
	IMAGE/factory.ubi := be7000-append-ubi
endef
TARGET_DEVICES += xiaomi_be7000

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
