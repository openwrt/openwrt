define Build/imx-combined-image-prepare
	rm -rf $@.boot
	mkdir -p $@.boot
endef

define Build/imx-combined-image-clean
	rm -rf $@.boot $@.fs
endef

define Build/imx-combined-image
	$(CP) $(IMAGE_KERNEL) $@.boot/uImage

	$(foreach dts,$(DEVICE_DTS), \
		$(CP) \
			$(DTS_DIR)/$(dts).dtb \
			$@.boot/;
	)

	mkimage -A arm -O linux -T script -C none -a 0 -e 0 \
		-n '$(DEVICE_ID) OpenWrt bootscript' \
		-d bootscript-$(DEVICE_NAME) \
		$@.boot/boot.scr

	cp $@ $@.fs

	$(SCRIPT_DIR)/gen_image_generic.sh $@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) \
		$@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$@.fs \
		1024
endef

define Build/imx-sdcard
	$(Build/imx-combined-image-prepare)

	if [ -f $(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot.img ]; then \
		$(CP) $(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot.img \
		$@.boot/u-boot.img; \
	fi

	if [ -f $(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot-dtb.img ]; then \
		$(CP) $(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot-dtb.img \
		$@.boot/u-boot-dtb.img; \
	fi

	$(Build/imx-combined-image)
	dd if=$(STAGING_DIR_IMAGE)/$(UBOOT)-SPL of=$@ bs=1024 seek=1 conv=notrunc

	$(Build/imx-combined-image-clean)
endef

define Build/imx-sdcard-raw-uboot
	$(Build/imx-combined-image-prepare)

	$(Build/imx-combined-image)
	dd if=$(STAGING_DIR_IMAGE)/$(UBOOT)-SPL of=$@ bs=1024 seek=1 conv=notrunc
	dd if=$(STAGING_DIR_IMAGE)/$(UBOOT)-u-boot-dtb.img of=$@ bs=1024 seek=69 conv=notrunc

	$(Build/imx-combined-image-clean)
endef
