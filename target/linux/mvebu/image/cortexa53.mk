define Build/gpt-image
	rm -fR $@.boot
	mkdir -p $@.boot
	$(foreach dts,$(DEVICE_DTS), $(CP) $(KDIR)/image-$(dts).dtb $@.boot/$(dts).dtb;)
	$(CP) $(IMAGE_KERNEL) $@.boot/$(KERNEL_NAME)
	-$(CP) $@-boot.scr $@.boot/boot.scr

	PADDING=1 GUID="$(IMG_PART_DISKGUID)" \
		$(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS) \
		1024
endef

define Device/glinet_gl-mv1000
  $(call Device/Default-arm64)
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MV1000
  DEVICE_PACKAGES += kmod-dsa-mv88e6xxx
  SOC := armada-3720
  BOOT_SCRIPT := gl-mv1000
endef
TARGET_DEVICES += glinet_gl-mv1000

define Device/globalscale_espressobin
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := ESPRESSObin
  DEVICE_VARIANT := Non-eMMC
  DEVICE_PACKAGES += kmod-dsa-mv88e6xxx
  DEVICE_ALT0_VENDOR := Marvell
  DEVICE_ALT0_MODEL := Armada 3700 Community Board
  DEVICE_ALT0_VARIANT := Non-eMMC
  SOC := armada-3720
  BOOT_SCRIPT := espressobin
endef
TARGET_DEVICES += globalscale_espressobin

define Device/globalscale_espressobin-emmc
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := ESPRESSObin
  DEVICE_VARIANT := eMMC
  DEVICE_PACKAGES += kmod-dsa-mv88e6xxx
  DEVICE_ALT0_VENDOR := Marvell
  DEVICE_ALT0_MODEL := Armada 3700 Community Board
  DEVICE_ALT0_VARIANT := eMMC
  SOC := armada-3720
  BOOT_SCRIPT := espressobin
endef
TARGET_DEVICES += globalscale_espressobin-emmc

define Device/globalscale_espressobin-ultra
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := ESPRESSObin
  DEVICE_VARIANT := Ultra
  DEVICE_PACKAGES += kmod-i2c-pxa kmod-rtc-pcf8563 kmod-dsa-mv88e6xxx
  SOC := armada-3720
  BOOT_SCRIPT := espressobin
endef
TARGET_DEVICES += globalscale_espressobin-ultra

define Device/globalscale_espressobin-v7
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := ESPRESSObin
  DEVICE_VARIANT := V7 Non-eMMC
  DEVICE_PACKAGES += kmod-dsa-mv88e6xxx
  DEVICE_ALT0_VENDOR := Marvell
  DEVICE_ALT0_MODEL := Armada 3700 Community Board
  DEVICE_ALT0_VARIANT := V7 Non-eMMC
  SOC := armada-3720
  BOOT_SCRIPT := espressobin
endef
TARGET_DEVICES += globalscale_espressobin-v7

define Device/globalscale_espressobin-v7-emmc
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := ESPRESSObin
  DEVICE_VARIANT := V7 eMMC
  DEVICE_PACKAGES += kmod-dsa-mv88e6xxx
  DEVICE_ALT0_VENDOR := Marvell
  DEVICE_ALT0_MODEL := Armada 3700 Community Board
  DEVICE_ALT0_VARIANT := V7 eMMC
  SOC := armada-3720
  BOOT_SCRIPT := espressobin
endef
TARGET_DEVICES += globalscale_espressobin-v7-emmc

define Device/iodata_hdl-ta
  $(call Device/Default-arm64)
  DEVICE_VENDOR := I-O DATA
  DEVICE_MODEL := HDL-TA Series NAS
  SOC := armada-3720
  IMAGES := hdd.img.gz usb.img.gz
  IMAGE/hdd.img.gz := boot-scr $(subst 00#,02,$(IMG_PART_DISKGUID)#) | \
	gpt-image | gzip | append-metadata
  IMAGE/usb.img.gz := boot-scr | \
	boot-img | sdcard-img | gzip | append-metadata
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs.dtb
  ARTIFACT/initramfs.dtb := append-dtb
endif
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-fs-vfat kmod-hwmon-drivetemp \
	kmod-i2c-pxa kmod-rtc-ds1307
  BOOT_SCRIPT := hdl-ta
endef
TARGET_DEVICES += iodata_hdl-ta

define Device/marvell_armada-3720-db
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Marvell
  DEVICE_MODEL := Armada 3720 Development Board (DB-88F3720-DDR3)
  DEVICE_DTS := armada-3720-db
endef
TARGET_DEVICES += marvell_armada-3720-db

define Device/methode_udpu
  $(call Device/Default-arm64)
  DEVICE_VENDOR := Methode
  DEVICE_MODEL := micro-DPU (uDPU)
  DEVICE_DTS := armada-3720-uDPU
  KERNEL_LOADADDR := 0x00800000
  KERNEL_INITRAMFS := kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS_SUFFIX := .itb
  DEVICE_PACKAGES += f2fs-tools fdisk kmod-i2c-pxa kmod-hwmon-lm75 kmod-dsa-mv88e6xxx
  DEVICE_IMG_NAME = $$(DEVICE_IMG_PREFIX)-$$(2)
  IMAGES := firmware.tgz
  IMAGE/firmware.tgz := boot-scr | boot-img-ext4 | uDPU-firmware | append-metadata
  BOOT_SCRIPT := udpu
endef
TARGET_DEVICES += methode_udpu

define Device/methode_edpu
  $(call Device/methode_udpu)
  DEVICE_MODEL := eDPU
  DEVICE_DTS := armada-3720-eDPU
  KERNEL_INITRAMFS := kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
endef
TARGET_DEVICES += methode_edpu
