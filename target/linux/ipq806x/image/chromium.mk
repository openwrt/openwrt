define Device/dsa-migration
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Config cannot be migrated from swconfig to DSA
endef

define Build/cros-gpt
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -o $@.tmp -g \
		-T cros_kernel	-N kernel -p $(CONFIG_TARGET_KERNEL_PARTSIZE)m \
				-N rootfs -p $(CONFIG_TARGET_ROOTFS_PARTSIZE)m \
				-N rootfs_data -p \
				$$((3687-$(CONFIG_TARGET_ROOTFS_PARTSIZE)-\
				         $(CONFIG_TARGET_KERNEL_PARTSIZE)))m
	cat $@.tmp >> $@
	rm $@.tmp
endef

define Build/append-kernel-part
	dd if=$(IMAGE_KERNEL) bs=$(CONFIG_TARGET_KERNEL_PARTSIZE)M conv=sync >> $@
endef

# NB: Chrome OS bootloaders replace the '%U' in command lines with the UUID of
# the kernel partition it chooses to boot from. This gives a flexible way to
# consistently build and sign kernels that always use the subsequent
# (PARTNROFF=1) partition as their rootfs.
define Build/cros-vboot
	$(STAGING_DIR_HOST)/bin/cros-vbutil \
		-k $@ \
		-c "root=PARTUUID=%U/PARTNROFF=1 rootwait fstools_partname_fallback_scan=1" \
		-o $@.new
	@mv $@.new $@
endef

define Device/OnhubImage
	KERNEL_LOADADDR = 0x44208000
	SOC := qcom-ipq8064
	KERNEL_SUFFIX := -fit-zImage.itb.vboot
	KERNEL_NAME := zImage
	KERNEL = kernel-bin | fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb | cros-vboot
	IMAGES := factory.bin sysupgrade.bin
	IMAGE/factory.bin := cros-gpt | append-kernel-part | append-rootfs
	IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
	# Note: Chromium/Depthcharge-based bootloaders insert a reserved-memory
	# ramoops node into the Device Tree automatically, so we can use
	# kmod-ramoops.
	DEVICE_PACKAGES := ath10k-firmware-qca988x-ct e2fsprogs kmod-fs-ext4 losetup \
			   partx-utils mkf2fs kmod-fs-f2fs \
			   ucode kmod-google-firmware kmod-tpm-i2c-infineon \
			   kmod-sound-soc-ipq8064-storm kmod-usb-storage \
			   kmod-ramoops
endef

define Device/asus_onhub
	$(call Device/OnhubImage)
	$(Device/dsa-migration)
	DEVICE_VENDOR := ASUS
	DEVICE_MODEL := OnHub SRT-AC1900
	DEVICE_DTS := $$(SOC)-asus-onhub
	BOARD_NAME := asus-onhub
endef
TARGET_DEVICES += asus_onhub

define Device/tplink_onhub
	$(call Device/OnhubImage)
	$(Device/dsa-migration)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := OnHub AC1900 Cloud Router
	DEVICE_DTS := $$(SOC)-tplink-onhub
	BOARD_NAME := tplink-onhub
endef
TARGET_DEVICES += tplink_onhub
