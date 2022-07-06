DEVICE_VARS += SERCOMM_KERNEL_OFFSET SERCOMM_ROOTFS_OFFSET
DEVICE_VARS += SERCOMM_0x10str

define Build/sercomm-crypto
	$(TOPDIR)/scripts/sercomm-crypto.py \
		--input-file $@ \
		--key-file $@.key \
		--output-file $@.ser \
		--version $(SERCOMM_SWVER)
	$(STAGING_DIR_HOST)/bin/openssl enc -md md5 -aes-256-cbc \
		-in $@ \
		-out $@.enc \
		-K `cat $@.key` \
		-iv 00000000000000000000000000000000
	dd if=$@.enc >> $@.ser 2>/dev/null
	mv $@.ser $@
	rm -f $@.enc $@.key
endef

define Build/sercomm-kernel
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@ \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.hdr
	dd if=$@ >> $@.hdr 2>/dev/null
	mv $@.hdr $@
endef

define Build/sercomm-part-tag
	$(call Build/sercomm-part-tag-common,$(word 1,$(1)) $@)
endef

define Build/sercomm-part-tag-common
	$(eval file=$(word 2,$(1)))
	$(TOPDIR)/scripts/sercomm-partition-tag.py \
		--input-file $(file) \
		--output-file $(file).tmp \
		--part-name $(word 1,$(1)) \
		--part-version $(SERCOMM_SWVER)
	mv $(file).tmp $(file)
endef

define Build/sercomm-payload
	$(TOPDIR)/scripts/sercomm-pid.py \
		--hw-version $(SERCOMM_HWVER) \
		--hw-id $(SERCOMM_HWID) \
		--sw-version $(SERCOMM_SWVER) \
		--pid-file $@.pid \
		--extra-padding-size 0x10 \
		--extra-padding-first-byte 0x0a
	$(TOPDIR)/scripts/sercomm-payload.py \
		--input-file $@ \
		--output-file $@.tmp \
		--pid "$$(cat $@.pid | od -t x1 -An -v | tr -d '\n')"
	mv $@.tmp $@
	rm $@.pid
endef

define Build/sercomm-prepend-tagged-kernel
	$(CP) $(IMAGE_KERNEL) $(IMAGE_KERNEL).tagged
	$(call Build/sercomm-part-tag-common,$(word 1,$(1)) \
		$(IMAGE_KERNEL).tagged)
	dd if=$@ >> $(IMAGE_KERNEL).tagged 2>/dev/null
	mv $(IMAGE_KERNEL).tagged $@
endef

define Build/sercomm-tag-factory-type-AB-nbn
	$(TOPDIR)/scripts/sercomm-pid.py \
		--hw-version $(SERCOMM_HWVER) \
		--hw-id $(SERCOMM_HWID) \
		--sw-version $(SERCOMM_SWVER) \
		--pid-file $@.pid
	# Sercomm PID 0x10
	printf $(SERCOMM_0x10str) | dd seek=$$((0x10)) of=$@.pid bs=1 \
		conv=notrunc 2>/dev/null
	dd if=$@.pid of=$@.hdrfactory conv=notrunc 2>/dev/null
	printf $$(stat -c%s $(IMAGE_KERNEL)) | dd seek=$$((0x70)) of=$@.hdrfactory \
		bs=1 conv=notrunc 2>/dev/null
	printf $$(stat -c%s $@) | dd seek=$$((0x80)) of=$@.hdrfactory bs=1 \
		conv=notrunc 2>/dev/null
	cat $(IMAGE_KERNEL) $@ | $(MKHASH) md5 | awk '{print $$1}' | \
		tr -d '\n' | dd seek=$$((0x1e0)) of=$@.hdrfactory bs=1 \
		conv=notrunc 2>/dev/null
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $(IMAGE_KERNEL) \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.hdrkrn1
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $(IMAGE_KERNEL) \
		--kernel-offset 0x1B00100 \
		--rootfs-offset 0x4D00000 \
		--output-header $@.hdrkrn2
	# Hack CRC for Kernel2
	dd if=/dev/zero of=$@.hdrkrn2 bs=1 seek=$$((0x18)) count=4 conv=notrunc \
		2>/dev/null
	cat $@.hdrfactory $@.hdrkrn1 $@.hdrkrn2 $(IMAGE_KERNEL) $@ > $@.new
	mv $@.new $@ ; rm -f $@.hdrfactory $@.hdrkrn1 $@.hdrkrn2 $@.pid
