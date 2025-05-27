define Build/boot-scr
	rm -f $@-boot.scr
	mkimage -A arm64 -O linux -T script -C none -a 0 -e 0 \
		-d bootscript-$(BOOT_SCRIPT) $@-boot.scr
endef

define Build/boot-img-ext4
	rm -fR $@.boot
	mkdir -p $@.boot
	$(foreach dts,$(DEVICE_DTS), $(CP) $(KDIR)/image-$(dts).dtb $@.boot/$(dts).dtb;)
	$(CP) $(IMAGE_KERNEL) $@.boot/$(KERNEL_NAME)
	-$(CP) $@-boot.scr $@.boot/boot.scr
	make_ext4fs -J -L kernel -l $(CONFIG_TARGET_KERNEL_PARTSIZE)M \
		$(if $(SOURCE_DATE_EPOCH),-T $(SOURCE_DATE_EPOCH)) \
		$@.bootimg $@.boot
endef

define Build/sdcard-img-ext4
	SIGNATURE="$(IMG_PART_SIGNATURE)" \
	PARTOFFSET="$(PARTITION_OFFSET)" PADDING=1 \
		$(if $(filter $(1),efi),GUID="$(IMG_PART_DISKGUID)") $(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS) \
		256
endef

define Device/Default
  PROFILES := Default
  FILESYSTEMS := squashfs ubifs ext4
  DEVICE_DTS_DIR := $(DTS_DIR)/freescale
  KERNEL_INSTALL := 1
  KERNEL_NAME := Image
  KERNEL := kernel-bin
endef

define Device/imx8m
  DEVICE_VENDOR := NXP
  DEVICE_MODEL := i.MX8M
  DEVICE_DTS := $(basename $(notdir $(wildcard $(DTS_DIR)/freescale/imx8m*.dts)))
endef
TARGET_DEVICES += imx8m

define Device/gateworks_venice
  $(call Device/Default)
  FILESYSTEMS := squashfs ext4
  DEVICE_VENDOR := Gateworks
  DEVICE_MODEL := i.MX8M Venice
  SUPPORTED_DEVICES := \
	gw,imx8mm-gw71xx-0x \
	gw,imx8mm-gw72xx-0x \
	gw,imx8mp-gw72xx-2x \
	gw,imx8mm-gw73xx-0x \
	gw,imx8mp-gw73xx-2x \
	gw,imx8mm-gw7901 \
	gw,imx8mm-gw7902 \
	gw,imx8mn-gw7902 \
	gw,imx8mm-gw7903 \
	gateworks,imx8mp-gw71xx-2x \
	gateworks,imx8mp-gw74xx \
	gateworks,imx8mm-gw7904 \
	gateworks,imx8mm-gw7905-0x \
	gateworks,imx8mp-gw7905-2x
  BOOT_SCRIPT := gateworks_venice
  PARTITION_OFFSET := 16M
  DEVICE_DTS := $(basename $(notdir $(wildcard $(DTS_DIR)/freescale/imx8m*-venice*.dts)))
  DEVICE_PACKAGES := \
	kmod-hwmon-gsc kmod-rtc-ds1672 kmod-eeprom-at24 \
	kmod-gpio-button-hotplug kmod-leds-gpio kmod-pps-gpio \
	kmod-lan743x kmod-sky2 kmod-iio-st_accel-i2c \
	kmod-can kmod-can-flexcan kmod-can-mcp251x
  IMAGES := img.gz
  IMAGE/img.gz := boot-scr | boot-img-ext4 | sdcard-img-ext4 | gzip | append-metadata
endef
TARGET_DEVICES += gateworks_venice