endef

define Build/sercomm-tag-factory-type-B-pro
	$(TOPDIR)/scripts/sercomm-pid.py \
		--hw-version $(SERCOMM_HWVER) \
		--hw-id $(SERCOMM_HWID) \
		--sw-version $(SERCOMM_SWVER) \
		--pid-file $@.pid
	dd if=$@.pid of=$@.hdrfactory conv=notrunc 2>/dev/null
	printf 11223344556677889900112233445566 | sed 's/../\\x&/g' | \
		xargs -d . printf | dd of=$@.footer conv=notrunc 2>/dev/null
	printf $$(stat -c%s $(IMAGE_KERNEL)) | dd seek=$$((0x70)) of=$@.hdrfactory \
		bs=1 conv=notrunc 2>/dev/null
	printf $$(stat -c%s $@) | dd seek=$$((0x80)) of=$@.hdrfactory bs=1 \
		conv=notrunc 2>/dev/null
	printf $$(stat -c%s $@.footer) | dd seek=$$((0x90)) of=$@.hdrfactory bs=1 \
		conv=notrunc 2>/dev/null
	cat $(IMAGE_KERNEL) $@ $@.footer | $(MKHASH) md5 | awk '{print $$1}' | \
		tr -d '\n' | dd seek=$$((0x1e0)) of=$@.hdrfactory bs=1 \
		conv=notrunc 2>/dev/null
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $(IMAGE_KERNEL) \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.hdrkrn1
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $(IMAGE_KERNEL) \
		--kernel-offset 0x1B00100 \
		--rootfs-offset 0x3D00000 \
		--output-header $@.hdrkrn2
	# Hack CRC for Kernel2
	dd if=/dev/zero of=$@.hdrkrn2 bs=1 seek=$$((0x18)) count=4 conv=notrunc \
		2>/dev/null
	cat $@.hdrfactory $@.hdrkrn1 $@.hdrkrn2 $(IMAGE_KERNEL) $@ > $@.new
	mv $@.new $@ ; rm -f $@.hdrfactory $@.hdrkrn1 $@.hdrkrn2 $@.pid
endef

define Device/sercomm_dxx
  $(Device/dsa-migration)
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  UBINIZE_OPTS := -E 5
  LOADER_TYPE := bin
  KERNEL_LOADADDR := 0x81001000
  LZMA_TEXT_START := 0x82800000
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma -a0 | \
	uImage lzma | sercomm-kernel
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	lzma -a0 | uImage lzma
  IMAGES += factory.img
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory.img := append-ubi | sercomm-part-tag rootfs | \
	sercomm-prepend-tagged-kernel kernel | gzip | sercomm-payload | \
	sercomm-crypto
  SERCOMM_KERNEL_OFFSET := 0x400100
  SERCOMM_ROOTFS_OFFSET := 0x1000000
endef

define Device/sercomm-s1500-common
  $(Device/sercomm_dxx)
  DEVICE_MODEL := S1500
  KERNEL_SIZE := 4m
  IMAGE_SIZE := 30m
  KERNEL_LOADADDR := 0x80001000
  IMAGES += factory.img
  IMAGE/factory.img := append-ubi | sercomm-tag-factory-type-B-pro
  SERCOMM_KERNEL_OFFSET := 0x1700100
  SERCOMM_ROOTFS_OFFSET := 0x1f00000
  SERCOMM_KERNEL1_OFFSET := 0x1b00100
  SERCOMM_ROOTFS1_OFFSET := 0x3d00000
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb3 uboot-envtools
endef
